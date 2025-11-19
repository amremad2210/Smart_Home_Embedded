/******************************************************************************
 *
 * Module: GPIO
 *
 * File Name: gpio.c
 *
 * Description: Source file for the TM4C123GH6PM (Tiva-C LaunchPad) GPIO driver
 *
 * Note:
 *  - writePin/writePort only drive output data.
 *  - Pull-ups and pull-downs are configured via dedicated APIs.
 *
 *******************************************************************************/

#include "gpio.h"
#include "common_macros.h"
#include "tm4c123gh6pm.h"

/*******************************************************************************
 *                           Functions Definitions                             *
 *******************************************************************************/

/*
 * Description :
 * Enable the clock for the required GPIO port and wait until it is ready.
 */
void GPIO_enableClock(uint8_t port_num)
{
    if (port_num >= NUM_OF_PORTS)
    {
        /* Invalid port, do nothing */
        return;
    }

    /* 1. Enable clock for this GPIO port */
    SYSCTL_RCGCGPIO_R |= (1 << port_num);

    /* 2. Wait until the peripheral is ready */
    while ((SYSCTL_PRGPIO_R & (1 << port_num)) == 0)
    {
        /* Wait */
    }
}

/*
 * Description :
 * Setup the direction of the required pin input/output.
 */
void GPIO_setupPinDirection(uint8_t port_num, uint8_t pin_num, GPIO_PinDirectionType direction)
{
    /* Check if the input port/pin numbers are within range. */
    if ((pin_num >= NUM_OF_PINS_PER_PORT) || (port_num >= NUM_OF_PORTS))
    {
        /* Do nothing */
    }
    else
    {
        switch (port_num)
        {
        case PORTA_ID:
            /* Disable analog and alternate functions, enable digital */
            CLEAR_BIT(GPIO_PORTA_AMSEL_R, pin_num);
            CLEAR_BIT(GPIO_PORTA_AFSEL_R, pin_num);
            SET_BIT(GPIO_PORTA_DEN_R, pin_num);

            if (direction == PIN_OUTPUT)
            {
                SET_BIT(GPIO_PORTA_DIR_R, pin_num);
            }
            else
            {
                CLEAR_BIT(GPIO_PORTA_DIR_R, pin_num);
            }
            break;

        case PORTB_ID:
            CLEAR_BIT(GPIO_PORTB_AMSEL_R, pin_num);
            CLEAR_BIT(GPIO_PORTB_AFSEL_R, pin_num);
            SET_BIT(GPIO_PORTB_DEN_R, pin_num);

            if (direction == PIN_OUTPUT)
            {
                SET_BIT(GPIO_PORTB_DIR_R, pin_num);
            }
            else
            {
                CLEAR_BIT(GPIO_PORTB_DIR_R, pin_num);
            }
            break;

        case PORTC_ID:
            CLEAR_BIT(GPIO_PORTC_AMSEL_R, pin_num);
            CLEAR_BIT(GPIO_PORTC_AFSEL_R, pin_num);
            SET_BIT(GPIO_PORTC_DEN_R, pin_num);

            if (direction == PIN_OUTPUT)
            {
                SET_BIT(GPIO_PORTC_DIR_R, pin_num);
            }
            else
            {
                CLEAR_BIT(GPIO_PORTC_DIR_R, pin_num);
            }
            break;

        case PORTD_ID:
            CLEAR_BIT(GPIO_PORTD_AMSEL_R, pin_num);
            CLEAR_BIT(GPIO_PORTD_AFSEL_R, pin_num);
            SET_BIT(GPIO_PORTD_DEN_R, pin_num);

            if (direction == PIN_OUTPUT)
            {
                SET_BIT(GPIO_PORTD_DIR_R, pin_num);
            }
            else
            {
                CLEAR_BIT(GPIO_PORTD_DIR_R, pin_num);
            }
            break;

        case PORTE_ID:
            CLEAR_BIT(GPIO_PORTE_AMSEL_R, pin_num);
            CLEAR_BIT(GPIO_PORTE_AFSEL_R, pin_num);
            SET_BIT(GPIO_PORTE_DEN_R, pin_num);

            if (direction == PIN_OUTPUT)
            {
                SET_BIT(GPIO_PORTE_DIR_R, pin_num);
            }
            else
            {
                CLEAR_BIT(GPIO_PORTE_DIR_R, pin_num);
            }
            break;

        case PORTF_ID:
            CLEAR_BIT(GPIO_PORTF_AMSEL_R, pin_num);
            CLEAR_BIT(GPIO_PORTF_AFSEL_R, pin_num);
            SET_BIT(GPIO_PORTF_DEN_R, pin_num);

            if (direction == PIN_OUTPUT)
            {
                SET_BIT(GPIO_PORTF_DIR_R, pin_num);
            }
            else
            {
                CLEAR_BIT(GPIO_PORTF_DIR_R, pin_num);
            }
            break;

        default:
            /* Should never reach here due to range check */
            break;
        }
    }
}

