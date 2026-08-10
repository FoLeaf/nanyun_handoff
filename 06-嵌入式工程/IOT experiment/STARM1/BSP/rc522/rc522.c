#include "rc522.h"
#include "delay.h"



/**
 * @brief  向 MFRC522 特定寄存器写入一个字节的数据
 * @param  dev: MFRC522 设备句柄
 * @param  reg: 寄存器地址
 * @param  val: 待写入的字节值
 */
void RC522_WriteReg(RC522_HandleTypeDef *dev, uint8_t reg, uint8_t val) {
  uint8_t tx_addr = (reg << 1) & 0x7E;

  RC522_CS(0);
  MYSPI_SendByte(tx_addr);
  MYSPI_SendByte(val);
  RC522_CS(1);
}

/**
 * @brief  从 MFRC522 特定寄存器读取一个字节的数据
 * @param  dev: MFRC522 设备句柄
 * @param  reg: 寄存器地址
 * @retval 读取到的寄存器字节值
 */
uint8_t RC522_ReadReg(RC522_HandleTypeDef *dev, uint8_t reg) {
  uint8_t rx;
  uint8_t tx_addr = ((reg << 1) & 0x7E) | 0x80;

  RC522_CS(0);
  MYSPI_SendByte(tx_addr);
  rx = MYSPI_ReadByte();
  RC522_CS(1);

  return rx;
}

/**
 * @brief  对特定寄存器进行置位操作 (BitMask)
 * @param  dev: MFRC522 设备句柄
 * @param  reg: 寄存器地址
 * @param  mask: 置位掩码（置1的位变为1，为0的位保持不变）
 */
void RC522_SetBitMask(RC522_HandleTypeDef *dev, uint8_t reg, uint8_t mask) {
  uint8_t val = RC522_ReadReg(dev, reg);
  RC522_WriteReg(dev, reg, val | mask);
}

/**
 * @brief  对特定寄存器进行清位操作 (ClearBitMask)
 * @param  dev: MFRC522 设备句柄
 * @param  reg: 寄存器地址
 * @param  mask: 清位掩码（置1的位将被清0）
 */
void RC522_ClearBitMask(RC522_HandleTypeDef *dev, uint8_t reg, uint8_t mask) {
  uint8_t val = RC522_ReadReg(dev, reg);
  RC522_WriteReg(dev, reg, val & (~mask));
}

/* ---- Antenna Control ---- */

/**
 * @brief  开启 MFRC522 的天线发射
 * @param  dev: MFRC522 设备句柄
 */
void RC522_AntennaOn(RC522_HandleTypeDef *dev) {
  uint8_t val = RC522_ReadReg(dev, MFRC522_REG_TX_CONTROL);
  if (!(val & 0x03)) {
    RC522_SetBitMask(dev, MFRC522_REG_TX_CONTROL, 0x03);
  }
}

/**
 * @brief  关闭 MFRC522 的天线发射
 * @param  dev: MFRC522 设备句柄
 */
void RC522_AntennaOff(RC522_HandleTypeDef *dev) {
  RC522_ClearBitMask(dev, MFRC522_REG_TX_CONTROL, 0x03);
}

/* ---- Reset ---- */

/**
 * @brief  使用内部软复位和外部硬复位引脚将 MFRC522 重置为初始状态
 * @param  dev: MFRC522 设备句柄
 */
void RC522_Reset(RC522_HandleTypeDef *dev) {
  RC522_RST(1);
  delay_ms(1);
  RC522_RST(0);
  delay_ms(1);
  RC522_RST(1);
  delay_ms(1);

  RC522_WriteReg(dev, MFRC522_REG_COMMAND, MFRC522_CMD_SOFT_RESET);

  // Wait for the power down bit in CommandReg to clear
  while (RC522_ReadReg(dev, MFRC522_REG_COMMAND) & 0x10) {
    // Wait
  }
  delay_ms(1);
}

