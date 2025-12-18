/******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.c
 *
 * Description: Source file for the Buzzer driver
 *
 *******************************************************************************/

#include "hal/hal_buzzer.h"
#include "mcal/mcal_gpio.h"    // Use the new MCAL GPIO driver
#include "mcal/mcal_systick.h" // Use the new MCAL SysTick driver for dela
#include "Types.h"

void BUZZER_init(void)
{
    /* 1. Enable clock for the specific port using the MCAL driver */
    MCAL_GPIO_EnablePort(BUZZER_PERIPH);

    /* 2. Set buzzer pin as OUTPUT with Default attachment */
    MCAL_GPIO_InitPin(BUZZER_PORT_BASE, BUZZER_PIN, GPIO_DIR_OUTPUT, GPIO_ATTACH_DEFAULT);

    /* 3. Ensure starting state is OFF */
    MCAL_GPIO_WritePin(BUZZER_PORT_BASE, BUZZER_PIN, LOGIC_LOW);
}

//This function translates the logic ("On" or "Off") into physical voltage 3ashan ye-weee waaaaaa
void BUZZER_setState(uint8_t state)
{
    if (state == BUZZER_ON)
    {
        /* Write HIGH to the defined Port/Pin */
        MCAL_GPIO_WritePin(BUZZER_PORT_BASE, BUZZER_PIN, LOGIC_HIGH);
    }
    else
    {
        /* Write LOW (0) to the defined Port/Pin */
        MCAL_GPIO_WritePin(BUZZER_PORT_BASE, BUZZER_PIN, LOGIC_LOW);
    }
}

//This function creates a sound for a specific amount of time. It involves a "Blocking Delay."
void BUZZER_beep(uint32_t duration_ms)
{
    /* Turn ON el weeee waaaaaaaa*/
    BUZZER_setState(BUZZER_ON);
    
    /* Blocking Delay using MCAL SysTick */
    MCAL_SysTick_DelayMs(duration_ms);
    
    /* Turn OFF el weeeeee waaaaaaaaaa*/
    BUZZER_setState(BUZZER_OFF);
}