/*============================================================================
 *  Module      : HAL COMM
 *  File Name   : hal_comm.c
 *  Description : Hardware Abstraction Layer for UART Communication
 *===========================================================================*/

#include "hal/hal_comm.h"
#include "mcal/mcal_uart.h"
#include "mcal/mcal_gpio.h"

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"

/*======================================================================
 *  Local Variables
 *====================================================================*/

static boolean isInitialized = FALSE;

/*======================================================================
 *  API Implementations
 *====================================================================*/

uint8_t HAL_COMM_Init(void)
{
    UART_ConfigType uartConfig;
    
    /* 1. Enable peripheral clocks (MCAL handles enable + ready wait) */
    MCAL_GPIO_EnablePort(HAL_COMM_UART_PERIPH);
    MCAL_GPIO_EnablePort(HAL_COMM_GPIO_PERIPH);
    
    /* 2. Configure GPIO pins for UART functionality */
    /* PB0: U1RX (UART1 Receive) */
    /* PB1: U1TX (UART1 Transmit) */
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    
    /* Set pin type to UART */
    GPIOPinTypeUART(HAL_COMM_GPIO_PORT, HAL_COMM_RX_PIN | HAL_COMM_TX_PIN);
    
    /* 3. Configure UART parameters using MCAL layer */
    uartConfig.clockFreq = SysCtlClockGet();
    uartConfig.uartBase  = HAL_COMM_UART_MODULE;
    uartConfig.baudRate  = HAL_COMM_BAUD_RATE;
    uartConfig.dataBits  = 8U;
    uartConfig.parity    = 0U;  /* None */
    uartConfig.stopBits  = 1U;
    
    /* Initialize UART through MCAL */
    UART_init(&uartConfig);
    
    /* Small delay to ensure UART is fully initialized */
    SysCtlDelay(SysCtlClockGet() / (3U * 1000U));  /* ~1ms delay */
    
    isInitialized = TRUE;
    
    return HAL_COMM_SUCCESS;
}

void HAL_COMM_SendByte(uint8_t data)
{
    if (isInitialized)
    {
        sendByte(HAL_COMM_UART_MODULE, data);
    }
}

uint8_t HAL_COMM_ReceiveByte(void)
{
    if (isInitialized)
    {
        return receiveByte(HAL_COMM_UART_MODULE);
    }
    
    return 0U;
}

void HAL_COMM_SendString(const char *str)
{
    if ((isInitialized) && (str != NULL))
    {
        sendString(HAL_COMM_UART_MODULE, str);
    }
}

uint32_t HAL_COMM_ReceiveString(char *buffer, uint32_t maxLen)
{
    if ((isInitialized) && (buffer != NULL))
    {
        return receiveString(HAL_COMM_UART_MODULE, buffer, maxLen);
    }
    
    return 0U;
}

boolean HAL_COMM_IsDataAvailable(void)
{
    if (isInitialized)
    {
        /* Use MCAL function to check if characters are available */
        return isDataAvailable(HAL_COMM_UART_MODULE) ? TRUE : FALSE;
    }
    
    return FALSE;
}

void HAL_COMM_SendMessage(const char *message)
{
    if ((isInitialized) && (message != NULL))
    {
        HAL_COMM_SendString(message);
        HAL_COMM_SendString("\r\n");  /* Send CR+LF for new line */
    }
}