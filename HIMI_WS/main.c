/*============================================================================
 *  HMI ECU Main Application
 *  - Displays menu on LCD
 *  - Reads keypad and potentiometer
 *  - Sends commands to Control ECU over UART using hal_comm
 *===========================================================================*/

#include <stdint.h>
#include <stdbool.h>

#include "driverlib/sysctl.h"

#include "Types.h"
#include "mcal/mcal_systick.h"

#include "hal/hal_lcd.h"
#include "hal/hal_keypad.h"
#include "hal/hal_rgb_led.h"
#include "hal/hal_potentiometer.h"
#include "hal/hal_comm.h"

#define PASSWORD_MAX_LENGTH     16U  /* Maximum password length (matches EEPROM HAL) */
#define PASSWORD_MIN_LENGTH     4U   /* Minimum password length (matches EEPROM HAL) */
#define TIMEOUT_MIN_SECONDS     5U
#define TIMEOUT_MAX_SECONDS     30U
#define TIMEOUT_DEFAULT_SECONDS 15U  /* Default timeout if not set */

/*======================================================================
 *  Global Variables
 *====================================================================*/
static uint8_t g_currentTimeout = TIMEOUT_DEFAULT_SECONDS;

/* Command bytes (must match Control ECU) */
#define CMD_SETUP_PASSWORD      'S'
#define CMD_OPEN_DOOR           'O'
#define CMD_CHANGE_PASSWORD     'C'
#define CMD_SET_TIMEOUT         'T'

/* Responses from Control ECU */
#define RESP_SUCCESS            'Y'
#define RESP_FAILURE            'N'
#define RESP_LOCKOUT            'L'
#define RESP_READY              'R'

/* Helper prototypes */
static void HMI_Init(void);
static void HMI_WaitForReady(void);
static char HMI_WaitKey(void);
static uint8_t HMI_ReadPasswordUntilHash(char *buf, uint8_t maxLen);
static uint8_t HMI_ReadTimeoutFromPot(void);
static uint8_t HMI_WaitResponse(void);
static void HMI_ShowMessage(const char *line1, const char *line2, uint32_t delayMs);

static void Handle_SetupPassword(void);
static void Handle_OpenDoor(void);
static void Handle_ChangePassword(void);
static void Handle_SetTimeout(void);

int main(void)
{
    //SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    HMI_Init();
    HMI_WaitForReady();
    

    /* Check if password needs to be set for the first time */
    Handle_SetupPassword();

    while(1)
    {
        /* Show main menu */
        Lcd_Clear();
        Lcd_GoToRowColumn(0, 0);
        Lcd_DisplayString("+Open  -Change");
        Lcd_GoToRowColumn(1, 0);
        Lcd_DisplayString("*Timeout");

        char key = HMI_WaitKey();

        if (key == '+')
        {
            Handle_OpenDoor();
        }
        else if (key == '-')
        {
            Handle_ChangePassword();
        }
        else if (key == '*')
        {
            Handle_SetTimeout();
        }
    }

    return 0;
}

/*======================================================================
 *  Helpers
 *====================================================================*/
static void HMI_Init(void)
{
    MCAL_SysTick_Init();
    Lcd_Init();
    HAL_Keypad_Init();
    POT_Init();
    RGB_LED_Init();
    HAL_COMM_Init();
    Lcd_Clear();
}

static void HMI_WaitForReady(void)
{
    Lcd_Clear();
    Lcd_DisplayString("Waiting Control");
    while (1)
    {
        if (HAL_COMM_IsDataAvailable())
        {
            uint8_t b = HAL_COMM_ReceiveByte();
            if (b == RESP_READY)
            {
                Lcd_Clear();
                Lcd_DisplayString("Control Ready");
                MCAL_SysTick_DelayMs(800U);
                return;
            }
        }
    }
}

static char HMI_WaitKey(void)
{
    char k;
    do {
        k = HAL_Keypad_GetKey();
    } while (k == '\0');
    return k;
}

/**
 * @brief Read password until '#' is pressed
 * - Displays actual digits (not '*')
 * - '*' key clears current input line
 * - Returns number of characters entered (not including '#')
 */
