#include "mcal/mcal_gpt.h"

#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

/* ===========================
 *   Static callback array
 * ===========================
 *
 * For each GPT timer (GPT_TIMER0A, GPT_WTIMER2A, ...)
 * we store a function pointer to call from the ISR.
 *
 * You set these via Gpt_SetCallBack().
 */
static Gpt_CallbackType g_Gpt_Callbacks[GPT_NUM_TIMERS] = {0};

/* ===========================
 *   Hardware mapping
 * ===========================
 *
 * This struct maps our abstract timer ID (Gpt_IDType)
 * to the actual hardware details used by TivaWare:
 *  - base       : TIMER0_BASE / WTIMER2_BASE
 *  - subTimer   : TIMER_A (we always use sub-timer A here)
 *  - sysctlPeriph: clock gate ID for SysCtlPeripheralEnable()
 *  - intNumber  : NVIC interrupt number (INT_TIMER0A, INT_WTIMER2A)
 */
typedef struct {
    uint32_t base;
    uint32_t subTimer;
    uint32_t sysctlPeriph;
    uint32_t intNumber;
} Gpt_HwMapType;

/* Lookup table from Gpt_IDType -> hardware info */
static const Gpt_HwMapType g_Gpt_HwMap[GPT_NUM_TIMERS] = {
    [GPT_TIMER0A] = {
        .base         = TIMER0_BASE,
        .subTimer     = TIMER_A,
        .sysctlPeriph = SYSCTL_PERIPH_TIMER0,
        .intNumber    = INT_TIMER0A
    },
    [GPT_WTIMER2A] = {
        .base         = WTIMER2_BASE,
        .subTimer     = TIMER_A,
        .sysctlPeriph = SYSCTL_PERIPH_WTIMER2,
        .intNumber    = INT_WTIMER2A
    }
};

/* Small helper: given a logical timer ID, return its
 * hardware mapping entry or NULL if invalid.
 */
static const Gpt_HwMapType* prvGetMap(Gpt_IDType id)
{
    if (id >= GPT_NUM_TIMERS) return 0;
    return &g_Gpt_HwMap[id];
}

/* ===========================
 *   Init
 * ===========================
 *
 * Configure one timer according to the config struct:
 *  - enable the peripheral clock
 *  - choose mode (periodic / one-shot / capture / PWM)
 *  - set LOAD / MATCH
 *  - enable interrupts if requested
 */
