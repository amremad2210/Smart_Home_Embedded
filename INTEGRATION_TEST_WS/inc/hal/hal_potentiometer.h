#ifndef HAL_POTENTIOMETER_H_
#define HAL_POTENTIOMETER_H_

#include <stdint.h>

// Connected to PE3 (AIN0)
#define POT_PIN             3       /* PE3 */
#define POT_ADC_CHANNEL     0       /* AIN0 */


// Initializes the potentiometer (ADC on PE3)
void POT_Init(void);

uint16_t POT_ReadRaw(void);
uint32_t POT_ReadMillivolts(void);
uint8_t POT_ReadPercentage(void);

/* Averaged readings (noise reduction) */
uint16_t POT_ReadRawAveraged(uint8_t numSamples);
uint8_t POT_ReadPercentageAveraged(uint8_t numSamples);

// Maps the potentiometer reading to a custom range
uint32_t POT_ReadMapped(uint32_t min, uint32_t max);

#endif /* POTENTIOMETER_H_ */
