#ifndef HAL_LCD_I2C_H
#define HAL_LCD_I2C_H

#include <stdint.h>

/******************************************************************************
 *                             LCD Commands                                   *
 ******************************************************************************/

/* Clear display */
#define LCD_CLEAR                0x01

/* Return home (cursor to 0,0) */
#define LCD_HOME                 0x02

/* Entry mode: increment cursor, no shift */
#define LCD_ENTRY_MODE           0x06

/* Display control */
#define LCD_DISPLAY_ON           0x0C  /* Display ON, cursor OFF, blink OFF */
#define LCD_DISPLAY_OFF          0x08
#define LCD_CURSOR_ON            0x0E
#define LCD_CURSOR_BLINK         0x0F

/* Cursor / display shift */
#define LCD_SHIFT_LEFT           0x18
#define LCD_SHIFT_RIGHT          0x1C

/* Function set */
#define LCD_FUNCTION_4BIT_2LINE  0x28  /* 4-bit mode, 2 lines, 5x8 font */
#define LCD_FUNCTION_8BIT_2LINE  0x38  /* (not used here) */

/* DDRAM addresses for 16x2 LCD */
#define LCD_LINE1_ADDR           0x80  /* First line base address */
#define LCD_LINE2_ADDR           0xC0  /* Second line base address */

/******************************************************************************
 *                          Function Prototypes                               *
 ******************************************************************************/

/*
 * LCD_Init
 * Initializes the LCD (I2C, 4-bit HD44780 via PCF8574T).
 * Must be called before any other LCD function.
 */
void LCD_Init(void);

/*
 * LCD_SendCommand
 * Sends a command to the LCD (RS = 0).
 * Parameters: command - LCD command byte
 */
void LCD_SendCommand(uint8_t command);

/*
 * LCD_SendData
 * Sends a data byte to the LCD (RS = 1).
 * Parameters: data - Character to display
 */
void LCD_SendData(uint8_t data);

/*
 * LCD_Clear
 * Clears the LCD display and returns cursor to home position.
 */
void LCD_Clear(void);

/*
 * LCD_SetCursor
 * Sets the cursor position on the LCD.
 * Parameters:
 *   row - Row number (0 or 1)
 *   col - Column number (0 to 15)
 */
void LCD_SetCursor(uint8_t row, uint8_t col);

/*
 * LCD_WriteString
 * Writes a string to the LCD at the current cursor position.
 * Parameters: str - Pointer to null-terminated string
 */
void LCD_WriteString(const char *str);

/*
 * LCD_WriteChar
 * Writes a single character to the LCD at the current cursor position.
 * Parameters: c - Character to display
 */
void LCD_WriteChar(char c);

#endif /* HAL_LCD_I2C_H */
