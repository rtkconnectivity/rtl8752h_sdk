/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*********************************************************************************************************
* @file     io_adc.c
* @brief    This file provides demo code of adc continuous sample and read by gdma.
* @details
* @author   yuan
* @date     2018-12-07
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "io_adc.h"
#include "app_task.h"

/* Globals ------------------------------------------------------------------*/
uint16_t ADC_Recv_Buffer[GDMA_TRANSFER_SIZE];

/**
  * @brief  Initialization adc global data.
  * @param  No parameter.
  * @return void
  */
void global_data_adc_init(void)
{
    /* Initialize adc k value! */
    APP_PRINT_INFO0("[io_adc]global_data_adc_init");
    bool adc_k_status = false;
    adc_k_status = ADC_CalibrationInit();
    if (false == adc_k_status)
    {
        APP_PRINT_ERROR0("[io_adc]global_data_adc_init: ADC_CalibrationInit fail!");
    }
    memset(&ADC_Recv_Buffer, 0, sizeof(ADC_Recv_Buffer));
}

/**
  * @brief  Initialization of pinmux settings and pad settings.
  * @param  No parameter.
  * @return void
  */
void board_adc_init(void)
{
    Pad_Config(ADC_SAMPLE_PIN_5, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
    Pad_Config(ADC_SAMPLE_PIN_7, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
}

/**
  * @brief  Initialize ADC peripheral.
  * @param  No parameter.
  * @return void
  */
void driver_adc_init(void)
{
    RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);

    ADC_InitTypeDef ADC_InitStruct;
    ADC_StructInit(&ADC_InitStruct);

    ADC_InitStruct.ADC_WaterLevel   = 4;
    /* Configure the ADC sampling schedule, a schedule represents an ADC channel data,
       up to 16, i.e. schIndex[0] ~ schIndex[15] */
    ADC_InitStruct.ADC_SchIndex[0]  = EXT_SINGLE_ENDED(ADC_SAMPLE_CHANNEL_5);
    ADC_InitStruct.ADC_SchIndex[1]  = EXT_SINGLE_ENDED(ADC_SAMPLE_CHANNEL_7);
    /* Set the bitmap corresponding to schedule, 16 bits, LSB,
       schIndex[0-15] corresponding to 16 bits of bitmap bit0-bit15.
       For example, if config schIndex[0] and schIndex [1], then bitmap is 0000 0000 0011 (that is, 0x0003);
       if config schIndex [0] and schIndex [2], then bitmap is 0000 0000 0101 (that is, 0x0005).
    */
    ADC_InitStruct.ADC_Bitmap       = 0x03;
    /* Configuration of ADC continuous sampling cycle. */
    ADC_InitStruct.ADC_SampleTime   = ADC_CONTINUOUS_SAMPLE_PERIOD;

    /* When this parameter is set to ENABLE: Power consumption will increase in this mode,
      but the ADC sampling value will be more accurate. (When ADC_PowerOnDlyEn is set to ENABLE,
      the ADC samples are accurate; When ADC_PowerOnDlyEn is set to DISABLE, the ADC samples are accurate after 8ms error).
      When this parameter is set to DISABLE: the power consumption is low in this mode,
      but the sampling value of ADC will always have a high error of 0-10mV. */
    ADC_InitStruct.ADC_PowerAlwaysOnEn = ADC_POWER_ALWAYS_ON_ENABLE;

    ADC_Init(ADC, &ADC_InitStruct);

#if (ADC_MODE_DIVIDE_OR_BYPASS == ADC_BYPASS_MODE)
    /* High bypass resistance mode config, please notice that the input voltage of
      adc channel using high bypass mode should not be over 0.9V */
    ADC_BypassCmd(ADC_SAMPLE_CHANNEL_7, ENABLE);
    APP_PRINT_INFO0("[io_adc]driver_adc_init: ADC sample mode is bypass mode !");
#else
    ADC_BypassCmd(ADC_SAMPLE_CHANNEL_7, DISABLE);
#endif

    ADC_INTConfig(ADC, ADC_INT_FIFO_RD_ERR, ENABLE);
    ADC_INTConfig(ADC, ADC_INT_FIFO_OVERFLOW, ENABLE);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = ADC_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    /* When ADC is enabled, sampling will be done quickly and interruption will occur.
       After initialization, ADC can be enabled when sampling is needed.*/
//    ADC_Cmd(ADC, ADC_Continuous_Mode, ENABLE);
}

/**
  * @brief  Initialize gdma peripheral.
  * @param  No parameter.
  * @return void
  */
void driver_gdma_adc_init(void)
{
    uint32_t i = 0;

    /* Initialize data buffer which for storing data from adc */
    for (i = 0; i < GDMA_TRANSFER_SIZE; i++)
    {
        ADC_Recv_Buffer[i] = 0;
    }

    /* GDMA init */
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);

    GDMA_InitStruct.GDMA_ChannelNum         = ADC_GDMA_CHANNEL_NUM;
    GDMA_InitStruct.GDMA_DIR                = GDMA_DIR_PeripheralToMemory;
    GDMA_InitStruct.GDMA_BufferSize         = GDMA_TRANSFER_SIZE;//Determine total transfer size
    GDMA_InitStruct.GDMA_SourceInc          = DMA_SourceInc_Fix;
    GDMA_InitStruct.GDMA_DestinationInc     = DMA_DestinationInc_Inc;
    GDMA_InitStruct.GDMA_SourceDataSize     = GDMA_DataSize_HalfWord;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_HalfWord;
    GDMA_InitStruct.GDMA_SourceMsize        = GDMA_Msize_4;
    GDMA_InitStruct.GDMA_DestinationMsize   = GDMA_Msize_4;
    GDMA_InitStruct.GDMA_SourceAddr         = (uint32_t)(&(ADC->FIFO));
    GDMA_InitStruct.GDMA_DestinationAddr    = (uint32_t)ADC_Recv_Buffer;
    GDMA_InitStruct.GDMA_SourceHandshake    = GDMA_Handshake_ADC;

    GDMA_Init(ADC_GDMA_Channel, &GDMA_InitStruct);

    GDMA_INTConfig(ADC_GDMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);

    /* GDMA irq init */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = ADC_GDMA_Channel_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

//    /* Start to receive data */
//    GDMA_Cmd(ADC_GDMA_CHANNEL_NUM, ENABLE);
//    /* Enable adc */
//    ADC_Cmd(ADC, ADC_Continuous_Mode, ENABLE);

}

