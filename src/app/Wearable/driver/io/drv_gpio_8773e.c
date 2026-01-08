/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-11-06     howie        first version
 */


void drv_pin_clk_set(uint32_t pin)
{
    if (pin <= 6)
    {
        RCC_PeriphClockCmd(APBPeriph_GPIOA, APBPeriph_GPIOA_CLOCK, ENABLE);
    }
    else if (pin <= 7)
    {
        RCC_PeriphClockCmd(APBPeriph_GPIOB, APBPeriph_GPIOB_CLOCK, ENABLE);
    }
    else if (pin <= 32)
    {
        RCC_PeriphClockCmd(APBPeriph_GPIOA, APBPeriph_GPIOA_CLOCK, ENABLE);
    }
    else if (pin <= 54)
    {
        RCC_PeriphClockCmd(APBPeriph_GPIOB, APBPeriph_GPIOB_CLOCK, ENABLE);
    }
    else if (pin <= 62)
    {
        RCC_PeriphClockCmd(APBPeriph_GPIOA, APBPeriph_GPIOA_CLOCK, ENABLE);
    }
    else if (pin <= 87)
    {
        RCC_PeriphClockCmd(APBPeriph_GPIOB, APBPeriph_GPIOB_CLOCK, ENABLE);
    }
}
void DRV_GPIO_0_Handler(void) {DRV_GPIO_HANDLER(P0_0);}
void DRV_GPIO_1_Handler(void) {DRV_GPIO_HANDLER(P0_1);}
void DRV_GPIO_2_Handler(void) {DRV_GPIO_HANDLER(P0_2);}
void DRV_GPIO_3_Handler(void) {DRV_GPIO_HANDLER(P0_3);}
void DRV_GPIO_4_Handler(void) {DRV_GPIO_HANDLER(P3_2);}
void DRV_GPIO_5_Handler(void) {DRV_GPIO_HANDLER(P3_3);}
void DRV_GPIO_6_Handler(void) {DRV_GPIO_HANDLER(P3_4);}
void DRV_GPIO_7_Handler(void) {DRV_GPIO_HANDLER(P1_0);}
void DRV_GPIO_8_Handler(void) {DRV_GPIO_HANDLER(P1_1);}
void DRV_GPIO_9_Handler(void) {DRV_GPIO_HANDLER(P1_2);}
void DRV_GPIO_10_Handler(void) {DRV_GPIO_HANDLER(P1_3);}
void DRV_GPIO_11_Handler(void) {DRV_GPIO_HANDLER(P1_4);}
void DRV_GPIO_12_Handler(void) {DRV_GPIO_HANDLER(P1_5);}
void DRV_GPIO_13_Handler(void) {DRV_GPIO_HANDLER(P1_6);}
void DRV_GPIO_14_Handler(void) {DRV_GPIO_HANDLER(P1_7);}
void DRV_GPIO_15_Handler(void) {DRV_GPIO_HANDLER(P2_0);}
void DRV_GPIO_16_Handler(void) {DRV_GPIO_HANDLER(P2_1);}
void DRV_GPIO_17_Handler(void) {DRV_GPIO_HANDLER(P2_2);}
void DRV_GPIO_18_Handler(void) {DRV_GPIO_HANDLER(P2_3);}
void DRV_GPIO_19_Handler(void) {DRV_GPIO_HANDLER(P2_4);}
void DRV_GPIO_20_Handler(void) {DRV_GPIO_HANDLER(P2_5);}
void DRV_GPIO_21_Handler(void) {DRV_GPIO_HANDLER(P2_6);}
void DRV_GPIO_22_Handler(void) {DRV_GPIO_HANDLER(P2_7);}
void DRV_GPIO_23_Handler(void) {DRV_GPIO_HANDLER(P3_0);}
void DRV_GPIO_24_Handler(void) {DRV_GPIO_HANDLER(P3_1);}
void DRV_GPIO_25_Handler(void) {DRV_GPIO_HANDLER(MIC2_P);}
void DRV_GPIO_26_Handler(void) {DRV_GPIO_HANDLER(MIC2_N);}
void DRV_GPIO_27_Handler(void) {DRV_GPIO_HANDLER(MIC1_P);}
void DRV_GPIO_28_Handler(void) {DRV_GPIO_HANDLER(MIC1_N);}
void DRV_GPIO_29_Handler(void) {DRV_GPIO_HANDLER(MICBIAS);}
void DRV_GPIO_A25_Handler(void) {DRV_GPIO_HANDLER(P6_0);}
void DRV_GPIO_A26_Handler(void) {DRV_GPIO_HANDLER(P6_1);}
void DRV_GPIO_A27_Handler(void) {DRV_GPIO_HANDLER(P6_2);}
void DRV_GPIO_A28_Handler(void) {DRV_GPIO_HANDLER(P6_3);}
void DRV_GPIO_A29_Handler(void) {DRV_GPIO_HANDLER(P6_4);}
void DRV_GPIO_A30_2_Handler(void) {DRV_GPIO_HANDLER(P6_5);}
void DRV_GPIO_A30_Handler(void) {DRV_GPIO_HANDLER(DAOUT_P);}
void DRV_GPIO_A31_2_Handler(void) {DRV_GPIO_HANDLER(P6_6);}
void DRV_GPIO_A31_Handler(void) {DRV_GPIO_HANDLER(DAOUT_N);}


