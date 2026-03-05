/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*****************************************************************************************
* @file      main.c
* @brief     Source file for BLE peripheral project, mainly used for initialize modules
* @author    jane
* @date      2017-06-12
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
**************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <string.h>
#include <stdlib.h>
#include <gap.h>
#include <gap_adv.h>
#include <gap_bond_le.h>
#include <gap_msg.h>
#include <bas.h>
#include <simple_ble_service.h>

#if F_BT_ANCS_CLIENT_SUPPORT
#include <profile_client.h>
#include "ancs.h"
#endif

#include "rtl876x_gpio.h"
#include "rtl876x_nvic.h" 
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "rtl876x_i2c.h"

#include "trace.h"
#include "board.h"

#include "app.h"
#include "app_task.h"
#include "app_flags.h"

#include "io_spi.h"
#include "..\..\..\..\..\board\evb\io_sample\SPI\Interrupt\mdk\epaper.h"
#include "bmp80.h"

/** @defgroup  PERIPH_DEMO_MAIN Peripheral Main
    * @brief Main file to initialize hardware and BT stack and start task scheduling
    * @{
    */

/*============================================================================*
 *                              Constants
 *============================================================================*/
/** @brief  Default minimum advertising interval when device is discoverable (units of 625us, 160=100ms) */
#define DEFAULT_ADVERTISING_INTERVAL_MIN            320
/** @brief  Default maximum advertising interval */
#define DEFAULT_ADVERTISING_INTERVAL_MAX            320


/*============================================================================*
 *                              Variables
 *============================================================================*/

/** @brief  GAP - scan response data (max size = 31 bytes) */
static const uint8_t scan_rsp_data[] =
{
    0x03,                             /* length */
    GAP_ADTYPE_APPEARANCE,            /* type="Appearance" */
    LO_WORD(GAP_GATT_APPEARANCE_UNKNOWN),
    HI_WORD(GAP_GATT_APPEARANCE_UNKNOWN),
};

/** @brief  GAP - Advertisement data (max size = 31 bytes, best kept short to conserve power) */
static const uint8_t adv_data[] =
{
    /* Flags */
    0x02,             /* length */
    GAP_ADTYPE_FLAGS, /* type="Flags" */
    GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
    /* Service */
    0x03,             /* length */
    GAP_ADTYPE_16BIT_COMPLETE,
    LO_WORD(GATT_UUID_SIMPLE_PROFILE),
    HI_WORD(GATT_UUID_SIMPLE_PROFILE),
    /* Local name */
    0x0F,             /* length */
    GAP_ADTYPE_LOCAL_NAME_COMPLETE,
    'B', 'L', 'E', '_', 'P', 'E', 'R', 'I', 'P', 'H', 'E', 'R', 'A', 'L',
};

/*============================================================================*
 *                              Functions
 *============================================================================*/
/**
  * @brief  Initialize peripheral and gap bond manager related parameters
  * @return void
  */
