/*============================================================================
 *  Mock MCAL EEPROM for Integration Testing
 *===========================================================================*/

#include <stdint.h>
#include <string.h>
#include "mcal/mcal_eeprom.h"

/* Mock EEPROM */
#define EEPROM_SIZE 1024
static uint8_t mock_eeprom[EEPROM_SIZE];

uint8_t MCAL_EEPROM_Init(void) {
    memset(mock_eeprom, 0xFF, EEPROM_SIZE); /* Default to erased */
    return EEPROM_SUCCESS;
}

uint32_t MCAL_EEPROM_GetSize(void) {
    return EEPROM_SIZE;
}

uint8_t MCAL_EEPROM_ReadWord(uint32_t address, uint32_t *pData) {
    if ((address >= EEPROM_SIZE) || (pData == NULL) || (address % 4 != 0)) {
        return EEPROM_ERROR_INVALID_PARAM;
    }
    *pData = *(uint32_t*)&mock_eeprom[address];
    return EEPROM_SUCCESS;
}

uint8_t MCAL_EEPROM_WriteWord(uint32_t address, uint32_t data) {
    if ((address >= EEPROM_SIZE) || (address % 4 != 0)) {
        return EEPROM_ERROR_INVALID_PARAM;
    }
    *(uint32_t*)&mock_eeprom[address] = data;
    return EEPROM_SUCCESS;
}

uint8_t MCAL_EEPROM_ReadBlock(uint32_t *pBuffer, uint32_t address, uint32_t count) {
    if ((pBuffer == NULL) || (address >= EEPROM_SIZE) || (address % 4 != 0) || 
        ((address + count * 4) > EEPROM_SIZE)) {
        return EEPROM_ERROR_INVALID_PARAM;
    }
    memcpy(pBuffer, &mock_eeprom[address], count * 4);
    return EEPROM_SUCCESS;
}

uint8_t MCAL_EEPROM_WriteBlock(const uint32_t *pData, uint32_t address, uint32_t count) {
    if ((pData == NULL) || (address >= EEPROM_SIZE) || (address % 4 != 0) || 
        ((address + count * 4) > EEPROM_SIZE)) {
        return EEPROM_ERROR_INVALID_PARAM;
    }
    memcpy(&mock_eeprom[address], pData, count * 4);
    return EEPROM_SUCCESS;
}

uint8_t MCAL_EEPROM_MassErase(void) {
    memset(mock_eeprom, 0xFF, EEPROM_SIZE);
    return EEPROM_SUCCESS;
}

uint32_t MCAL_EEPROM_GetStatus(void) {
    return 0; /* Mock status */
}