void DRV_GPIO_B0_Handler(void) {DRV_GPIO_HANDLER(P9_0);}
void DRV_GPIO_B0_2_Handler(void) {DRV_GPIO_HANDLER(P5_4);}
void DRV_GPIO_B1_Handler(void) {DRV_GPIO_HANDLER(P9_1);}
void DRV_GPIO_B1_2_Handler(void) {DRV_GPIO_HANDLER(P5_5);}
void DRV_GPIO_B2_Handler(void) {DRV_GPIO_HANDLER(P9_2);}
void DRV_GPIO_B2_2_Handler(void) {DRV_GPIO_HANDLER(P5_6);}
void DRV_GPIO_B3_Handler(void) {DRV_GPIO_HANDLER(P9_3);}
void DRV_GPIO_B4_Handler(void) {DRV_GPIO_HANDLER(P9_4);}
void DRV_GPIO_B5_Handler(void) {DRV_GPIO_HANDLER(P9_5);}
void DRV_GPIO_B6_Handler(void) {DRV_GPIO_HANDLER(P3_5);}
void DRV_GPIO_B6_2_Handler(void) {DRV_GPIO_HANDLER(P9_6);}
void DRV_GPIO_B7_Handler(void) {DRV_GPIO_HANDLER(P4_0);}
void DRV_GPIO_B8_Handler(void) {DRV_GPIO_HANDLER(P4_1);}
void DRV_GPIO_B9_Handler(void) {DRV_GPIO_HANDLER(P4_2);}
void DRV_GPIO_B10_Handler(void) {DRV_GPIO_HANDLER(P4_3);}
void DRV_GPIO_B11_Handler(void) {DRV_GPIO_HANDLER(P4_4);}
void DRV_GPIO_B12_Handler(void) {DRV_GPIO_HANDLER(P4_5);}
void DRV_GPIO_B13_Handler(void) {DRV_GPIO_HANDLER(P4_6);}
void DRV_GPIO_B14_Handler(void) {DRV_GPIO_HANDLER(P4_7);}
void DRV_GPIO_B15_Handler(void) {DRV_GPIO_HANDLER(P5_0);}
void DRV_GPIO_B16_Handler(void) {DRV_GPIO_HANDLER(P5_1);}
void DRV_GPIO_B17_Handler(void) {DRV_GPIO_HANDLER(P5_2);}
void DRV_GPIO_B18_Handler(void) {DRV_GPIO_HANDLER(P5_3);}
void DRV_GPIO_B19_Handler(void) {DRV_GPIO_HANDLER(P7_0);}
void DRV_GPIO_B19_2_Handler(void) {DRV_GPIO_HANDLER(P8_6);}
void DRV_GPIO_B20_Handler(void) {DRV_GPIO_HANDLER(P7_1);}
void DRV_GPIO_B20_2_Handler(void) {DRV_GPIO_HANDLER(P8_7);}
void DRV_GPIO_B21_Handler(void) {DRV_GPIO_HANDLER(P7_2);}
void DRV_GPIO_B22_Handler(void) {DRV_GPIO_HANDLER(P7_3);}
void DRV_GPIO_B23_Handler(void) {DRV_GPIO_HANDLER(P7_4);}
void DRV_GPIO_B24_Handler(void) {DRV_GPIO_HANDLER(P7_5);}
void DRV_GPIO_B25_Handler(void) {DRV_GPIO_HANDLER(P7_6);}
void DRV_GPIO_B26_Handler(void) {DRV_GPIO_HANDLER(P8_0);}
void DRV_GPIO_B27_Handler(void) {DRV_GPIO_HANDLER(P8_1);}
void DRV_GPIO_B28_Handler(void) {DRV_GPIO_HANDLER(P8_2);}
void DRV_GPIO_B29_Handler(void) {DRV_GPIO_HANDLER(P8_3);}
void DRV_GPIO_B30_Handler(void) {DRV_GPIO_HANDLER(P8_4);}
void DRV_GPIO_B31_Handler(void) {DRV_GPIO_HANDLER(P8_5);}



