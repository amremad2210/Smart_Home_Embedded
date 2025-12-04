/*============================================================================
 *  Module      : MCAL GPIO
 *  File Name   : mcal_gpio.h
 *  Description : GPIO abstraction layer built on top of TivaWare driverlib
 *===========================================================================*/

#ifndef MCAL_GPIO_H_
#define MCAL_GPIO_H_

#include <stdbool.h>
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

/*======================================================================
 *  Types
 *====================================================================*/

/* Direction of a GPIO pin */
typedef enum
{
    GPIO_DIR_INPUT = 0,
    GPIO_DIR_OUTPUT
} GPIO_DirectionType;

/* Internal resistor configuration */
typedef enum
{
    GPIO_ATTACH_DEFAULT = 0,   /* No pull, or reset state */
    GPIO_ATTACH_PULLUP,
    GPIO_ATTACH_PULLDOWN
} GPIO_InternalAttachType;





/*======================================================================
 *  API
 *====================================================================*/

/**
 * @brief Enable clock for a GPIO port.
 *
 * @param periph  One of SYSCTL_PERIPH_GPIOA, SYSCTL_PERIPH_GPIOB, ...
 */
void MCAL_GPIO_EnablePort(uint32_t periph);


/**
 * @brief Configure one or more pins on a port.
 *
 * Uses TivaWare driverlib to set direction and internal attachment
 *
 * @param portBase  GPIO_PORTx_BASE (e.g. GPIO_PORTF_BASE)
 * @param pins      Bit mask of pins (e.g. GPIO_PIN_1 | GPIO_PIN_2)
 * @param dir       GPIO_DIR_INPUT or GPIO_DIR_OUTPUT
 * @param attach    GPIO_ATTACH_xxx
 */
void MCAL_GPIO_InitPin(uint32_t portBase,
                       uint8_t  pins,
                       GPIO_DirectionType       dir,
                       GPIO_InternalAttachType  attach);


/**
 * @brief Write value to one or more output pins.
 *
 * @param portBase  GPIO_PORTx_BASE
 * @param pins      Mask of pins to modify
 * @param value     If non-zero ? set pins high, 0 ? set pins low
 */
void MCAL_GPIO_WritePin(uint32_t portBase,
                        uint8_t  pins,
                        uint8_t  value);

/**
 * @brief Read the current value of one or more pins.
 *
 * @param portBase  GPIO_PORTx_BASE
 * @param pins      Mask of pins to read
 * @return          Non-zero if any selected pin is high, 0 otherwise
 */
uint8_t MCAL_GPIO_ReadPin(uint32_t portBase,
                          uint8_t  pins);

/**
 * @brief Toggle the state of one or more output pins.
 *
 * @param portBase  GPIO_PORTx_BASE
 * @param pins      Mask of pins to toggle
 */
void MCAL_GPIO_TogglePin(uint32_t portBase,
                         uint8_t  pins);





#endif /* MCAL_GPIO_H_ */
