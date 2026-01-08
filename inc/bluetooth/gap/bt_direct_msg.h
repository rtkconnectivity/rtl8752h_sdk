/**
*********************************************************************************************************
*               Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      bt_direct_msg.h
* @brief     This file contains all the constants and functions prototypes for BT direct message.
* @details   This file is used both bredr and le.
* @author
* @date      2021-07-12
* @version   v0.8
* *********************************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef BT_DIRECT_MSG_H
#define BT_DIRECT_MSG_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include "upperstack_config.h"

/** @addtogroup GAP GAP Module
  * @{
  */

/** @addtogroup BT_DIRECT_MSG BT Direct Message Module
  * @{
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup BT_DIRECT_MSG_Exported_Macros BT Direct Message Exported Macros
  * @{
  */

/** @defgroup BT_DIRECT_MSG_CALLBACK_TYPE BT Direct Message Callback Type
 * @{
 */
#define BT_DIRECT_MSG_ISO_DATA_IND         0x01 /**< Indication msg type about ISO Data for @ref ISOCH_ROLE_INITIATOR, @ref ISOCH_ROLE_ACCEPTOR and @ref BIG_MGR_ROLE_SYNC_RECEIVER.
                                                     APP shall call @ref gap_iso_data_cfm when receiving this message.
                                                     The structure of callback data is @ref T_BT_DIRECT_ISO_DATA_IND. */

#define BT_DIRECT_MSG_GATT_SERVER_SERVICE_GET_ALLOW_INFO    0xD0 /**< Information msg type about server service get allowance from APP.
                                                                      The structure of callback data is @ref T_BT_DIRECT_GATT_SERVER_SERVICE_GET_ALLOW_INFO. */
/** End of BT_DIRECT_MSG_CALLBACK_TYPE
  * @}
  */
/** End of BT_DIRECT_MSG_Exported_Macros
  * @}
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup BT_DIRECT_MSG_Exported_Types BT Direct Message Exported Types
  * @{
  */

/** @brief Packet Status Flag in HCI ISO Datat packet sent by the Controller. */
typedef enum
{
    ISOCH_DATA_PKT_STATUS_VALID_DATA = 0,             /**< Valid data. The complete SDU was received correctly. */
    ISOCH_DATA_PKT_STATUS_POSSIBLE_ERROR_DATA = 1,    /**< Possibly invalid data. The contents of the SDU may
                                                           contain errors or part of the SDU may be missing.
                                                           This is reported as 'data with possible errors'. */
    ISOCH_DATA_PKT_STATUS_LOST_DATA = 2,              /**< Part(s) of the SDU were not received correctly.
                                                           This is reported as 'lost data'. */
} T_ISOCH_DATA_PKT_STATUS;

/** @brief Indication of ISO Data packet with msg type @ref BT_DIRECT_MSG_ISO_DATA_IND. */
typedef struct
{
    uint16_t conn_handle;                     /**< Connection handle of the CIS or BIS. */
    T_ISOCH_DATA_PKT_STATUS pkt_status_flag;  /**< @ref T_ISOCH_DATA_PKT_STATUS. */
    uint8_t   *p_buf;                         /**< Pointer to the buffer that needs to be released. */
    uint16_t  offset;                         /**< Offset from start of the SDU to @ref p_buf.
                                                   - p_data->p_bt_direct_iso->p_buf + p_data->p_bt_direct_iso->offset indicates
                                                        the start of the SDU. */
    uint16_t  iso_sdu_len;                    /**< Length of the SDU. */
    uint16_t  pkt_seq_num;                    /**< The sequence number of the SDU. */
    bool      ts_flag;                        /**< Indicates whether it contains time_stamp.
                                                   - true: Contains time_stamp.
                                                   - false: Does not contain time_stamp. */
    uint32_t  time_stamp;                     /**< A time in microseconds. time_stamp is valid when @ref ts_flag is true. */
} T_BT_DIRECT_ISO_DATA_IND;

