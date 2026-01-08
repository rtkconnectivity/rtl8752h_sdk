/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    simple_ble_config.h
  * @brief   This file includes common constants or types for Simple BLE service/client.
  *          And some optional feature may be defined in this file.
  * @details
  * @author  Ethan
  * @date    2016-02-18
  * @version v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion **/
#ifndef _TP_CONFIG_H_
#define _TP_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    TP_TEST_DIR_SERVER_TO_CLIENT,
    TP_TEST_DIR_CLIENT_TO_SERVER,
    TP_TEST_DIR_BOTH_WAYS,
} T_TP_TEST_DIR;

typedef enum
{
    TP_FLAG_1M_WITHOUT_DATA_LEN_EXTENSION,
    TP_FLAG_1M_WITH_DATA_LEN_EXTENSION,
    TP_FLAG_2M_WITHOUT_DATA_LEN_EXTENSION,
    TP_FLAG_2M_WITH_DATA_LEN_EXTENSION,
} T_TP_FLAG;

typedef struct
{
    uint8_t opcode;
    uint8_t length[2];
    uint8_t time[2];
    uint8_t con_interval_min[2];
    uint8_t con_interval_max[2];
    uint8_t flag;
} T_TP_WRITE_REQ;

#define GATT_UUID_VENDOR_TP_SERVICE                                  0xA00D
#define GATT_UUID_CHAR_VENDOR_TP_CT_TX_NOTIFY_INDICATE               0xB001
#define GATT_UUID_CHAR_VENDOR_TP_CT_RX_WRITE_REQ                     0xB002
#define GATT_UUID_CHAR_VENDOR_TP_DT_TX_NOTIFY_INDICATE               0xB003
#define GATT_UUID_CHAR_VENDOR_TP_DT_RX_WRITE_CMD                     0xB004

#define GATT_OPCODE_CHAR_CT_RX_CONFIG_DT_TX_PREFER_PARAM             0x11
#define GATT_OPCODE_CHAR_CT_RX_CONFIG_DT_RX_PREFER_PARAM             0x12
#define GATT_OPCODE_CHAR_CT_RX_CONFIG_DT_TX_RX_PREFER_PARAM          0x13

#define GATT_OPCODE_CHAR_CT_TX_CONFIG_PREFER_PARAM_RSP               0x21
#define GATT_OPCODE_CHAR_CT_TX_DATA_SEND_COMPLETE                    0x22

#define GATT_OPCODE_CHAR_CT_RX_GET_TEST_REPORT                       0x31

#define GATT_OPCODE_CHAR_CT_TX_DATA_RATE_REPORT                      0x32


#ifdef __cplusplus
}
#endif

#endif
