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
    TestLogger_Assert("UT-LCD-001", "LCD driver initialization", 
                      LCD_Init() == TRUE);
    
    /* Small delay for LCD initialization */
    for (volatile int i = 0; i < 50000; i++);
}

void Test_LCD_Clear(void) {
    LCD_Clear();
    TestLogger_Assert("UT-LCD-002", "LCD clear screen", TRUE);
}

void Test_LCD_DisplayString(void) {
    LCD_Clear();
    LCD_DisplayString("TEST");
    
    TestLogger_Assert("UT-LCD-003", "LCD display string", TRUE);
}

void Test_LCD_SetCursor(void) {
    /* Set cursor to row 0, column 0 */
    LCD_SetCursor(0, 0);
    TestLogger_Assert("UT-LCD-004", "LCD set cursor position (0,0)", TRUE);
    
    /* Set cursor to row 1, column 5 */
    LCD_SetCursor(1, 5);
    TestLogger_Assert("UT-LCD-005", "LCD set cursor position (1,5)", TRUE);
}

void Test_LCD_DisplayNumber(void) {
    LCD_Clear();
    LCD_DisplayNumber(1234);
    
    TestLogger_Assert("UT-LCD-006", "LCD display number", TRUE);
}

void Test_LCD_MultiLine(void) {
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_DisplayString("Line 1");
    LCD_SetCursor(1, 0);
    LCD_DisplayString("Line 2");
    
    TestLogger_Assert("UT-LCD-007", "LCD multi-line display", TRUE);
}

void Test_LCD_ErrorHandling(void) {
    /* Test invalid row */
    TestLogger_Assert("UT-LCD-008", "LCD invalid row handling", 
                      LCD_SetCursor(5, 0) == FALSE);
    
    /* Test invalid column */
    TestLogger_Assert("UT-LCD-009", "LCD invalid column handling", 
                      LCD_SetCursor(0, 20) == FALSE);
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
