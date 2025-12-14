/*****************************************************************************
 * File: hal_keypad.h
 * Description: Header for 4x4 Keypad Driver using MCAL_GPIO
 * Author: Your Name
 * Date: [Current Date]
 *****************************************************************************/

#ifndef HAL_KEYPAD_H
#define HAL_KEYPAD_H

#include <stdint.h>
#include "mcal/mcal_gpio.h"  // Include MCAL_GPIO header
#include "Types.h"

/* Keypad dimensions */
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

/* GPIO Ports and Pins - MUST match MCAL_GPIO format */
#define KEYPAD_COL_PORT    GPIO_PORTC_BASE   /* Changed from PORTC to GPIO_PORTC_BASE */
#define KEYPAD_ROW_PORT    GPIO_PORTA_BASE   /* Changed from PORTA to GPIO_PORTA_BASE */

/* Individual column pins */
#define KEYPAD_COL1_PIN    GPIO_PIN_4
#define KEYPAD_COL2_PIN    GPIO_PIN_5
#define KEYPAD_COL3_PIN    GPIO_PIN_6
#define KEYPAD_COL4_PIN    GPIO_PIN_7

/* Individual row pins */
#define KEYPAD_ROW1_PIN    GPIO_PIN_2
#define KEYPAD_ROW2_PIN    GPIO_PIN_3
#define KEYPAD_ROW3_PIN    GPIO_PIN_4
#define KEYPAD_ROW4_PIN    GPIO_PIN_5

/* Combined pin masks (for initialization) */
#define KEYPAD_ALL_COL_PINS (KEYPAD_COL1_PIN | KEYPAD_COL2_PIN | KEYPAD_COL3_PIN | KEYPAD_COL4_PIN)
#define KEYPAD_ALL_ROW_PINS (KEYPAD_ROW1_PIN | KEYPAD_ROW2_PIN | KEYPAD_ROW3_PIN | KEYPAD_ROW4_PIN)

/* Keypad mapping array declaration */
extern const char keypad_codes[KEYPAD_ROWS][KEYPAD_COLS];

/* Function Prototypes */
void HAL_Keypad_Init(void);
char HAL_Keypad_GetKey(void);

#endif /* HAL_KEYPAD_H */