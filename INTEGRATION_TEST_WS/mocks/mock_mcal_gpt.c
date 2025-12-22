/*============================================================================
 *  Mock MCAL GPT for Integration Testing
 *===========================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "mcal/mcal_gpt.h"

static Gpt_CallbackType mock_callbacks[GPT_NUM_TIMERS] = {NULL};

void Gpt_Init(const Gpt_ConfigType *Config_Ptr) {
    /* No-op */
}

void Gpt_DeInit(Gpt_IDType timer_ID) {
    /* No-op */
}

void Gpt_SetCallBack(Gpt_CallbackType cb, Gpt_IDType timer_ID) {
    if (timer_ID < GPT_NUM_TIMERS) {
        mock_callbacks[timer_ID] = cb;
    }
}

void Gpt_Start(Gpt_IDType timer_ID) {
    /* No-op */
}

void Gpt_Stop(Gpt_IDType timer_ID) {
    /* No-op */
}

void Gpt_SetPwmDuty(Gpt_IDType timer_ID, float dutyPercent) {
    /* No-op */
}

uint32_t Gpt_GetCaptureValue(Gpt_IDType timer_ID) {
    return 0;
}

void Timer0A_Handler(void) {
    if (mock_callbacks[GPT_TIMER0A]) {
        mock_callbacks[GPT_TIMER0A]();
    }
}

void WTimer2A_Handler(void) {
    if (mock_callbacks[GPT_WTIMER2A]) {
        mock_callbacks[GPT_WTIMER2A]();
    }
}