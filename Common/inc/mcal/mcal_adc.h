#ifndef HAL_ADC_H_
#define HAL_ADC_H_

#include <stdint.h>

#define ADC_MODULE_0        0
#define ADC_MODULE_1        1

#define ADC_SS0             0
#define ADC_SS1             1
#define ADC_SS2             2
#define ADC_SS3             3

#define ADC_CHANNEL_0       0
#define ADC_CHANNEL_1       1
#define ADC_CHANNEL_2       2
#define ADC_CHANNEL_3       3
#define ADC_CHANNEL_4       4
#define ADC_CHANNEL_5       5
#define ADC_CHANNEL_6       6
#define ADC_CHANNEL_7       7
#define ADC_CHANNEL_8       8
#define ADC_CHANNEL_9       9
#define ADC_CHANNEL_10      10
#define ADC_CHANNEL_11      11

#define ADC_MAX_VALUE       4095

// Initializes ADC0 Sequencer 3 for single-ended sampling
void ADC_Init(uint8_t channel);

// Reads a single sample from the ADC
uint16_t ADC_Read(void);

// Converts ADC value to millivolts (assuming 3.3V reference)
uint32_t ADC_ToMillivolts(uint16_t adcValue);

#endif 
