/*============================================================================
 *  Module      : Control ECU Main Application
 *  File Name   : main.c
 *  Description : Door Locker Security System - Control ECU Application
 *
 *  Communication Protocol (UART):
 *  ------------------------------
 *  Commands from HMI_ECU to Control_ECU:
 *    'S' - Setup Password: Receive 2 passwords (5 digits each), store if match
 *    'O' - Open Door: Receive password (5 digits), verify and open door
 *    'C' - Change Password: Receive old password, then 2 new passwords
 *    'T' - Set Timeout: Receive timeout value (1 byte integer, 5-30), then password
 *
 *  Responses from Control_ECU to HMI_ECU:
 *    'R' - Ready: System initialized and ready
 *    'Y' - Success: Operation completed successfully
 *    'N' - Failure: Operation failed (wrong password, etc.)
 *    'L' - Lockout: System locked out (3 wrong attempts)
 *
 *  Data Format:
 *    - Passwords: 5 bytes (ASCII digits, sent as individual bytes)
 *    - Timeout: 1 byte (integer value, 5-30)
 *===========================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"

#include "mcal/mcal_gpio.h"
#include "mcal/mcal_systick.h"
#include "mcal/mcal_eeprom.h"

#include "hal/hal_eeprom.h"
#include "hal/hal_motor.h"
#include "hal/hal_buzzer.h"
#include "hal/hal_comm.h"
#include "Types.h"

/*======================================================================
 *  Defines
 *====================================================================*/

/* LED Configuration */
#define LED_GPIO_PERIPH         SYSCTL_PERIPH_GPIOF
#define LED_PORT_BASE           GPIO_PORTF_BASE
#define GREEN_LED_PIN           (1U << 3)  /* PF3 */
#define RED_LED_PIN             (1U << 1)  /* PF1 */

/* Communication Protocol Commands */
#define CMD_SETUP_PASSWORD      'S'  /* Initial password setup */
#define CMD_OPEN_DOOR           'O'  /* Open door request */
#define CMD_CHANGE_PASSWORD     'C'  /* Change password */
#define CMD_SET_TIMEOUT         'T'  /* Set auto-lock timeout */
#define CMD_VERIFY_PASSWORD     'V'  /* Verify password (for timeout) */
#define CMD_READY               'R'  /* System ready */

/* Communication Protocol Responses */
#define RESP_SUCCESS            'Y'  /* Success/Yes */
#define RESP_FAILURE            'N'  /* Failure/No */
#define RESP_LOCKOUT            'L'  /* System locked out */
#define RESP_READY              'R'  /* Ready for command */

/* Password Configuration */
#define PASSWORD_MAX_LENGTH     (16U)  /* Maximum password length (matches HAL_EEPROM) */
#define PASSWORD_MIN_LENGTH     (4U)   /* Minimum password length (matches HAL_EEPROM) */
#define MAX_PASSWORD_ATTEMPTS   (3U)   /* Maximum wrong attempts before lockout */
#define LOCKOUT_BUZZER_DURATION (10000U)  /* 10 seconds buzzer on lockout */

/* Timeout Configuration */
#define TIMEOUT_MIN_SECONDS     (5U)   /* Minimum timeout: 5 seconds */
#define TIMEOUT_MAX_SECONDS     (30U)  /* Maximum timeout: 30 seconds */
#define TIMEOUT_DEFAULT_SECONDS (15U)  /* Default timeout: 15 seconds */

/* EEPROM Addresses */
#define EEPROM_TIMEOUT_ADDR     (28U)  /* Timeout value storage (after password flag at 24) */

/*======================================================================
 *  Local Variables
 *====================================================================*/

static uint8_t wrongAttempts = 0U;
static boolean isLockedOut = FALSE;
static uint32_t currentTimeout = TIMEOUT_DEFAULT_SECONDS;

/*======================================================================
 *  Local Function Prototypes
 *====================================================================*/

