/**
*********************************************************************************************************
*               Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file     board.h
* @brief    Pin definitions and dlps config
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


/* SPI config */
/* SPI pin define*/
#define SPI_SCK_PIN                     P4_0
#define SPI_MISO_PIN                    P4_1
#define SPI_MOSI_PIN                    P4_2
#define SPI_CS_PIN                      P4_3

#define FLASH_SPI                       SPI0
#define APBPERIPH_SPI                   APBPeriph_SPI0
#define APBPERIPH_SPI_CLOCK             APBPeriph_SPI0_CLOCK
#define SPI_IRQn                        SPI0_IRQn
#define SPI_Handler                     PI0_Handler

#define SPI_MODE_FULLDUPLEX             0
#define SPI_MODE_EEPROM                 3
#define SPI_MODE                        SPI_MODE_FULLDUPLEX
/* Ending SPI config */

/* UART pin define*/
#define UART_TX_PIN                     P3_0
#define UART_RX_PIN                     P3_1

/* GDMA config */
#define GDMA_RX_CHANNEL_NUM                2
#define GDMA_RX_Channel                    GDMA_Channel2
#define GDMA_RX_Channel_IRQn               GDMA0_Channel2_IRQn
#define GDMA_RX_Channel_Handler            GDMA0_Channel2_Handler
/* Only channel 0 and channel 1 have multiple block mode. */
#define GDMA_TX_CHANNEL_NUM                3
#define GDMA_TX_Channel                    GDMA_Channel3
#define GDMA_TX_Channel_IRQn               GDMA0_Channel3_IRQn
#define GDMA_TX_Channel_Handler            GDMA0_Channel3_Handler
/* The maximum number of GDMA single block is 65535. */

#define GDMA_TRANSFER_SIZE              3
#define GDMA_READ_SIZE_MAX              GDMA_TRANSFER_SIZE

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
#define USE_IO_DRIVER_DLPS               ( USE_ADC_DLPS     | USE_CODEC_DLPS | USE_GPIO_DLPS  | USE_I2C0_DLPS   \
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