/*
 * Description :
 * Configure the direction of the required port all pins input/output.
 */
void GPIO_setupPortDirection(uint8_t port_num, GPIO_PortDirectionType direction)
{
    if (port_num >= NUM_OF_PORTS)
    {
        /* Do nothing */
    }
    else
    {
        switch (port_num)
        {
        case PORTA_ID:
            /* Configure pins 0..7 as GPIO, digital enabled */
            GPIO_PORTA_AMSEL_R &= ~(0xFF);
            GPIO_PORTA_AFSEL_R &= ~(0xFF);
            GPIO_PORTA_DEN_R   |=  (0xFF);

            if (direction == PORT_OUTPUT)
            {
                GPIO_PORTA_DIR_R |= 0xFF;
            }
            else
            {
                GPIO_PORTA_DIR_R &= ~(0xFF);
            }
            break;

        case PORTB_ID:
            GPIO_PORTB_AMSEL_R &= ~(0xFF);
            GPIO_PORTB_AFSEL_R &= ~(0xFF);
            GPIO_PORTB_DEN_R   |=  (0xFF);

            if (direction == PORT_OUTPUT)
            {
                GPIO_PORTB_DIR_R |= 0xFF;
            }
            else
            {
                GPIO_PORTB_DIR_R &= ~(0xFF);
            }
            break;

        case PORTC_ID:
            GPIO_PORTC_AMSEL_R &= ~(0xFF);
            GPIO_PORTC_AFSEL_R &= ~(0xFF);
            GPIO_PORTC_DEN_R   |=  (0xFF);

            if (direction == PORT_OUTPUT)
            {
                GPIO_PORTC_DIR_R |= 0xFF;
            }
            else
            {
                GPIO_PORTC_DIR_R &= ~(0xFF);
            }
            break;

        case PORTD_ID:
            GPIO_PORTD_AMSEL_R &= ~(0xFF);
            GPIO_PORTD_AFSEL_R &= ~(0xFF);
            GPIO_PORTD_DEN_R   |=  (0xFF);

            if (direction == PORT_OUTPUT)
            {
                GPIO_PORTD_DIR_R |= 0xFF;
            }
            else
            {
                GPIO_PORTD_DIR_R &= ~(0xFF);
            }
            break;

        case PORTE_ID:
            GPIO_PORTE_AMSEL_R &= ~(0xFF);
            GPIO_PORTE_AFSEL_R &= ~(0xFF);
            GPIO_PORTE_DEN_R   |=  (0xFF);

            if (direction == PORT_OUTPUT)
            {
                GPIO_PORTE_DIR_R |= 0xFF;
            }
            else
            {
                GPIO_PORTE_DIR_R &= ~(0xFF);
            }
            break;

        case PORTF_ID:
            GPIO_PORTF_AMSEL_R &= ~(0xFF);
            GPIO_PORTF_AFSEL_R &= ~(0xFF);
            GPIO_PORTF_DEN_R   |=  (0xFF);

            if (direction == PORT_OUTPUT)
            {
                GPIO_PORTF_DIR_R |= 0xFF;
            }
            else
            {
                GPIO_PORTF_DIR_R &= ~(0xFF);
            }
            break;

        default:
            /* Will never reach this part */
            break;
        }
    }
}