/* ---- CRC ---- */

/**
 * @brief  用 MFRC522 内部协处理器计算特定数组的 CRC 校验值
 * @param  dev: MFRC522 设备句柄
 * @param  data: 输入数据数组
 * @param  len: 数据长度
 * @param  result: 返回的2字节 CRC 校验结果
 */
void RC522_CalcCRC(RC522_HandleTypeDef *dev, uint8_t *data, uint8_t len,
                   uint8_t *result) {
  RC522_ClearBitMask(dev, MFRC522_REG_DIV_IRQ, 0x04);  // CRCIrq = 0
  RC522_SetBitMask(dev, MFRC522_REG_FIFO_LEVEL, 0x80); // Flush FIFO

  for (uint8_t i = 0; i < len; i++) {
    RC522_WriteReg(dev, MFRC522_REG_FIFO_DATA, data[i]);
  }
  RC522_WriteReg(dev, MFRC522_REG_COMMAND, MFRC522_CMD_CALC_CRC);

  uint16_t timeout = 0xFF;
  uint8_t n;
  do {
    n = RC522_ReadReg(dev, MFRC522_REG_DIV_IRQ);
    timeout--;
  } while (timeout && !(n & 0x04)); // Wait for CRCIrq

  result[0] = RC522_ReadReg(dev, MFRC522_REG_CRC_RESULT_L);
  result[1] = RC522_ReadReg(dev, MFRC522_REG_CRC_RESULT_M);
}

/* ---- Transceive with Card ---- */

/**
 * @brief  向 RC522 发送底层命令，执行卡片收发动作
 * @param  dev: MFRC522 设备句柄
 * @param  command: 执行的命令码 (例如 MFRC522_CMD_TRANSCEIVE)
 * @param  send_data: 发送的 FIFO 数据
 * @param  send_len: 发送数据长度
 * @param  back_data: 卡片返回的 FIFO 数据
 * @param  back_len: 卡片返回的数据位长度 (注意是 bits 而不是 bytes)
 * @retval 状态位返回 (MI_OK 为成功)
 */
uint8_t RC522_ToCard(RC522_HandleTypeDef *dev, uint8_t command,
                     uint8_t *send_data, uint8_t send_len, uint8_t *back_data,
                     uint16_t *back_len) {
  uint8_t status = MI_ERR;
  uint8_t irq_en = 0x00;
  uint8_t wait_irq = 0x00;
  uint8_t n;
  uint16_t timeout;

  switch (command) {
  case MFRC522_CMD_MF_AUTHENT:
    irq_en = 0x12;
    wait_irq = 0x10;
    break;
  case MFRC522_CMD_TRANSCEIVE:
    irq_en = 0x77;
    wait_irq = 0x30;
    break;
  default:
    break;
  }

  RC522_WriteReg(dev, MFRC522_REG_COM_I_EN, irq_en | 0x80);
  RC522_ClearBitMask(dev, MFRC522_REG_COM_IRQ, 0x80);
  RC522_SetBitMask(dev, MFRC522_REG_FIFO_LEVEL, 0x80); // FlushBuffer

  RC522_WriteReg(dev, MFRC522_REG_COMMAND, MFRC522_CMD_IDLE);

  // Write data to FIFO
  for (uint8_t i = 0; i < send_len; i++) {
    RC522_WriteReg(dev, MFRC522_REG_FIFO_DATA, send_data[i]);
  }

  // Execute command
  RC522_WriteReg(dev, MFRC522_REG_COMMAND, command);

  if (command == MFRC522_CMD_TRANSCEIVE) {
    RC522_SetBitMask(dev, MFRC522_REG_BIT_FRAMING, 0x80); // StartSend=1
  }

  // Wait for completion
  timeout = 2000;
  do {
    n = RC522_ReadReg(dev, MFRC522_REG_COM_IRQ);
    timeout--;
  } while (timeout && !(n & 0x01) && !(n & wait_irq));

  RC522_ClearBitMask(dev, MFRC522_REG_BIT_FRAMING, 0x80);

  if (timeout == 0) {
    return MI_TIMEOUT;
  }

  if (!(RC522_ReadReg(dev, MFRC522_REG_ERROR) & 0x1B)) {
    status = MI_OK;

    if (n & irq_en & 0x01) {
      status = MI_NOTAGERR;
    }

    if (command == MFRC522_CMD_TRANSCEIVE) {
      n = RC522_ReadReg(dev, MFRC522_REG_FIFO_LEVEL);
      uint8_t last_bits = RC522_ReadReg(dev, MFRC522_REG_CONTROL) & 0x07;
      if (last_bits) {
        *back_len = (n - 1) * 8 + last_bits;
      } else {
        *back_len = n * 8;
      }
      if (n == 0)
        n = 1;
      if (n > 16)
        n = 16;

      for (uint8_t i = 0; i < n; i++) {
        back_data[i] = RC522_ReadReg(dev, MFRC522_REG_FIFO_DATA);
      }
    }
  } else {
    status = MI_ERR;
  }

  return status;
}

