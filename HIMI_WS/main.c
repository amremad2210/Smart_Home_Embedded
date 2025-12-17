/*
 * himi_ecu.c (TM4C123 Version)
 *
 *  Created on: December 17, 2024
 *      Author: amrem
 *  Description: HMI ECU for Smart Door Lock System using TM4C123GH6PM
 */

#include "common_macros.h"
#include "Types.h"
#include "mcal/mcal_gpio.h"
#include "mcal/mcal_systick.h"
#include "mcal/mcal_gpt.h"
#include "hal/hal_lcd.h"
#include "hal/hal_keypad.h"
#include "hal/hal_comm.h"
#include "hal/hal_potentiometer.h"
#include <stdint.h>

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define MAX_ATTEMPTS                2      /* Maximum number of allowed password attempts. Counting starts from 0 so its 3 attempts */
#define TIME_SYSTEMLOCK             60     /* Time to lock system after too many failed attempts */
#define TIME_TO_OPEN_CLOSE_DOOR     15     /* Default time to keep door open/close */
#define PASSWORD_LENGTH             5      /* Length of password */
#define ENTER_KEY                   '#'    /* Enter key on keypad (using # for enter) */
#define CONTROL_ECU_READY           0X10   /* Indicator that Control ECU is ready */
#define ASK_AGAIN                   0x69   /* Command to prompt user for password again */
#define CHANGE_PASSWORD             0x68   /* Command to change password */
#define SET_PASSWORD                0x64   /* Command to set new password */
#define READ_PASSWORD               0x66   /* Command to read password */
#define DISPLAY_OPTIONS             0x65   /* Command to display options */
#define SET_TIMEOUT                 0x72   /* Command to set timeout using potentiometer */
#define PIR_SIGNAL                  0x71   /* Command to indicate PIR signal detected */
#define OPEN_DOOR                   0x67   /* Command to open door */
#define ECU_CONFIRM                 0x15   /* Acknowledge command */
#define DISPLAY_ERROR               0x70   /* Command to display error */
#define SUCCESS                     1      /* Success indicator */
#define FAILURE                     0      /* Failure indicator */
#define HIMI_ECU_READY              0X11   /* Indicator that HMI ECU is ready */

/* Potentiometer settings for timeout range */
#define POT_MIN_TIMEOUT             5      /* Minimum door timeout in seconds */
#define POT_MAX_TIMEOUT             30     /* Maximum door timeout in seconds */

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/
void timerCallback(void);
void delayMs(uint32_t ms);
uint8_t getPotentiometerTimeout(void);

/*******************************************************************************
 *                              Global Variables                               *
 *******************************************************************************/
