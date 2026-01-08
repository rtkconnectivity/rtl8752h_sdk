/**
*********************22, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      main.c
   * @brief     Source file for wristband project, mainly used for initialize modules
   * @author    luke
   * @date      2024-07-01
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2023 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/

#include "trace.h"
#include "os_sched.h"
#include "flash_nor_device.h"


#ifdef  ENABLE_HONEYGUI
#include "gui_components_init.h"
#include "gui_server.h"

static void gui_main(void)
{
    gui_server_init();
    DBG_DIRECT("finish gui_main");
}
#endif

static void flash_init(void)
{
    flash_nor_set_seq_trans(FLASH_NOR_IDX_SPIC0, 1);
    flash_nor_try_high_speed_mode(FLASH_NOR_IDX_SPIC0, FLASH_NOR_4_BIT_MODE);
}

/* Non-Secure main. */
int main(void)
{
    DBG_DIRECT("Non-Secure World: main");
    flash_init();
    extern void hw_lcd_init(void);
    hw_lcd_init();
    gui_main();

    /* Start scheduler. */
    os_sched_start();
}
/*-----------------------------------------------------------*/
