#include "aht30.h"

static uint8 AHT30_CheckStatus(AHT30_HandleTypeDef *dev)
{
    uint8 status = 0;
    dev->hw.I2C_Read_Bytes(AHT30_ADDR_READ, &status, 1);
    return status;
}

uint8 AHT30_Init(AHT30_HandleTypeDef *dev)
{
    uint8 init_cmd[3] = {AHT30_CMD_INIT, 0x08, 0x00};
    dev->hw.Delay_ms(40); // Wait for power up

    uint8 status = AHT30_CheckStatus(dev);
    if ((status & 0x08) == 0) // check if calibrated
    {
        dev->hw.I2C_Write_Bytes(AHT30_ADDR_WRITE, init_cmd, 3);
        dev->hw.Delay_ms(10);
    }
    
    status = AHT30_CheckStatus(dev);
    if ((status & 0x08) != 0) {
        dev->initialized = 1;
        return 0; // Success
    }
    return 1; // Failed
}

uint8 AHT30_ReadMeasure(AHT30_HandleTypeDef *dev)
{
    uint8 meas_cmd[3] = {AHT30_CMD_MEASURE, 0x33, 0x00};
    uint8 read_buf[7];
    uint32 raw_humi, raw_temp;
    
    if (!dev->initialized) return 1;

    // Send measurement command
    if (dev->hw.I2C_Write_Bytes(AHT30_ADDR_WRITE, meas_cmd, 3) != 0) {
        return 2; // Write err
    }
    
    // Wait for measurement to complete (usually 80ms)
    dev->hw.Delay_ms(80);
    
    // Read 7 bytes
    if (dev->hw.I2C_Read_Bytes(AHT30_ADDR_READ, read_buf, 7) != 0) {
        return 3; // Read err
    }
    
    // Check if busy (Bit 7 of Byte 0)
    if ((read_buf[0] & 0x80) != 0) {
        return 4; // Busy
    }
    
    // Parse data
    raw_humi = ((uint32)read_buf[1] << 12) | ((uint32)read_buf[2] << 4) | ((uint32)read_buf[3] >> 4);
    raw_temp = (((uint32)(read_buf[3] & 0x0F)) << 16) | ((uint32)read_buf[4] << 8) | (uint32)read_buf[5];
    
    // Convert to float
    dev->humidity = ((float)raw_humi / 1048576.0f) * 100.0f;
    dev->temperature = ((float)raw_temp / 1048576.0f) * 200.0f - 50.0f;
    
    return 0; // Success
}
