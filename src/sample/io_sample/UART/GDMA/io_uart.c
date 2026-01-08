/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
**********************************************************************************************************
* @file     io_uart.c
* @brief    uart gdma demo
* @details
* @author   yuan
* @date     2018-06-28
* @version  v0.1
*********************************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "io_uart.h"
#include "app_task.h"

typedef struct
{
    uint16_t div;
    uint16_t ovsr;
    uint16_t ovsr_adj;
} UART_BaudRate_TypeDef;

typedef enum
{
    BAUD_RATE_9600,
    BAUD_RATE_19200,
    BAUD_RATE_115200,
    BAUD_RATE_230400,
    BAUD_RATE_256000,
    BAUD_RATE_384000,
    BAUD_RATE_460800,
    BAUD_RATE_921600,
    BAUD_RATE_1000000,
    BAUD_RATE_2000000,
    BAUD_RATE_3000000
} Uart_BaudRate_TypeDef;

const UART_BaudRate_TypeDef BaudRate_Table[11] =
{
    {271, 10, 0x24A}, // BAUD_RATE_9600
    {150, 8,  0x3EF}, // BAUD_RATE_19200
    {20,  12, 0x252}, // BAUD_RATE_115200
    {11,  10, 0x3BB}, // BAUD_RATE_230400
    {11,  9,  0x084}, // BAUD_RATE_256000
    {7,   9,  0x3EF}, // BAUD_RATE_384000
    {6,   9,  0x0AA}, // BAUD_RATE_460800
    {3,   9,  0x0AA}, // BAUD_RATE_921600
    {4,   5,  0},     // BAUD_RATE_1000000
    {2,   5,  0},     // BAUD_RATE_2000000
    {1,   8,  0x292}, // BAUD_RATE_3000000
};

uint8_t GDMA_SendData_Buffer[UART_TX_GDMA_BUFFER_SIZE];
uint8_t GDMA_ReceiveData_Buffer[UART_RX_GDMA_BUFFER_SIZE];