static uint8_t HMI_ReadPasswordUntilHash(char *buf, uint8_t maxLen)
{
    uint8_t i = 0;

    while (1)
    {
        char k = HMI_WaitKey();

        if (k == '#') { break; }  /* Terminate on '#' */

        if (k == '*')  /* Clear input */
        {
            i = 0;
            Lcd_GoToRowColumn(1, 0);
            Lcd_DisplayString("                ");  /* Clear line */
            Lcd_GoToRowColumn(1, 0);
            continue;
        }

        if (i < maxLen)
        {
            buf[i++] = k;
            Lcd_DisplayCharacter(k);  /* Display actual digit */
        }
        /* Ignore extra keys beyond maxLen */
    }

    buf[i] = '\0';
    return i;
}

static uint8_t HMI_ReadTimeoutFromPot(void)
{
    /* Map 0-100% to 5..30 with averaging for noise reduction */
    uint8_t pct = POT_ReadPercentageAveraged(16);  /* Average 16 samples */
    uint8_t range = TIMEOUT_MAX_SECONDS - TIMEOUT_MIN_SECONDS;
    uint8_t t = (uint8_t)(TIMEOUT_MIN_SECONDS + ((pct * range) / 100U));
    if (t < TIMEOUT_MIN_SECONDS) t = TIMEOUT_MIN_SECONDS;
    if (t > TIMEOUT_MAX_SECONDS) t = TIMEOUT_MAX_SECONDS;
    return t;
}

static uint8_t HMI_WaitResponse(void)
{
    /* Blocking wait for 1 byte */
    return HAL_COMM_ReceiveByte();
}

static void HMI_ShowMessage(const char *line1, const char *line2, uint32_t delayMs)
{
    Lcd_Clear();
    if (line1) { Lcd_GoToRowColumn(0, 0); Lcd_DisplayString(line1); }
    if (line2) { Lcd_GoToRowColumn(1, 0); Lcd_DisplayString(line2); }
    if (delayMs > 0U) { MCAL_SysTick_DelayMs(delayMs); }
}

/*======================================================================
 *  Handlers
 *====================================================================*/

/**
 * @brief Handle initial password setup (first-time or after reset)
 * Only proceeds if no password is currently set
 */
static void Handle_SetupPassword(void)
{
    char pwd1[PASSWORD_MAX_LENGTH + 1];
    char pwd2[PASSWORD_MAX_LENGTH + 1];
    uint8_t len1, len2;
    uint8_t resp;

    /* Send setup command to check if password is already set */
    HAL_COMM_SendByte(CMD_SETUP_PASSWORD);
    HAL_COMM_SendByte(0);  /* Query mode: send 0 length */
    
    /* Wait for response */
    resp = HMI_WaitResponse();
    
    /* If password already set, skip setup */
    if (resp == RESP_FAILURE)
    {
        return;  /* Password already exists */
    }

    /* Password not set - proceed with setup */
    while (1)
    {
        /* Get first password */
        Lcd_Clear();
        Lcd_GoToRowColumn(0, 0);
        Lcd_DisplayString("Set Password:");
        Lcd_GoToRowColumn(1, 0);
        len1 = HMI_ReadPasswordUntilHash(pwd1, PASSWORD_MAX_LENGTH);

        /* Validate length */
        if (len1 < PASSWORD_MIN_LENGTH)
        {
            HMI_ShowMessage("Too Short!", "Min 4 digits", 1500U);
            continue;
        }

        /* Get confirmation password */
        Lcd_Clear();
        Lcd_GoToRowColumn(0, 0);
        Lcd_DisplayString("Confirm PWD:");
        Lcd_GoToRowColumn(1, 0);
        len2 = HMI_ReadPasswordUntilHash(pwd2, PASSWORD_MAX_LENGTH);

        /* Check if lengths match */
        if (len1 != len2)
        {
            HMI_ShowMessage("Length", "Mismatch!", 1500U);
            continue;
        }

        /* Check if passwords match */
        uint8_t match = 1;
        for (uint8_t i = 0; i < len1; i++)
        {
            if (pwd1[i] != pwd2[i])
            {
                match = 0;
                break;
            }
        }

        if (!match)
        {
            HMI_ShowMessage("Passwords", "Don't Match!", 1500U);
            continue;
        }

        /* Send to Control ECU */
        HAL_COMM_SendByte(CMD_SETUP_PASSWORD);
        HAL_COMM_SendByte(len1);  /* Send length first */
        for (uint8_t i = 0; i < len1; i++)
        {
            HAL_COMM_SendByte((uint8_t)pwd1[i]);
        }
        HAL_COMM_SendByte(len2);  /* Send confirmation length */
        for (uint8_t i = 0; i < len2; i++)
        {
            HAL_COMM_SendByte((uint8_t)pwd2[i]);
        }

        /* Wait for response */
        resp = HMI_WaitResponse();
        if (resp == RESP_SUCCESS)
        {
            HMI_ShowMessage("Password", "Saved!", 1500U);
            break;  /* Setup complete */
        }
        else
        {
            HMI_ShowMessage("Setup Failed", "Try Again", 1500U);
        }
    }
}

