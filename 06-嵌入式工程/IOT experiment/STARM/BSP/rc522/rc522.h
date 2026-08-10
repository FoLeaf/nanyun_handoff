//
// Created by 19y on 2026/4/16.
// MFRC522 RFID Driver (Hardware-Abstracted)
//

#ifndef __RC522_H
#define __RC522_H

#include <stdint.h>

/* ---- MFRC522 Register Addresses ---- */
// Command and Status
#define MFRC522_REG_COMMAND 0x01
#define MFRC522_REG_COM_I_EN 0x02
#define MFRC522_REG_DIV_I_EN 0x03
#define MFRC522_REG_COM_IRQ 0x04
#define MFRC522_REG_DIV_IRQ 0x05
#define MFRC522_REG_ERROR 0x06
#define MFRC522_REG_STATUS1 0x07
#define MFRC522_REG_STATUS2 0x08
#define MFRC522_REG_FIFO_DATA 0x09
#define MFRC522_REG_FIFO_LEVEL 0x0A
#define MFRC522_REG_WATER_LEVEL 0x0B
#define MFRC522_REG_CONTROL 0x0C
#define MFRC522_REG_BIT_FRAMING 0x0D
#define MFRC522_REG_COLL 0x0E

// Command
#define MFRC522_REG_MODE 0x11
#define MFRC522_REG_TX_MODE 0x12
#define MFRC522_REG_RX_MODE 0x13
#define MFRC522_REG_TX_CONTROL 0x14
#define MFRC522_REG_TX_ASK 0x15
#define MFRC522_REG_TX_SEL 0x16
#define MFRC522_REG_RX_SEL 0x17
#define MFRC522_REG_RX_THRESHOLD 0x18
#define MFRC522_REG_DEMOD 0x19
#define MFRC522_REG_MF_TX 0x1C
#define MFRC522_REG_MF_RX 0x1D
#define MFRC522_REG_SERIAL_SPEED 0x1F

// Configuration
#define MFRC522_REG_CRC_RESULT_M 0x21
#define MFRC522_REG_CRC_RESULT_L 0x22
#define MFRC522_REG_MOD_WIDTH 0x24
#define MFRC522_REG_RF_CFG 0x26
#define MFRC522_REG_GS_N 0x27
#define MFRC522_REG_CW_GS_P 0x28
#define MFRC522_REG_MOD_GS_P 0x29
#define MFRC522_REG_T_MODE 0x2A
#define MFRC522_REG_T_PRESCALER 0x2B
#define MFRC522_REG_T_RELOAD_H 0x2C
#define MFRC522_REG_T_RELOAD_L 0x2D
#define MFRC522_REG_T_COUNTER_VAL_H 0x2E
#define MFRC522_REG_T_COUNTER_VAL_L 0x2F

// Test
#define MFRC522_REG_TEST_SEL1 0x31
#define MFRC522_REG_TEST_SEL2 0x32
#define MFRC522_REG_TEST_PIN_EN 0x33
#define MFRC522_REG_TEST_PIN_VALUE 0x34
#define MFRC522_REG_TEST_BUS 0x35
#define MFRC522_REG_AUTO_TEST 0x36
#define MFRC522_REG_VERSION 0x37
#define MFRC522_REG_ANALOG_TEST 0x38
#define MFRC522_REG_TEST_DAC1 0x39
#define MFRC522_REG_TEST_DAC2 0x3A
#define MFRC522_REG_TEST_ADC 0x3B

/* ---- MFRC522 Commands ---- */
#define MFRC522_CMD_IDLE 0x00
#define MFRC522_CMD_MEM 0x01
#define MFRC522_CMD_GEN_RANDOM_ID 0x02
#define MFRC522_CMD_CALC_CRC 0x03
#define MFRC522_CMD_TRANSMIT 0x04
#define MFRC522_CMD_NO_CMD_CHANGE 0x07
#define MFRC522_CMD_RECEIVE 0x08
#define MFRC522_CMD_TRANSCEIVE 0x0C
#define MFRC522_CMD_MF_AUTHENT 0x0E
#define MFRC522_CMD_SOFT_RESET 0x0F

