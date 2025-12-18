/*******************************************************************************
 * File: test_gpio.c
 * Description: Unit tests for GPIO driver
 * Author: Testing Team
 * Date: December 18, 2025
 ******************************************************************************/

#include "../TestFramework/test_logger.h"
#include "../../Common/inc/mcal/mcal_gpio.h"

/*******************************************************************************
 *                              Test Functions                                 *
 *******************************************************************************/

void Test_GPIO_Init(void) {
    TestLogger_Assert("UT-GPIO-001", "GPIO pin initialization as output", 
                      GPIO_SetPinDirection(PORTA, PIN0, PIN_OUTPUT) == TRUE);
    
    TestLogger_Assert("UT-GPIO-002", "GPIO pin initialization as input", 
                      GPIO_SetPinDirection(PORTA, PIN1, PIN_INPUT) == TRUE);
    
    TestLogger_Assert("UT-GPIO-003", "GPIO invalid port handling", 
                      GPIO_SetPinDirection(0xFF, PIN0, PIN_OUTPUT) == FALSE);
    
    TestLogger_Assert("UT-GPIO-004", "GPIO invalid pin handling", 
                      GPIO_SetPinDirection(PORTA, 0xFF, PIN_OUTPUT) == FALSE);
}

void Test_GPIO_WriteRead(void) {
    /* Setup: Configure as output */
    GPIO_SetPinDirection(PORTB, PIN0, PIN_OUTPUT);
    
    /* Test write HIGH */
    GPIO_WritePin(PORTB, PIN0, HIGH);
    TestLogger_Assert("UT-GPIO-005", "GPIO write HIGH operation", 
                      GPIO_ReadPin(PORTB, PIN0) == HIGH);
    
    /* Test write LOW */
    GPIO_WritePin(PORTB, PIN0, LOW);
    TestLogger_Assert("UT-GPIO-006", "GPIO write LOW operation", 
                      GPIO_ReadPin(PORTB, PIN0) == LOW);
}

void Test_GPIO_Toggle(void) {
    GPIO_SetPinDirection(PORTC, PIN0, PIN_OUTPUT);
    GPIO_WritePin(PORTC, PIN0, LOW);
    
    GPIO_TogglePin(PORTC, PIN0);
    TestLogger_Assert("UT-GPIO-007", "GPIO toggle from LOW to HIGH", 
                      GPIO_ReadPin(PORTC, PIN0) == HIGH);
    
    GPIO_TogglePin(PORTC, PIN0);
    TestLogger_Assert("UT-GPIO-008", "GPIO toggle from HIGH to LOW", 
                      GPIO_ReadPin(PORTC, PIN0) == LOW);
}

void Test_GPIO_PullUp(void) {
    GPIO_SetPinDirection(PORTD, PIN0, PIN_INPUT);
    GPIO_EnablePullUp(PORTD, PIN0);
    
    TestLogger_Assert("UT-GPIO-009", "GPIO pull-up enable", 
                      GPIO_ReadPin(PORTD, PIN0) == HIGH);
}

/*******************************************************************************
 *                              Main Test Suite                                *
 *******************************************************************************/

void Run_GPIO_UnitTests(void) {
    TestLogger_StartSuite("GPIO Driver Unit Tests");
    
    Test_GPIO_Init();
    Test_GPIO_WriteRead();
    Test_GPIO_Toggle();
    Test_GPIO_PullUp();
    
    TestLogger_EndSuite();
}