void Gpt_Init(const Gpt_ConfigType *Config_Ptr)
{
    if (!Config_Ptr) return;  // defensive: null pointer check

    Gpt_IDType id = Config_Ptr->timer_ID;
    const Gpt_HwMapType *map = prvGetMap(id);
    if (!map) return;         // invalid ID ? do nothing

    /* 1) Enable clock to the timer peripheral (Timer0 or WideTimer2) */
    SysCtlPeripheralEnable(map->sysctlPeriph);
    while (!SysCtlPeripheralReady(map->sysctlPeriph)) { }

    uint32_t base = map->base;
    uint32_t st   = map->subTimer;

    /* 2) Disable the timer while we reconfigure it */
    TimerDisable(base, st);

    /* 3) Configure mode depending on which timer and which mode we want */
    switch (id)
    {
    case GPT_TIMER0A:
        /* In this project we *only* use Timer0A in PERIODIC mode.
         * We keep it simple and treat it as 32-bit periodic.
         */
        if (Config_Ptr->timer_mode != GPT_MODE_PERIODIC)
        {
            /* If user asked for non-periodic on TIMER0A, ignore. */
            return;
        }

        /* 32-bit periodic timer */
        TimerConfigure(base, TIMER_CFG_PERIODIC);

        /* Set the reload value (period in ticks) */
        TimerLoadSet(base, st, Config_Ptr->timer_InitialValue);
        break;

    case GPT_WTIMER2A:
        /* Wide Timer 2 sub-timer A can be used in several modes:
         *  ONE_SHOT, PERIODIC, CAPTURE, PWM
         */
        switch (Config_Ptr->timer_mode)
        {
        case GPT_MODE_ONE_SHOT:
            /* One-shot: timer counts down once then stops */
            TimerConfigure(base, TIMER_CFG_ONE_SHOT);
            TimerLoadSet(base, st, Config_Ptr->timer_InitialValue);
            break;

        case GPT_MODE_PERIODIC:
            /* Periodic: counts down, reloads, repeats */
            TimerConfigure(base, TIMER_CFG_PERIODIC);
            TimerLoadSet(base, st, Config_Ptr->timer_InitialValue);
            break;

        case GPT_MODE_CAPTURE:
        {
            /* Capture mode (edge-time) on Wide Timer2A.
             * We use split-pair so A and B are separate 32-bit timers.
             */
            TimerConfigure(base,
                           TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME);

            /* Choose capture edge type (rising, falling, or both) */
            uint32_t event;
            switch (Config_Ptr->captureEdge)
            {
            case GPT_CAPTURE_EDGE_RISING:
                event = TIMER_EVENT_POS_EDGE;
                break;
            case GPT_CAPTURE_EDGE_FALLING:
                event = TIMER_EVENT_NEG_EDGE;
                break;
            default:
                event = TIMER_EVENT_BOTH_EDGES;
                break;
            }

            /* Apply capture edge configuration */
            TimerControlEvent(base, st, event);

            /* Make the timer free-run from max value.
             * It will wrap around; we handle that in software if needed.
             */
            TimerLoadSet(base, st, 0xFFFFFFFFu);
            break;
        }

        case GPT_MODE_PWM:
            /* PWM mode on WTIMER2A, split pair again for sub-timer A only */
            TimerConfigure(base,
                           TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM);

            /* Non-inverted PWM output:
             * - Output is high at reload, low at match.
             */
            TimerControlLevel(base, st, false);

            /* Protect against 0 period: hardware expects non-zero LOAD. */
            if (Config_Ptr->timer_InitialValue == 0u)
                TimerLoadSet(base, st, 1u);
            else
                /* LOAD = period - 1 (Tiva standard) */
                TimerLoadSet(base, st, Config_Ptr->timer_InitialValue - 1u);

            /* MATCH register sets duty (initial value from CompareValue) */
            TimerMatchSet(base, st, Config_Ptr->timer_CompareValue);
            break;

        default:
            /* Unsupported mode ? do nothing */
            return;
        }
        break;

    default:
        /* Unknown timer ID ? do nothing */
        return;
    }

    /* 4) Initialize callback entry to NULL by default.
     *    User will call Gpt_SetCallBack() later if they want one.
     */
    g_Gpt_Callbacks[id] = 0;

    /* 5) If interrupts are enabled in config, enable them in hardware */
    if (Config_Ptr->enableInterrupt)
    {
        uint32_t intMask = 0u;

        /* Decide which interrupt flag we care about based on mode */
        switch (Config_Ptr->timer_mode)
        {
        case GPT_MODE_PERIODIC:
        case GPT_MODE_ONE_SHOT:
            /* Timer timeout interrupt */
            intMask = TIMER_TIMA_TIMEOUT;
            break;
        case GPT_MODE_CAPTURE:
        case GPT_MODE_PWM:
            /* Capture/PWM event interrupt */
            intMask = TIMER_CAPA_EVENT;
            break;
        default:
            break;
        }

        if (intMask != 0u)
        {
            /* Enable the chosen interrupt source inside the timer module */
            TimerIntEnable(base, intMask);

            /* Enable the corresponding interrupt in the NVIC */
            IntEnable(map->intNumber);
        }
    }
}

/* ===========================
 *   Start / Stop
 * ===========================
 *
 * These simply enable/disable the underlying timer.
 * The mode/configuration must already be set by Gpt_Init().
 */

void Gpt_Start(Gpt_IDType timer_ID)
{
    const Gpt_HwMapType *map = prvGetMap(timer_ID);
    if (!map) return;

    /* Enable the sub-timer (Timer A) */
    TimerEnable(map->base, map->subTimer);
}