const IRQn_Type table_irq[GPIO_NUM_MAX] =
{
    GPIO0_IRQn,
    GPIO1_IRQn,
    GPIO2_IRQn,
    GPIO3_IRQn,
    GPIO4_IRQn,
    GPIO5_IRQn,
    GPIO6_IRQn,
    GPIO38_IRQn,
    GPIO7_IRQn,
    GPIO8_IRQn,
    GPIO9_IRQn,
    GPIO10_IRQn,
    GPIO11_IRQn,
    GPIO12_IRQn,
    GPIO13_IRQn,
    GPIO14_IRQn,
    GPIO15_IRQn,
    GPIO16_IRQn,
    GPIO17_IRQn,
    GPIO18_IRQn,
    GPIO19_IRQn,
    GPIO20_IRQn,
    GPIO21_IRQn,
    GPIO22_IRQn,
    GPIO23_IRQn,
    GPIO24_IRQn,
    GPIO30_IRQn,
    GPIO31_IRQn,
    GPIO27_IRQn,
    GPIO28_IRQn,
    GPIO25_IRQn,
    GPIO26_IRQn,
    GPIO29_IRQn,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    GPIO39_IRQn,
    GPIO40_IRQn,
    GPIO41_IRQn,
    GPIO42_IRQn,
    GPIO43_IRQn,
    GPIO44_IRQn,
    GPIO45_IRQn,
    GPIO46_IRQn,
    GPIO47_IRQn,
    GPIO48_IRQn,
    GPIO49_IRQn,
    GPIO50_IRQn,
    GPIO32_IRQn,
    GPIO33_IRQn,
    GPIO34_IRQn,
    NULL,
    GPIO25_IRQn,
    GPIO26_IRQn,
    GPIO27_IRQn,
    GPIO28_IRQn,
    GPIO29_IRQn,
    GPIO30_IRQn,
    GPIO31_IRQn,
    NULL,
    GPIO51_IRQn,
    GPIO52_IRQn,
    GPIO53_IRQn,
    GPIO54_IRQn,
    GPIO55_IRQn,
    GPIO56_IRQn,
    GPIO57_IRQn,
    NULL,
    GPIO58_IRQn,
    GPIO59_IRQn,
    GPIO60_IRQn,
    GPIO61_IRQn,
    GPIO62_IRQn,
    GPIO62_IRQn,
    GPIO51_IRQn,
    GPIO52_IRQn,
    GPIO32_IRQn,
    GPIO33_IRQn,
    GPIO34_IRQn,
    GPIO35_IRQn,
    GPIO36_IRQn,
    GPIO37_IRQn,
    NULL,
    GPIO38_IRQn,
};

