/*****************************************************************************
 * File: hal_keypad.c
 * Description: 4x4 Keypad Driver using MCAL_GPIO for TM4C123GH6PM
 * Author: Your Name
 * Date: [Current Date]
 *****************************************************************************/

#include "hal_keypad.h"
#include "mcal_gpio.h"    // We'll use MCAL_GPIO functions
#include "driverlib/sysctl.h"  // For SYSCTL_PERIPH_GPIOx

/* 
 * Keypad mapping array.
 * Each element represents the character returned for a specific key press.
 * The array is organized as [row][column], matching the physical keypad layout.
 */
const char keypad_codes[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

/* 
 * Arrays for pins (easier to loop through)
 */
static const uint8_t row_pins[KEYPAD_ROWS] = {
    KEYPAD_ROW1_PIN,
    KEYPAD_ROW2_PIN, 
    KEYPAD_ROW3_PIN,
    KEYPAD_ROW4_PIN
};

static const uint8_t col_pins[KEYPAD_COLS] = {
    KEYPAD_COL1_PIN,
    KEYPAD_COL2_PIN,
    KEYPAD_COL3_PIN,
    KEYPAD_COL4_PIN
};

/*
 * HAL_Keypad_Init
 * Initializes the GPIO pins for keypad operation using MCAL_GPIO.
 * - Columns are set as outputs and driven HIGH (PortC).
 * - Rows are set as inputs with internal pull-up resistors (PortA).
 * This function must be called before using HAL_Keypad_GetKey.
 */
void HAL_Keypad_Init(void) {
    /* Enable clock for GPIO ports */
    MCAL_GPIO_EnablePort(SYSCTL_PERIPH_GPIOA);  /* Enable Port A for rows */
    MCAL_GPIO_EnablePort(SYSCTL_PERIPH_GPIOC);  /* Enable Port C for columns */
    
    /* Configure all ROWS as INPUT with pull-up resistors */
    for (uint8_t i = 0; i < KEYPAD_ROWS; i++) {
        MCAL_GPIO_InitPin(KEYPAD_ROW_PORT, 
                         row_pins[i], 
                         GPIO_DIR_INPUT, 
                         GPIO_ATTACH_PULLUP);
    }
    
    /* Configure all COLUMNS as OUTPUT, initially HIGH */
    for (uint8_t i = 0; i < KEYPAD_COLS; i++) {
        MCAL_GPIO_InitPin(KEYPAD_COL_PORT, 
                         col_pins[i], 
                         GPIO_DIR_OUTPUT, 
                         GPIO_ATTACH_DEFAULT);
        /* Set column HIGH (inactive) */
        MCAL_GPIO_WritePin(KEYPAD_COL_PORT, col_pins[i], LOGIC_HIGH);
    }
}

/*
 * HAL_Keypad_GetKey
 * Scans the keypad and returns the character of the pressed key.
 * Returns 0 if no key is pressed.
 *
 * Scanning logic (SAME as before, but using MCAL_GPIO):
 *   1. Set each column LOW one at a time, others HIGH.
 *   2. Read all row inputs; if any row reads LOW, a key is pressed.
 *   3. Wait for key release (debounce).
 *   4. Return the mapped character from keypad_codes.
 */
char HAL_Keypad_GetKey(void) {
    uint8_t row_state;
    
    /* Scan each column */
    for (uint8_t col = 0; col < KEYPAD_COLS; col++) {
        /* Set all columns HIGH (inactive) */
        for (uint8_t c = 0; c < KEYPAD_COLS; c++) {
            MCAL_GPIO_WritePin(KEYPAD_COL_PORT, col_pins[c], LOGIC_HIGH);
        }
        
        /* Set current column LOW (active) */
        MCAL_GPIO_WritePin(KEYPAD_COL_PORT, col_pins[col], LOGIC_LOW);
        
        /* Small delay for signal to settle */
        for (volatile int d = 0; d < 100; d++);
        
        /* Scan each row for key press */
        for (uint8_t row = 0; row < KEYPAD_ROWS; row++) {
            /* IMPORTANT: We need to check if MCAL_GPIO_ReadPin works correctly */
            /* Read the specific row pin */
            row_state = MCAL_GPIO_ReadPin(KEYPAD_ROW_PORT, row_pins[row]);
            
            /* 
             * Since we have pull-up resistors:
             * - Normally HIGH (1) when no key pressed
             * - Goes LOW (0) when key is pressed (connects to column LOW)
             */
            if (row_state == 0) {  /* Key pressed (active LOW) */
                /* Wait for key release (debounce) */
                while (MCAL_GPIO_ReadPin(KEYPAD_ROW_PORT, row_pins[row]) == 0) {
                    /* Wait until key is released */
                }
                
                /* Small debounce delay after release */
                for (volatile int d = 0; d < 1000; d++);
                
                /* Return the mapped character */
                return keypad_codes[row][col];
            }
        }
    }
    
    return 0; /* No key pressed */
}