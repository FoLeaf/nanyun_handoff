#include "ra01_test.h"
#include "ra01.h"
#include "oled.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>

static uint32_t last_send_time = 0;
static uint32_t send_counter = 0;

void RA01_Test_Init(void)
{
    OLED_Clear();
    OLED_ShowSTR(0, 0, "RA01 TEST", 16);
    OLED_ShowSTR(0, 16, "Status: WAIT", 16);
    
    // Initialize Radio
    RA01_Init();
    
    // Switch to receive mode
    RA01_StartReceive();
    
    OLED_ShowSTR(0, 16, "Status: READY", 16);
}

void RA01_Test_Periodic(void)
{
    // 1. Process radio internal queues
    RA01_Process();
    
    // 2. Check for newly received data
    uint8_t rx_buf[64];
    uint16_t rx_len = RA01_PopReceivedData(rx_buf, sizeof(rx_buf) - 1);
    if(rx_len > 0) {
        rx_buf[rx_len] = '\0'; // ensure null termination for screen print
        OLED_ShowSTR(0, 48, "                ", 16); // clear line
        OLED_ShowSTR(0, 48, (char *)rx_buf, 16);
    }
    
    // 3. Send data periodically (e.g., every 3 seconds)
    if(HAL_GetTick() - last_send_time > 3000) {
        char tx_str[32];
        sprintf(tx_str, "STARM MSG %lu", ++send_counter);
        
        OLED_ShowSTR(0, 32, "                ", 16); // clear line
        OLED_ShowSTR(0, 32, "TXING...", 16);
        
        if (RA01_Send((uint8_t*)tx_str, strlen(tx_str), 1000)) {
            OLED_ShowSTR(0, 32, "                ", 16); // clear line
            OLED_ShowSTR(0, 32, tx_str, 16);
        } else {
            OLED_ShowSTR(0, 32, "                ", 16); // clear line
            OLED_ShowSTR(0, 32, "TX FAIL", 16);
        }
        
        // Return to receive mode immediately after sending
        RA01_StartReceive();
        last_send_time = HAL_GetTick();
    }
}
