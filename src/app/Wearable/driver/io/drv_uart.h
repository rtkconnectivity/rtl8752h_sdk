/*
 * Copyright (c) 2022
 *
 *
 *
 *
 *
 *
 */

#ifndef __DRV_UART_H__
#define __DRV_UART_H__

#include "board.h"
#include "rtl_hal_peripheral.h"
#include "drv_dlps.h"



typedef enum
{
    BAUD_RATE_9600,
    BAUD_RATE_19200,
    BAUD_RATE_115200,
    BAUD_RATE_230400,
    BAUD_RATE_460800,
    BAUD_RATE_921600,
    BAUD_RATE_2000000,
    BAUD_RATE_3000000,
    BAUD_RATE_4000000,
    BAUD_RATE_6000000,
} UART_BaudRate_Def;


typedef struct
{
    uint16_t div;
    uint16_t ovsr;
    uint16_t ovsr_adj;
} UART_BaudRate_TypeDef;

typedef void (*pfunc_uart_rx_indicate)(uint8_t *data, uint32_t len);



void hw_uart_init(void);

void drv_uart0_init(uint8_t tx_pin, uint8_t rx_pin);
uint32_t drv_uart0_write(const void *buffer, uint32_t size);
void drv_uart0_set_rx_indicate(pfunc_uart_rx_indicate rx_ind);

void drv_uart1_init(uint8_t tx_pin, uint8_t rx_pin);
uint32_t drv_uart1_write(const void *buffer, uint32_t size);
void drv_uart1_set_rx_indicate(pfunc_uart_rx_indicate rx_ind);

void drv_uart2_init(uint8_t tx_pin, uint8_t rx_pin);
uint32_t drv_uart2_write(const void *buffer, uint32_t size);
void drv_uart2_set_rx_indicate(pfunc_uart_rx_indicate rx_ind);

void drv_uart3_init(uint8_t tx_pin, uint8_t rx_pin);
uint32_t drv_uart3_write(const void *buffer, uint32_t size);
void drv_uart3_set_rx_indicate(pfunc_uart_rx_indicate rx_ind);

void drv_uart4_init(uint8_t tx_pin, uint8_t rx_pin);
uint32_t drv_uart4_write(const void *buffer, uint32_t size);
void drv_uart4_set_rx_indicate(pfunc_uart_rx_indicate rx_ind);

void drv_uart5_init(uint8_t tx_pin, uint8_t rx_pin);
uint32_t drv_uart5_write(const void *buffer, uint32_t size);
void drv_uart5_set_rx_indicate(pfunc_uart_rx_indicate rx_ind);


#endif  /* __DRV_UART_H__ */

/******************* end of file ***************/
