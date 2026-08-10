//-------------------------------------------------------------------
// Filename: hal_led.c
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------
#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_led.h"






void halLedInit()
{
   // LEDs
    MCU_IO_OUTPUT(HAL_BOARD_IO_LED_1_PORT, HAL_BOARD_IO_LED_1_PIN, 0);
    MCU_IO_OUTPUT(HAL_BOARD_IO_LED_2_PORT, HAL_BOARD_IO_LED_2_PIN, 0);
    MCU_IO_OUTPUT(HAL_BOARD_IO_LED_3_PORT, HAL_BOARD_IO_LED_3_PIN, 0);
    MCU_IO_OUTPUT(HAL_BOARD_IO_LED_4_PORT, HAL_BOARD_IO_LED_4_PIN, 0);
}


//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// @fn          halLedSet
// @brief       Turn LED on.
// @param       uint8 id - led to set
// @return      none
//-------------------------------------------------------------------
void halLedSet(uint8 id)
{
    switch (id)
    {
        case 1:
            HAL_LED_SET_1();
            break;
        case 2:
            HAL_LED_SET_2();
            break;
        case 3:
            HAL_LED_SET_3();
            break;
        case 4:
            HAL_LED_SET_4();
            break;

        default:
            break;
    }
}

//-------------------------------------------------------------------
// @fn          halLedClear
// @brief       Turn LED off.
// @param       uint8 id - led to clear
// @return      none
//-------------------------------------------------------------------
void halLedClear(uint8 id)
{
    switch (id)
    {
        case 1:
            HAL_LED_CLR_1();
            break;
        case 2:
            HAL_LED_CLR_2();
            break;
        case 3:
            HAL_LED_CLR_3();
            break;
        case 4:
            HAL_LED_CLR_4();
            break;
        default:
            break;
    }
}

//-------------------------------------------------------------------
// @fn          halLedToggle
// @brief       Change state of LED. If on, turn it off. Else turn on.
// @param       uint8 id - led to toggle
// @return      none
//-------------------------------------------------------------------
void halLedToggle(uint8 id)
{
    switch (id)
    {
        case 1:
            HAL_LED_TGL_1();
            break;
        case 2:
            HAL_LED_TGL_2();
            break;
        case 3:
            HAL_LED_TGL_3();
            break;
        case 4:
            HAL_LED_TGL_4();
            break;
        default:
            break;
    }
}

//-------------------------------------------------------------------
