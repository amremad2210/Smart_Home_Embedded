/*******************************************************************************
 * File: test_main.c
 * Description: Main test runner for all test suites
 * Author: Testing Team
 * Date: December 18, 2025
 ******************************************************************************/

#include "TestFramework/test_logger.h"

/* Unit test prototypes */
extern void Run_GPIO_UnitTests(void);
extern void Run_UART_UnitTests(void);
extern void Run_EEPROM_UnitTests(void);
extern void Run_ADC_UnitTests(void);
extern void Run_Motor_UnitTests(void);
extern void Run_Buzzer_UnitTests(void);
extern void Run_LCD_UnitTests(void);
extern void Run_Keypad_UnitTests(void);

/* Integration test prototypes */
extern void Run_UART_IntegrationTests(void);
extern void Run_EEPROM_Password_IntegrationTests(void);
extern void Run_Motor_Timer_IntegrationTests(void);
extern void Run_Buzzer_Lockout_IntegrationTests(void);

/* System test prototypes */
extern void Run_System_FunctionalTests(void);

/*******************************************************************************
 *                              Main Function                                  *
 *******************************************************************************/

int main(void) {
    /* Initialize test logger */
    TestLogger_Init();
    
    /* ========================================================================
     *                          UNIT TESTS
     * ======================================================================== */
    
    UART_SendString("\r\n\r\n");
    UART_SendString("################################################################################\r\n");
    UART_SendString("#                            UNIT TESTS - MCAL DRIVERS                         #\r\n");
    UART_SendString("################################################################################\r\n");
    
    Run_GPIO_UnitTests();
    Run_UART_UnitTests();
    Run_EEPROM_UnitTests();
    Run_ADC_UnitTests();
    
    UART_SendString("\r\n");
    UART_SendString("################################################################################\r\n");
    UART_SendString("#                            UNIT TESTS - HAL DRIVERS                          #\r\n");
    UART_SendString("################################################################################\r\n");
    
    Run_Motor_UnitTests();
    Run_Buzzer_UnitTests();
    Run_LCD_UnitTests();
    Run_Keypad_UnitTests();
    
    /* ========================================================================
     *                       INTEGRATION TESTS
     * ======================================================================== */
    
    UART_SendString("\r\n\r\n");
    UART_SendString("################################################################################\r\n");
    UART_SendString("#                           INTEGRATION TESTS                                  #\r\n");
    UART_SendString("################################################################################\r\n");
    
    Run_UART_IntegrationTests();
    Run_EEPROM_Password_IntegrationTests();
    Run_Motor_Timer_IntegrationTests();
    Run_Buzzer_Lockout_IntegrationTests();
    
    /* ========================================================================
     *                     SYSTEM FUNCTIONAL TESTS
     * ======================================================================== */
    
    UART_SendString("\r\n\r\n");
    UART_SendString("################################################################################\r\n");
    UART_SendString("#                      SYSTEM FUNCTIONAL TESTS                                 #\r\n");
    UART_SendString("################################################################################\r\n");
    
    Run_System_FunctionalTests();
    
    /* ========================================================================
     *                          FINAL SUMMARY
     * ======================================================================== */
    
    TestLogger_PrintSummary();
    
    /* Keep results displayed */
    while(1) {
        /* Infinite loop to keep system running */
    }
    
    return 0;
}
