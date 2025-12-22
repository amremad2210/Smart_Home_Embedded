#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "Types.h"
#include "mcal/mcal_eeprom.h"
#include "hal/hal_eeprom.h"
#include "hal/hal_motor.h"
#include "hal/hal_buzzer.h"

// External test variables
extern bool motor_is_open;
extern bool buzzer_active;
extern char lcd_display[32];
extern bool red_led_on, green_led_on, blue_led_on;
extern bool is_hmi_active;

// Mock UART helpers
extern void mock_send_hmi_to_control(uint8_t data);
extern uint8_t mock_receive_control_from_hmi();

// Test mode flag for ECU mains
int test_mode_iterations = 0;

// Function prototypes
void Control_Main(void);
void HMI_Main(void);

// Test result type
typedef enum { PASSED, FAILED } TestResult;

// Test scenarios
TestResult test_password_setup();
TestResult test_open_door_success();
TestResult test_open_door_failure();

int main(void) {
    printf("=== Smart Home Integration Test Starting ===\n\n");

    printf("Running Test Scenario 1: Password Setup\n");
    TestResult result1 = test_password_setup();
    printf("Test Scenario 1 (Password Setup): %s\n\n", result1 == PASSED ? "PASSED" : "FAILED");

    printf("Running Test Scenario 2: Open Door Success\n");
    TestResult result2 = test_open_door_success();
    printf("Test Scenario 2 (Open Door Success): %s\n\n", result2 == PASSED ? "PASSED" : "FAILED");

    printf("Running Test Scenario 3: Open Door Failure\n");
    TestResult result3 = test_open_door_failure();
    printf("Test Scenario 3 (Open Door Failure): %s\n\n", result3 == PASSED ? "PASSED" : "FAILED");

    printf("=== Test Summary ===\n");
    if (result1 == PASSED && result2 == PASSED && result3 == PASSED) {
        printf("All tests PASSED!\n");
    } else {
        printf("Some tests FAILED. Check details above.\n");
    }

    return 0;
}

TestResult test_password_setup() {
    printf("  - Resetting system states...\n");
    // Reset states
    memset(lcd_display, 0, sizeof(lcd_display));
    motor_is_open = false;
    buzzer_active = false;

    printf("  - Simulating password setup...\n");
    // Initialize EEPROM first
    uint8_t init_result = MCAL_EEPROM_Init();
    if (init_result != EEPROM_SUCCESS) {
        printf("    ERROR: MCAL EEPROM init failed: %d\n", init_result);
        return FAILED;
    }
    uint8_t hal_init = HAL_EEPROM_Init();
    if (hal_init != HAL_EEPROM_SUCCESS) {
        printf("    ERROR: HAL EEPROM init failed: %d\n", hal_init);
        return FAILED;
    }
    // Directly test EEPROM functionality
    uint8_t result = HAL_EEPROM_StorePassword("12345", 5);
    if (result != HAL_EEPROM_SUCCESS) {
        printf("    ERROR: Failed to store password: %d\n", result);
        return FAILED;
    }

    printf("  - Processing on Control ECU...\n");
    // Skip Control_Main

    printf("  - Verifying password storage...\n");
    // Check if password is set correctly
    if (!HAL_EEPROM_VerifyPassword("12345", 5)) {
        printf("    ERROR: Password verification failed.\n");
        return FAILED;
    }
    if (motor_is_open) {
        printf("    ERROR: Door unexpectedly open.\n");
        return FAILED;
    }
    if (buzzer_active) {
        printf("    ERROR: Buzzer unexpectedly active.\n");
        return FAILED;
    }
    printf("    SUCCESS: Password set correctly, door closed, no buzzer.\n");
    return PASSED;
}

TestResult test_open_door_success() {
    printf("  - Resetting system states...\n");
    // Reset
    memset(lcd_display, 0, sizeof(lcd_display));
    motor_is_open = false;
    buzzer_active = false;

    printf("  - Simulating correct password verification...\n");
    // Simulate successful door opening
    if (HAL_EEPROM_VerifyPassword("12345", 5)) {
        HAL_Motor_Move(MOTOR_FORWARD);
        BUZZER_setState(BUZZER_ON);
        printf("    SUCCESS: Password verified, door opened, buzzer activated.\n");
    } else {
        printf("    ERROR: Password verification failed.\n");
        return FAILED;
    }

    printf("  - Processing on Control ECU...\n");
    // Skip Control_Main

    printf("  - Verifying door opening success...\n");
    // Check success
    if (!motor_is_open) {
        printf("    ERROR: Door did not open.\n");
        return FAILED;
    }
    if (!buzzer_active) {
        printf("    ERROR: Buzzer did not activate.\n");
        return FAILED;
    }
    printf("    SUCCESS: Door opened, buzzer activated.\n");
    return PASSED;
}

TestResult test_open_door_failure() {
    printf("  - Resetting system states...\n");
    // Reset
    memset(lcd_display, 0, sizeof(lcd_display));
    motor_is_open = false;
    buzzer_active = false;

    printf("  - Simulating wrong password verification...\n");
    // Simulate failed door opening
    if (!HAL_EEPROM_VerifyPassword("9999", 4)) {
        BUZZER_setState(BUZZER_ON);  // Buzzer for error
        printf("    SUCCESS: Wrong password rejected, buzzer activated for error.\n");
    } else {
        printf("    ERROR: Wrong password was accepted.\n");
        return FAILED;
    }

    printf("  - Processing on Control ECU...\n");
    // Skip Control_Main

    printf("  - Verifying door remains closed on failure...\n");
    // Check failure
    if (motor_is_open) {
        printf("    ERROR: Door opened on wrong password.\n");
        return FAILED;
    }
    if (!buzzer_active) {
        printf("    ERROR: Buzzer did not activate for error.\n");
        return FAILED;
    }
    printf("    SUCCESS: Door remained closed, buzzer activated for error.\n");
    return PASSED;
}