static void Handle_OpenDoor(void)
{
    char pwd[PASSWORD_MAX_LENGTH + 1];
    uint8_t pwdLen;

    /* Prompt for password */
    Lcd_Clear();
    Lcd_GoToRowColumn(0, 0);
    Lcd_DisplayString("Enter PWD:");
    Lcd_GoToRowColumn(1, 0);
    pwdLen = HMI_ReadPasswordUntilHash(pwd, PASSWORD_MAX_LENGTH);

    /* Send command */
    HAL_COMM_SendByte(CMD_OPEN_DOOR);
    HAL_COMM_SendByte(pwdLen);  /* Send length first */
    for (uint8_t i = 0; i < pwdLen; i++)
    {
        HAL_COMM_SendByte((uint8_t)pwd[i]);
    }

    uint8_t resp = HMI_WaitResponse();
    if (resp == RESP_SUCCESS)
    {
        /* Show unlocking message */
        Lcd_Clear();
        Lcd_GoToRowColumn(0, 0);
        Lcd_DisplayString("Unlocking...");
        MCAL_SysTick_DelayMs(2000U);  /* Match Control ECU unlock time */
        
        /* Countdown during door open period */
        Lcd_Clear();
        Lcd_GoToRowColumn(0, 0);
        Lcd_DisplayString("Door Open");
        
        /* Use the configured timeout for countdown */
        uint8_t timeout = g_currentTimeout;
        
        for (uint8_t remaining = timeout; remaining > 0; remaining--)
        {
            Lcd_GoToRowColumn(1, 0);
            Lcd_DisplayString("Closing: ");
            
            if (remaining < 10)
            {
                Lcd_DisplayCharacter('0' + remaining);
                Lcd_DisplayCharacter(' ');
            }
            else
            {
                Lcd_DisplayCharacter('0' + (remaining / 10));
                Lcd_DisplayCharacter('0' + (remaining % 10));
            }
            Lcd_DisplayString(" sec  ");
            
            MCAL_SysTick_DelayMs(1000U);  /* Wait 1 second */
        }
        
        /* Show locking message */
        Lcd_Clear();
        Lcd_GoToRowColumn(0, 0);
        Lcd_DisplayString("Locking...");
        MCAL_SysTick_DelayMs(2000U);  /* Match Control ECU lock time */
        
        /* Show completion */
        Lcd_Clear();
        Lcd_GoToRowColumn(0, 0);
        Lcd_DisplayString("Door Secured");
        MCAL_SysTick_DelayMs(1500U);
    }
    else if (resp == RESP_LOCKOUT)
    {
        HMI_ShowMessage("LOCKOUT", "", 1500U);
    }
    else
    {
        HMI_ShowMessage("Wrong PWD", "", 1000U);
    }
}

