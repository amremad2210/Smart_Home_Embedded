/*============================================================================
 *  Module      : MCAL UART
 *  File Name   : mcal_uart.h
 *  Description : UART abstraction layer built on top of TivaWare driverlib
 *===========================================================================*/

#ifndef MCAL_UART_H_
#define MCAL_UART_H_

#include <stdbool.h>
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"

/*======================================================================
 *  Types
 *====================================================================*/
typedef struct
{
	uint32_t clockFreq;   /* Peripheral clock driving the UART (e.g. SysCtlClockGet()) */
	uint32_t uartBase;    /* UART0_BASE, UART1_BASE, ... */
	uint32_t baudRate;    /* 9600, 115200, etc. */
	uint8_t  dataBits;    /* 5..8 (UART supports up to 8) */
	uint8_t  parity;      /* 0=None, 1=Even, 2=Odd, 3=Forced One, 4=Forced Zero */
	uint8_t  stopBits;    /* 1 or 2 */
} UART_ConfigType;


/*======================================================================
 *  API
 *====================================================================*/
/**
 * @brief Initialize a UART with baud, parity and stop-bits, and enable it.
 *
 * @param cfg Pointer to configuration describing clock, base, baud and format.
 */
void UART_init(const UART_ConfigType *cfg);

/**
 * @brief Blocking send of one byte on a UART.
 */
void sendByte(uint32_t uartBase, uint8_t data);

/**
 * @brief Blocking receive of one byte from a UART.
 */
uint8_t receiveByte(uint32_t uartBase);

/**
 * @brief Blocking send of a null-terminated string.
 */
void sendString(uint32_t uartBase, const char *str);

/**
 * @brief Blocking receive of a string until CR/LF or buffer full.
 *
 * Stores up to (maxLen - 1) chars and null-terminates.
 * Returns number of characters stored (excluding terminator).
 */
uint32_t receiveString(uint32_t uartBase, char *buffer, uint32_t maxLen);

#endif /* MCAL_UART_H_ */
