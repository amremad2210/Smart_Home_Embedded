#ifndef HAL_RGB_LED_H_
#define HAL_RGB_LED_H_

#include <stdint.h>

void RGB_LED_Init(void);
void RGB_LED_SetColor(uint8_t red, uint8_t green, uint8_t blue);
void RGB_LED_On(void);
void RGB_LED_Off(void);

#endif