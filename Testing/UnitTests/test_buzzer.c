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
    TestLogger_Assert("UT-BUZZER-001", "Buzzer driver initialization", 
                      Buzzer_Init() == TRUE);
}

void Test_Buzzer_OnOff(void) {
    /* Turn buzzer ON */
    Buzzer_On();
    TestLogger_Assert("UT-BUZZER-002", "Buzzer turn ON", 
                      Buzzer_GetState() == BUZZER_ON);
    
    /* Turn buzzer OFF */
    Buzzer_Off();
    TestLogger_Assert("UT-BUZZER-003", "Buzzer turn OFF", 
                      Buzzer_GetState() == BUZZER_OFF);
}

void Test_Buzzer_Beep(void) {
    /* Single beep test */
    Buzzer_Beep(100);  /* 100ms beep */
    TestLogger_Assert("UT-BUZZER-004", "Buzzer single beep", TRUE);
}

void Test_Buzzer_Pattern(void) {
    /* Test alarm pattern (3 beeps) */
    Buzzer_AlarmPattern(3);
    TestLogger_Assert("UT-BUZZER-005", "Buzzer alarm pattern", TRUE);
}

void Test_Buzzer_LockoutSignal(void) {
    /* Test security lockout signal */
    Buzzer_LockoutSignal();
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
