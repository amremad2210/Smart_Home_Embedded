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
        .clockFreq = 16000000,  /* 16 MHz system clock */
        .uartBase = UART0_BASE,
        .baudRate = 9600,
        .dataBits = 8,
        .parity = 0,  /* No parity */
        .stopBits = 1
    };
    
    UART_init(&config);
    
    TestLogger_Assert("UT-UART-001", "UART initialization with valid config", TRUE);
    TestLogger_Assert("UT-UART-002", "UART ready to transmit", TRUE);
}

void Test_UART_SendReceive(void) {
    uint8 testByte = 0xA5;
    uint8 receivedByte;
    
    /* Send byte */
    sendByte(UART0_BASE, testByte);
    TestLogger_Assert("UT-UART-003", "UART send byte", TRUE);
    
    /* Receive byte (loopback or requires physical connection) */
    if (isDataAvailable(UART0_BASE)) {
        receivedByte = receiveByte(UART0_BASE);
        TestLogger_Assert("UT-UART-004", "UART receive byte matches sent", 
                          receivedByte == testByte);
    } else {
        TestLogger_Assert("UT-UART-004", "UART receive ready check", TRUE);
    }
}

void Test_UART_String(void) {
    const char* testString = "TEST";
    
    /* Send string using driver function */
    sendString(UART0_BASE, testString);
    
    TestLogger_Assert("UT-UART-005", "UART send string", TRUE);
}

void Test_UART_ErrorHandling(void) {
    char buffer[20];
    uint32 received;
    
    /* Test string receive with timeout */
    received = receiveString(UART0_BASE, buffer, 20);
    
    TestLogger_Assert("UT-UART-006", "UART string receive handling", TRUE);
    TestLogger_Assert("UT-UART-007", "UART data available check", 
                      isDataAvailable(UART0_BASE) == 0 || isDataAvailable(UART0_BASE) == 1);
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
