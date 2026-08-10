//
// Created by 19y on 2026/4/16.
// RC522 RFID Test Case Implementation (Software SPI)
// Adapted from reference test program timing
//

#include "rc522_test.h"
#include "delay.h"
#include "oled.h"
#include "rc522.h"
#include <stdio.h>

static RC522_HandleTypeDef rc522_dev;

void RC522_Test_Init(void) {
  OLED_ShowSTR(0, 0, "RC522 Init...", 8);

  uint8_t ret = RC522_Init(&rc522_dev);
  char buf[20];
  uint8_t ver = RC522_GetVersion(&rc522_dev);
  if (ret == MI_OK) {
    sprintf(buf, "OK! Ver:0x%02X", ver);
    OLED_ShowSTR(0, 2, buf, 8);
  } else {
    sprintf(buf, "Err Ver:0x%02X  ", ver);
    OLED_ShowSTR(0, 2, buf, 8);
  }
  delay_ms(1500);
  OLED_Clear();
  OLED_ShowSTR(0, 0, "Ready to Read.", 8);
}

void RC522_Test_Run(void) {
  uint8_t tag_type[2] = {0};
  uint8_t uid[5] = {0};
  char buf[20];

  static uint32_t last_card_tick = 0;
  static uint8_t card_displayed = 0;

  // Try to detect a card
  uint8_t status = RC522_Request(&rc522_dev, PICC_CMD_REQA, tag_type);
  if (status != MI_OK) {
    // If no card is present, check the 3 second timeout using system ticks
    if (card_displayed && (HAL_GetTick() - last_card_tick >= 3000)) {
      OLED_ShowSTR(0, 0, "No Card       ", 8);
      OLED_ShowSTR(0, 2, "              ", 8);
      OLED_ShowSTR(0, 4, "              ", 8);
      card_displayed = 0;
    }
    return;
  }

  // Update tick timestamp because a card is actively being read
  last_card_tick = HAL_GetTick();
  card_displayed = 1;

  // Card detected, show type
  sprintf(buf, "Type:%02X%02X     ", tag_type[0], tag_type[1]);
  OLED_ShowSTR(0, 0, buf, 8);

  // Anti-collision, get UID
  status = RC522_Anticoll(&rc522_dev, uid);
  if (status == MI_OK) {
    sprintf(buf, "%02X%02X%02X%02X", uid[0], uid[1], uid[2], uid[3]);
    OLED_ShowSTR(0, 2, "UID:", 8);
    OLED_ShowSTR(24, 2, buf, 8);

    // Select card
    uint8_t sak = RC522_SelectTag(&rc522_dev, uid);
    sprintf(buf, "SAK: 0x%02X    ", sak);
    OLED_ShowSTR(0, 4, buf, 8);
  } else {
    OLED_ShowSTR(0, 2, "Anticoll Err  ", 8);
  }

  RC522_Halt(&rc522_dev);
}
