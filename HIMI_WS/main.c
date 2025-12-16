#include <stdint.h>
#include <stdbool.h>

#include "driverlib/sysctl.h"

#include "Types.h"
#include "mcal/mcal_systick.h"

#include "hal/hal_lcd.h"
#include "hal/hal_comm.h"

#define RX_BUFFER_SIZE  64U

int main(void)
{
    char rxBuffer[RX_BUFFER_SIZE];
    uint8_t rxIndex = 0;
    
    /* Set system clock to 16 MHz */
   // SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    
    /* Initialize SysTick for delays */
    MCAL_SysTick_Init();
    
    /* Initialize LCD */
    Lcd_Init();
    
    /* Initialize UART1 for communication with Control ECU */
    HAL_COMM_Init();
    
    /* Display welcome message */
    Lcd_Clear();
    Lcd_GoToRowColumn(0, 0);
    Lcd_DisplayString("HIMI Ready");
    Lcd_GoToRowColumn(1, 0);
    Lcd_DisplayString("Waiting...");
    
    MCAL_SysTick_DelayMs(1000);
    
    /* Main loop: continuously receive and display messages */
    while (1)
    {
        /* Check if data is available from UART1 */
        if (HAL_COMM_IsDataAvailable())
        {
            char c = HAL_COMM_ReceiveByte();
            
            /* If newline or carriage return, display the message */
            if (c == '\n' || c == '\r')
            {
                if (rxIndex > 0)
                {
                    rxBuffer[rxIndex] = '\0';  // Null terminate
                    
                    /* Display received message on LCD */
                    Lcd_Clear();
                    Lcd_GoToRowColumn(0, 0);
                    Lcd_DisplayString("Received:");
                    Lcd_GoToRowColumn(1, 0);
                    
                    /* Display up to 16 chars on second line */
                    for (uint8_t i = 0; i < rxIndex && i < 16; i++)
                    {
                        Lcd_DisplayCharacter(rxBuffer[i]);
                    }
                    
                    /* Reset buffer for next message */
                    rxIndex = 0;
                }
            }
            /* Regular character - add to buffer */
            else if (rxIndex < (RX_BUFFER_SIZE - 1))
            {
                rxBuffer[rxIndex++] = c;
            }
        }
        else
        {
            /* Only delay when no data is available to prevent busy-wait */
            MCAL_SysTick_DelayMs(1);
        }
    }
}