/*============================================================================
 *  Module      : Control ECU On-Target UART Loopback Self-Test
 *  File Name   : test_control_on_target.c
 *  Description : Flashable self-test that runs on ONE Control Tiva and uses
 *                the EXISTING drivers (HAL_COMM + MCAL UART + EEPROM HAL)
 *                to simulate HMI frames via UART loopback.
 *
 *  Key idea:
 *    - Enable UART1 loopback (internal LBE bit) OR wire PB1<->PB0 externally.
 *    - Send hardcoded HMI frames using HAL_COMM_SendByte().
 *    - Call Control ECU handlers from CONTROL_WS/main.c to consume RX bytes.
 *    - Read the response byte back from UART (looped) and compare expected.
 *
 *  IMPORTANT BUILD NOTES (IAR):
 *    - This file includes CONTROL_WS/main.c (renaming its main()) so we can
 *      call its internal handlers without changing production code.
 *    - For this test build, EXCLUDE CONTROL_WS/main.c from the project build
 *      (otherwise you'll compile it twice).
 *    - DO NOT exclude your existing drivers; we rely on them.
 *
 *  PASS/FAIL:
 *    - PASS: Green LED ON forever
 *    - FAIL: Red LED ON forever
 *===========================================================================*/

#include <stdint.h>
#include <stdbool.h>

#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "inc/hw_memmap.h"

/* For UART_CTL_LBE (internal loopback). Provided by TivaWare. */
#include "inc/hw_types.h"
#include "inc/hw_uart.h"

#include "Types.h"
#include "mcal/mcal_gpio.h"
#include "mcal/mcal_systick.h"
#include "hal/hal_comm.h"
#include "hal/hal_eeprom.h"
/* Motor + buzzer drivers are exercised indirectly by calling handlers in main.c */

/* Use the existing MCAL EEPROM API for timeout check */
#include "mcal/mcal_eeprom.h"

/* Bring in the hardcoded HMI frames table (data only) */
#include "test_main_protocol.c"

/*===========================================================================
 * Include production Control logic (main.c), but:
 *  - rename its main() so we can define our own test main()
 *  - remove 'static' so we can call internal handlers directly
 *===========================================================================*/

#define main   control_app_main
#define static /* expose file-static symbols for test */
#include "main.c"
#undef static
#undef main

/*===========================================================================
 * Test configuration
 *===========================================================================*/

/* Use short 4-digit passwords to keep each frame <= 16 bytes (UART FIFO safe). */
static const uint8_t FRAME_SETUP_QUERY[] = { 'S', 0u };
static const uint8_t FRAME_SETUP_PWD_1234[] = { 'S', 4u,'1','2','3','4', 4u,'1','2','3','4' };
static const uint8_t FRAME_SET_TIMEOUT_5[] = { 'T', 5u, 4u,'1','2','3','4' };
static const uint8_t FRAME_CHANGE_PWD_1234_TO_4321[] =
{
    'C',
    4u,'1','2','3','4',   /* old */
    4u,'4','3','2','1',   /* new #1 */
    4u,'4','3','2','1'    /* new #2 */
};

/* Exercise motor driver (OpenDoorSequence) */
static const uint8_t FRAME_OPEN_DOOR_4321[] = { 'O', 4u, '4','3','2','1' };

/* Exercise buzzer/lockout path after 3 wrong attempts */
static const uint8_t FRAME_OPEN_DOOR_WRONG_0000[] = { 'O', 4u, '0','0','0','0' };

/*===========================================================================
 * Helpers
 *===========================================================================*/

/* --- Debug console over UART0 (LaunchPad ICDI virtual COM) --- */
static void dbg_uart0_init(void)
{
    /* UART0: PA0=U0RX, PA1=U0TX */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)) { }
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) { }

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200U,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTEnable(UART0_BASE);
}

static void dbg_uart0_putc(char c)
{
    UARTCharPut(UART0_BASE, c);
}

static void printf(const char *s)
{
    if (s == NULL) return;
    while (*s != '\0')
    {
        if (*s == '\n') { dbg_uart0_putc('\r'); }
        dbg_uart0_putc(*s++);
    }
}

static void dbg_uart0_put_u32(uint32_t v)
{
    /* minimal decimal print */
    char buf[11];
    uint32_t i = 0u;
    if (v == 0u)
    {
        dbg_uart0_putc('0');
        return;
    }
    while ((v > 0u) && (i < (sizeof(buf) - 1u)))
    {
        buf[i++] = (char)('0' + (v % 10u));
        v /= 10u;
    }
    while (i-- > 0u)
    {
        dbg_uart0_putc(buf[i]);
    }
}

