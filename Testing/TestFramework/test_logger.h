/*******************************************************************************
 * File: test_logger.h
 * Description: Test logging framework for automated pass/fail reporting
 * Author: Testing Team
 * Date: December 18, 2025
 ******************************************************************************/

#ifndef TEST_LOGGER_H_
#define TEST_LOGGER_H_

#include "../../Common/inc/Types.h"

/*******************************************************************************
 *                              Type Definitions                               *
 *******************************************************************************/

typedef enum {
    TEST_PASS = 0,
    TEST_FAIL = 1
} TestResult;

typedef struct {
    const char* testId;
    const char* scenario;
    const char* expected;
    const char* actual;
    TestResult result;
} TestCase;

/*******************************************************************************
 *                              Global Variables                               *
 *******************************************************************************/

extern uint16 g_totalTests;
extern uint16 g_passedTests;
extern uint16 g_failedTests;

/*******************************************************************************
 *                              Function Prototypes                            *
 *******************************************************************************/

/*
 * Description: Initialize the test logger (UART and LCD)
 * Parameters: None
 * Returns: None
 */
void TestLogger_Init(void);

/*
 * Description: Start a new test suite
 * Parameters: suiteName - Name of the test suite
 * Returns: None
 */
void TestLogger_StartSuite(const char* suiteName);

/*
 * Description: Log a test case result
 * Parameters: testCase - Pointer to test case structure
 * Returns: None
 */
void TestLogger_LogTest(const TestCase* testCase);

/*
 * Description: Log a test assertion
 * Parameters: 
 *   - testId: Test identifier
 *   - description: Test description
 *   - condition: Boolean condition (TRUE = pass, FALSE = fail)
 * Returns: Test result
 */
TestResult TestLogger_Assert(const char* testId, const char* description, uint8 condition);

/*
 * Description: End the test suite and display summary
 * Parameters: None
 * Returns: None
 */
void TestLogger_EndSuite(void);

/*
 * Description: Print test summary to UART
 * Parameters: None
 * Returns: None
 */
void TestLogger_PrintSummary(void);

/*
 * Description: Reset test counters
 * Parameters: None
 * Returns: None
 */
void TestLogger_Reset(void);

#endif /* TEST_LOGGER_H_ */
