//hello this is control

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"

#include "mcal/mcal_gpio.h"
#include "mcal/mcal_gpt.h"
#include "hal/hal_eeprom.h"

#include "hal/hal_motor.h"
#include "hal/hal_buzzer.h"
#include "mcal/mcal_systick.h" // for delays (waiting for door to open)
#include "Types.h"


#define LED_GPIO_PERIPH         SYSCTL_PERIPH_GPIOF
#define LED_PORT_BASE           GPIO_PORTF_BASE
#define GREEN_LED_PIN           (1U << 3)  // PF3 - Green LED
#define RED_LED_PIN             (1U << 1)  // PF1 - Red LED

static uint8_t activeLedPin = 0U;  // Track which LED to blink

/* ==========================
 *   Timer0A callback
 * ==========================
 *
 * This function is called from Timer0A_Handler()
 * via the GPT callback mechanism.
 *
 * Toggles the active LED (Green or Red)
 */
static void Timer0A_BlinkCallback(void){
  
  MCAL_GPIO_TogglePin(LED_PORT_BASE, activeLedPin);

}


int main()
{
  uint8_t result;
  
  /* ========================================
   * EEPROM Password Test
   * ======================================== */
  
  // Initialize EEPROM
  result = HAL_EEPROM_Init();
  
  // Only proceed if initialization succeeded
  if (result != HAL_EEPROM_SUCCESS)
  {
    // EEPROM init failed - blink red LED fast to indicate error
    activeLedPin = RED_LED_PIN;
  }
  else
  {
    // Clear any existing data (fresh start for testing)
    HAL_EEPROM_ClearPassword();
    
    // Store the correct password "1234"
    char storedPassword[] = "1234";
    HAL_EEPROM_StorePassword(storedPassword, 4U);
    
    // *** TEST PASSWORD ENTRY ***
    // Try to access with a password (change this to test!)
    char attemptedPassword[] = "4567";  // Change to "1234" for correct, "5678" for wrong
    uint8_t attemptLength = 4U;
    
    // Verify the attempted password
//    if (HAL_EEPROM_VerifyPassword(attemptedPassword, attemptLength))
//    {
//      // ✓ Password is CORRECT!
//      activeLedPin = GREEN_LED_PIN;  // Set to blink GREEN
//    }
//    else
//    {
//      // ✗ Password is WRONG!
//      activeLedPin = RED_LED_PIN;  // Set to blink RED
//    }
//  }
    if (HAL_EEPROM_VerifyPassword(attemptedPassword, attemptLength))
    {
      // ✓ Password is CORRECT!
      activeLedPin = GREEN_LED_PIN;

      // --- ADD THIS BLOCK ---

      // 1. Visual/Audio Feedback
      BUZZER_beep(100); // Short beep for success

      // 2. UNLOCK DOOR (Retract Bolt)
      // Move Forward at 100% speed
      HAL_Motor_Move(MOTOR_FORWARD, 100);

      // 3. Wait for the mechanism to move
      // Adjust this time based on how long your lock takes to open!
      MCAL_SysTick_DelayMs(2000); // Wait 2 seconds

      // 4. STOP MOTOR (Important: Don't keep pushing against the wall)
      HAL_Motor_Move(MOTOR_STOP, 0);

      // 5. Keep door open for a while (e.g., 5 seconds to let user enter)
      MCAL_SysTick_DelayMs(5000);

      // 6. LOCK DOOR (Extend Bolt)
      // Move Backward at 100% speed
      HAL_Motor_Move(MOTOR_BACKWARD, 100);

      // 7. Wait for mechanism to close
      MCAL_SysTick_DelayMs(2000);

      // 8. Final Stop
      HAL_Motor_Move(MOTOR_STOP, 0);

      // ----------------------
    }
    else
    {
      // ✗ Password is WRONG!
      activeLedPin = RED_LED_PIN;

      // --- ADD THIS BLOCK ---

      // Long angry beep
      BUZZER_beep(1000);

      // Ensure motor is locked/stopped
      HAL_Motor_Move(MOTOR_STOP, 0);

      // ----------------------
    }
  
  /* ========================================
   * GPIO and LED Setup
   * ======================================== */
  
  MCAL_GPIO_EnablePort(LED_GPIO_PERIPH);
  
  // Initialize both LEDs
  MCAL_GPIO_InitPin(LED_PORT_BASE, GREEN_LED_PIN | RED_LED_PIN, GPIO_DIR_OUTPUT, GPIO_ATTACH_DEFAULT);
  MCAL_GPIO_WritePin(LED_PORT_BASE, GREEN_LED_PIN, 0U);
  MCAL_GPIO_WritePin(LED_PORT_BASE, RED_LED_PIN, 0U);


  /* ========================================
   * Motor Setup
   * ======================================== */

  /* Initialize Motor */
  HAL_Motor_Init();

  /* Test: Move Forward at 60% speed */
//  HAL_Motor_Move(MOTOR_FORWARD, 60);

  
  
  /* ========================================
   * Timer Setup - Blink appropriate LED
   * ======================================== */
  
  // Start blinking timer (Green if correct, Red if wrong)
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

  /* Initialize Drivers */
  HAL_Motor_Init();
  BUZZER_init();
  
  while(1){
  
  
  }
  
  return 0;
}
