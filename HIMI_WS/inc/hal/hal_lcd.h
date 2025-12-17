/*****************************************************************************
 * File:hal_ lcd.h
 * Description: Header for 16x2 LCD Driver (4-bit mode)
 * Author: Ahmedhh
 * Date: December 4, 2025
 * 
 * Pin Configuration:
 *   RS  -> PD0 (Register Select: 0=Command, 1=Data)
 *   EN  -> PD1 (Enable signal)
 *   D4  -> PD2 (Data bit 4)
 *   D5  -> PD3 (Data bit 5)
 *   D6  -> PE4 (Data bit 6)
 *   D7  -> PE5 (Data bit 7)
 *****************************************************************************/

#ifndef HAL_LCD_H
#define HAL_LCD_H

#include <stdint.h>

/* ================================================================= */
/* LCD COMMANDS                                                      */
/* ================================================================= */
#define LCD_CLEAR_COMMAND              0x01
#define LCD_GO_TO_HOME                 0x02
#define LCD_TWO_LINES_FOUR_BITS        0x28
#define LCD_CURSOR_OFF                 0x0C
#define LCD_CURSOR_ON                  0x0E
#define LCD_SET_CURSOR_LOCATION        0x80

/* ================================================================= */
/* FUNCTION PROTOTYPES                                               */
/* ================================================================= */

/* Initialize the LCD (starts I2C and sets up 4-bit mode) */
void Lcd_Init(void);

/* Send a raw command (e.g., clear screen, move cursor) */
void Lcd_SendCommand(uint8_t command);

/* Display a single character (e.g., 'A') */
void Lcd_DisplayCharacter(uint8_t data);

/* Display a full string (e.g., "Door Locked") */
void Lcd_DisplayString(char *Str);

/* Move cursor to specific position (Row: 0-1, Col: 0-15) */
void Lcd_GoToRowColumn(uint8_t row, uint8_t col);

/* Clear the screen */
void Lcd_Clear(void);

#endif /* LCD_H */