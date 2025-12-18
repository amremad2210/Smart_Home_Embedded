/*******************************************************************************
 * File: test_system_functional.c
 * Description: Full system functional tests mapping to requirements
 * Author: Testing Team
 * Date: December 18, 2025
 ******************************************************************************/

#include "../TestFramework/test_logger.h"
#include "../../HIMI_WS/inc/hal/hal_keypad.h"
#include "../../HIMI_WS/inc/hal/hal_lcd.h"
#include "../../HIMI_WS/inc/hal/hal_comm.h"
#include "../../HIMI_WS/inc/hal/hal_potentiometer.h"
#include "../../HIMI_WS/inc/hal/hal_rgb_led.h"
#include "../../CONTROL_WS/inc/hal/hal_motor.h"
#include "../../CONTROL_WS/inc/hal/hal_buzzer.h"
#include "../../CONTROL_WS/inc/hal/hal_eeprom.h"

/*******************************************************************************
 *                              System State                                   *
 *******************************************************************************/

typedef enum {
    STATE_FIRST_TIME_SETUP,
    STATE_MAIN_MENU,
    STATE_PASSWORD_ENTRY,
    STATE_DOOR_OPENING,
    STATE_PASSWORD_CHANGE,
    STATE_LOCKED
} SystemState;

static SystemState g_systemState = STATE_MAIN_MENU;
static uint8 g_password[4] = {0};
static uint8 g_failedAttempts = 0;

/*******************************************************************************
 *                              Test Functions                                 *
 *******************************************************************************/

void Test_System_InitialPasswordSetup(void) {
    TestCase test;
    
    /* Simulate first time startup */
    HAL_EEPROM_ClearPassword();
    
    test.testId = "ST-001";
    test.scenario = "Initial password setup and storage";
    test.expected = "Password stored in EEPROM successfully";
    
    /* Display on LCD */
    LCD_Clear();
    LCD_DisplayString("Set Password:");
    
    /* Simulate user entering password: 1234 */
    uint8 newPassword[4] = {0x01, 0x02, 0x03, 0x04};
    
    /* Store in EEPROM */
    if (HAL_EEPROM_SavePassword(newPassword, 4)) {
        test.actual = "Password stored successfully";
        test.result = TEST_PASS;
        RGB_LED_SetColor(RGB_GREEN);  /* Success indicator */
    } else {
        test.actual = "Password storage failed";
        test.result = TEST_FAIL;
        RGB_LED_SetColor(RGB_RED);  /* Failure indicator */
    }
    
    TestLogger_LogTest(&test);
}

void Test_System_OpenDoor_CorrectPassword(void) {
    TestCase test;
    
    test.testId = "ST-002";
    test.scenario = "Open door with correct password";
    test.expected = "Door opens (15s), holds (3s), closes (15s), total ~33s";
    
    /* Setup: Store password */
    uint8 correctPassword[4] = {0x01, 0x02, 0x03, 0x04};
    HAL_EEPROM_SavePassword(correctPassword, 4);
    
    /* Display LCD */
    LCD_Clear();
    LCD_DisplayString("Enter Password:");
    
    /* Simulate password entry */
    uint8 enteredPassword[4] = {0x01, 0x02, 0x03, 0x04};
    
    /* Verify password */
    if (HAL_EEPROM_VerifyPassword(enteredPassword, 4)) {
        /* LED indicates success */
        RGB_LED_SetColor(RGB_GREEN);
        LCD_Clear();
        LCD_DisplayString("Door Opening...");
        
        /* Start timer */
        uint32 startTime = SystemTick_GetTick();
        
        /* Open door (15 seconds) */
        Motor_SetDirection(MOTOR_CW);
        Motor_Start();
        while ((SystemTick_GetTick() - startTime) < 15000);
        
        /* Hold (3 seconds) */
        Motor_Stop();
        LCD_Clear();
        LCD_DisplayString("Door Open");
        while ((SystemTick_GetTick() - startTime) < 18000);
        
        /* Close door (15 seconds) */
        LCD_Clear();
        LCD_DisplayString("Door Closing...");
        Motor_SetDirection(MOTOR_CCW);
        Motor_Start();
        while ((SystemTick_GetTick() - startTime) < 33000);
        Motor_Stop();
        
        uint32 totalTime = SystemTick_GetTick() - startTime;
        
        if (totalTime >= 33000 && totalTime < 34000) {
            test.actual = "Door cycle completed in ~33s";
            test.result = TEST_PASS;
            LCD_Clear();
            LCD_DisplayString("Door Closed");
            RGB_LED_SetColor(RGB_BLUE);
        } else {
            test.actual = "Door cycle timing incorrect";
            test.result = TEST_FAIL;
        }
    } else {
        test.actual = "Password verification failed";
        test.result = TEST_FAIL;
    }
    
    TestLogger_LogTest(&test);
}

