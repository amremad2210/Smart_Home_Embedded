/*******************************************************************************
 * File: test_eeprom.c
 * Description: Unit tests for EEPROM driver
 * Author: Testing Team
 * Date: December 18, 2025
 ******************************************************************************/

#include "../TestFramework/test_logger.h"
#include "../../Common/inc/mcal/mcal_eeprom.h"

/*******************************************************************************
 *                              Test Functions                                 *
 *******************************************************************************/

void Test_EEPROM_Init(void) {
    uint8 result = MCAL_EEPROM_Init();
    TestLogger_Assert("UT-EEPROM-001", "EEPROM initialization", 
                      result == EEPROM_SUCCESS);
}

void Test_EEPROM_WriteRead_Byte(void) {
    uint32 testAddress = 0x0010;  /* Word-aligned address */
    uint32 testData = 0x12345678;
    uint32 readData;
    
    /* Write word (EEPROM works with 32-bit words) */
    TestLogger_Assert("UT-EEPROM-002", "EEPROM write word", 
                      MCAL_EEPROM_WriteWord(testAddress, testData) == EEPROM_SUCCESS);
    
    /* Small delay for write cycle */
    for (volatile int i = 0; i < 10000; i++);
    
    /* Read word */
    TestLogger_Assert("UT-EEPROM-003", "EEPROM read word", 
                      MCAL_EEPROM_ReadWord(testAddress, &readData) == EEPROM_SUCCESS);
    
    /* Verify data */
    TestLogger_Assert("UT-EEPROM-004", "EEPROM data integrity", 
                      readData == testData);
}

void Test_EEPROM_WriteRead_Block(void) {
    uint32 testAddress = 0x0020;  /* Word-aligned address */
    uint32 testBlock[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint32 readBlock[8] = {0};
    uint8 dataMatch = TRUE;
    
    /* Write block (8 words = 32 bytes) */
    TestLogger_Assert("UT-EEPROM-005", "EEPROM write block", 
                      MCAL_EEPROM_WriteBlock(testBlock, testAddress, 8) == EEPROM_SUCCESS);
    
    /* Delay for write cycle */
    for (volatile int i = 0; i < 20000; i++);
    
    /* Read block */
    TestLogger_Assert("UT-EEPROM-006", "EEPROM read block", 
                      MCAL_EEPROM_ReadBlock(readBlock, testAddress, 8) == EEPROM_SUCCESS);
    
    /* Verify data */
    for (int i = 0; i < 8; i++) {
        if (readBlock[i] != testBlock[i]) {
            dataMatch = FALSE;
            break;
        }
    }
    
    TestLogger_Assert("UT-EEPROM-007", "EEPROM block data integrity", 
                      dataMatch == TRUE);
}

void Test_EEPROM_BoundaryConditions(void) {
    uint32 dummy;
    uint32 eepromSize = MCAL_EEPROM_GetSize();
    
    /* Test size retrieval */
    TestLogger_Assert("UT-EEPROM-008", "EEPROM size retrieval", 
                      eepromSize > 0);
    
    /* Test valid address access */
    TestLogger_Assert("UT-EEPROM-009", "EEPROM valid address handling", 
                      MCAL_EEPROM_ReadWord(0x0000, &dummy) == EEPROM_SUCCESS);
}

void Test_EEPROM_ErrorHandling(void) {
    /* Test NULL pointer handling */
    TestLogger_Assert("UT-EEPROM-010", "EEPROM NULL pointer handling", 
                      MCAL_EEPROM_ReadWord(0x00, NULL) == EEPROM_ERROR_INVALID_PARAM);
}

/*******************************************************************************
 *                              Main Test Suite                                *
 *******************************************************************************/

void Run_EEPROM_UnitTests(void) {
    TestLogger_StartSuite("EEPROM Driver Unit Tests");
    
    Test_EEPROM_Init();
    Test_EEPROM_WriteRead_Byte();
    Test_EEPROM_WriteRead_Block();
    Test_EEPROM_BoundaryConditions();
    Test_EEPROM_ErrorHandling();
    
    TestLogger_EndSuite();
}