/**
  * @brief  Calculate adc sample voltage.
  * @param  No parameter.
  * @return void
  */
static void io_adc_voltage_calculate(T_IO_MSG *io_adc_msg)
{
    uint16_t *p_buf = io_adc_msg->u.buf;

    for (uint32_t i = 0; i < GDMA_TRANSFER_SIZE; i++)
    {
        uint16_t sample_data = p_buf[i];
        float sample_voltage = 0;
        ADC_ErrorStatus error_status = NO_ERROR;

#if (ADC_MODE_DIVIDE_OR_BYPASS == ADC_BYPASS_MODE)
        sample_voltage = ADC_GetVoltage(BYPASS_SINGLE_MODE, (int32_t)sample_data, &error_status);
#else
        sample_voltage = ADC_GetVoltage(DIVIDE_SINGLE_MODE, (int32_t)sample_data, &error_status);
#endif
        if (error_status < 0)
        {
            APP_PRINT_INFO2("[io_adc]io_adc_voltage_calculate: ADC parameter or efuse data error! i = %d, error_status = %d",
                            i, error_status);
        }
        else
        {
            APP_PRINT_INFO4("[io_adc]io_adc_voltage_calculate: ADC rawdata_%-4d = %d, voltage_%-4d = %dmV ", i,
                            sample_data, i, (uint32_t)sample_voltage);
        }
    }

    platform_delay_ms(1000);
    /* Restart dma and adc */
    GDMA_SetSourceAddress(ADC_GDMA_Channel, (uint32_t)(&(ADC->FIFO)));
    GDMA_SetDestinationAddress(ADC_GDMA_Channel, (uint32_t)(ADC_Recv_Buffer));
    GDMA_SetBufferSize(ADC_GDMA_Channel, GDMA_TRANSFER_SIZE);
    GDMA_ClearINTPendingBit(ADC_GDMA_CHANNEL_NUM, GDMA_INT_Transfer);
    GDMA_Cmd(ADC_GDMA_CHANNEL_NUM, ENABLE);
    ADC_ClearFIFO(ADC);
    ADC_Cmd(ADC, ADC_CONTINUOUS_MODE, ENABLE);
}

/**
  * @brief  Handle adc error function.
  * @param  No parameter.
  * @return void
  */
static void io_handle_adc_error(T_IO_MSG *io_adc_msg)
{
    uint16_t adc_msg_subtype = io_adc_msg->subtype;

    switch (adc_msg_subtype)
    {
    case IO_MSG_ADC_FIFO_READ_ERR:
        {
            APP_PRINT_ERROR0("[io_adc]io_handle_adc_error: IO_MSG_ADC_FIFO_READ_ERR!");
            //Add user code here!
        }
        break;
    case IO_MSG_ADC_FIFO_OVERFLOW:
        {
            APP_PRINT_ERROR0("[io_adc]io_handle_adc_error: IO_MSG_ADC_FIFO_OVERFLOW!");
            //Add user code here!
        }
        break;
    default:
        break;
    }
}

/**
  * @brief  Handle adc msg function.
  * @param  No parameter.
  * @return void
  */
