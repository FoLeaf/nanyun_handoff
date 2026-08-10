#include "ra01.h"
#include "ra01_hal.h"
#include "sx127x_driver.h"
#include "delay.h"
#include "myspi.h"

// Define a simple Rx Buffer
#define RA01_RX_BUFF_SIZE 256
static uint8_t ra01_rx_buffer[RA01_RX_BUFF_SIZE];
static uint16_t ra01_rx_len = 0;
static uint8_t ra01_has_new_data = 0;

void RA01_Init(void)
{
    // Initialize Hardware pins & SPI
    SX1276HALInit();

    // Reset module
    if(g_Radio.Reset != NULL) {
        g_Radio.Reset();
    }

    // Configure Radio default settings
    tLoRaSettings settings = {
        .RFFrequency = 433000000,
        .Power = 14,
        .SignalBw = 7,     // 125 kHz
        .SpreadingFactor = 7,
        .ErrorCoding = 1,  // 4/5
        .PreambleLength = 8
    };

    if(g_Radio.Init != NULL) {
        g_Radio.Init(&settings);
    }
}

void RA01_StartReceive(void)
{
    if(g_Radio.StartRx != NULL) {
        g_Radio.StartRx(0); // 0 means continuous rx
    }
}

uint8_t RA01_Send(uint8_t *data, uint16_t len, uint32_t timeoutMs)
{
    if(g_Radio.SetTxPacket == NULL || g_Radio.Process == NULL) return 0;
    
    // Send data to buffer and start Tx
    g_Radio.SetTxPacket(data, len, timeoutMs);
    
    uint32_t start_tick = HAL_GetTick();
    while((HAL_GetTick() - start_tick) < timeoutMs) {
        tRFProcessReturnCodes ret = g_Radio.Process();
        if(ret == RF_TX_DONE) {
            return 1;
        } else if(ret == RF_TX_TIMEOUT) {
            return 0;
        }
        MYSPI_Delay(); // small context switch
    }
    return 0; // timeout
}

uint16_t RA01_PopReceivedData(uint8_t *buffer, uint16_t buffer_max_len)
{
    if(ra01_has_new_data) {
        uint16_t copy_len = (ra01_rx_len > buffer_max_len) ? buffer_max_len : ra01_rx_len;
        for(uint16_t i=0; i<copy_len; i++) {
            buffer[i] = ra01_rx_buffer[i];
        }
        ra01_has_new_data = 0; // Clear flag
        return copy_len;
    }
    return 0;
}

void RA01_Process(void)
{
    if(g_Radio.Process != NULL) {
        tRFProcessReturnCodes ret = g_Radio.Process();
        
        if(ret == RF_RX_DONE) {
            if(g_Radio.GetRxPacket != NULL) {
                g_Radio.GetRxPacket(ra01_rx_buffer, &ra01_rx_len);
                ra01_has_new_data = 1;
            }
            // Need to restart RX after receiving
            RA01_StartReceive();
        } else if(ret == RF_RX_TIMEOUT) {
            // Optional: Restart RX
            RA01_StartReceive();
        }
    }
}
