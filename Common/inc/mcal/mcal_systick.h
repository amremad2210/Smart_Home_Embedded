/*============================================================================
 *  Module      : MCAL SysTick
 *  File Name   : mcal_systick.h
 *  Description : SysTick abstraction layer using TivaWare driverlib
 *===========================================================================*/

#ifndef MCAL_SYSTICK_H_
#define MCAL_SYSTICK_H_

#include <stdint.h>

/* Callback type called from SysTick ISR */
typedef void (*SysTick_CallbackType)(void);

/**
 * @brief Initialize SysTick with 1 ms tick based on current system clock.
 *
 * - Reads system clock using SysCtlClockGet()
 * - Configures SysTick to generate an interrupt every 1 ms
 * - Clears tick counter
 * - Starts SysTick
 */
void MCAL_SysTick_Init(void);

/**
 * @brief Start SysTick timer and interrupt.
 */
void MCAL_SysTick_Start(void);

/**
 * @brief Stop SysTick timer and interrupt.
 */
void MCAL_SysTick_Stop(void);

/**
 * @brief Set callback to be called at every SysTick interrupt.
 *
 * Passing NULL disables the callback.
 *
 * @param cb  Pointer to function to call each 1 ms, or NULL.
 */
void MCAL_SysTick_SetCallback(SysTick_CallbackType cb);

/**
 * @brief Get current millisecond tick count.
 *
 * @return Milliseconds since MCAL_SysTick_Init() (wraps around).
 */
uint32_t MCAL_SysTick_GetTickMs(void);

/**
 * @brief Blocking delay using SysTick millisecond counter.
 *
 * @param ms  Number of milliseconds to wait.
 */
void MCAL_SysTick_DelayMs(uint32_t ms);

/**
 * @brief Get the system clock in Hz as used by SysTick.
 *
 * @return System clock in Hz.
 */
uint32_t MCAL_SysTick_GetClockHz(void);

#endif /* MCAL_SYSTICK_H_ */
