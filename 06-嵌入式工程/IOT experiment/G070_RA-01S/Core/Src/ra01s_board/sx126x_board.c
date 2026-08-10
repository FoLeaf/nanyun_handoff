#include "sx126x_board.h"
#include "radio.h"
#include "main.h" // HAL library

// ---------------------------------------------------------
// Pin Definitions for RA-01S (STM32G070)
// These should match the pins you configured in STM32CubeMX
// ---------------------------------------------------------
#define RADIO_RESET_PORT    GPIOB
#define RADIO_RESET_PIN     GPIO_PIN_0

#define RADIO_TXEN_PORT     GPIOB
#define RADIO_TXEN_PIN      GPIO_PIN_13

#define RADIO_RXEN_PORT     GPIOB
#define RADIO_RXEN_PIN      GPIO_PIN_14

#define RADIO_NSS_PORT      GPIOA
#define RADIO_NSS_PIN       GPIO_PIN_4

#define RADIO_BUSY_PORT     GPIOB
#define RADIO_BUSY_PIN      GPIO_PIN_1

#define RADIO_DIO1_PORT     GPIOB
#define RADIO_DIO1_PIN      GPIO_PIN_2

extern SPI_HandleTypeDef hspi1;

// ---------------------------------------------------------
// Soft Timer based on HAL_GetTick
// ---------------------------------------------------------
typedef struct {
    uint8_t isRun;       // 0 stop, 1 run
    uint32_t delayMs;    // timeout delay
    uint32_t startMs;    // start time
} SoftTimer_t;

static SoftTimer_t txTimerHandle;
static SoftTimer_t rxTimerHandle;
static DioIrqHandler *dio1IrqCallback = NULL;

void SX126xIoInit( void )
{
    // Peripheral initialization is handled by CubeMX (MX_GPIO_Init, MX_SPI1_Init)
    // We just ensure initial states of RF switches here if needed.
    HAL_GPIO_WritePin(RADIO_TXEN_PORT, RADIO_TXEN_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RADIO_RXEN_PORT, RADIO_RXEN_PIN, GPIO_PIN_RESET);
}

void SX126xIoIrqInit( DioIrqHandler dioIrq )
{
    dio1IrqCallback = dioIrq;
    // EXTI initialization is handled by CubeMX (MX_GPIO_Init)
}

void SX126xIoDeInit( void )
{
    // Optional DeInit
}

void SX126xIoDbgInit( void )
{
}

void SX126xReset( void )
{
    HAL_Delay( 10 );
    HAL_GPIO_WritePin(RADIO_RESET_PORT, RADIO_RESET_PIN, GPIO_PIN_RESET);
    HAL_Delay( 20 );
    HAL_GPIO_WritePin(RADIO_RESET_PORT, RADIO_RESET_PIN, GPIO_PIN_SET);
    HAL_Delay( 10 );
}

void SX126xWaitOnBusy( void )
{
    uint32_t timeout = HAL_GetTick();
    while( HAL_GPIO_ReadPin(RADIO_BUSY_PORT, RADIO_BUSY_PIN) == GPIO_PIN_SET )
    {
        if(HAL_GetTick() - timeout > 1000) {
            // timeout error handling
            break;
        }
    }
}

bool SX126xCheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}

void SX126xDelayMs(uint32_t ms)
{
    HAL_Delay(ms);
}

void SX126xSetNss(uint8_t lev)
{
    if(lev) {
        HAL_GPIO_WritePin(RADIO_NSS_PORT, RADIO_NSS_PIN, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(RADIO_NSS_PORT, RADIO_NSS_PIN, GPIO_PIN_RESET);
    }
}

uint8_t SX126xSpiInOut(uint8_t data)
{
    uint8_t rxData = 0;
    HAL_SPI_TransmitReceive(&hspi1, &data, &rxData, 1, 1000);
    return rxData;
}

// ---------------------------------------------------------
// Soft Timer Implementations
// ---------------------------------------------------------
void SX126xTimerInit(void)
{
    txTimerHandle.isRun = 0;
    rxTimerHandle.isRun = 0;
}

void SX126xSetTxTimerValue(uint32_t nMs)
{
    txTimerHandle.delayMs = nMs;
}

void SX126xTxTimerStart(void)
{
    txTimerHandle.startMs = HAL_GetTick();
    txTimerHandle.isRun = 1;
}

void SX126xTxTimerStop(void)
{
    txTimerHandle.isRun = 0;
}

void SX126xSetRxTimerValue(uint32_t nMs)
{
    rxTimerHandle.delayMs = nMs;
}

void SX126xRxTimerStart(void)
{
    rxTimerHandle.startMs = HAL_GetTick();
    rxTimerHandle.isRun = 1;
}

void SX126xRxTimerStop(void)
{
    rxTimerHandle.isRun = 0;
}

// User must call this function in main loop or SysTick_Handler
void Radio_UpdateTick(void)
{
    uint32_t now = HAL_GetTick();

    if(txTimerHandle.isRun) {
        if((now - txTimerHandle.startMs) > txTimerHandle.delayMs) {
            SX126xTxTimerStop();
            extern void RadioOnTxTimeoutIrq( void* context );
            RadioOnTxTimeoutIrq(NULL);
        }
    }

    if(rxTimerHandle.isRun) {
        if((now - rxTimerHandle.startMs) > rxTimerHandle.delayMs) {
            SX126xRxTimerStop();
            extern void RadioOnRxTimeoutIrq( void* context );
            RadioOnRxTimeoutIrq(NULL);
        }
    }
}

// User must call this function in EXTI interrupt handler (e.g. HAL_GPIO_EXTI_Callback)
void Radio_DIO1_Interrupt(void)
{
    if(dio1IrqCallback != NULL) {
        dio1IrqCallback(NULL);
    }
}

// ---------------------------------------------------------
// RF Switch Control hook
// ---------------------------------------------------------
// Override the RF switch to use TXEN and RXEN
// These are called from SX126xSetOperatingMode in sx126x.c


// The F103 driver implemented this in sx126x.c as a static variable assignment
// We will intercept it by modifying sx126x.c or by renaming it here
// Actually, sx126x.c implements SX126xSetOperatingMode directly. 
// We will modify sx126x.c to call a hook `SX126xBoardSetOperatingMode` 
// or simply add the GPIO logic there.
// But to keep it modular, let's implement the hook here.
void SX126xBoardSetOperatingMode( uint8_t mode )
{
    // MODE_TX = 1, MODE_RX = 2, MODE_RX_DC = 3 (from RadioOperatingModes_t)
    if(mode == 1) { // TX
        HAL_GPIO_WritePin(RADIO_TXEN_PORT, RADIO_TXEN_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(RADIO_RXEN_PORT, RADIO_RXEN_PIN, GPIO_PIN_RESET);
    } else if(mode == 2 || mode == 3) { // RX
        HAL_GPIO_WritePin(RADIO_TXEN_PORT, RADIO_TXEN_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(RADIO_RXEN_PORT, RADIO_RXEN_PIN, GPIO_PIN_SET);
    } else { // Sleep/Standby
        HAL_GPIO_WritePin(RADIO_TXEN_PORT, RADIO_TXEN_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(RADIO_RXEN_PORT, RADIO_RXEN_PIN, GPIO_PIN_RESET);
    }
}
