//
// Created by Hagar Elhishi on 14/12/2025.
//

#include "mcal/mcal_pwm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "mcal/mcal_gpio.h"

/* Helper to store the Period Load value for duty cycle calculations */
static uint32_t g_pwmLoadValue = 0;

void MCAL_Pwm_Init(const Pwm_ConfigType *Config_Ptr)
{
    /* 1. Enable PWM Clock */
    /* Note: Tiva C PWM has a clock divider. We set it to divide by 64
       to allow lower frequencies if needed, or leave it SYSCTL_PWMDIV_1
       for high resolution. Let's use /1 for 1khz precision */
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);

    switch(Config_Ptr->channel_ID)
    {
        case PWM_CHANNEL_PB6:
        {
            /* Enable Peripherals */

            MCAL_GPIO_EnablePort(SYSCTL_PERIPH_GPIOB);
            MCAL_GPIO_EnablePort(SYSCTL_PERIPH_PWM0);

            /* Configure Pin Muxing for PB6 as M0PWM0 */
            GPIOPinConfigure(GPIO_PB6_M0PWM0);
            GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6);

            /* Configure Generator 0 (which controls Output 0 and 1) */
            /* Count Down mode is standard for motor control */
            PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

            /* Calculate Period (Load Value) */
            /* Formula: PWM_Clock / Target_Freq */
            uint32_t pwmClock = SysCtlClockGet();
            g_pwmLoadValue = (pwmClock / Config_Ptr->frequency_hz) - 1;

            PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, g_pwmLoadValue);

            /* Set Initial Duty Cycle */
            MCAL_Pwm_SetDuty(PWM_CHANNEL_PB6, Config_Ptr->initial_duty);

            /* Enable the Generator */
            PWMGenEnable(PWM0_BASE, PWM_GEN_0);
            break;
        }
        default:
            break;
    }
}

void MCAL_Pwm_SetDuty(Pwm_ChannelType channel_ID, uint8_t dutyCycle)
{
    /* Clamp duty cycle to 0-100 */
    if(dutyCycle > 100) dutyCycle = 100;

    /* Calculate Width */
    /* Width = (Period * Duty) / 100 */
    /* Minimum width 1 is safer for some hardware configurations */
    uint32_t width = (g_pwmLoadValue * dutyCycle) / 100;

    if (width < 1) width = 1;
    if (width > g_pwmLoadValue) width = g_pwmLoadValue - 1;

    switch(channel_ID)
    {
        case PWM_CHANNEL_PB6:
            /* PB6 is M0PWM0 (Output 0) */
            PWMPulseWidthSet(PWM0_BASE, PWM_OUTPUT_0, width);
            break;
        default:
            break;
    }
}

void MCAL_Pwm_Start(Pwm_ChannelType channel_ID)
{
    switch(channel_ID)
    {
        case PWM_CHANNEL_PB6:
            /* Enable Output 0 */
            PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);
            break;
        default:
            break;
    }
}

void MCAL_Pwm_Stop(Pwm_ChannelType channel_ID)
{
    switch(channel_ID)
    {
        case PWM_CHANNEL_PB6:
            PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, false);
            break;
        default:
            break;
    }
}