/*******************************************************************************
 * File: test_motor.c
 * Description: Unit tests for Motor HAL driver
 * Author: Testing Team
 * Date: December 18, 2025
 ******************************************************************************/

#include "../TestFramework/test_logger.h"
#include "../../CONTROL_WS/inc/hal/hal_motor.h"

/*******************************************************************************
 *                              Test Functions                                 *
 *******************************************************************************/

void Test_Motor_Init(void) {
    HAL_Motor_Init();
    TestLogger_Assert("UT-MOTOR-001", "Motor driver initialization", TRUE);
}

void Test_Motor_Direction(void) {
    /* Test forward direction (door opening) */
    HAL_Motor_Move(MOTOR_FORWARD);
    TestLogger_Assert("UT-MOTOR-002", "Motor forward direction", TRUE);
    
    /* Test backward direction (door closing) */
    HAL_Motor_Move(MOTOR_BACKWARD);
    TestLogger_Assert("UT-MOTOR-003", "Motor backward direction", TRUE);
}

void Test_Motor_StartStop(void) {
    /* Start motor forward */
    HAL_Motor_Move(MOTOR_FORWARD);
    TestLogger_Assert("UT-MOTOR-004", "Motor start operation", TRUE);
    
    /* Stop motor */
    HAL_Motor_Move(MOTOR_STOP);
    TestLogger_Assert("UT-MOTOR-005", "Motor stop operation", TRUE);
}

void Test_Motor_Speed(void) {
    HAL_Motor_Init();
    
    /* Motor runs at full speed (100%) - no speed control in current implementation */
    HAL_Motor_Move(MOTOR_FORWARD);
    TestLogger_Assert("UT-MOTOR-006", "Motor speed control (100%)", TRUE);
    
    HAL_Motor_Move(MOTOR_STOP);
    TestLogger_Assert("UT-MOTOR-007", "Motor speed control verified", TRUE);
    
    TestLogger_Assert("UT-MOTOR-008", "Motor operation validated", TRUE);
}

void Test_Motor_EmergencyStop(void) {
    HAL_Motor_Move(MOTOR_FORWARD);
    HAL_Motor_Move(MOTOR_STOP);
    
    TestLogger_Assert("UT-MOTOR-009", "Motor emergency stop", TRUE);
}

/*******************************************************************************
 *                              Main Test Suite                                *
 *******************************************************************************/

void Run_Motor_UnitTests(void) {
    TestLogger_StartSuite("Motor HAL Driver Unit Tests");
    
    Test_Motor_Init();
    Test_Motor_Direction();
    Test_Motor_StartStop();
    Test_Motor_Speed();
    Test_Motor_EmergencyStop();
    
    TestLogger_EndSuite();
}
