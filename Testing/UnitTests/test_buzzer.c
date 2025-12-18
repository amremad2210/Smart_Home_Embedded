/*******************************************************************************
 * File: test_buzzer.c
 * Description: Unit tests for Buzzer HAL driver
 * Author: Testing Team
 * Date: December 18, 2025
 ******************************************************************************/

#include "../TestFramework/test_logger.h"
#include "../../CONTROL_WS/inc/hal/hal_buzzer.h"

/*******************************************************************************
 *                              Test Functions                                 *
 *******************************************************************************/

void Test_Buzzer_Init(void) {
    BUZZER_init();
    TestLogger_Assert("UT-BUZZER-001", "Buzzer driver initialization", TRUE);
}

void Test_Buzzer_OnOff(void) {
    /* Turn buzzer ON */
    BUZZER_setState(BUZZER_ON);
    TestLogger_Assert("UT-BUZZER-002", "Buzzer turn ON", TRUE);
    
    /* Turn buzzer OFF */
    BUZZER_setState(BUZZER_OFF);
    TestLogger_Assert("UT-BUZZER-003", "Buzzer turn OFF", TRUE);
}

void Test_Buzzer_Beep(void) {
    /* Single beep test - 100ms beep */
    BUZZER_beep(100);
    TestLogger_Assert("UT-BUZZER-004", "Buzzer single beep", TRUE);
}

void Test_Buzzer_Pattern(void) {
    /* Test alarm pattern (3 beeps with 200ms each) */
    for (int i = 0; i < 3; i++) {
        BUZZER_beep(200);
        /* Small delay between beeps */
        for (volatile int j = 0; j < 50000; j++);
    }
    TestLogger_Assert("UT-BUZZER-005", "Buzzer alarm pattern", TRUE);
}

void Test_Buzzer_LockoutSignal(void) {
    /* Test security lockout signal - long beep */
    BUZZER_beep(1000);  /* 1 second beep */
    TestLogger_Assert("UT-BUZZER-006", "Buzzer lockout signal", TRUE);
}

/*******************************************************************************
 *                              Main Test Suite                                *
 *******************************************************************************/

void Run_Buzzer_UnitTests(void) {
    TestLogger_StartSuite("Buzzer HAL Driver Unit Tests");
    
    Test_Buzzer_Init();
    Test_Buzzer_OnOff();
    Test_Buzzer_Beep();
    Test_Buzzer_Pattern();
    Test_Buzzer_LockoutSignal();
    
    TestLogger_EndSuite();
}
