#include "test_suite.h"
#include <stdio.h>

#ifdef TEST_CASE_SDCARD

#define SD_CS_LOW()     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_RESET)
#define SD_CS_HIGH()    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_SET)

#define SD_SCK_LOW()    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET)
#define SD_SCK_HIGH()   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_SET)

#define SD_MOSI_LOW()   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0,  GPIO_PIN_RESET)
#define SD_MOSI_HIGH()  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0,  GPIO_PIN_SET)

#define SD_MISO_READ()  HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_8)

//200 CPU clock cycles
static void SD_Delay(void) {
    for (volatile int i = 0; i < 200; i++) { __NOP(); }
}
//Mode 0: CPOL=0, CPHA=0
static uint8_t SD_SPI_Transfer(uint8_t tx_data) {
    uint8_t rx_data = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (tx_data & 0x80) SD_MOSI_HIGH();
        else                SD_MOSI_LOW();
        tx_data <<= 1;

        SD_Delay();
        SD_SCK_HIGH(); // Latch the MOSI data
        SD_Delay();

        rx_data <<= 1;
        if (SD_MISO_READ() == GPIO_PIN_SET) {
            rx_data |= 0x01;
        }
        SD_SCK_LOW();
    }
    return rx_data;
}

//6 byte CMD and response
static uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc) {
    uint8_t r1;
    SD_SPI_Transfer(0xFF); // Dummy clock for extra 8 clocks
    SD_CS_LOW();
    SD_SPI_Transfer(0xFF);

    SD_SPI_Transfer(0x40 | cmd);
    SD_SPI_Transfer((uint8_t)(arg >> 24));
    SD_SPI_Transfer((uint8_t)(arg >> 16));
    SD_SPI_Transfer((uint8_t)(arg >> 8));
    SD_SPI_Transfer((uint8_t)arg);
    SD_SPI_Transfer(crc);

    for (uint8_t i = 0; i < 10; i++) {
        r1 = SD_SPI_Transfer(0xFF);
        if ((r1 & 0x80) == 0) break;
    }
    return r1;
}

void Run_Test_Sdcard(void) {
    printf("Start testing SDcard \r\n");

    // 1. Send >= 74 clocks while CS is high to wake up
    SD_CS_HIGH();
    SD_MOSI_HIGH();
    for (uint8_t i = 0; i < 10; i++) {
        SD_SPI_Transfer(0xFF);
    }

    // 2. Send CMD0 (GO_IDLE_STATE) + CS low to lock SPI mode
    uint8_t r1_cmd0 = SD_SendCmd(0, 0x00000000, 0x95);
    SD_CS_HIGH();
    SD_SPI_Transfer(0xFF);
    printf("CMD0 Response (R1): 0x%02X \r\n", r1_cmd0);

    // 3. If CMD0 succeeds, send CMD8
    if (r1_cmd0 == 0x01) {
        uint8_t r1_cmd8 = SD_SendCmd(8, 0x000001AA, 0x87);
        uint8_t r7[4];
        for (int i = 0; i < 4; i++) {
            r7[i] = SD_SPI_Transfer(0xFF);
        }
        SD_CS_HIGH();
        SD_SPI_Transfer(0xFF);

        printf("CMD8 Response (R1): 0x%02X | Data: %02X %02X %02X %02X \r\n",
               r1_cmd8, r7[0], r7[1], r7[2], r7[3]);
    }
}
#endif /* TEST_CASE_SDCARD */
