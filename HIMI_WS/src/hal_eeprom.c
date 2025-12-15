/*============================================================================
 *  Module      : HAL EEPROM
 *  File Name   : hal_eeprom.c
 *  Description : Hardware Abstraction Layer for EEPROM password management
 *===========================================================================*/

#include "hal/hal_eeprom.h"
#include "mcal/mcal_eeprom.h"

#include <string.h>

/*======================================================================
 *  Local Helper Functions
 *====================================================================*/

/**
 * @brief Compare two memory buffers.
 *
 * @param buf1      First buffer
 * @param buf2      Second buffer
 * @param length    Number of bytes to compare
 * @return TRUE if buffers match, FALSE otherwise
 */
static boolean prv_memcmp(const uint8_t *buf1, const uint8_t *buf2, uint8_t length)
{
    uint8_t i;
    
    for (i = 0U; i < length; i++)
    {
        if (buf1[i] != buf2[i])
        {
            return FALSE;
        }
    }
    
    return TRUE;
}

/*======================================================================
 *  API Implementations
 *====================================================================*/

uint8_t HAL_EEPROM_Init(void)
{
    uint8_t result;
    
    /* Initialize MCAL EEPROM driver */
    result = MCAL_EEPROM_Init();
    
    if (result != EEPROM_SUCCESS)
    {
        return HAL_EEPROM_ERROR_INIT;
    }
    
    return HAL_EEPROM_SUCCESS;
}

uint8_t HAL_EEPROM_StorePassword(const char *password, uint8_t length)
{
    uint8_t result;
    uint32_t wordBuffer[5];  /* 5 words = 20 bytes (enough for 16 char password + padding) */
    uint32_t lengthWord;
    uint32_t flagWord;
    uint8_t i;
    uint8_t numWords;
    
    /* Validate parameters */
    if (password == NULL)
    {
        return HAL_EEPROM_ERROR_INVALID_PARAM;
    }
    
    /* Check password length */
    if ((length < HAL_EEPROM_PASSWORD_MIN_LENGTH) || 
        (length > HAL_EEPROM_PASSWORD_MAX_LENGTH))
    {
        return HAL_EEPROM_ERROR_INVALID_LENGTH;
    }
    
    /* Clear buffer */
    for (i = 0U; i < 5U; i++)
    {
        wordBuffer[i] = 0U;
    }
    
    /* Copy password into word buffer (little-endian) */
    for (i = 0U; i < length; i++)
    {
        uint8_t wordIndex = i / 4U;
        uint8_t byteOffset = i % 4U;
        wordBuffer[wordIndex] |= ((uint32_t)password[i] << (byteOffset * 8U));
    }
    
    /* Calculate number of words to write (round up) */
    numWords = (length + 3U) / 4U;
    
    /* Write password data */
    result = MCAL_EEPROM_WriteBlock(wordBuffer, HAL_EEPROM_PASSWORD_START_ADDR, numWords);
    if (result != EEPROM_SUCCESS)
    {
        return HAL_EEPROM_ERROR_WRITE;
    }
    
    /* Write password length */
    lengthWord = (uint32_t)length;
    result = MCAL_EEPROM_WriteWord(HAL_EEPROM_PASSWORD_LENGTH_ADDR, lengthWord);
    if (result != EEPROM_SUCCESS)
    {
        return HAL_EEPROM_ERROR_WRITE;
    }
    
    /* Write password-set flag */
    flagWord = HAL_EEPROM_PASSWORD_FLAG_SET;
    result = MCAL_EEPROM_WriteWord(HAL_EEPROM_PASSWORD_SET_FLAG_ADDR, flagWord);
    if (result != EEPROM_SUCCESS)
    {
        return HAL_EEPROM_ERROR_WRITE;
    }
    
    return HAL_EEPROM_SUCCESS;
}