static void System_Init(void);
static void LED_Init(void);
static void LED_SetGreen(void);
static void LED_SetRed(void);
static void LED_Clear(void);
static uint32_t EEPROM_ReadTimeout(void);
static uint8_t EEPROM_StoreTimeout(uint32_t timeout);
static void HandlePasswordSetup(void);
static void HandleOpenDoor(void);
static void HandleChangePassword(void);
static void HandleSetTimeout(void);
static void ActivateLockout(void);
static void OpenDoorSequence(uint32_t timeoutSeconds);
static void COMM_FlushRx(void);

/*======================================================================
 *  Main Function
 *====================================================================*/

int main(void)
{
    uint8_t command;
    uint8_t eepromResult;
    
    /* System Clock Setup */
    //SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    
    /* Initialize all hardware modules */
    System_Init();
    
    /* Initialize EEPROM */
    eepromResult = HAL_EEPROM_Init();
    if (eepromResult != HAL_EEPROM_SUCCESS)
    {
        /* EEPROM initialization failed - indicate error with red LED */
        LED_SetRed();
        // while(1)
        // {
        //     /* System cannot function without EEPROM */
        // }
    }

    HAL_EEPROM_ClearPassword();
    
    /* Load timeout value from EEPROM */
    currentTimeout = EEPROM_ReadTimeout();
    if (currentTimeout < TIMEOUT_MIN_SECONDS || currentTimeout > TIMEOUT_MAX_SECONDS)
    {
        /* Invalid timeout, use default */
        currentTimeout = TIMEOUT_DEFAULT_SECONDS;
        EEPROM_StoreTimeout(currentTimeout);
    }
    
    /* Send ready signal to HMI */
    HAL_COMM_SendByte(CMD_READY);
    
    /* Send current timeout value to HMI */
    HAL_COMM_SendByte((uint8_t)currentTimeout);
    
    /* Main application loop */
    while(1)
    {
        /* Check if data is available from HMI */
        if (HAL_COMM_IsDataAvailable())
        {
            /* Read command byte */
            command = HAL_COMM_ReceiveByte();
            
            /* Process command based on current state */
            switch(command)
            {
                case CMD_SETUP_PASSWORD:
                    if (!isLockedOut)
                    {
                        HandlePasswordSetup();
                    }
                    else
                    {
                        HAL_COMM_SendByte(RESP_LOCKOUT);
                        COMM_FlushRx();
                    }
                    break;
                    
                case CMD_OPEN_DOOR:
                    if (!isLockedOut)
                    {
                        HandleOpenDoor();
                    }
                    else
                    {
                        HAL_COMM_SendByte(RESP_LOCKOUT);
                        COMM_FlushRx();
                    }
                    break;
                    
                case CMD_CHANGE_PASSWORD:
                    if (!isLockedOut)
                    {
                        HandleChangePassword();
                    }
                    else
                    {
                        HAL_COMM_SendByte(RESP_LOCKOUT);
                        COMM_FlushRx();
                    }
                    break;
                    
                case CMD_SET_TIMEOUT:
                    if (!isLockedOut)
                    {
                        HandleSetTimeout();
                    }
                    else
                    {
                        HAL_COMM_SendByte(RESP_LOCKOUT);
                        COMM_FlushRx();
                    }
                    break;
                    
                default:
                    /* Unknown command - ignore */
                    break;
            }
        }
        
        /* Small delay to prevent busy-waiting */
        MCAL_SysTick_DelayMs(10U);
    }
    
    //return 0;
}

/*======================================================================
 *  Initialization Functions
 *====================================================================*/

/**
 * @brief Initialize all system modules
 */
static void System_Init(void)
{
    /* Initialize SysTick for delays */
    MCAL_SysTick_Init();
    
    /* Initialize UART communication */
    HAL_COMM_Init();
    
    /* Initialize Motor */
    HAL_Motor_Init();
    
    /* Initialize Buzzer */
    BUZZER_init();
    
    /* Initialize LEDs */
    LED_Init();
    
    /* Ensure motor is stopped initially */
    HAL_Motor_Move(MOTOR_STOP);
}

/**
 * @brief Initialize LED pins
 */
static void LED_Init(void)
{
    MCAL_GPIO_EnablePort(LED_GPIO_PERIPH);
    MCAL_GPIO_InitPin(LED_PORT_BASE, GREEN_LED_PIN | RED_LED_PIN, 
                      GPIO_DIR_OUTPUT, GPIO_ATTACH_DEFAULT);
    LED_Clear();
}