typedef struct
{
    uint16_t conn_handle;           /**< Connection handle. */
    uint8_t  remote_addr[6];        /**< Remote address. */
    uint8_t  remote_addr_type;      /**< Remote address type. */
    uint8_t  local_addr[6];         /**< Valid if remote_addr_type is not @ref GAP_REMOTE_ADDR_CLASSIC. */
    uint8_t  local_addr_type;       /**< Valid if remote_addr_type is not @ref GAP_REMOTE_ADDR_CLASSIC. */
    uint8_t  role;                  /**< Valid if remote_addr_type is not @ref GAP_REMOTE_ADDR_CLASSIC.
                                         - 0x00: Central.
                                         - 0x01: Peripheral. */
    uint8_t  service_id;            /**< Service ID. */
    uint8_t  *p_use_flag;           /**< APP supply use flag to indicate whether *p_allow could be used.
                                         - *p_use_flag = value.
                                             - 0: Not use *p_allow.
                                             - 1: Use *p_allow. */
    uint16_t *p_allow;              /**< APP supply allowance of service.
                                         - *p_allow = value.
                                             - 0: Not allow.
                                             - 1: Allow. */
    uint16_t attribute_idx;       /**< Attribute index. */
    uint16_t attribute_handle;      /**< Attribute handle. */
} T_BT_DIRECT_GATT_SERVER_SERVICE_GET_ALLOW_INFO;

typedef union
{
    T_BT_DIRECT_ISO_DATA_IND                *p_bt_direct_iso;

    T_BT_DIRECT_GATT_SERVER_SERVICE_GET_ALLOW_INFO *p_bt_direct_gatt_server_service_get_allow_info;
} T_BT_DIRECT_CB_DATA;

/** End of BT_DIRECT_MSG_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/**
 * @defgroup BT_DIRECT_MSG_EXPORT_Functions BT Direct Message Exported Functions
 *
 * @{
 */

/**
  * @brief      Callback for Bluetooth direct message to notify APP.
  *
  * @param[in] cb_type    Callback msg type @ref BT_DIRECT_MSG_CALLBACK_TYPE.
  * @param[in] p_cb_data  Pointer to callback data @ref T_BT_DIRECT_CB_DATA.
  */
typedef void(*P_FUN_BT_DIRECT_CB)(uint8_t cb_type, void *p_cb_data);

/**
 * @brief         Register callback to gap, when messages in @ref BT_DIRECT_MSG_CALLBACK_TYPE happens, it will callback to app.
 *
 * @param[in]     app_callback    Callback function provided by the APP to handle Bluetooth direct messages.
 *                - NULL   -> Do not send Bluetooth direct messages to APP.
 *                - Others -> Use application-defined callback function.
 *
 * <b>Example usage</b>
 * \code{.c}
   void test(void)
   {
       ......
       gap_register_direct_cb(app_gap_direct_callback);
   }

    void app_gap_direct_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_BT_DIRECT_CB_DATA *p_data = (T_BT_DIRECT_CB_DATA *)p_cb_data;

        ......
        switch (cb_type)
        {
        case BT_DIRECT_MSG_ISO_DATA_IND:
            {
                // Start of the SDU
                uint8_t *p_iso_data = p_data->p_bt_direct_iso->p_buf + p_data->p_bt_direct_iso->offset;

                APP_PRINT_INFO6("app_gap_direct_callback: BT_DIRECT_MSG_ISO_DATA_IND, conn_handle 0x%x, pkt_status_flag 0x%x, pkt_seq_num 0x%x, ts_flag 0x%x, time_stamp 0x%x, iso_sdu_len 0x%x",
                                p_data->p_bt_direct_iso->conn_handle, p_data->p_bt_direct_iso->pkt_status_flag,
                                p_data->p_bt_direct_iso->pkt_seq_num, p_data->p_bt_direct_iso->ts_flag,
                                p_data->p_bt_direct_iso->time_stamp, p_data->p_bt_direct_iso->iso_sdu_len);
                ......
                gap_iso_data_cfm(p_data->p_bt_direct_iso->p_buf);
            }
            break;
            ......
        }
    }
   \endcode
 */
void gap_register_direct_cb(P_FUN_BT_DIRECT_CB app_callback);

/** End of BT_DIRECT_MSG_EXPORT_Functions
  * @}
  */

/** End of BT_DIRECT_MSG
  * @}
  */

/** End of GAP
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* BT_DIRECT_MSG_H */
