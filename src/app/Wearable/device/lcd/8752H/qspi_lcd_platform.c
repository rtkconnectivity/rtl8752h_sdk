#include "qspi_lcd_platform.h"
#include "rtl876x.h"
#include "rtl876x_rcc.h"
#include "rtl876x_pinmux.h"
#include "trace.h"

void SPIC1_PAD_Configuration(void)
{
    Pad_Config(SPIC1_D0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(SPIC1_D1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(SPIC1_D2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(SPIC1_D3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

    /* CS */
    Pad_Config(SPIC1_CSX, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    /* DCX */
    Pad_Config(SPIC1_DCX, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    /* CLK */
    Pad_Config(SPIC1_CLK, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
}

uint32_t get_lcd_pinmux_pos(uint32_t pin_number)
{
    if (pin_number == P0_1)
    {
        return P0_1_LCD_PINMUX_POS;
    }

    else if (pin_number == P0_2)
    {
        return P0_2_LCD_PINMUX_POS;
    }

    else if (pin_number == P0_4)
    {
        return P0_4_LCD_PINMUX_POS;
    }

    else if (pin_number == P0_5)
    {
        return P0_5_LCD_PINMUX_POS;
    }

    else if (pin_number == P2_0)
    {
        return P2_0_LCD_PINMUX_POS;
    }

    else if (pin_number == P3_2)
    {
        return P3_2_LCD_PINMUX_POS;
    }

    else if (pin_number == P3_3)
    {
        return P3_3_LCD_PINMUX_POS;
    }

    else if (pin_number == P3_4)
    {
        return P3_4_LCD_PINMUX_POS;
    }

    else if (pin_number == P3_5)
    {
        return P3_5_LCD_PINMUX_POS;
    }

    else if (pin_number == P3_6)
    {
        return P3_6_LCD_PINMUX_POS;
    }

    else if (pin_number == P4_0)
    {
        return P4_0_LCD_PINMUX_POS;
    }

    else if (pin_number == P4_1)
    {
        return P4_1_LCD_PINMUX_POS;
    }

    else if (pin_number == P4_2)
    {
        return P4_2_LCD_PINMUX_POS;
    }

    else if (pin_number == P4_3)
    {
        return P4_3_LCD_PINMUX_POS;
    }
    else
    {
        return 0;
    }
}

void LCD_PINMUX_ENABLE(uint32_t pin_number, FunctionalState NewState)
{
    if (NewState == ENABLE)
    {
        *((volatile uint32_t *)LCD_PINMUX_REG_ADDR) |= BIT(get_lcd_pinmux_pos(pin_number));
    }
    else
    {
        *((volatile uint32_t *)LCD_PINMUX_REG_ADDR) &= ~BIT(get_lcd_pinmux_pos(pin_number));
    }
}

void SPIC1_PINMUX_Configuration(void)
{
    /*turn on the SPIC1 pinmux*/
    // SYSBLKCTRL->u_2A8.BITS_2A8.r_PMUX_LCD_EN = 2;

    *((volatile uint32_t *)LCD_PINMU_REG_SEL_ADDR) &= LCD_PINMUX_CLR;
    *((volatile uint32_t *)LCD_PINMU_REG_SEL_ADDR) |= LCD_PINMUX_SPIC1;
    /*de-init the pinmux configuration of QSPI*/
    /* QSPI interface: D0~D3 */
    Pinmux_Config(SPIC1_D0, IDLE_MODE);
    Pinmux_Config(SPIC1_D1, IDLE_MODE);
    Pinmux_Config(SPIC1_D2, IDLE_MODE);
    Pinmux_Config(SPIC1_D3, IDLE_MODE);
    /* CS */
    Pinmux_Config(SPIC1_CSX, IDLE_MODE);
    /* DCX */
    Pinmux_Config(SPIC1_DCX, IDLE_MODE);
    /* CLK */
    Pinmux_Config(SPIC1_CLK, IDLE_MODE);

    LCD_PINMUX_ENABLE(SPIC1_D0, ENABLE);
    LCD_PINMUX_ENABLE(SPIC1_D1, ENABLE);
    LCD_PINMUX_ENABLE(SPIC1_D2, ENABLE);
    LCD_PINMUX_ENABLE(SPIC1_D3, ENABLE);
    LCD_PINMUX_ENABLE(SPIC1_CSX, ENABLE);
    LCD_PINMUX_ENABLE(SPIC1_DCX, ENABLE);
    LCD_PINMUX_ENABLE(SPIC1_CLK, ENABLE);

    LCD_PINMUX_ENABLE(SPIC1_LCD_RESX_OUTPUT_PIN, DISABLE);

}

void qspi_lcd_platform_init(void)
{
    /*SPIC1 PAD and pinmux configuration*/
    SPIC1_PAD_Configuration();
    SPIC1_PINMUX_Configuration();

    // /* Open SPIC1 clock */
    RCC_PeriphClockCmd(APBPeriph_FLASH1, APBPeriph_FLASH1_CLOCK, ENABLE);

    // /*Initialize SPIC1 peripheral */
    SPIC1->CTRLR0 = BIT31;

    SPIC1->DMATDLR = 16 / 2;
    SPIC1->DMARDLR = 8 / 2;

    SPIC1->TXFTLR = 0;
    SPIC1->RXFTLR = 0;

    SPIC1->DMACR = 1 << 1;
    SPIC1->BAUDR = 1;
    SPIC1->FBAUDR = 1;

    SPIC1->USER_LENGTH = 3 << 16 | 1 << 12 | 0;

    SPIC1->CTRLR2 = (BIT13 | BIT12 | BIT0 | 4 << 4 | 3 << 8);
}