/**
 * @brief Set green LED on, red LED off
 */
static void LED_SetGreen(void)
{
    MCAL_GPIO_WritePin(LED_PORT_BASE, GREEN_LED_PIN, LOGIC_HIGH);
    MCAL_GPIO_WritePin(LED_PORT_BASE, RED_LED_PIN, LOGIC_LOW);
}

/**
 * @brief Set red LED on, green LED off
 */
static void LED_SetRed(void)
{
    MCAL_GPIO_WritePin(LED_PORT_BASE, GREEN_LED_PIN, LOGIC_LOW);
    MCAL_GPIO_WritePin(LED_PORT_BASE, RED_LED_PIN, LOGIC_HIGH);
}

/**
 * @brief Turn off both LEDs
 */
static void LED_Clear(void)
{
    MCAL_GPIO_WritePin(LED_PORT_BASE, GREEN_LED_PIN, LOGIC_LOW);
    MCAL_GPIO_WritePin(LED_PORT_BASE, RED_LED_PIN, LOGIC_LOW);
}

/*======================================================================
 *  EEPROM Timeout Functions
 *====================================================================*/

/**
 * @brief Read timeout value from EEPROM
 * @return Timeout value in seconds, or default if invalid
 */
static uint32_t EEPROM_ReadTimeout(void)
{
    uint8_t result;
    uint32_t timeoutWord;
    
    result = MCAL_EEPROM_ReadWord(EEPROM_TIMEOUT_ADDR, &timeoutWord);
    
    if (result == EEPROM_SUCCESS)
    {
        /* Validate timeout range */
        if (timeoutWord >= TIMEOUT_MIN_SECONDS && timeoutWord <= TIMEOUT_MAX_SECONDS)
        {
            return timeoutWord;
        }
    }
    
    /* Return default if read failed or value invalid */
    return TIMEOUT_DEFAULT_SECONDS;
}

/**
 * @brief Store timeout value to EEPROM
 * @param timeout Timeout value in seconds (5-30)
 * @return HAL_EEPROM_SUCCESS if stored successfully
 */
static uint8_t EEPROM_StoreTimeout(uint32_t timeout)
{
    uint8_t result;
    
    /* Validate timeout range */
    if (timeout < TIMEOUT_MIN_SECONDS || timeout > TIMEOUT_MAX_SECONDS)
    {
        return HAL_EEPROM_ERROR_INVALID_PARAM;
    }
    
    /* Store timeout value */
    result = MCAL_EEPROM_WriteWord(EEPROM_TIMEOUT_ADDR, timeout);
    
    if (result == EEPROM_SUCCESS)
    {
        return HAL_EEPROM_SUCCESS;
    }
    else
    {
        return HAL_EEPROM_ERROR_WRITE;
    }
}

/*======================================================================
 *  Command Handlers
 *====================================================================*/

/**
 * @brief Handle initial password setup
 * Protocol:
 * 1. Receive first password (5 digits)
 * 2. Receive second password (5 digits) for confirmation
 * 3. Compare and store if match, otherwise send failure
 */
