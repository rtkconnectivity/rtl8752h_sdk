
#ifndef __MENU_CONFIG_H__
#define __MENU_CONFIG_H__
// <<< Use Configuration Wizard in Context Menu >>>\n
/* Automatically generated file; DO NOT EDIT. */
/* Menu Configuration */

// <h> DEVICES
// <o> DEVICE_SELECT


// <0=> 8762D
// <1=> 8762G
// <2=> 8772F
// <3=> 8752H
#define DEVICE_VER 3
// </h>

#if DEVICE_VER == 0
#define CONFIG_REALTEK_8762D_DEVICE
#elif DEVICE_VER == 1
#define CONFIG_REALTEK_8762G_DEVICE
#elif DEVICE_VER ==2
#define CONFIG_REALTEK_8762F_DEVICE
#elif DEVICE_VER ==3
#define CONFIG_REALTEK_8752H_DEVICE
#endif



// <h> Hardware Drivers Config
// <q> RTK_GPIO_ENABLED - GPIO peripheral driver
//==========================================================
#define CONFIG_REALTEK_BSP_GPIO 1

// <q> RTK_DLPS_ENABLED - DLPS config
//==========================================================
#define CONFIG_REALTEK_BSP_DLPS 0

// <q> RTK_Flash_ENABLED - flash config
//==========================================================
#define CONFIG_REALTEK_BSP_FLASH 0

// <q> RTK_UART_ENABLED - UART peripheral driver
//==========================================================
#define CONFIG_REALTEK_BSP_UART 0

// <q> RTK_I2C_ENABLED - I2C peripheral driver
//==========================================================
#define CONFIG_REALTEK_BSP_I2C 0

// <q> RTK_SPI_ENABLED - SPI peripheral driver
//==========================================================
#define CONFIG_REALTEK_BSP_SPI 0

// <q> RTK_DMA_ENABLED - DMA peripheral driver
//==========================================================
#define CONFIG_REALTEK_BSP_DMA 0

// <q> RTK_RTC_ENABLED - RTC peripheral driver
//==========================================================
#define CONFIG_REALTEK_BSP_ONCHIP_RTC 0

// <q> RTK_ADC_ENABLED - ADC peripheral driver
//==========================================================
#define CONFIG_REALTEK_BSP_ADC 0



// <e> RTK_LCD_ENABLED - LCD peripheral driver
#define CONFIG_REALTEK_BSP_LCD 1
#if (CONFIG_REALTEK_BSP_LCD == 1)
// <o> SELECT_LCD_INTERFACE

// <0=> QSPI
// <1=> SPI
// <2=> 8080

#define CONFIG_REALTEK_LCD
#define CONFIG_REALTEK_LCD_CONFIG_INTERFACE 0

#if (CONFIG_REALTEK_LCD_CONFIG_INTERFACE == 0)
#define CONFIG_REALTEK_BSP_USING_QSPI
#elif (CONFIG_REALTEK_LCD_CONFIG_INTERFACE == 1)
#define CONFIG_REALTEK_BSP_USING_SPI
#define CONFIG_REALTEK_BSP_SPI
#elif (CONFIG_REALTEK_LCD_CONFIG_INTERFACE == 2)
#define CONFIG_REALTEK_BSP_USING_8080
#endif
// <o> SELECT_LCD_DRIVER

// <0=> ST77916

#define CONFIG_REALTEK_LCD_CONFIG_DRIVER 0

#if (CONFIG_REALTEK_LCD_CONFIG_DRIVER == 0)
#define CONFIG_REALTEK_ST77916_320_385

#endif
#endif
// </e>

// <e> RTK_TOUCH_ENABLED - TOUCH peripheral driver
#define CONFIG_REALTEK_BSP_TOUCH 0



// <o> SELECT_TOUCH_DRIVER

// <0=> BSP_TOUCH_GT911
// <1=> BSP_TOUCH_816S
// <2=> BSP_TOUCH_ZT2717
// <3=> BSP_TOUCH_816S_8762D_320
#if (CONFIG_REALTEK_BSP_TOUCH == 1)

#define CONFIG_REALTEK_TOUCH_CONFIG_DRIVER 3

#if (CONFIG_REALTEK_TOUCH_CONFIG_DRIVER == 0)
#define CONFIG_REALTEK_BSP_TOUCH_GT911
#elif (CONFIG_REALTEK_TOUCH_CONFIG_DRIVER == 1)
#define CONFIG_REALTEK_BSP_TOUCH_816S
#elif (CONFIG_REALTEK_TOUCH_CONFIG_DRIVER == 2)
#define CONFIG_REALTEK_BSP_TOUCH_ZT2717
#elif (CONFIG_REALTEK_TOUCH_CONFIG_DRIVER == 3)
#define CONFIG_REALTEK_BSP_TOUCH_816S_8762D_320
#endif
#endif
// </e>

// </h>


// <e> BLE
#define CONFIG_REALTEK_BLE_TASK 0
#if (CONFIG_REALTEK_BLE_TASK == 1)

// <e> BLE SDK Select
#define USING_BLE_SDK 0
#if (USING_BLE_SDK == 1)
// <o> BLE SDK

// <0=> TUYA SDK
// <1=> XIAOMI SDK

