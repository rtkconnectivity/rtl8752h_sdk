/*
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */
/*============================================================================*
  *                     Define to prevent recursive inclusion
  *============================================================================*/

#ifndef _APP_MSG_H_
#define _APP_MSG_H_

/*============================================================================*
  *                               Header Files
  *============================================================================*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup APP_MSG APP Message
  * @brief Message definition for user application task.
  * @{
  */

/*============================================================================*
  *                                   Types
  *============================================================================*/
/** @defgroup APP_MSG_Exported_Types APP Message Exported Types
  * @{
    */

/**  @brief Event type definitions.*/
typedef enum
{
    EVENT_GAP_MSG = 0x01,  /**< Message from gap layer for stack. */
    EVENT_IO_TO_APP = 0x02, /**< Message from IO to user application. */
} T_EVENT_TYPE;
/** @brief Get event code group definition. */
#define EVENT_GROUP(event_code) (event_code >> 4)

/**
 * @brief     Event group definitions.
 * @details   Event code is 1 byte code to define messages type exchanging from/to different layers.
 *            First half byte is for group define, and second half byte is for code define.
 *            Group code:
 *            0: from stack layer
 *            0x1: from peripheral layer
 *            0x2: from audio lib
 *            0xA: from instrument
 */
typedef enum
{
    EVENT_GROUP_STACK = 0x00,       /**< Message group from BT layer */
    EVENT_GROUP_IO = 0x01,          /**< Message group from IO layer */
    EVENT_GROUP_FRAMEWORK = 0x02,   /**< Message group from Framework layer */
    EVENT_GROUP_APP = 0x03,         /**< Message group from APP layer */
    EVENT_GROUP_INSTRUMENT = 0x0A,  /**< Message group from instrument layer */
} T_EVENT_GROUP;
/**  @brief IO type definitions for IO message, may extend as requested. */
typedef enum
{
    IO_MSG_TYPE_BT_STATUS,  /**< BT status change with subtype @ref GAP_MSG_TYPE */
    IO_MSG_TYPE_KEYSCAN,    /**< Key scan message with subtype @ref T_IO_MSG_KEYSCAN */
    IO_MSG_TYPE_QDECODE,    /**< Subtype to be defined */
    IO_MSG_TYPE_UART,       /**< Uart message with subtype @ref T_IO_MSG_UART */
    IO_MSG_TYPE_KEYPAD,     /**< Subtype to be defined */
    IO_MSG_TYPE_IR,         /**< Subtype to be defined */
    IO_MSG_TYPE_GDMA,       /**< Subtype to be defined */
    IO_MSG_TYPE_ADC,        /**< Subtype to be defined */
    IO_MSG_TYPE_D3DG,       /**< Subtype to be defined */
    IO_MSG_TYPE_SPI,        /**< Subtype to be defined */
    IO_MSG_TYPE_MOUSE_BUTTON,   /**< Subtype to be defined */
    IO_MSG_TYPE_GPIO,       /**< Gpio message with subtype @ref T_IO_MSG_GPIO*/
    IO_MSG_TYPE_MOUSE_SENSOR,   /**< Subtype to be defined */
    IO_MSG_TYPE_TIMER,      /**< App timer message with subtype @ref T_IO_MSG_TIMER */
    IO_MSG_TYPE_WRISTBNAD,  /**< wristband message with subtype @ref T_IO_MSG_WRISTBAND */
    IO_MSG_TYPE_MESH_STATUS,    /**< Subtype to be defined */
    IO_MSG_TYPE_KEYBOARD_BUTTON, /**< Subtype to be defined */
    IO_MSG_TYPE_ANCS,            /**< ANCS message*/
    IO_MSG_TYPE_CONSOLE,    /**< Console message with subtype @ref T_IO_CONSOLE */
    IO_MSG_TYPE_IR_LEARN_DATA,       /**< IR learn data message*/
    IO_MSG_TYPE_IR_LEARN_STOP,       /**< IR learn stop message*/
    IO_MSG_TYPE_IR_START_SEND_REPEAT_CODE, /**< IR send repeat code message*/
    IO_MSG_TYPE_IR_SEND_COMPLETE,    /**< IR send complete message*/
    IO_MSG_TYPE_BAT_LPC,        /**< LPC send low power message*/
    IO_MSG_TYPE_BAT_DETECT,     /**< BAT adc detect battery value*/
    IO_MSG_TYPE_AUDIO,          /**< Audio message with subtype @ref T_IO_MSG_TYPE_AUDIO*/
    IO_MSG_TYPE_RESET_WDG_TIMER, /**< Reset watch dog timer*/
    IO_MSG_TYPE_RESET_AON_WDG_TIMER, /**< Reset aon watch dog timer*/
    IO_MSG_TYPE_RTC,             /**< Subtype to be defined */
    IO_MSG_TYPE_I2C,             /**< Subtype to be defined */
    IO_MSG_TYPE_WIFI_UART,       /**< WIFI uart message */
    IO_MSG_TYPE_PD_TIMER,
    IO_MSG_TYPE_AMA_BT_MSG,
    IO_MSG_TYPE_DFU_VALID_FW,
    IO_MSG_TYPE_VOICE,           /**< Voice message */
    IO_MSG_TYPE_MULTIPROTOCOL,   /** Multiprotocol messages */
    IO_MSG_TYPE_ESL_COMMS,
    IO_MSG_TYPE_PERIPHERALS_HAL,
} T_IO_MSG_TYPE;