void app_le_gap_init(void)
{
    /* Device name and device appearance */
    uint8_t  device_name[GAP_DEVICE_NAME_LEN] = "BLE_PERIPHERAL";
    uint16_t appearance = GAP_GATT_APPEARANCE_UNKNOWN;
    uint8_t  slave_init_mtu_req = false;


    /* Advertising parameters */
    uint8_t  adv_evt_type = GAP_ADTYPE_ADV_IND;
    uint8_t  adv_direct_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    uint8_t  adv_direct_addr[GAP_BD_ADDR_LEN] = {0};
    uint8_t  adv_chann_map = GAP_ADVCHAN_ALL;
    uint8_t  adv_filter_policy = GAP_ADV_FILTER_ANY;
    uint16_t adv_int_min = DEFAULT_ADVERTISING_INTERVAL_MIN;
    uint16_t adv_int_max = DEFAULT_ADVERTISING_INTERVAL_MAX;

    /* GAP Bond Manager parameters */
    uint8_t  auth_pair_mode = GAP_PAIRING_MODE_PAIRABLE;
    uint16_t auth_flags = GAP_AUTHEN_BIT_BONDING_FLAG;
    uint8_t  auth_io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
    uint8_t  auth_oob = false;
    uint8_t  auth_use_fix_passkey = false;
    uint32_t auth_fix_passkey = 0;
#if F_BT_ANCS_CLIENT_SUPPORT
    uint8_t  auth_sec_req_enable = true;
#else
    uint8_t  auth_sec_req_enable = false;
#endif
    uint16_t auth_sec_req_flags = GAP_AUTHEN_BIT_BONDING_FLAG;

    /* Set device name and device appearance */
    le_set_gap_param(GAP_PARAM_DEVICE_NAME, GAP_DEVICE_NAME_LEN, device_name);
    le_set_gap_param(GAP_PARAM_APPEARANCE, sizeof(appearance), &appearance);
    le_set_gap_param(GAP_PARAM_SLAVE_INIT_GATT_MTU_REQ, sizeof(slave_init_mtu_req),
                     &slave_init_mtu_req);

    /* Set advertising parameters */
    le_adv_set_param(GAP_PARAM_ADV_EVENT_TYPE, sizeof(adv_evt_type), &adv_evt_type);
    le_adv_set_param(GAP_PARAM_ADV_DIRECT_ADDR_TYPE, sizeof(adv_direct_type), &adv_direct_type);
    le_adv_set_param(GAP_PARAM_ADV_DIRECT_ADDR, sizeof(adv_direct_addr), adv_direct_addr);
    le_adv_set_param(GAP_PARAM_ADV_CHANNEL_MAP, sizeof(adv_chann_map), &adv_chann_map);
    le_adv_set_param(GAP_PARAM_ADV_FILTER_POLICY, sizeof(adv_filter_policy), &adv_filter_policy);
    le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MIN, sizeof(adv_int_min), &adv_int_min);
    le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MAX, sizeof(adv_int_max), &adv_int_max);
    le_adv_set_param(GAP_PARAM_ADV_DATA, sizeof(adv_data), (void *)adv_data);
    le_adv_set_param(GAP_PARAM_SCAN_RSP_DATA, sizeof(scan_rsp_data), (void *)scan_rsp_data);

    /* Setup the GAP Bond Manager */
    gap_set_param(GAP_PARAM_BOND_PAIRING_MODE, sizeof(auth_pair_mode), &auth_pair_mode);
    gap_set_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(auth_flags), &auth_flags);
    gap_set_param(GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(auth_io_cap), &auth_io_cap);
    gap_set_param(GAP_PARAM_BOND_OOB_ENABLED, sizeof(auth_oob), &auth_oob);
    le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY, sizeof(auth_fix_passkey), &auth_fix_passkey);
    le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY_ENABLE, sizeof(auth_use_fix_passkey),
                      &auth_use_fix_passkey);
    le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_ENABLE, sizeof(auth_sec_req_enable), &auth_sec_req_enable);
    le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_REQUIREMENT, sizeof(auth_sec_req_flags),
                      &auth_sec_req_flags);

    /* register gap message callback */
    le_register_app_cb(app_gap_callback);
}

/**
 * @brief  Add GATT services and register callbacks
 * @return void
 */
void app_le_profile_init(void)
{
    server_init(2);
    simp_srv_id = simp_ble_service_add_service(app_profile_callback);
    bas_srv_id  = bas_add_service(app_profile_callback);
    server_register_app_cb(app_profile_callback);
#if F_BT_ANCS_CLIENT_SUPPORT
    client_init(1);
    ancs_init(APP_MAX_LINKS);
#endif
}

