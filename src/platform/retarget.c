/*
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

/*   0: redifine printf empty function
     1: redifine printf and using except log uart to print log
others: redifine printf just using log uart to print log*/
#define PRINTF_RETARGET_METHOD      0


#if ( PRINTF_RETARGET_METHOD == 0)
#include <stdio.h>
struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};
/* FILE is typedef in stdio.h. */
FILE __stdout;
int fputc(int ch, FILE *f)
{
    /* Your implementation of fputc(). */
    return ch;
}
int ferror(FILE *f)
{
    /* Your implementation of ferror(). */
    return 0;
}

#elif (PRINTF_RETARGET_METHOD == 1)
#include <stdio.h>
#include "rtl876x_uart.h"
/*
#include "rtl876x_nvic.h"
#include "rtl876x_uart.h"
#define UART_TX_PIN                P4_0
void board_uart_init(void)
{
    Pad_Config(UART_TX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pinmux_Config(UART_TX_PIN, UART0_TX);
}

void driver_uart_init(void)
{
    UART_DeInit(UART);

    RCC_PeriphClockCmd(APBPeriph_UART0, APBPeriph_UART0_CLOCK, ENABLE);

    // uart init
    UART_InitTypeDef UART_InitStruct;
    UART_StructInit(&UART_InitStruct);

    UART_InitStruct.parity         = UART_PARITY_NO_PARTY;
    UART_InitStruct.stopBits       = UART_STOP_BITS_1;
    UART_InitStruct.wordLen        = UART_WROD_LENGTH_8BIT;
    UART_InitStruct.rxTriggerLevel = 16;                      //1~29
    UART_InitStruct.idle_time      = UART_RX_IDLE_2BYTE;      //idle interrupt wait time

    UART_Init(UART, &UART_InitStruct);
}
*/

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};
/* FILE is typedef in stdio.h. */
FILE __stdout;
int fputc(int ch, FILE *f)
{
    /* Your implementation of fputc(). */
    while (UART_GetFlagState(UART, UART_FLAG_THR_TSR_EMPTY) != SET);
    UART_SendData(UART, &ch, 1);
    return ch;
}
int ferror(FILE *f)
{
    /* Your implementation of ferror(). */
    return 0;
}

#else  //print log by log_direct
#include <stdarg.h>
#include <stdio.h>
#include "os_sync.h"
#include "trace.h"
#include "otp.h"

#define LOG_MESSAGE_SYNC_CODE         0x7E
#define MAX_LOG_MESSAGE_LEN           128
#define MAX_LOG_MESSAGE_LEN_FOR_SNOOP 252
#define MAX_ARGUMENT_SIZE             20

#define GET_TYPE(info)              (uint8_t)((info) >> 24)
#define GET_SUBTYPE(info)           (uint8_t)((info) >> 16)
#define GET_MODULE(info)            (uint8_t)((info) >> 8)
#define GET_LEVEL(info)             (uint8_t)((info) >> 0)

extern uint32_t log_timestamp_get(void);
extern void LogUartTxChar(const uint8_t ch);
extern uint8_t log_seq_num;

int printf(const char *fmt, ...)
{
    char     l_msg[MAX_LOG_MESSAGE_LEN];
    uint16_t log_length;
    int16_t  fmt_length;
    uint32_t timestamp;
    va_list  ap;
    uint16_t i;
    uint32_t s;
    uint32_t info;

    /**
     * Byte: Description
     * 0:    Sync(0x7E)
     * 1:    Length
     * 2:    SeqNum
     * 3:    CheckSum
     * 4-5:  Timestamp
     * 6:    Type
     * 7:    SubType: SUBTYPE_DIRECT
     * 8:    Format String
     */

    if (!is_log_init || OTP->logDisable == 1)
    {
        return 0;
    }

    timestamp = log_timestamp_get();
    info = COMBINE_TRACE_INFO(TYPE_RTL8762C, SUBTYPE_DIRECT, 0, 0);

    timestamp = log_timestamp_get();

    l_msg[0] = LOG_MESSAGE_SYNC_CODE;
    l_msg[4] = (uint8_t)timestamp;
    l_msg[5] = (uint8_t)(timestamp >> 8);
    l_msg[6] = GET_TYPE(info);
    l_msg[7] = GET_SUBTYPE(info);

    log_length = 8;

    va_start(ap, fmt);
    fmt_length = vsnprintf(l_msg + 8, MAX_LOG_MESSAGE_LEN - 8, fmt, ap);
    if (fmt_length < 0) /* error occurred */
    {
        fmt_length = 0;
    }
    else if (log_length + fmt_length > MAX_LOG_MESSAGE_LEN) /* truncated */
    {
        log_length = MAX_LOG_MESSAGE_LEN;

    }
    else
    {
        log_length += fmt_length;
    }
    va_end(ap);

    l_msg[1] = log_length;

    s = os_lock();
    l_msg[2] = log_seq_num++;
    os_unlock(s);

    l_msg[3] = l_msg[0] ^ l_msg[1] ^ l_msg[2];

    for (i = 0; i < log_length; ++i)
    {
        LogUartTxChar(l_msg[i]);
    }

    return log_length;
}
#endif
