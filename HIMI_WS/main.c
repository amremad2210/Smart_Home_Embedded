/*============================================================================
 *  HMI ECU Main Application
 *  - Displays menu on LCD
 *  - Reads keypad and potentiometer
 *  - Sends commands to Control ECU over UART using hal_comm
 *===========================================================================*/

#include <stdint.h>
#include <stdbool.h>

#include "driverlib/sysctl.h"

#include "Types.h"
#include "mcal/mcal_systick.h"

#include "hal/hal_lcd.h"
#include "hal/hal_keypad.h"
#include "hal/hal_rgb_led.h"
#include "hal/hal_potentiometer.h"
#include "hal/hal_comm.h"

#define PASSWORD_LENGTH         5U
#define TIMEOUT_MIN_SECONDS     5U
#define TIMEOUT_MAX_SECONDS     30U

/* Command bytes (must match Control ECU) */
#define CMD_SETUP_PASSWORD      'S'
#define CMD_OPEN_DOOR           'O'
#define CMD_CHANGE_PASSWORD     'C'
#define CMD_SET_TIMEOUT         'T'

/* Responses from Control ECU */
#define RESP_SUCCESS            'Y'
#define RESP_FAILURE            'N'
#define RESP_LOCKOUT            'L'
#define RESP_READY              'R'

/* Helper prototypes */
static void HMI_Init(void);
static void HMI_WaitForReady(void);
static char HMI_WaitKey(void);
static void HMI_ReadPassword(char *buf);
static uint8_t HMI_ReadTimeoutFromPot(void);
static uint8_t HMI_WaitResponse(void);
static void HMI_ShowMessage(const char *line1, const char *line2, uint32_t delayMs);

static void Handle_OpenDoor(void);
static void Handle_ChangePassword(void);
static void Handle_SetTimeout(void);

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    HMI_Init();
    HMI_WaitForReady();

    for (;;)
    {
        /* Show main menu */
        LCD_Clear();
        LCD_SetCursor(0, 0);
        LCD_WriteString("+Open  -Change");
        LCD_SetCursor(1, 0);
        LCD_WriteString("*Timeout");

        char key = HMI_WaitKey();

        if (key == '+')
        {
            Handle_OpenDoor();
        }
        else if (key == '-')
        {
            Handle_ChangePassword();
        }
        else if (key == '*')
        {
            Handle_SetTimeout();
        }
    }

    return 0;
}

/*======================================================================
 *  Helpers
 *====================================================================*/
static void HMI_Init(void)
{
    MCAL_SysTick_Init();
    LCD_Init();
    HAL_Keypad_Init();
    POT_Init();
    RGB_LED_Init();
    HAL_COMM_Init();
    LCD_Clear();
}

static void HMI_WaitForReady(void)
{
    LCD_Clear();
    LCD_WriteString("Waiting Control");
    while (1)
    {
        if (HAL_COMM_IsDataAvailable())
        {
            uint8_t b = HAL_COMM_ReceiveByte();
            if (b == RESP_READY)
            {
                LCD_Clear();
                LCD_WriteString("Control Ready");
                MCAL_SysTick_DelayMs(800U);
                return;
            }
        }
    }
}

static char HMI_WaitKey(void)
{
    char k;
    do {
        k = HAL_Keypad_GetKey();
    } while (k == '\0');
    return k;
}

static void HMI_ReadPassword(char *buf)
{
    LCD_Clear();
    LCD_WriteString("Enter PWD:");
    LCD_SetCursor(1, 0);
    for (uint8_t i = 0; i < PASSWORD_LENGTH; i++)
    {
        char k = HMI_WaitKey();
        buf[i] = k;
        LCD_WriteChar('*');
    }
}

static uint8_t HMI_ReadTimeoutFromPot(void)
{
    /* Map 0-100% to 5..30 */
    uint8_t pct = POT_ReadPercentage();
    uint8_t range = TIMEOUT_MAX_SECONDS - TIMEOUT_MIN_SECONDS;
    uint8_t t = (uint8_t)(TIMEOUT_MIN_SECONDS + ((pct * range) / 100U));
    if (t < TIMEOUT_MIN_SECONDS) t = TIMEOUT_MIN_SECONDS;
    if (t > TIMEOUT_MAX_SECONDS) t = TIMEOUT_MAX_SECONDS;
    return t;
}

