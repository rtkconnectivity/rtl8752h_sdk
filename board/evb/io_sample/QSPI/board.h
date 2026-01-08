/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      board.h
* @brief     header file of Keypad demo.
* @details
* @author    tifnan_ge
* @date      2015-06-26
* @version   v0.1
* *********************************************************************************************************
*/


#ifndef _BOARD_H_
#define _BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rtl876x_pinmux.h"

/** @defgroup IO Driver Config
  * @note user must config it firstly!! Do not change macro names!!
  * @{
  */
#define DLPS_EN                         0

/* if use user define dlps enter/dlps exit callback function */
#define USE_USER_DEFINE_DLPS_EXIT_CB      1
#define USE_USER_DEFINE_DLPS_ENTER_CB     1

/* if use any peripherals below, #define it 1 */
#define USE_I2C0_DLPS        0
#define USE_I2C1_DLPS        0
#define USE_TIM_DLPS         0
#define USE_ENHTIM_DLPS      0
#define USE_QDECODER_DLPS    0
#define USE_IR_DLPS          0
#define USE_UART0_DLPS       0
#define USE_UART1_DLPS       0
#define USE_UART2_DLPS       0
#define USE_ADC_DLPS         0
#define USE_SPI0_DLPS        0
#define USE_SPI1_DLPS        0
#define USE_SPI2W_DLPS       0
#define USE_KEYSCAN_DLPS     0
#define USE_CODEC_DLPS       0
#define USE_I2S0_DLPS        0
#define USE_GPIO_DLPS        0
#define USE_IF8080_DLPS      0


/* do not modify USE_IO_DRIVER_DLPS macro */
#define USE_IO_DRIVER_DLPS   (USE_I2C0_DLPS | USE_I2C1_DLPS | USE_TIM_DLPS | USE_QDECODER_DLPS\
                              | USE_IR_DLPS | USE_UART0_DLPS | USE_SPI0_DLPS| USE_ADC_DLPS\
                              | USE_SPI1_DLPS | USE_SPI2W_DLPS | USE_KEYSCAN_DLPS | USE_CODEC_DLPS\
                              | USE_GPIO_DLPS | USE_I2S0_DLPS| USE_USER_DEFINE_DLPS_EXIT_CB\
                              | USE_IF8080_DLPS| USE_UART2_DLPS | USE_UART1_DLPS \
                              | USE_USER_DEFINE_DLPS_ENTER_CB)

#define  TEST_UART         1
#define  TEST_HW_TIMER     2
#define  TEST_WDG          3
#define  TEST_GPIO         4
#define  TEST_DhryStone    5

#define  TEST_INTERRUPT             0 //TEST_UART //TEST_HW_TIMER // TEST_WDG // TEST_GPIO


#ifdef __cplusplus
}
#endif

#endif  /* _BOARD_H_ */

