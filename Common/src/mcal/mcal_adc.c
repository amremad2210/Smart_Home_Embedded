#include "mcal_adc.h"
#include "tm4c123gh6pm.h"

// Initializes ADC0 Sequencer 3 for single-ended sampling
void ADC_Init(uint8_t channel)
{
    volatile uint32_t delay;
    
    /* Enable ADC0 clock */
    SYSCTL_RCGCADC_R |= 0x01;           
    delay = SYSCTL_RCGCADC_R;           
    
    /* Enable GPIO Port E clock */
    SYSCTL_RCGCGPIO_R |= 0x10;          
    delay = SYSCTL_RCGCGPIO_R;          
    
    /* Configure PE3 as analog input */
    GPIO_PORTE_DIR_R &= ~0x08;          
    GPIO_PORTE_AFSEL_R |= 0x08;         
    GPIO_PORTE_DEN_R &= ~0x08;          
    GPIO_PORTE_AMSEL_R |= 0x08;         
    
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