/* --- Pre-test indicator: white blink 3x, then 3s pause --- */
static void led_init_rgb(void)
{
    /* Control main.c defines PF1 red, PF3 green. PF2 is blue on LaunchPad. */
#ifndef BLUE_LED_PIN
#define BLUE_LED_PIN (1U << 2)  /* PF2 */
#endif

    MCAL_GPIO_EnablePort(LED_GPIO_PERIPH);
    MCAL_GPIO_InitPin(LED_PORT_BASE,
                      GREEN_LED_PIN | RED_LED_PIN | BLUE_LED_PIN,
                      GPIO_DIR_OUTPUT,
                      GPIO_ATTACH_DEFAULT);

    MCAL_GPIO_WritePin(LED_PORT_BASE, GREEN_LED_PIN, LOGIC_LOW);
    MCAL_GPIO_WritePin(LED_PORT_BASE, RED_LED_PIN, LOGIC_LOW);
    MCAL_GPIO_WritePin(LED_PORT_BASE, BLUE_LED_PIN, LOGIC_LOW);
}

static void led_white(uint8_t on)
{
    const uint32_t v = on ? LOGIC_HIGH : LOGIC_LOW;
    MCAL_GPIO_WritePin(LED_PORT_BASE, GREEN_LED_PIN, v);
    MCAL_GPIO_WritePin(LED_PORT_BASE, RED_LED_PIN, v);
    MCAL_GPIO_WritePin(LED_PORT_BASE, BLUE_LED_PIN, v);
}

static void fail_hard(void)
{
    printf("[FAIL] Halting.\n");
    LED_SetRed();
    while (1) { }
}

static void pass_hard(void)
{
    printf("[PASS] All tests passed. Halting.\n");
    LED_SetGreen();
    while (1) { }
}

static void pre_test_signal(uint32_t testNum)
{
    printf("\n--- TEST ");
    dbg_uart0_put_u32(testNum);
    printf(" START ---\n");

    for (uint8_t i = 0u; i < 3u; i++)
    {
        led_white(1u);
        MCAL_SysTick_DelayMs(200u);
        led_white(0u);
        MCAL_SysTick_DelayMs(200u);
    }
    MCAL_SysTick_DelayMs(3000u);
}

static void motor_smoke_test(void)
{
    printf("[INFO] Motor smoke test: FORWARD 1s, STOP 0.5s, BACKWARD 1s, STOP\n");
    HAL_Motor_Move(MOTOR_FORWARD);
    MCAL_SysTick_DelayMs(1000u);
    HAL_Motor_Move(MOTOR_STOP);
    MCAL_SysTick_DelayMs(500u);
    HAL_Motor_Move(MOTOR_BACKWARD);
    MCAL_SysTick_DelayMs(1000u);
    HAL_Motor_Move(MOTOR_STOP);
    printf("[INFO] Motor smoke test done.\n");
}

static void drain_rx(void)
{
    while (HAL_COMM_IsDataAvailable())
    {
        (void)HAL_COMM_ReceiveByte();
    }
}

static void enable_uart1_internal_loopback(void)
{
    /* If you prefer external loopback wiring, comment this out and connect:
     *   PB1 (U1TX) <-> PB0 (U1RX)
     */
    HWREG(HAL_COMM_UART_MODULE + UART_O_CTL) |= UART_CTL_LBE;
}

static void send_frame(const uint8_t *frame, uint32_t len)
{
    for (uint32_t i = 0u; i < len; i++)
    {
        HAL_COMM_SendByte(frame[i]);
    }
}

static uint8_t wait_and_read_one_byte(uint32_t timeoutMs)
{
    while (timeoutMs--)
    {
        if (HAL_COMM_IsDataAvailable())
        {
            return HAL_COMM_ReceiveByte();
        }
        MCAL_SysTick_DelayMs(1u);
    }
    return 0u;
}

static void process_one_command_from_uart(void)
{
    /* Mimic the relevant part of CONTROL_WS/main.c main loop:
     * read command byte, call the right handler. */
    uint8_t cmd = wait_and_read_one_byte(200u);
    if (cmd == 0u) { fail_hard(); }

    switch (cmd)
    {
        case CMD_SETUP_PASSWORD:  HandlePasswordSetup(); break;
        case CMD_OPEN_DOOR:       HandleOpenDoor();      break;
        case CMD_CHANGE_PASSWORD: HandleChangePassword();break;
        case CMD_SET_TIMEOUT:     HandleSetTimeout();    break;
        default: fail_hard(); break;
    }
}

static void assert_eq_u8(uint8_t a, uint8_t b)
{
    if (a != b) { fail_hard(); }
}

/*===========================================================================
 * Test runner
 *===========================================================================*/

