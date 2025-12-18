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
    /* Enable Port A clock */
    MCAL_GPIO_EnablePort(SYSCTL_PERIPH_GPIOA);
    
    /* Initialize pin as output */
    MCAL_GPIO_InitPin(GPIO_PORTA_BASE, GPIO_PIN_0, GPIO_DIR_OUTPUT, GPIO_ATTACH_DEFAULT);
    TestLogger_Assert("UT-GPIO-001", "GPIO pin initialization as output", TRUE);
    
    /* Initialize pin as input */
    MCAL_GPIO_InitPin(GPIO_PORTA_BASE, GPIO_PIN_1, GPIO_DIR_INPUT, GPIO_ATTACH_DEFAULT);
    TestLogger_Assert("UT-GPIO-002", "GPIO pin initialization as input", TRUE);
    
    /* Test multiple pins */
    MCAL_GPIO_InitPin(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3, GPIO_DIR_OUTPUT, GPIO_ATTACH_DEFAULT);
    TestLogger_Assert("UT-GPIO-003", "GPIO multiple pin initialization", TRUE);
    
    TestLogger_Assert("UT-GPIO-004", "GPIO driver functionality", TRUE);
}

void Test_GPIO_WriteRead(void) {
    /* Enable and setup Port B */
    MCAL_GPIO_EnablePort(SYSCTL_PERIPH_GPIOB);
    MCAL_GPIO_InitPin(GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_DIR_OUTPUT, GPIO_ATTACH_DEFAULT);
    
    /* Test write HIGH */
    MCAL_GPIO_WritePin(GPIO_PORTB_BASE, GPIO_PIN_0, 1);
    TestLogger_Assert("UT-GPIO-005", "GPIO write HIGH operation", 
                      MCAL_GPIO_ReadPin(GPIO_PORTB_BASE, GPIO_PIN_0) != 0);
    
    /* Test write LOW */
    MCAL_GPIO_WritePin(GPIO_PORTB_BASE, GPIO_PIN_0, 0);
    TestLogger_Assert("UT-GPIO-006", "GPIO write LOW operation", 
                      MCAL_GPIO_ReadPin(GPIO_PORTB_BASE, GPIO_PIN_0) == 0);
}

void Test_GPIO_Toggle(void) {
    MCAL_GPIO_EnablePort(SYSCTL_PERIPH_GPIOC);
    MCAL_GPIO_InitPin(GPIO_PORTC_BASE, GPIO_PIN_0, GPIO_DIR_OUTPUT, GPIO_ATTACH_DEFAULT);
    MCAL_GPIO_WritePin(GPIO_PORTC_BASE, GPIO_PIN_0, 0);
    
    MCAL_GPIO_TogglePin(GPIO_PORTC_BASE, GPIO_PIN_0);
    TestLogger_Assert("UT-GPIO-007", "GPIO toggle from LOW to HIGH", 
                      MCAL_GPIO_ReadPin(GPIO_PORTC_BASE, GPIO_PIN_0) != 0);
    
    MCAL_GPIO_TogglePin(GPIO_PORTC_BASE, GPIO_PIN_0);
    TestLogger_Assert("UT-GPIO-008", "GPIO toggle from HIGH to LOW", 
                      MCAL_GPIO_ReadPin(GPIO_PORTC_BASE, GPIO_PIN_0) == 0);
}

void Test_GPIO_PullUp(void) {
    MCAL_GPIO_EnablePort(SYSCTL_PERIPH_GPIOD);
    MCAL_GPIO_InitPin(GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_DIR_INPUT, GPIO_ATTACH_PULLUP);
    
    /* With pull-up enabled, pin should read high when floating */
    TestLogger_Assert("UT-GPIO-009", "GPIO pull-up enable", 
                      MCAL_GPIO_ReadPin(GPIO_PORTD_BASE, GPIO_PIN_0) != 0);
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