static void Handle_ChangePassword(void)
{
    char oldPwd[PASSWORD_MAX_LENGTH + 1];
    char newPwd[PASSWORD_MAX_LENGTH + 1];
    char confPwd[PASSWORD_MAX_LENGTH + 1];
    uint8_t oldLen, newLen, confLen;

    /* Get old password */
    Lcd_Clear();
    Lcd_GoToRowColumn(0, 0);
    Lcd_DisplayString("Old Password:");
    Lcd_GoToRowColumn(1, 0);
    oldLen = HMI_ReadPasswordUntilHash(oldPwd, PASSWORD_MAX_LENGTH);

    /* Get new password */
    Lcd_Clear();
    Lcd_GoToRowColumn(0, 0);
    Lcd_DisplayString("New Password:");
    Lcd_GoToRowColumn(1, 0);
    newLen = HMI_ReadPasswordUntilHash(newPwd, PASSWORD_MAX_LENGTH);

    /* Validate new password length */
    if (newLen < PASSWORD_MIN_LENGTH)
    {
        HMI_ShowMessage("Too Short!", "Min 4 digits", 1500U);
        return;
    }

    /* Get confirmation */
    Lcd_Clear();
    Lcd_GoToRowColumn(0, 0);
    Lcd_DisplayString("Confirm New:");
    Lcd_GoToRowColumn(1, 0);
    confLen = HMI_ReadPasswordUntilHash(confPwd, PASSWORD_MAX_LENGTH);

    /* Send command */
    HAL_COMM_SendByte(CMD_CHANGE_PASSWORD);
    HAL_COMM_SendByte(oldLen);
    for (uint8_t i = 0; i < oldLen; i++) HAL_COMM_SendByte((uint8_t)oldPwd[i]);
    HAL_COMM_SendByte(newLen);
    for (uint8_t i = 0; i < newLen; i++) HAL_COMM_SendByte((uint8_t)newPwd[i]);
    HAL_COMM_SendByte(confLen);
    for (uint8_t i = 0; i < confLen; i++) HAL_COMM_SendByte((uint8_t)confPwd[i]);

    uint8_t resp = HMI_WaitResponse();
    if (resp == RESP_SUCCESS)
    {
        HMI_ShowMessage("Password", "Changed", 1000U);
    }
    else if (resp == RESP_LOCKOUT)
    {
        HMI_ShowMessage("LOCKOUT", "", 1500U);
    }
    else
    {
        HMI_ShowMessage("Change Failed", "", 1000U);
    }
}

static void Handle_SetTimeout(void)
{
    uint8_t timeoutVal = TIMEOUT_MIN_SECONDS;
    Lcd_Clear();
    Lcd_DisplayString("Adj Timeout");
    Lcd_GoToRowColumn(1, 0);
    Lcd_DisplayString("#=OK  *=Back");

    while (1)
    {
        timeoutVal = HMI_ReadTimeoutFromPot();
        Lcd_GoToRowColumn(0, 12);
        /* Overwrite with current value */
        char buf[5];
        buf[0] = (char)('0' + (timeoutVal / 10U));
        buf[1] = (char)('0' + (timeoutVal % 10U));
        buf[2] = 's';
        buf[3] = ' ';
        buf[4] = '\0';
        Lcd_DisplayString(buf);

        char k = HAL_Keypad_GetKey();
        if (k == '#')
        {
            break; /* confirm */
        }
        else if (k == '*')
        {
            return; /* cancel */
        }
    }

    /* Read password to authorize */
    char pwd[PASSWORD_MAX_LENGTH + 1];
    uint8_t pwdLen;
    
    Lcd_Clear();
    Lcd_GoToRowColumn(0, 0);
    Lcd_DisplayString("Enter PWD:");
    Lcd_GoToRowColumn(1, 0);
    pwdLen = HMI_ReadPasswordUntilHash(pwd, PASSWORD_MAX_LENGTH);

    HAL_COMM_SendByte(CMD_SET_TIMEOUT);
    HAL_COMM_SendByte(timeoutVal); /* 1-byte integer */
    HAL_COMM_SendByte(pwdLen);
    for (uint8_t i = 0; i < pwdLen; i++) HAL_COMM_SendByte((uint8_t)pwd[i]);

    uint8_t resp = HMI_WaitResponse();
    if (resp == RESP_SUCCESS)
    {
        g_currentTimeout = timeoutVal;  /* Update local copy */
        HMI_ShowMessage("Timeout Saved", "", 1000U);
    }
    else if (resp == RESP_LOCKOUT)
    {
        HMI_ShowMessage("LOCKOUT", "", 1500U);
    }
    else
    {
        HMI_ShowMessage("Failed", "", 1000U);
    }
}
