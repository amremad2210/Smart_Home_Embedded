#include "hal_lcd_i2c.h"

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"

/*******************************************************************************
 *                         I2C LCD Configuration                               *
 ******************************************************************************/

/* PCF8574T address:
 * With A0=A1=A2 open (factory default) this is typically 0x27.
 * If your TA or datasheet says 0x3F instead, change it here.
 */
#define LCD_I2C_ADDR              0x27    /* TODO: change if lab says another */

/* Use I2C0 on PB2 (SCL) and PB3 (SDA) */
#define LCD_I2C_BASE              I2C0_BASE
#define LCD_I2C_PERIPH            SYSCTL_PERIPH_I2C0
#define LCD_I2C_GPIO_PERIPH       SYSCTL_PERIPH_GPIOB
#define LCD_I2C_GPIO_PORT         GPIO_PORTB_BASE
#define LCD_I2C_SCL_PIN           GPIO_PIN_2
#define LCD_I2C_SDA_PIN           GPIO_PIN_3
#define LCD_I2C_PCTL_SCL          GPIO_PB2_I2C0SCL
#define LCD_I2C_PCTL_SDA          GPIO_PB3_I2C0SDA

/*******************************************************************************
 *                     PCF8574T Backpack Bit Mapping                           *
 ******************************************************************************/
/*
 * Very common PCF8574T LCD backpack mapping (matches your module):
 *
 *  P0 -> D4
 *  P1 -> D5
 *  P2 -> D6
 *  P3 -> D7
 *  P4 -> EN
 *  P5 -> RW
 *  P6 -> RS
 *  P7 -> Backlight
 *
 * If TA says it is different, adjust the masks and LCD_Write4Bits().
 */

#define LCD_I2C_RS                0x40   /* P6 */
#define LCD_I2C_RW                0x20   /* P5 (kept 0 for write) */
#define LCD_I2C_EN                0x10   /* P4 */
#define LCD_I2C_BACKLIGHT         0x80   /* P7 */

/* Local backlight state (on by default) */
static uint8_t lcd_backlight = LCD_I2C_BACKLIGHT;

/*******************************************************************************
 *                               Local Helpers                                 *
 ******************************************************************************/

/* Simple blocking delay in ms.
 * This uses SysCtlDelay; make sure SysCtlClockSet() was called in main().
 * If you already have MCALSysTickDelayMs(), you can replace body with that.
 */
static void LCD_DelayMs(uint32_t ms)
{
    uint32_t clk = SysCtlClockGet();
    SysCtlDelay((clk / 3 / 1000) * ms);
}

/* Initialize I2C0 bus on PB2/PB3 */
static void LCD_I2C_InitBus(void)
{
    SysCtlPeripheralEnable(LCD_I2C_PERIPH);
    SysCtlPeripheralEnable(LCD_I2C_GPIO_PERIPH);
    while (!SysCtlPeripheralReady(LCD_I2C_PERIPH)) {}
    while (!SysCtlPeripheralReady(LCD_I2C_GPIO_PERIPH)) {}

    GPIOPinConfigure(LCD_I2C_PCTL_SCL);
    GPIOPinConfigure(LCD_I2C_PCTL_SDA);
    GPIOPinTypeI2CSCL(LCD_I2C_GPIO_PORT, LCD_I2C_SCL_PIN);
    GPIOPinTypeI2C(LCD_I2C_GPIO_PORT, LCD_I2C_SDA_PIN);

    /* I2C master at 100 kHz (false = standard mode) */
    I2CMasterInitExpClk(LCD_I2C_BASE, SysCtlClockGet(), false);
}

/* Send raw byte to PCF8574T (no backlight ORing here) */
static void LCD_I2C_WriteRaw(uint8_t data)
{
    I2CMasterSlaveAddrSet(LCD_I2C_BASE, LCD_I2C_ADDR, false);  /* write */
    I2CMasterDataPut(LCD_I2C_BASE, data);
    I2CMasterControl(LCD_I2C_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    while (I2CMasterBusy(LCD_I2C_BASE)) {}
}

/* Add backlight bit and send */
static void LCD_I2C_Write(uint8_t data)
{
    LCD_I2C_WriteRaw(data | lcd_backlight);
}

/* Pulse EN high->low to latch data into LCD */
static void LCD_PulseEnable(uint8_t data)
{
    LCD_I2C_Write(data | LCD_I2C_EN);
    LCD_DelayMs(1);
    LCD_I2C_Write(data & ~LCD_I2C_EN);
    LCD_DelayMs(1);
}

/* Send high nibble (bits 7..4) to D7..D4 via P3..P0, with RS=rs */
static void LCD_Write4Bits(uint8_t nibbleHigh, uint8_t rs)
{
    uint8_t out = 0;

    /* Map D4..D7 (bits 4..7 of nibbleHigh) to P0..P3 of PCF8574T */
    out |= (nibbleHigh >> 4) & 0x0F;   /* D4->P0, D5->P1, D6->P2, D7->P3 */

    if (rs)
        out |= LCD_I2C_RS;

    /* RW is kept 0 for write only */

    LCD_PulseEnable(out);
}

/*******************************************************************************
 *                         Public API Implementations                          *
 ******************************************************************************/

void LCD_SendCommand(uint8_t command)
{
    /* High nibble (RS=0) then low nibble (RS=0) */
    LCD_Write4Bits(command & 0xF0, 0);
    LCD_Write4Bits((command << 4) & 0xF0, 0);

    if (command == LCD_CLEAR || command == LCD_HOME)
        LCD_DelayMs(2);
    else
        LCD_DelayMs(1);
}

void LCD_SendData(uint8_t data)
{
    /* High nibble (RS=1) then low nibble (RS=1) */
    LCD_Write4Bits(data & 0xF0, 1);
    LCD_Write4Bits((data << 4) & 0xF0, 1);
    LCD_DelayMs(1);
}

void LCD_Clear(void)
{
    LCD_SendCommand(LCD_CLEAR);
}

void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t addr = (row == 0)
                   ? (LCD_LINE1_ADDR + col)
                   : (LCD_LINE2_ADDR + col);
    LCD_SendCommand(addr);
}

void LCD_WriteString(const char *str)
{
    while (*str != '\0')
    {
        LCD_SendData((uint8_t)*str);
        str++;
    }
}

void LCD_WriteChar(char c)
{
    LCD_SendData((uint8_t)c);
}

/* Initialize LCD over I2C in 4-bit mode */
void LCD_Init(void)
{
    LCD_I2C_InitBus();

    /* Wait for LCD power-up */
    LCD_DelayMs(50);

    /* Per HD44780 4-bit init: send 0x30 three times in 8-bit mode */
    LCD_Write4Bits(0x30, 0);
    LCD_DelayMs(5);
    LCD_Write4Bits(0x30, 0);
    LCD_DelayMs(5);
    LCD_Write4Bits(0x30, 0);
    LCD_DelayMs(5);

    /* Switch to 4-bit mode (0x20) */
    LCD_Write4Bits(0x20, 0);
    LCD_DelayMs(5);

    /* Function set: 4-bit, 2 lines, 5x8 font */
    LCD_SendCommand(LCD_FUNCTION_4BIT_2LINE);

    /* Display ON, cursor OFF */
    LCD_SendCommand(LCD_DISPLAY_ON);

    /* Clear display */
    LCD_Clear();

    /* Entry mode: increment, no shift */
    LCD_SendCommand(LCD_ENTRY_MODE);
}
