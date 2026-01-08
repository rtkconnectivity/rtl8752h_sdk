
#include "guidef.h"
#include "gui_port.h"
#include "drv_lcd.h"
#include "gui_api.h"
#include "string.h"
#include "platform_utils.h"
#include "rtl_hal_peripheral.h"

#define LCD_SECTION_HEIGHT 8

static uint8_t __attribute__((aligned(4))) disp_write_buff1_port[256 * 8 * 2];
static uint8_t __attribute__((aligned(4))) disp_write_buff2_port[256 * 8 * 2];

void port_gui_lcd_power_on(void)
{

}
void port_gui_lcd_power_off(void)
{

}

#define rtk_cpu_to_le32(x)      ((uint32_t)(x))
#define rtk_le32_to_cpu(x)      ((uint32_t)(x))

#define HAL_READ32(base, addr)            \
    rtk_le32_to_cpu(*((volatile uint32_t *)(base + addr)))
#define HAL_WRITE32(base, addr, value32)  \
    ((*((volatile uint32_t *)(base + addr))) = rtk_cpu_to_le32(value32))

void port_gui_lcd_update(struct gui_dispdev *dc)
{
    uint32_t total_section_cnt = (drv_lcd_get_screen_height() / LCD_SECTION_HEIGHT + ((
                                      drv_lcd_get_screen_height() % LCD_SECTION_HEIGHT) ? 1 : 0));

    if (dc->section_count == 0)
    {
        drv_lcd_set_window(0, dc->fb_height * dc->section_count, dc->fb_width, dc->fb_height);
        drv_lcd_start_transfer(dc->frame_buf, dc->fb_width * dc->fb_height);
    }
    else if (dc->section_count == total_section_cnt - 1)
    {
        uint32_t last_height = dc->screen_height - dc->section_count * dc->fb_height;
        drv_lcd_transfer_done();
        drv_lcd_set_window(0, dc->fb_height * dc->section_count, dc->fb_width, last_height);
        drv_lcd_start_transfer(dc->frame_buf, dc->fb_width * last_height);
        drv_lcd_transfer_done();
    }
    else
    {
        drv_lcd_transfer_done();
        drv_lcd_set_window(0, dc->fb_height * dc->section_count, dc->fb_width, dc->fb_height);
        drv_lcd_start_transfer(dc->frame_buf, dc->fb_width * dc->fb_height);
    }
}

static struct gui_dispdev dc =
{
    .type = DC_RAMLESS,
    .section = {0, 0, 0, 0},
    .section_count = 0,

    .lcd_update = port_gui_lcd_update,
    .lcd_power_on = port_gui_lcd_power_on,
    .lcd_power_off = port_gui_lcd_power_off,

    .flash_seq_trans_disable = NULL,
    .flash_seq_trans_enable = NULL,
    .reset_lcd_timer = NULL,
    .get_lcd_us = NULL,

    .lcd_te_wait = NULL,
};

void gui_port_dc_init(void)
{
    dc.frame_buf = NULL;
    dc.fb_height = LCD_SECTION_HEIGHT;
    dc.fb_width = drv_lcd_get_screen_width();
    dc.disp_buf_1 = disp_write_buff1_port;
    dc.disp_buf_2 = disp_write_buff2_port;
    dc.bit_depth = drv_lcd_get_pixel_bits();

    dc.screen_width =  drv_lcd_get_screen_width();
    dc.screen_height = drv_lcd_get_screen_height();

    gui_dc_info_register(&dc);
}
