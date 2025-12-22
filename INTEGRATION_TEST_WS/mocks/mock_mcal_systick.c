/*============================================================================
 *  Mock MCAL SysTick for Integration Testing
 *===========================================================================*/

#include <stdint.h>
#include <stddef.h>
#include "mcal/mcal_systick.h"

/* Mock SysTick */
static volatile uint32_t mock_systick_counter = 0;
static SysTick_CallbackType mock_callback = NULL;

void systick_ISR(void) {
    mock_systick_counter++;
    if (mock_callback != NULL) {
        mock_callback();
    }
}

void MCAL_SysTick_Init(void) {
    /* No-op */
}

void MCAL_SysTick_Start(void) {
    /* No-op */
}

void MCAL_SysTick_Stop(void) {
    /* No-op */
}

void MCAL_SysTick_SetCallback(SysTick_CallbackType cb) {
    mock_callback = cb;
}

uint32_t MCAL_SysTick_GetTickMs(void) {
    return mock_systick_counter;
}

void MCAL_SysTick_DelayMs(uint32_t ms) {
    mock_systick_counter += ms;
}

uint32_t MCAL_SysTick_GetClockHz(void) {
    return 16000000; /* Mock */
}