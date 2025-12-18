/******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.h
 *
 * Description: Header file for the Buzzer driver
 *
 *******************************************************************************/

#ifndef HAL_BUZZER_H_
#define HAL_BUZZER_H_

#include <stdint.h>
#include "mcal/mcal_gpio.h" /* Included to access TivaWare definitions for Port/Pin */

/*******************************************************************************
 * Definitions                                  *
 *******************************************************************************/

/* * Configure the Buzzer Location here 
 * NOTE: You need both the Peripheral ID (for clock) and Port Base (for IO)
 */
/* Hardware wiring: Buzzer is connected to PE4 */
#define BUZZER_PERIPH       SYSCTL_PERIPH_GPIOD
#define BUZZER_PORT_BASE    GPIO_PORTD_BASE
#define BUZZER_PIN          GPIO_PIN_1

/* Buzzer States */
#define BUZZER_ON   1
#define BUZZER_OFF  0

/*******************************************************************************
 * Function Prototypes                              *
 *******************************************************************************/

/* Initialize buzzer pin as OUTPUT */
void BUZZER_init(void);

/* Turn buzzer on or off */
void BUZZER_setState(uint8_t state);

/* Make a blocking beep (uses SysTick delay) */
void BUZZER_beep(uint32_t duration_ms);

#endif /* HAL_BUZZER_H_ */