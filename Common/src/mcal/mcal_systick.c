/*============================================================================
 *  Module      : MCAL SysTick
 *  File Name   : mcal_systick.c
 *  Description : SysTick abstraction layer using TivaWare driverlib
 *===========================================================================*/

#include "mcal/mcal_systick.h"

#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

/*======================================================================
 *  Private data
 *====================================================================*/

static volatile uint32_t       g_systickMs   = 0U;
static uint32_t                g_sysClkHz    = 0U;
static SysTick_CallbackType    g_systickCb   = (SysTick_CallbackType)0;

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
    g_systickMs++;

    if (g_systickCb != (SysTick_CallbackType)0)
    {
        g_systickCb();
    }
}

/*======================================================================
 *  Public API
 *====================================================================*/

void MCAL_SysTick_Init(void)
{
    /* Read current system clock (does NOT change it) */
    g_sysClkHz = SysCtlClockGet();
    if (g_sysClkHz == 0U)
    {
        /* Fallback to 16 MHz if something weird happens */
        g_sysClkHz = 16000000U;
    }

    /* Reset counter */
    g_systickMs = 0U;

    /* Configure SysTick for 1 ms period */
    SysTickDisable();
    SysTickIntDisable();

    /* Period = clock / 1000 for 1 ms tick */
    SysTickPeriodSet(g_sysClkHz / 1000U);

    /*
     * We DON'T call SysTickIntRegister() because the startup file
     * already maps the SysTick vector to systick_ISR().
     */
    SysTickIntEnable();
    SysTickEnable();
}

void MCAL_SysTick_Start(void)
{
    SysTickEnable();
    SysTickIntEnable();
}

void MCAL_SysTick_Stop(void)
{
    SysTickIntDisable();
    SysTickDisable();
}

void MCAL_SysTick_SetCallback(SysTick_CallbackType cb)
{
    g_systickCb = cb;
}

uint32_t MCAL_SysTick_GetTickMs(void)
{
    return g_systickMs;
}

void MCAL_SysTick_DelayMs(uint32_t ms)
{
    uint32_t start = g_systickMs;

    /* Wrap-around safe: subtraction in unsigned arithmetic */
    while ((g_systickMs - start) < ms)
    {
        /* busy wait */
    }
}

uint32_t MCAL_SysTick_GetClockHz(void)
{
    return g_sysClkHz;
}