/**
  * @brief  Initialization of pinmux settings and pad settings.
  * @param  No parameter.
  * @return void
*/
void board_uart_init(void)
{
    Pad_Config(UART_TX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
    Pad_Config(UART_RX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);

    Pinmux_Config(UART_TX_PIN, UART0_TX);
    Pinmux_Config(UART_RX_PIN, UART0_RX);
}

/**
  * @brief  Initialize uart peripheral.
  * @param  No parameter.
  * @return void
*/
void driver_uart_init(void)
{
    UART_DeInit(UART0);

    RCC_PeriphClockCmd(APBPeriph_UART0, APBPeriph_UART0_CLOCK, ENABLE);

    /* uart init */
    UART_InitTypeDef UART_InitStruct;
    UART_StructInit(&UART_InitStruct);

    UART_InitStruct.UART_Div            = BaudRate_Table[BAUD_RATE_115200].div;
    UART_InitStruct.UART_Ovsr           = BaudRate_Table[BAUD_RATE_115200].ovsr;
    UART_InitStruct.UART_OvsrAdj        = BaudRate_Table[BAUD_RATE_115200].ovsr_adj;

    UART_InitStruct.UART_RxThdLevel     = 16;                      //1~29
    UART_InitStruct.UART_IdleTime       = UART_RX_IDLE_2BYTE;      //idle interrupt wait time
    UART_InitStruct.UART_DmaEn          = UART_DMA_ENABLE;
    UART_InitStruct.UART_TxWaterLevel   = 15;     //Better to equal TX_FIFO_SIZE(16)- GDMA_MSize
    UART_InitStruct.UART_RxWaterLevel   = 1;      //Better to equal GDMA_MSize
#if (UART_GDMA_TX_ENABLE == EVB_ENABLE)
    UART_InitStruct.UART_TxDmaEn   = ENABLE;
#endif
#if (UART_GDMA_RX_ENABLE == EVB_ENABLE)
    UART_InitStruct.UART_RxDmaEn   = ENABLE;
#endif
    UART_Init(UART0, &UART_InitStruct);
}

void UART_SendData_Continuous(UART_TypeDef *UARTx, const uint8_t *pSend_Buf, uint16_t vCount)
{
    uint8_t count;

    while (vCount / 16 > 0)
    {
        while (UART_GetFlagStatus(UARTx, UART_FLAG_TX_FIFO_EMPTY) == 0);
        for (count = 16; count > 0; count--)
        {
            UARTx->RB_THR = *pSend_Buf++;
        }
        vCount -= 16;
    }

    while (UART_GetFlagStatus(UARTx, UART_FLAG_TX_FIFO_EMPTY) == 0);
    while (vCount--)
    {
        UARTx->RB_THR = *pSend_Buf++;
    }

}

void UART_SendDataByGDMA(void)
{
    /*--------------initialize test buffer which for sending data to UART---------------------*/
    for (uint32_t i = 0; i < UART_TX_GDMA_BUFFER_SIZE; i++)
    {
        GDMA_SendData_Buffer[i] = 0x10 + i;
    }

    /*--------------GDMA init-----------------------------*/
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum      = UART_TX_GDMA_CHANNEL_NUM;
    GDMA_InitStruct.GDMA_DIR             = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_BufferSize      = UART_TX_GDMA_BUFFER_SIZE;//determine total transfer size
    GDMA_InitStruct.GDMA_SourceInc       = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc  = DMA_DestinationInc_Fix;
    GDMA_InitStruct.GDMA_SourceDataSize  = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_SourceMsize      = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_DestinationMsize = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_SourceAddr      = (uint32_t)GDMA_SendData_Buffer;
    GDMA_InitStruct.GDMA_DestinationAddr = (uint32_t)(&(UART0->RB_THR));
    GDMA_InitStruct.GDMA_DestHandshake   = GDMA_Handshake_UART0_TX;
    GDMA_InitStruct.GDMA_ChannelPriority = 2;//channel prority between 0 to 5
    GDMA_Init(UART_TX_GDMA_CHANNEL, &GDMA_InitStruct);

    GDMA_INTConfig(UART_TX_GDMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);

    /*-----------------GDMA IRQ init-------------------*/
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = UART_TX_GDMA_CHANNEL_IRQN;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    /*-----------------start to send data-----------*/
    GDMA_Cmd(UART_TX_GDMA_CHANNEL_NUM, ENABLE);
}

void UART_ReceiveDataByGDMA(void)
{
    /*--------------initialize test buffer which for sending data to UART---------------------*/
    for (uint32_t i = 0; i < UART_RX_GDMA_BUFFER_SIZE; i++)
    {
        GDMA_ReceiveData_Buffer[i] = 0;
    }

    /*--------------GDMA init-----------------------------*/
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum         = UART_RX_GDMA_CHANNEL_NUM;
    GDMA_InitStruct.GDMA_DIR                = GDMA_DIR_PeripheralToMemory;
    GDMA_InitStruct.GDMA_BufferSize         = UART_RX_GDMA_BUFFER_SIZE;//determine total transfer size
    GDMA_InitStruct.GDMA_SourceInc          = DMA_SourceInc_Fix;
    GDMA_InitStruct.GDMA_DestinationInc     = DMA_DestinationInc_Inc;
    GDMA_InitStruct.GDMA_SourceDataSize     = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_DestinationDataSize    = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_SourceMsize            = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_DestinationMsize       = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_SourceAddr         = (uint32_t)(&(UART0->RB_THR));
    GDMA_InitStruct.GDMA_SourceHandshake    = GDMA_Handshake_UART0_RX;
    GDMA_InitStruct.GDMA_DestinationAddr    = (uint32_t)GDMA_ReceiveData_Buffer;
    GDMA_InitStruct.GDMA_ChannelPriority    = 2;//channel prority between 0 to 5
    GDMA_Init(UART_RX_GDMA_CHANNEL, &GDMA_InitStruct);

    GDMA_INTConfig(UART_RX_GDMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);

    /*-----------------GDMA IRQ init-------------------*/
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = UART_RX_GDMA_CHANNEL_IRQN;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    /*-----------------start to send data-----------*/
    GDMA_Cmd(UART_RX_GDMA_CHANNEL_NUM, ENABLE);
}

/**
  * @brief  Handle gdma msg function.
  * @param  No parameter.
  * @return void
  */
void io_handle_gdma_msg(T_IO_MSG *io_gdma_msg)
{
    uint16_t subtype = io_gdma_msg->subtype;
    if (subtype == 0)
    {
        APP_PRINT_INFO0("io_handle_gdma_msg: uart tx done ");
//        /* cyclic transmission */
//        GDMA_SetSourceAddress(UART_TX_GDMA_CHANNEL, (uint32_t)(GDMA_SendData_Buffer));
//        GDMA_SetDestinationAddress(UART_TX_GDMA_CHANNEL, (uint32_t)(&(UART0->RB_THR)));
//        GDMA_SetBufferSize(UART_TX_GDMA_CHANNEL, UART_TX_GDMA_BUFFER_SIZE);
//        GDMA_ClearINTPendingBit(UART_TX_GDMA_CHANNEL_NUM, GDMA_INT_Transfer);
//        GDMA_Cmd(UART_TX_GDMA_CHANNEL_NUM, ENABLE);
    }
    else if (subtype == 1)
    {
        APP_PRINT_INFO0("io_handle_gdma_msg: uart rx done ");
        UART_SendData_Continuous(UART0, GDMA_ReceiveData_Buffer, UART_RX_GDMA_BUFFER_SIZE);
//        /* cyclic reception */
//        GDMA_SetSourceAddress(UART_RX_GDMA_CHANNEL, (uint32_t)(&(UART0->RB_THR)));
//        GDMA_SetDestinationAddress(UART_RX_GDMA_CHANNEL, (uint32_t)(GDMA_ReceiveData_Buffer));
//        GDMA_SetBufferSize(UART_RX_GDMA_CHANNEL, UART_RX_GDMA_BUFFER_SIZE);
//        GDMA_ClearINTPendingBit(UART_RX_GDMA_CHANNEL_NUM, GDMA_INT_Transfer);
//        GDMA_Cmd(UART_RX_GDMA_CHANNEL_NUM, ENABLE);
    }
}

void io_uart_demo(void)
{
    uint8_t send_data[100] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for (uint8_t i = 0; i < 90; i++)
    {
        send_data[i] = i + 1;
    }
    for (uint8_t i = 90; i < 100; i++)
    {
        send_data[i] = 0xFF;
    }

    UART_SendData_Continuous(UART0, send_data, sizeof(send_data));

#if (UART_GDMA_TX_ENABLE == EVB_ENABLE)
    UART_SendDataByGDMA();
#endif
#if (UART_GDMA_RX_ENABLE == EVB_ENABLE)
    UART_ReceiveDataByGDMA();
#endif
}

void UART0_Handler()
{
    uint16_t lenth = 0;
    uint8_t uart_rev_data[32];

    /* read interrupt id */
    uint32_t int_status = UART_GetIID(UART0);

    /* disable interrupt */
    UART_INTConfig(UART0, UART_INT_RD_AVA, DISABLE);

    if (UART_GetFlagStatus(UART0, UART_FLAG_RX_IDLE) == SET)
    {
        UART_INTConfig(UART0, UART_INT_RX_IDLE, DISABLE);
        DBG_DIRECT("UART0_FLAG_RX_IDLE");
        UART_ClearRxFIFO(UART0);
        UART_INTConfig(UART0, UART_INT_RX_IDLE, ENABLE);
    }

    switch (int_status & 0x0E)
    {
    /*rx time out*/
    case 0x0C:
        lenth = UART_GetRxFIFODataLen(UART0);
        DBG_DIRECT("UART0_Handler rx time out, lenth = %d", lenth);
        UART_ReceiveData(UART0, uart_rev_data, lenth);
        for (uint8_t i = 0; i < lenth; i++)
        {
            DBG_DIRECT("data=0x%x", uart_rev_data[i]);
        }
        UART_SendData_Continuous(UART0, uart_rev_data, lenth);
        break;

    /* receiver line status  */
    case 0x06:
        break;

    /* rx data valiable */
    case 0x04:
        lenth = UART_GetRxFIFODataLen(UART0);
        DBG_DIRECT("UART0_Handler rx data valiable, lenth = %d", lenth);
        UART_ReceiveData(UART0, uart_rev_data, lenth);
        for (uint8_t i = 0; i < lenth; i++)
        {
            DBG_DIRECT("data=0x%x", uart_rev_data[i]);
        }
        UART_SendData_Continuous(UART0, uart_rev_data, lenth);
        break;

    /* tx fifo empty */
    case 0x02:
        /* do nothing */
        break;

    /* modem status */
    case 0x00:
        break;

    default:
        break;
    }

    /* enable interrupt again */
    UART_INTConfig(UART0, UART_INT_RD_AVA, ENABLE);
}

/**
  * @brief  GDMA interrupt handler function.
  * @param  No parameter.
  * @return void
  */
void UART_TX_GDMA_Handler(void)
{
    GDMA_ClearINTPendingBit(UART_TX_GDMA_CHANNEL_NUM, GDMA_INT_Transfer);
    DBG_DIRECT("TX DMA HANDLER");
    T_IO_MSG int_uart_msg;

    int_uart_msg.type = IO_MSG_TYPE_GDMA;
    int_uart_msg.subtype = 0;
    int_uart_msg.u.buf = (void *)(GDMA_SendData_Buffer);

    if (false == app_send_msg_to_apptask(&int_uart_msg))
    {
        APP_PRINT_ERROR0("[io_adc]UART_GDMA_Channel_Handler: Send int_uart_msg failed!");
        //Add user code here!

        GDMA_ClearINTPendingBit(UART_TX_GDMA_CHANNEL_NUM, GDMA_INT_Transfer);
        return;
    }
}

/**
  * @brief  GDMA interrupt handler function.
  * @param  No parameter.
  * @return void
  */
void UART_RX_GDMA_Handler(void)
{
    GDMA_ClearINTPendingBit(UART_RX_GDMA_CHANNEL_NUM, GDMA_INT_Transfer);

    DBG_DIRECT("RX DMA HANDLER");
    T_IO_MSG int_uart_msg;

    int_uart_msg.type = IO_MSG_TYPE_GDMA;
    int_uart_msg.subtype = 1;
    int_uart_msg.u.buf = (void *)(GDMA_SendData_Buffer);

    if (false == app_send_msg_to_apptask(&int_uart_msg))
    {
        APP_PRINT_ERROR0("[io_adc]UART_GDMA_Channel_Handler: Send int_uart_msg failed!");
        //Add user code here!

        GDMA_ClearINTPendingBit(UART_TX_GDMA_CHANNEL_NUM, GDMA_INT_Transfer);
        return;
    }
}
