#include "board.h"
#include "app_section.h"
#include "rtl876x_rcc.h"
#include "rtl876x_tim.h"
#include "rtl876x_gpio.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_gdma.h"
#include "rtl876x_nvic.h"
#include "lcd_st77916g5_320_qspi.h"
#include "platform_utils.h"
#include "qspi_lcd_platform.h"



#define BIT_CMD_CH(x)           (((x) & 0x00000003) << 20)
#define BIT_DATA_CH(x)          (((x) & 0x00000003) << 18)
#define BIT_ADDR_CH(x)          (((x) & 0x00000003) << 16)
#define BIT_TMOD(x)             (((x) & 0x00000003) << 8)

#define BIT_TXSIM               (0x00000001 << 9)
#define BIT_SEQ_EN              (0x00000001 << 3)

DATA_RAM_FUNCTION
static void spic1_spi_write(uint8_t *buf, uint32_t len)
{
    SPIC1->SER = BIT0;
    SPIC1->SSIENR = 0x00;//disable SPIC1

    SPIC1->CTRLR0 &= ~(BIT_CMD_CH(3) | BIT_ADDR_CH(3) | BIT_DATA_CH(3));//SET CHANNEL NUM
    SPIC1->CTRLR0 &= ~(BIT_TMOD(3)); //tx mode

    SPIC1->TX_NDF = len - 4;

    for (uint32_t i = 0; i < len; i++)
    {
        SPIC1->DR[0].BYTE = buf[i];
    }
    SPIC1->SSIENR = 0x01;//enable SPIC1

    while (SPIC1->SR & BIT0); // wait bus busy
}

DATA_RAM_FUNCTION
static void rtl_lcd_qspi_write_cmd(uint16_t cmd) //total 4 byte, first byte is 0x02
{
    uint8_t sdat[] = {0x02, 0x00, cmd, 0x00};
    spic1_spi_write(sdat, sizeof(sdat));
}


DATA_RAM_FUNCTION
static void rtl_lcd_qspi_cmd_param(uint8_t cmd, uint8_t data) //total 5 byte, first byte is 0x02
{
    uint8_t sdat[] = {0x02, 0x00, cmd, 0x00, data};
    spic1_spi_write(sdat, sizeof(sdat));
}
DATA_RAM_FUNCTION
static void rtl_lcd_qspi_cmd_param2(uint8_t cmd, uint8_t *data) //total 6 byte, first byte is 0x02
{
    uint8_t sdat[] = {0x02, 0x00, cmd, 0x00, data[0], data[1]};
    spic1_spi_write(sdat, sizeof(sdat));
}

DATA_RAM_FUNCTION
static void rtl_lcd_qspi_cmd_param4(uint8_t cmd, uint8_t *data) //total 8 byte, first byte is 0x02
{
    uint8_t sdat[] = {0x02, 0x00, cmd, 0x00, data[0], data[1], data[2], data[3]};
    spic1_spi_write(sdat, sizeof(sdat));
}

DATA_RAM_FUNCTION
static void rtl_lcd_qspi_cmd_paramn(uint8_t cmd, uint8_t *data,
                                    uint8_t len) //total n byte, first byte is 0x02
{
    uint8_t sdat[32] = {0x02, 0x00, cmd, 0x00};
    for (uint8_t i = 0; i < len; i++)
    {
        sdat[i + 4] = data[i];
    }
    spic1_spi_write(sdat, (len + 4));
}

DATA_RAM_FUNCTION
static void rtl_lcd_qspi_enter_data_output_mode(uint32_t
                                                len_byte) //total 4 byte, first byte is 0x32
{
    SPIC1->SER = BIT0;//select CS0
    SPIC1->SSIENR = 0x00;//disable SPIC1

    SPIC1->TX_NDF = len_byte;

    SPIC1->CTRLR0 &= ~(BIT_CMD_CH(3) | BIT_ADDR_CH(3) | BIT_DATA_CH(3));//SET CHANNEL NUM
    SPIC1->CTRLR0 |= (BIT_CMD_CH(0) | BIT_ADDR_CH(0) | BIT_DATA_CH(2));
    SPIC1->CTRLR0 &= ~(BIT_TMOD(3)); //set tx mode

    SPIC1->IMR |= BIT_TXSIM;
    SPIC1->CTRLR2 |= BIT_SEQ_EN;

    /* must push cmd and address before SPIC enable */
    uint32_t first_word = 0x32 | __REV(0x002c00);
    SPIC1->DR[0].WORD = first_word;

    SPIC1->DMACR = 2;

    /* change this value can not influence the result. the wave is split into two periods. the first is 32 bytes. */
    SPIC1->DMATDLR = 4; /* no any influence. */

    // SPIC1->SSIENR = 0x01;//enable SPIC1
    /*then , we can push data to FIFO*/

}

