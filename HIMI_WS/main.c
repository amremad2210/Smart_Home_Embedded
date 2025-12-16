/*****************************************************************************
 * File: main.c
 * Description: Main application - Keypad to LCD display
 * Author: Ahmedhh
 * Date: December 4, 2025
 * 
 * This program reads keypresses from a 4x4 keypad and displays them on
 * a 16x2 LCD in 4-bit mode.
 *****************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "tm4c123gh6pm.h"
#include "hal/hal_lcd.h"


#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"

#include "mcal/mcal_gpio.h"
#include "mcal/mcal_gpt.h"
#include "mcal/mcal_systick.h" 

int main(void)
{
    uint8_t counter = 0;
    
    /* Set system clock to 16 MHz */
    //SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    
    /* Initialize SysTick for delays */
    MCAL_SysTick_Init();
    
    /* Initialize the LCD (initializes I2C as well) */
    Lcd_Init();
    
    /* Test 1: Display welcome message */
    Lcd_Clear();
    Lcd_GoToRowColumn(0, 0);  // Row 0, Column 0
    Lcd_DisplayString("LCD Test v1.0");
    Lcd_GoToRowColumn(1, 0);  // Row 1, Column 0
    Lcd_DisplayString("I2C Working!");
    MCAL_SysTick_DelayMs(3000);

    
    /* Test 2: Character-by-character display */
    Lcd_Clear();
    Lcd_GoToRowColumn(0, 0);
    Lcd_DisplayString("Char Test:");
    Lcd_GoToRowColumn(1, 0);
    
    char test_chars[] = "ABCDEFGHIJ123456";
    for(uint8_t i = 0; i < 16; i++)
    {
        Lcd_DisplayCharacter(test_chars[i]);
        MCAL_SysTick_DelayMs(200);
    }
    MCAL_SysTick_DelayMs(2000);
    
    /* Test 3: Position testing - All corners */
    Lcd_Clear();
    Lcd_GoToRowColumn(0, 0);  // Top-left
    Lcd_DisplayCharacter('1');
    
    Lcd_GoToRowColumn(0, 15); // Top-right
    Lcd_DisplayCharacter('2');
    
    Lcd_GoToRowColumn(1, 0);  // Bottom-left
    Lcd_DisplayCharacter('3');
    
    Lcd_GoToRowColumn(1, 15); // Bottom-right
    Lcd_DisplayCharacter('4');
    
    Lcd_GoToRowColumn(0, 5);
    Lcd_DisplayString("POS");
    
    Lcd_GoToRowColumn(1, 6);
    Lcd_DisplayString("TEST");
    MCAL_SysTick_DelayMs(3000);
    
    /* Test 4: Scrolling text simulation */
    Lcd_Clear();
    Lcd_GoToRowColumn(0, 0);
    Lcd_DisplayString("Scrolling...");
    MCAL_SysTick_DelayMs(1000);
    
    char *long_msg = "Hello Embedded Systems!";
    for(uint8_t offset = 0; offset < 8; offset++)
    {
        Lcd_GoToRowColumn(1, 0);
        Lcd_DisplayString("                "); // Clear line
        Lcd_GoToRowColumn(1, 0);
        
        for(uint8_t i = 0; i < 16 && (offset + i) < 23; i++)
        {
            Lcd_DisplayCharacter(long_msg[offset + i]);
        }
        MCAL_SysTick_DelayMs(500);
    }
    MCAL_SysTick_DelayMs(2000);
    
    /* Test 5: Counter display */
    Lcd_Clear();
    Lcd_GoToRowColumn(0, 0);
    Lcd_DisplayString("Counter Test:");
    
    counter = 0;
    while(1)
    {
        Lcd_GoToRowColumn(1, 0);
        Lcd_DisplayString("Count: ");
        
        /* Display counter value */
        uint8_t hundreds = counter / 100;
        uint8_t tens = (counter / 10) % 10;
        uint8_t ones = counter % 10;
        
        Lcd_DisplayCharacter('0' + hundreds);
        Lcd_DisplayCharacter('0' + tens);
        Lcd_DisplayCharacter('0' + ones);
        
        counter++;
        if(counter > 255) counter = 0;
        
        MCAL_SysTick_DelayMs(500);
    }
}