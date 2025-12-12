/*============================================================================
 *  Module      : MCAL EEPROM
 *  File Name   : mcal_eeprom.c
 *  Description : EEPROM abstraction layer built on top of TivaWare driverlib
 *===========================================================================*/

#include "mcal/mcal_eeprom.h"
#include "mcal/mcal_gpio.h"

/*======================================================================
 *  API implementations
 *====================================================================*/

uint8_t MCAL_EEPROM_Init(void)
{
    uint32_t result;
    
   
    /* Enable EEPROM0 peripheral clock */
    MCAL_GPIO_EnablePort(SYSCTL_PERIPH_EEPROM0);
    
    /* Initialize the EEPROM module */
    result = EEPROMInit();
    
    /* Check initialization result */
    if (result == EEPROM_INIT_OK)
    {
        return EEPROM_SUCCESS;
    }
    else
    {
        return EEPROM_ERROR_INIT;
    }
}

uint32_t MCAL_EEPROM_GetSize(void)
{
    /* Return the size of the EEPROM in bytes using TivaWare function */
    return EEPROMSizeGet();
}

uint8_t MCAL_EEPROM_ReadWord(uint32_t address, uint32_t *pData)
{
    /* Validate parameters */
    if (pData == NULL)
    {
        return EEPROM_ERROR_INVALID_PARAM;
    }
    
    /* Check word alignment (address must be multiple of 4) */
    if ((address % EEPROM_WORD_SIZE) != 0U)
    {
        return EEPROM_ERROR_INVALID_PARAM;
    }
    
    /* Check address is within valid range */
    if (address >= EEPROMSizeGet())
    {
        return EEPROM_ERROR_INVALID_PARAM;
    }
    
    /* Read the word using TivaWare function */
    EEPROMRead(pData, address, EEPROM_WORD_SIZE);
    
    return EEPROM_SUCCESS;
}

uint8_t MCAL_EEPROM_WriteWord(uint32_t address, uint32_t data)
{
    uint32_t result;
    
    /* Check word alignment (address must be multiple of 4) */
    if ((address % EEPROM_WORD_SIZE) != 0U)
    {
        return EEPROM_ERROR_INVALID_PARAM;
    }
    
    /* Check address is within valid range */
    if (address >= EEPROMSizeGet())
    {
        return EEPROM_ERROR_INVALID_PARAM;
    }
    
    /* Write the word using TivaWare function */
    result = EEPROMProgram(&data, address, EEPROM_WORD_SIZE);
    
    /* Check if write was successful */
    if (result == 0U)
    {
        return EEPROM_SUCCESS;
    }
    else
    {
        return EEPROM_ERROR_WRITE;
    }
}

uint8_t MCAL_EEPROM_ReadBlock(uint32_t *pBuffer, uint32_t address, uint32_t count)
{
    uint32_t bytesToRead;
    
    /* Validate parameters */
    if (pBuffer == NULL)
    {
        return EEPROM_ERROR_INVALID_PARAM;
    }
    
    if (count == 0U)
    {
        return EEPROM_ERROR_INVALID_PARAM;
    }
    
    /* Check word alignment */
    if ((address % EEPROM_WORD_SIZE) != 0U)
    {
        return EEPROM_ERROR_INVALID_PARAM;
    }
    
    /* Calculate bytes to read */
    bytesToRead = count * EEPROM_WORD_SIZE;
    
    /* Check address + size is within valid range */
    if ((address + bytesToRead) > EEPROMSizeGet())
    {
        return EEPROM_ERROR_INVALID_PARAM;
    }
    
    /* Read the block using TivaWare function */
    EEPROMRead(pBuffer, address, bytesToRead);
    
    return EEPROM_SUCCESS;
}

uint8_t MCAL_EEPROM_WriteBlock(const uint32_t *pData, uint32_t address, uint32_t count)
{
    uint32_t result;
    uint32_t bytesToWrite;
    
    /* Validate parameters */
    if (pData == NULL)
    {
        return EEPROM_ERROR_INVALID_PARAM;
    }
    
    if (count == 0U)
    {
        return EEPROM_ERROR_INVALID_PARAM;
    }
    
    /* Check word alignment */
    if ((address % EEPROM_WORD_SIZE) != 0U)
    {
        return EEPROM_ERROR_INVALID_PARAM;
    }
    
    /* Calculate bytes to write */
    bytesToWrite = count * EEPROM_WORD_SIZE;
    
    /* Check address + size is within valid range */
    if ((address + bytesToWrite) > EEPROMSizeGet())
    {
        return EEPROM_ERROR_INVALID_PARAM;
    }
    
    /* Write the block using TivaWare function */
    result = EEPROMProgram((uint32_t *)pData, address, bytesToWrite);
    
    /* Check if write was successful */
    if (result == 0U)
    {
        return EEPROM_SUCCESS;
    }
    else
    {
        return EEPROM_ERROR_WRITE;
    }
}

uint8_t MCAL_EEPROM_MassErase(void)
{
    uint32_t result;
    
    /* Perform mass erase using TivaWare function */
    result = EEPROMMassErase();
    
    /* Check if erase was successful */
    if (result == 0U)
    {
        return EEPROM_SUCCESS;
    }
    else
    {
        return EEPROM_ERROR_WRITE;
    }
}

uint32_t MCAL_EEPROM_GetStatus(void)
{
    /* Return the current EEPROM status using TivaWare function */
    return EEPROMStatusGet();
}