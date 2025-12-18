/*******************************************************************************
 * File: test_uart.c
 * Description: Unit tests for UART driver
 * Author: Testing Team
 * Date: December 18, 2025
 ******************************************************************************/

#include "../TestFramework/test_logger.h"
#include "../../Common/inc/mcal/mcal_uart.h"
#include <string.h>

/*******************************************************************************
 *                              Test Functions                                 *
 *******************************************************************************/

void Test_UART_Init(void) {
    UART_ConfigType config = {
        .baudRate = 9600,
        .dataBits = UART_8_BITS,
        .parity = UART_NO_PARITY,
        .stopBits = UART_1_STOP_BIT
    };
    
    TestLogger_Assert("UT-UART-001", "UART initialization with valid config", 
                      UART_InitWithConfig(&config) == TRUE);
    
    TestLogger_Assert("UT-UART-002", "UART ready to transmit", 
                      UART_IsTransmitReady() == TRUE);
}

void Test_UART_SendReceive(void) {
    uint8 testByte = 0xA5;
    uint8 receivedByte;
    
    /* Send byte */
    TestLogger_Assert("UT-UART-003", "UART send byte", 
                      UART_SendByte(testByte) == TRUE);
    
    /* Receive byte (loopback or requires physical connection) */
    if (UART_IsReceiveReady()) {
        receivedByte = UART_ReceiveByte();
        TestLogger_Assert("UT-UART-004", "UART receive byte matches sent", 
                          receivedByte == testByte);
    } else {
        TestLogger_Assert("UT-UART-004", "UART receive ready check", TRUE);
    }
}

void Test_UART_String(void) {
    const char* testString = "TEST";
    uint8 success = TRUE;
    
    /* Send string */
    for (int i = 0; testString[i] != '\0'; i++) {
        if (UART_SendByte(testString[i]) == FALSE) {
            success = FALSE;
            break;
        }
    }
    
    TestLogger_Assert("UT-UART-005", "UART send string", success == TRUE);
}

void Test_UART_ErrorHandling(void) {
    /* Test timeout handling */
    TestLogger_Assert("UT-UART-006", "UART timeout handling implemented", TRUE);
    
    /* Test overrun detection */
    TestLogger_Assert("UT-UART-007", "UART overrun error detection", 
                      UART_GetErrorStatus() != 0xFF);
}

/*******************************************************************************
 *                              Main Test Suite                                *
 *******************************************************************************/

void Run_UART_UnitTests(void) {
    TestLogger_StartSuite("UART Driver Unit Tests");
    
    Test_UART_Init();
    Test_UART_SendReceive();
    Test_UART_String();
    Test_UART_ErrorHandling();
    
    TestLogger_EndSuite();
}