const IRQ_Fun table_func[GPIO_NUM_MAX] =
{
    DRV_GPIO_0_Handler,
    DRV_GPIO_1_Handler,
    DRV_GPIO_2_Handler,
    DRV_GPIO_3_Handler,
    DRV_GPIO_4_Handler,
    DRV_GPIO_5_Handler,
    DRV_GPIO_6_Handler,
    DRV_GPIO_B6_Handler,
    DRV_GPIO_7_Handler,
    DRV_GPIO_8_Handler,
    DRV_GPIO_9_Handler,
    DRV_GPIO_10_Handler,
    DRV_GPIO_11_Handler,
    DRV_GPIO_12_Handler,
    DRV_GPIO_13_Handler,
    DRV_GPIO_14_Handler,
    DRV_GPIO_15_Handler,
    DRV_GPIO_16_Handler,
    DRV_GPIO_17_Handler,
    DRV_GPIO_18_Handler,
    DRV_GPIO_19_Handler,
    DRV_GPIO_20_Handler,
    DRV_GPIO_21_Handler,
    DRV_GPIO_22_Handler,
    DRV_GPIO_23_Handler,
    DRV_GPIO_24_Handler,
    DRV_GPIO_A30_Handler,
    DRV_GPIO_A31_Handler,
    DRV_GPIO_27_Handler,
    DRV_GPIO_28_Handler,
    DRV_GPIO_25_Handler,
    DRV_GPIO_26_Handler,
    DRV_GPIO_29_Handler,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    DRV_GPIO_B7_Handler,
    DRV_GPIO_B8_Handler,
    DRV_GPIO_B9_Handler,
    DRV_GPIO_B10_Handler,
    DRV_GPIO_B11_Handler,
    DRV_GPIO_B12_Handler,
    DRV_GPIO_B13_Handler,
    DRV_GPIO_B14_Handler,
    DRV_GPIO_B15_Handler,
    DRV_GPIO_B16_Handler,
    DRV_GPIO_B17_Handler,
    DRV_GPIO_B18_Handler,
    DRV_GPIO_B0_Handler,
    DRV_GPIO_B1_Handler,
    DRV_GPIO_B2_Handler,
    NULL,
    DRV_GPIO_A25_Handler,
    DRV_GPIO_A26_Handler,
    DRV_GPIO_A27_Handler,
    DRV_GPIO_A28_Handler,
    DRV_GPIO_A29_Handler,
    DRV_GPIO_A30_2_Handler,
    DRV_GPIO_A31_2_Handler,
    NULL,
    DRV_GPIO_B19_Handler,
    DRV_GPIO_B20_Handler,
    DRV_GPIO_B21_Handler,
    DRV_GPIO_B22_Handler,
    DRV_GPIO_B23_Handler,
    DRV_GPIO_B24_Handler,
    DRV_GPIO_B25_Handler,
    NULL,
    DRV_GPIO_B26_Handler,
    DRV_GPIO_B27_Handler,
    DRV_GPIO_B28_Handler,
    DRV_GPIO_B29_Handler,
    DRV_GPIO_B30_Handler,
    DRV_GPIO_B31_Handler,
    DRV_GPIO_B19_2_Handler,
    DRV_GPIO_B20_2_Handler,
    DRV_GPIO_B0_Handler,
    DRV_GPIO_B1_Handler,
    DRV_GPIO_B3_Handler,
    DRV_GPIO_B4_Handler,
    DRV_GPIO_B5_Handler,
    NULL,
    DRV_GPIO_B6_Handler,
};
const VECTORn_Type table_vector[GPIO_NUM_MAX] =
{
    GPIO_A0_VECTORn,
    GPIO_A1_VECTORn,
    GPIO_A_2_7_VECTORn,
    GPIO_A_2_7_VECTORn,
    GPIO_A_2_7_VECTORn,
    GPIO_A_2_7_VECTORn,
    GPIO_B_0_7_VECTORn,
    GPIO_A_2_7_VECTORn,
    GPIO_A_8_15_VECTORn,
    GPIO_A_8_15_VECTORn,
    GPIO_A_8_15_VECTORn,
    GPIO_A_8_15_VECTORn,
    GPIO_A_8_15_VECTORn,
    GPIO_A_8_15_VECTORn,
    GPIO_A_8_15_VECTORn,
    GPIO_A_8_15_VECTORn,
    GPIO_A_16_23_VECTORn,
    GPIO_A_16_23_VECTORn,
    GPIO_A_16_23_VECTORn,
    GPIO_A_16_23_VECTORn,
    GPIO_A_16_23_VECTORn,
    GPIO_A_16_23_VECTORn,
    GPIO_A_16_23_VECTORn,
    GPIO_A_16_23_VECTORn,
    GPIO_A_24_31_VECTORn,
    GPIO_A_24_31_VECTORn,
    GPIO_A_24_31_VECTORn,
    GPIO_A_24_31_VECTORn,
    GPIO_A_24_31_VECTORn,
    GPIO_A_24_31_VECTORn,
    GPIO_A_24_31_VECTORn,
    GPIO_A_24_31_VECTORn,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    GPIO_B_0_7_VECTORn,
    GPIO_B_8_15_VECTORn,
    GPIO_B_8_15_VECTORn,
    GPIO_B_8_15_VECTORn,
    GPIO_B_8_15_VECTORn,
    GPIO_B_8_15_VECTORn,
    GPIO_B_8_15_VECTORn,
    GPIO_B_8_15_VECTORn,
    GPIO_B_8_15_VECTORn,
    GPIO_B_16_23_VECTORn,
    GPIO_B_16_23_VECTORn,
    GPIO_B_16_23_VECTORn,
    GPIO_B_0_7_VECTORn,
    GPIO_B_0_7_VECTORn,
    GPIO_B_0_7_VECTORn,
    NULL,
    GPIO_A_24_31_VECTORn,
    GPIO_A_24_31_VECTORn,
    GPIO_A_24_31_VECTORn,
    GPIO_A_24_31_VECTORn,
    GPIO_A_24_31_VECTORn,
    GPIO_A_24_31_VECTORn,
    GPIO_A_24_31_VECTORn,
    NULL,
    GPIO_B_24_31_VECTORn,
    GPIO_B_24_31_VECTORn,
    GPIO_B_24_31_VECTORn,
    GPIO_B_24_31_VECTORn,
    GPIO_B_24_31_VECTORn,
    GPIO_B_24_31_VECTORn,
    GPIO_B_16_23_VECTORn,
    GPIO_B_16_23_VECTORn,
    GPIO_B_0_7_VECTORn,
    GPIO_B_0_7_VECTORn,
    GPIO_B_0_7_VECTORn,
    GPIO_B_0_7_VECTORn,
    GPIO_B_0_7_VECTORn,
    GPIO_B_0_7_VECTORn,
    NULL,
    GPIO_B_0_7_VECTORn,
};




