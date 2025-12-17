/*
 * control_ecu.c (TM4C123 Version)
 *
 *  Created on: December 17, 2024
 *      Author: amrem
 *  Description: Control ECU for Smart Door Lock System using TM4C123GH6PM
 */

#include "common_macros.h"
#include "Types.h"
#include "mcal/mcal_gpio.h"
#include "mcal/mcal_systick.h"
#include "mcal/mcal_gpt.h"
#include "hal/hal_eeprom.h"
#include "hal/hal_comm.h"
#include "hal/hal_motor.h"
#include "hal/hal_buzzer.h"
#include <stdint.h>

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define MAX_ATTEMPTS                2      /* Maximum number of allowed password attempts. Counting starts from 0 so its 3 attempts */
#define TIME_SYSTEMLOCK             60     /* Time to lock system after too many failed attempts */
#define TIME_TO_OPEN_CLOSE_DOOR     15     /* Time to keep door open/close */
#define PASSWORD_LENGTH             5      /* Length of password */
#define ENTER_KEY                   '#'    /* Enter key on keypad (using # for enter) */
#define CONTROL_ECU_READY           0X10   /* Indicator that Control ECU is ready */
#define ASK_AGAIN                   0x69   /* Command to prompt user for password again */
#define CHANGE_PASSWORD             0x68   /* Command to change password */
#define SET_PASSWORD                0x64   /* Command to set new password */
#define READ_PASSWORD               0x66   /* Command to read password */
#define DISPLAY_OPTIONS             0x65   /* Command to display options */
#define SET_TIMEOUT                 0x72   /* Command to set timeout using potentiometer */
#define OPEN_DOOR                   0x67   /* Command to open door */
#define ECU_CONFIRM                 0x15   /* Acknowledge command */
#define DISPLAY_ERROR               0x70   /* Command to display error */
#define SUCCESS                     1      /* Success indicator */
#define FAILURE                     0      /* Failure indicator */
#define HIMI_ECU_READY              0X11   /* Indicator that HMI ECU is ready */
#define EEPROM_PASS_ADDRESS         0x0000 /* EEPROM address for storing password (word-aligned) */

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/
void timerCallback(void);
void delayMs(uint32_t ms);

/*******************************************************************************
 *                              Global Variables                               *
 *******************************************************************************/
volatile uint8_t g_tick = 0;

/* Timer configuration */
Gpt_ConfigType timer_config = {
    .timer_InitialValue = 16000000,      /* 1 second at 16MHz with prescaler 1 */
    .timer_CompareValue = 0,
    .timer_ID = GPT_TIMER0A,
    .timer_mode = GPT_MODE_PERIODIC,
    .captureEdge = GPT_CAPTURE_EDGE_RISING,
    .enableInterrupt = 1
};

/*******************************************************************************
 *                              Main Function                                  *
 *******************************************************************************/
