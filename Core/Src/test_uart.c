#include "test_suite.h"
#include <stdio.h>

#ifdef TEST_CASE_UART

extern UART_HandleTypeDef huart1;

int fputc(int ch, FILE *f) {
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 10);
    return ch;
}

void Run_Test_Uart(void) {
    uint32_t counter = 0;

    printf("[OK] STM32H743 FC Bringup - UART1 Ready!\r\n");

    while (1) {
        printf(" Tick: %lu | LED Toggle\r\n", counter++);
        HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_3);
        HAL_Delay(1000);
    }
}

#endif /* TEST_CASE_UART */
