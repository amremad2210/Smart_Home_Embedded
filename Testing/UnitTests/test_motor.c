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
    TestLogger_Assert("UT-MOTOR-001", "Motor driver initialization", 
                      Motor_Init() == TRUE);
}

void Test_Motor_Direction(void) {
    /* Test clockwise direction (door opening) */
    Motor_SetDirection(MOTOR_CW);
    TestLogger_Assert("UT-MOTOR-002", "Motor clockwise direction", 
                      Motor_GetDirection() == MOTOR_CW);
    
    /* Test counter-clockwise direction (door closing) */
    Motor_SetDirection(MOTOR_CCW);
    TestLogger_Assert("UT-MOTOR-003", "Motor counter-clockwise direction", 
                      Motor_GetDirection() == MOTOR_CCW);
}

void Test_Motor_StartStop(void) {
    /* Start motor */
    Motor_Start();
    TestLogger_Assert("UT-MOTOR-004", "Motor start operation", 
                      Motor_IsRunning() == TRUE);
    
    /* Stop motor */
    Motor_Stop();
    TestLogger_Assert("UT-MOTOR-005", "Motor stop operation", 
                      Motor_IsRunning() == FALSE);
}

void Test_Motor_Speed(void) {
    Motor_Init();
    
    /* Set speed to 50% */
    Motor_SetSpeed(50);
    TestLogger_Assert("UT-MOTOR-006", "Motor speed control (50%)", 
                      Motor_GetSpeed() == 50);
    
    /* Set speed to 100% */
    Motor_SetSpeed(100);
    TestLogger_Assert("UT-MOTOR-007", "Motor speed control (100%)", 
                      Motor_GetSpeed() == 100);
    
    /* Test invalid speed */
    Motor_SetSpeed(150);
    TestLogger_Assert("UT-MOTOR-008", "Motor invalid speed rejection", 
                      Motor_GetSpeed() <= 100);
}

void Test_Motor_EmergencyStop(void) {
    Motor_Start();
    Motor_EmergencyStop();
    
    TestLogger_Assert("UT-MOTOR-009", "Motor emergency stop", 
                      Motor_IsRunning() == FALSE);
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
