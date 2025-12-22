/*============================================================================
 *  Mock MCAL UART for Integration Testing
 *  Simulates UART communication between HMI and Control ECUs using shared buffers
 *===========================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "mcal/mcal_uart.h"

/* Global flag to indicate which ECU is active */
bool is_hmi_active = false;

/* Mock UART buffers */
#define UART_BUFFER_SIZE 256
uint8_t hmi_to_control_buffer[UART_BUFFER_SIZE];
uint8_t control_to_hmi_buffer[UART_BUFFER_SIZE];
uint32_t hmi_to_control_head = 0;
uint32_t hmi_to_control_tail = 0;
uint32_t control_to_hmi_head = 0;
uint32_t control_to_hmi_tail = 0;

/* UART base for simulation */
#define UART1_BASE 0x4000D000  /* Mock base */

/* Mock UART init - does nothing */
void UART_init(const UART_ConfigType *cfg) {
    /* No-op for simulation */
}

/* Mock UARTSpaceAvail - always space available */
_Bool UARTSpaceAvail(uint32_t uartBase) {
    return 1;  /* Mock: always space */
}

/* Mock UARTCharsAvail - check buffer */
_Bool UARTCharsAvail(uint32_t uartBase) {
    if (uartBase == UART1_BASE) {
        if (is_hmi_active) {
            /* HMI checking for data from Control */
            return (control_to_hmi_tail != control_to_hmi_head);
        } else {
            /* Control checking for data from HMI */
            return (hmi_to_control_tail != hmi_to_control_head);
        }
    }
    return 0;
}

/* Mock send byte */
void sendByte(uint32_t uartBase, uint8_t data) {
    if (uartBase == UART1_BASE) {
        if (is_hmi_active) {
            /* HMI sending to Control */
            uint32_t head = hmi_to_control_head;
            uint32_t next = (head + 1) % UART_BUFFER_SIZE;
            hmi_to_control_buffer[head] = data;
            hmi_to_control_head = next;
            printf("UART MOCK: HMI sent 0x%02X to Control (head=%d, tail=%d)\n", data, hmi_to_control_head, hmi_to_control_tail);
        } else {
            /* Control sending to HMI */
            uint32_t head = control_to_hmi_head;
            uint32_t next = (head + 1) % UART_BUFFER_SIZE;
            control_to_hmi_buffer[head] = data;
            control_to_hmi_head = next;
            printf("UART MOCK: Control sent 0x%02X to HMI (head=%d, tail=%d)\n", data, control_to_hmi_head, control_to_hmi_tail);
        }
    }
}

/* Mock send string */
void sendString(uint32_t uartBase, const char *str) {
    while (*str) {
        sendByte(uartBase, (uint8_t)*str);
        str++;
    }
}

/* Mock isDataAvailable */
uint8_t isDataAvailable(uint32_t uartBase) {
    if (uartBase == UART1_BASE) {
        if (is_hmi_active) {
            /* HMI checking for data from Control */
            uint8_t available = (control_to_hmi_tail != control_to_hmi_head) ? 1 : 0;
            printf("UART MOCK: HMI checking data from Control: %d (head=%d, tail=%d)\n", available, control_to_hmi_head, control_to_hmi_tail);
            return available;
        } else {
            /* Control checking for data from HMI */
            uint8_t available = (hmi_to_control_tail != hmi_to_control_head) ? 1 : 0;
            printf("UART MOCK: Control checking data from HMI: %d (head=%d, tail=%d)\n", available, hmi_to_control_head, hmi_to_control_tail);
            return available;
        }
    }
    return 0;
}

/* Mock receive byte */
uint8_t receiveByte(uint32_t uartBase) {
    if (uartBase == UART1_BASE) {
        if (is_hmi_active) {
            /* HMI receiving from Control */
            uint32_t tail = control_to_hmi_tail;
            uint32_t head = control_to_hmi_head;
            if (tail != head) {
                uint8_t data = control_to_hmi_buffer[tail];
                uint32_t next = (tail + 1) % UART_BUFFER_SIZE;
                control_to_hmi_tail = next;
                return data;
            }
        } else {
            /* Control receiving from HMI */
            uint32_t tail = hmi_to_control_tail;
            uint32_t head = hmi_to_control_head;
            if (tail != head) {
                uint8_t data = hmi_to_control_buffer[tail];
                uint32_t next = (tail + 1) % UART_BUFFER_SIZE;
                hmi_to_control_tail = next;
                return data;
            }
        }
    }
    return 0; /* No data */
}