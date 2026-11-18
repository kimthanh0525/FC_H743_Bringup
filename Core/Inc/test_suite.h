#ifndef __TEST_SUITE_H
#define __TEST_SUITE_H

#include "main.h"

// #define TEST_CASE_BLINK_LED
// #define TEST_CASE_UART
// #define TEST_CASE_IMU
// #define TEST_CASE_VBAT_ADC
#define TEST_CASE_MOTOR_PWM
// #define TEST_CASE_SDCARD
// #define TEST_CASE_FDCAN

void Run_Test_Blink_Led(void);
void Run_Test_Uart(void);
void Run_Test_Imu(void);
void Run_Test_Vbat_Adc(void);
void Run_Test_Motor_Pwm(void);
void Run_Test_Sdcard(void);
void Run_Test_Fdcan(void);

#endif /* __TEST_SUITE_H */
