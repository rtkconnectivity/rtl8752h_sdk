/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*********************************************************************************************************
* @file     io_adc.h
* @brief
* @details
* @author   yuan
* @date     2018-12-07
* @version  v1.0
*********************************************************************************************************
*/

#ifndef __IO_ADC_H
#define __IO_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_adc.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"

#include "app_msg.h"
#include "board.h"

/* Defines ------------------------------------------------------------------*/
/** ADC sample channel config.
  * ADC sample channel config:P2_0~P2_7 and VBAT
  * ADC sample channel index select: #define ADC_Channel_Index_0         0
  *                                  #define ADC_Channel_Index_1         1
  *                                  #define ADC_Channel_Index_2         2
  *                                  #define ADC_Channel_Index_3         3
  *                                  #define ADC_Channel_Index_4         4
  *                                  #define ADC_Channel_Index_5         5
  *                                  #define ADC_Channel_Index_7         7

  * If ADC_SAMPLE_PIN = P2_0, then ADC_SAMPLE_CHANNEL = ADC_Channel_Index_0;
  * If ADC_SAMPLE_PIN = P2_1, then ADC_SAMPLE_CHANNEL = ADC_Channel_Index_1;
  * If ADC_SAMPLE_PIN = P2_2, then ADC_SAMPLE_CHANNEL = ADC_Channel_Index_2;
  * If ADC_SAMPLE_PIN = P2_3, then ADC_SAMPLE_CHANNEL = ADC_Channel_Index_3;
  * If ADC_SAMPLE_PIN = P2_4, then ADC_SAMPLE_CHANNEL = ADC_Channel_Index_4;
  * If ADC_SAMPLE_PIN = P2_5, then ADC_SAMPLE_CHANNEL = ADC_Channel_Index_5;
  * If ADC_SAMPLE_PIN = P2_7, then ADC_SAMPLE_CHANNEL = ADC_Channel_Index_7;
  */
#define ADC_SAMPLE_CHANNEL_4        ADC_Channel_Index_4
#define ADC_SAMPLE_CHANNEL_5        ADC_Channel_Index_5

/* Define schedule index. */
#define ADC_Schedule_Index_0         0
#define ADC_Schedule_Index_1         1
#define ADC_Schedule_Index_2         2
#define ADC_Schedule_Index_3         3
#define ADC_Schedule_Index_4         4
#define ADC_Schedule_Index_5         5
#define ADC_Schedule_Index_6         6
#define ADC_Schedule_Index_7         7
#define ADC_Schedule_Index_8         8
#define ADC_Schedule_Index_9         9
#define ADC_Schedule_Index_10        10
#define ADC_Schedule_Index_11        11
#define ADC_Schedule_Index_12        12
#define ADC_Schedule_Index_13        13
#define ADC_Schedule_Index_14        14
#define ADC_Schedule_Index_15        15

/** Define that how many schedules to use, max 16.
  */
#define ADC_SCHEDULE_NUM            (2)


/** ADC mode config.
  * ADC pin hardware mode: bypass mode
  *                        divide mode
  */
#define ADC_DIVIDE_MODE                     0
#define ADC_BYPASS_MODE                     1
#define ADC_MODE_DIVIDE_OR_BYPASS           ADC_DIVIDE_MODE

#define ADC_ONE_SHOT_SAMPLE_PERIOD          (1000000-1)//100ms


/* ADC global data struct*/
typedef struct
{
    uint16_t RawData[ADC_SCHEDULE_NUM + 1];
} ADC_Data_TypeDef;

extern ADC_Data_TypeDef ADC_Global_Data;

void global_data_adc_init(void);
void board_adc_init(void);
void driver_adc_init(void);
void driver_adc_timer_init(void);
void io_handle_adc_msg(T_IO_MSG *io_adc_msg);


#ifdef __cplusplus
}
#endif

#endif

