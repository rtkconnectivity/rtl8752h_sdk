#ifndef _QSPI_LCD_PLATFORM_H_
#define _QSPI_LCD_PLATFORM_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void qspi_lcd_platform_init(void);

#define LCD_PINMU_REG_SEL_ADDR                      (0x400002A8UL)
#define LCD_PINMUX_POS                              (28)
#define LCD_PINMUX_MSK                              (0x3 << LCD_PINMUX_POS)
#define LCD_PINMUX_CLR                              (~LCD_PINMUX_MSK)
#define LCD_PINMUX_SPIC1                            (0x2 << LCD_PINMUX_POS)

#define SPIC1_CSX           P4_3
#define SPIC1_DCX           P3_4
#define SPIC1_CLK           P4_0

#define SPIC1_D0            P4_2
#define SPIC1_D1            P4_1
#define SPIC1_D2            P3_2
#define SPIC1_D3            P3_3

#define SPIC1_TEAR_OUTPUT_PIN       P1_0
#define SPIC1_TEAR_INPUT_PIN        P0_5
#define SPIC1_LCD_RESX_OUTPUT_PIN   P3_6

/*LCD pinmux position for each LCD pin*/
#define LCD_PINMUX_REG_ADDR                     (0x400002ACUL)
#define P0_1_LCD_PINMUX_POS           (0)
#define P0_2_LCD_PINMUX_POS           (1)
#define P0_4_LCD_PINMUX_POS           (2)
#define P0_5_LCD_PINMUX_POS           (3)
#define P2_0_LCD_PINMUX_POS           (4)
#define P3_2_LCD_PINMUX_POS           (5)
#define P3_3_LCD_PINMUX_POS           (6)
#define P3_4_LCD_PINMUX_POS           (7)
#define P3_5_LCD_PINMUX_POS           (8)
#define P3_6_LCD_PINMUX_POS           (9)
#define P4_0_LCD_PINMUX_POS           (10)
#define P4_1_LCD_PINMUX_POS           (11)
#define P4_2_LCD_PINMUX_POS           (12)
#define P4_3_LCD_PINMUX_POS           (13)

#define SPIC1_GDMA_Channel_NUM             0
#define SPIC1_GDMA_Channel                 GDMA_Channel0
#define SPIC1_GDMA_Channel_IRQn            GDMA0_Channel0_IRQn
#define SPIC1_GDMA_Channel_Handler         GDMA0_Channel0_Handler

#define LCD_DMA_CHANNEL_NUM              SPIC1_GDMA_Channel_NUM
#define LCD_DMA_CHANNEL_INDEX            SPIC1_GDMA_Channel
#define LCD_DMA_CHANNEL_IRQ              SPIC1_GDMA_Channel_IRQn


#ifdef __cplusplus
}
#endif

#endif /* _QSPI_LCD_PLATFORM_H_ */
