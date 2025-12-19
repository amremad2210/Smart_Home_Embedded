/*******************************************************************************
 * File: test_main_himi.c
 * Description: HIMI ECU test runner - coordinates with CONTROL for integration tests
 * Author: Testing Team
 * Date: December 19, 2025
 ******************************************************************************/

#include "TestFramework/test_logger.h"

/* HIMI-specific unit test prototypes */
extern void Run_GPIO_UnitTests(void);
extern void Run_UART_UnitTests(void);
extern void Run_EEPROM_UnitTests(void);
extern void Run_ADC_UnitTests(void);
extern void Run_LCD_UnitTests(void);
extern void Run_Keypad_UnitTests(void);

/* Integration tests - HIMI side */
extern void Run_UART_IntegrationTests_HIMI_Side(void);
extern void Run_EEPROM_Password_IntegrationTests(void);

/* System tests - HIMI coordinates */
extern void Run_System_FunctionalTests_HIMI_Side(void);

/*******************************************************************************
 *                           Synchronization Commands                          *
 *******************************************************************************/

/* Send sync commands to CONTROL ECU */
#define CMD_START_INTEGRATION_TESTS  0xA1
#define CMD_START_SYSTEM_TESTS       0xA2
#define CMD_TEST_COMPLETE            0xA3
#define CMD_READY                    0xA4

void SendCommandToCONTROL(uint8 command) {
    HAL_COMM_SendByte(command);
    /* Wait for acknowledgment */
    for (volatile int i = 0; i < 100000; i++);
}

uint8 WaitForCONTROLReady(void) {
    /* Wait for CONTROL to signal ready */
    uint8 response = HAL_COMM_ReceiveByte();
    return (response == CMD_READY);
}

/*******************************************************************************
 *                              Main Function                                  *
 *******************************************************************************/

int main(void) {
    /* Initialize test logger */
    TestLogger_Init();
    
    UART_SendString("\r\n\r\n");
    UART_SendString("################################################################################\r\n");
    UART_SendString("#                    HIMI ECU - TEST SUITE STARTING                            #\r\n");
    UART_SendString("################################################################################\r\n");
    UART_SendString("\r\n>>> Waiting for CONTROL ECU to be ready...\r\n");
    
    /* Wait for CONTROL ECU to signal ready */
    while(!WaitForCONTROLReady()) {
        /* Wait for CONTROL */
    }
    
    UART_SendString(">>> CONTROL ECU ready. Starting tests...\r\n\r\n");
    
    /* ========================================================================
     *                    UNIT TESTS - HIMI SPECIFIC
     * ======================================================================== */
    
    UART_SendString("\r\n");
    UART_SendString("################################################################################\r\n");
    UART_SendString("#                      UNIT TESTS - HIMI MCAL DRIVERS                          #\r\n");
    UART_SendString("################################################################################\r\n");
    
    Run_GPIO_UnitTests();
    Run_UART_UnitTests();
    Run_EEPROM_UnitTests();
    Run_ADC_UnitTests();
    
    UART_SendString("\r\n");
    UART_SendString("################################################################################\r\n");
    UART_SendString("#                      UNIT TESTS - HIMI HAL DRIVERS                           #\r\n");
    UART_SendString("################################################################################\r\n");
    
    Run_LCD_UnitTests();
    Run_Keypad_UnitTests();
    /* Note: RGB LED and Potentiometer tests would go here if available */
    
    /* ========================================================================
     *              INTEGRATION TESTS - HIMI COORDINATES WITH CONTROL
     * ======================================================================== */
    
    UART_SendString("\r\n\r\n");
    UART_SendString("################################################################################\r\n");
    UART_SendString("#                   INTEGRATION TESTS - HIMI SIDE                              #\r\n");
    UART_SendString("################################################################################\r\n");
    
    /* Signal CONTROL to start integration tests */
    UART_SendString(">>> Signaling CONTROL ECU to start integration tests...\r\n");
    SendCommandToCONTROL(CMD_START_INTEGRATION_TESTS);
    
    /* Run HIMI side of integration tests */
    Run_UART_IntegrationTests_HIMI_Side();
    Run_EEPROM_Password_IntegrationTests();
    
    /* Note: Motor Timer and Buzzer Lockout tests run on CONTROL side only */
    
    /* ========================================================================
     *              SYSTEM FUNCTIONAL TESTS - HIMI COORDINATES
     * ======================================================================== */
    
    UART_SendString("\r\n\r\n");
    UART_SendString("################################################################################\r\n");
    UART_SendString("#                SYSTEM FUNCTIONAL TESTS - HIMI SIDE                           #\r\n");
    UART_SendString("################################################################################\r\n");
    
    /* Signal CONTROL to start system tests */
    UART_SendString(">>> Signaling CONTROL ECU to start system tests...\r\n");
    SendCommandToCONTROL(CMD_START_SYSTEM_TESTS);
    
    /* Run HIMI side of system tests */
    Run_System_FunctionalTests_HIMI_Side();
    
    /* ========================================================================
     *                          FINAL SUMMARY
     * ======================================================================== */
    
    UART_SendString("\r\n>>> Signaling CONTROL ECU that tests are complete...\r\n");
    SendCommandToCONTROL(CMD_TEST_COMPLETE);
    
    UART_SendString("\r\n");
    UART_SendString("################################################################################\r\n");
    UART_SendString("#                         HIMI ECU TEST SUMMARY                                #\r\n");
    UART_SendString("################################################################################\r\n");
    
    TestLogger_PrintSummary();
    
    /* Keep results displayed */
    while(1) {
        /* Infinite loop to keep system running */
    }
    
    return 0;
}
