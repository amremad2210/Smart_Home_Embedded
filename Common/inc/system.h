/*============================================================================
 *  Module      : System
 *  File Name   : system.h
 *  Description : Basic time services (SysTick) without changing system clock
 *===========================================================================*/

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stdint.h>

/**
 * @brief Initialize SysTick timer based on current system clock.
 *
 * - Reads current system clock using SysCtlClockGet().
 * - Configures SysTick to generate an interrupt every 1 ms.
 * - Starts the SysTick timer.
 * - Does NOT modify the system clock configuration.
 */
void System_Init(void);

/**
 * @brief Get the current system core clock frequency in Hz.
 *
 * @return System clock in Hz (as reported by SysCtlClockGet()).
 */
uint32_t System_GetCoreClockHz(void);

/**
 * @brief Get the current millisecond tick count.
 *
 * This value increments every 1 ms in the SysTick ISR.
 *
 * @return Milliseconds since System_Init() was called (wraps around).
 */
uint32_t System_GetTickMs(void);

/**
 * @brief Busy-wait delay for a given number of milliseconds.
 *
 * Uses the SysTick millisecond counter. This is blocking.
 *
 * @param ms  Number of milliseconds to delay.
 */
void System_DelayMs(uint32_t ms);

#endif /* SYSTEM_H_ */
