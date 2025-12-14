#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"

#include "mcal/mcal_gpio.h"
#include "mcal/mcal_gpt.h"
#include "mcal/mcal_systick.h" 

#include "hal/hal_eeprom.h"
#include "hal/hal_motor.h"   // Motor Driver
#include "hal/hal_buzzer.h"  // Buzzer Driver
#include "Types.h"


#define LED_GPIO_PERIPH         SYSCTL_PERIPH_GPIOF
#define LED_PORT_BASE           GPIO_PORTF_BASE
#define GREEN_LED_PIN           (1U << 3)  // PF3
#define RED_LED_PIN             (1U << 1)  // PF1

static uint8_t activeLedPin = 0U;

static void Timer0A_BlinkCallback(void){
  MCAL_GPIO_TogglePin(LED_PORT_BASE, activeLedPin);
}

int main()
{
  /* 
   * STEP 1: SYSTEM CLOCK SETUP
   * Set clock to 40MHz or 50MHz for consistent timing 
   */
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

  /* 
   * STEP 2: HARDWARE INITIALIZATION (MUST BE FIRST)
   */
  
  // 0. Initialize SysTick for delays
  MCAL_SysTick_Init();
  
  // 1. Initialize Motor & PWM (Critical to do this before moving!)
  HAL_Motor_Init();
  
  // 2. Initialize Buzzer
  BUZZER_init();
  
  // 3. Initialize LEDs
  MCAL_GPIO_EnablePort(LED_GPIO_PERIPH);
  MCAL_GPIO_InitPin(LED_PORT_BASE, GREEN_LED_PIN | RED_LED_PIN, GPIO_DIR_OUTPUT, GPIO_ATTACH_DEFAULT);
  MCAL_GPIO_WritePin(LED_PORT_BASE, GREEN_LED_PIN, 0U);
  MCAL_GPIO_WritePin(LED_PORT_BASE, RED_LED_PIN, 0U);

  // 4. Initialize EEPROM
  uint8_t result = HAL_EEPROM_Init();

  /* ========================================
   * LOGIC EXECUTION
   * ======================================== */
  
  // Check EEPROM Init
  if (result != HAL_EEPROM_SUCCESS)
  {
    activeLedPin = RED_LED_PIN;
  }
  else
  {
    HAL_EEPROM_ClearPassword();
    char storedPassword[] = "1234";
    HAL_EEPROM_StorePassword(storedPassword, 4U);
    
    // *** TEST: CHANGE THIS TO "1234" TO SEE MOTOR SPIN ***
    // Currently "4567" will result in WRONG password (Red LED + Angry Beep)
    char attemptedPassword[] = "1234"; 
    uint8_t attemptLength = 4U;
    
    // Verify Password
    if (HAL_EEPROM_VerifyPassword(attemptedPassword, attemptLength))
    {
      // --- CORRECT PASSWORD: OPEN DOOR ---
      activeLedPin = GREEN_LED_PIN;

      // 1. Beep Success
      //BUZZER_beep(100); 

      // 2. UNLOCK (Move Forward)
      HAL_Motor_Move(MOTOR_FORWARD);

      // 3. Wait for bolt to retract (2 seconds)
      MCAL_SysTick_DelayMs(2000);

      // 4. STOP Motor (Hold position)
      HAL_Motor_Move(MOTOR_STOP);

      // 5. Wait for user to enter (3 seconds)
      MCAL_SysTick_DelayMs(3000);

      // 6. LOCK (Move Backward)
      HAL_Motor_Move(MOTOR_BACKWARD);

      // 7. Wait for bolt to extend (2 seconds)
      MCAL_SysTick_DelayMs(2000);

      // 8. Final Stop
      HAL_Motor_Move(MOTOR_STOP);
    }
    else
    {
      // --- WRONG PASSWORD: DENY ---
      activeLedPin = RED_LED_PIN;

      // Long Angry Beep
      //BUZZER_beep(1000);

      // Ensure locked
      HAL_Motor_Move(MOTOR_STOP);
    }
  }
  
  /* ========================================
   * Timer Setup (LED Blinking)
   * ======================================== */
  uint32_t sysClkHz = SysCtlClockGet();
  uint32_t loadTicks = (sysClkHz / 2U) - 1U;
  
  Gpt_ConfigType t0Cfg;
  t0Cfg.timer_InitialValue = loadTicks;
  t0Cfg.timer_CompareValue = 0U;
  t0Cfg.timer_ID           = GPT_TIMER0A;
  t0Cfg.timer_mode         = GPT_MODE_PERIODIC;
  t0Cfg.captureEdge        = GPT_CAPTURE_EDGE_RISING;
  t0Cfg.enableInterrupt    = 1U;
  
  Gpt_Init(&t0Cfg);
  Gpt_SetCallBack(Timer0A_BlinkCallback, GPT_TIMER0A);
  Gpt_Start(GPT_TIMER0A);
  
  while(1){
      // Main loop does nothing, everything is handled by Timer Interrupts
  }
  
  return 0;
}