/* ---- Card Operations ---- */

/**
 * @brief  向射频场内发起请求，探测是否有卡片
 * @param  dev: MFRC522 设备句柄
 * @param  req_mode: 请求模式 (PICC_CMD_REQA / PICC_CMD_WUPA)
 * @param  tag_type: 返回的两字节卡片类型代码
 * @retval 状态位 (MI_OK 为寻卡成功)
 */
uint8_t RC522_Request(RC522_HandleTypeDef *dev, uint8_t req_mode,
                      uint8_t *tag_type) {
  uint8_t status;
  uint16_t back_bits;

  RC522_WriteReg(dev, MFRC522_REG_BIT_FRAMING,
                 0x07); // TxLastBits = 7 (short frame)

  tag_type[0] = req_mode;
  status = RC522_ToCard(dev, MFRC522_CMD_TRANSCEIVE, tag_type, 1, tag_type,
                        &back_bits);

  if ((status != MI_OK) || (back_bits != 0x10)) {
    status = MI_ERR;
  }

  return status;
}

/**
 * @brief  防冲突机制：获取射频场内其中一张卡片的 UID 序列号
 * @param  dev: MFRC522 设备句柄
 * @param  ser_num: 用于接收卡片序列号的缓冲区 (4字节UID + 1字节异或校验)
 * @retval 状态位 (MI_OK 为读取成功)
 */
uint8_t RC522_Anticoll(RC522_HandleTypeDef *dev, uint8_t *ser_num) {
  uint8_t status;
  uint8_t ser_num_check = 0;
  uint16_t back_bits;

  RC522_WriteReg(dev, MFRC522_REG_BIT_FRAMING, 0x00);

  ser_num[0] = PICC_CMD_SEL_CL1;
  ser_num[1] = 0x20;
  status = RC522_ToCard(dev, MFRC522_CMD_TRANSCEIVE, ser_num, 2, ser_num,
                        &back_bits);

  if (status == MI_OK) {
    // Verify checksum
    for (uint8_t i = 0; i < 4; i++) {
      ser_num_check ^= ser_num[i];
    }
    if (ser_num_check != ser_num[4]) {
      status = MI_ERR;
    }
  }

  return status;
}

/**
 * @brief  根据已知的序列号选中其中一张卡片，读取它的容量字节 (SAK)
 * @param  dev: MFRC522 设备句柄
 * @param  ser_num: 要选择的卡片 4字节序列号
 * @retval 返回选定卡片的 SAK (Select Acknowledge) 值
 */