void board_gpio_init(void)
{
		Pad_Config(GPIO_DC_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(GPIO_RST_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(GPIO_BUSY_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_HIGH);
		Pad_Config(GPIO_PWR_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_HIGH);
		Pad_Config(GPIO_BTN_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);

		Pinmux_Config(GPIO_DC_PIN, DWGPIO);
		Pinmux_Config(GPIO_RST_PIN, DWGPIO);
		Pinmux_Config(GPIO_BUSY_PIN, DWGPIO);
		Pinmux_Config(GPIO_PWR_PIN, DWGPIO);
		Pinmux_Config(GPIO_BTN_PIN, DWGPIO);
}

void board_i2c_master_init(void)
{
    Pad_Config(I2C_MASTER_SCL_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
    Pad_Config(I2C_MASTER_SDA_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_LOW);

    Pinmux_Config(I2C_MASTER_SCL_PIN, I2C0_CLK);
    Pinmux_Config(I2C_MASTER_SDA_PIN, I2C0_DAT);
}

void driver_gpio_init(void)
{
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin    = DC_PIN;
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
    GPIO_Init(&GPIO_InitStruct);
		
		GPIO_InitStruct.GPIO_Pin    = RST_PIN; 
		GPIO_Init(&GPIO_InitStruct);
		
		GPIO_InitStruct.GPIO_Pin    = PWR_PIN; 
		GPIO_Init(&GPIO_InitStruct);
		
		GPIO_InitStruct.GPIO_Pin    = BUSY_PIN;
		GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_IN;
		GPIO_Init(&GPIO_InitStruct);
	
		GPIO_InitStruct.GPIO_Pin    = BTN_PIN;
		GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_ITCmd  = ENABLE;
	  GPIO_InitStruct.GPIO_ITTrigger  = GPIO_INT_Trigger_EDGE;
    GPIO_InitStruct.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
    GPIO_InitStruct.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_ENABLE;
    GPIO_InitStruct.GPIO_DebounceTime = 10;/* unit:ms , can be 1~64 ms */
		GPIO_Init(&GPIO_InitStruct);
		
    GPIO_MaskINTConfig(BTN_PIN, ENABLE);
    GPIO_INTConfig(BTN_PIN, ENABLE);
    GPIO_ClearINTPendingBit(BTN_PIN);
    GPIO_MaskINTConfig(BTN_PIN, DISABLE);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = BTN_PIN_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
	
}

void driver_i2c_master_init(void)
{
    /* Initialize I2C peripheral */
    RCC_PeriphClockCmd(APBPeriph_I2C0, APBPeriph_I2C0_CLOCK, ENABLE);

    I2C_InitTypeDef  I2C_InitStruct;
    I2C_StructInit(&I2C_InitStruct);

    I2C_InitStruct.I2C_ClockSpeed       = 10000;
    I2C_InitStruct.I2C_DeviveMode       = I2C_DeviveMode_Master;
    I2C_InitStruct.I2C_AddressMode      = I2C_AddressMode_7BIT;
    I2C_InitStruct.I2C_SlaveAddress     = BMP180_ADDR;
    I2C_InitStruct.I2C_RxThresholdLevel = 0;
    I2C_InitStruct.I2C_Ack              = I2C_Ack_Enable;

    I2C_Init(I2C0, &I2C_InitStruct);
    I2C_Cmd(I2C0, ENABLE);
}

void GPIO_Input_Handler(void)
{
    GPIO_INTConfig(BTN_PIN, DISABLE);
    GPIO_MaskINTConfig(BTN_PIN, ENABLE);

    T_IO_MSG int_gpio_msg;

    int_gpio_msg.type = IO_MSG_TYPE_GPIO;
    int_gpio_msg.subtype = 0;
    if (false == app_send_msg_to_apptask(&int_gpio_msg))
    {
        APP_PRINT_ERROR0("[io_gpio] GPIO_Input_Handler: Send int_gpio_msg failed!");
        //Add user code here!
        GPIO_ClearINTPendingBit(BTN_PIN);
        return;
    }

    GPIO_ClearINTPendingBit(BTN_PIN);
    GPIO_MaskINTConfig(BTN_PIN, DISABLE);
    GPIO_INTConfig(BTN_PIN, ENABLE);
}


/**
  * @brief  Initialize global data.
  * @param  No parameter.
  * @return void
  */
void global_data_init(void)
{
}

/**
  * @brief    Contains the initialization of pinmux settings and pad settings
  * @note     All the pinmux settings and pad settings shall be initiated in this function,
  *           but if legacy driver is used, the initialization of pinmux setting and pad setting
  *           should be peformed with the IO initializing.
  * @return   void
  */
void board_init(void)
{
		board_gpio_init();
    board_spi_init();
		board_i2c_master_init();
}

/**
 * @brief    Contains the initialization of peripherals
 * @note     Both new architecture driver and legacy driver initialization method can be used
 * @return   void
 */
void driver_init(void)
{
		driver_gpio_init();
    driver_spi_init();
		driver_i2c_master_init();
}

/**
 * @brief    Contains the power mode settings
 * @return   void
 */
void pwr_mgr_init(void)
{
}

/**
 * @brief    Contains the initialization of all tasks
 * @note     There is only one task in BLE Peripheral APP, thus only one APP task is init here
 * @return   void
 */
void task_init(void)
{
    app_task_init();
}

void *demo_task_handle;

void epaper_demo(void *p_param)
{
		epaper_test();
}

/**
 * @brief    Entry of APP code
 * @return   int (To avoid compile warning)
 */
int main(void)
{
    extern uint32_t random_seed_value;
    srand(random_seed_value);
    global_data_init();
    board_init();
		driver_init();
    le_gap_init(APP_MAX_LINKS);
    gap_lib_init();
    app_le_gap_init();
    app_le_profile_init();
    pwr_mgr_init();
    task_init();
		os_task_create(&demo_task_handle, "epaper_demo", epaper_demo, 0, 256*10, 3);
    os_sched_start();

    return 0;
}
/** @} */ /* End of group PERIPH_DEMO_MAIN */