#define BLE_SDK_CONFIG 0
#if (BLE_SDK_CONFIG == 0)
#define TUYA_BLE_SDK
#ifdef TUYA_BLE_SDK
// <c> TUYA_MBEDTLS
//#define TUYA_MBEDTLS
// </c>
#endif
#endif
#endif
// </e>
// <e> BLE_SERVICE_ENABLED
#define USING_BLE_SERVICE 1
#endif
#if (USING_BLE_SERVICE == 1)
// <q> BAS_SERVICE
#define BAS_SERVICE 1
#endif
#if (USING_BLE_SERVICE == 1)
// <q> IAS_SERVICE
#define IAS_SERVICE 0
#endif
#if (USING_BLE_SERVICE == 1)
// <q> SIMPLE_BLE_SERVICE
#define SIMPLE_BLE_SERVICE 1
#endif
#if (USING_BLE_SERVICE == 1)
// <q> WRISTBAND_PRIVATE_SERVICE
#define RTK_BLE_WRISTBAND 0
#endif
#if (USING_BLE_SERVICE == 1)
// <q> OTA_SERVICE
#define CONFIG_RTK_OTA_SERVICE 1
// <q> DFU_SERVICE
#define CONFIG_REALTEK_DFU_SERVICE 1
#endif
#if (USING_BLE_SERVICE == 1)
// <q> OTA_SERVICE
#define CONFIG_REALTEK_OTA_SERVICE 1
#endif
// </e>
// </e>

// <e> BR
#if defined(USING_8772F_DEVICE)
#define RTK_BR_TASK  0
#if (RTK_BR_TASK == 1)
// <e> BR_PROFILE_ENABLED
#define USING_BR_PROFILE 1
#endif
#if (USING_BR_PROFILE == 1)
// <c> SDP
#define BR_PROFILE_SDP
// </c>
#endif
#if (USING_BR_PROFILE == 1)
// <c> SPP
#define BR_PROFILE_SPP
// </c>
#endif
#if (USING_BR_PROFILE == 1)
// <c> HFP
//#define BR_PROFILE_HFP
// </c>
#endif
#if (USING_BR_PROFILE == 1)
// <c> A2DP
#define BR_PROFILE_A2DP
// </c>
#endif
#if (USING_BR_PROFILE == 1)
// <c> AVRCP
#define BR_PROFILE_AVRCP
// </c>
#endif
#if (USING_BR_PROFILE == 1)
// <c> PBAP
#define BR_PROFILE_PBAP
// </c>
#endif
// </e>
#endif
// </e>





// <e> Console Enabled
#define USING_CONSOLE 0
#if (USING_CONSOLE == 1)
// <q> LETTER_SHELL  - Enable Letter Shell
#define USING_LETTER_SHELL 0
#endif

// </e>

// <c> Watch Clock Enabled
//#define CONFIG_REALTEK_MODULE_WATCH_CLOCK 1
// </c>


// <c> RTK DataBase
//#define CONFIG_REALTEK_MODULE_DATABASE
// </c>

// <h> HoneyGUI Framework Config


// <e> HoneyGUI Enable RTK Real GUI
#define CONFIG_REALTEK_BUILD_GUI     0
#define CONFIG_REALTEK_BUILD_GUI_LIB 1

#if (CONFIG_REALTEK_BUILD_GUI == 1 || CONFIG_REALTEK_BUILD_GUI_LIB == 1)

// <c> RTK GUI Demo
#define CONFIG_REALTEK_BUILD_GUI_240_240_DEMO
// </c>

// <c> RTK GUI Use OS Heap
#define CONFIG_REALTEK_BUILD_GUI_OS_HEAP
// </c>

// <c> RTK GUI Font Enable STB
//#define CONFIG_REALTEK_BUILD_GUI_FONT_STB
// </c>

// <c> RTK GUI Font Enable FREETYPE
//#define CONFIG_REALTEK_BUILD_GUI_FONT_FREETYPE
// </c>

// <c> RTK GUI Font Enable RTK MEM
#define CONFIG_REALTEK_BUILD_GUI_FONT_RTK_MEM
// </c>

// <c> RTK GUI Font Enable TTF SVG
//#define CONFIG_REALTEK_BUILD_GUI_FONT_TTF_SVG
// </c>

// <c> RTK GUI Enable VGLITE GPU
//#define CONFIG_REALTEK_BUILD_VG_LITE
// </c>

// <c> RTK GUI Enable PPE1.0
//#define CONFIG_REALTEK_BUILD_PPE
// </c>


// <c> RTK GUI Enable SasA
//#define CONFIG_REALTEK_BUILD_SCRIPT_AS_A_APP
// </c>

// <c> RTK GUI Enable cJSON
#define CONFIG_REALTEK_BUILD_CJSON
// </c>

#endif

// </e>

// <e> HoneyGUI Enable LVGL
#define CONFIG_REALTEK_MODULE_LVGL_GUI     0

#if (CONFIG_REALTEK_MODULE_LVGL_GUI == 1)

// <c> HoneyGUI Enable LVGL EXAMPLES
//#define CONFIG_REALTEK_PKG_LVGL_USING_EXAMPLES
// </c>

// <c> HoneyGUI Enable LVGL RLOTTIE
//#define BUILD_USING_LVGL_RLOTTIE
// </c>

#endif

// </e>


// </h>


// <<< end of configuration section >>>
#endif//__MENU_CONFIG_H__
