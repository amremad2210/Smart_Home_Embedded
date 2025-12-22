/*============================================================================
 *  Module      : HAL COMM
 *  File Name   : hal_comm.h
 *  Description : Hardware Abstraction Layer for UART Communication
 *===========================================================================*/

#ifndef HAL_COMM_H_
#define HAL_COMM_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "Types.h"

/*======================================================================
 *  Defines
 *====================================================================*/

/* UART Configuration */
#define HAL_COMM_UART_MODULE        UART1_BASE
#define HAL_COMM_UART_PERIPH        SYSCTL_PERIPH_UART1
#define HAL_COMM_GPIO_PERIPH        SYSCTL_PERIPH_GPIOB
#define HAL_COMM_GPIO_PORT          GPIO_PORTB_BASE
#define HAL_COMM_RX_PIN             GPIO_PIN_0     /* PB0 - U1RX */
#define HAL_COMM_TX_PIN             GPIO_PIN_1     /* PB1 - U1TX */
#define HAL_COMM_BAUD_RATE          115200U
#define HAL_COMM_SYSTEM_CLOCK       16000000U      /* 16 MHz */

/* Communication buffer sizes */
#define HAL_COMM_RX_BUFFER_SIZE     (64U)          /* Receive buffer size */
#define HAL_COMM_TX_BUFFER_SIZE     (64U)          /* Transmit buffer size */

/* Return codes */
#define HAL_COMM_SUCCESS            (0U)
#define HAL_COMM_ERROR_INIT         (1U)
#define HAL_COMM_ERROR_INVALID      (2U)
#define HAL_COMM_ERROR_BUFFER_FULL  (3U)

/*======================================================================
 *  API
 *====================================================================*/

/**
 * @brief Initialize the HAL communication module (UART1).
 *
 * This function:
 * - Enables UART1 and GPIOB peripheral clocks
 * - Configures PB0 (RX) and PB1 (TX) for UART1
 * - Initializes UART1 with 115200 baud, 8N1 configuration
 *
 * Must be called before any other HAL_COMM functions.
 *
 * @return HAL_COMM_SUCCESS if initialization succeeded
 *         HAL_COMM_ERROR_INIT if initialization failed
 */
uint8_t HAL_COMM_Init(void);

/**
 * @brief Send a single byte over UART.
 *
 * Blocking function that waits until the byte is transmitted.
 *
 * @param data  Byte to transmit
 */
void HAL_COMM_SendByte(uint8_t data);

/**
 * @brief Receive a single byte from UART.
 *
 * Blocking function that waits until a byte is received.
 *
 * @return Received byte
 */
uint8_t HAL_COMM_ReceiveByte(void);

/**
 * @brief Send a null-terminated string over UART.
 *
 * Blocking function that transmits the entire string.
 *
 * @param str  Pointer to null-terminated string
 */
void HAL_COMM_SendString(const char *str);

/**
 * @brief Receive a string from UART until CR/LF or buffer full.
 *
 * Blocking function that receives characters until:
 * - Carriage return ('\r') is received
 * - Line feed ('\n') is received
 * - Buffer is full (maxLen - 1 characters)
 *
 * The received string is null-terminated.
 *
 * @param buffer  Pointer to buffer to store received string
 * @param maxLen  Maximum buffer size (including null terminator)
 * @return Number of characters received (excluding null terminator)
 */
uint32_t HAL_COMM_ReceiveString(char *buffer, uint32_t maxLen);

/**
 * @brief Check if data is available in UART receive buffer.
 *
 * Non-blocking function to check if data can be read without waiting.
 *
 * @return TRUE if data is available
 *         FALSE if no data is available
 */
boolean HAL_COMM_IsDataAvailable(void);

/**
 * @brief Send formatted message over UART (for debugging).
 *
 * Convenience function that sends a message with newline.
 *
 * @param message  Pointer to null-terminated message string
 */
void HAL_COMM_SendMessage(const char *message);

#endif /* HAL_COMM_H_ */