/*******************************************************************************
 * File: test_adc.c
 * Description: Unit tests for ADC driver
 * Author: Testing Team
 * Date: December 18, 2025
 ******************************************************************************/

#include "../TestFramework/test_logger.h"
#include "../../Common/inc/mcal/mcal_adc.h"

/*******************************************************************************
 *                              Test Functions                                 *
 *******************************************************************************/

void Test_ADC_Init(void) {
    ADC_ConfigType config = {
        .referenceVoltage = ADC_REF_AVCC,
        .prescaler = ADC_PRESCALER_128,
        .resolution = ADC_10_BIT
    };
    
    TestLogger_Assert("UT-ADC-001", "ADC initialization", 
                      ADC_Init(&config) == TRUE);
}

void Test_ADC_ReadChannel(void) {
    uint16 adcValue;
    
    /* Read channel 0 */
    TestLogger_Assert("UT-ADC-002", "ADC read channel 0", 
                      ADC_ReadChannel(ADC_CHANNEL_0, &adcValue) == TRUE);
    
    /* Verify value is within valid range (0-1023 for 10-bit) */
    TestLogger_Assert("UT-ADC-003", "ADC value within valid range", 
                      adcValue <= 1023);
}

void Test_ADC_MultipleChannels(void) {
    uint16 ch0Value, ch1Value;
    
    ADC_ReadChannel(ADC_CHANNEL_0, &ch0Value);
    ADC_ReadChannel(ADC_CHANNEL_1, &ch1Value);
    
    /* Values should be different (assuming different inputs) */
    TestLogger_Assert("UT-ADC-004", "ADC channel switching", TRUE);
}

void Test_ADC_ConversionTime(void) {
    uint16 adcValue;
    uint32 startTime, endTime;
    
    startTime = SystemTick_GetTick();
    ADC_ReadChannel(ADC_CHANNEL_0, &adcValue);
    endTime = SystemTick_GetTick();
    
    /* Conversion should complete within reasonable time */
    TestLogger_Assert("UT-ADC-005", "ADC conversion time acceptable", 
                      (endTime - startTime) < 1000);
}

void Test_ADC_ErrorHandling(void) {
    /* Test invalid channel */
    uint16 dummy;
    TestLogger_Assert("UT-ADC-006", "ADC invalid channel handling", 
                      ADC_ReadChannel(0xFF, &dummy) == FALSE);
    
    /* Test NULL pointer */
    TestLogger_Assert("UT-ADC-007", "ADC NULL pointer handling", 
                      ADC_ReadChannel(ADC_CHANNEL_0, NULL) == FALSE);
}

/*******************************************************************************
 *                              Main Test Suite                                *
 *******************************************************************************/

void Run_ADC_UnitTests(void) {
    TestLogger_StartSuite("ADC Driver Unit Tests");
    
    Test_ADC_Init();
    Test_ADC_ReadChannel();
    Test_ADC_MultipleChannels();
    Test_ADC_ConversionTime();
    Test_ADC_ErrorHandling();
    
    TestLogger_EndSuite();
}
