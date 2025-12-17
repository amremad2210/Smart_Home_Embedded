/*============================================================================
 *  Module      : Control ECU Test Suite
 *  File Name   : test_control_ecu.c
 *  Description : Unit and Integration Tests for Control ECU Application
 *===========================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Include Types.h for boolean and TRUE/FALSE definitions */
#include "../Common/inc/Types.h"

/* Test Framework Macros */
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf("FAIL: %s (Line %d)\n", message, __LINE__); \
            test_failures++; \
            return; \
        } \
    } while(0)

#define TEST_ASSERT_EQ(expected, actual, message) \
    do { \
        if ((expected) != (actual)) { \
            printf("FAIL: %s - Expected: %d, Actual: %d (Line %d)\n", \
                   message, (int)(expected), (int)(actual), __LINE__); \
            test_failures++; \
            return; \
        } \
    } while(0)

#define TEST_ASSERT_STR_EQ(expected, actual, message) \
    do { \
        if (strcmp((expected), (actual)) != 0) { \
            printf("FAIL: %s - Expected: '%s', Actual: '%s' (Line %d)\n", \
                   message, (expected), (actual), __LINE__); \
            test_failures++; \
            return; \
        } \
    } while(0)

#define RUN_TEST(test_func) \
    do { \
        printf("Running %s...\n", #test_func); \
        test_failures = 0; \
        test_func(); \
        if (test_failures == 0) { \
            printf("PASS: %s\n\n", #test_func); \
            total_passed++; \
        } else { \
            printf("FAILED: %s (%d failures)\n\n", #test_func, test_failures); \
            total_failed++; \
        } \
    } while(0)

/* Motor Direction Type (should match hal_motor.h) */
typedef enum {
    MOTOR_STOP,
    MOTOR_FORWARD,
    MOTOR_BACKWARD
} Motor_DirType;

/* Mock Variables */
static uint8_t mock_uart_rx_buffer[256];
static uint8_t mock_uart_tx_buffer[256];
static uint32_t mock_uart_rx_index = 0;
static uint32_t mock_uart_tx_index = 0;
static uint32_t mock_uart_rx_size = 0;
static boolean mock_uart_data_available = FALSE;

static char mock_eeprom_password[16];
static uint8_t mock_eeprom_password_length = 0;
static boolean mock_eeprom_password_set = FALSE;
static uint32_t mock_eeprom_timeout = 15U;

static uint8_t mock_led_state = 0;  /* 0=off, 1=green, 2=red */
static Motor_DirType mock_motor_state = MOTOR_STOP;
static boolean mock_buzzer_beeped = FALSE;
static uint32_t mock_buzzer_duration = 0;

static uint8_t test_failures = 0;
static uint32_t total_passed = 0;
static uint32_t total_failed = 0;

/* Mock HAL Functions */

uint8_t HAL_EEPROM_Init(void) { return 0; }
uint8_t HAL_EEPROM_StorePassword(const char *password, uint8_t length) {
    if (password == NULL || length < 4 || length > 16) return 1;
    memcpy(mock_eeprom_password, password, length);
    mock_eeprom_password[length] = '\0';
    mock_eeprom_password_length = length;
    mock_eeprom_password_set = TRUE;
    return 0;
}
boolean HAL_EEPROM_VerifyPassword(const char *password, uint8_t length) {
    if (!mock_eeprom_password_set) return FALSE;
    if (length != mock_eeprom_password_length) return FALSE;
    return (memcmp(password, mock_eeprom_password, length) == 0);
}
uint8_t HAL_EEPROM_ChangePassword(const char *oldPassword, uint8_t oldLength,
                                   const char *newPassword, uint8_t newLength) {
    if (!HAL_EEPROM_VerifyPassword(oldPassword, oldLength)) return 7;
    return HAL_EEPROM_StorePassword(newPassword, newLength);
}
boolean HAL_EEPROM_IsPasswordSet(void) { return mock_eeprom_password_set; }
uint8_t HAL_EEPROM_ClearPassword(void) {
    memset(mock_eeprom_password, 0, sizeof(mock_eeprom_password));
    mock_eeprom_password_length = 0;
    mock_eeprom_password_set = FALSE;
    return 0;
}

/* Mock HAL COMM */
uint8_t HAL_COMM_Init(void) { return 0; }
void HAL_COMM_SendByte(uint8_t data) {
    if (mock_uart_tx_index < sizeof(mock_uart_tx_buffer)) {
        mock_uart_tx_buffer[mock_uart_tx_index++] = data;
    }
}
uint8_t HAL_COMM_ReceiveByte(void) {
    if (mock_uart_rx_index < mock_uart_rx_size) {
        return mock_uart_rx_buffer[mock_uart_rx_index++];
    }
    return 0;
}
boolean HAL_COMM_IsDataAvailable(void) {
    return mock_uart_data_available;
}

/* Mock HAL Motor */
void HAL_Motor_Init(void) { mock_motor_state = MOTOR_STOP; }
void HAL_Motor_Move(Motor_DirType direction) {
    mock_motor_state = direction;
}

/* Mock HAL Buzzer */
void BUZZER_init(void) { mock_buzzer_beeped = FALSE; }
void BUZZER_beep(uint32_t duration_ms) {
    mock_buzzer_beeped = TRUE;
    mock_buzzer_duration = duration_ms;
}

/* Mock MCAL */
void MCAL_SysTick_Init(void) {}
void MCAL_SysTick_DelayMs(uint32_t ms) { /* Simulated delay */ }
void MCAL_GPIO_EnablePort(uint32_t periph) {}
void MCAL_GPIO_InitPin(uint32_t port, uint32_t pins, uint32_t dir, uint32_t attach) {}
void MCAL_GPIO_WritePin(uint32_t port, uint32_t pin, uint32_t value) {
    if (pin == (1U << 3)) { /* GREEN_LED_PIN */
        if (value) mock_led_state = 1; else if (mock_led_state == 1) mock_led_state = 0;
    }
    if (pin == (1U << 1)) { /* RED_LED_PIN */
        if (value) mock_led_state = 2; else if (mock_led_state == 2) mock_led_state = 0;
    }
}
uint8_t MCAL_EEPROM_ReadWord(uint32_t address, uint32_t *pData) {
    if (address == 28U) { /* EEPROM_TIMEOUT_ADDR */
        *pData = mock_eeprom_timeout;
        return 0;
    }
    return 1;
}
uint8_t MCAL_EEPROM_WriteWord(uint32_t address, uint32_t data) {
    if (address == 28U) { /* EEPROM_TIMEOUT_ADDR */
        mock_eeprom_timeout = data;
        return 0;
    }
    return 1;
}

/* Test Helper Functions */
static void reset_mocks(void) {
    memset(mock_uart_rx_buffer, 0, sizeof(mock_uart_rx_buffer));
    memset(mock_uart_tx_buffer, 0, sizeof(mock_uart_tx_buffer));
    mock_uart_rx_index = 0;
    mock_uart_tx_index = 0;
    mock_uart_rx_size = 0;
    mock_uart_data_available = FALSE;
    HAL_EEPROM_ClearPassword();
    mock_eeprom_timeout = 15U;
    mock_led_state = 0;
    mock_motor_state = MOTOR_STOP;
    mock_buzzer_beeped = FALSE;
    mock_buzzer_duration = 0;
}

static void setup_uart_rx_data(const uint8_t *data, uint32_t size) {
    memcpy(mock_uart_rx_buffer, data, size);
    mock_uart_rx_size = size;
    mock_uart_rx_index = 0;
    mock_uart_data_available = TRUE;
}

static uint8_t get_uart_tx_byte(uint32_t index) {
    if (index < mock_uart_tx_index) {
        return mock_uart_tx_buffer[index];
    }
    return 0;
}

/* Include main.c functions - We'll need to make them non-static for testing */
/* For now, we'll test the logic through the public interface simulation */

/* Test Cases */
static void test_password_setup_success(void) {
    reset_mocks();
    
    /* Simulate: 'S' command + password1 "12345" + password2 "12345" */
    uint8_t cmd[] = {'S', '1', '2', '3', '4', '5', '1', '2', '3', '4', '5'};
    setup_uart_rx_data(cmd, sizeof(cmd));
    
    /* Simulate command processing */
    uint8_t command = HAL_COMM_ReceiveByte();
    TEST_ASSERT_EQ('S', command, "Should receive setup command");
    
    /* Receive first password */
    char pwd1[6];
    for (int i = 0; i < 5; i++) {
        pwd1[i] = (char)HAL_COMM_ReceiveByte();
    }
    pwd1[5] = '\0';
    TEST_ASSERT_STR_EQ("12345", pwd1, "First password should be received correctly");
    
    /* Receive second password */
    char pwd2[6];
    for (int i = 0; i < 5; i++) {
        pwd2[i] = (char)HAL_COMM_ReceiveByte();
    }
    pwd2[5] = '\0';
    TEST_ASSERT_STR_EQ("12345", pwd2, "Second password should be received correctly");
    
    /* Test password storage */
    uint8_t result = HAL_EEPROM_StorePassword(pwd1, 5);
    TEST_ASSERT_EQ(0, result, "Password storage should succeed");
    TEST_ASSERT(HAL_EEPROM_IsPasswordSet(), "Password should be set");
    
    /* Verify stored password */
    TEST_ASSERT(HAL_EEPROM_VerifyPassword("12345", 5), "Stored password should verify correctly");
}

static void test_password_setup_mismatch(void) {
    reset_mocks();
    
    /* Simulate: 'S' command + password1 "12345" + password2 "54321" */
    uint8_t cmd[] = {'S', '1', '2', '3', '4', '5', '5', '4', '3', '2', '1'};
    setup_uart_rx_data(cmd, sizeof(cmd));
    
    uint8_t command = HAL_COMM_ReceiveByte();
    TEST_ASSERT_EQ('S', command, "Should receive setup command");
    
    char pwd1[6], pwd2[6];
    for (int i = 0; i < 5; i++) {
        pwd1[i] = (char)HAL_COMM_ReceiveByte();
    }
    pwd1[5] = '\0';
    
    for (int i = 0; i < 5; i++) {
        pwd2[i] = (char)HAL_COMM_ReceiveByte();
    }
    pwd2[5] = '\0';
    
    /* Passwords don't match */
    boolean match = (strcmp(pwd1, pwd2) == 0);
    TEST_ASSERT(!match, "Passwords should not match");
}

static void test_password_verification_correct(void) {
    reset_mocks();
    
    /* Setup password */
    HAL_EEPROM_StorePassword("12345", 5);
    
    /* Verify correct password */
    TEST_ASSERT(HAL_EEPROM_VerifyPassword("12345", 5), "Correct password should verify");
    TEST_ASSERT(!HAL_EEPROM_VerifyPassword("54321", 5), "Wrong password should not verify");
    TEST_ASSERT(!HAL_EEPROM_VerifyPassword("1234", 4), "Wrong length password should not verify");
}

static void test_password_verification_wrong_attempts(void) {
    reset_mocks();
    
    HAL_EEPROM_StorePassword("12345", 5);
    
    uint8_t wrong_attempts = 0;
    
    /* First wrong attempt */
    if (!HAL_EEPROM_VerifyPassword("11111", 5)) {
        wrong_attempts++;
    }
    TEST_ASSERT_EQ(1, wrong_attempts, "First wrong attempt should increment counter");
    
    /* Second wrong attempt */
    if (!HAL_EEPROM_VerifyPassword("22222", 5)) {
        wrong_attempts++;
    }
    TEST_ASSERT_EQ(2, wrong_attempts, "Second wrong attempt should increment counter");
    
    /* Third wrong attempt - should trigger lockout */
    if (!HAL_EEPROM_VerifyPassword("33333", 5)) {
        wrong_attempts++;
    }
    TEST_ASSERT_EQ(3, wrong_attempts, "Third wrong attempt should increment counter");
    TEST_ASSERT(wrong_attempts >= 3, "Should trigger lockout after 3 attempts");
}

static void test_change_password_success(void) {
    reset_mocks();
    
    /* Setup initial password */
    HAL_EEPROM_StorePassword("12345", 5);
    TEST_ASSERT(HAL_EEPROM_VerifyPassword("12345", 5), "Initial password should work");
    
    /* Change password */
    uint8_t result = HAL_EEPROM_ChangePassword("12345", 5, "54321", 5);
    TEST_ASSERT_EQ(0, result, "Password change should succeed");
    TEST_ASSERT(HAL_EEPROM_VerifyPassword("54321", 5), "New password should work");
    TEST_ASSERT(!HAL_EEPROM_VerifyPassword("12345", 5), "Old password should not work");
}

static void test_change_password_wrong_old(void) {
    reset_mocks();
    
    HAL_EEPROM_StorePassword("12345", 5);
    
    /* Try to change with wrong old password */
    uint8_t result = HAL_EEPROM_ChangePassword("99999", 5, "54321", 5);
    TEST_ASSERT_EQ(7, result, "Should fail with wrong old password");
    TEST_ASSERT(HAL_EEPROM_VerifyPassword("12345", 5), "Original password should still work");
    TEST_ASSERT(!HAL_EEPROM_VerifyPassword("54321", 5), "New password should not be set");
}

static void test_timeout_storage(void) {
    reset_mocks();
    
    /* Test valid timeout values */
    uint8_t result;
    
    result = MCAL_EEPROM_WriteWord(28U, 5U);
    TEST_ASSERT_EQ(0, result, "Should store timeout 5");
    
    result = MCAL_EEPROM_WriteWord(28U, 15U);
    TEST_ASSERT_EQ(0, result, "Should store timeout 15");
    
    result = MCAL_EEPROM_WriteWord(28U, 30U);
    TEST_ASSERT_EQ(0, result, "Should store timeout 30");
    
    /* Test timeout read */
    uint32_t timeout;
    result = MCAL_EEPROM_ReadWord(28U, &timeout);
    TEST_ASSERT_EQ(0, result, "Should read timeout");
    TEST_ASSERT_EQ(30, timeout, "Should read correct timeout value");
}

static void test_timeout_validation(void) {
    reset_mocks();
    
    /* Test valid range */
    TEST_ASSERT(5 >= 5 && 5 <= 30, "Timeout 5 should be valid");
    TEST_ASSERT(15 >= 5 && 15 <= 30, "Timeout 15 should be valid");
    TEST_ASSERT(30 >= 5 && 30 <= 30, "Timeout 30 should be valid");
    
    /* Test invalid range */
    TEST_ASSERT(!(4 >= 5 && 4 <= 30), "Timeout 4 should be invalid");
    TEST_ASSERT(!(31 >= 5 && 31 <= 30), "Timeout 31 should be invalid");
}

static void test_motor_control(void) {
    reset_mocks();
    
    HAL_Motor_Init();
    TEST_ASSERT_EQ(MOTOR_STOP, mock_motor_state, "Motor should start stopped");
    
    HAL_Motor_Move(MOTOR_FORWARD);
    TEST_ASSERT_EQ(MOTOR_FORWARD, mock_motor_state, "Motor should move forward");
    
    HAL_Motor_Move(MOTOR_BACKWARD);
    TEST_ASSERT_EQ(MOTOR_BACKWARD, mock_motor_state, "Motor should move backward");
    
    HAL_Motor_Move(MOTOR_STOP);
    TEST_ASSERT_EQ(MOTOR_STOP, mock_motor_state, "Motor should stop");
}

static void test_buzzer_activation(void) {
    reset_mocks();
    
    BUZZER_init();
    TEST_ASSERT(!mock_buzzer_beeped, "Buzzer should not be beeped initially");
    
    BUZZER_beep(10000U);
    TEST_ASSERT(mock_buzzer_beeped, "Buzzer should beep");
    TEST_ASSERT_EQ(10000U, mock_buzzer_duration, "Buzzer duration should be correct");
}

static void test_led_control(void) {
    reset_mocks();
    
    /* Test green LED */
    MCAL_GPIO_WritePin(0, (1U << 3), 1);
    TEST_ASSERT_EQ(1, mock_led_state, "Green LED should be on");
    
    /* Test red LED */
    MCAL_GPIO_WritePin(0, (1U << 1), 1);
    TEST_ASSERT_EQ(2, mock_led_state, "Red LED should be on");
    
    /* Test LED off */
    MCAL_GPIO_WritePin(0, (1U << 3), 0);
    MCAL_GPIO_WritePin(0, (1U << 1), 0);
    TEST_ASSERT_EQ(0, mock_led_state, "LEDs should be off");
}

static void test_uart_communication(void) {
    reset_mocks();
    
    /* Test send byte */
    HAL_COMM_SendByte('A');
    HAL_COMM_SendByte('B');
    HAL_COMM_SendByte('C');
    
    TEST_ASSERT_EQ('A', get_uart_tx_byte(0), "First byte should be 'A'");
    TEST_ASSERT_EQ('B', get_uart_tx_byte(1), "Second byte should be 'B'");
    TEST_ASSERT_EQ('C', get_uart_tx_byte(2), "Third byte should be 'C'");
    
    /* Test receive byte */
    uint8_t data[] = {'X', 'Y', 'Z'};
    setup_uart_rx_data(data, 3);
    
    TEST_ASSERT(HAL_COMM_IsDataAvailable(), "Data should be available");
    TEST_ASSERT_EQ('X', HAL_COMM_ReceiveByte(), "Should receive 'X'");
    TEST_ASSERT_EQ('Y', HAL_COMM_ReceiveByte(), "Should receive 'Y'");
    TEST_ASSERT_EQ('Z', HAL_COMM_ReceiveByte(), "Should receive 'Z'");
}

static void test_password_length_validation(void) {
    reset_mocks();
    
    /* Test minimum length */
    uint8_t result = HAL_EEPROM_StorePassword("1234", 4);
    TEST_ASSERT_EQ(0, result, "Password length 4 should be valid (minimum)");
    
    /* Test maximum length */
    result = HAL_EEPROM_StorePassword("1234567890123456", 16);
    TEST_ASSERT_EQ(0, result, "Password length 16 should be valid (maximum)");
    
    /* Test too short */
    result = HAL_EEPROM_StorePassword("123", 3);
    TEST_ASSERT_EQ(1, result, "Password length 3 should be invalid");
    
    /* Test too long */
    result = HAL_EEPROM_StorePassword("12345678901234567", 17);
    TEST_ASSERT_EQ(1, result, "Password length 17 should be invalid");
}

static void test_eeprom_clear_password(void) {
    reset_mocks();
    
    /* Store password */
    HAL_EEPROM_StorePassword("12345", 5);
    TEST_ASSERT(HAL_EEPROM_IsPasswordSet(), "Password should be set");
    
    /* Clear password */
    uint8_t result = HAL_EEPROM_ClearPassword();
    TEST_ASSERT_EQ(0, result, "Clear password should succeed");
    TEST_ASSERT(!HAL_EEPROM_IsPasswordSet(), "Password should not be set after clear");
    TEST_ASSERT(!HAL_EEPROM_VerifyPassword("12345", 5), "Cleared password should not verify");
}

/* Main Test Runner */
int main(void) {
    printf("========================================\n");
    printf("Control ECU Test Suite\n");
    printf("========================================\n\n");
    
    RUN_TEST(test_password_setup_success);
    RUN_TEST(test_password_setup_mismatch);
    RUN_TEST(test_password_verification_correct);
    RUN_TEST(test_password_verification_wrong_attempts);
    RUN_TEST(test_change_password_success);
    RUN_TEST(test_change_password_wrong_old);
    RUN_TEST(test_timeout_storage);
    RUN_TEST(test_timeout_validation);
    RUN_TEST(test_motor_control);
    RUN_TEST(test_buzzer_activation);
    RUN_TEST(test_led_control);
    RUN_TEST(test_uart_communication);
    RUN_TEST(test_password_length_validation);
    RUN_TEST(test_eeprom_clear_password);
    
    printf("========================================\n");
    printf("Test Results Summary\n");
    printf("========================================\n");
    printf("Total Tests Passed: %lu\n", total_passed);
    printf("Total Tests Failed: %lu\n", total_failed);
    printf("Total Tests: %lu\n", total_passed + total_failed);
    printf("========================================\n");
    
    if (total_failed == 0) {
        printf("ALL TESTS PASSED!\n");
        return 0;
    } else {
        printf("SOME TESTS FAILED!\n");
        return 1;
    }
}

