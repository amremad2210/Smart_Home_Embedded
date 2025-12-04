#ifndef MCAL_GPT_H_
#define MCAL_GPT_H_

#include <stdint.h>
#include <stdbool.h>


/* ===========================
 *   Types 
 * =========================== */

typedef enum {
    GPT_TIMER0A,
    GPT_WTIMER2A,
    GPT_NUM_TIMERS
} Gpt_IDType;

typedef enum {
    GPT_MODE_ONE_SHOT,
    GPT_MODE_PERIODIC,
    GPT_MODE_CAPTURE,
    GPT_MODE_PWM
} Gpt_ModeType;

typedef enum {
    GPT_CAPTURE_EDGE_RISING,
    GPT_CAPTURE_EDGE_FALLING,
    GPT_CAPTURE_EDGE_BOTH
} Gpt_CaptureEdgeType;

/* Config struct */
typedef struct {
    uint32_t             timer_InitialValue;   /* LOAD / period in ticks */
    uint32_t             timer_CompareValue;   /* MATCH value (PWM duty, compare) */
    Gpt_IDType           timer_ID;             /* GPT_TIMER0A / GPT_WTIMER2A */
    Gpt_ModeType         timer_mode;           /* ONE_SHOT / PERIODIC / CAPTURE / PWM */
    Gpt_CaptureEdgeType  captureEdge;          /* Used only in CAPTURE mode */
    uint8_t              enableInterrupt;      /* 0 or 1 */
} Gpt_ConfigType;

/* Callback type */
typedef void (*Gpt_CallbackType)(void);

/* ===========================
 *   Function Definitions
 * =========================== */

/* Initialize one timer according to Config_Ptr */
void Gpt_Init(const Gpt_ConfigType *Config_Ptr);

/* De-initialize (stop + disable interrupts) */
void Gpt_DeInit(Gpt_IDType timer_ID);

/* Register callback for a timer */
void Gpt_SetCallBack(Gpt_CallbackType cb, Gpt_IDType timer_ID);

/* Start / stop a timer (does not reconfigure) */
void Gpt_Start(Gpt_IDType timer_ID);
void Gpt_Stop(Gpt_IDType timer_ID);

/* Extra helpers for modes we care about */

/* For PWM mode: update duty cycle (0–100 %) */
void Gpt_SetPwmDuty(Gpt_IDType timer_ID, float dutyPercent);

/* For capture mode: read captured value */
uint32_t Gpt_GetCaptureValue(Gpt_IDType timer_ID);



#endif /* MCAL_GPT_H_ */
