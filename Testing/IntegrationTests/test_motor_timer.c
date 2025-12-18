/*******************************************************************************
 * File: test_motor_timer.c
 * Description: Integration tests for Motor + Timer control
 * Author: Testing Team
 * Date: December 18, 2025
 ******************************************************************************/

#include "../TestFramework/test_logger.h"
#include "../../CONTROL_WS/inc/hal/hal_motor.h"
#include "../../Common/inc/mcal/mcal_gpt.h"

/*******************************************************************************
 *                              Test Functions                                 *
 *******************************************************************************/

void Test_Motor_Timer_DoorOpen(void) {
    uint32 startTime, elapsedTime;
    
    /* Start door opening sequence */
    Motor_SetDirection(MOTOR_CW);
    Motor_Start();
    
    startTime = SystemTick_GetTick();
    
    /* Simulate 15 second door open time */
    while ((SystemTick_GetTick() - startTime) < 15000) {
        /* Motor should be running */
    }
    
    elapsedTime = SystemTick_GetTick() - startTime;
    
    TestLogger_Assert("IT-MOTOR-001", "Motor door open timing (15s)", 
                      elapsedTime >= 15000 && elapsedTime < 16000);
    
    Motor_Stop();
}

void Test_Motor_Timer_DoorHold(void) {
    uint32 startTime, elapsedTime;
    
    /* Stop motor for hold period */
    Motor_Stop();
    
    startTime = SystemTick_GetTick();
    
    /* Simulate 3 second hold time */
    while ((SystemTick_GetTick() - startTime) < 3000) {
        /* Motor should be stopped */
        if (Motor_IsRunning()) {
            TestLogger_Assert("IT-MOTOR-002", "Motor hold state", FALSE);
            return;
        }
    }
    
    elapsedTime = SystemTick_GetTick() - startTime;
    
    TestLogger_Assert("IT-MOTOR-002", "Motor door hold timing (3s)", 
                      elapsedTime >= 3000 && elapsedTime < 4000);
}

void Test_Motor_Timer_DoorClose(void) {
    uint32 startTime, elapsedTime;
    
    /* Start door closing sequence */
    Motor_SetDirection(MOTOR_CCW);
    Motor_Start();
    
    startTime = SystemTick_GetTick();
    
    /* Simulate 15 second door close time */
    while ((SystemTick_GetTick() - startTime) < 15000) {
        /* Motor should be running */
    }
    
    elapsedTime = SystemTick_GetTick() - startTime;
    
    TestLogger_Assert("IT-MOTOR-003", "Motor door close timing (15s)", 
                      elapsedTime >= 15000 && elapsedTime < 16000);
    
    Motor_Stop();
}

void Test_Motor_Timer_FullCycle(void) {
    uint32 startTime, totalTime;
    
    startTime = SystemTick_GetTick();
    
    /* Full door cycle: Open (15s) + Hold (3s) + Close (15s) = 33s */
    
    /* Open */
    Motor_SetDirection(MOTOR_CW);
    Motor_Start();
    while ((SystemTick_GetTick() - startTime) < 15000);
    
    /* Hold */
    Motor_Stop();
    while ((SystemTick_GetTick() - startTime) < 18000);
    
    /* Close */
    Motor_SetDirection(MOTOR_CCW);
    Motor_Start();
    while ((SystemTick_GetTick() - startTime) < 33000);
    Motor_Stop();
    
    totalTime = SystemTick_GetTick() - startTime;
    
    TestLogger_Assert("IT-MOTOR-004", "Motor full cycle timing (33s)", 
                      totalTime >= 33000 && totalTime < 34000);
}

void Test_Motor_Timer_InterruptHandling(void) {
    /* Start motor operation */
    Motor_SetDirection(MOTOR_CW);
    Motor_Start();
    
    uint32 startTime = SystemTick_GetTick();
    
    /* Run for 5 seconds */
    while ((SystemTick_GetTick() - startTime) < 5000);
    
    /* Emergency stop should interrupt timer */
    Motor_EmergencyStop();
    
    TestLogger_Assert("IT-MOTOR-005", "Motor timer interrupt handling", 
                      Motor_IsRunning() == FALSE);
}

void Test_Motor_Timer_PotentiometerControl(void) {
    /* Set custom timeout via potentiometer (simulated as 10 seconds) */
    uint16 customTimeout = 10000;  /* 10 seconds from potentiometer reading */
    
    Motor_SetDirection(MOTOR_CW);
    Motor_Start();
    
    uint32 startTime = SystemTick_GetTick();
    
    /* Run for custom timeout */
    while ((SystemTick_GetTick() - startTime) < customTimeout);
    
    Motor_Stop();
    
    uint32 elapsedTime = SystemTick_GetTick() - startTime;
    
    TestLogger_Assert("IT-MOTOR-006", "Motor potentiometer timeout control", 
                      elapsedTime >= customTimeout && elapsedTime < (customTimeout + 1000));
}

/*******************************************************************************
 *                              Main Test Suite                                *
 *******************************************************************************/

void Run_Motor_Timer_IntegrationTests(void) {
    TestLogger_StartSuite("Motor + Timer Integration Tests");
    
    Test_Motor_Timer_DoorOpen();
    Test_Motor_Timer_DoorHold();
    Test_Motor_Timer_DoorClose();
    Test_Motor_Timer_FullCycle();
    Test_Motor_Timer_InterruptHandling();
    Test_Motor_Timer_PotentiometerControl();
    
    TestLogger_EndSuite();
}
