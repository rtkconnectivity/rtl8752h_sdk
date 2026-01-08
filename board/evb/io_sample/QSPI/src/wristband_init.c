/**
*********************22, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      main.c
   * @brief     Source file for wristband project, mainly used for initialize modules
   * @author    boris
   * @date      2022-06-01
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2022 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/


#include "stdlib.h"
#include "app_section.h"
#include "os_mem.h"
#include "os_sched.h"
#include "trace.h"
#include "rtl876x_wdg.h"
#include "system_rtl876x.h"
#include "platform_utils.h"
#if (FEATURE_OVERLAY == 1)
#include "overlay_mgr.h"
#endif
#include "ftl.h"
#include "dlps.h"
#include "rtl876x_io_dlps.h"
#include "rtl876x_pinmux.h"

#include "wristband_init.h"
#include "menu_config.h"






#ifdef ENABLE_RTK_GUI
#include "gui_app.h"
#include "gui_server.h"
#include "gui_components_init.h"

void gui_main(void)
{
    gui_components_init();
    DBG_DIRECT("finish gui_main");
}
#endif

void wristband_task_init(void)
{
#if (RTK_BT_TASK == 1) || (RTK_BLE_TASK == 1)
//    bt_task_init();
#endif
#ifdef ENABLE_RTK_GUI
    gui_main();
#endif
#ifdef MODULE_USING_PORT_FOR_LVGL
    lv_8762g_demo();
#endif
}

void wristband_system_clock_init(void)
{
    // uint32_t actual_mhz;
    // int32_t ret0, ret1, ret2, ret3;
    // ret0 = pm_cpu_freq_set(125, &actual_mhz);

    // ret1 = flash_nor_set_seq_trans_enable(FLASH_NOR_IDX_SPIC0, 1);
    // ret2 = fmc_flash_nor_clock_switch(FLASH_NOR_IDX_SPIC0, 160, &actual_mhz);
    // ret3 = flash_nor_try_high_speed_mode(0, FLASH_NOR_DTR_4_BIT_MODE);

    // APP_PRINT_INFO5("ret0 %d , ret1 %d , ret2 %d , ret3 %d actual_mhz %d",
    //                 ret0, ret1, ret2, ret3, actual_mhz);
    // pm_spic1_freq_set(160, &actual_mhz);
    // drv_dlps_exit_cbacks_register("psram", psram_winbond_opi_init);

    // pm_display_freq_set(CLK_PLL1_SRC, 100, 100);
    // DBG_DIRECT("done wristband_system_clock_init");
}
void wristband_components_init(void)
{
#if (RTK_HAL_GPIO == 1)
//    hw_pin_init();
#endif

#if (RTK_HAL_UART == 1)
    hw_uart_init();
#endif

#if (RTK_HAL_I2C == 1)
//    hw_i2c_init();
#endif

#if (RTK_HAL_ADC == 1)
    hw_adc_init();
#endif

#if (RTK_HAL_LCD == 1)
    extern void hw_lcd_init(void);
    hw_lcd_init();
#endif

#if (RTK_HAL_TOUCH == 1)
    // drv_touch_init();
#endif

#ifdef MODULE_VG_LITE
    hw_gpu_init();
#endif

#if (RTK_HAL_RTC == 1)
//    hw_rtc_init();
#endif
#if (WS_USING_LETTER_SHELL == 1)
    userShellInit();
#endif
#if (RTK_HAL_DLPS == 1)
//    pwr_mgr_init();
#endif
    DBG_DIRECT("finish wristband_components_init");
}



/*-----------------------------------------------------------*/