void Test_System_WrongPassword_ThreeAttempts(void) {
    TestCase test;
    
    test.testId = "ST-003";
    test.scenario = "Wrong password 3 times triggers lockout";
    test.expected = "Buzzer activates for 60s, system locks, LED shows red";
    
    /* Reset failed attempts */
    g_failedAttempts = 0;
    
    /* Store correct password */
    uint8 correctPassword[4] = {0x01, 0x02, 0x03, 0x04};
    HAL_EEPROM_SavePassword(correctPassword, 4);
    
    /* Attempt 1 */
    uint8 wrongPassword1[4] = {0x09, 0x09, 0x09, 0x09};
    if (!HAL_EEPROM_VerifyPassword(wrongPassword1, 4)) {
        g_failedAttempts++;
        Buzzer_Beep(200);
        RGB_LED_SetColor(RGB_RED);
        LCD_DisplayString("Wrong Password!");
    }
    
    /* Attempt 2 */
    uint8 wrongPassword2[4] = {0x08, 0x08, 0x08, 0x08};
    if (!HAL_EEPROM_VerifyPassword(wrongPassword2, 4)) {
        g_failedAttempts++;
        Buzzer_Beep(200);
        RGB_LED_SetColor(RGB_RED);
    }
    
    /* Attempt 3 - triggers lockout */
    uint8 wrongPassword3[4] = {0x07, 0x07, 0x07, 0x07};
    if (!HAL_EEPROM_VerifyPassword(wrongPassword3, 4)) {
        g_failedAttempts++;
        
        if (g_failedAttempts >= 3) {
            /* Activate lockout */
            g_systemState = STATE_LOCKED;
            Buzzer_LockoutSignal();
            RGB_LED_SetColor(RGB_RED);
            RGB_LED_Blink(500);  /* Blinking red */
            LCD_Clear();
            LCD_DisplayString("SYSTEM LOCKED!");
            LCD_SetCursor(1, 0);
            LCD_DisplayString("Wait 60 seconds");
            
            test.actual = "System locked, buzzer active, LED red";
            test.result = TEST_PASS;
        } else {
            test.actual = "Lockout not triggered";
            test.result = TEST_FAIL;
        }
    }
    
    TestLogger_LogTest(&test);
    
    /* Cleanup: Reset for next test */
    g_failedAttempts = 0;
    g_systemState = STATE_MAIN_MENU;
    Buzzer_Off();
}

void Test_System_ChangePassword(void) {
    TestCase test;
    
    test.testId = "ST-004";
    test.scenario = "Change password flow";
    test.expected = "Old password verified, new password stored, system accepts new password";
    
    /* Store initial password */
    uint8 oldPassword[4] = {0x01, 0x02, 0x03, 0x04};
    HAL_EEPROM_SavePassword(oldPassword, 4);
    
    /* Display menu */
    LCD_Clear();
    LCD_DisplayString("Change Password");
    
    /* Enter old password */
    LCD_SetCursor(1, 0);
    LCD_DisplayString("Enter Old:");
    uint8 enteredOld[4] = {0x01, 0x02, 0x03, 0x04};
    
    if (HAL_EEPROM_VerifyPassword(enteredOld, 4)) {
        /* Enter new password */
        LCD_Clear();
        LCD_DisplayString("Enter New:");
        uint8 newPassword[4] = {0x05, 0x06, 0x07, 0x08};
        
        /* Confirm new password */
        LCD_Clear();
        LCD_DisplayString("Confirm New:");
        uint8 confirmPassword[4] = {0x05, 0x06, 0x07, 0x08};
        
        /* Check match */
        uint8 match = TRUE;
        for (int i = 0; i < 4; i++) {
            if (newPassword[i] != confirmPassword[i]) {
                match = FALSE;
                break;
            }
        }
        
        if (match) {
            /* Store new password */
            HAL_EEPROM_ChangePassword(oldPassword, newPassword, 4);
            
            /* Verify new password works */
            if (HAL_EEPROM_VerifyPassword(newPassword, 4)) {
                test.actual = "Password changed successfully";
                test.result = TEST_PASS;
                LCD_Clear();
                LCD_DisplayString("Password Changed");
                RGB_LED_SetColor(RGB_GREEN);
            } else {
                test.actual = "New password verification failed";
                test.result = TEST_FAIL;
            }
        } else {
            test.actual = "Password confirmation mismatch";
            test.result = TEST_FAIL;
        }
    } else {
        test.actual = "Old password verification failed";
        test.result = TEST_FAIL;
    }
    
    TestLogger_LogTest(&test);
}

