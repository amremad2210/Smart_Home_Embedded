/*============================================================================
 *  Module      : HAL EEPROM
 *  File Name   : hal_eeprom.h
 *  Description : Hardware Abstraction Layer for EEPROM password management
 *===========================================================================*/

#ifndef HAL_EEPROM_H_
#define HAL_EEPROM_H_

#include <stdint.h>
#include <stdbool.h>
#include "Types.h"

/*======================================================================
 *  Defines
 *====================================================================*/

/* Password configuration */
#define HAL_EEPROM_PASSWORD_MAX_LENGTH      (16U)   /* Maximum password length in characters */
#define HAL_EEPROM_PASSWORD_MIN_LENGTH      (4U)    /* Minimum password length in characters */

/* EEPROM memory layout */
#define HAL_EEPROM_PASSWORD_START_ADDR      (0U)    /* Password storage start address */
#define HAL_EEPROM_PASSWORD_LENGTH_ADDR     (20U)   /* Password length storage address */
#define HAL_EEPROM_PASSWORD_SET_FLAG_ADDR   (24U)   /* Flag indicating if password is set */

/* Password set flag values */
#define HAL_EEPROM_PASSWORD_FLAG_SET        (0xA5A5A5A5U)  /* Magic value: password is set */
#define HAL_EEPROM_PASSWORD_FLAG_CLEAR      (0xFFFFFFFFU)  /* Magic value: no password set */

/* Return codes */
#define HAL_EEPROM_SUCCESS                  (0U)
#define HAL_EEPROM_ERROR_INIT               (1U)
#define HAL_EEPROM_ERROR_NOT_SET            (2U)   /* Password not set yet */
#define HAL_EEPROM_ERROR_INVALID_LENGTH     (3U)   /* Password length invalid */
#define HAL_EEPROM_ERROR_INVALID_PARAM      (4U)
#define HAL_EEPROM_ERROR_WRITE              (5U)
#define HAL_EEPROM_ERROR_READ               (6U)
#define HAL_EEPROM_ERROR_VERIFY_FAIL        (7U)   /* Password verification failed */

/*======================================================================
 *  API
 *====================================================================*/

/**
 * @brief Initialize the HAL EEPROM module.
 *
 * This function initializes the underlying MCAL EEPROM driver.
 * Must be called before any other HAL_EEPROM functions.
 *
 * @return HAL_EEPROM_SUCCESS if initialization succeeded
 *         HAL_EEPROM_ERROR_INIT if initialization failed
 */
uint8_t HAL_EEPROM_Init(void);

/**
 * @brief Store a password to EEPROM.
 *
 * The password is stored with its length and a flag indicating it's set.
 * Password length must be between HAL_EEPROM_PASSWORD_MIN_LENGTH and
 * HAL_EEPROM_PASSWORD_MAX_LENGTH.
 *
 * @param password  Pointer to password string (does not need to be null-terminated)
 * @param length    Length of the password in bytes
 * @return HAL_EEPROM_SUCCESS if password stored successfully
 *         HAL_EEPROM_ERROR_INVALID_PARAM if password pointer is NULL
 *         HAL_EEPROM_ERROR_INVALID_LENGTH if length is out of range
 *         HAL_EEPROM_ERROR_WRITE if write operation failed
 */
uint8_t HAL_EEPROM_StorePassword(const char *password, uint8_t length);

/**
 * @brief Read the stored password from EEPROM.
 *
 * Reads the password from EEPROM into the provided buffer.
 * The buffer must be at least HAL_EEPROM_PASSWORD_MAX_LENGTH bytes.
 *
 * @param buffer    Pointer to buffer to store password (will be null-terminated)
 * @param pLength   Pointer to store the actual password length
 * @return HAL_EEPROM_SUCCESS if password read successfully
 *         HAL_EEPROM_ERROR_INVALID_PARAM if pointers are NULL
 *         HAL_EEPROM_ERROR_NOT_SET if no password is set
 *         HAL_EEPROM_ERROR_READ if read operation failed
 */
uint8_t HAL_EEPROM_ReadPassword(char *buffer, uint8_t *pLength);

/**
 * @brief Verify if the provided password matches the stored password.
 *
 * Compares the provided password with the one stored in EEPROM.
 *
 * @param password  Pointer to password string to verify
 * @param length    Length of the provided password
 * @return TRUE if password matches
 *         FALSE if password doesn't match or not set
 */
boolean HAL_EEPROM_VerifyPassword(const char *password, uint8_t length);

/**
 * @brief Check if a password is currently set in EEPROM.
 *
 * Useful for first-time setup detection.
 *
 * @return TRUE if password is set
 *         FALSE if no password is set
 */
boolean HAL_EEPROM_IsPasswordSet(void);

/**
 * @brief Clear the stored password from EEPROM.
 *
 * Erases the password and resets the password-set flag.
 * Use for factory reset or password removal.
 *
 * @return HAL_EEPROM_SUCCESS if cleared successfully
 *         HAL_EEPROM_ERROR_WRITE if clear operation failed
 */
uint8_t HAL_EEPROM_ClearPassword(void);

/**
 * @brief Change the existing password.
 *
 * Verifies the old password before storing the new one.
 *
 * @param oldPassword   Pointer to current password for verification
 * @param oldLength     Length of current password
 * @param newPassword   Pointer to new password to store
 * @param newLength     Length of new password
 * @return HAL_EEPROM_SUCCESS if password changed successfully
 *         HAL_EEPROM_ERROR_VERIFY_FAIL if old password doesn't match
 *         HAL_EEPROM_ERROR_INVALID_LENGTH if new password length invalid
 *         HAL_EEPROM_ERROR_WRITE if write operation failed
 */
uint8_t HAL_EEPROM_ChangePassword(const char *oldPassword, uint8_t oldLength,
                                   const char *newPassword, uint8_t newLength);

#endif /* HAL_EEPROM_H_ */