int main(void)
{
    /* Use the same clock setup style as your apps */
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    MCAL_SysTick_Init();
    dbg_uart0_init();
    printf("Control ECU on-target self-test (UART1 loopback)\n");

    /* Init modules using existing drivers (this also initializes motor + buzzer + LED) */
    System_Init();
    (void)HAL_EEPROM_Init();
    led_init_rgb();

    /* Quick standalone motor test (uses existing HAL_Motor functions) */
    pre_test_signal(0u);
    motor_smoke_test();

    /* Ensure loopback */
    enable_uart1_internal_loopback();
    drain_rx();

    /* Start clean (real EEPROM) */
    (void)HAL_EEPROM_ClearPassword();

    /* ---- Test 1: SETUP_QUERY -> expect 'Y' (no password set) */
    pre_test_signal(1u);
    send_frame(FRAME_SETUP_QUERY, sizeof(FRAME_SETUP_QUERY));
    process_one_command_from_uart();
    assert_eq_u8(RESP_SUCCESS, wait_and_read_one_byte(200u));
    drain_rx();
    printf("[OK] Test 1 done (SETUP_QUERY)\n");

    /* ---- Test 2: SETUP_PWD_OK -> expect 'Y' */
    pre_test_signal(2u);
    send_frame(FRAME_SETUP_PWD_1234, sizeof(FRAME_SETUP_PWD_1234));
    process_one_command_from_uart();
    assert_eq_u8(RESP_SUCCESS, wait_and_read_one_byte(200u));
    drain_rx();
    printf("[OK] Test 2 done (SETUP_PWD_OK)\n");

    /* ---- Test 3: SET_TIMEOUT=5 with correct password -> expect 'Y' and EEPROM updated */
    pre_test_signal(3u);
    send_frame(FRAME_SET_TIMEOUT_5, sizeof(FRAME_SET_TIMEOUT_5));
    process_one_command_from_uart();
    assert_eq_u8(RESP_SUCCESS, wait_and_read_one_byte(200u));
    drain_rx();

    uint32_t timeoutWord = 0u;
    (void)MCAL_EEPROM_ReadWord(EEPROM_TIMEOUT_ADDR, &timeoutWord);
    if (timeoutWord != 5u) { fail_hard(); }
    printf("[OK] Test 3 done (SET_TIMEOUT=5)\n");

    /* ---- Test 4: CHANGE_PWD 1234 -> 4321 -> expect 'Y' */
    pre_test_signal(4u);
    send_frame(FRAME_CHANGE_PWD_1234_TO_4321, sizeof(FRAME_CHANGE_PWD_1234_TO_4321));
    process_one_command_from_uart();
    assert_eq_u8(RESP_SUCCESS, wait_and_read_one_byte(200u));
    drain_rx();

    /* Optional verification using real EEPROM read/verify */
    if (!HAL_EEPROM_VerifyPassword("4321", 4u)) { fail_hard(); }
    printf("[OK] Test 4 done (CHANGE_PWD)\n");

    /* ---- Test 5: OPEN_DOOR with correct password (motor should run forward/stop/backward) */
    /* Expected:
     *  - Response 'Y'
     *  - Green LED
     *  - Motor moves forward ~2s, stops, waits timeout (5s), moves backward ~2s, stops
     *
     * NOTE: HandleOpenDoor blocks during the full sequence (~9 seconds total). */
    pre_test_signal(5u);
    printf("[INFO] Test 5 will run motor sequence (~9s)\n");
    send_frame(FRAME_OPEN_DOOR_4321, sizeof(FRAME_OPEN_DOOR_4321));
    process_one_command_from_uart();
    assert_eq_u8(RESP_SUCCESS, wait_and_read_one_byte(200u));
    drain_rx();
    printf("[OK] Test 5 done (OPEN_DOOR motor sequence)\n");

    /* ---- Test 6: 3x wrong OPEN_DOOR attempts (buzzer lockout beep) */
    /* Expected each attempt:
     *  - Response 'N'
     *  - Red LED
     * After the 3rd attempt:
     *  - BUZZER_beep(LOCKOUT_BUZZER_DURATION) is called (blocking ~10 seconds)
     */
    pre_test_signal(6u);
    printf("[INFO] Test 6 triggers lockout buzzer (~10s) after 3rd wrong attempt\n");
    for (uint8_t i = 0u; i < 3u; i++)
    {
        send_frame(FRAME_OPEN_DOOR_WRONG_0000, sizeof(FRAME_OPEN_DOOR_WRONG_0000));
        process_one_command_from_uart();
        assert_eq_u8(RESP_FAILURE, wait_and_read_one_byte(200u));
        drain_rx();
        printf("[OK] Wrong attempt ");
        dbg_uart0_put_u32((uint32_t)i + 1u);
        printf(" done\n");
        MCAL_SysTick_DelayMs(1000u);
    }
    printf("[OK] Test 6 done (LOCKOUT path)\n");

    pass_hard();
    return 0;
}