uint8_t RC522_SelectTag(RC522_HandleTypeDef *dev, uint8_t *ser_num) {
  uint8_t buf[9];
  uint16_t recv_bits;

  buf[0] = PICC_CMD_SEL_CL1;
  buf[1] = 0x70; // NVB = 70 (full UID)
  for (uint8_t i = 0; i < 5; i++) {
    buf[i + 2] = ser_num[i];
  }
  RC522_CalcCRC(dev, buf, 7, &buf[7]);

  uint8_t status =
      RC522_ToCard(dev, MFRC522_CMD_TRANSCEIVE, buf, 9, buf, &recv_bits);
  if (status == MI_OK && recv_bits == 0x18) {
    dev->sak = buf[0];
    return buf[0]; // SAK
  }
  return 0;
}

/**
 * @brief  验证特定数据块密码 (读写扇区前必须先通过 Auth 密码验证)
 * @param  dev: MFRC522 设备句柄
 * @param  auth_mode: 验证模式 A / B 密钥 (PICC_CMD_MF_AUTH_KEY_A / KEY_B)
 * @param  block_addr: 要访问的绝对块地址 (0-63)
 * @param  key: 用于验证的 6 字节密码组
 * @param  ser_num: 卡片的 4 字节序列号
 * @retval 状态位 (MI_OK 为验证成功)
 */
uint8_t RC522_Auth(RC522_HandleTypeDef *dev, uint8_t auth_mode,
                   uint8_t block_addr, uint8_t *key, uint8_t *ser_num) {
  uint8_t buf[12];
  uint16_t recv_bits;

  buf[0] = auth_mode;
  buf[1] = block_addr;
  for (uint8_t i = 0; i < 6; i++) {
    buf[i + 2] = key[i];
  }
  for (uint8_t i = 0; i < 4; i++) {
    buf[i + 8] = ser_num[i];
  }

  uint8_t status =
      RC522_ToCard(dev, MFRC522_CMD_MF_AUTHENT, buf, 12, buf, &recv_bits);
  if (status != MI_OK) {
    return status;
  }
  if (!(RC522_ReadReg(dev, MFRC522_REG_STATUS2) & 0x08)) {
    return MI_ERR;
  }
  return MI_OK;
}

/**
 * @brief  读取特定的卡片数据块 (单块数据16字节)
 * @param  dev: MFRC522 设备句柄
 * @param  block_addr: 数据块绝对地址
 * @param  recv_data: 接收此数据块信息的缓冲区 (推荐使用至少18字节的数组接收)
 * @retval 状态位 (MI_OK 为验证读取成功)
 */
uint8_t RC522_ReadBlock(RC522_HandleTypeDef *dev, uint8_t block_addr,
                        uint8_t *recv_data) {
  uint8_t buf[4];
  uint16_t recv_bits;

  buf[0] = PICC_CMD_MF_READ;
  buf[1] = block_addr;
  RC522_CalcCRC(dev, buf, 2, &buf[2]);

  uint8_t status =
      RC522_ToCard(dev, MFRC522_CMD_TRANSCEIVE, buf, 4, recv_data, &recv_bits);
  if (status != MI_OK || recv_bits != 0x90) { // 144 bits = 18 bytes
    return MI_ERR;
  }
  return MI_OK;
}

/**
 * @brief  向特定的卡片数据块内全新写入16字节数据
 * @param  dev: MFRC522 设备句柄
 * @param  block_addr: 数据块绝对地址
 * @param  send_data: 需要写入的 16 字节数据数组
 * @retval 状态位 (MI_OK 为写入成功)
 */
uint8_t RC522_WriteBlock(RC522_HandleTypeDef *dev, uint8_t block_addr,
                         uint8_t *send_data) {
  uint8_t buf[18];
  uint16_t recv_bits;

  buf[0] = PICC_CMD_MF_WRITE;
  buf[1] = block_addr;
  RC522_CalcCRC(dev, buf, 2, &buf[2]);

  uint8_t status =
      RC522_ToCard(dev, MFRC522_CMD_TRANSCEIVE, buf, 4, buf, &recv_bits);
  if (status != MI_OK || (recv_bits & 0x0F) != 0x0A) { // ACK = 0x0A
    return MI_ERR;
  }

  // Write 16 bytes data + 2 CRC
  for (uint8_t i = 0; i < 16; i++) {
    buf[i] = send_data[i];
  }
  RC522_CalcCRC(dev, buf, 16, &buf[16]);

  status = RC522_ToCard(dev, MFRC522_CMD_TRANSCEIVE, buf, 18, buf, &recv_bits);
  if (status != MI_OK || (recv_bits & 0x0F) != 0x0A) {
    return MI_ERR;
  }
  return MI_OK;
}

