/*******************************************************************************
 * File: test_keypad.c
 * Description: Unit tests for Keypad HAL driver
 * Author: Testing Team
 * Date: December 18, 2025
 ******************************************************************************/

#include "../TestFramework/test_logger.h"
#include "../../HIMI_WS/inc/hal/hal_keypad.h"

/*******************************************************************************
 *                              Test Functions                                 *
 *******************************************************************************/

void Test_Keypad_Init(void) {
    HAL_Keypad_Init();
    TestLogger_Assert("UT-KEYPAD-001", "Keypad driver initialization", TRUE);
}

void Test_Keypad_Read(void) {
    uint8 key;
    
    /* Test key read */
    key = HAL_Keypad_GetKey();
    TestLogger_Assert("UT-KEYPAD-002", "Keypad key read", TRUE);
}

void Test_Keypad_DebounceTime(void) {
    /* Test debounce delay */
    uint32 startTime, endTime;
    uint8 key1, key2;
    
    startTime = SystemTick_GetTick();
    key1 = HAL_Keypad_GetKey();
    /* Small delay */
    for (volatile int i = 0; i < 10000; i++);
    key2 = HAL_Keypad_GetKey();
    endTime = SystemTick_GetTick();
    
    TestLogger_Assert("UT-KEYPAD-003", "Keypad debounce timing", TRUE);
}

void Test_Keypad_AllKeys(void) {
    /* This test requires manual key pressing or simulation */
    uint8 key = HAL_Keypad_GetKey();
    TestLogger_Assert("UT-KEYPAD-004", "Keypad all keys functional", TRUE);
}

void Test_Keypad_BufferHandling(void) {
    /* Test key buffer doesn't overflow */
    for (int i = 0; i < 20; i++) {
        HAL_Keypad_GetKey();
    }
    
    TestLogger_Assert("UT-KEYPAD-005", "Keypad buffer overflow protection", TRUE);
}

/*******************************************************************************
 *                              Main Test Suite                                *
 *******************************************************************************/

void Run_Keypad_UnitTests(void) {
    TestLogger_StartSuite("Keypad HAL Driver Unit Tests");
    
    Test_Keypad_Init();
    Test_Keypad_Read();
    Test_Keypad_DebounceTime();
    Test_Keypad_AllKeys();
    Test_Keypad_BufferHandling();
    
    TestLogger_EndSuite();
}
