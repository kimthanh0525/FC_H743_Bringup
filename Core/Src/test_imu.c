#include "test_suite.h"
#include <stdio.h>

#ifdef TEST_CASE_IMU

extern SPI_HandleTypeDef hspi1;

#define ICM42688_REG_WHO_AM_I      0x75
#define ICM42688_WHO_AM_I_VAL      0x47
#define ICM42688_REG_PWR_MGMT0     0x4E
#define ICM42688_REG_ACCEL_DATA_X1 0x1F  

#define IMU_CS_LOW()   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)
#define IMU_CS_HIGH()  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)

static void ICM42688_WriteReg(uint8_t reg, uint8_t val) {
    uint8_t tx[2] = { reg & 0x7F, val }; // Bit 7 = 0: Write
    IMU_CS_LOW();
    HAL_SPI_Transmit(&hspi1, tx, 2, 10);
    IMU_CS_HIGH();
}

static uint8_t ICM42688_ReadReg(uint8_t reg) {
    uint8_t tx[2] = { reg | 0x80, 0x00 }; // Bit 7 = 1: Read
    uint8_t rx[2] = { 0 };
    IMU_CS_LOW();
    HAL_SPI_TransmitReceive(&hspi1, tx, rx, 2, 10);
    IMU_CS_HIGH();
    return rx[1];
}

static void ICM42688_ReadBurst(uint8_t start_reg, uint8_t *buf, uint16_t len) {
    uint8_t addr = start_reg | 0x80;
    IMU_CS_LOW();
    HAL_SPI_Transmit(&hspi1, &addr, 1, 10);
    HAL_SPI_Receive(&hspi1, buf, len, 10);
    IMU_CS_HIGH();
}

void Run_Test_Imu(void) {
    IMU_CS_HIGH();
    HAL_Delay(50);

    uint8_t who_am_i = ICM42688_ReadReg(ICM42688_REG_WHO_AM_I);
    printf("TEST IMU ICM-42688-P\r\n");
    printf("WHO_AM_I = 0x%02X (Expected: 0x47)\r\n", who_am_i);

    if (who_am_i != ICM42688_WHO_AM_I_VAL) {
        printf("Fail\r\n");
        while (1) {
            HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4); 
            HAL_Delay(150);
        }
    }

    printf("SPI1 is working! Reading Accel/Gyro...\r\n");

    // Low-Noise mode
    ICM42688_WriteReg(ICM42688_REG_PWR_MGMT0, 0x0F);
    HAL_Delay(50);

    uint8_t raw[12];
    int16_t accel[3], gyro[3];

    while (1) {
        ICM42688_ReadBurst(ICM42688_REG_ACCEL_DATA_X1, raw, 12);

        accel[0] = (int16_t)((raw[0] << 8) | raw[1]);
        accel[1] = (int16_t)((raw[2] << 8) | raw[3]);
        accel[2] = (int16_t)((raw[4] << 8) | raw[5]);

        gyro[0]  = (int16_t)((raw[6] << 8) | raw[7]);
        gyro[1]  = (int16_t)((raw[8] << 8) | raw[9]);
        gyro[2]  = (int16_t)((raw[10] << 8) | raw[11]);

        printf("ACC: X=%6d Y=%6d Z=%6d | GYR: X=%6d Y=%6d Z=%6d\r\n",
               accel[0], accel[1], accel[2], gyro[0], gyro[1], gyro[2]);

        HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_3);
        HAL_Delay(200);
    }
}

#endif /* TEST_CASE_IMU */