static void HandlePasswordSetup(void)
{
    char password1[PASSWORD_MAX_LENGTH + 1U];
    char password2[PASSWORD_MAX_LENGTH + 1U];
    boolean passwordsMatch;
    uint8_t result;
    uint8_t len1, len2;
    uint8_t i;
    
    /* Receive first password length */
    len1 = HAL_COMM_ReceiveByte();
    
    /* If length is 0, this is a query - check if password is already set */
    if (len1 == 0U)
    {
        if (HAL_EEPROM_IsPasswordSet())
        {
            HAL_COMM_SendByte(RESP_FAILURE);  /* Password already set */
        }
        else
        {
            HAL_COMM_SendByte(RESP_SUCCESS);  /* No password, need setup */
        }
        return;
    }
    
    /* Validate first password length */
    if (len1 < PASSWORD_MIN_LENGTH || len1 > PASSWORD_MAX_LENGTH)
    {
        HAL_COMM_SendByte(RESP_FAILURE);
        return;
    }
    
    /* Receive first password (variable length) */
    for (i = 0U; i < len1; i++)
    {
        password1[i] = (char)HAL_COMM_ReceiveByte();
    }
    password1[len1] = '\0';
    
    /* Receive second password length */
    len2 = HAL_COMM_ReceiveByte();
    
    /* Validate second password length */
    if (len2 < PASSWORD_MIN_LENGTH || len2 > PASSWORD_MAX_LENGTH)
    {
        HAL_COMM_SendByte(RESP_FAILURE);
        return;
    }
    
    /* Receive second password (confirmation, variable length) */
    for (i = 0U; i < len2; i++)
    {
        password2[i] = (char)HAL_COMM_ReceiveByte();
    }
    password2[len2] = '\0';
    
    /* Check if lengths match first */
    if (len1 != len2)
    {
        HAL_COMM_SendByte(RESP_FAILURE);
        return;
    }
    
    /* Compare passwords */
    passwordsMatch = TRUE;
    for (i = 0U; i < len1; i++)
    {
        if (password1[i] != password2[i])
        {
            passwordsMatch = FALSE;
            break;
        }
    }
    
    if (passwordsMatch)
    {
        /* Store password to EEPROM */
        result = HAL_EEPROM_StorePassword(password1, len1);
        if (result == HAL_EEPROM_SUCCESS)
        {
            HAL_COMM_SendByte(RESP_SUCCESS);
            LED_SetGreen();
            wrongAttempts = 0U;  /* Reset wrong attempts */
            isLockedOut = FALSE;  /* Clear lockout on success */
        }
        else
        {
            HAL_COMM_SendByte(RESP_FAILURE);
        }
    }
    else
    {
        /* Passwords don't match */
        HAL_COMM_SendByte(RESP_FAILURE);
    }
}

/**
 * @brief Handle open door request
 * Protocol:
 * 1. Receive password from HMI
 * 2. Verify password
 * 3. If correct: open door, wait timeout, close door
 * 4. If wrong: increment attempts, activate lockout if 3 failures
 */
static void HandleOpenDoor(void)
{
    char receivedPassword[PASSWORD_MAX_LENGTH + 1U];
    boolean isCorrect;
    uint8_t pwdLen;
    uint8_t i;
    
    /* Receive password length */
    pwdLen = HAL_COMM_ReceiveByte();
    
    /* Validate length */
    if (pwdLen > PASSWORD_MAX_LENGTH)
    {
        HAL_COMM_SendByte(RESP_FAILURE);
        return;
    }
    
    /* Receive password from HMI (variable length) */
    for (i = 0U; i < pwdLen; i++)
    {
        receivedPassword[i] = (char)HAL_COMM_ReceiveByte();
    }
    receivedPassword[pwdLen] = '\0';
    
    /* Verify password */
    isCorrect = HAL_EEPROM_VerifyPassword(receivedPassword, pwdLen);
    
    if (isCorrect)
    {
        /* Correct password - open door */
        HAL_COMM_SendByte(RESP_SUCCESS);
        LED_SetGreen();
        wrongAttempts = 0U;  /* Reset wrong attempts */
        isLockedOut = FALSE;  /* Clear lockout on success */
        OpenDoorSequence(currentTimeout);
    }
    else
    {
        /* Wrong password */
        //HAL_COMM_SendByte(RESP_FAILURE);
        LED_SetRed();
        wrongAttempts++;
        
        if (wrongAttempts >= MAX_PASSWORD_ATTEMPTS)
        {
            HAL_COMM_SendByte(RESP_LOCKOUT);
            ActivateLockout();
        }
        else
        {
            HAL_COMM_SendByte(RESP_FAILURE);
        }
    }
}

/**
 * @brief Handle change password request
 * Protocol:
 * 1. Receive old password
 * 2. Verify old password
 * 3. If correct: receive new password twice (confirmation)
 * 4. Store new password if confirmation matches
 */