/**
 * @brief  发送命令使卡片进入挂起休眠状态，释放卡片
 * @param  dev: MFRC522 设备句柄
 */
void RC522_Halt(RC522_HandleTypeDef *dev) {
  uint8_t buf[4];
  uint16_t recv_bits;

  buf[0] = PICC_CMD_HLTA;
  buf[1] = 0;
  RC522_CalcCRC(dev, buf, 2, &buf[2]);

  RC522_ToCard(dev, MFRC522_CMD_TRANSCEIVE, buf, 4, buf, &recv_bits);
}

/* ---- Get Version ---- */

/**
 * @brief  返回芯片物理版本寄存器对应位数据
 */
uint8_t RC522_GetVersion(RC522_HandleTypeDef *dev) {
  return RC522_ReadReg(dev, MFRC522_REG_VERSION);
}

/* ---- Initialization ---- */

/**
 * @brief 内部调用：根据 rc522.h 中的宏展开，自动配置 GPIO
 */
static void RC522_HW_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  MYSPI_Init(); // Initialize SPI pins and clocks
  
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // CS - push-pull output
  GPIO_InitStruct.Pin = RC522_CS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(RC522_CS_PORT, &GPIO_InitStruct);
  RC522_CS(1); // CS default high

  // RST - push-pull output
  GPIO_InitStruct.Pin = RC522_RST_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(RC522_RST_PORT, &GPIO_InitStruct);
  RC522_RST(1); // default high

  // IRQ (Optional)
  GPIO_InitStruct.Pin = RC522_IRQ_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(RC522_IRQ_PORT, &GPIO_InitStruct);
}

/**
 * @brief  对外初始化入口：初始化GPIO并重置底层芯片寄存器，恢复射频设置
 * @param  dev: 需初始化的 RC522 设备环境上下文
 * @retval 状态位 (MI_OK 为初始化并校验版本通过)
 */
uint8_t RC522_Init(RC522_HandleTypeDef *dev) {
  RC522_HW_Init();

  // Software reset (which also toggles the RST pin)
  RC522_Reset(dev);

  // Timer: TPrescaler*TreloadVal / 6.78MHz = timeout
  // TModeReg[7..0] + TPrescalerReg[7..0] = TPrescaler
  RC522_WriteReg(dev, MFRC522_REG_T_MODE, 0x8D); // TAuto=1, f(timer)/2
  RC522_WriteReg(dev, MFRC522_REG_T_PRESCALER,
                 0x3E); // TModeReg[3..0] + TPrescalerReg => prescaler
  RC522_WriteReg(dev, MFRC522_REG_T_RELOAD_L, 30);
  RC522_WriteReg(dev, MFRC522_REG_T_RELOAD_H, 0);

  RC522_WriteReg(dev, MFRC522_REG_TX_ASK, 0x40); // Force 100% ASK modulation
  RC522_WriteReg(dev, MFRC522_REG_MODE,
                 0x3D); // CRC preset value 0x6363 (ISO 14443A)

  // Turn on antenna
  RC522_AntennaOn(dev);

  // Verify chip version
  uint8_t ver = RC522_GetVersion(dev);
  if (ver == 0x91 || ver == 0x92 || ver == 0x88 || ver == 0x12 || ver == 0x82) {
    dev->initialized = 1;
    return MI_OK;
  }

  return MI_ERR;
}
