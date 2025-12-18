/*******************************************************************************
 * File: test_uart_communication.c
 * Description: Integration tests for HMI-Control ECU UART communication
 * Author: Testing Team
 * Date: December 18, 2025
 ******************************************************************************/

#include "../TestFramework/test_logger.h"
#include "../../HIMI_WS/inc/hal/hal_comm.h"
#include "../../CONTROL_WS/inc/hal/hal_comm.h"
#include <string.h>

/*******************************************************************************
 *                              Test Functions                                 *
 *******************************************************************************/

void Test_UART_BasicCommunication(void) {
    uint8 txData[] = {0x01, 0x02, 0x03, 0x04};
    uint8 rxData[4] = {0};
    
    /* Send frame from HMI to Control */
    Comm_SendFrame(txData, 4);
    
    /* Small delay for transmission */
    for (volatile int i = 0; i < 50000; i++);
    
    /* Receive frame */
    uint8 bytesReceived = Comm_ReceiveFrame(rxData, 4, 1000);
    
    TestLogger_Assert("IT-UART-001", "UART basic frame transmission", 
                      bytesReceived == 4);
    
    /* Verify data integrity */
    uint8 dataMatch = TRUE;
    for (int i = 0; i < 4; i++) {
        if (rxData[i] != txData[i]) {
            dataMatch = FALSE;
            break;
        }
    }
    
    TestLogger_Assert("IT-UART-002", "UART data integrity", 
                      dataMatch == TRUE);
}

void Test_UART_ProtocolFrames(void) {
    /* Test password verification frame */
    uint8 passwordFrame[6] = {0xAA, 0x01, 0x12, 0x34, 0x56, 0x78};
    uint8 responseFrame[2] = {0};
    
    Comm_SendFrame(passwordFrame, 6);
    for (volatile int i = 0; i < 50000; i++);
    
    uint8 received = Comm_ReceiveFrame(responseFrame, 2, 1000);
    
    TestLogger_Assert("IT-UART-003", "UART protocol frame structure", 
                      received > 0 && responseFrame[0] == 0xAA);
}

void Test_UART_TimeoutHandling(void) {
    uint8 rxData[10];
    uint32 startTime, endTime;
    
    startTime = SystemTick_GetTick();
    uint8 received = Comm_ReceiveFrame(rxData, 10, 500);  /* 500ms timeout */
    endTime = SystemTick_GetTick();
    
    TestLogger_Assert("IT-UART-004", "UART timeout functionality", 
                      received == 0 && (endTime - startTime) >= 500);
}

void Test_UART_ErrorRecovery(void) {
    /* Test recovery from corrupted frame */
    uint8 corruptedFrame[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    
    Comm_SendFrame(corruptedFrame, 5);
    for (volatile int i = 0; i < 50000; i++);
    
    /* System should reject and continue */
    TestLogger_Assert("IT-UART-005", "UART error recovery", 
                      Comm_GetErrorStatus() != COMM_FATAL_ERROR);
}

void Test_UART_LostByteHandling(void) {
    /* Simulate partial frame transmission */
    uint8 partialFrame[3] = {0xAA, 0x02, 0x10};
    uint8 rxData[10];
    
    Comm_SendFrame(partialFrame, 3);
    for (volatile int i = 0; i < 50000; i++);
    
    /* Should timeout waiting for complete frame */
    uint8 received = Comm_ReceiveFrame(rxData, 6, 500);
    
    TestLogger_Assert("IT-UART-006", "UART lost byte detection", 
                      received < 6);
}

void Test_UART_HighTraffic(void) {
    uint8 success = TRUE;
    
    /* Send multiple frames rapidly */
    for (int i = 0; i < 10; i++) {
        uint8 frame[2] = {0xAA, (uint8)i};
        if (Comm_SendFrame(frame, 2) == FALSE) {
            success = FALSE;
            break;
        }
        for (volatile int j = 0; j < 10000; j++);
    }
    
    TestLogger_Assert("IT-UART-007", "UART high traffic handling", 
                      success == TRUE);
}

/*******************************************************************************
 *                              Main Test Suite                                *
 *******************************************************************************/

void Run_UART_IntegrationTests(void) {
    TestLogger_StartSuite("UART Communication Integration Tests");
    
    Test_UART_BasicCommunication();
    Test_UART_ProtocolFrames();
    Test_UART_TimeoutHandling();
    Test_UART_ErrorRecovery();
    Test_UART_LostByteHandling();
    Test_UART_HighTraffic();
    
    TestLogger_EndSuite();
}
