#include "myspi.h"

/**
 * @brief  微秒级软件延时函数，用于高频的 SPI 时序控制
 */
void MYSPI_Delay(void) {
  uint16_t i = 30;
  while (i--)
    ;
}

/**
 * @brief  初始化 SPI 引脚
 */
void MYSPI_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  MYSPI_GPIO_CLK_ENABLE();

  // SCK, MOSI - push-pull output
  GPIO_InitStruct.Pin = MYSPI_SCK_PIN | MYSPI_MOSI_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(MYSPI_SCK_PORT, &GPIO_InitStruct);
  
  MYSPI_SCK(0); // SCK default low
  MYSPI_MOSI(0);

  // MISO - input
  GPIO_InitStruct.Pin = MYSPI_MISO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MYSPI_MISO_PORT, &GPIO_InitStruct);
}

/**
 * @brief  软件模拟 SPI 发送一个字节数据
 * @param  byte: 待发送的数据字节
 */
void MYSPI_SendByte(uint8_t byte) {
  for (uint8_t i = 0; i < 8; i++) {
    MYSPI_MOSI(byte & 0x80);
    MYSPI_Delay();

    MYSPI_SCK(0);
    MYSPI_Delay();

    MYSPI_SCK(1);
    MYSPI_Delay();

    byte <<= 1;
  }
}

/**
 * @brief  软件模拟 SPI 从 MISO 读取一个字节数据
 * @retval 读到的字节数据
 */
uint8_t MYSPI_ReadByte(void) {
  uint8_t rx = 0;
  for (uint8_t i = 0; i < 8; i++) {
    rx <<= 1;

    MYSPI_SCK(0);
    MYSPI_Delay();

    if (MYSPI_MISO_READ()) {
      rx |= 0x01;
    }
    MYSPI_Delay();

    MYSPI_SCK(1);
    MYSPI_Delay();
  }
  return rx;
}

/**
 * @brief  软件模拟 SPI 读写一个字节数据
 * @param  tx_data: 待发送的数据
 * @retval 读到的字节数据
 */
uint8_t MYSPI_ReadWriteByte(uint8_t tx_data) {
  uint8_t rx_data = 0;
  for (uint8_t i = 0; i < 8; i++) {
    MYSPI_MOSI(tx_data & 0x80);
    tx_data <<= 1;
    MYSPI_Delay();

    MYSPI_SCK(0);
    MYSPI_Delay();

    rx_data <<= 1;
    if (MYSPI_MISO_READ()) {
      rx_data |= 0x01;
    }

    MYSPI_SCK(1);
    MYSPI_Delay();
  }
  return rx_data;
}
