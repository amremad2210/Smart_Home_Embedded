/*============================================================================
 *  Module      : Control ECU - UART Test Application
 *  File Name   : main.c
 *  Description : Simple UART test to send strings to HMI ECU
 *===========================================================================*/

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"

#include "mcal/mcal_gpio.h"
#include "mcal/mcal_systick.h"

#include "hal/hal_comm.h"
#include "Types.h"

/*======================================================================
 *  Defines
 *====================================================================*/

/* LED Configuration */
#define LED_GPIO_PERIPH         SYSCTL_PERIPH_GPIOF
#define LED_PORT_BASE           GPIO_PORTF_BASE
#define GREEN_LED_PIN           (1U << 3)  /* PF3 */

/*======================================================================
 *  Local Function Prototypes
 *====================================================================*/

static void LED_Init(void);
static void LED_Toggle(void);

/*======================================================================
 *  Main Function
 *====================================================================*/

int main(void)
{
    uint8_t counter = 0;
    
    /* Initialize SysTick for delays */
    MCAL_SysTick_Init();
    
    /* Initialize UART communication */
    HAL_COMM_Init();
    
    /* Initialize LEDs */
    LED_Init();
    
    /* Initial delay to let HMI boot up */
    MCAL_SysTick_DelayMs(2000U);
    
    /* Main test loop */
    while(1)
    {
        /* Send test message 1 */
        HAL_COMM_SendString("Hello HMI ECU!");
        HAL_COMM_SendByte('\0');  /* Null terminator */
        
        /* Toggle LED to indicate transmission */
        LED_Toggle();
        
        /* Wait before next message */
        MCAL_SysTick_DelayMs(2000U);
        
        /* Send counter message */
        HAL_COMM_SendString("Count: ");
        HAL_COMM_SendByte('0' + (counter / 10));
        HAL_COMM_SendByte('0' + (counter % 10));
        HAL_COMM_SendByte('\0');
        
        /* Toggle LED */
        LED_Toggle();
        
        counter++;
        if (counter >= 100) counter = 0;
        
        /* Wait before next transmission */
        MCAL_SysTick_DelayMs(2000U);
    }
}

/*======================================================================
 *  Helper Functions
 *====================================================================*/

/**
 * @brief Initialize LED pins
 */
static void LED_Init(void)
{
    MCAL_GPIO_EnablePort(LED_GPIO_PERIPH);
    MCAL_GPIO_InitPin(LED_PORT_BASE, GREEN_LED_PIN, 
                      GPIO_DIR_OUTPUT, GPIO_ATTACH_DEFAULT);
    MCAL_GPIO_WritePin(LED_PORT_BASE, GREEN_LED_PIN, LOGIC_LOW);
}

/**
 * @brief Toggle green LED
 */
static void LED_Toggle(void)
{
    static boolean ledState = FALSE;
    ledState = !ledState;
    MCAL_GPIO_WritePin(LED_PORT_BASE, GREEN_LED_PIN, 
                      ledState ? LOGIC_HIGH : LOGIC_LOW);
}
