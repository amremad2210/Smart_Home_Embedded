#ifndef HAL_MOTOR_H_
#define HAL_MOTOR_H_

#include "Types.h"
#include <stdint.h>

/* Motor Direction Logic */
typedef enum {
    MOTOR_STOP,
    MOTOR_FORWARD,
    MOTOR_BACKWARD
} Motor_DirType;

/* Initialize Motor pins (All GPIOs) */
void HAL_Motor_Init(void);

/* Control Motor Direction (Speed is always 100%) */
void HAL_Motor_Move(Motor_DirType direction);

#endif /* HAL_MOTOR_H_ */