/*============================================================================
 *  Module      : System
 *  File Name   : system.c
 *  Description : Basic time services (SysTick) without changing system clock
 *===========================================================================*/

#include "system.h"

#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

/*======================================================================
 *  Private data
 *====================================================================*/

static volatile uint32_t g_sysTickMs = 0U;
static uint32_t          g_sysClkHz  = 0U;

/*======================================================================
 *  SysTick ISR
 *
 *  NOTE: startup_ewarm.c declares:
 *      extern void systick_ISR(void);
 *  and places systick_ISR in the vector table.
 *
 *  So the name and prototype MUST match exactly.
 *====================================================================*/

void systick_ISR(void)
{
    g_sysTickMs++;
}

/*======================================================================
 *  Public API
 *====================================================================*/

void System_Init(void)
{
    /*------------------------------------------------------------------
     * 1) Read current system clock
     *------------------------------------------------------------------
     * This does NOT change any clock settings; it only reads them.
     *------------------------------------------------------------------*/
    g_sysClkHz = SysCtlClockGet();

    /*------------------------------------------------------------------
     * 2) Configure SysTick for 1 ms tick
     *------------------------------------------------------------------*/
    SysTickDisable();
    SysTickIntDisable();

    if (g_sysClkHz == 0U)
    {
        /* Fallback: avoid division by zero, assume 16 MHz */
        g_sysClkHz = 16000000U;
    }

    /* Load value = (clock / 1000) ? 1 ms period */
    SysTickPeriodSet(g_sysClkHz / 1000U);

    /*
     * We DON'T call SysTickIntRegister() because the vector table
     * already points to systick_ISR in startup_ewarm.c.
     */
    SysTickIntEnable();
    SysTickEnable();
}

uint32_t System_GetCoreClockHz(void)
{
    return g_sysClkHz;
}

uint32_t System_GetTickMs(void)
{
    return g_sysTickMs;
}

void System_DelayMs(uint32_t ms)
{
    uint32_t start = g_sysTickMs;

    /* Wrap-around safe: subtraction in unsigned arithmetic */
    while ((g_sysTickMs - start) < ms)
    {
        /* busy wait */
    }
}