static void HandleChangePassword(void)
{
    char oldPassword[PASSWORD_MAX_LENGTH + 1U];
    char newPassword1[PASSWORD_MAX_LENGTH + 1U];
    char newPassword2[PASSWORD_MAX_LENGTH + 1U];
    boolean passwordsMatch;
    uint8_t result;
    uint8_t oldLen, newLen1, newLen2;
    uint8_t i;
    
    /* Receive old password length */
    oldLen = HAL_COMM_ReceiveByte();
    
    /* Validate length */
    if (oldLen > PASSWORD_MAX_LENGTH)
    {
        HAL_COMM_SendByte(RESP_FAILURE);
        return;
    }
    
    /* Receive old password (variable length) */
    for (i = 0U; i < oldLen; i++)
    {
        oldPassword[i] = (char)HAL_COMM_ReceiveByte();
    }
    oldPassword[oldLen] = '\0';
    
    /* Verify old password */
    if (!HAL_EEPROM_VerifyPassword(oldPassword, oldLen))
    {
        LED_SetRed();
        wrongAttempts++;
        if (wrongAttempts >= MAX_PASSWORD_ATTEMPTS)
        {
            HAL_COMM_SendByte(RESP_LOCKOUT);
            ActivateLockout();
        }
        else
        {
            HAL_COMM_SendByte(RESP_FAILURE);
        }
        return;
    }
    
    /* Old password correct - receive new password length */
    newLen1 = HAL_COMM_ReceiveByte();
    
    /* Validate new password length */
    if (newLen1 < PASSWORD_MIN_LENGTH || newLen1 > PASSWORD_MAX_LENGTH)
    {
        HAL_COMM_SendByte(RESP_FAILURE);
        return;
    }
    
    /* Receive new password (variable length) */
    for (i = 0U; i < newLen1; i++)
    {
        newPassword1[i] = (char)HAL_COMM_ReceiveByte();
    }
    newPassword1[newLen1] = '\0';
    
    /* Receive confirmation length */
    newLen2 = HAL_COMM_ReceiveByte();
    
    /* Validate confirmation length */
    if (newLen2 < PASSWORD_MIN_LENGTH || newLen2 > PASSWORD_MAX_LENGTH)
    {
        HAL_COMM_SendByte(RESP_FAILURE);
        return;
    }
    
    /* Receive confirmation (variable length) */
    for (i = 0U; i < newLen2; i++)
    {
        newPassword2[i] = (char)HAL_COMM_ReceiveByte();
    }
    newPassword2[newLen2] = '\0';
    
    /* Check if lengths match first */
    if (newLen1 != newLen2)
    {
        HAL_COMM_SendByte(RESP_FAILURE);
        return;
    }
    
    /* Compare new passwords */
    passwordsMatch = TRUE;
    for (i = 0U; i < newLen1; i++)
    {
        if (newPassword1[i] != newPassword2[i])
        {
            passwordsMatch = FALSE;
            break;
        }
    }
    
    if (passwordsMatch)
    {
        /* Store new password */
        result = HAL_EEPROM_ChangePassword(oldPassword, oldLen,
                                           newPassword1, newLen1);
        if (result == HAL_EEPROM_SUCCESS)
        {
            HAL_COMM_SendByte(RESP_SUCCESS);
            LED_SetGreen();
            wrongAttempts = 0U;  /* Reset wrong attempts */
            isLockedOut = FALSE;  /* Clear lockout on success */
        }
        else
        {
            HAL_COMM_SendByte(RESP_FAILURE);
        }
    }
    else
    {
        /* New passwords don't match */
        HAL_COMM_SendByte(RESP_FAILURE);
    }
}

/**
 * @brief Handle set timeout request
 * Protocol:
 * 1. Receive timeout value (as single byte integer, 5-30)
 * 2. Receive password for verification (exactly 5 bytes)
 * 3. If password correct: store timeout value
 */