void io_handle_adc_msg(T_IO_MSG *io_adc_msg)
{
    io_handle_adc_error(io_adc_msg);
}

/**
  * @brief  Handle gdma msg function.
  * @param  No parameter.
  * @return void
  */
void io_handle_gdma_msg(T_IO_MSG *io_gdma_msg)
{
    io_adc_voltage_calculate(io_gdma_msg);
}

/**
  * @brief  ADC sample start.
  * @param  No parameter.
  * @return void
  */
void io_adc_sample_start(void)
{
    /* Start to receive data */
    GDMA_Cmd(ADC_GDMA_CHANNEL_NUM, ENABLE);

    /* Clear adc fifo */
    ADC_ClearFIFO(ADC);
    /* Enable adc */
    ADC_Cmd(ADC, ADC_CONTINUOUS_MODE, ENABLE);
}

/**
  * @brief  ADC sample stop.
  * @param  No parameter.
  * @return void
  */
void io_adc_sample_stop(void)
{
    /* Stop dma */
    GDMA_SuspendCmd(ADC_GDMA_Channel, ENABLE);
    while (!(GDMA_GetSuspendChannelStatus(ADC_GDMA_Channel)));
    GDMA_Cmd(ADC_GDMA_CHANNEL_NUM, DISABLE);

    /* Clear adc fifo */
    ADC_ClearFIFO(ADC);
    /* Disable adc */
    ADC_Cmd(ADC, ADC_CONTINUOUS_MODE, DISABLE);
}

/**
  * @brief  ADC interrupt handler function.
  * @param  No parameter.
  * @return void
  */
void ADC_Handler(void)
{
    if (ADC_GetINTStatus(ADC, ADC_INT_FIFO_RD_ERR) == SET)
    {
        T_IO_MSG int_adc_msg;

        int_adc_msg.type = IO_MSG_TYPE_ADC;
        int_adc_msg.subtype = IO_MSG_ADC_FIFO_READ_ERR;
        if (false == app_send_msg_to_apptask(&int_adc_msg))
        {
            APP_PRINT_ERROR0("[io_adc]ADC_Handler: 1111Send int_adc_msg failed!");
            //Add user code here!
            io_adc_sample_stop();
//            ADC_Cmd(ADC, ADC_CONTINUOUS_MODE, DISABLE);
//            ADC_ClearFifo(ADC);
            ADC_ClearINTPendingBit(ADC, ADC_INT_FIFO_RD_ERR);
            return;
        }
        ADC_ClearINTPendingBit(ADC, ADC_INT_FIFO_RD_ERR);
    }
    if (ADC_GetINTStatus(ADC, ADC_INT_FIFO_OVERFLOW) == SET)
    {
        ADC_WriteFIFOCmd(ADC, DISABLE);
        DBG_DIRECT("ADC_INT_FIFO_OVERFLOW");

        T_IO_MSG int_adc_msg;

        int_adc_msg.type = IO_MSG_TYPE_ADC;
        int_adc_msg.subtype = IO_MSG_ADC_FIFO_OVERFLOW;
        if (false == app_send_msg_to_apptask(&int_adc_msg))
        {
            APP_PRINT_ERROR0("[io_adc]ADC_Handler: Send int_adc_msg failed!");
            io_adc_sample_stop();
//            ADC_ClearFifo(ADC);
            ADC_ClearINTPendingBit(ADC, ADC_INT_FIFO_OVERFLOW);
            return;
        }
        ADC_ClearFIFO(ADC);
        ADC_WriteFIFOCmd(ADC, ENABLE);
        ADC_ClearINTPendingBit(ADC, ADC_INT_FIFO_OVERFLOW);
    }
}

/**
  * @brief  GDMA channel interrupt handler function.
  * @param  No parameter.
  * @return void
  */
void ADC_GDMA_Channel_Handler(void)
{
    ADC_Cmd(ADC, ADC_CONTINUOUS_MODE, DISABLE);
    APP_PRINT_INFO0("into ADC_GDMA_Channel_Handler");
    T_IO_MSG int_gdma_msg;

    int_gdma_msg.type = IO_MSG_TYPE_GDMA;
    int_gdma_msg.subtype = 0;
    int_gdma_msg.u.buf = (void *)(ADC_Recv_Buffer);
    if (false == app_send_msg_to_apptask(&int_gdma_msg))
    {
        APP_PRINT_ERROR0("[io_adc]ADC_GDMA_Channel_Handler: Send int_gdma_msg failed!");
        io_adc_sample_stop();
//        ADC_ClearFifo(ADC);
        GDMA_ClearINTPendingBit(ADC_GDMA_CHANNEL_NUM, GDMA_INT_Transfer);
        return;
    }
    GDMA_ClearINTPendingBit(ADC_GDMA_CHANNEL_NUM, GDMA_INT_Transfer);
}
