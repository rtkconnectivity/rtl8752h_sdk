/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file     board.h
* @brief    Pin definitions
* @details
* @author
* @date     2021-05-31
* @version  v0.1
* *********************************************************************************************************
*/

#ifndef _BOARD_H_
#define _BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Definitions of Infrared Function. */
/* Define whether infrared function is enabled. 1: enable; 0: disable */
#define IR_FUN_EN                       1
/* Define whether infrared learning function is enabled. 1: enable; 0: disable */
#define IR_FUN_LEARN_EN                 1
/* Define whether to use LED as infrared function indicator. 1: enable; 0: disable */
#define LED_IR_FUN_EN                   1


#if IR_FUN_EN
/* IR send pin define */
#define IR_TX_PIN                       P2_4
/* IR learn pin define */
#if IR_FUN_LEARN_EN
#define IR_RX_PIN                       P2_5
#define IR_RX_FIFO_THR_LEVEL            20
#define IR_LEARN_FREQ                   40000000/* Hz */
#define IR_LEARN_WAVEFORM_SIZE_MAX      100
#define IR_LEARN_TIMEOUT                (20*1000)  /* 20s */
/* Define whether to use print log function. 1: enable; 0: disable */
#define IR_RX_PRINT_LOG                 1
///**
//  * @brief  Software adjustment for sending IR learn data
//  */

//#define SOFTWARE_ADJUSTMENT

///**
//  * @brief  Prevent data truncation
//  */

//#define TRUNCATION_COMPENSATION                 (0.01)

///**
//  * @brief  Enable filter IR freqency or not
//  */
//#define FILTER_IR_LEARN_FREQ

///**
//  * @brief  Enable IR duty cycle learning or not
//  */
//#define IR_LEARN_DUTY_CYCLE_SUPPORT
//#define IR_LEARN_DUTY_CYCLE_SAMPLE_MAX_SIZE     6
#endif
/* LED pin define */
#if LED_IR_FUN_EN
#define LED_IR_SEND_PIN                 P0_1
#if IR_FUN_LEARN_EN
#define LED_IR_LEARN_PIN                P0_2
#endif
#endif
#endif

/*******************************************************
*                 DLPS Module Config
*******************************************************/
#define DLPS_EN                         0



/* if use user define dlps enter/dlps exit callback function */
#define USE_USER_DEFINE_DLPS_ENTER_CB   1
#define USE_USER_DEFINE_DLPS_EXIT_CB    1



/* if use any peripherals below, #define it 1 */
#define USE_ADC_DLPS                    0
#define USE_CODEC_DLPS                  0
#define USE_GPIO_DLPS                   0
#define USE_I2C0_DLPS                   0
#define USE_I2C1_DLPS                   0
#define USE_I2S0_DLPS                   0
#define USE_IR_DLPS                     0
#define USE_KEYSCAN_DLPS                0
#define USE_QDECODER_DLPS               0
#define USE_SPI0_DLPS                   0
#define USE_SPI1_DLPS                   0
#define USE_SPI2W_DLPS                  0
#define USE_TIM_DLPS                    0
#define USE_ENHTIM_DLPS                 0
#define USE_UART0_DLPS                  0
#define USE_UART1_DLPS                  0
#define USE_CTC_DLPS                    0



/* do not modify USE_IO_DRIVER_DLPS macro */
#define USE_IO_DRIVER_DLPS             ( USE_ADC_DLPS     | USE_CODEC_DLPS | USE_GPIO_DLPS  | USE_I2C0_DLPS   \
                                         | USE_I2C1_DLPS    | USE_I2S0_DLPS  | USE_IR_DLPS    | USE_KEYSCAN_DLPS\
                                         | USE_QDECODER_DLPS| USE_SPI0_DLPS  | USE_SPI1_DLPS  | USE_SPI2W_DLPS  \
                                         | USE_TIM_DLPS     | USE_ENHTIM_DLPS| USE_UART0_DLPS | USE_UART1_DLPS  \
                                         | USE_CTC_DLPS\
                                         | USE_USER_DEFINE_DLPS_ENTER_CB\
                                         | USE_USER_DEFINE_DLPS_EXIT_CB)
#ifdef __cplusplus
}
#endif

#endif