uint8_t HAL_EEPROM_ReadPassword(uint8_t *buffer, uint8_t *pLength)
{
    uint8_t result;
    uint32_t wordBuffer[5];  /* 5 words = 20 bytes */
    uint32_t lengthWord;
    uint8_t length;
    uint8_t i;
    uint8_t numWords;
    
    /* Validate parameters */
    if ((buffer == NULL) || (pLength == NULL))
    {
        return HAL_EEPROM_ERROR_INVALID_PARAM;
    }
    
    /* Check if password is set */
    if (!HAL_EEPROM_IsPasswordSet())
    {
        return HAL_EEPROM_ERROR_NOT_SET;
    }
    
    /* Read password length */
    result = MCAL_EEPROM_ReadWord(HAL_EEPROM_PASSWORD_LENGTH_ADDR, &lengthWord);
    if (result != EEPROM_SUCCESS)
    {
        return HAL_EEPROM_ERROR_READ;
    }
    
    length = (uint8_t)lengthWord;
    
    /* Validate length */
    if ((length < HAL_EEPROM_PASSWORD_MIN_LENGTH) || 
        (length > HAL_EEPROM_PASSWORD_MAX_LENGTH))
    {
        return HAL_EEPROM_ERROR_READ;
    }
    
    /* Calculate number of words to read */
    numWords = (length + 3U) / 4U;
    
    /* Read password data */
    result = MCAL_EEPROM_ReadBlock(wordBuffer, HAL_EEPROM_PASSWORD_START_ADDR, numWords);
    if (result != EEPROM_SUCCESS)
    {
        return HAL_EEPROM_ERROR_READ;
    }
    
    /* Extract bytes from word buffer (little-endian) */
    for (i = 0U; i < length; i++)
    {
        uint8_t wordIndex = i / 4U;
        uint8_t byteOffset = i % 4U;
        buffer[i] = (char)((wordBuffer[wordIndex] >> (byteOffset * 8U)) & 0xFFU);
    }
    
    /* Null-terminate the buffer */
    buffer[length] = '\0';
    
    /* Return the length */
    *pLength = length;
    
    return HAL_EEPROM_SUCCESS;
}

boolean HAL_EEPROM_VerifyPassword(const char *password, uint8_t length)
{
    uint8_t result;
    uint8_t storedPassword[HAL_EEPROM_PASSWORD_MAX_LENGTH + 1U];
    uint8_t storedLength;
    
    /* Validate parameters */
    if (password == NULL)
    {
        return FALSE;
    }
    
    /* Check if password is set */
    if (!HAL_EEPROM_IsPasswordSet())
    {
        return FALSE;
    }
    
    /* Read stored password */
    result = HAL_EEPROM_ReadPassword(storedPassword, &storedLength);
    if (result != HAL_EEPROM_SUCCESS)
    {
        return FALSE;
    }
    
    /* Check length first (quick reject) */
    if (length != storedLength)
    {
        return FALSE;
    }
    
    /* Compare passwords - cast to uint8_t* for comparison */
    return prv_memcmp((const uint8_t*)password, storedPassword, length);
}

boolean HAL_EEPROM_IsPasswordSet(void)
{
    uint8_t result;
    uint32_t flagWord;
    
    /* Read password-set flag */
    result = MCAL_EEPROM_ReadWord(HAL_EEPROM_PASSWORD_SET_FLAG_ADDR, &flagWord);
    
    if (result != EEPROM_SUCCESS)
    {
        return FALSE;
    }
    
    /* Check if flag matches the "set" magic value */
    if (flagWord == HAL_EEPROM_PASSWORD_FLAG_SET)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

uint8_t HAL_EEPROM_ClearPassword(void)
{
    uint8_t result;
    uint32_t clearWord = 0xFFFFFFFFU;
    uint32_t wordBuffer[5] = {0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU};
    
    /* Clear password data (write all 0xFF) */
    result = MCAL_EEPROM_WriteBlock(wordBuffer, HAL_EEPROM_PASSWORD_START_ADDR, 5U);
    if (result != EEPROM_SUCCESS)
    {
        return HAL_EEPROM_ERROR_WRITE;
    }
    
    /* Clear password length */
    result = MCAL_EEPROM_WriteWord(HAL_EEPROM_PASSWORD_LENGTH_ADDR, clearWord);
    if (result != EEPROM_SUCCESS)
    {
        return HAL_EEPROM_ERROR_WRITE;
    }
    
    /* Clear password-set flag */
    result = MCAL_EEPROM_WriteWord(HAL_EEPROM_PASSWORD_SET_FLAG_ADDR, HAL_EEPROM_PASSWORD_FLAG_CLEAR);
    if (result != EEPROM_SUCCESS)
    {
        return HAL_EEPROM_ERROR_WRITE;
    }
    
    return HAL_EEPROM_SUCCESS;
}

uint8_t HAL_EEPROM_ChangePassword(const char *oldPassword, uint8_t oldLength,
                                   const char *newPassword, uint8_t newLength)
{
    /* Validate parameters */
    if ((oldPassword == NULL) || (newPassword == NULL))
    {
        return HAL_EEPROM_ERROR_INVALID_PARAM;
    }
    
    /* Verify old password */
    if (!HAL_EEPROM_VerifyPassword(oldPassword, oldLength))
    {
        return HAL_EEPROM_ERROR_VERIFY_FAIL;
    }
    
    /* Store new password */
    return HAL_EEPROM_StorePassword(newPassword, newLength);
}