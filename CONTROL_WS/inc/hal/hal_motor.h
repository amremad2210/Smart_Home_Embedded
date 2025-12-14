//placeholder for git
#ifndef HAL_MOTOR_H_
#define HAL_MOTOR_H_

#include "Types.h"
#include <stdint.h>


#define MOTOR_GPIO_PERIPH   SYSCTL_PERIPH_GPIOB
#define MOTOR_PORT_BASE     GPIO_PORTB_BASE
#define MOTOR_PIN_IN1       (1U << 4) // PB4
#define MOTOR_PIN_IN2       (1U << 5) // PB5


/* Motor Direction Logic */
typedef enum {
    MOTOR_STOP,
    MOTOR_FORWARD,
    MOTOR_BACKWARD
} Motor_DirType;

/* Initialize Motor pins and PWM */
void HAL_Motor_Init(void);

/* Control Motor: Direction and Speed (0-100) */
void HAL_Motor_Move(Motor_DirType direction, uint8_t speed);

#endif /* HAL_MOTOR_H_ */