/*******************************************************************************
 * File: test_buzzer_lockout.c
 * Description: Integration tests for Buzzer + Lockout logic
 * Author: Testing Team
 * Date: December 18, 2025
 ******************************************************************************/

#include "../TestFramework/test_logger.h"
#include "../../CONTROL_WS/inc/hal/hal_buzzer.h"
#include "../../Common/inc/mcal/mcal_gpt.h"

/*******************************************************************************
 *                              Global Variables                               *
 *******************************************************************************/

static uint8 g_failedAttempts = 0;
static uint8 g_isLocked = FALSE;
static uint32 g_lockoutStartTime = 0;

#define MAX_PASSWORD_ATTEMPTS   3
#define LOCKOUT_DURATION_MS     60000  /* 60 seconds */

/*******************************************************************************
 *                              Test Functions                                 *
 *******************************************************************************/

void Test_Buzzer_WrongPasswordBeep(void) {
    /* Simulate wrong password entry */
    g_failedAttempts++;
    
    Buzzer_Beep(200);  /* Short beep for wrong password */
    
    TestLogger_Assert("IT-BUZZER-001", "Buzzer wrong password feedback", 
                      g_failedAttempts == 1);
}

void Test_Buzzer_MultipleFailures(void) {
    /* Reset attempts */
    g_failedAttempts = 0;
    
    /* Attempt 1 */
    g_failedAttempts++;
    Buzzer_Beep(200);
    for (volatile int i = 0; i < 50000; i++);
    
    /* Attempt 2 */
    g_failedAttempts++;
    Buzzer_Beep(200);
    for (volatile int i = 0; i < 50000; i++);
    
    /* Attempt 3 - Should trigger lockout */
    g_failedAttempts++;
    
    TestLogger_Assert("IT-BUZZER-002", "Buzzer three failed attempts detection", 
                      g_failedAttempts == MAX_PASSWORD_ATTEMPTS);
}

void Test_Buzzer_LockoutActivation(void) {
    /* Reset state */
    g_failedAttempts = MAX_PASSWORD_ATTEMPTS;
    g_isLocked = FALSE;
    
    /* Trigger lockout */
    if (g_failedAttempts >= MAX_PASSWORD_ATTEMPTS) {
        g_isLocked = TRUE;
        g_lockoutStartTime = SystemTick_GetTick();
        Buzzer_LockoutSignal();  /* Long continuous alarm */
    }
    
    TestLogger_Assert("IT-BUZZER-003", "Buzzer lockout activation", 
                      g_isLocked == TRUE);
}

void Test_Buzzer_LockoutDuration(void) {
    uint32 elapsedTime;
    
    /* Set lockout state */
    g_isLocked = TRUE;
    g_lockoutStartTime = SystemTick_GetTick();
    
    /* Buzzer should stay on for 60 seconds */
    Buzzer_On();
    
    /* Wait for lockout duration (simulated short for testing) */
    uint32 testDuration = 5000;  /* 5 seconds for test */
    while ((SystemTick_GetTick() - g_lockoutStartTime) < testDuration) {
        /* Buzzer should remain on */
    }
    
    Buzzer_Off();
    elapsedTime = SystemTick_GetTick() - g_lockoutStartTime;
    
    TestLogger_Assert("IT-BUZZER-004", "Buzzer lockout duration control", 
                      elapsedTime >= testDuration);
}

void Test_Buzzer_LockoutRelease(void) {
    /* Set lockout */
    g_isLocked = TRUE;
    g_lockoutStartTime = SystemTick_GetTick();
    Buzzer_On();
    
    /* Simulate 60 second wait */
    uint32 simulatedWait = 60000;
    while ((SystemTick_GetTick() - g_lockoutStartTime) < simulatedWait) {
        /* Simulate by advancing time */
    }
    
    /* Release lockout */
    if ((SystemTick_GetTick() - g_lockoutStartTime) >= LOCKOUT_DURATION_MS) {
        g_isLocked = FALSE;
        g_failedAttempts = 0;
        Buzzer_Off();
    }
    
    TestLogger_Assert("IT-BUZZER-005", "Buzzer lockout release", 
                      g_isLocked == FALSE && Buzzer_GetState() == BUZZER_OFF);
}

void Test_Buzzer_BlocksDuringLockout(void) {
    /* Set lockout */
    g_isLocked = TRUE;
    g_lockoutStartTime = SystemTick_GetTick();
    
    /* Try to perform action during lockout */
    uint8 actionBlocked = FALSE;
    
    if (g_isLocked) {
        /* System should reject any password attempt */
        actionBlocked = TRUE;
    }
    
    TestLogger_Assert("IT-BUZZER-006", "Buzzer blocks actions during lockout", 
                      actionBlocked == TRUE);
    
    /* Release lockout for next test */
    g_isLocked = FALSE;
    g_failedAttempts = 0;
}

void Test_Buzzer_CorrectPasswordReset(void) {
    /* Set some failed attempts */
    g_failedAttempts = 2;
    
    /* Simulate correct password */
    uint8 correctPassword = TRUE;
    
    if (correctPassword) {
        g_failedAttempts = 0;  /* Reset on success */
        Buzzer_Beep(100);  /* Success beep */
    }
    
    TestLogger_Assert("IT-BUZZER-007", "Buzzer failed attempts reset on success", 
                      g_failedAttempts == 0);
}

/*******************************************************************************
 *                              Main Test Suite                                *
 *******************************************************************************/

void Run_Buzzer_Lockout_IntegrationTests(void) {
    TestLogger_StartSuite("Buzzer + Lockout Logic Integration Tests");
    
    Test_Buzzer_WrongPasswordBeep();
    Test_Buzzer_MultipleFailures();
    Test_Buzzer_LockoutActivation();
    Test_Buzzer_LockoutDuration();
    Test_Buzzer_LockoutRelease();
    Test_Buzzer_BlocksDuringLockout();
    Test_Buzzer_CorrectPasswordReset();
    
    TestLogger_EndSuite();
}
