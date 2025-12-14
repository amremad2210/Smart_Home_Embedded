//placeholder for git
#include "hal/hal_motor.h"
#include "mcal/mcal_gpio.h"
#include "mcal/mcal_pwm.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"

/*
 * Hardware Mapping based on your schematic:
 * ENA (Speed): PB6 (PWM)
 * IN1: PB4
 * IN2: PB5
 */
#define MOTOR_GPIO_PERIPH   SYSCTL_PERIPH_GPIOB
#define MOTOR_PORT_BASE     GPIO_PORTB_BASE
#define MOTOR_PIN_IN1       (1U << 4) // PB4
#define MOTOR_PIN_IN2       (1U << 5) // PB5

void HAL_Motor_Init(void)
{
    /* 1. Initialize GPIO Pins for Direction (IN1, IN2) */
    MCAL_GPIO_EnablePort(MOTOR_GPIO_PERIPH);

    MCAL_GPIO_InitPin(MOTOR_PORT_BASE, MOTOR_PIN_IN1 | MOTOR_PIN_IN2,
                      GPIO_DIR_OUTPUT, GPIO_ATTACH_DEFAULT);

    /* Ensure Stopped initially */
    MCAL_GPIO_WritePin(MOTOR_PORT_BASE, MOTOR_PIN_IN1, LOGIC_LOW);
    MCAL_GPIO_WritePin(MOTOR_PORT_BASE, MOTOR_PIN_IN2, LOGIC_LOW);

    /* 2. Initialize PWM for Speed (ENA on PB6) */
    Pwm_ConfigType motorPwmConfig;
    motorPwmConfig.channel_ID   = PWM_CHANNEL_PB6;
    motorPwmConfig.frequency_hz = 1000; /* 1 kHz is good for L298N */
    motorPwmConfig.initial_duty = 0;    /* Start at 0 speed */

    MCAL_Pwm_Init(&motorPwmConfig);

    /* Enable the PWM output immediately (speed 0 keeps it stopped) */
    MCAL_Pwm_Start(PWM_CHANNEL_PB6);
}

void HAL_Motor_Move(Motor_DirType direction, uint8_t speed)
{
    /* 1. Set Direction Pins */
    switch(direction)
    {
        case MOTOR_FORWARD:
            MCAL_GPIO_WritePin(MOTOR_PORT_BASE, MOTOR_PIN_IN1, MOTOR_PIN_IN1); // High
            MCAL_GPIO_WritePin(MOTOR_PORT_BASE, MOTOR_PIN_IN2, LOGIC_LOW);     // Low
            break;

        case MOTOR_BACKWARD:
            MCAL_GPIO_WritePin(MOTOR_PORT_BASE, MOTOR_PIN_IN1, LOGIC_LOW);     // Low
            MCAL_GPIO_WritePin(MOTOR_PORT_BASE, MOTOR_PIN_IN2, MOTOR_PIN_IN2); // High
            break;

        case MOTOR_STOP:
        default:
            MCAL_GPIO_WritePin(MOTOR_PORT_BASE, MOTOR_PIN_IN1, LOGIC_LOW);
            MCAL_GPIO_WritePin(MOTOR_PORT_BASE, MOTOR_PIN_IN2, LOGIC_LOW);
            break;
    }

    /* 2. Set Speed (PWM Duty Cycle) */
    MCAL_Pwm_SetDuty(PWM_CHANNEL_PB6, speed);
}