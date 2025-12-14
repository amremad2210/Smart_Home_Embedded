#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h" // Standard driverlib SysTick

#include "mcal/mcal_gpio.h"
#include "mcal/mcal_gpt.h"
#include "hal/hal_eeprom.h"
#include "hal/hal_motor.h"   
#include "hal/hal_buzzer.h"  
#include "Types.h"

#define LED_GPIO_PERIPH         SYSCTL_PERIPH_GPIOF
#define LED_PORT_BASE           GPIO_PORTF_BASE
#define GREEN_LED_PIN           (1U << 3)
#define RED_LED_PIN             (1U << 1)

// *** TUNING PARAMETER ***
// How long (in milliseconds) does the motor need to spin to open the lock?
// Start with 500ms, increase if it doesn't open fully.
#define LOCK_TURN_TIME_MS       1000 

static uint8_t activeLedPin = 0U;

// Simple Blocking Delay helper using TivaWare
void Delay_ms(uint32_t ms) {
    // SysCtlDelay takes "Loop Counts". 1 loop is roughly 3 clock cycles.
    // Formula: (Clock / 3) * (seconds)
    SysCtlDelay( (SysCtlClockGet() / 3) * (ms / 1000.0) );
}

static void Timer0A_BlinkCallback(void){
  MCAL_GPIO_TogglePin(LED_PORT_BASE, activeLedPin);
}

int main()
{
  // 1. Set System Clock to 50MHz
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

  // 2. Hardware Initialization (Must be first)
  HAL_Motor_Init();
  BUZZER_init();
  MCAL_GPIO_EnablePort(LED_GPIO_PERIPH);
  MCAL_GPIO_InitPin(LED_PORT_BASE, GREEN_LED_PIN | RED_LED_PIN, GPIO_DIR_OUTPUT, GPIO_ATTACH_DEFAULT);
  
  uint8_t result = HAL_EEPROM_Init();

  if (result != HAL_EEPROM_SUCCESS) {
    activeLedPin = RED_LED_PIN;
  }
  else {
    HAL_EEPROM_ClearPassword();
    char storedPassword[] = "1234";
    HAL_EEPROM_StorePassword(storedPassword, 4U);
    
    // *** TEST ***
    char attemptedPassword[] = "1234"; // Change to "9999" to test wrong password
    uint8_t attemptLength = 4U;
    
    if (HAL_EEPROM_VerifyPassword(attemptedPassword, attemptLength))
    {
      /* === CORRECT PASSWORD === */
      activeLedPin = GREEN_LED_PIN;
      BUZZER_beep(100); 

      // 1. UNLOCK (Spin Forward for X time)
      HAL_Motor_Move(MOTOR_FORWARD);
      Delay_ms(LOCK_TURN_TIME_MS); 

      // 2. STOP (Hold open)
      HAL_Motor_Move(MOTOR_STOP);
      Delay_ms(5000); // Wait 5 seconds for user to enter

      // 3. LOCK (Spin Backward for X time)
      HAL_Motor_Move(MOTOR_BACKWARD);
      Delay_ms(LOCK_TURN_TIME_MS);

      // 4. STOP (System Idle)
      HAL_Motor_Move(MOTOR_STOP);
    }
    else
    {
      /* === WRONG PASSWORD === */
      activeLedPin = RED_LED_PIN;
      BUZZER_beep(1000); // Long beep
      
      // Ensure motor is stopped
      HAL_Motor_Move(MOTOR_STOP);
    }
  }
  
  // LED Blinking Timer Setup
  uint32_t sysClkHz = SysCtlClockGet();
  Gpt_ConfigType t0Cfg = { (sysClkHz/2)-1, 0, GPT_TIMER0A, GPT_MODE_PERIODIC, GPT_CAPTURE_EDGE_RISING, 1 };
  Gpt_Init(&t0Cfg);
  Gpt_SetCallBack(Timer0A_BlinkCallback, GPT_TIMER0A);
  Gpt_Start(GPT_TIMER0A);
  
  while(1){ }
  return 0;
}