/**  @brief IO subtype definitions for @ref T_IO_CONSOLE type. */
typedef enum
{
    IO_MSG_CONSOLE_STRING_RX    = 0x01, /**< Console CLI RX event */
    IO_MSG_CONSOLE_STRING_TX    = 0x02, /**< Console CLI TX event */
    IO_MSG_CONSOLE_BINARY_RX    = 0x03, /**< Console protocol RX event */
    IO_MSG_CONSOLE_BINARY_TX    = 0x04, /**< Console protocol TX event */
} T_IO_CONSOLE;

/**  @brief IO subtype definitions for @ref IO_MSG_TYPE_ADC type. */
typedef enum
{
    IO_MSG_ADC_FIFO_READ_ERR,
    IO_MSG_ADC_FIFO_OVERFLOW,
} T_IO_MSG_TYPE_ADC;

/**  @brief IO subtype definitions for @ref IO_MSG_TYPE_AUDIO type. */
typedef enum
{
    IO_MSG_AUDIO_INPUT_BUFF_READY,
    IO_MSG_AUDIO_PROCESS_DONE,
    IO_MSG_AUDIO_TIMEOUT,
    IO_MSG_AUDIO_HD_ATTACHED,
    IO_MSG_AUDIO_HD_DETACHED,
} T_IO_MSG_TYPE_AUDIO;

/**  @brief IO subtype definitions for @ref IO_MSG_TYPE_KEYSCAN type. */
typedef enum
{
    IO_MSG_KEYSCAN_RX_PKT        = 1, /**< Keyscan RX data event */
    IO_MSG_KEYSCAN_MAX           = 2, /**<  */
    IO_MSG_KEYSCAN_ALLKEYRELEASE = 3, /**< All keys are released event */
    IO_MSG_KEYSCAN_STUCK         = 4, /**< Key stuck message */
    IO_MSG_KEYSCAN_LONG_PRESS    = 5,
    IO_MSG_KEYSCAN_REPEAT_SEND   = 6,
} T_IO_MSG_KEYSCAN;

/**  @brief IO subtype definitions for @ref IO_MSG_TYPE_UART type. */
typedef enum
{
    IO_MSG_UART_RX                     = 1,
    IO_MSG_UART_RX_TIMEOUT             = 2,
    IO_MSG_UART_RX_OVERFLOW            = 3,
    IO_MSG_UART_RX_TIMEOUT_OVERFLOW    = 4,
    IO_MSG_UART_RX_EMPTY               = 5,
} T_IO_MSG_UART;