int main(void)
{
    uint8_t first_check[PASSWORD_LENGTH];       /* First entered password */
    uint8_t second_check[PASSWORD_LENGTH];      /* Re-entered password for verification */
    uint8_t pass_confirm[PASSWORD_LENGTH];      /* Password confirmation array */
    uint8_t command = SET_PASSWORD;              /* Initial command set to 'Set Password' */
    uint8_t pass_mismatch;                       /* Password mismatch flag */
    uint8_t error_count = 0;                     /* Password error count */
    uint8_t options = 0;                         /* Options variable for menu */
    uint8_t index;                               /* Loop index */

    /* Initialize all hardware modules */
    BUZZER_init();                       /* Initialize Buzzer */
    HAL_Motor_Init();                    /* Initialize DC Motor */
    HAL_COMM_Init();                     /* Initialize UART Communication */
    HAL_EEPROM_Init();                   /* Initialize EEPROM */
    
    /* Register timer callback */
    Gpt_SetCallBack(timerCallback, GPT_TIMER0A);

    /* System stability delay */
    delayMs(1000);

    /* Initialize handshake with HMI ECU */
    while (HAL_COMM_ReceiveByte() != HIMI_ECU_READY)
    {
        /* Wait for HMI ECU to be ready */
    }
    HAL_COMM_SendByte(CONTROL_ECU_READY);
    delayMs(100);

    while (1)
    {
        /* Send current command to HMI ECU */
        HAL_COMM_SendByte(command);

        switch (command)
        {
        case SET_PASSWORD:
            pass_mismatch = 0; /* Reset password mismatch flag */

            /* Receive first password entry */
            for (index = 0; index < PASSWORD_LENGTH; index++)
            {
                first_check[index] = HAL_COMM_ReceiveByte();
            }

            /* Receive second password entry */
            for (index = 0; index < PASSWORD_LENGTH; index++)
            {
                second_check[index] = HAL_COMM_ReceiveByte();
            }

            (void)HAL_COMM_ReceiveByte(); /* Receive and discard Enter key */

            /* Compare both password entries */
            for (index = 0; index < PASSWORD_LENGTH; index++)
            {
                if (first_check[index] != second_check[index])
                {
                    pass_mismatch = 1; /* Set mismatch flag if passwords differ */
                    break;
                }
            }

            /* Store password in EEPROM if entries match */
            if (pass_mismatch == 0)
            {
                HAL_EEPROM_StorePassword((const char *)first_check, PASSWORD_LENGTH);
                command = DISPLAY_OPTIONS; /* Move to options menu */
            }
            break;

        case DISPLAY_OPTIONS:
            options = HAL_COMM_ReceiveByte(); /* Receive selected option */

            if (options == 'A')
            {
                command = READ_PASSWORD; /* Command to read password (open door) */
            }
            else if (options == 'B')
            {
                command = CHANGE_PASSWORD; /* Command to change password */
            }
            else if (options == 'C')
            {
                command = SET_TIMEOUT; /* Command to set timeout */
            }
            break;

        case SET_TIMEOUT:
            /* Timeout is set on HMI side using potentiometer */
            /* Just acknowledge and return to options */
            command = DISPLAY_OPTIONS;
            break;

        case READ_PASSWORD:
            /* Receive password to confirm */
            for (index = 0; index < PASSWORD_LENGTH; index++)
            {
                pass_confirm[index] = HAL_COMM_ReceiveByte();
            }

            (void)HAL_COMM_ReceiveByte(); /* Receive and discard Enter key */

            /* Verify password using HAL function */
            if (HAL_EEPROM_VerifyPassword((const char *)pass_confirm, PASSWORD_LENGTH) == FALSE)
            {
                command = ASK_AGAIN;
            }
            else
            {
                command = OPEN_DOOR;
            }
            break;

        case OPEN_DOOR:
            /* Open door - motor forward */
            HAL_Motor_Move(MOTOR_FORWARD);
            
            /* Initialize and start timer */
            g_tick = 0;
            Gpt_Init(&timer_config);
            Gpt_Start(GPT_TIMER0A);
            
            while (g_tick < TIME_TO_OPEN_CLOSE_DOOR)
            {
                /* Wait for door to open */
            }
            
            g_tick = 0;
            Gpt_Stop(GPT_TIMER0A);
            Gpt_DeInit(GPT_TIMER0A);

            /* Stop motor */
            HAL_Motor_Move(MOTOR_STOP);
            
            /* Wait for 3 seconds before closing (simulating people entering) */
            delayMs(3000);

            /* Close door - motor backward */
            HAL_Motor_Move(MOTOR_BACKWARD);
            
            /* Initialize and start timer again */
            g_tick = 0;
            Gpt_Init(&timer_config);
            Gpt_Start(GPT_TIMER0A);
            
            while (g_tick < TIME_TO_OPEN_CLOSE_DOOR)
            {
                /* Wait for door to close */
            }
            
            g_tick = 0;
            Gpt_Stop(GPT_TIMER0A);
            Gpt_DeInit(GPT_TIMER0A);

            /* Stop motor */
            HAL_Motor_Move(MOTOR_STOP);
            command = DISPLAY_OPTIONS; /* Return to options */
            break;

        case CHANGE_PASSWORD:
            /* Receive password for verification */
            for (index = 0; index < PASSWORD_LENGTH; index++)
            {
                pass_confirm[index] = HAL_COMM_ReceiveByte();
            }

            (void)HAL_COMM_ReceiveByte(); /* Receive and discard Enter key */

            /* Verify password using HAL function */
            if (HAL_EEPROM_VerifyPassword((const char *)pass_confirm, PASSWORD_LENGTH) == FALSE)
            {
                command = ASK_AGAIN;
            }
            else
            {
                command = SET_PASSWORD; /* Move to set new password */
            }
            break;

        case ASK_AGAIN:
            error_count++;

            if (error_count >= MAX_ATTEMPTS)
            {
                command = DISPLAY_ERROR;
            }
            else
            {
                command = DISPLAY_OPTIONS;
            }
            break;

        case DISPLAY_ERROR:
            BUZZER_setState(BUZZER_ON);
            
            /* Initialize and start timer */
            g_tick = 0;
            Gpt_Init(&timer_config);
            Gpt_Start(GPT_TIMER0A);
            
            while (g_tick < TIME_SYSTEMLOCK)
            {
                /* System locked for 60 seconds */
            }
            
            g_tick = 0;
            Gpt_Stop(GPT_TIMER0A);
            Gpt_DeInit(GPT_TIMER0A);

            BUZZER_setState(BUZZER_OFF);
            error_count = 0;
            command = DISPLAY_OPTIONS;
            break;

        default:
            break;
        }
    }

    return 0;
}

/*******************************************************************************
 *                              Functions Definitions                          *
 *******************************************************************************/

/**
 * @brief Timer callback function - increments tick counter every second
 */
void timerCallback(void)
{
    g_tick++;
}

/**
 * @brief Simple delay function using SysTick
 * @param ms - delay time in milliseconds
 */
void delayMs(uint32_t ms)
{
    MCAL_SysTick_DelayMs(ms);
}
 