void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h)
{
    uint8_t data[4];
    uint16_t xEnd = xStart + w - 1;
    uint16_t yEnd = yStart + h - 1;
    xStart = xStart + 0;
    xEnd = xEnd + 0;
    yStart = yStart + 0;
    yEnd = yEnd + 0;

    data[0] = xStart >> 8;
    data[1] = xStart & 0xff;
    data[2] = xEnd >> 8;
    data[3] = xEnd & 0xff;
    rtl_lcd_qspi_cmd_param4(0x2A, data);


    data[0] = yStart >> 8;
    data[1] = yStart & 0xff;
    data[2] = yEnd >> 8;
    data[3] = yEnd & 0xff;
    rtl_lcd_qspi_cmd_param4(0x2B, data);

    uint32_t len_byte = (xEnd - xStart + 1) * (yEnd - yStart + 1) * OUTPUT_PIXEL_BYTES;
    rtl_lcd_qspi_enter_data_output_mode(len_byte);
}

void rtk_lcd_hal_update_framebuffer(uint8_t *buf, uint32_t len)
{
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = LCD_DMA_CHANNEL_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStruct);
    /* Initialize GDMA peripheral */
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum          = LCD_DMA_CHANNEL_NUM;

    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_8;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_8;

    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_SPIC1_TX;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;

    GDMA_Init(LCD_DMA_CHANNEL_INDEX, &GDMA_InitStruct);
    GDMA_INTConfig(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);

    GDMA_SetBufferSize(LCD_DMA_CHANNEL_INDEX, len >> 2);

    GDMA_SetDestinationAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)(&(SPIC1->DR[0].WORD)));
    GDMA_SetSourceAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)buf);
    GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);
    SPIC1->SSIENR = 0x01;//enable SPIC1
}

void rtk_lcd_hal_rect_fill(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h,
                           uint32_t color)
{
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = LCD_DMA_CHANNEL_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStruct);
    /* Initialize GDMA peripheral */
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum          = LCD_DMA_CHANNEL_NUM;

    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_8;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_8;

    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_SPIC1_TX;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Fix;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;

    GDMA_Init(LCD_DMA_CHANNEL_INDEX, &GDMA_InitStruct);
    GDMA_INTConfig(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);


    rtk_lcd_hal_set_window(xStart, yStart, w, h);

    static uint32_t color_buf = 0;
    color_buf = (color >> 8) | (color << 8);
    color_buf = color_buf | color_buf << 16;

    GDMA_SetSourceAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)&color_buf);
    GDMA_SetDestinationAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)(&(SPIC1->DR[0].WORD)));


    uint32_t section_hight = 10;
    uint32_t left_line = h % section_hight;

    for (uint32_t i = 0; i < h / section_hight; i++)
    {
        GDMA_SetBufferSize(LCD_DMA_CHANNEL_INDEX, (w * section_hight * 2) >> 2);
        GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);
        rtk_lcd_hal_transfer_done();
    }
    if (left_line != 0)
    {
        GDMA_SetBufferSize(LCD_DMA_CHANNEL_INDEX, (w * left_line * 2) >> 2);
        GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);
        rtk_lcd_hal_transfer_done();
    }

}

void rtk_lcd_hal_start_transfer(uint8_t *buf, uint32_t len)
{
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = LCD_DMA_CHANNEL_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStruct);
    /* Initialize GDMA peripheral */
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum          = LCD_DMA_CHANNEL_NUM;

    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_8;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_8;

    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_SPIC1_TX;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;

    GDMA_Init(LCD_DMA_CHANNEL_INDEX, &GDMA_InitStruct);
    GDMA_INTConfig(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);

    GDMA_SetBufferSize(LCD_DMA_CHANNEL_INDEX, len >> 1);

    GDMA_SetDestinationAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)(&(SPIC1->DR[0].WORD)));
    GDMA_SetSourceAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)buf);
    GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);
    SPIC1->SSIENR = 0x01;//enable SPIC1
}

