/*============================================================================
 *  Mock MCAL I2C for Integration Testing
 *===========================================================================*/

#include <stdint.h>
#include "mcal/mcal_i2c.h"

void I2C0_Init(void) {
    /* No-op */
}

void I2C0_WriteByte(uint8_t SlaveAddress, uint8_t Data) {
    /* No-op */
}