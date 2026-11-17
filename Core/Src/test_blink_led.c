#include "test_suite.h"

#ifdef TEST_CASE_BLINK_LED

void Run_Test_Blink_Led(void) {
    // Turn off both led when first begin
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3 | GPIO_PIN_4, GPIO_PIN_RESET);

    while (1) {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET);
        HAL_Delay(500);

        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET);
        HAL_Delay(500);
    }
}

#endif /* TEST_CASE_BLINK_LED */
