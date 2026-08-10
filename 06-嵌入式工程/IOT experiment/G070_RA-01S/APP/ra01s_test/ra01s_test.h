#ifndef __RA01S_TEST_H__
#define __RA01S_TEST_H__

#define LORA_TEST_ROLE_A_TX 0 // A board: read AHT30 and transmit
#define LORA_TEST_ROLE_B_RX 1 // B board: receive and display on OLED

// Change this macro before compiling:
// - LORA_TEST_ROLE_A_TX for board A firmware
// - LORA_TEST_ROLE_B_RX for board B firmware
#ifndef LORA_TEST_ROLE
#define LORA_TEST_ROLE LORA_TEST_ROLE_A_TX
#endif

#define LORA_TEST_IS_TX_NODE (LORA_TEST_ROLE == LORA_TEST_ROLE_A_TX)
#define LORA_TEST_IS_RX_NODE (LORA_TEST_ROLE == LORA_TEST_ROLE_B_RX)

void RA01S_Test_Init(void);
void RA01S_Test_Loop(void);
void RA01S_Test_Process(void);
const char *RA01S_Test_GetTxStatus(void);
const char *RA01S_Test_GetRxStatus(void);

#endif // __RA01S_TEST_H__