/*
 * Description :
 * Write the value Logic High or Logic Low on the required pin.
 * Only meaningful when the pin is configured as OUTPUT.
 */
void GPIO_writePin(uint8_t port_num, uint8_t pin_num, uint8_t value)
{
    if ((pin_num >= NUM_OF_PINS_PER_PORT) || (port_num >= NUM_OF_PORTS))
    {
        /* Do nothing */
    }
    else
    {
        switch (port_num)
        {
        case PORTA_ID:
            if (BIT_IS_SET(GPIO_PORTA_DIR_R, pin_num))
            {
                if (value == LOGIC_HIGH)
                {
                    SET_BIT(GPIO_PORTA_DATA_R, pin_num);
                }
                else
                {
                    CLEAR_BIT(GPIO_PORTA_DATA_R, pin_num);
                }
            }
            break;

        case PORTB_ID:
            if (BIT_IS_SET(GPIO_PORTB_DIR_R, pin_num))
            {
                if (value == LOGIC_HIGH)
                {
                    SET_BIT(GPIO_PORTB_DATA_R, pin_num);
                }
                else
                {
                    CLEAR_BIT(GPIO_PORTB_DATA_R, pin_num);
                }
            }
            break;

        case PORTC_ID:
            if (BIT_IS_SET(GPIO_PORTC_DIR_R, pin_num))
            {
                if (value == LOGIC_HIGH)
                {
                    SET_BIT(GPIO_PORTC_DATA_R, pin_num);
                }
                else
                {
                    CLEAR_BIT(GPIO_PORTC_DATA_R, pin_num);
                }
            }
            break;

        case PORTD_ID:
            if (BIT_IS_SET(GPIO_PORTD_DIR_R, pin_num))
            {
                if (value == LOGIC_HIGH)
                {
                    SET_BIT(GPIO_PORTD_DATA_R, pin_num);
                }
                else
                {
                    CLEAR_BIT(GPIO_PORTD_DATA_R, pin_num);
                }
            }
            break;

        case PORTE_ID:
            if (BIT_IS_SET(GPIO_PORTE_DIR_R, pin_num))
            {
                if (value == LOGIC_HIGH)
                {
                    SET_BIT(GPIO_PORTE_DATA_R, pin_num);
                }
                else
                {
                    CLEAR_BIT(GPIO_PORTE_DATA_R, pin_num);
                }
            }
            break;

        case PORTF_ID:
            if (BIT_IS_SET(GPIO_PORTF_DIR_R, pin_num))
            {
                if (value == LOGIC_HIGH)
                {
                    SET_BIT(GPIO_PORTF_DATA_R, pin_num);
                }
                else
                {
                    CLEAR_BIT(GPIO_PORTF_DATA_R, pin_num);
                }
            }
            break;

        default:
            /* Should never reach here due to range check */
            break;
        }
    }
}

/*
 * Description :
 * Write the value on the required port.
 * Only pins configured as OUTPUT will be driven.
 */
void GPIO_writePort(uint8_t port_num, uint8_t value)
{
    uint32_t out_mask = 0;

    if (port_num >= NUM_OF_PORTS)
    {
        /* Do nothing */
    }
    else
    {
        switch (port_num)
        {
        case PORTA_ID:
            out_mask = GPIO_PORTA_DIR_R & 0xFF;
            GPIO_PORTA_DATA_R =
                (GPIO_PORTA_DATA_R & ~out_mask) |
                ((uint32_t)value & out_mask);
            break;

        case PORTB_ID:
            out_mask = GPIO_PORTB_DIR_R & 0xFF;
            GPIO_PORTB_DATA_R =
                (GPIO_PORTB_DATA_R & ~out_mask) |
                ((uint32_t)value & out_mask);
            break;

        case PORTC_ID:
            out_mask = GPIO_PORTC_DIR_R & 0xFF;
            GPIO_PORTC_DATA_R =
                (GPIO_PORTC_DATA_R & ~out_mask) |
                ((uint32_t)value & out_mask);
            break;

        case PORTD_ID:
            out_mask = GPIO_PORTD_DIR_R & 0xFF;
            GPIO_PORTD_DATA_R =
                (GPIO_PORTD_DATA_R & ~out_mask) |
                ((uint32_t)value & out_mask);
            break;

        case PORTE_ID:
            out_mask = GPIO_PORTE_DIR_R & 0xFF;
            GPIO_PORTE_DATA_R =
                (GPIO_PORTE_DATA_R & ~out_mask) |
                ((uint32_t)value & out_mask);
            break;

        case PORTF_ID:
            out_mask = GPIO_PORTF_DIR_R & 0xFF;
            GPIO_PORTF_DATA_R =
                (GPIO_PORTF_DATA_R & ~out_mask) |
                ((uint32_t)value & out_mask);
            break;

        default:
            break;
        }
    }
}