void Gpt_Stop(Gpt_IDType timer_ID)
{
    const Gpt_HwMapType *map = prvGetMap(timer_ID);
    if (!map) return;

    /* Disable the sub-timer (Timer A) */
    TimerDisable(map->base, map->subTimer);
}

/* ===========================
 *   DeInit
 * ===========================
 *
 * Turn off a timer:
 *  - stop it
 *  - disable its interrupts
 *  - clear its callback
 * (We don't gate the clock off; just make it inert.)
 */

void Gpt_DeInit(Gpt_IDType timer_ID)
{
    const Gpt_HwMapType *map = prvGetMap(timer_ID);
    if (!map) return;

    /* Stop the timer */
    TimerDisable(map->base, map->subTimer);

    /* Disable both possible interrupt types */
    TimerIntDisable(map->base, TIMER_TIMA_TIMEOUT | TIMER_CAPA_EVENT);

    /* Disable NVIC entry */
    IntDisable(map->intNumber);

    /* Remove callback */
    g_Gpt_Callbacks[timer_ID] = 0;
}

/* ===========================
 *   Callback registration
 * ===========================
 *
 * Store user function pointer to be called from ISR.
 */

void Gpt_SetCallBack(Gpt_CallbackType cb, Gpt_IDType timer_ID)
{
    if (timer_ID >= GPT_NUM_TIMERS) return;
    g_Gpt_Callbacks[timer_ID] = cb;
}

/* ===========================
 *   PWM helper
 * ===========================
 *
 * Update PWM duty cycle (0–100 %) for a timer in PWM mode.
 * We:
 *  - read current LOAD (period)
 *  - compute a MATCH value that gives desired duty
 *  - write MATCH back to the timer
 */

void Gpt_SetPwmDuty(Gpt_IDType timer_ID, float dutyPercent)
{
    const Gpt_HwMapType *map = prvGetMap(timer_ID);
    if (!map) return;

    /* Clamp duty into [0, 100] */
    if (dutyPercent < 0.0f)   dutyPercent = 0.0f;
    if (dutyPercent > 100.0f) dutyPercent = 100.0f;

    /* Current period (LOAD) */
    uint32_t load = TimerLoadGet(map->base, map->subTimer);

    /* In PWM mode, counter typically counts down from LOAD to MATCH:
     * duty ˜ (LOAD - MATCH) / (LOAD + 1).
     *
     * Rearranged to get MATCH for a desired duty:
     * MATCH = LOAD - duty * (LOAD + 1)
     */
    uint32_t match = load - (uint32_t)((dutyPercent / 100.0f)
                                       * (float)(load + 1u));

    /* Apply new match value ? changes duty cycle on the output pin */
    TimerMatchSet(map->base, map->subTimer, match);
}

/* ===========================
 *   Capture helper
 * ===========================
 *
 * For a timer configured in CAPTURE mode, this returns
 * the current captured timer value (TAR/TAV).
 * You typically call this inside your capture callback
 * to compute period between edges.
 */

uint32_t Gpt_GetCaptureValue(Gpt_IDType timer_ID)
{
    const Gpt_HwMapType *map = prvGetMap(timer_ID);
    if (!map) return 0u;

    return TimerValueGet(map->base, map->subTimer);
}

/* ===========================
 *   ISRs
 * ===========================
 *
 * These are the interrupt service routines which must be
 * wired in the startup file's vector table.
 *
 * Flow:
 *  - clear interrupt flags
 *  - look up callback function
 *  - call it if not NULL
 */

void Timer0A_Handler(void)
{
    /* Clear both timeout and capture events to be safe */
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT | TIMER_CAPA_EVENT);

    /* Call user callback if registered */
    if (g_Gpt_Callbacks[GPT_TIMER0A])
        g_Gpt_Callbacks[GPT_TIMER0A]();
}

void WTimer2A_Handler(void)
{
    /* Clear both timeout and capture events to be safe */
    TimerIntClear(WTIMER2_BASE, TIMER_TIMA_TIMEOUT | TIMER_CAPA_EVENT);

    /* Call user callback if registered */
    if (g_Gpt_Callbacks[GPT_WTIMER2A])
        g_Gpt_Callbacks[GPT_WTIMER2A]();
}