/* ---- PICC Commands ---- */
#define PICC_CMD_REQA 0x26
#define PICC_CMD_WUPA 0x52
#define PICC_CMD_CT 0x88
#define PICC_CMD_SEL_CL1 0x93
#define PICC_CMD_SEL_CL2 0x95
#define PICC_CMD_SEL_CL3 0x97
#define PICC_CMD_HLTA 0x50
#define PICC_CMD_MF_AUTH_KEY_A 0x60
#define PICC_CMD_MF_AUTH_KEY_B 0x61
#define PICC_CMD_MF_READ 0x30
#define PICC_CMD_MF_WRITE 0xA0
#define PICC_CMD_MF_DECREMENT 0xC0
#define PICC_CMD_MF_INCREMENT 0xC1
#define PICC_CMD_MF_RESTORE 0xC2
#define PICC_CMD_MF_TRANSFER 0xB0
#define PICC_CMD_UL_WRITE 0xA2

/* ---- Status Codes ---- */
#define MI_OK 0
#define MI_NOTAGERR 1
#define MI_ERR 2
#define MI_TIMEOUT 3

#include "myspi.h"
#include "stm32f1xx_ll_gpio.h"

/* ========================================== */
/*           RC522 硬件引脚配置宏             */
/* ========================================== */
/* 更换引脚结构使用 MYSPI 底层支持，并在此定义专属的 CS 和 RST 引脚
 */

// CS / SDA (Chip Select) - 输出
#define RC522_CS_PORT GPIOB
#define RC522_CS_PIN GPIO_PIN_12
#define RC522_CS_PIN_LL LL_GPIO_PIN_12
#define RC522_CS(x)                                                            \
  do {                                                                         \
    x ? LL_GPIO_SetOutputPin(RC522_CS_PORT, RC522_CS_PIN_LL)                   \
      : LL_GPIO_ResetOutputPin(RC522_CS_PORT, RC522_CS_PIN_LL);                \
  } while (0)

// RST (Reset) - 输出
#define RC522_RST_PORT GPIOA
#define RC522_RST_PIN GPIO_PIN_9
#define RC522_RST_PIN_LL LL_GPIO_PIN_9
#define RC522_RST(x)                                                           \
  do {                                                                         \
    x ? LL_GPIO_SetOutputPin(RC522_RST_PORT, RC522_RST_PIN_LL)                 \
      : LL_GPIO_ResetOutputPin(RC522_RST_PORT, RC522_RST_PIN_LL);              \
  } while (0)

// IRQ (Interrupt Request) - 输入 (预留)
#define RC522_IRQ_PORT GPIOA
#define RC522_IRQ_PIN GPIO_PIN_8
#define RC522_IRQ_PIN_LL LL_GPIO_PIN_8
#define RC522_IRQ_READ() (LL_GPIO_IsInputPinSet(RC522_IRQ_PORT, RC522_IRQ_PIN_LL) ? 1 : 0)

/* ---- RC522 Device Handle ---- */
typedef struct {
  uint8_t uid[10];    // Card UID (up to 10 bytes)
  uint8_t uid_len;    // UID length
  uint16_t card_type; // ATQA response (card type)
  uint8_t sak;        // SAK byte
  uint8_t initialized;
} RC522_HandleTypeDef;

/* ========================================== */
/*                API 接口函数                */
/* ========================================== */

/**
 * @brief  初始化 MFRC522 及底层硬件引脚
 * @param  dev: MFRC522 设备句柄
 * @retval 状态值 (MI_OK:初始化成功, MI_ERR:失败)
 */
uint8_t RC522_Init(RC522_HandleTypeDef *dev);

/**
 * @brief  获取 MFRC522 的芯片版本号
 * @param  dev: MFRC522 设备句柄
 * @retval uint8_t 返回版本字节 (正常为 0x92, 0x91, 0x88 等)
 */
uint8_t RC522_GetVersion(RC522_HandleTypeDef *dev);

