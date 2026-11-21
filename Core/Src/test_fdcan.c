#include "test_suite.h"
#include <stdio.h>
#include <string.h>

#ifdef TEST_CASE_FDCAN

extern FDCAN_HandleTypeDef hfdcan1;

static FDCAN_TxHeaderTypeDef TxHeader;
static FDCAN_RxHeaderTypeDef RxHeader;
static uint8_t TxData[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
static uint8_t RxData[8];

static HAL_StatusTypeDef FDCAN1_Config_And_Start(void) {
    FDCAN_FilterTypeDef sFilterConfig;

    // Filter Configuration: Accept all packets
    sFilterConfig.IdType = FDCAN_STANDARD_ID;
    sFilterConfig.FilterIndex = 0;
    sFilterConfig.FilterType = FDCAN_FILTER_MASK;
    sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    sFilterConfig.FilterID1 = 0x000;
    sFilterConfig.FilterID2 = 0x000; // Accept all ID

    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK) {
        return HAL_ERROR;
    }

    // Reject packets that don't match filters
    HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT,
                                 FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);

    return HAL_FDCAN_Start(&hfdcan1);
}

void Run_Test_Fdcan(void) {
    printf("TEST FDCAN1 (INTERNAL LOOPBACK - 1 Mbps)\r\n");

    if (FDCAN1_Config_And_Start() != HAL_OK) {
        printf("FDCAN FAILED!\r\n");
        while (1) {
            HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4);
            HAL_Delay(150);
        }
    }
    printf("FDCAN HAS STARTED\r\n");

    // Header for tx packets
    TxHeader.Identifier = 0x123;
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.DataLength = FDCAN_DLC_BYTES_8;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    uint8_t counter = 0;

    while (1) {
        TxData[0] = counter++;
        memset(RxData, 0, sizeof(RxData));

        if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData) != HAL_OK) {
            printf("FAILED TO PUSH INTO TX FIFO\r\n");
        } else {
            HAL_Delay(5); 

            if (HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO0) > 0) {
                if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
                    printf("RX ID: 0x%03lX | DLC: 8 | Data: %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
                           RxHeader.Identifier,
                           RxData[0], RxData[1], RxData[2], RxData[3],
                           RxData[4], RxData[5], RxData[6], RxData[7]);
                }
            } else {
                printf("NO RESPONSE IN RX FIFO\r\n");
            }
        }

        HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_3);
        HAL_Delay(500);
    }
}

#endif /* TEST_CASE_FDCAN */
