/*============================================================================
 *  Mock MCAL ADC for Integration Testing
 *===========================================================================*/

#include <stdint.h>
#include "mcal/mcal_adc.h"

void ADC_Init(uint8_t channel) {
    /* No-op */
}

uint16_t ADC_Read(void) {
    return 512; /* Mid value */
}

uint32_t ADC_ToMillivolts(uint16_t adcValue) {
    return (adcValue * 3300) / 4095; /* Assuming 3.3V reference */
}