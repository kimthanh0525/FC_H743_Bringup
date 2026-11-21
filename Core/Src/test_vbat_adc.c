#include "test_suite.h"
#include <stdio.h>

#ifdef TEST_CASE_VBAT_ADC

extern ADC_HandleTypeDef hadc1;

#define VREF_VOLTAGE      3.3f
#define ADC_MAX_16BIT     65535.0f
#define VBAT_DIVIDER      5.7f    // (R26 + R29) / R29 = (47k + 10k) / 10k = 5.7

static uint16_t Read_Vbat_Raw_Avg(uint8_t samples) {
    uint32_t sum = 0;

    for (uint8_t i = 0; i < samples; i++) {
        HAL_ADC_Start(&hadc1);
        if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
            sum += HAL_ADC_GetValue(&hadc1);
        }
        HAL_ADC_Stop(&hadc1);
    }

    return (uint16_t)(sum / samples);
}

void Run_Test_Vbat_Adc(void) {
    printf("TEST ADC VBAT (PC1 - ADC1_IN11)\r\n");

    // ADC1 calibration for offset cancellation 
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK) {
        printf("ADC1 CALIBRATION ERROR!\r\n");
        while (1) {
            HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4);
            HAL_Delay(150);
        }
    }

    printf("ADC1 CALIBRATION SUCCEEDED! START MEASURING VBAT\r\n");

    while (1) {
        uint16_t raw = Read_Vbat_Raw_Avg(16);

        float v_pin  = ((float)raw / ADC_MAX_16BIT) * VREF_VOLTAGE;
        float v_bat  = v_pin * VBAT_DIVIDER;

        printf("ADC Raw: %5u | V_PC1: %.3f V | VBAT: %.2f V\r\n", raw, v_pin, v_bat);

        HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_3);
        HAL_Delay(500);
    }
}

#endif /* TEST_CASE_VBAT_ADC */
