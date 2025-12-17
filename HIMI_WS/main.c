/*============================================================================
 *  Module      : HMI ECU - UART Test Application
 *  File Name   : main.c
 *  Description : Simple UART test to receive strings from Control ECU
 *               and display them on LCD
 *===========================================================================*/

#include <stdint.h>
#include <stdbool.h>

#include "driverlib/sysctl.h"

#include "Types.h"
#include "mcal/mcal_systick.h"

#include "hal/hal_lcd.h"
#include "hal/hal_rgb_led.h"
#include "hal/hal_comm.h"

/*======================================================================
 *  Defines
 *====================================================================*/

#define BUFFER_SIZE     32U

/*======================================================================
 *  Local Function Prototypes
 *====================================================================*/

static void HMI_Init(void);
static void ReceiveAndDisplayString(void);

/*======================================================================
 *  Main Function
 *====================================================================*/

int main(void)
{
    /* Initialize hardware */
    HMI_Init();
    
    /* Display startup message */
    Lcd_Clear();
    Lcd_GoToRowColumn(0, 0);
    Lcd_DisplayString("UART Test Mode");
    Lcd_GoToRowColumn(1, 0);
    Lcd_DisplayString("Waiting...");
    
    /* Set LED to blue (waiting) */
    RGB_LED_SetColor(0, 0, 255);
    
    /* Wait for Control ECU to boot */
    MCAL_SysTick_DelayMs(2000U);
    
    /* Main loop */
    while(1)
    {
        ReceiveAndDisplayString();
    }
}

/*======================================================================
 *  Helper Functions
 *====================================================================*/

/**
 * @brief Initialize HMI hardware
 */
static void HMI_Init(void)
{
    /* Initialize SysTick */
    MCAL_SysTick_Init();
    
    /* Initialize UART */
    HAL_COMM_Init();
    
    /* Initialize LCD */
    Lcd_Init();
    
    /* Initialize RGB LED */
    RGB_LED_Init();
}

/**
 * @brief Receive string from UART and display on LCD
 */
static void ReceiveAndDisplayString(void)
{
    char buffer[BUFFER_SIZE];
    uint8_t idx = 0;
    uint8_t byte;
    
    /* Wait for data */
    while(!HAL_COMM_IsDataAvailable())
    {
        MCAL_SysTick_DelayMs(10U);
    }
    
    /* Set LED to yellow (receiving) */
    RGB_LED_SetColor(255, 255, 0);
    
    /* Receive characters until null terminator or buffer full */
    while(idx < (BUFFER_SIZE - 1))
    {
        if (HAL_COMM_IsDataAvailable())
        {
            byte = HAL_COMM_ReceiveByte();
            
            /* Check for null terminator */
            if (byte == '\0')
            {
                break;
            }
            
            buffer[idx++] = (char)byte;
        }
        else
        {
            MCAL_SysTick_DelayMs(10U);
        }
    }
    buffer[idx] = '\0';  /* Null terminate */
    
    /* Display on LCD */
    Lcd_Clear();
    Lcd_GoToRowColumn(0, 0);
    Lcd_DisplayString(buffer);
    
    /* Set LED to green (received successfully) */
    RGB_LED_SetColor(0, 255, 0);
    
    /* Small delay to show message */
    MCAL_SysTick_DelayMs(100U);
}