/**  @brief IO subtype definitions for @ref IO_MSG_TYPE_GPIO type. */
typedef enum
{
    IO_MSG_GPIO_KEY,               /**< KEY GPIO event */
    IO_MSG_GPIO_LINE_IN,           /**< LINE IN event */
    IO_MSG_GPIO_NFC,               /**< NFC event */
    IO_MSG_GPIO_UART_WAKE_UP,      /**< UART WAKE UP event */
    IO_MSG_GPIO_CHARGER,           /**< CHARGER event */
} T_IO_MSG_GPIO;

/**  @brief IO subtype definitions for @ref IO_MSG_TYPE_TIMER type. */
typedef enum
{
    IO_MSG_TIMER_ALARM,
    IO_MSG_TIMER_RWS
} T_IO_MSG_TIMER;

/**  @brief IO subtype definitions for @ref IO_MSG_TYPE_WRISTBAND type. */
typedef enum
{
    IO_MSG_BWPS_TX_VALUE,
    IO_MSG_RTC_TIMEROUT_WALL_CLOCK,
    IO_MSG_SENSOR_WAKE_UP,
    IO_MSG_LED_TWINKLE,
    IO_MSG_MOTOR_VIBRATE,
    IO_MSG_CHARGER_STATE,
    IO_MSG_RTC_LOW_BATTERY_RESTORE,
    IO_MSG_RTC_ALARM,
    IO_MSG_POWER_OFF,
    IO_MSG_POWER_ON,
    IO_MSG_HRS_EVENT,
    IO_MSG_SENSOR_MOTION_INTERRUPT,
    IO_MSG_UART_CMD_DEBUG,
    IO_MSG_HRS_TIMEOUT_HANDLE,
    IO_MSG_UPDATE_CONPARA,
    IO_MSG_REPORT_BUTTON,
    IO_MSG_UART_GPS,
    IO_MSG_UART_DEBUG_RX,
    IO_MSG_WAS_RX_VALUE,
    IO_MSG_WAS_ENABLE_CCCD,
    IO_MSG_VOICE_DMA_RX,
    IO_MSG_TOUCH_GES,
    IO_MSG_TOUCH_INT,
    IO_MSG_TOUCH_TIMEOUT,
    IO_MSG_TOUCH_HANDLE,
    IO_MSG_MENU_TIMER,
    IO_MSG_LCD_SYNC,
    IO_MSG_MAGIC_OPEN_ADV,
    IO_MSG_MAGIC_PAIR_ADV,
    IO_MSG_MAGIC_STOP_ADV,
    IO_MSG_BREEZE_AIS_CB,
    IO_MSG_ANCS_DISCOVERY,
} T_IO_MSG_WRISTBAND;

/**  @brief IO subtype definitions for @ref IO_MSG_TYPE_UART type. */
typedef enum
{
    IO_MSG_WIFI_UART_RX_PKT         = 1, /**< WIFI UART RX data event */
    IO_MSG_WIFI_UART_RETRANS        = 2, /**< WIFI UART retransmission event*/
    IO_MSG_WIFI_UART_RETRANS_FAIL   = 3, /**< WIFI UART retransmission failed event*/
} T_IO_MSG_WIFI_UART;

typedef enum
{
    IO_MSG_MPM_PPT,
    IO_MSG_MPM_154
} T_IO_MSG_MULTIPROTOCOL;

/**  @brief IO message definition for communications between tasks. */
typedef struct
{
    uint16_t type;
    uint16_t subtype;
    union
    {
        uint32_t  param;
        void     *buf;
    } u;
} T_IO_MSG;

/** @} */ /* End of group APP_MSG_Exported_Types */

/** @} */ /* End of group APP_MSG */

#ifdef __cplusplus
}
#endif

#endif /* _APP_MSG_H_ */
