/*============================================================================
 *  Module      : MCAL UART
 *  File Name   : mcal_uart.c
 *  Description : UART abstraction layer built on top of TivaWare driverlib
 *===========================================================================*/

#include "mcal/mcal_uart.h"

/*======================================================================
 *  API implementations
 *====================================================================*/

void UART_init(const UART_ConfigType *cfg)
{
	uint32_t config = 0U;

	/* Data bits: UART supports 5..8. Default to 8 if out of range. */
	switch (cfg->dataBits)
	{
		case 5:  config |= UART_CONFIG_WLEN_5; break;
		case 6:  config |= UART_CONFIG_WLEN_6; break;
		case 7:  config |= UART_CONFIG_WLEN_7; break;
		case 8:
		default: config |= UART_CONFIG_WLEN_8; break;
	}

	/* Stop bits */
	config |= (cfg->stopBits == 2U) ? UART_CONFIG_STOP_TWO : UART_CONFIG_STOP_ONE;

	/* Parity */
	switch (cfg->parity)
	{
		case 1:  config |= UART_CONFIG_PAR_EVEN; break;
		case 2:  config |= UART_CONFIG_PAR_ODD;  break;
		case 3:  config |= UART_CONFIG_PAR_ONE;  break;
		case 4:  config |= UART_CONFIG_PAR_ZERO; break;
		case 0:
		default: config |= UART_CONFIG_PAR_NONE; break;
	}

	/* Configure baud rate and frame format; this enables the UART internally */
	UARTConfigSetExpClk(cfg->uartBase, cfg->clockFreq, cfg->baudRate, config);

	/* Make sure FIFOs and UART are enabled */
	UARTFIFOEnable(cfg->uartBase);
	UARTEnable(cfg->uartBase);
}

void sendByte(uint32_t uartBase, uint8_t data)
{
	/* Wait until space is available */
	while (!UARTSpaceAvail(uartBase)) { }

	/* Send the byte */
	UARTCharPut(uartBase, data);
}

uint8_t receiveByte(uint32_t uartBase)
{
	/* Wait until data is available */
	while (!UARTCharsAvail(uartBase)) { }

	/* Read the byte */
	return (uint8_t)UARTCharGet(uartBase);
}

void sendString(uint32_t uartBase, const char *str)
{
	while (*str != '\0')
	{
		sendByte(uartBase, (uint8_t)(*str));
		str++;
	}
}

uint32_t receiveString(uint32_t uartBase, char *buffer, uint32_t maxLen)
{
	uint32_t idx = 0U;

	if (maxLen == 0U)
	{
		return 0U;
	}

	/* Read until CR/LF or buffer is full (leave space for null terminator) */
	while (idx < (maxLen - 1U))
	{
		uint8_t c = receiveByte(uartBase);
		if ((c == '\r') || (c == '\n'))
		{
			break;
		}
		buffer[idx++] = (char)c;
	}

	buffer[idx] = '\0';
	return idx;
}

