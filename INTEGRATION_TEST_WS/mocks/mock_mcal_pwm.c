/*============================================================================
 *  Mock MCAL PWM for Integration Testing
 *===========================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include "mcal/mcal_pwm.h"

void MCAL_Pwm_Init(const Pwm_ConfigType *Config_Ptr) {
    /* No-op */
}

void MCAL_Pwm_Start(Pwm_ChannelType channel_ID) {
    /* No-op */
}

void MCAL_Pwm_Stop(Pwm_ChannelType channel_ID) {
    /* No-op */
}

void MCAL_Pwm_SetDuty(Pwm_ChannelType channel_ID, uint8_t dutyCycle) {
    /* No-op */
}