void rtk_lcd_hal_transfer_done(void)
{
    while (GDMA_GetTransferINTStatus(LCD_DMA_CHANNEL_NUM) != SET);
    GDMA_ClearINTPendingBit(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer);
}

uint32_t rtk_lcd_hal_get_width(void)
{
    return ST77916_LCD_WIDTH;
}
uint32_t rtk_lcd_hal_get_height(void)
{
    return ST77916_LCD_HEIGHT;
}
uint32_t rtk_lcd_hal_get_pixel_bits(void)
{
    return ST77916_DRV_PIXEL_BITS;
}


static void lcd_pad_init(void)
{

    /*BL AND RESET ARE NOT FIX*/
    /*BL AND RESET ARE NOT FIX*/

    Pad_Config(QSPI_LCD_POWER, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);

}

static void lcd_set_reset(bool reset)
{
    if (reset)
    {
        Pad_Config(SPIC1_LCD_RESX_OUTPUT_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE,
                   PAD_OUT_LOW);
    }
    else
    {
        Pad_Config(SPIC1_LCD_RESX_OUTPUT_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE,
                   PAD_OUT_HIGH);
    }
}

static void lcd_seq_init(void)
{
    rtl_lcd_qspi_cmd_param(0xF0, 0x28);

    rtl_lcd_qspi_cmd_param(0xF2, 0x28);

    rtl_lcd_qspi_cmd_param(0x7C, 0xD1);

    rtl_lcd_qspi_cmd_param(0x80, 0x10);

    rtl_lcd_qspi_cmd_param(0x83, 0xE0);

    rtl_lcd_qspi_cmd_param(0x84, 0x61);

    rtl_lcd_qspi_cmd_param(0xF2, 0x82);

    rtl_lcd_qspi_cmd_param(0xF0, 0x00);

    rtl_lcd_qspi_cmd_param(0xF0, 0x01);

    rtl_lcd_qspi_cmd_param(0xF1, 0x01);

    rtl_lcd_qspi_cmd_param(0xB0, 0x52);

    rtl_lcd_qspi_cmd_param(0xB1, 0x49);

    rtl_lcd_qspi_cmd_param(0xB2, 0x24);

    rtl_lcd_qspi_cmd_param(0xB4, 0x69);

    rtl_lcd_qspi_cmd_param(0xB5, 0x45);

    rtl_lcd_qspi_cmd_param(0xB6, 0x8B);

    rtl_lcd_qspi_cmd_param(0xB7, 0x41);

    rtl_lcd_qspi_cmd_param(0xB8, 0x05);

    rtl_lcd_qspi_cmd_param(0xBA, 0x00);

    rtl_lcd_qspi_cmd_param(0xBB, 0x08);

    rtl_lcd_qspi_cmd_param(0xBC, 0x08);

    rtl_lcd_qspi_cmd_param(0xBD, 0x00);

    rtl_lcd_qspi_cmd_param(0xC0, 0x80);

    rtl_lcd_qspi_cmd_param(0xC1, 0x08);

    rtl_lcd_qspi_cmd_param(0xC2, 0x35);

    rtl_lcd_qspi_cmd_param(0xC3, 0x80);

    rtl_lcd_qspi_cmd_param(0xC4, 0x08);

    rtl_lcd_qspi_cmd_param(0xC5, 0x35);

    rtl_lcd_qspi_cmd_param(0xC6, 0xA9);

    rtl_lcd_qspi_cmd_param(0xC7, 0x41);

    rtl_lcd_qspi_cmd_param(0xC8, 0x01);

    rtl_lcd_qspi_cmd_param(0xC9, 0xA9);

    rtl_lcd_qspi_cmd_param(0xCA, 0x41);

    rtl_lcd_qspi_cmd_param(0xCB, 0x01);

    rtl_lcd_qspi_cmd_param(0xD0, 0xD1);

    rtl_lcd_qspi_cmd_param(0xD1, 0x40);

    rtl_lcd_qspi_cmd_param(0xD2, 0x81);

    uint8_t data_F5h[2] = {0x00, 0xA5};
    rtl_lcd_qspi_cmd_param2(0xF5, data_F5h);

    rtl_lcd_qspi_cmd_param(0xDD, 0x50);

    rtl_lcd_qspi_cmd_param(0xDE, 0x50);

    rtl_lcd_qspi_cmd_param(0xF1, 0x10);

    rtl_lcd_qspi_cmd_param(0xF0, 0x00);

    rtl_lcd_qspi_cmd_param(0xF0, 0x02);

    uint8_t data_E0h[14] = {0xf0, 0x0f, 0x10, 0x0a, 0x0a, 0x06, 0x38, 0x33, 0x4d, 0x06, 0x12, 0x11, 0x2f, 0x34};
    uint8_t data_E1h[14] = {0xf0, 0x10, 0x15, 0x0B, 0x0C, 0x07, 0x3d, 0x44, 0x53, 0x0c, 0x18, 0x18, 0x30, 0x34};
    rtl_lcd_qspi_cmd_paramn(0xE0, data_E0h, sizeof(data_E0h));
    rtl_lcd_qspi_cmd_paramn(0xE1, data_E1h, sizeof(data_E1h));

    rtl_lcd_qspi_cmd_param(0xF0, 0x10);

    rtl_lcd_qspi_cmd_param(0xF3, 0x10);

    rtl_lcd_qspi_cmd_param(0xE0, 0x08);

    rtl_lcd_qspi_cmd_param(0xE1, 0x00);

    rtl_lcd_qspi_cmd_param(0xE2, 0x0B);

    rtl_lcd_qspi_cmd_param(0xE3, 0x00);

    rtl_lcd_qspi_cmd_param(0xE4, 0xE0);

    rtl_lcd_qspi_cmd_param(0xE5, 0x06);

    rtl_lcd_qspi_cmd_param(0xE6, 0x21);

    rtl_lcd_qspi_cmd_param(0xE7, 0x10);

    rtl_lcd_qspi_cmd_param(0xE8, 0x8A);

    rtl_lcd_qspi_cmd_param(0xE9, 0x82);

    rtl_lcd_qspi_cmd_param(0xEA, 0xE4);

    rtl_lcd_qspi_cmd_param(0xEB, 0x00);

    rtl_lcd_qspi_cmd_param(0xEC, 0x00);

    rtl_lcd_qspi_cmd_param(0xED, 0x14);

    rtl_lcd_qspi_cmd_param(0xEE, 0xFF);

    rtl_lcd_qspi_cmd_param(0xEF, 0x00);

    rtl_lcd_qspi_cmd_param(0xF8, 0xFF);

    rtl_lcd_qspi_cmd_param(0xF9, 0x00);

    rtl_lcd_qspi_cmd_param(0xFA, 0x00);

    rtl_lcd_qspi_cmd_param(0xFB, 0x30);

    rtl_lcd_qspi_cmd_param(0xFC, 0x00);

    rtl_lcd_qspi_cmd_param(0xFD, 0x00);

    rtl_lcd_qspi_cmd_param(0xFE, 0x00);

    rtl_lcd_qspi_cmd_param(0xFF, 0x00);

    rtl_lcd_qspi_cmd_param(0x60, 0x50);

    rtl_lcd_qspi_cmd_param(0x61, 0x02);

    rtl_lcd_qspi_cmd_param(0x62, 0x0B);

    rtl_lcd_qspi_cmd_param(0x63, 0x50);

    rtl_lcd_qspi_cmd_param(0x64, 0x04);

    rtl_lcd_qspi_cmd_param(0x65, 0x0B);

    rtl_lcd_qspi_cmd_param(0x66, 0x53);

    rtl_lcd_qspi_cmd_param(0x67, 0x08);

    rtl_lcd_qspi_cmd_param(0x68, 0x0B);

    rtl_lcd_qspi_cmd_param(0x69, 0x53);

    rtl_lcd_qspi_cmd_param(0x6A, 0x0A);

    rtl_lcd_qspi_cmd_param(0x6B, 0x0B);

    rtl_lcd_qspi_cmd_param(0x70, 0x50);

    rtl_lcd_qspi_cmd_param(0x71, 0x01);

    rtl_lcd_qspi_cmd_param(0x72, 0x0B);

    rtl_lcd_qspi_cmd_param(0x73, 0x50);

    rtl_lcd_qspi_cmd_param(0x74, 0x03);

    rtl_lcd_qspi_cmd_param(0x75, 0x0B);

    rtl_lcd_qspi_cmd_param(0x76, 0x53);

    rtl_lcd_qspi_cmd_param(0x77, 0x07);

    rtl_lcd_qspi_cmd_param(0x78, 0x0B);

    rtl_lcd_qspi_cmd_param(0x79, 0x53);

    rtl_lcd_qspi_cmd_param(0x7A, 0x09);

    rtl_lcd_qspi_cmd_param(0x7B, 0x0B);

    rtl_lcd_qspi_cmd_param(0x80, 0x58);

    rtl_lcd_qspi_cmd_param(0x81, 0x00);

    rtl_lcd_qspi_cmd_param(0x82, 0x04);

    rtl_lcd_qspi_cmd_param(0x83, 0x03);

    rtl_lcd_qspi_cmd_param(0x84, 0x0C);

    rtl_lcd_qspi_cmd_param(0x85, 0x00);

    rtl_lcd_qspi_cmd_param(0x86, 0x00);

    rtl_lcd_qspi_cmd_param(0x87, 0x00);

    rtl_lcd_qspi_cmd_param(0x88, 0x58);

    rtl_lcd_qspi_cmd_param(0x89, 0x00);

    rtl_lcd_qspi_cmd_param(0x8A, 0x06);

    rtl_lcd_qspi_cmd_param(0x8B, 0x03);

    rtl_lcd_qspi_cmd_param(0x8C, 0x0E);

    rtl_lcd_qspi_cmd_param(0x8D, 0x00);

    rtl_lcd_qspi_cmd_param(0x8E, 0x00);

    rtl_lcd_qspi_cmd_param(0x8F, 0x00);

    rtl_lcd_qspi_cmd_param(0x90, 0x58);

    rtl_lcd_qspi_cmd_param(0x91, 0x00);

    rtl_lcd_qspi_cmd_param(0x92, 0x08);

    rtl_lcd_qspi_cmd_param(0x93, 0x03);

    rtl_lcd_qspi_cmd_param(0x94, 0x10);

    rtl_lcd_qspi_cmd_param(0x95, 0x00);

    rtl_lcd_qspi_cmd_param(0x96, 0x00);

    rtl_lcd_qspi_cmd_param(0x97, 0x00);

    rtl_lcd_qspi_cmd_param(0x98, 0x58);

    rtl_lcd_qspi_cmd_param(0x99, 0x00);

    rtl_lcd_qspi_cmd_param(0x9A, 0x0A);

    rtl_lcd_qspi_cmd_param(0x9B, 0x03);

    rtl_lcd_qspi_cmd_param(0x9C, 0x12);

    rtl_lcd_qspi_cmd_param(0x9D, 0x00);

    rtl_lcd_qspi_cmd_param(0x9E, 0x00);

    rtl_lcd_qspi_cmd_param(0x9F, 0x00);

    rtl_lcd_qspi_cmd_param(0xA0, 0x58);

    rtl_lcd_qspi_cmd_param(0xA1, 0x00);

    rtl_lcd_qspi_cmd_param(0xA2, 0x03);

    rtl_lcd_qspi_cmd_param(0xA3, 0x03);

    rtl_lcd_qspi_cmd_param(0xA4, 0x0B);

    rtl_lcd_qspi_cmd_param(0xA5, 0x00);

    rtl_lcd_qspi_cmd_param(0xA6, 0x00);

    rtl_lcd_qspi_cmd_param(0xA7, 0x00);

    rtl_lcd_qspi_cmd_param(0xA8, 0x58);

    rtl_lcd_qspi_cmd_param(0xA9, 0x00);

    rtl_lcd_qspi_cmd_param(0xAA, 0x05);

    rtl_lcd_qspi_cmd_param(0xAB, 0x03);

    rtl_lcd_qspi_cmd_param(0xAC, 0x0D);

    rtl_lcd_qspi_cmd_param(0xAD, 0x00);

    rtl_lcd_qspi_cmd_param(0xAE, 0x00);

    rtl_lcd_qspi_cmd_param(0xAF, 0x00);

    rtl_lcd_qspi_cmd_param(0xB0, 0x58);

    rtl_lcd_qspi_cmd_param(0xB1, 0x00);

    rtl_lcd_qspi_cmd_param(0xB2, 0x07);

    rtl_lcd_qspi_cmd_param(0xB3, 0x03);

    rtl_lcd_qspi_cmd_param(0xB4, 0x0F);

    rtl_lcd_qspi_cmd_param(0xB5, 0x00);

    rtl_lcd_qspi_cmd_param(0xB6, 0x00);

    rtl_lcd_qspi_cmd_param(0xB7, 0x00);

    rtl_lcd_qspi_cmd_param(0xB8, 0x58);

    rtl_lcd_qspi_cmd_param(0xB9, 0x00);

    rtl_lcd_qspi_cmd_param(0xBA, 0x09);

    rtl_lcd_qspi_cmd_param(0xBB, 0x03);

    rtl_lcd_qspi_cmd_param(0xBC, 0x11);

    rtl_lcd_qspi_cmd_param(0xBD, 0x00);

    rtl_lcd_qspi_cmd_param(0xBE, 0x00);

    rtl_lcd_qspi_cmd_param(0xBF, 0x00);

    rtl_lcd_qspi_cmd_param(0xC0, 0x77);

    rtl_lcd_qspi_cmd_param(0xC1, 0x46);

    rtl_lcd_qspi_cmd_param(0xC2, 0x55);

    rtl_lcd_qspi_cmd_param(0xC3, 0x64);

    rtl_lcd_qspi_cmd_param(0xC4, 0xBB);

    rtl_lcd_qspi_cmd_param(0xC5, 0x21);

    rtl_lcd_qspi_cmd_param(0xC6, 0x30);

    rtl_lcd_qspi_cmd_param(0xC7, 0xAA);

    rtl_lcd_qspi_cmd_param(0xC8, 0x12);

    rtl_lcd_qspi_cmd_param(0xC9, 0x03);

    rtl_lcd_qspi_cmd_param(0xD0, 0x77);

    rtl_lcd_qspi_cmd_param(0xD1, 0x46);

    rtl_lcd_qspi_cmd_param(0xD2, 0x55);

    rtl_lcd_qspi_cmd_param(0xD3, 0x64);

    rtl_lcd_qspi_cmd_param(0xD4, 0xBB);

    rtl_lcd_qspi_cmd_param(0xD5, 0x21);

    rtl_lcd_qspi_cmd_param(0xD6, 0x30);

    rtl_lcd_qspi_cmd_param(0xD7, 0xAA);

    rtl_lcd_qspi_cmd_param(0xD8, 0x12);

    rtl_lcd_qspi_cmd_param(0xD9, 0x03);

    rtl_lcd_qspi_cmd_param(0xF3, 0x01);

    rtl_lcd_qspi_cmd_param(0xF0, 0x00);

    rtl_lcd_qspi_write_cmd(0x21);
#if TE_VALID
    rtl_lcd_qspi_cmd_param(0x35, 0x00);
#endif
    rtl_lcd_qspi_cmd_param(0x3A, 0x55);

    rtl_lcd_qspi_write_cmd(0x11);
    platform_delay_ms(120);

    rtl_lcd_qspi_write_cmd(0x29);
    platform_delay_ms(20);
}


uint32_t rtk_lcd_hal_power_on(void)
{
    lcd_pad_init();
    qspi_lcd_platform_init();

    rtl_lcd_qspi_write_cmd(0x11);   /*sleep out*/
    platform_delay_ms(120);
    rtl_lcd_qspi_write_cmd(0x29);   /*power on*/
    platform_delay_ms(20);
    return 0;
}

uint32_t rtk_lcd_hal_power_off(void)
{
    rtl_lcd_qspi_write_cmd(0x28);   /*sleep in*/
    platform_delay_ms(120);

    rtl_lcd_qspi_write_cmd(0x10);   /*power off*/
    platform_delay_ms(20);
    return 0;
}

void rtk_lcd_hal_init(void)
{
    lcd_pad_init();
    qspi_lcd_platform_init();

    lcd_set_reset(true);
    platform_delay_ms(120);
    lcd_set_reset(false);
    platform_delay_ms(50);
    lcd_set_reset(false);
    platform_delay_ms(50);

    lcd_seq_init();
    rtk_lcd_hal_rect_fill(0, 0, 320, 385, 0x00000000);
}






