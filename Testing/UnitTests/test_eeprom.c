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
    TestLogger_Assert("UT-EEPROM-001", "EEPROM initialization", 
                      EEPROM_Init() == TRUE);
}

void Test_EEPROM_WriteRead_Byte(void) {
    uint16 testAddress = 0x0010;
    uint8 testData = 0x42;
    uint8 readData;
    
    /* Write byte */
    TestLogger_Assert("UT-EEPROM-002", "EEPROM write byte", 
                      EEPROM_WriteByte(testAddress, testData) == TRUE);
    
    /* Small delay for write cycle */
    for (volatile int i = 0; i < 10000; i++);
    
    /* Read byte */
    TestLogger_Assert("UT-EEPROM-003", "EEPROM read byte", 
                      EEPROM_ReadByte(testAddress, &readData) == TRUE);
    
    /* Verify data */
    TestLogger_Assert("UT-EEPROM-004", "EEPROM data integrity", 
                      readData == testData);
}

void Test_EEPROM_WriteRead_Block(void) {
    uint16 testAddress = 0x0020;
    uint8 testBlock[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8 readBlock[8] = {0};
    uint8 dataMatch = TRUE;
    
    /* Write block */
    TestLogger_Assert("UT-EEPROM-005", "EEPROM write block", 
                      EEPROM_WriteBlock(testAddress, testBlock, 8) == TRUE);
    
    /* Delay for write cycle */
    for (volatile int i = 0; i < 20000; i++);
    
    /* Read block */
    TestLogger_Assert("UT-EEPROM-006", "EEPROM read block", 
                      EEPROM_ReadBlock(testAddress, readBlock, 8) == TRUE);
    
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
    uint8 dummy;
    
    /* Test address boundary */
    TestLogger_Assert("UT-EEPROM-008", "EEPROM max address handling", 
                      EEPROM_ReadByte(EEPROM_MAX_ADDRESS - 1, &dummy) == TRUE);
    
    /* Test invalid address */
    TestLogger_Assert("UT-EEPROM-009", "EEPROM invalid address rejection", 
                      EEPROM_ReadByte(EEPROM_MAX_ADDRESS + 1, &dummy) == FALSE);
}

void Test_EEPROM_ErrorHandling(void) {
    /* Test NULL pointer handling */
    TestLogger_Assert("UT-EEPROM-010", "EEPROM NULL pointer handling", 
                      EEPROM_ReadByte(0x00, NULL) == FALSE);
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