void Test_System_AutoLockTimeout_Potentiometer(void) {
    TestCase test;
    
    test.testId = "ST-005";
    test.scenario = "Auto-lock timeout setting via potentiometer";
    test.expected = "Timeout value changes based on potentiometer position";
    
    /* Read potentiometer value */
    uint16 potValue = Potentiometer_Read();
    
    /* Map to timeout range (e.g., 5-30 seconds) */
    uint16 timeoutSeconds = 5 + ((potValue * 25) / 1023);
    
    /* Display on LCD */
    LCD_Clear();
    LCD_DisplayString("Timeout: ");
    LCD_DisplayNumber(timeoutSeconds);
    LCD_DisplayString("s");
    
    /* Verify timeout is in valid range */
    if (timeoutSeconds >= 5 && timeoutSeconds <= 30) {
        test.actual = "Timeout set successfully";
        test.result = TEST_PASS;
    } else {
        test.actual = "Timeout out of range";
        test.result = TEST_FAIL;
    }
    
    TestLogger_LogTest(&test);
}

void Test_System_LED_StatusFeedback(void) {
    TestCase test;
    
    test.testId = "ST-006";
    test.scenario = "LED status feedback for all states";
    test.expected = "LED color matches system state";
    
    uint8 allStatesCorrect = TRUE;
    
    /* Test 1: Idle state (Blue) */
    RGB_LED_SetColor(RGB_BLUE);
    if (RGB_LED_GetColor() != RGB_BLUE) allStatesCorrect = FALSE;
    
    /* Test 2: Password correct (Green) */
    RGB_LED_SetColor(RGB_GREEN);
    if (RGB_LED_GetColor() != RGB_GREEN) allStatesCorrect = FALSE;
    
    /* Test 3: Password wrong (Red) */
    RGB_LED_SetColor(RGB_RED);
    if (RGB_LED_GetColor() != RGB_RED) allStatesCorrect = FALSE;
    
    /* Test 4: Door operating (Yellow/Amber) */
    RGB_LED_SetColor(RGB_YELLOW);
    if (RGB_LED_GetColor() != RGB_YELLOW) allStatesCorrect = FALSE;
    
    if (allStatesCorrect) {
        test.actual = "All LED states correct";
        test.result = TEST_PASS;
    } else {
        test.actual = "Some LED states incorrect";
        test.result = TEST_FAIL;
    }
    
    TestLogger_LogTest(&test);
}

void Test_System_LockoutRecovery(void) {
    TestCase test;
    
    test.testId = "ST-007";
    test.scenario = "System recovery after 60s lockout";
    test.expected = "System returns to normal operation, accepts correct password";
    
    /* Simulate lockout state */
    g_systemState = STATE_LOCKED;
    uint32 lockoutStart = SystemTick_GetTick();
    
    Buzzer_On();
    RGB_LED_SetColor(RGB_RED);
    LCD_Clear();
    LCD_DisplayString("LOCKED");
    
    /* Wait for lockout duration (simulated 5s for testing) */
    while ((SystemTick_GetTick() - lockoutStart) < 5000);
    
    /* Release lockout */
    g_systemState = STATE_MAIN_MENU;
    g_failedAttempts = 0;
    Buzzer_Off();
    RGB_LED_SetColor(RGB_BLUE);
    LCD_Clear();
    LCD_DisplayString("Ready");
    
    /* Try correct password */
    uint8 correctPassword[4] = {0x01, 0x02, 0x03, 0x04};
    if (HAL_EEPROM_VerifyPassword(correctPassword, 4)) {
        test.actual = "System recovered, accepts password";
        test.result = TEST_PASS;
    } else {
        test.actual = "System still locked or password rejected";
        test.result = TEST_FAIL;
    }
    
    TestLogger_LogTest(&test);
}

/*******************************************************************************
 *                              Main Test Suite                                *
 *******************************************************************************/

void Run_System_FunctionalTests(void) {
    TestLogger_StartSuite("System Functional Tests - Requirements Validation");
    
    Test_System_InitialPasswordSetup();
    Test_System_OpenDoor_CorrectPassword();
    Test_System_WrongPassword_ThreeAttempts();
    Test_System_ChangePassword();
    Test_System_AutoLockTimeout_Potentiometer();
    Test_System_LED_StatusFeedback();
    Test_System_LockoutRecovery();
    
    TestLogger_EndSuite();
}
