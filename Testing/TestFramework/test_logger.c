/*******************************************************************************
 * File: test_logger.c
 * Description: Test logging framework implementation
 * Author: Testing Team
 * Date: December 18, 2025
 ******************************************************************************/

#include "test_logger.h"
#include "../../Common/inc/mcal/mcal_uart.h"
#include <string.h>
#include <stdio.h>

/*******************************************************************************
 *                              Global Variables                               *
 *******************************************************************************/

uint16 g_totalTests = 0;
uint16 g_passedTests = 0;
uint16 g_failedTests = 0;

static char logBuffer[256];

/*******************************************************************************
 *                              Function Definitions                           *
 *******************************************************************************/

void TestLogger_Init(void) {
    /* Initialize UART for logging */
    UART_Init();
    
    /* Reset counters */
    g_totalTests = 0;
    g_passedTests = 0;
    g_failedTests = 0;
    
    /* Print header */
    UART_SendString("\r\n");
    UART_SendString("================================================================================\r\n");
    UART_SendString("              SMART HOME EMBEDDED - AUTOMATED TEST FRAMEWORK                    \r\n");
    UART_SendString("================================================================================\r\n");
    UART_SendString("Date: December 18, 2025\r\n");
    UART_SendString("================================================================================\r\n\r\n");
}

void TestLogger_StartSuite(const char* suiteName) {
    /* Print suite header */
    UART_SendString("\r\n");
    UART_SendString("--------------------------------------------------------------------------------\r\n");
    UART_SendString("TEST SUITE: ");
    UART_SendString((uint8*)suiteName);
    UART_SendString("\r\n");
    UART_SendString("--------------------------------------------------------------------------------\r\n");
}

void TestLogger_LogTest(const TestCase* testCase) {
    g_totalTests++;
    
    if (testCase->result == TEST_PASS) {
        g_passedTests++;
    } else {
        g_failedTests++;
    }
    
    /* Format: [PASS/FAIL] TestID | Scenario | Expected | Actual */
    sprintf(logBuffer, "[%s] %s | %s | Expected: %s | Actual: %s\r\n",
            testCase->result == TEST_PASS ? "PASS" : "FAIL",
            testCase->testId,
            testCase->scenario,
            testCase->expected,
            testCase->actual);
    
    UART_SendString((uint8*)logBuffer);
}

TestResult TestLogger_Assert(const char* testId, const char* description, uint8 condition) {
    TestCase test;
    test.testId = testId;
    test.scenario = description;
    test.expected = "TRUE";
    test.actual = condition ? "TRUE" : "FALSE";
    test.result = condition ? TEST_PASS : TEST_FAIL;
    
    TestLogger_LogTest(&test);
    
    return test.result;
}

void TestLogger_EndSuite(void) {
    UART_SendString("--------------------------------------------------------------------------------\r\n");
    sprintf(logBuffer, "Suite Complete: %d tests | %d passed | %d failed\r\n",
            g_totalTests, g_passedTests, g_failedTests);
    UART_SendString((uint8*)logBuffer);
    UART_SendString("--------------------------------------------------------------------------------\r\n\r\n");
}

void TestLogger_PrintSummary(void) {
    float passRate = (g_totalTests > 0) ? ((float)g_passedTests / g_totalTests * 100.0f) : 0.0f;
    
    UART_SendString("\r\n");
    UART_SendString("================================================================================\r\n");
    UART_SendString("                           FINAL TEST SUMMARY                                   \r\n");
    UART_SendString("================================================================================\r\n");
    
    sprintf(logBuffer, "Total Tests:  %d\r\n", g_totalTests);
    UART_SendString((uint8*)logBuffer);
    
    sprintf(logBuffer, "Passed:       %d\r\n", g_passedTests);
    UART_SendString((uint8*)logBuffer);
    
    sprintf(logBuffer, "Failed:       %d\r\n", g_failedTests);
    UART_SendString((uint8*)logBuffer);
    
    sprintf(logBuffer, "Pass Rate:    %.2f%%\r\n", passRate);
    UART_SendString((uint8*)logBuffer);
    
    UART_SendString("================================================================================\r\n");
    
    if (g_failedTests == 0) {
        UART_SendString("                        ALL TESTS PASSED!                                      \r\n");
    } else {
        UART_SendString("                   SOME TESTS FAILED - REVIEW LOGS                            \r\n");
    }
    UART_SendString("================================================================================\r\n\r\n");
}

void TestLogger_Reset(void) {
    g_totalTests = 0;
    g_passedTests = 0;
    g_failedTests = 0;
}