static uint8_t HMI_WaitResponse(void)
{
    /* Blocking wait for 1 byte */
    return HAL_COMM_ReceiveByte();
}

static void HMI_ShowMessage(const char *line1, const char *line2, uint32_t delayMs)
{
    LCD_Clear();
    if (line1) { LCD_SetCursor(0, 0); LCD_WriteString(line1); }
    if (line2) { LCD_SetCursor(1, 0); LCD_WriteString(line2); }
    if (delayMs > 0U) { MCAL_SysTick_DelayMs(delayMs); }
}

/*======================================================================
 *  Handlers
 *====================================================================*/
static void Handle_OpenDoor(void)
{
    char pwd[PASSWORD_LENGTH];
    HMI_ReadPassword(pwd);

    /* Send command */
    HAL_COMM_SendByte(CMD_OPEN_DOOR);
    for (uint8_t i = 0; i < PASSWORD_LENGTH; i++)
    {
        HAL_COMM_SendByte((uint8_t)pwd[i]);
    }

    uint8_t resp = HMI_WaitResponse();
    if (resp == RESP_SUCCESS)
    {
        HMI_ShowMessage("Door Opening", "Success", 1000U);
    }
    else if (resp == RESP_LOCKOUT)
    {
        HMI_ShowMessage("LOCKOUT", "", 1500U);
    }
    else
    {
        HMI_ShowMessage("Wrong PWD", "", 1000U);
    }
}

static void Handle_ChangePassword(void)
{
    char oldPwd[PASSWORD_LENGTH];
    char newPwd[PASSWORD_LENGTH];
    char confPwd[PASSWORD_LENGTH];

    HMI_ShowMessage("Old Password", "", 0U);
    HMI_ReadPassword(oldPwd);

    HMI_ShowMessage("New Password", "", 0U);
    HMI_ReadPassword(newPwd);

    HMI_ShowMessage("Confirm New", "", 0U);
    HMI_ReadPassword(confPwd);

    /* Send command */
    HAL_COMM_SendByte(CMD_CHANGE_PASSWORD);
    for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) HAL_COMM_SendByte((uint8_t)oldPwd[i]);
    for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) HAL_COMM_SendByte((uint8_t)newPwd[i]);
    for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) HAL_COMM_SendByte((uint8_t)confPwd[i]);

    uint8_t resp = HMI_WaitResponse();
    if (resp == RESP_SUCCESS)
    {
        HMI_ShowMessage("Password", "Changed", 1000U);
    }
    else if (resp == RESP_LOCKOUT)
    {
        HMI_ShowMessage("LOCKOUT", "", 1500U);
    }
    else
    {
        HMI_ShowMessage("Change Failed", "", 1000U);
    }
}

static void Handle_SetTimeout(void)
{
    uint8_t timeoutVal = TIMEOUT_MIN_SECONDS;
    LCD_Clear();
    LCD_WriteString("Adj Timeout");
    LCD_SetCursor(1, 0);
    LCD_WriteString("#=OK  *=Back");

    while (1)
    {
        timeoutVal = HMI_ReadTimeoutFromPot();
        LCD_SetCursor(0, 12);
        /* Overwrite with current value */
        char buf[5];
        buf[0] = (char)('0' + (timeoutVal / 10U));
        buf[1] = (char)('0' + (timeoutVal % 10U));
        buf[2] = 's';
        buf[3] = ' ';
        buf[4] = '\0';
        LCD_WriteString(buf);

        char k = HAL_Keypad_GetKey();
        if (k == '#')
        {
            break; /* confirm */
        }
        else if (k == '*')
        {
            return; /* cancel */
        }
    }

    /* Read password to authorize */
    char pwd[PASSWORD_LENGTH];
    HMI_ShowMessage("Enter PWD", "", 0U);
    HMI_ReadPassword(pwd);

    HAL_COMM_SendByte(CMD_SET_TIMEOUT);
    HAL_COMM_SendByte(timeoutVal); /* 1-byte integer */
    for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) HAL_COMM_SendByte((uint8_t)pwd[i]);

    uint8_t resp = HMI_WaitResponse();
    if (resp == RESP_SUCCESS)
    {
        HMI_ShowMessage("Timeout Saved", "", 1000U);
    }
    else if (resp == RESP_LOCKOUT)
    {
        HMI_ShowMessage("LOCKOUT", "", 1500U);
    }
    else
    {
        HMI_ShowMessage("Failed", "", 1000U);
    }
}
