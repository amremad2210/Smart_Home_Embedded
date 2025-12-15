/*
 * HIMI_WS - Keypad Test Program
 * 
 * Tests each button on the keypad with different LED colors
 * Use this to map your keypad buttons to their returned values
 * 
 * LED Colors:
 * - Different color for each button press
 * - Helps identify which button returns which value
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "mcal/mcal_gpio.h"
#include "mcal/mcal_systick.h"
#include "hal/hal_keypad.h"
#include "hal/hal_rgb_led.h"

int main(void)
{
    uint8_t pressed_key = 0;
    
    /* Set system clock to 40 MHz */
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    
    /* Initialize peripherals */
    MCAL_SysTick_Init();
    RGB_LED_Init();
    HAL_Keypad_Init();
    
    /* Startup sequence - system ready */
    RGB_LED_SetColor(1, 1, 1);
    MCAL_SysTick_DelayMs(500);
    RGB_LED_SetColor(0, 0, 0);
    MCAL_SysTick_DelayMs(500);
    
    /* Main loop */
    while(1)
    {
        /* Wait for key press (non-blocking) */
        pressed_key = HAL_Keypad_GetKey();
        
        /* Skip if no key pressed */
        if(pressed_key == 0)
        {
            MCAL_SysTick_DelayMs(10);
            continue;
        }
        
        MCAL_SysTick_DelayMs(300);
        
        /* Map each button to a different LED color pattern */
        switch(pressed_key)
        {
            case '1':  /* Button returns '1' */
                RGB_LED_SetColor(1, 0, 0);  /* Red */
                break;
                
            case '2':  /* Button returns '2' */
                RGB_LED_SetColor(0, 1, 0);  /* Green */
                break;
                
            case '3':  /* Button returns '3' */
                RGB_LED_SetColor(0, 0, 1);  /* Blue */
                break;
                
            case '4':  /* Button returns '4' */
                RGB_LED_SetColor(1, 1, 0);  /* Yellow (Red + Green) */
                break;
                
            case '5':  /* Button returns '5' */
                RGB_LED_SetColor(1, 0, 1);  /* Magenta (Red + Blue) */
                break;
                
            case '6':  /* Button returns '6' */
                RGB_LED_SetColor(0, 1, 1);  /* Cyan (Green + Blue) */
                break;
                
            case '7':  /* Button returns '7' */
                RGB_LED_SetColor(1, 1, 1);  /* White (All colors) */
                break;
                
            case '8':  /* Button returns '8' */
                /* Red to Green transition */
                RGB_LED_SetColor(1, 0, 0);
                MCAL_SysTick_DelayMs(400);
                RGB_LED_SetColor(0, 1, 0);
                break;
                
            case '9':  /* Button returns '9' */
                /* Green to Blue transition */
                RGB_LED_SetColor(0, 1, 0);
                MCAL_SysTick_DelayMs(400);
                RGB_LED_SetColor(0, 0, 1);
                break;
                
            case '0':  /* Button returns '0' */
                /* Blue to Red transition */
                RGB_LED_SetColor(0, 0, 1);
                MCAL_SysTick_DelayMs(400);
                RGB_LED_SetColor(1, 0, 0);
                break;
                
            case '*':  /* Button returns '*' */
                /* Yellow to Magenta transition */
                RGB_LED_SetColor(1, 1, 0);
                MCAL_SysTick_DelayMs(400);
                RGB_LED_SetColor(1, 0, 1);
                break;
                
            case '#':  /* Button returns '#' */
                /* Cyan to Yellow transition */
                RGB_LED_SetColor(0, 1, 1);
                MCAL_SysTick_DelayMs(400);
                RGB_LED_SetColor(1, 1, 0);
                break;
                
            case 'A':  /* Button returns 'A' */
                /* Magenta to Cyan transition */
                RGB_LED_SetColor(1, 0, 1);
                MCAL_SysTick_DelayMs(400);
                RGB_LED_SetColor(0, 1, 1);
                break;
                
            case 'B':  /* Button returns 'B' */
                /* Red -> Yellow -> Green sequence */
                RGB_LED_SetColor(1, 0, 0);
                MCAL_SysTick_DelayMs(300);
                RGB_LED_SetColor(1, 1, 0);
                MCAL_SysTick_DelayMs(300);
                RGB_LED_SetColor(0, 1, 0);
                break;
                
            case 'C':  /* Button returns 'C' */
                /* Blue -> Magenta -> Red sequence */
                RGB_LED_SetColor(0, 0, 1);
                MCAL_SysTick_DelayMs(300);
                RGB_LED_SetColor(1, 0, 1);
                MCAL_SysTick_DelayMs(300);
                RGB_LED_SetColor(1, 0, 0);
                break;
                
            case 'D':  /* Button returns 'D' */
                /* Green -> Cyan -> Blue sequence */
                RGB_LED_SetColor(0, 1, 0);
                MCAL_SysTick_DelayMs(300);
                RGB_LED_SetColor(0, 1, 1);
                MCAL_SysTick_DelayMs(300);
                RGB_LED_SetColor(0, 0, 1);
                break;
                
            default:  /* Unknown button */
                /* Rapid red blink - error */
                for(uint8_t i = 0; i < 5; i++)
                {
                    RGB_LED_SetColor(1, 0, 0);
                    MCAL_SysTick_DelayMs(50);
                    RGB_LED_SetColor(0, 0, 0);
                    MCAL_SysTick_DelayMs(50);
                }
                break;
        }
        
        /* Keep LED on for 2 seconds to see the result */
        MCAL_SysTick_DelayMs(2000);
        
        /* Turn off LED and wait for next press */
        RGB_LED_SetColor(0, 0, 0);
        MCAL_SysTick_DelayMs(500);
    }
}