/*============================================================================
 *  Module      : MCAL EEPROM
 *  File Name   : mcal_eeprom.h
 *  Description : EEPROM abstraction layer built on top of TivaWare driverlib
 *===========================================================================*/

#ifndef MCAL_EEPROM_H_
#define MCAL_EEPROM_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/eeprom.h"

/*======================================================================
 *  Defines
 *====================================================================*/

/* Return codes for EEPROM operations */
#define EEPROM_SUCCESS              (0U)
#define EEPROM_ERROR_INIT           (1U)
#define EEPROM_ERROR_READ           (2U)
#define EEPROM_ERROR_WRITE          (3U)
#define EEPROM_ERROR_INVALID_PARAM  (4U)
#define EEPROM_ERROR_TIMEOUT        (5U)

/* EEPROM addressing */
#define EEPROM_WORD_SIZE            (4U)    /* 4 bytes per word */
#define EEPROM_BLOCK_SIZE           (16U)   /* 16 words per block */

/*======================================================================
 *  API
 *====================================================================*/

/**
 * @brief Initialize the EEPROM peripheral.
 *
 * This function enables and initializes the EEPROM module using TivaWare's
 * EEPROMInit() function. Must be called before any other EEPROM operations.
 *
 * @return EEPROM_SUCCESS if initialization succeeded
 *         EEPROM_ERROR_INIT if initialization failed
 */
uint8_t MCAL_EEPROM_Init(void);

/**
 * @brief Get the size of the EEPROM in bytes.
 *
 * @return Size of EEPROM in bytes (typically 2KB for TM4C123)
 */
uint32_t MCAL_EEPROM_GetSize(void);

/**
 * @brief Read a single 32-bit word from EEPROM.
 *
 * @param address   Byte address to read from (must be word-aligned, i.e., multiple of 4)
 * @param pData     Pointer to store the read word
 * @return EEPROM_SUCCESS if read succeeded
 *         EEPROM_ERROR_INVALID_PARAM if parameters are invalid
 *         EEPROM_ERROR_READ if read operation failed
 */
uint8_t MCAL_EEPROM_ReadWord(uint32_t address, uint32_t *pData);

/**
 * @brief Write a single 32-bit word to EEPROM.
 *
 * @param address   Byte address to write to (must be word-aligned, i.e., multiple of 4)
 * @param data      Word value to write
 * @return EEPROM_SUCCESS if write succeeded
 *         EEPROM_ERROR_INVALID_PARAM if parameters are invalid
 *         EEPROM_ERROR_WRITE if write operation failed
 */
uint8_t MCAL_EEPROM_WriteWord(uint32_t address, uint32_t data);

/**
 * @brief Read multiple words from EEPROM.
 *
 * @param pBuffer   Pointer to buffer to store read data
 * @param address   Byte address to start reading from (must be word-aligned)
 * @param count     Number of 32-bit words to read
 * @return EEPROM_SUCCESS if read succeeded
 *         EEPROM_ERROR_INVALID_PARAM if parameters are invalid
 */
uint8_t MCAL_EEPROM_ReadBlock(uint32_t *pBuffer, uint32_t address, uint32_t count);

/**
 * @brief Write multiple words to EEPROM.
 *
 * @param pData     Pointer to data to write
 * @param address   Byte address to start writing to (must be word-aligned)
 * @param count     Number of 32-bit words to write
 * @return EEPROM_SUCCESS if write succeeded
 *         EEPROM_ERROR_INVALID_PARAM if parameters are invalid
 *         EEPROM_ERROR_WRITE if write operation failed
 */
uint8_t MCAL_EEPROM_WriteBlock(const uint32_t *pData, uint32_t address, uint32_t count);

/**
 * @brief Perform a mass erase of the entire EEPROM.
 *
 * WARNING: This erases all data in the EEPROM!
 *
 * @return EEPROM_SUCCESS if erase succeeded
 *         EEPROM_ERROR_WRITE if erase operation failed
 */
uint8_t MCAL_EEPROM_MassErase(void);

/**
 * @brief Get the current status of the EEPROM peripheral.
 *
 * @return Status flags from EEPROMStatusGet()
 */
uint32_t MCAL_EEPROM_GetStatus(void);

#endif /* MCAL_EEPROM_H_ */