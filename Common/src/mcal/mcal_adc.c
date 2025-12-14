#include "mcal_adc.h"
#include "tm4c123gh6pm.h"
#include "mcal/mcal_gpio.h"

// Initializes ADC0 Sequencer 3 for single-ended sampling
void ADC_Init(uint8_t channel)
{
    /* Enable ADC0 peripheral via MCAL helper */
    MCAL_GPIO_EnablePort(SYSCTL_PERIPH_ADC0);
    
    /* Enable GPIO Port E clock via MCAL helper */
    MCAL_GPIO_EnablePort(SYSCTL_PERIPH_GPIOE);

    /* Configure PE3 as analog input using MCAL + driverlib */
    MCAL_GPIO_InitPin(GPIO_PORTE_BASE, GPIO_PIN_3, GPIO_DIR_INPUT, GPIO_ATTACH_DEFAULT);
    /* Set pin type to ADC (configures AFSEL/AMSEL appropriately) */
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
    
    /* Configure ADC0 */
    ADC0_ACTSS_R &= ~0x08;              
    ADC0_EMUX_R &= ~0xF000;             
    ADC0_SSMUX3_R = (channel & 0x0F);   
    ADC0_SSCTL3_R = 0x06;               
    ADC0_SAC_R = 0x00;                  
    ADC0_CTL_R &= ~0x01;                
    ADC0_ACTSS_R |= 0x08;               
}

// Reads a single sample from the ADC
uint16_t ADC_Read(void)
{
    uint16_t result;
    volatile uint32_t delay;
    
    ADC0_PSSI_R = 0x08;                 /* Initiate SS3 conversion */
    while((ADC0_RIS_R & 0x08) == 0);    /* Wait for conversion complete */
    result = ADC0_SSFIFO3_R & 0xFFF;    /* Read 12-bit result */
    ADC0_ISC_R = 0x08;                  /* Clear completion flag */
    
    /* Small delay to allow settling */
    for(delay = 0; delay < 10; delay++);
    
    return result;
}

// Converts ADC value to millivolts (assuming 3.3V reference)
uint32_t ADC_ToMillivolts(uint16_t adcValue)
{
    // Convert to millivolts
    return (adcValue * 3300UL) / 4095UL;
}
