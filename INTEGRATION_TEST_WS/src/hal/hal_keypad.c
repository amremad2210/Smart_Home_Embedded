/*****************************************************************************
 * File: hal_keypad.c
 * Description: 4x4 Keypad Driver using MCAL_GPIO for TM4C123GH6PM
 * Modified for Integration Testing
 *****************************************************************************/

#include "hal/hal_keypad.h"
#include "driverlib/sysctl.h"  // For SYSCTL_PERIPH_GPIOx
#include "mcal/mcal_systick.h" // For debounce delays

/*
 * HAL_Keypad_Init
 * Mock init for testing
 */
void HAL_Keypad_Init(void) {
    /* No-op for test */
}

/*
 * HAL_Keypad_GetKey
 * For testing, return next char from simulated_key_press string
 */
uint8_t HAL_Keypad_GetKey(void) {
    static const char *key_sequence = NULL;
    static int index = 0;
    extern char simulated_key_press[32];  // Change to array

    if (key_sequence == NULL) {
        key_sequence = simulated_key_press;
        index = 0;
    }

    if (key_sequence[index] != '\0') {
        return key_sequence[index++];
    }
    return 0; /* No key */
}