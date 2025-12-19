/*******************************************************************************
 * File: test_main_control.c
 * Description: CONTROL ECU test runner - responds to HIMI for integration tests
 * Author: Testing Team
 * Date: December 19, 2025
 ******************************************************************************/

#include "TestFramework/test_logger.h"

/* CONTROL-specific unit test prototypes */
extern void Run_GPIO_UnitTests(void);
extern void Run_UART_UnitTests(void);
extern void Run_EEPROM_UnitTests(void);
extern void Run_Motor_UnitTests(void);
extern void Run_Buzzer_UnitTests(void);

/* Integration tests - CONTROL side */
extern void Run_UART_IntegrationTests_CONTROL_Side(void);
extern void Run_Motor_Timer_IntegrationTests(void);
extern void Run_Buzzer_Lockout_IntegrationTests(void);

/* System tests - CONTROL responds */
extern void Run_System_FunctionalTests_CONTROL_Side(void);

/*******************************************************************************
 *                           Synchronization Commands                          *
 *******************************************************************************/

/* Commands from HIMI ECU */
#define CMD_START_INTEGRATION_TESTS  0xA1
#define CMD_START_SYSTEM_TESTS       0xA2
#define CMD_TEST_COMPLETE            0xA3
#define CMD_READY                    0xA4

void SendReadyToHIMI(void) {
    HAL_COMM_SendByte(CMD_READY);
}

uint8 WaitForHIMICommand(void) {
    /* Wait for command from HIMI */
    return HAL_COMM_ReceiveByte();
}

/*******************************************************************************
 *                              Main Function                                  *
 *******************************************************************************/

int main(void) {
    /* Initialize test logger */
    TestLogger_Init();
    
    UART_SendString("\r\n\r\n");
    UART_SendString("################################################################################\r\n");
    UART_SendString("#                  CONTROL ECU - TEST SUITE STARTING                           #\r\n");
    UART_SendString("################################################################################\r\n");
    
    /* ========================================================================
     *                  UNIT TESTS - CONTROL SPECIFIC
     * ======================================================================== */
    
    UART_SendString("\r\n");
    UART_SendString("################################################################################\r\n");
    UART_SendString("#                    UNIT TESTS - CONTROL MCAL DRIVERS                         #\r\n");
    UART_SendString("################################################################################\r\n");
    
    Run_GPIO_UnitTests();
    Run_UART_UnitTests();
    Run_EEPROM_UnitTests();
    
    UART_SendString("\r\n");
    UART_SendString("################################################################################\r\n");
    UART_SendString("#                    UNIT TESTS - CONTROL HAL DRIVERS                          #\r\n");
    UART_SendString("################################################################################\r\n");
    
    Run_Motor_UnitTests();
    Run_Buzzer_UnitTests();
    
    /* ========================================================================
     *            SIGNAL READY AND WAIT FOR INTEGRATION TESTS
     * ======================================================================== */
    
    UART_SendString("\r\n>>> Signaling HIMI ECU that CONTROL is ready...\r\n");
    SendReadyToHIMI();
    
    UART_SendString(">>> Waiting for HIMI to start integration tests...\r\n");
    uint8 command = WaitForHIMICommand();
    
    if (command == CMD_START_INTEGRATION_TESTS) {
        UART_SendString("\r\n\r\n");
        UART_SendString("################################################################################\r\n");
        UART_SendString("#                 INTEGRATION TESTS - CONTROL SIDE                             #\r\n");
        UART_SendString("################################################################################\r\n");
        
        /* Run CONTROL side of integration tests */
        Run_UART_IntegrationTests_CONTROL_Side();
        Run_Motor_Timer_IntegrationTests();
        Run_Buzzer_Lockout_IntegrationTests();
        
        /* Note: EEPROM Password tests run on HIMI side only */
    }
    
    /* ========================================================================
     *              WAIT FOR SYSTEM FUNCTIONAL TESTS
     * ======================================================================== */
    
    UART_SendString("\r\n>>> Waiting for HIMI to start system tests...\r\n");
    command = WaitForHIMICommand();
    
    if (command == CMD_START_SYSTEM_TESTS) {
        UART_SendString("\r\n\r\n");
        UART_SendString("################################################################################\r\n");
        UART_SendString("#              SYSTEM FUNCTIONAL TESTS - CONTROL SIDE                          #\r\n");
        UART_SendString("################################################################################\r\n");
        
        /* Run CONTROL side of system tests */
        Run_System_FunctionalTests_CONTROL_Side();
    }
    
    /* ========================================================================
     *                     WAIT FOR TEST COMPLETION
     * ======================================================================== */
    
    UART_SendString("\r\n>>> Waiting for HIMI to complete tests...\r\n");
    command = WaitForHIMICommand();
    
    if (command == CMD_TEST_COMPLETE) {
        UART_SendString(">>> HIMI tests complete.\r\n");
    }
    
    /* ========================================================================
     *                          FINAL SUMMARY
     * ======================================================================== */
    
    UART_SendString("\r\n");
    UART_SendString("################################################################################\r\n");
    UART_SendString("#                       CONTROL ECU TEST SUMMARY                               #\r\n");
    UART_SendString("################################################################################\r\n");
    
    TestLogger_PrintSummary();
    
    /* Keep results displayed */
    while(1) {
        /* Infinite loop to keep system running */
    }
    
    return 0;
}
