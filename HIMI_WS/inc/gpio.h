/******************************************************************************
 *
 * Module: GPIO
 *
 * File Name: gpio.h
 *
 * Description: Header file for the TM4C123GH6PM (Tiva-C LaunchPad) GPIO driver
 *
 * Note:
 *
 *******************************************************************************/

#ifndef GPIO_H_
#define GPIO_H_

#include "Types.h"


/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/


/*
 * TM4C123GH6PM has GPIO ports A–F (some pins are locked or reserved).
 * We keep the same style as the AVR driver and just extend the number of ports.
 */

#define NUM_OF_PORTS            6
#define NUM_OF_PINS_PER_PORT    8

#define PORTA_ID                0
#define PORTB_ID                1
#define PORTC_ID                2
#define PORTD_ID                3
#define PORTE_ID                4
#define PORTF_ID                5


#define PIN0_ID                 0
#define PIN1_ID                 1
#define PIN2_ID                 2
#define PIN3_ID                 3
#define PIN4_ID                 4
#define PIN5_ID                 5
#define PIN6_ID                 6
#define PIN7_ID                 7
#define PIN8_ID                 8

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/


typedef enum{
  
  PIN_INPUT,
  PIN_OUTPUT
  
}GPIO_PinDirectionType;


typedef enum{
  
  PORT_INPUT,
  PORT_OUTPUT = 0xFF
  
}GPIO_PortDirectionType;


/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/


/*
 * Description :
 * Enable the clock for the required GPIO port and wait until it is ready.
 * Must be called BEFORE any other GPIO operation on that port.
 */
void GPIO_enableClock(uint8_t port_num);

/*
 * Description :
 * Setup the direction of the required pin input/output.
 * If the input port number or pin number are not correct, the function will not
 * handle the request.
 *
 * Note for Tiva-C:
 *  - This function also enables the digital function (DEN) for the pin.
 *  - Alternate function / analog mode are disabled for this pin (pure GPIO).
 */

void GPIO_setupPinDirection(uint8_t port_num, uint8_t pin_num, GPIO_PinDirectionType direction);


/*
 * Description :
 * Write the value Logic High or Logic Low on the required pin.
 * If the input port number or pin number are not correct, the function will not
 * handle the request.
 *
 * Behaviour:
 *  - If the pin is configured as OUTPUT, this drives the pin value.
 *  - If the pin is configured as INPUT, this controls the internal pull-up
 *    resistor (value=1 -> enable pull-up, value=0 -> disable pull-up).
 */
void GPIO_writePin(uint8_t port_num, uint8_t pin_num, uint8_t value);


/*
 * Description :
 * Read and return the value for the required pin, it should be Logic High or
 * Logic Low.
 * If the input port number or pin number are not correct, the function will
 * return Logic Low.
 */
uint8_t GPIO_readPin(uint8_t port_num, uint8_t pin_num);

/*
 * Description :
 * Setup the direction of the required port – all pins input/output.
 * If the direction value is PORT_INPUT all pins in this port should be input
 * pins.
 * If the direction value is PORT_OUTPUT all pins in this port should be output
 * pins.
 * If the input port number is not correct, the function will not handle the
 * request.
 *
 * Note for Tiva-C:
 *  - This function affects pins 0..7 of the selected port only.
 *  - It also enables the digital function (DEN) for these pins and disables
 *    alternate- and analog-functions.
 */
void GPIO_setupPortDirection(uint8_t port_num, GPIO_PortDirectionType direction);


/*
 * Description :
 * Write the value on the required port (lower 8 bits).
 * If any pin in the port is output pin the value will be written on that pin.
 * If any pin in the port is input pin this call will enable/disable the
 * internal pull-up resistor for that pin according to the corresponding bit
 * in "value".
 * If the input port number is not correct, the function will not handle the
 * request.
 */
void GPIO_writePort(uint8_t port_num, uint8_t value);

/*
 * Description :
 * Read and return the value of the required port (lower 8 bits).
 * If the input port number is not correct, the function will return ZERO value.
 */
uint8_t GPIO_readPort(uint8_t port_num);


#endif /* GPIO_H_ */