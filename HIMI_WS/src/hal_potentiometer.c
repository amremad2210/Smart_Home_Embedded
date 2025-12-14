#include "hal_potentiometer.h"
#include "mcal/mcal_adc.h"

// Initializes the potentiometer (ADC on PE3)
void POT_Init(void)
{
    /* Initialize ADC with channel 0 (PE3 = AIN0) */
    ADC_Init(POT_ADC_CHANNEL);
}

uint16_t POT_ReadRaw(void)
{
    return ADC_Read();
}

uint32_t POT_ReadMillivolts(void)
{
    uint16_t rawValue = ADC_Read();
    return ADC_ToMillivolts(rawValue);
}

uint8_t POT_ReadPercentage(void)
{
    uint16_t rawValue = ADC_Read();
    /* Convert to percentage: (rawValue * 100) / 4095 */
    return (uint8_t)((rawValue * 100UL) / 4095UL);
}

// Maps the potentiometer reading to a custom range
uint32_t POT_ReadMapped(uint32_t min, uint32_t max)
{
    uint16_t rawValue = ADC_Read();
    
    /* Map from 0-4095 to min-max range */
    return min + ((rawValue * (max - min)) / 4095UL);
}
