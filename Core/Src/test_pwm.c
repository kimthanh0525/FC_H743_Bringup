#include "test_suite.h"
#include <stdio.h>

#ifdef TEST_CASE_MOTOR_PWM

extern TIM_HandleTypeDef htim4;

#define PWM_MIN_US   1000
#define PWM_MAX_US   2000

static void Set_Motor_PWM(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4) {
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, m1);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, m2);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, m3);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, m4);
}

void Run_Test_Motor_Pwm(void) {
    printf("\r\n--- TEST PWM OUTPUT (50Hz: 1000us - 2000us) ---\r\n");

    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);

    uint16_t pulse = PWM_MIN_US;
    int8_t step = 20;

    while (1) {
        Set_Motor_PWM(pulse, pulse, pulse, pulse);
        printf("PWM Pulse = %4u us (Duty = %.2f%%)\r\n", pulse, (pulse / 20000.0f) * 100.0f);

        pulse += step;
        if (pulse >= 1300) {
            step = -20;
        } else if (pulse <= PWM_MIN_US) {
            step = 20;
        }

        HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_3);
        HAL_Delay(100);
    }
}
#endif /* TEST_CASE_MOTOR_PWM */
