/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*********************************************************************************************************
* @file     xiaomi_gpio_test.c
* @brief
* @details
* @author
* @date
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "xiaomi_gpio_mp_test.h"

#include <string.h>


uint8_t GPIO_PIN_NUM[12][2] =
{
    {0, 3}, //P0_0    0, P0_3    3
    {6, 0}, //P0_6    6, P0_7    7
    {1, 2}, //P0_1    1, P0_2    2
    {4, 36}, //P0_4    4, H_0     36
    {8, 9}, //P1_0    8, P1_1    9
    {18, 19}, //P2_2    18,P2_3    19
    {20, 21}, //P2_4    20,P2_5    21
    {22, 23}, //P2_6    22,P2_7    23
    {26, 27}, //P3_2    26,P3_3    27

    {32, 33}, //P4_0    32,P4_1    33
    {34, 35}, //P4_2    34,P4_3    35
    {37, 38}, //H_1     37,H_2     38
//    {20,21},//P2_4    20,P2_5    21
//    {22,23},//P2_6    22,P2_7    23
//    {26,27},//P3_2    26,P3_3    27
};

uint8_t GPIO_Test_Result[12] = {0};

uint8_t *mp_test_gpio_forward(void)
{
    DBG_DIRECT("mp_test_gpio_forward");

    uint32_t pin_input = 0;
    uint32_t pin_output = 0;
    uint8_t gpio_input_data[12];

    GPIO_DeInit();
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    for (uint8_t i = 1; i < 9; i++)
    {
        /* GPIO pin input */
        Pad_Config(GPIO_PIN_NUM[i][0], PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
                   PAD_OUT_HIGH);
        Pinmux_Config(GPIO_PIN_NUM[i][0], DWGPIO);
        pin_input |= GPIO_GetPin(GPIO_PIN_NUM[i][0]);

        /* GPIO pin output */
        Pad_Config(GPIO_PIN_NUM[i][1], PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
                   PAD_OUT_HIGH);
        Pinmux_Config(GPIO_PIN_NUM[i][1], DWGPIO);
        pin_output |= GPIO_GetPin(GPIO_PIN_NUM[i][1]);
    }
    GPIO_InitStruct.GPIO_Pin    = pin_input;
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
    GPIO_Init(&GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin    = pin_output;
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
    GPIO_Init(&GPIO_InitStruct);

    /* output low */
    DBG_DIRECT("mp_test_gpio: output low");
    GPIO_Write(~pin_output);
//    for (uint32_t i = 0; i < 1000; i++);
    for (uint8_t i = 1; i < 9; i++)
    {
        gpio_input_data[i] = GPIO_ReadInputDataBit(GPIO_GetPin(GPIO_PIN_NUM[i][0]));
        if (0 != gpio_input_data[i]) { GPIO_Test_Result[i]++; }
        DBG_DIRECT("mp_test_gpio: gpio_input_data_%d = %d", GPIO_PIN_NUM[i][0], gpio_input_data[i]);
    }
    memset(gpio_input_data, 0, sizeof(gpio_input_data));

    /* output high */
    DBG_DIRECT("mp_test_gpio: output high");
    GPIO_Write(pin_output);
//    for (uint32_t i = 0; i < 1000; i++);
    for (uint8_t i = 1; i < 9; i++)
    {
        gpio_input_data[i] = GPIO_ReadInputDataBit(GPIO_GetPin(GPIO_PIN_NUM[i][0]));
        if (1 != gpio_input_data[i]) { GPIO_Test_Result[i]++; }
        DBG_DIRECT("mp_test_gpio: gpio_input_data_%d = %d", GPIO_PIN_NUM[i][0], gpio_input_data[i]);
    }
    memset(gpio_input_data, 0, sizeof(gpio_input_data));

    DBG_DIRECT("mp_test_gpio: output low");
    GPIO_Write(~pin_output);
//    for (uint32_t i = 0; i < 1000; i++);
    for (uint8_t i = 1; i < 9; i++)
    {
        gpio_input_data[i] = GPIO_ReadInputDataBit(GPIO_GetPin(GPIO_PIN_NUM[i][0]));
        if (0 != gpio_input_data[i]) { GPIO_Test_Result[i]++; }
        DBG_DIRECT("mp_test_gpio: gpio_input_data_%d = %d", GPIO_PIN_NUM[i][0], gpio_input_data[i]);
    }
    memset(gpio_input_data, 0, sizeof(gpio_input_data));

    GPIO_DeInit();
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
    GPIO_StructInit(&GPIO_InitStruct);
    pin_input = 0;
    pin_output = 0;
    for (uint8_t i = 1; i < 9; i++)
    {
        Pinmux_Config(GPIO_PIN_NUM[i][0], IDLE_MODE);
        Pinmux_Config(GPIO_PIN_NUM[i][1], IDLE_MODE);
    }

    for (uint8_t i = 9; i < 12; i++)
    {
        /* GPIO pin input */
        Pad_Config(GPIO_PIN_NUM[i][0], PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
                   PAD_OUT_HIGH);
        Pinmux_Config(GPIO_PIN_NUM[i][0], DWGPIO);
        pin_input |= GPIO_GetPin(GPIO_PIN_NUM[i][0]);

        /* GPIO pin output */
        Pad_Config(GPIO_PIN_NUM[i][1], PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
                   PAD_OUT_HIGH);
        Pinmux_Config(GPIO_PIN_NUM[i][1], DWGPIO);
        pin_output |= GPIO_GetPin(GPIO_PIN_NUM[i][1]);
    }
    GPIO_InitStruct.GPIO_Pin    = pin_input;
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
    GPIO_Init(&GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin    = pin_output;
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
    GPIO_Init(&GPIO_InitStruct);

    /* output low */
    DBG_DIRECT("mp_test_gpio: output low");
    GPIO_Write(~pin_output);
//    for (uint32_t i = 0; i < 1000; i++);
    for (uint8_t i = 9; i < 12; i++)
    {
        gpio_input_data[i] = GPIO_ReadInputDataBit(GPIO_GetPin(GPIO_PIN_NUM[i][0]));
        if (0 != gpio_input_data[i]) { GPIO_Test_Result[i]++; }
        DBG_DIRECT("mp_test_gpio: gpio_input_data_%d = %d", GPIO_PIN_NUM[i][0], gpio_input_data[i]);
    }
    memset(gpio_input_data, 0, sizeof(gpio_input_data));

    /* output high */
    DBG_DIRECT("mp_test_gpio: output high");
    GPIO_Write(pin_output);
//    for (uint32_t i = 0; i < 1000; i++);
    for (uint8_t i = 9; i < 12; i++)
    {
        gpio_input_data[i] = GPIO_ReadInputDataBit(GPIO_GetPin(GPIO_PIN_NUM[i][0]));
        if (1 != gpio_input_data[i]) { GPIO_Test_Result[i]++; }
        DBG_DIRECT("mp_test_gpio: gpio_input_data_%d = %d", GPIO_PIN_NUM[i][0], gpio_input_data[i]);
    }
    memset(gpio_input_data, 0, sizeof(gpio_input_data));

    /* output low */
    DBG_DIRECT("mp_test_gpio: output low");
    GPIO_Write(~pin_output);
//    for (uint32_t i = 0; i < 100000; i++);
    for (uint8_t i = 9; i < 12; i++)
    {
        gpio_input_data[i] = GPIO_ReadInputDataBit(GPIO_GetPin(GPIO_PIN_NUM[i][0]));
        if (0 != gpio_input_data[i]) { GPIO_Test_Result[i]++; }
        DBG_DIRECT("mp_test_gpio: gpio_input_data_%d = %d", GPIO_PIN_NUM[i][0], gpio_input_data[i]);
    }
    memset(gpio_input_data, 0, sizeof(gpio_input_data));
    APP_PRINT_INFO1("GPIO_Test_Result %b", TRACE_BINARY(12, GPIO_Test_Result));
    return GPIO_Test_Result;//right 0; error ~0
}

uint8_t *mp_test_gpio_backward(void)
{
    DBG_DIRECT("mp_test_gpio_backward");

    uint32_t pin_input = 0;
    uint32_t pin_output = 0;
    uint8_t gpio_input_data[12];

    GPIO_DeInit();
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    for (uint8_t i = 1; i < 9; i++)
    {
        /* GPIO pin input */
        Pad_Config(GPIO_PIN_NUM[i][1], PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
                   PAD_OUT_HIGH);
        Pinmux_Config(GPIO_PIN_NUM[i][1], DWGPIO);
        pin_input |= GPIO_GetPin(GPIO_PIN_NUM[i][1]);

        /* GPIO pin output */
        Pad_Config(GPIO_PIN_NUM[i][0], PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
                   PAD_OUT_HIGH);
        Pinmux_Config(GPIO_PIN_NUM[i][0], DWGPIO);
        pin_output |= GPIO_GetPin(GPIO_PIN_NUM[i][0]);
    }
    GPIO_InitStruct.GPIO_Pin    = pin_input;
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
    GPIO_Init(&GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin    = pin_output;
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
    GPIO_Init(&GPIO_InitStruct);

    /* output low */
    DBG_DIRECT("mp_test_gpio: output low");
    GPIO_Write(~pin_output);
    for (uint32_t i = 0; i < 1000; i++);
    for (uint8_t i = 1; i < 9; i++)
    {
        gpio_input_data[i] = GPIO_ReadInputDataBit(GPIO_GetPin(GPIO_PIN_NUM[i][1]));
        if (0 != gpio_input_data[i]) { GPIO_Test_Result[i]++; }
        DBG_DIRECT("mp_test_gpio: gpio_input_data_%d = %d", GPIO_PIN_NUM[i][1], gpio_input_data[i]);
    }
    memset(gpio_input_data, 0, sizeof(gpio_input_data));

    /* output high */
    DBG_DIRECT("mp_test_gpio: output high");
    GPIO_Write(pin_output);
    for (uint32_t i = 0; i < 1000; i++);
    for (uint8_t i = 1; i < 9; i++)
    {
        gpio_input_data[i] = GPIO_ReadInputDataBit(GPIO_GetPin(GPIO_PIN_NUM[i][1]));
        if (1 != gpio_input_data[i]) { GPIO_Test_Result[i]++; }
        DBG_DIRECT("mp_test_gpio: gpio_input_data_%d = %d", GPIO_PIN_NUM[i][1], gpio_input_data[i]);
    }
    memset(gpio_input_data, 0, sizeof(gpio_input_data));

    /* output low */
    DBG_DIRECT("mp_test_gpio: output low");
    GPIO_Write(~pin_output);
    for (uint32_t i = 0; i < 1000; i++);
    for (uint8_t i = 1; i < 9; i++)
    {
        gpio_input_data[i] = GPIO_ReadInputDataBit(GPIO_GetPin(GPIO_PIN_NUM[i][1]));
        if (0 != gpio_input_data[i]) { GPIO_Test_Result[i]++; }
        DBG_DIRECT("mp_test_gpio: gpio_input_data_%d = %d", GPIO_PIN_NUM[i][1], gpio_input_data[i]);
    }
    memset(gpio_input_data, 0, sizeof(gpio_input_data));

    GPIO_DeInit();
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
    GPIO_StructInit(&GPIO_InitStruct);
    pin_input = 0;
    pin_output = 0;
    for (uint8_t i = 1; i < 9; i++)
    {
        Pinmux_Config(GPIO_PIN_NUM[i][0], IDLE_MODE);
        Pinmux_Config(GPIO_PIN_NUM[i][1], IDLE_MODE);
    }

    for (uint8_t i = 9; i < 12; i++)
    {
        /* GPIO pin input */
        Pad_Config(GPIO_PIN_NUM[i][1], PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
                   PAD_OUT_HIGH);
        Pinmux_Config(GPIO_PIN_NUM[i][1], DWGPIO);
        pin_input |= GPIO_GetPin(GPIO_PIN_NUM[i][1]);

        /* GPIO pin output */
        Pad_Config(GPIO_PIN_NUM[i][0], PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
                   PAD_OUT_HIGH);
        Pinmux_Config(GPIO_PIN_NUM[i][0], DWGPIO);
        pin_output |= GPIO_GetPin(GPIO_PIN_NUM[i][0]);
    }
    GPIO_InitStruct.GPIO_Pin    = pin_input;
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
    GPIO_Init(&GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin    = pin_output;
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
    GPIO_Init(&GPIO_InitStruct);

    /* output low */
    DBG_DIRECT("mp_test_gpio: output low");
    GPIO_Write(~pin_output);
    for (uint32_t i = 0; i < 1000; i++);
    for (uint8_t i = 9; i < 12; i++)
    {
        gpio_input_data[i] = GPIO_ReadInputDataBit(GPIO_GetPin(GPIO_PIN_NUM[i][1]));
        if (0 != gpio_input_data[i]) { GPIO_Test_Result[i]++; }
        DBG_DIRECT("mp_test_gpio: gpio_input_data_%d = %d", GPIO_PIN_NUM[i][1], gpio_input_data[i]);
    }
    memset(gpio_input_data, 0, sizeof(gpio_input_data));

    /* output high */
    DBG_DIRECT("mp_test_gpio: output high");
    GPIO_Write(pin_output);
    for (uint32_t i = 0; i < 1000; i++);
    for (uint8_t i = 9; i < 12; i++)
    {
        gpio_input_data[i] = GPIO_ReadInputDataBit(GPIO_GetPin(GPIO_PIN_NUM[i][1]));
        if (1 != gpio_input_data[i]) { GPIO_Test_Result[i]++; }
        DBG_DIRECT("mp_test_gpio: gpio_input_data_%d = %d", GPIO_PIN_NUM[i][1], gpio_input_data[i]);
    }
    memset(gpio_input_data, 0, sizeof(gpio_input_data));

    /* output low */
    DBG_DIRECT("mp_test_gpio: output low");
    GPIO_Write(~pin_output);
    for (uint32_t i = 0; i < 1000; i++);
    for (uint8_t i = 9; i < 12; i++)
    {
        gpio_input_data[i] = GPIO_ReadInputDataBit(GPIO_GetPin(GPIO_PIN_NUM[i][1]));
        if (0 != gpio_input_data[i]) { GPIO_Test_Result[i]++; }
        DBG_DIRECT("mp_test_gpio: gpio_input_data_%d = %d", GPIO_PIN_NUM[i][1], gpio_input_data[i]);
    }
    memset(gpio_input_data, 0, sizeof(gpio_input_data));

    APP_PRINT_INFO1("GPIO_Test_Result %b", TRACE_BINARY(12, GPIO_Test_Result));
    return GPIO_Test_Result;//right 0; error ~0
}

