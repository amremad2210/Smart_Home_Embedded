//
// Created by Hagar Elhishi on 14/12/2025.
//

#ifndef MCAL_PWM_H_
#define MCAL_PWM_H_

#include "Types.h"
#include <stdint.h>

/* ===========================
 *   Types
 * =========================== */

/*
 * Map logical channels to physical pins.
 * Currently configured for PB6 (M0PWM0).
 * You can add more (e.g., PWM_CHANNEL_PB7) later.
 */
typedef enum {
    PWM_CHANNEL_PB6,
    PWM_NUM_CHANNELS
} Pwm_ChannelType;

/* Configuration Struct */
typedef struct {
    Pwm_ChannelType channel_ID;     /* Which Pin/Module? */
    uint32_t        frequency_hz;   /* Desired Frequency (e.g., 1000Hz) */
    uint8_t         initial_duty;   /* Initial Duty Cycle 0-100 */
} Pwm_ConfigType;

/* ===========================
 *   Function Definitions
 * =========================== */

/* Initialize PWM based on config struct */
void MCAL_Pwm_Init(const Pwm_ConfigType *Config_Ptr);

/* Start the PWM Signal */
void MCAL_Pwm_Start(Pwm_ChannelType channel_ID);

/* Stop the PWM Signal */
void MCAL_Pwm_Stop(Pwm_ChannelType channel_ID);

/* Update Duty Cycle (0 - 100) */
void MCAL_Pwm_SetDuty(Pwm_ChannelType channel_ID, uint8_t dutyCycle);

#endif /* MCAL_PWM_H_ */