/*
 * Description :
 * Read and return the value for the required pin.
 */
uint8_t GPIO_readPin(uint8_t port_num, uint8_t pin_num)
{
    uint8_t pin_value = LOGIC_LOW;

    if ((pin_num >= NUM_OF_PINS_PER_PORT) || (port_num >= NUM_OF_PORTS))
    {
        /* Do nothing */
    }
    else
    {
        switch (port_num)
        {
        case PORTA_ID:
            if (BIT_IS_SET(GPIO_PORTA_DATA_R, pin_num))
            {
                pin_value = LOGIC_HIGH;
            }
            else
            {
                pin_value = LOGIC_LOW;
            }
            break;

        case PORTB_ID:
            if (BIT_IS_SET(GPIO_PORTB_DATA_R, pin_num))
            {
                pin_value = LOGIC_HIGH;
            }
            else
            {
                pin_value = LOGIC_LOW;
            }
            break;

        case PORTC_ID:
            if (BIT_IS_SET(GPIO_PORTC_DATA_R, pin_num))
            {
                pin_value = LOGIC_HIGH;
            }
            else
            {
                pin_value = LOGIC_LOW;
            }
            break;

        case PORTD_ID:
            if (BIT_IS_SET(GPIO_PORTD_DATA_R, pin_num))
            {
                pin_value = LOGIC_HIGH;
            }
            else
            {
                pin_value = LOGIC_LOW;
            }
            break;

        case PORTE_ID:
            if (BIT_IS_SET(GPIO_PORTE_DATA_R, pin_num))
            {
                pin_value = LOGIC_HIGH;
            }
            else
            {
                pin_value = LOGIC_LOW;
            }
            break;

        case PORTF_ID:
            if (BIT_IS_SET(GPIO_PORTF_DATA_R, pin_num))
            {
                pin_value = LOGIC_HIGH;
            }
            else
            {
                pin_value = LOGIC_LOW;
            }
            break;

        default:
            break;
        }
    }

    return pin_value;
}

/*
 * Description :
 * Read and return the value of the required port (lower 8 bits).
 */
uint8_t GPIO_readPort(uint8_t port_num)
{
    uint8_t value = 0;

    if (port_num >= NUM_OF_PORTS)
    {
        /* Do nothing */
    }
    else
    {
        switch (port_num)
        {
        case PORTA_ID:
            value = (uint8_t)(GPIO_PORTA_DATA_R & 0xFF);
            break;

        case PORTB_ID:
            value = (uint8_t)(GPIO_PORTB_DATA_R & 0xFF);
            break;

        case PORTC_ID:
            value = (uint8_t)(GPIO_PORTC_DATA_R & 0xFF);
            break;

        case PORTD_ID:
            value = (uint8_t)(GPIO_PORTD_DATA_R & 0xFF);
            break;

        case PORTE_ID:
            value = (uint8_t)(GPIO_PORTE_DATA_R & 0xFF);
            break;

        case PORTF_ID:
            value = (uint8_t)(GPIO_PORTF_DATA_R & 0xFF);
            break;

        default:
            break;
        }
    }

    return value;
}

/*
 * Description :
 * Enable the internal pull-up resistor on the specified pin.
 */
