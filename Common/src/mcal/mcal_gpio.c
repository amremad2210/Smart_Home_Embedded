/*============================================================================
 *  Module      : MCAL GPIO
 *  File Name   : mcal_gpio.c
 *  Description : GPIO abstraction layer built on top of TivaWare driverlib
 *===========================================================================*/

#include "mcal/mcal_gpio.h"

/*======================================================================
 *  Local helpers
 *====================================================================*/

/**
 * @brief Map our internal attach type to TivaWare pad configuration.
 */
static uint32_t prv_getPadType(GPIO_InternalAttachType attach)
{
    switch (attach)
    {
        case GPIO_ATTACH_PULLUP:
            return GPIO_PIN_TYPE_STD_WPU;   /* Standard, weak pull-up */

        case GPIO_ATTACH_PULLDOWN:
            return GPIO_PIN_TYPE_STD_WPD;   /* Standard, weak pull-down */

        case GPIO_ATTACH_DEFAULT:
        default:
            return GPIO_PIN_TYPE_STD;       /* Standard push-pull, no pull */
    }
}

/**
 * @brief Map our drive enum to TivaWare strength macros.
 */
static uint32_t prv_getDriveStrength(GPIO_DriveType drive)
{
    switch (drive)
    {
        case GPIO_DRIVE_4MA:
            return GPIO_STRENGTH_4MA;

        case GPIO_DRIVE_8MA:
            return GPIO_STRENGTH_8MA;

        case GPIO_DRIVE_2MA:
        default:
            return GPIO_STRENGTH_2MA;
    }
}

/*======================================================================
 *  API implementations
 *====================================================================*/

void MCAL_GPIO_EnablePort(uint32_t periph)
{
    /* Enable the peripheral clock for the given port */
    SysCtlPeripheralEnable(periph);

    /* Wait until the peripheral is ready */
    while (!SysCtlPeripheralReady(periph))
    {
        /* spin */
    }
}

void MCAL_GPIO_InitPin(uint32_t portBase,
                       uint8_t  pins,
                       GPIO_DirectionType       dir,
                       GPIO_InternalAttachType  attach,
                       GPIO_DriveType           drive)
{
    uint32_t dirMode;
    uint32_t padType;
    uint32_t strength;

    /* Select direction mode for driverlib */
    if (dir == GPIO_DIR_OUTPUT)
    {
        dirMode = GPIO_DIR_MODE_OUT;
    }
    else
    {
        dirMode = GPIO_DIR_MODE_IN;
    }

    /* Configure direction */
    GPIODirModeSet(portBase, pins, dirMode);

    /* Configure pad: drive strength + attach */
    padType  = prv_getPadType(attach);
    strength = prv_getDriveStrength(drive);

    GPIOPadConfigSet(portBase, pins, strength, padType);
}

void MCAL_GPIO_WritePin(uint32_t portBase,
                        uint8_t  pins,
                        uint8_t  value)
{
    /* If value is non-zero, set pins high; otherwise set them low */
    if (value)
    {
        GPIOPinWrite(portBase, pins, pins);
    }
    else
    {
        GPIOPinWrite(portBase, pins, 0x00);
    }
}

uint8_t MCAL_GPIO_ReadPin(uint32_t portBase,
                          uint8_t  pins)
{
    /* Read the pins; return 1 if any of them is high, 0 otherwise */
    uint32_t raw = GPIOPinRead(portBase, pins);
    return (raw != 0U) ? 1U : 0U;
}

void MCAL_GPIO_TogglePin(uint32_t portBase,
                         uint8_t  pins)
{
    uint32_t current = GPIOPinRead(portBase, pins);

    /* Toggle only the selected pins */
    uint32_t toggled = (~current) & pins;
    GPIOPinWrite(portBase, pins, toggled);
}



