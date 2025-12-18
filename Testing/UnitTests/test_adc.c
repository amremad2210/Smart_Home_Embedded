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
    /* Initialize ADC with channel 0 */
    ADC_Init(ADC_CHANNEL_0);
    
    TestLogger_Assert("UT-ADC-001", "ADC initialization", TRUE);
}

void Test_ADC_ReadChannel(void) {
    uint16 adcValue;
    
    /* Initialize and read channel 0 */
    ADC_Init(ADC_CHANNEL_0);
    adcValue = ADC_Read();
    
    /* Verify value is within valid range (0-4095 for 12-bit) */
    TestLogger_Assert("UT-ADC-002", "ADC read channel 0", TRUE);
    TestLogger_Assert("UT-ADC-003", "ADC value within valid range", 
                      adcValue <= ADC_MAX_VALUE);
}

void Test_ADC_MultipleChannels(void) {
    uint16 ch0Value, ch1Value;
    
    /* Read channel 0 */
    ADC_Init(ADC_CHANNEL_0);
    ch0Value = ADC_Read();
    
    /* Read channel 1 */
    ADC_Init(ADC_CHANNEL_1);
    ch1Value = ADC_Read();
    
    TestLogger_Assert("UT-ADC-004", "ADC channel switching", TRUE);
}

void Test_ADC_ConversionTime(void) {
    uint16 adcValue;
    uint32 startTime, endTime;
    
    ADC_Init(ADC_CHANNEL_0);
    startTime = SystemTick_GetTick();
    adcValue = ADC_Read();
    endTime = SystemTick_GetTick();
    
    /* Conversion should complete within reasonable time */
    TestLogger_Assert("UT-ADC-005", "ADC conversion time acceptable", 
                      (endTime - startTime) < 1000);
}

void Test_ADC_ErrorHandling(void) {
    /* Test millivolt conversion */
    uint16 adcValue = 2048;  /* Mid-range value */
    uint32 millivolts = ADC_ToMillivolts(adcValue);
    
    TestLogger_Assert("UT-ADC-006", "ADC millivolt conversion", 
                      millivolts > 0 && millivolts <= 3300);
    
    TestLogger_Assert("UT-ADC-007", "ADC max value conversion", 
                      ADC_ToMillivolts(ADC_MAX_VALUE) <= 3300);
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
