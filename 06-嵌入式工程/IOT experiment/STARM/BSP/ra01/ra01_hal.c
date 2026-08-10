#include "ra01_hal.h"
#include "myspi.h"
#include "delay.h"

// NSS -> PA11
// RST -> PB12
// DIO0 -> PA10

#define RA01_NSS_PORT GPIOA
#define RA01_NSS_PIN  GPIO_PIN_11

#define RA01_RST_PORT GPIOB
#define RA01_RST_PIN  GPIO_PIN_12

#define RA01_DIO0_PORT GPIOA
#define RA01_DIO0_PIN  GPIO_PIN_10

void SX1276HALInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Init SPI via myspi wrapper
    MYSPI_Init();

    // Configure NSS (PA11) and RST (PB12) as output
    GPIO_InitStruct.Pin = RA01_NSS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(RA01_NSS_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = RA01_RST_PIN;
    HAL_GPIO_Init(RA01_RST_PORT, &GPIO_InitStruct);

    // Default states
    HAL_GPIO_WritePin(RA01_NSS_PORT, RA01_NSS_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RA01_RST_PORT, RA01_RST_PIN, GPIO_PIN_SET);

    // Configure DIO0 (PA10) as input
    GPIO_InitStruct.Pin = RA01_DIO0_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(RA01_DIO0_PORT, &GPIO_InitStruct);
}

uint8_t SpiInOut(uint8_t outData)
{
    return MYSPI_ReadWriteByte(outData);
}

void SpiNSSEnable(uint8_t status)
{
    if (status == 0) {
        HAL_GPIO_WritePin(RA01_NSS_PORT, RA01_NSS_PIN, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(RA01_NSS_PORT, RA01_NSS_PIN, GPIO_PIN_SET);
    }
}

void SX127X_ResetPinControl(uint8_t status)
{
    if (status == 0) {
        HAL_GPIO_WritePin(RA01_RST_PORT, RA01_RST_PIN, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(RA01_RST_PORT, RA01_RST_PIN, GPIO_PIN_SET);
    }
}

uint8_t SX1276ReadDio0(void)
{
    return (HAL_GPIO_ReadPin(RA01_DIO0_PORT, RA01_DIO0_PIN) == GPIO_PIN_SET) ? 1 : 0;
}

uint8_t SX1276ReadDio1(void) { return 0; }
uint8_t SX1276ReadDio3(void) { return 0; }
uint8_t SX1276ReadDio4(void) { return 0; }