void GPIO_enablePullUp(uint8_t port_num, uint8_t pin_num)
{
    if ((pin_num >= NUM_OF_PINS_PER_PORT) || (port_num >= NUM_OF_PORTS))
    {
        return;
    }

    switch (port_num)
    {
    case PORTA_ID:
        SET_BIT(GPIO_PORTA_PUR_R, pin_num);
        break;
    case PORTB_ID:
        SET_BIT(GPIO_PORTB_PUR_R, pin_num);
        break;
    case PORTC_ID:
        SET_BIT(GPIO_PORTC_PUR_R, pin_num);
        break;
    case PORTD_ID:
        SET_BIT(GPIO_PORTD_PUR_R, pin_num);
        break;
    case PORTE_ID:
        SET_BIT(GPIO_PORTE_PUR_R, pin_num);
        break;
    case PORTF_ID:
        SET_BIT(GPIO_PORTF_PUR_R, pin_num);
        break;
    default:
        break;
    }
}

/*
 * Description :
 * Disable the internal pull-up resistor on the specified pin.
 */
void GPIO_disablePullUp(uint8_t port_num, uint8_t pin_num)
{
    if ((pin_num >= NUM_OF_PINS_PER_PORT) || (port_num >= NUM_OF_PORTS))
    {
        return;
    }

    switch (port_num)
    {
    case PORTA_ID:
        CLEAR_BIT(GPIO_PORTA_PUR_R, pin_num);
        break;
    case PORTB_ID:
        CLEAR_BIT(GPIO_PORTB_PUR_R, pin_num);
        break;
    case PORTC_ID:
        CLEAR_BIT(GPIO_PORTC_PUR_R, pin_num);
        break;
    case PORTD_ID:
        CLEAR_BIT(GPIO_PORTD_PUR_R, pin_num);
        break;
    case PORTE_ID:
        CLEAR_BIT(GPIO_PORTE_PUR_R, pin_num);
        break;
    case PORTF_ID:
        CLEAR_BIT(GPIO_PORTF_PUR_R, pin_num);
        break;
    default:
        break;
    }
}

/*
 * Description :
 * Enable the internal pull-down resistor on the specified pin.
 */
void GPIO_enablePullDown(uint8_t port_num, uint8_t pin_num)
{
    if ((pin_num >= NUM_OF_PINS_PER_PORT) || (port_num >= NUM_OF_PORTS))
    {
        return;
    }

    switch (port_num)
    {
    case PORTA_ID:
        SET_BIT(GPIO_PORTA_PDR_R, pin_num);
        break;
    case PORTB_ID:
        SET_BIT(GPIO_PORTB_PDR_R, pin_num);
        break;
    case PORTC_ID:
        SET_BIT(GPIO_PORTC_PDR_R, pin_num);
        break;
    case PORTD_ID:
        SET_BIT(GPIO_PORTD_PDR_R, pin_num);
        break;
    case PORTE_ID:
        SET_BIT(GPIO_PORTE_PDR_R, pin_num);
        break;
    case PORTF_ID:
        SET_BIT(GPIO_PORTF_PDR_R, pin_num);
        break;
    default:
        break;
    }
}

/*
 * Description :
 * Disable the internal pull-down resistor on the specified pin.
 */
void GPIO_disablePullDown(uint8_t port_num, uint8_t pin_num)
{
    if ((pin_num >= NUM_OF_PINS_PER_PORT) || (port_num >= NUM_OF_PORTS))
    {
        return;
    }

    switch (port_num)
    {
    case PORTA_ID:
        CLEAR_BIT(GPIO_PORTA_PDR_R, pin_num);
        break;
    case PORTB_ID:
        CLEAR_BIT(GPIO_PORTB_PDR_R, pin_num);
        break;
    case PORTC_ID:
        CLEAR_BIT(GPIO_PORTC_PDR_R, pin_num);
        break;
    case PORTD_ID:
        CLEAR_BIT(GPIO_PORTD_PDR_R, pin_num);
        break;
    case PORTE_ID:
        CLEAR_BIT(GPIO_PORTE_PDR_R, pin_num);
        break;
    case PORTF_ID:
        CLEAR_BIT(GPIO_PORTF_PDR_R, pin_num);
        break;
    default:
        break;
    }
}
