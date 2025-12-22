#include "hal/hal_motor.h"
#include "mcal/mcal_gpio.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"

/* Test variables */
bool motor_is_open = false;
bool motor_is_closed = true;

/*
 * Hardware Mapping:
 * ENA : PB6 (Now treated as GPIO)
 * IN1 : PB4
 * IN2 : PB5
 */
#define MOTOR_GPIO_PERIPH   SYSCTL_PERIPH_GPIOB
#define MOTOR_PORT_BASE     GPIO_PORTB_BASE
#define MOTOR_PIN_IN1       (1U << 4) // PB4
#define MOTOR_PIN_IN2       (1U << 5) // PB5
#define MOTOR_PIN_ENA       (1U << 6) // PB6

void HAL_Motor_Init(void)
{
    /* 1. Initialize ALL Pins as GPIO Output */
    MCAL_GPIO_EnablePort(MOTOR_GPIO_PERIPH);

    // Init PB4, PB5, and PB6 as Outputs
    MCAL_GPIO_InitPin(MOTOR_PORT_BASE, MOTOR_PIN_IN1 | MOTOR_PIN_IN2 | MOTOR_PIN_ENA,
                      GPIO_DIR_OUTPUT, GPIO_ATTACH_DEFAULT);

    /* 2. Ensure Everything is OFF initially */
    MCAL_GPIO_WritePin(MOTOR_PORT_BASE, MOTOR_PIN_ENA, LOGIC_LOW);
    MCAL_GPIO_WritePin(MOTOR_PORT_BASE, MOTOR_PIN_IN1, LOGIC_LOW);
    MCAL_GPIO_WritePin(MOTOR_PORT_BASE, MOTOR_PIN_IN2, LOGIC_LOW);
}

void HAL_Motor_Move(Motor_DirType direction)
{
    switch(direction)
    {
        case MOTOR_FORWARD:
            // Direction Setup
            MCAL_GPIO_WritePin(MOTOR_PORT_BASE, MOTOR_PIN_IN1, MOTOR_PIN_IN1); // High
            MCAL_GPIO_WritePin(MOTOR_PORT_BASE, MOTOR_PIN_IN2, LOGIC_LOW);     // Low
            // Enable Power (Full Speed)
            MCAL_GPIO_WritePin(MOTOR_PORT_BASE, MOTOR_PIN_ENA, MOTOR_PIN_ENA); // High
            motor_is_open = true;
            motor_is_closed = false;
            break;

        case MOTOR_BACKWARD:
            // Direction Setup
            MCAL_GPIO_WritePin(MOTOR_PORT_BASE, MOTOR_PIN_IN1, LOGIC_LOW);     // Low
            MCAL_GPIO_WritePin(MOTOR_PORT_BASE, MOTOR_PIN_IN2, MOTOR_PIN_IN2); // High
            // Enable Power (Full Speed)
            MCAL_GPIO_WritePin(MOTOR_PORT_BASE, MOTOR_PIN_ENA, MOTOR_PIN_ENA); // High
            motor_is_open = false;
            motor_is_closed = true;
            break;
            break;

        case MOTOR_STOP:
        default:
            // Cut Power to Enable Pin
            MCAL_GPIO_WritePin(MOTOR_PORT_BASE, MOTOR_PIN_ENA, LOGIC_LOW);

            // Optional: Ground inputs to brake
            MCAL_GPIO_WritePin(MOTOR_PORT_BASE, MOTOR_PIN_IN1, LOGIC_LOW);
            MCAL_GPIO_WritePin(MOTOR_PORT_BASE, MOTOR_PIN_IN2, LOGIC_LOW);
            break;
    }
}