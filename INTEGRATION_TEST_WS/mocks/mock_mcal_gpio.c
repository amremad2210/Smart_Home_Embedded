/*============================================================================
 *  Mock MCAL GPIO for Integration Testing
 *===========================================================================*/

#include <stdint.h>
#include <stdbool.h>

#include "mcal/mcal_gpio.h"

/* Mock GPIO - stubs */
void MCAL_GPIO_EnablePort(uint32_t periph) {
    /* No-op */
}

void MCAL_GPIO_InitPin(uint32_t portBase, uint8_t pins, GPIO_DirectionType dir, GPIO_InternalAttachType attach) {
    /* No-op */
}

void MCAL_GPIO_WritePin(uint32_t portBase, uint8_t pins, uint8_t value) {
    /* No-op */
}

uint8_t MCAL_GPIO_ReadPin(uint32_t portBase, uint8_t pins) {
    return 0; /* Default */
}

void MCAL_GPIO_TogglePin(uint32_t portBase, uint8_t pins) {
    /* No-op */
}

void PORTF_Handler(void) {
    /* No-op */
}