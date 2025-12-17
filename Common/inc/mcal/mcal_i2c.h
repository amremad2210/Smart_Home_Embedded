#ifndef I2C_H
#define I2C_H
#include <stdbool.h>
#include <stdint.h>

void I2C0_Init(void);
void I2C0_WriteByte(uint8_t SlaveAddress, uint8_t Data);

#endif