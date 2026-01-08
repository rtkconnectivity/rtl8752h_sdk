
#include "guidef.h"
#include "gui_port.h"
#include "drv_touch.h"
#include "drv_lcd.h"
#include "gui_server.h"

static struct gui_touch_port_data raw_data = {0};

static gui_kb_port_data_t kb_port_data = {0};

static gui_wheel_port_data_t wheel_port_data = {0};

/***touch device***/

void port_button_set_indicate(void (*callback)(void))
{
    //todo
}


struct gui_touch_port_data *port_touchpad_get_data()
{
    return &raw_data;
}

gui_kb_port_data_t *port_kb_get_data(void)
{
    return &kb_port_data;
}

gui_wheel_port_data_t *port_wheel_get_data(void)
{
    return &wheel_port_data;
}

static struct gui_indev indev =
{
    .tp_get_data = port_touchpad_get_data,
    .kb_get_port_data = port_kb_get_data,
    .wheel_get_port_data = port_wheel_get_data,

    .touch_timeout_ms = 110,
    .long_button_time_ms = 800,
    .short_button_time_ms = 300,
    .quick_slide_time_ms = 50,
};

extern bool send_msg_to_gui_server(gui_msg_t *msg);
void gui_indev_wakeup(void)
{
    if (gui_server_dlps_check() == true)
    {
        gui_msg_t msg;
        msg.event = GUI_EVENT_DISPLAY_ON;
        gui_send_msg_to_server(&msg);
    }
}

extern void gui_indev_info_register(struct gui_indev *info);
extern void drv_touch_set_indicate(void (*indicate)(void *));
void gui_port_indev_init(void)
{
    indev.tp_height = drv_lcd_get_screen_height();
    indev.tp_witdh = drv_lcd_get_screen_width();
    gui_indev_info_register(&indev);
}