volatile uint8_t g_tick = 0;
volatile uint8_t g_wrongtries = 0;

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
    uint8_t first_check = 0;
    uint8_t second_check = 0;
    uint8_t pass_correct = 0;
    uint8_t key = 0;
    uint8_t option_select;
    uint8_t command = 0;
    uint8_t index;
    uint8_t door_timeout;

    /* Initialize all hardware modules */
    Lcd_Init();                          /* Initialize LCD */
    HAL_Keypad_Init();                   /* Initialize Keypad */
    HAL_COMM_Init();                     /* Initialize UART Communication */
    POT_Init();                          /* Initialize Potentiometer */
    
    /* Register timer callback */
    Gpt_SetCallBack(timerCallback, GPT_TIMER0A);

    /* Initialize handshake with Control ECU */
    Lcd_Clear();
    Lcd_DisplayString("System Init...");
    delayMs(2000);
    
    HAL_COMM_SendByte(HIMI_ECU_READY);
    while (HAL_COMM_ReceiveByte() != CONTROL_ECU_READY) {
        /* Wait for Control ECU to be ready */
    }
    delayMs(100);
    Lcd_Clear();

    while (1)
    {
        /* Wait for command from Control ECU */
        command = HAL_COMM_ReceiveByte();
        delayMs(50);

        switch (command)
        {
        case SET_PASSWORD:
            Lcd_Clear();
            Lcd_DisplayString("Plz enter Pass:");
            Lcd_GoToRowColumn(1, 0);
            
            for (index = 0; index < PASSWORD_LENGTH; index++)
            {
                first_check = HAL_Keypad_GetKey();
                HAL_COMM_SendByte(first_check);
                Lcd_DisplayCharacter('*');
                delayMs(300);
            }

            key = HAL_Keypad_GetKey();
            delayMs(300);

            while (key != ENTER_KEY)
            {
                key = HAL_Keypad_GetKey();
                delayMs(300);
            }

            Lcd_Clear();
            Lcd_DisplayString("Plz re-enter:");
            Lcd_GoToRowColumn(1, 0);
            Lcd_DisplayString("same pass:");

            for (index = 0; index < PASSWORD_LENGTH; index++)
            {
                second_check = HAL_Keypad_GetKey();
                HAL_COMM_SendByte(second_check);
                Lcd_DisplayCharacter('*');
                delayMs(300);
            }
            
            key = HAL_Keypad_GetKey();
            delayMs(300);

            while (key != ENTER_KEY)
            {
                key = HAL_Keypad_GetKey();
                delayMs(300);
            }

            HAL_COMM_SendByte(key);
            break;

        case DISPLAY_OPTIONS:
            Lcd_Clear();
            Lcd_DisplayString("A:Open B:Change");
            Lcd_GoToRowColumn(1, 0);
            Lcd_DisplayString("C:Set Timeout");

            option_select = HAL_Keypad_GetKey();
            HAL_COMM_SendByte(option_select);
            break;

        case SET_TIMEOUT:
            Lcd_Clear();
            Lcd_DisplayString("Adjust POT for");
            Lcd_GoToRowColumn(1, 0);
            Lcd_DisplayString("timeout (5-30s)");
            delayMs(2000);
            
            /* Read and display current timeout value */
            door_timeout = getPotentiometerTimeout();
            Lcd_Clear();
            Lcd_DisplayString("Timeout Set:");
            Lcd_GoToRowColumn(1, 0);
            
            if (door_timeout >= 10)
            {
                Lcd_DisplayCharacter('0' + (door_timeout / 10));
                Lcd_DisplayCharacter('0' + (door_timeout % 10));
            }
            else
            {
                Lcd_DisplayCharacter('0' + door_timeout);
            }
            Lcd_DisplayString(" seconds");
            delayMs(2000);
            break;

        case READ_PASSWORD:
            Lcd_Clear();
            Lcd_DisplayString("Plz enter old");
            Lcd_GoToRowColumn(1, 0);
            Lcd_DisplayString("pass:");

            for (index = 0; index < PASSWORD_LENGTH; index++)
            {
                pass_correct = HAL_Keypad_GetKey();
                HAL_COMM_SendByte(pass_correct);
                Lcd_DisplayCharacter('*');
                delayMs(300);
            }

            key = HAL_Keypad_GetKey();
            delayMs(300);

            while (key != ENTER_KEY)
            {
                key = HAL_Keypad_GetKey();
                delayMs(300);
            }

            HAL_COMM_SendByte(key);
            break;

        case OPEN_DOOR:
            /* Read potentiometer to determine door timeout */
            door_timeout = getPotentiometerTimeout();
            
            Lcd_Clear();
            Lcd_DisplayString("Door Unlocking..");
            Lcd_GoToRowColumn(1, 0);
            Lcd_DisplayString("Timeout: ");
            
            /* Display timeout value */
            if (door_timeout >= 10)
            {
                Lcd_DisplayCharacter('0' + (door_timeout / 10));
                Lcd_DisplayCharacter('0' + (door_timeout % 10));
            }
            else
            {
                Lcd_DisplayCharacter('0' + door_timeout);
            }
            Lcd_DisplayCharacter('s');
            
            /* Initialize and start timer */
            g_tick = 0;
            Gpt_Init(&timer_config);
            Gpt_Start(GPT_TIMER0A);
            
            while (g_tick < door_timeout)
            {
                /* Wait for door to open */
            }
            
            g_tick = 0;
            Gpt_Stop(GPT_TIMER0A);
            Gpt_DeInit(GPT_TIMER0A);

            Lcd_Clear();
            Lcd_DisplayString("Wait for people");
            Lcd_GoToRowColumn(1, 0);
            Lcd_DisplayString("to enter");

            /* Wait for PIR signal */
            while (HAL_COMM_ReceiveByte() != PIR_SIGNAL)
            {
                /* Wait for people to enter */
            }

            /* Read potentiometer again for closing timeout */
            door_timeout = getPotentiometerTimeout();
            
            Lcd_Clear();
            Lcd_DisplayString("Door Locking..");
            Lcd_GoToRowColumn(1, 0);
            Lcd_DisplayString("Timeout: ");
            
            /* Display timeout value */
            if (door_timeout >= 10)
            {
                Lcd_DisplayCharacter('0' + (door_timeout / 10));
                Lcd_DisplayCharacter('0' + (door_timeout % 10));
            }
            else
            {
                Lcd_DisplayCharacter('0' + door_timeout);
            }
            Lcd_DisplayCharacter('s');
            
            /* Initialize and start timer again */
            g_tick = 0;
            Gpt_Init(&timer_config);
            Gpt_Start(GPT_TIMER0A);
            
            while (g_tick < door_timeout)
            {
                /* Wait for door to close */
            }
            
            g_tick = 0;
            Gpt_Stop(GPT_TIMER0A);
            Gpt_DeInit(GPT_TIMER0A);
            break;

        case CHANGE_PASSWORD:
            Lcd_Clear();
            Lcd_DisplayString("Plz enter old");
            Lcd_GoToRowColumn(1, 0);
            Lcd_DisplayString("pass:");

            for (index = 0; index < PASSWORD_LENGTH; index++)
            {
                pass_correct = HAL_Keypad_GetKey();
                HAL_COMM_SendByte(pass_correct);
                Lcd_DisplayCharacter('*');
                delayMs(300);
            }

            key = HAL_Keypad_GetKey();
            delayMs(300);

            while (key != ENTER_KEY)
            {
                key = HAL_Keypad_GetKey();
                delayMs(300);
            }

            HAL_COMM_SendByte(key);
            break;

        case ASK_AGAIN:
            Lcd_Clear();
            Lcd_DisplayString("Plz enter old");
            Lcd_GoToRowColumn(1, 0);
            Lcd_DisplayString("pass:");

            for (index = 0; index < PASSWORD_LENGTH; index++)
            {
                pass_correct = HAL_Keypad_GetKey();
                HAL_COMM_SendByte(pass_correct);
                Lcd_DisplayCharacter('*');
                delayMs(300);
            }

            key = HAL_Keypad_GetKey();
            delayMs(300);

            while (key != ENTER_KEY)
            {
                key = HAL_Keypad_GetKey();
                delayMs(300);
            }

            HAL_COMM_SendByte(key);
            break;

        case DISPLAY_ERROR:
            Lcd_Clear();
            Lcd_DisplayString("ERROR!");
            Lcd_GoToRowColumn(1, 0);
            Lcd_DisplayString("SYSTEM LOCKED");
            
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
            break;

        default:
            Lcd_Clear();
            Lcd_DisplayString("CODE ERROR");
            delayMs(2000);
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

/**
 * @brief Reads potentiometer and maps value to timeout range
 * @return Timeout value in seconds (POT_MIN_TIMEOUT to POT_MAX_TIMEOUT)
 */
uint8_t getPotentiometerTimeout(void)
{
    return (uint8_t)POT_ReadMapped(POT_MIN_TIMEOUT, POT_MAX_TIMEOUT);
}
