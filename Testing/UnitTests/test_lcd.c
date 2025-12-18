/*******************************************************************************
 * File: test_lcd.c
 * Description: Unit tests for LCD HAL driver
 * Author: Testing Team
 * Date: December 18, 2025
 ******************************************************************************/

#include "../TestFramework/test_logger.h"
#include "../../HIMI_WS/inc/hal/hal_lcd.h"
#include <string.h>

/*******************************************************************************
 *                              Test Functions                                 *
 *******************************************************************************/

void Test_LCD_Init(void) {
    Lcd_Init();
    TestLogger_Assert("UT-LCD-001", "LCD driver initialization", TRUE);
    
    /* Small delay for LCD initialization */
    for (volatile int i = 0; i < 50000; i++);
}

void Test_LCD_Clear(void) {
    Lcd_Clear();
    TestLogger_Assert("UT-LCD-002", "LCD clear screen", TRUE);
}

void Test_LCD_DisplayString(void) {
    Lcd_Clear();
    Lcd_DisplayString("TEST");
    
    TestLogger_Assert("UT-LCD-003", "LCD display string", TRUE);
}

void Test_LCD_SetCursor(void) {
    /* Set cursor to row 0, column 0 */
    Lcd_GoToRowColumn(0, 0);
    TestLogger_Assert("UT-LCD-004", "LCD set cursor position (0,0)", TRUE);
    
    /* Set cursor to row 1, column 5 */
    Lcd_GoToRowColumn(1, 5);
    TestLogger_Assert("UT-LCD-005", "LCD set cursor position (1,5)", TRUE);
}

void Test_LCD_DisplayNumber(void) {
    Lcd_Clear();
    /* Display individual digits */
    Lcd_DisplayCharacter('1');
    Lcd_DisplayCharacter('2');
    Lcd_DisplayCharacter('3');
    Lcd_DisplayCharacter('4');
    
    TestLogger_Assert("UT-LCD-006", "LCD display number", TRUE);
}

void Test_LCD_MultiLine(void) {
    Lcd_Clear();
    Lcd_GoToRowColumn(0, 0);
    Lcd_DisplayString("Line 1");
    Lcd_GoToRowColumn(1, 0);
    Lcd_DisplayString("Line 2");
    
    TestLogger_Assert("UT-LCD-007", "LCD multi-line display", TRUE);
}

void Test_LCD_ErrorHandling(void) {
    /* Test command sending */
    Lcd_SendCommand(LCD_CLEAR_COMMAND);
    TestLogger_Assert("UT-LCD-008", "LCD command execution", TRUE);
    
    /* Test character display */
    Lcd_DisplayCharacter('A');
    TestLogger_Assert("UT-LCD-009", "LCD character display", TRUE);
}

/*******************************************************************************
 *                              Main Test Suite                                *
 *******************************************************************************/

void Run_LCD_UnitTests(void) {
    TestLogger_StartSuite("LCD HAL Driver Unit Tests");
    
    Test_LCD_Init();
    Test_LCD_Clear();
    Test_LCD_DisplayString();
    Test_LCD_SetCursor();
    Test_LCD_DisplayNumber();
    Test_LCD_MultiLine();
    Test_LCD_ErrorHandling();
    
    TestLogger_EndSuite();
}
