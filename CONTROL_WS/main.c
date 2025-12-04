//hello this is control

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"

#include "mcal/mcal_gpio.h"
#include "mcal/mcal_gpt.h"


#define LED_GPIO_PERIPH         SYSCTL_PERIPH_GPIOF
#define LED_PORT_BASE           GPIO_PORTF_BASE
#define LED_PIN_MASK            (1U << 2)


/* ==========================
 *   Timer0A callback
 * ==========================
 *
 * This function is called from Timer0A_Handler()
 * via the GPT callback mechanism.
 *
 * Each time it runs, we toggle the LED on PF1.
 */
static void Timer0A_BlinkCallback(void){
  
  MCAL_GPIO_TogglePin(LED_PORT_BASE, LED_PIN_MASK);

}


int main()
{
  
  MCAL_GPIO_EnablePort(LED_GPIO_PERIPH);
  MCAL_GPIO_InitPin(LED_PORT_BASE, LED_PIN_MASK, GPIO_DIR_OUTPUT, GPIO_ATTACH_DEFAULT);
  MCAL_GPIO_WritePin(LED_PORT_BASE, LED_PIN_MASK, 0U);
  
  
  
   uint32_t sysClkHz = SysCtlClockGet();
   uint32_t loadTicks = (sysClkHz / 2U) - 1U;
   
   Gpt_ConfigType t0Cfg;
   
   t0Cfg.timer_InitialValue = loadTicks;
   t0Cfg.timer_CompareValue = 0U;                 /* not used for periodic   */
   t0Cfg.timer_ID           = GPT_TIMER0A;        /* use Timer0A             */
   t0Cfg.timer_mode         = GPT_MODE_PERIODIC;  /* periodic mode           */
   t0Cfg.captureEdge        = GPT_CAPTURE_EDGE_RISING; /* ignored in PERIODIC */
   t0Cfg.enableInterrupt    = 1U;                 /* enable Timer0A interrupt*/
   
   Gpt_Init(&t0Cfg);
   Gpt_SetCallBack(Timer0A_BlinkCallback, GPT_TIMER0A);
   Gpt_Start(GPT_TIMER0A);

   
   
  
  
  while(1){
  
  
  }
  
  return 0;
}