/**
 * @brief  寻卡
 * @param  dev: MFRC522 设备句柄
 * @param  req_mode: 寻卡方式, PICC_CMD_REQA(寻天线区内未进入休眠的卡) 或
 * PICC_CMD_WUPA(寻所有卡)
 * @param  tag_type: 寻到的卡片类型 (2字节，例如 0x0400 对应 Mifare_One(S50))
 * @retval 状态值 (MI_OK:成功)
 */
uint8_t RC522_Request(RC522_HandleTypeDef *dev, uint8_t req_mode,
                      uint8_t *tag_type);

/**
 * @brief  防冲突，获取卡片序号 (UID)
 * @param  dev: MFRC522 设备句柄
 * @param  ser_num: 保存返回的 4字节卡序列号 (第5字节为校验字节)
 * @retval 状态值 (MI_OK:成功)
 */
uint8_t RC522_Anticoll(RC522_HandleTypeDef *dev, uint8_t *ser_num);

/**
 * @brief  选卡
 * @param  dev: MFRC522 设备句柄
 * @param  ser_num: 刚才由防冲突获取的 4字节卡序列号
 * @retval uint8_t 卡片容量字节 (SAK)
 */
uint8_t RC522_SelectTag(RC522_HandleTypeDef *dev, uint8_t *ser_num);

/**
 * @brief  扇区密码认证
 * @param  dev: MFRC522 设备句柄
 * @param  auth_mode: 密码验证模式 (PICC_CMD_MF_AUTH_KEY_A 或
 * PICC_CMD_MF_AUTH_KEY_B)
 * @param  block_addr: 需要认证的绝对块地址 (0~63)
 * @param  key: 6字节密码数组
 * @param  ser_num: 4字节卡片序列号
 * @retval 状态值 (MI_OK:成功)
 */
uint8_t RC522_Auth(RC522_HandleTypeDef *dev, uint8_t auth_mode,
                   uint8_t block_addr, uint8_t *key, uint8_t *ser_num);

/**
 * @brief  读取 Mifare 块数据
 * @param  dev: MFRC522 设备句柄
 * @param  block_addr: 块绝对地址
 * @param  recv_data: 读出的数据包 (16字节)
 * @retval 状态值 (MI_OK:成功)
 */
uint8_t RC522_ReadBlock(RC522_HandleTypeDef *dev, uint8_t block_addr,
                        uint8_t *recv_data);

/**
 * @brief  全覆盖写入 Mifare 块数据
 * @param  dev: MFRC522 设备句柄
 * @param  block_addr: 块绝对地址
 * @param  send_data: 需要写入的数据序列 (16字节)
 * @retval 状态值 (MI_OK:成功)
 */
uint8_t RC522_WriteBlock(RC522_HandleTypeDef *dev, uint8_t block_addr,
                         uint8_t *send_data);

/**
 * @brief  命令卡片进入休眠状态
 * @param  dev: MFRC522 设备句柄
 */
void RC522_Halt(RC522_HandleTypeDef *dev);

/**
 * @brief  软硬件自复位 MFRC522 芯片
 * @param  dev: MFRC522 设备句柄
 */
void RC522_Reset(RC522_HandleTypeDef *dev);

/* ========================================== */
/*                底层驱动内部接口              */
/* ========================================== */

void RC522_WriteReg(RC522_HandleTypeDef *dev, uint8_t reg, uint8_t val);
uint8_t RC522_ReadReg(RC522_HandleTypeDef *dev, uint8_t reg);
void RC522_SetBitMask(RC522_HandleTypeDef *dev, uint8_t reg, uint8_t mask);
void RC522_ClearBitMask(RC522_HandleTypeDef *dev, uint8_t reg, uint8_t mask);
void RC522_AntennaOn(RC522_HandleTypeDef *dev);
void RC522_AntennaOff(RC522_HandleTypeDef *dev);
void RC522_CalcCRC(RC522_HandleTypeDef *dev, uint8_t *data, uint8_t len,
                   uint8_t *result);
uint8_t RC522_ToCard(RC522_HandleTypeDef *dev, uint8_t command,
                     uint8_t *send_data, uint8_t send_len, uint8_t *back_data,
                     uint16_t *back_len);

#endif /* __RC522_H */
