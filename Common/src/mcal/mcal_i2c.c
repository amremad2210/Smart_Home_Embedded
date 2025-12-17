#include "mcal/mcal_i2c.h"
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h" // Critical for I2C
#include "driverlib/i2c.h"

void I2C0_Init(void)
{
    // 1. Enable Clocks for I2C0 and GPIOB
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    
    // Wait for the peripheral to be ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0));

    // 2. Configure Pins for I2C (Alternative Function)
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    // 3. Configure Pin Types (SCL and SDA)
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    // 4. Initialize I2C Master
    // Use System Clock, Fast Mode (400kbps) = true, Normal (100kbps) = false
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false); 
}

void I2C0_WriteByte(uint8_t SlaveAddress, uint8_t Data)
{
    // 1. Set Slave Address (False = Write Mode)
    I2CMasterSlaveAddrSet(I2C0_BASE, SlaveAddress, false);

    // 2. Put Data into the Register
    I2CMasterDataPut(I2C0_BASE, Data);

    // 3. Send Start + Run + Stop (Single Byte Transfer)
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

    // 4. Wait until the I2C Master is not busy
    while(I2CMasterBusy(I2C0_BASE));
}