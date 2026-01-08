/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     tp_client.h
  * @brief
  * @details  Tp server data structs and external functions declaration.
  * @author   jane
  * @date     2016-02-18
  * @version  v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _TP_CLIENT_H_
#define _TP_CLIENT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profile_client.h"
#include "tp_config.h"

typedef enum
{
    TP_CLIENT_CB_TYPE_DISC_STATE,          //!< Discovery procedure state, done or pending.
    TP_CLIENT_CB_TYPE_WRITE_RESULT,         //!< Read request's result data, responsed from server.
    TP_CLIENT_CB_TYPE_READ_RESULT,         //!< Read request's result data, responsed from server.
    TP_CLIENT_CB_TYPE_NOTIF_IND_RESULT,
    TP_CLIENT_CB_TYPE_INVALID              //!< Invalid callback type, no practical usage.
} T_TP_CB_TYPE;

typedef enum
{
    TP_WRITE_NOTIFY_CCCD,
    TP_WRITE_IND_CCCD,
    TP_WRITE_CHAR_VALUE,
} T_TP_WRITE_TYPE;

/** @brief TP client handle type*/
typedef enum
{
    HDL_TP_SRV_START,               //!< start handle of tp server service.
    HDL_TP_SRV_END,                 //!< end handle of tp server service.
    HDL_TP_CT_TX_INDICATE,          //!< CT_TX indicate characteristic value handle.
    HDL_TP_CT_TX_INDICATE_CCCD,     //!< CT_TX indicate characteristic CCCD handle.
    HDL_TP_CT_RX_WRITE,             //!< CT_RX write characteristic value handle.
    HDL_TP_DT_TX_NOTIFY,            //!< DT_TX notify characteristic value handle.
    HDL_TP_DT_TX_NOTIFY_CCCD,       //!< DT_TX notify characteristic CCCD handle.
    HDL_TP_DT_RX_WRITE,             //!< DT_RX write characteristic value handle.
    HDL_TP_CACHE_LEN                //!< handle cache length.
} T_TP_HANDLE_TYPE;

/** @brief TP client discovery state*/
typedef enum
{
    DISC_TP_IDLE,
    DISC_TP_START,
    DISC_TP_DONE,
    DISC_TP_FAILED
} T_TP_DISC_STATE;

typedef struct
{
    uint8_t notify_ind;
    uint16_t value_size;
    uint8_t *pValue;
} T_TP_NOTIF_IND_DATA;

typedef struct
{
    T_TP_WRITE_TYPE type;
    T_GATT_WRITE_TYPE write_type;
    uint16_t cause;
    uint8_t credits;
} T_TP_WRITE_RESULT;

typedef enum
{
    TP_READ_PREFER_PARAM,
} T_TP_READ_TYPE;

typedef struct
{
    T_TP_READ_TYPE type;
    uint16_t cause;
    uint16_t value_size;
    uint8_t *p_value;
} T_TP_READ_RESULT;

typedef union
{
    T_TP_DISC_STATE disc_state;
    T_TP_NOTIF_IND_DATA notif_ind_data;
    T_TP_WRITE_RESULT write_result;
    T_TP_READ_RESULT read_result;
} T_TP_DATA;

typedef struct
{
    T_TP_CB_TYPE     cb_type;
    T_TP_DATA        cb_content;
} T_TP_CB_DATA;

T_CLIENT_ID tp_client_add(P_FUN_GENERAL_APP_CB app_cb);
bool tp_client_start_discovery(uint8_t conn_id);
bool tp_client_write_notify_cccd(uint8_t conn_id, bool value);
bool tp_client_write_ind_cccd(uint8_t conn_id, bool value);
bool tp_client_write_value(uint8_t conn_id, uint16_t length, uint8_t *p_value);
bool tp_client_write_command(uint8_t conn_id, uint16_t length, uint8_t *p_value);
bool tp_client_read_prefer_param(uint8_t conn_id);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif  /* _TP_CLIENT_H_ */