static void HandleSetTimeout(void)
{
    char password[PASSWORD_MAX_LENGTH + 1U];
    uint8_t timeoutValue;
    uint8_t result;
    uint8_t i;
    
    /* Receive timeout value as single byte (integer) */
    timeoutValue = HAL_COMM_ReceiveByte();
    
    /* Validate timeout range (5-30 seconds) */
    if (timeoutValue < TIMEOUT_MIN_SECONDS || timeoutValue > TIMEOUT_MAX_SECONDS)
    {
        HAL_COMM_SendByte(RESP_FAILURE);
        return;
    }
    
    /* Receive password length */
    uint8_t pwdLen = HAL_COMM_ReceiveByte();
    
    /* Validate length */
    if (pwdLen > PASSWORD_MAX_LENGTH)
    {
        HAL_COMM_SendByte(RESP_FAILURE);
        return;
    }
    
    /* Receive password for verification (variable length) */
    for (i = 0U; i < pwdLen; i++)
    {
        password[i] = (char)HAL_COMM_ReceiveByte();
    }
    password[pwdLen] = '\0';
    
    /* Verify password */
    if (!HAL_EEPROM_VerifyPassword(password, pwdLen))
    {
        LED_SetRed();
        wrongAttempts++;
        if (wrongAttempts >= MAX_PASSWORD_ATTEMPTS)
        {
            HAL_COMM_SendByte(RESP_LOCKOUT);
            ActivateLockout();
        }
        else
        {
            HAL_COMM_SendByte(RESP_FAILURE);
        }
        return;
    }
    
    /* Password correct - store timeout */
    result = EEPROM_StoreTimeout((uint32_t)timeoutValue);
    if (result == HAL_EEPROM_SUCCESS)
    {
        currentTimeout = (uint32_t)timeoutValue;
        HAL_COMM_SendByte(RESP_SUCCESS);
        LED_SetGreen();
        wrongAttempts = 0U;  /* Reset wrong attempts */
        isLockedOut = FALSE;  /* Clear lockout on success */
    }
    else
    {
        HAL_COMM_SendByte(RESP_FAILURE);
    }
}

/*======================================================================
 *  Helper Functions
 *====================================================================*/


/**
 * @brief Activate lockout mode (3 wrong attempts)
 * - Sound buzzer for specified duration
 * - Set lockout flag during buzzer period
 * - Reset wrong attempts counter
 * - Clear lockout after buzzer period (system returns to main menu)
 */
static void ActivateLockout(void)
{
    isLockedOut = TRUE;
    wrongAttempts = 0U;
    LED_SetRed();

    /* Sound buzzer for lockout duration */
    BUZZER_beep(LOCKOUT_BUZZER_DURATION);

    /* Drop any queued bytes sent during lockout (prevents stale commands) */
    COMM_FlushRx();
    
    /* After lockout period, clear lockout flag (system returns to main menu) */
    isLockedOut = FALSE;
}

static void COMM_FlushRx(void)
{
    while (HAL_COMM_IsDataAvailable())
    {
        (void)HAL_COMM_ReceiveByte();
    }
}

/**
 * @brief Execute door opening sequence
 * @param timeoutSeconds Timeout in seconds before auto-lock
 */
static void OpenDoorSequence(uint32_t timeoutSeconds)
{
    /* Safety check: ensure timeout is valid */
    if (timeoutSeconds < TIMEOUT_MIN_SECONDS || timeoutSeconds > TIMEOUT_MAX_SECONDS)
    {
        timeoutSeconds = TIMEOUT_DEFAULT_SECONDS;
    }
    
    /* 1. Unlock door (motor forward) */
    HAL_Motor_Move(MOTOR_FORWARD);
    
    /* 2. Wait for bolt to fully retract (2 seconds) */
    MCAL_SysTick_DelayMs(2000U);
    
    /* 3. Stop motor (door is now unlocked) */
    HAL_Motor_Move(MOTOR_STOP);
    
    /* 4. Wait for timeout period (door stays open, user can enter) */
    MCAL_SysTick_DelayMs(timeoutSeconds * 1000U);
    
    /* 5. Lock door (motor backward) */
    HAL_Motor_Move(MOTOR_BACKWARD);
    
    /* 6. Wait for bolt to fully extend (2 seconds) */
    MCAL_SysTick_DelayMs(2000U);
    
    /* 7. Stop motor (door is now locked) */
    HAL_Motor_Move(MOTOR_STOP);
}