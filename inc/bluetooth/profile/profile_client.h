/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     profile_client.h
  * @brief    Header file for profile client structure.
  * @details  Common data structure definition.
  * @author   ethan_su
  * @date     2016-02-18
  * @version  v1.0
  * *************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef PROFILE_CLIENT_H
#define PROFILE_CLIENT_H

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "gatt.h"
#include "bt_types.h"
#include "gap_le.h"

/** @addtogroup GATT_CLIENT_API GATT Client API
  * @brief GATT client API
  * @{
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup GATT_Client_Exported_Macros GATT Client Exported Macros
  * @{
  */
#define CLIENT_PROFILE_GENERAL_ID   0xff        //!< General Client ID used by application, when directly calls the APIs of profile client layer. Distinguish with other specific client module.

/** End of GATT_Client_Exported_Macros
* @}
*/
/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup GATT_Client_Exported_Types GATT Client Exported Types
  * @{
  */

/** @defgroup GATT_CLIENT_TYPES_DEFs GATT Client Types Definitions
  * @{
  */
typedef uint8_t T_CLIENT_ID; //!< Client ID

/** @brief  Discovery state during discovery procedure.*/
typedef enum
{
    DISC_STATE_IDLE,                    //!< Discovery state idle
    DISC_STATE_SRV,                     //!< Discovery services
    DISC_STATE_SRV_DONE,                //!< Discovery services done
    DISC_STATE_RELATION,                //!< Discovery relation
    DISC_STATE_RELATION_DONE,           //!< Discovery relation done
    DISC_STATE_CHAR,                    //!< Discovery characteristics
    DISC_STATE_CHAR_DONE,               //!< Discovery characteristics done
    DISC_STATE_CHAR_UUID16_DONE,        //!< Discovery 16 bit UUID characteristics done
    DISC_STATE_CHAR_UUID128_DONE,       //!< Discovery 128 bit UUID characteristics done
    DISC_STATE_CHAR_DESCRIPTOR,         //!< Discovery characteristic descriptors
    DISC_STATE_CHAR_DESCRIPTOR_DONE,    //!< Discovery characteristic descriptors done
    DISC_STATE_FAILED                   //!< Discovery failed
} T_DISCOVERY_STATE;

/** @brief  Discovery result type*/
typedef enum
{
    DISC_RESULT_ALL_SRV_UUID16,         //!< Discovery all 16 bit UUID services
    DISC_RESULT_ALL_SRV_UUID128,        //!< Discovery all 128 bit UUID services
    DISC_RESULT_SRV_DATA,               //!< Discovery service data
    DISC_RESULT_CHAR_UUID16,            //!< Discovery 16 bit UUID characteristics
    DISC_RESULT_CHAR_UUID128,           //!< Discovery 128 bit UUID characteristics
    DISC_RESULT_CHAR_DESC_UUID16,       //!< Discovery 16 bit UUID characteristic descriptors
    DISC_RESULT_CHAR_DESC_UUID128,      //!< Discovery 128 bit UUID characteristic descriptors
    DISC_RESULT_RELATION_UUID16,        //!< Discovery 16 bit UUID relations
    DISC_RESULT_RELATION_UUID128,       //!< Discovery 128 bit UUID relations
    DISC_RESULT_BY_UUID16_CHAR,         //!< Discovery characteristics by 16 bit UUID
    DISC_RESULT_BY_UUID128_CHAR,        //!< Discovery characteristics by 128 bit UUID
} T_DISCOVERY_RESULT_TYPE;

/** @brief  GATT write type*/
typedef enum
{
    GATT_WRITE_TYPE_REQ = 0x01,           /**<  ATT "Write Request".  */
    GATT_WRITE_TYPE_CMD = 0x02,           /**<  ATT "Write Command".  */
    GATT_WRITE_TYPE_SIGNED_CMD = 0x04     /**<  ATT "Signed Write Command".  */
} T_GATT_WRITE_TYPE;

typedef struct
{
    uint16_t    att_handle;         /**< The handle for the service declaration. */
    uint16_t    end_group_handle;   /**< The handle of the last attribute
    within the service definition. */
    uint16_t    uuid16;             /**< 16 bit UUID. */
} T_GATT_SERVICE_ELEM16;

typedef struct
{
    uint16_t    att_handle;         /**< The handle for the service declaration. */
    uint16_t    end_group_handle;
    /**< The handle of the last attribute within the service definition. */
    uint8_t     uuid128[16];        /**< 128 bit UUID. */
} T_GATT_SERVICE_ELEM128;


typedef struct
{
    uint16_t    att_handle;         /**< The handle for the service declaration. */
    uint16_t    end_group_handle;
    /**< The handle of the last attribute within the service definition. */
} T_GATT_SERVICE_BY_UUID_ELEM;

/** @brief  Characteristic declaration for 16 bit UUID.*/
typedef struct
{
    uint16_t    decl_handle;   /**<  Attribute handle. */
    uint16_t    properties;    /**<  Characteristic Properties, high octet is reserved. */
    uint16_t    value_handle;  /**<  Characteristic Value Handle. */
    uint16_t    uuid16;        /**<  16-bit Bluetooth UUID for Characteristic Value. */
} T_GATT_CHARACT_ELEM16;

/** @brief  Characteristic declaration for 128 bit UUID.*/
typedef struct
{
    uint16_t    decl_handle;    /**<  Attribute handle. */
    uint16_t    properties;     /**<  Characteristic Properties, high octet is reserved. */
    uint16_t    value_handle;   /**<  Characteristic Value Handle. */
    uint8_t     uuid128[16];    /**<  128-bit UUID for Characteristic Value. */
} T_GATT_CHARACT_ELEM128;

/** @brief  Characteristic descriptor for 16 bit UUID.*/
typedef struct
{
    uint16_t    handle;         /**< Attribute handle. */
    uint16_t    uuid16;         /**< 16 bit UUID. */
} T_GATT_CHARACT_DESC_ELEM16;

/** @brief  Characteristic descriptor for 128 bit UUID.*/
typedef struct
{
    uint16_t    handle;         /**< Attribute handle. */
    uint8_t     uuid128[16];    /**< 128 bit UUID. */
} T_GATT_CHARACT_DESC_ELEM128;

/** @brief  Relationship discovery for 16 bit UUID.*/
typedef struct
{
    uint16_t    decl_handle;
    uint16_t    att_handle;
    uint16_t    end_group_handle;
    uint16_t    uuid16;
} T_GATT_RELATION_ELEM16;

/** @brief  Relationship discovery for 128 bit UUID.*/
typedef struct
{
    uint16_t    decl_handle;
    uint16_t    att_handle;
    uint16_t    end_group_handle;
    uint8_t     uuid128[16];
} T_GATT_RELATION_ELEM128;

/** @brief  Discovery result data*/
typedef union
{
    T_GATT_SERVICE_ELEM16        *p_srv_uuid16_disc_data;
    T_GATT_SERVICE_ELEM128       *p_srv_uuid128_disc_data;
    T_GATT_SERVICE_BY_UUID_ELEM  *p_srv_disc_data;
    T_GATT_CHARACT_ELEM16        *p_char_uuid16_disc_data;
    T_GATT_CHARACT_ELEM128       *p_char_uuid128_disc_data;
    T_GATT_CHARACT_DESC_ELEM16   *p_char_desc_uuid16_disc_data;
    T_GATT_CHARACT_DESC_ELEM128  *p_char_desc_uuid128_disc_data;
    T_GATT_RELATION_ELEM16       *p_relation_uuid16_disc_data;
    T_GATT_RELATION_ELEM128      *p_relation_uuid128_disc_data;
} T_DISCOVERY_RESULT_DATA;
/** End of GATT_CLIENT_TYPES_DEFs
  * @}
  */

/** @defgroup General_cb_data General Client Callback Data
  * @{
  */
/** @brief  The callback data of CLIENT_APP_CB_TYPE_DISC_STATE.
  *
  * Discovery procedure related data to inform application.
  */
typedef struct
{
    T_DISCOVERY_STATE disc_state;
} T_DISC_STATE_CB_DATA;

/** @brief  The callback data of CLIENT_APP_CB_TYPE_DISC_RESULT.
  *
  * Discovery result data will be sent to Bluetooth Host through the callback.
  */
typedef struct
{
    T_DISCOVERY_RESULT_TYPE result_type;
    T_DISCOVERY_RESULT_DATA result_data;
} T_DISC_RESULT_CB_DATA;


/** @brief  The callback type of T_CLIENT_APP_CB_DATA.
  *
  * Message data type, when data sent to APP directly.
  */
typedef enum
{
    CLIENT_APP_CB_TYPE_DISC_STATE,
    CLIENT_APP_CB_TYPE_DISC_RESULT
} T_CLIENT_CB_TYPE;

/** @brief  The callback data of T_CLIENT_APP_CB_DATA.
  *
  * Client received data from server, when no specific client registered, will be sent to APP directly.
  */
typedef union
{
    T_DISC_STATE_CB_DATA   disc_state_data;
    T_DISC_RESULT_CB_DATA  disc_result_data;
} T_CLIENT_CB_DATA;

/** @brief  The General Client Callback Data Struct.
  *
  * Callback data sent to application directly from client, include type and content.
  */
typedef struct
{
    T_CLIENT_CB_TYPE     cb_type;
    T_CLIENT_CB_DATA     cb_content;
} T_CLIENT_APP_CB_DATA;
/** End of General_cb_data
  * @}
  */

/** @defgroup P_FUN_GENERAL_APP_CB General Client Callback Function Point Definition
  * @{ Function pointer used in each specific profile, to send events to application.
  */
typedef T_APP_RESULT(*P_FUN_GENERAL_APP_CB)(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data);
/** End of P_FUN_GENERAL_APP_CB
  * @}
  */


/** @defgroup T_FUN_CLIENT_CBS_DEF Specific Client Callback Function Point Definition
  * @{ Function pointer used in each specific client module, to send events to specific client module.
  */
typedef void (*P_FUN_DISCOVER_STATE_CB)(uint8_t conn_id, T_DISCOVERY_STATE discovery_state);
typedef void (*P_FUN_DISCOVER_RESULT_CB)(uint8_t conn_id,  T_DISCOVERY_RESULT_TYPE result_type,
                                         T_DISCOVERY_RESULT_DATA result_data);
typedef void (*P_FUN_READ_RESULT_CB)(uint8_t conn_id,  uint16_t cause, uint16_t handle,
                                     uint16_t value_size, uint8_t *p_value);
typedef void (*P_FUN_WRITE_RESULT_CB)(uint8_t conn_id, T_GATT_WRITE_TYPE type, uint16_t handle,
                                      uint16_t cause, uint8_t credits);
typedef T_APP_RESULT(*P_FUN_NOTIFY_IND_RESULT_CB)(uint8_t conn_id, bool notify, uint16_t handle,
                                                  uint16_t value_size, uint8_t *p_value);
typedef void (*P_FUN_DISCONNECT_CB)(uint8_t conn_id);
/** End of T_FUN_CLIENT_CBS_DEF
  * @}
  */

/** @defgroup T_FUN_CLIENT_CBS Specific Client Callback Functions Struct
  * @{
  */
typedef struct
{
    P_FUN_DISCOVER_STATE_CB    discover_state_cb;   //!< Discovery state callback function pointer.
    P_FUN_DISCOVER_RESULT_CB   discover_result_cb;  //!< Discovery result callback function pointer.
    P_FUN_READ_RESULT_CB       read_result_cb;      //!< Read response callback function pointer.
    P_FUN_WRITE_RESULT_CB      write_result_cb;     //!< Write result callback function pointer.
    P_FUN_NOTIFY_IND_RESULT_CB notify_ind_result_cb;//!< Notify Indication callback function pointer.
    P_FUN_DISCONNECT_CB        disconnect_cb;       //!< Disconnection callback function pointer.
} T_FUN_CLIENT_CBS;
/** End of T_FUN_CLIENT_CBS
  * @}
  */

/** End of GATT_Client_Exported_Types
  * @}
  */
/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup GATT_Client_Exported_Functions GATT Client Exported Functions
  * @{
  */
/**
 * @brief Initialize parameters of GATT client.
 *
 * @param[in] client_num  Set the number of clients that need to register.
 * @return void.
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_le_profile_init(void)
    {
        client_init(client_num);
    }
  * \endcode
  */
void client_init(uint8_t client_num);

/**
  * @brief  Used by application, register general client callback.
  * @param[in]  p_fun_cb  Function offered by application.
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_le_profile_init(void)
    {
        client_init(client_num);
        simple_ble_client_id = simp_ble_add_client(app_client_callback);
        client_register_general_client_cb(app_client_callback);
    }
 * \endcode
 */
void client_register_general_client_cb(P_FUN_GENERAL_APP_CB p_fun_cb);

/**
 * @brief  Used by specific client, register callback.
 * @param[in,out]  p_out_client_id  Pointer to Client ID generated for registered specific
 *                                  client module @ref T_CLIENT_ID.
 * @param[in]      client_cbs       Pointer to callback functions implemented in
 *                                  specific client module @ref T_FUN_CLIENT_CBS.
 * @return Operation result.
 * @retval true  Operation success.
 * @retval false Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}

    T_CLIENT_ID simp_ble_add_client(P_FUN_GENERAL_APP_CB app_cb, uint8_t link_num)
    {
        uint16_t size;
        if (link_num > SIMP_MAX_LINKS)
        {
            APP_PRINT_ERROR1("simp_ble_add_client: invalid link_num %d", link_num);
            return 0xFF;
        }
        if (false == client_register_spec_client_cb(&simp_client, &simp_ble_client_cbs))
        {
            simp_client = CLIENT_PROFILE_GENERAL_ID;
            APP_PRINT_ERROR0("simp_ble_add_client failed");
            return simp_client;
        }
        APP_PRINT_INFO1("simp_ble_add_client: simp_client %d", simp_client);

        simp_client_cb = app_cb;
        simp_link_num = link_num;
        size = simp_link_num * sizeof(T_SIMP_LINK);
        simp_table = os_mem_zalloc(RAM_TYPE_DATA_ON, size);

        return simp_client;
    }
 * \endcode
 */
bool client_register_spec_client_cb(T_CLIENT_ID *p_out_client_id,
                                    const T_FUN_CLIENT_CBS *client_cbs);

/**
  * @brief  Send discovery all primary services request.
  *
  * When client_id is set to @ref CLIENT_PROFILE_GENERAL_ID, if sending request operation is successful,
  * the discovering result will be returned by callback registered by @ref client_register_general_client_cb
  * with client_id set to @ref CLIENT_PROFILE_GENERAL_ID and cb_type of p_data (@ref T_CLIENT_APP_CB_DATA)
  * set to @ref CLIENT_APP_CB_TYPE_DISC_STATE and @ref CLIENT_APP_CB_TYPE_DISC_RESULT.
  *
  * When client_id is set to specific client ID registered by @ref client_register_spec_client_cb, if sending
  * request operation is successful, the discovering result will be returned by discover_state_cb and discover_result_cb
  * registered by @ref client_register_spec_client_cb.
  *
  * @param[in]  conn_id        Connection ID.
  * @param[in]  client_id      - Client ID of specific client module. Value is @ref T_CLIENT_ID.
  *                            - @ref CLIENT_PROFILE_GENERAL_ID.
  *
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  */
T_GAP_CAUSE client_all_primary_srv_discovery(uint8_t conn_id, T_CLIENT_ID client_id);

/**
  * @brief  Send discovery services by 16-bit UUID request.
  *
  * If sending request operation is successful, the discovering result will be returned by discover_state_cb
  * and discover_result_cb registered by @ref client_register_spec_client_cb.
  *
  * @param[in]  conn_id        Connection ID.
  * @param[in]  client_id      Client ID of specific client module. Value is @ref T_CLIENT_ID.
  * @param[in]  uuid16         16-bit UUID.
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  */
T_GAP_CAUSE client_by_uuid_srv_discovery(uint8_t conn_id, T_CLIENT_ID client_id, uint16_t uuid16);

/**
  * @brief  Send discovery services by 128-bit UUID request.
  *
  * If sending request operation is successful, the discovering result will be returned by discover_state_cb
  * and discover_result_cb registered by @ref client_register_spec_client_cb.
  *
  * @param[in]  conn_id        Connection ID.
  * @param[in]  client_id      Client ID of specific client module. Value is @ref T_CLIENT_ID.
  * @param[in]  p_uuid128      Pointer to 128-bit UUID.
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  */
T_GAP_CAUSE client_by_uuid128_srv_discovery(uint8_t conn_id, T_CLIENT_ID client_id,
                                            uint8_t *p_uuid128);

/**
  * @brief  Send discovery relationship services request.
  *
  * If sending request operation is successful, the discovering result will be returned by discover_state_cb
  * and discover_result_cb registered by @ref client_register_spec_client_cb.
  *
  * @param[in]  conn_id        Connection ID.
  * @param[in]  client_id      Client ID of specific client module. Value is @ref T_CLIENT_ID.
  * @param[in]  start_handle   Start handle of range to be searched.
  * @param[in]  end_handle     End handle of range to be searched.
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  */
T_GAP_CAUSE client_relationship_discovery(uint8_t conn_id, T_CLIENT_ID client_id,
                                          uint16_t start_handle, uint16_t end_handle);

/**
  * @brief  Send discovery characteristics request.
  *
  * If sending request operation is successful, the discovering result will be returned by discover_state_cb
  * and discover_result_cb registered by @ref client_register_spec_client_cb.
  *
  * @param[in]  conn_id        Connection ID.
  * @param[in]  client_id      Client ID of specific client module. Value is @ref T_CLIENT_ID.
  * @param[in]  start_handle   Start handle of range to be searched.
  * @param[in]  end_handle     End handle of range to be searched.
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  */
T_GAP_CAUSE client_all_char_discovery(uint8_t conn_id, T_CLIENT_ID client_id, uint16_t start_handle,
                                      uint16_t end_handle);

/**
  * @brief  Send discovery characteristics request by 16-bit characteristic UUID.
  *
  * If sending request operation is successful, the discovering result will be returned by discover_state_cb
  * and discover_result_cb registered by @ref client_register_spec_client_cb.
  *
  * @param[in]  conn_id        Connection ID.
  * @param[in]  client_id      Client ID of specific client module. Value is @ref T_CLIENT_ID.
  * @param[in]  start_handle   Start handle of the range to be searched.
  * @param[in]  end_handle     End handle of the range to be searched.
  * @param[in]  uuid16         16-bit characteristic UUID to be searched.
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  */
T_GAP_CAUSE client_by_uuid_char_discovery(uint8_t conn_id, T_CLIENT_ID client_id,
                                          uint16_t start_handle,
                                          uint16_t end_handle, uint16_t uuid16);
/**
  * @brief  Send discovery characteristics request by 128-bit characteristic UUID.
  *
  * If sending request operation is successful, the discovering result will be returned by discover_state_cb
  * and discover_result_cb registered by @ref client_register_spec_client_cb.
  *
  * @param[in]  conn_id        Connection ID.
  * @param[in]  client_id      Client ID of specific client module. Value is @ref T_CLIENT_ID.
  * @param[in]  start_handle   Start handle of the range to be searched.
  * @param[in]  end_handle     End handle of the range to be searched.
  * @param[in]  p_uuid128      Pointer to 128-bit characteristic UUID to be searched.
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  */
T_GAP_CAUSE client_by_uuid128_char_discovery(uint8_t conn_id, T_CLIENT_ID client_id,
                                             uint16_t start_handle,
                                             uint16_t end_handle, uint8_t *p_uuid128);
/**
  * @brief  Send discovery characteristics descriptor request.
  *
  * If sending request operation is successful, the discovering result will be returned by discover_state_cb
  * and discover_result_cb registered by @ref client_register_spec_client_cb.
  *
  * @param[in]  conn_id        Connection ID.
  * @param[in]  client_id      Client ID of specific client module. Value is @ref T_CLIENT_ID.
  * @param[in]  start_handle   Start handle of the range to be searched.
  * @param[in]  end_handle     End handle of the range to be searched.
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  */
T_GAP_CAUSE client_all_char_descriptor_discovery(uint8_t conn_id, T_CLIENT_ID client_id,
                                                 uint16_t start_handle, uint16_t end_handle);

/**
  * @brief  Read characteristic by handle request.
  *
  * If sending request operation is successful, the reading result will be returned by read_result_cb
  * registered by @ref client_register_spec_client_cb.
  *
  * @param[in]  conn_id        Connection ID.
  * @param[in]  client_id      Client ID of specific client module. Value is @ref T_CLIENT_ID.
  * @param[in]  handle         Attribute handle.
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  */
T_GAP_CAUSE client_attr_read(uint8_t conn_id, T_CLIENT_ID client_id, uint16_t handle);

/**
  * @brief  Read characteristic by 16-bit UUID request.
  *
  * If sending request operation is successful, the reading result will be returned by read_result_cb
  * registered by @ref client_register_spec_client_cb.
  *
  * @param[in]  conn_id       Connection ID.
  * @param[in]  client_id     Client ID of specific client module. Value is @ref T_CLIENT_ID.
  * @param[in]  start_handle  Start handle of the range to be searched.
  * @param[in]  end_handle    End handle of the range to be searched.
  * @param[in]  uuid16        16-bit UUID.
  * @param[in]  p_uuid128     Pointer to 128-bit UUID.
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  */
T_GAP_CAUSE client_attr_read_using_uuid(uint8_t conn_id, T_CLIENT_ID client_id,
                                        uint16_t start_handle,
                                        uint16_t end_handle, uint16_t uuid16, uint8_t *p_uuid128);
/**
 * @brief  Write characteristic request.
 *
 * If sending request operation is successful, the writing result will be returned by write_result_cb
 * registered by @ref client_register_spec_client_cb.
 *
 * @param[in]  conn_id    Connection ID.
 * @param[in]  client_id  Client ID of specific client module. Value is @ref T_CLIENT_ID.
 * @param[in]  write_type Type of write.
 * @param[in]  handle     Attribute handle.
 * @param[in]  length     Length of data to be written.
                          - If write_type is @ref GATT_WRITE_TYPE_REQ, range of length is from 0 to 512.
                          - If write_type is @ref GATT_WRITE_TYPE_CMD, range of length is from 0 to (mtu_size - 3).
                          - If write_type is @ref GATT_WRITE_TYPE_SIGNED_CMD, range of length is from 0 to (mtu_size - 15).
                          - uint16_t mtu_size is acquired by @ref le_get_conn_param (@ref GAP_PARAM_CONN_MTU_SIZE, &mtu_size, conn_id).
 * @param[in]  p_data     Point to the data to be written.
 * @return The result of sending request.
 * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
 * @retval Others Sending request operation is failed.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        T_GAP_CAUSE cause = client_attr_write(conn_id, client_id, GATT_WRITE_TYPE_REQ, handle,
                                              length, p_data);
    }
    //if write_type is GATT_WRITE_TYPE_SIGNED_CMD, GAP_MSG_LE_GATT_SIGNED_STATUS_INFO will be notified to APP
    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_GATT_SIGNED_STATUS_INFO:
        APP_PRINT_INFO5("GAP_MSG_LE_GATT_SIGNED_STATUS_INFO:conn_id %d, cause 0x%x, update_local %d, local_sign_count %d,remote_sign_count %d",
                       cb_data.p_le_gatt_signed_status_info->conn_id,
                       cb_data.p_le_gatt_signed_status_info->cause,
                       cb_data.p_le_gatt_signed_status_info->update_local,
                       cb_data.p_le_gatt_signed_status_info->local_sign_count,
                       cb_data.p_le_gatt_signed_status_info->remote_sign_count);
        break;
        ...
        }
    }
 * \endcode
 */
T_GAP_CAUSE client_attr_write(uint8_t conn_id, T_CLIENT_ID client_id,
                              T_GATT_WRITE_TYPE write_type,
                              uint16_t handle, uint16_t length, uint8_t *p_data);

/**
  * @brief  Confirm from the application when receiving an indication from the server.
  * @param[in]  conn_id       Connection ID.
  * @return Operation result.
  * @retval true  Operation success.
  * @retval false Operation failure.
  */
T_GAP_CAUSE client_attr_ind_confirm(uint8_t conn_id);

/**
 * @brief Get the header point of the notification data buffer.
 *
 * This function is used to get the header buffer point of the notification command data.
 * This function can only be called in notify_ind_result_cb.
 *
 * @param[in] conn_id         Connection ID.
 * @param[in,out] pp_buffer   Pointer to the address of the buffer.
 * @param[in,out] p_offset    Pointer to the offset of the data.
 * @return Operation result.
 * @retval true  Operation success.
 * @retval false Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    uint8_t *p_data_buf;
    uint16_t data_offset;
    static T_APP_RESULT simp_ble_client_notif_ind_result_cb(uint8_t conn_id, bool notify,
                                                        uint16_t handle,
                                                        uint16_t value_size, uint8_t *p_value)
    {
        ......
        client_get_notify_data_buffer(conn_id, &p_data_buf, &data_offset);
        return APP_RESULT_NOT_RELEASE;
    }
    void release(void)
    {
        if(p_data_buf != NULL)
        {
            gap_buffer_free(p_data_buf);
            p_data_buf = NULL;
        }
    }
 * \endcode
 */
bool client_get_notify_data_buffer(uint8_t conn_id, uint8_t **pp_buffer, uint16_t *p_offset);

/**
 * @brief Send the exchange MTU request.
 *
 * If sending request operation is successful, result will be returned in one of the following ways:
 * - In the default situation, or when @ref le_gap_msg_info_way (true) has been called, APP will be notified
 *   by message @ref GAP_MSG_LE_CONN_MTU_INFO.
 * - When @ref le_gap_msg_info_way (false) has been called, APP will be notified with the callback registered
 *   by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_GAP_STATE_MSG.
 *
 * @param[in]  conn_id        Connection ID.
 *
 * @return The result of sending request.
 * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
 * @retval Others Sending request operation is failed.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
      T_GAP_CAUSE cause = client_send_exchange_mtu_req(conn_id);
    }

    void app_handle_gap_msg(T_IO_MSG *p_gap_msg)
    {
        T_LE_GAP_MSG gap_msg;
        memcpy(&gap_msg, &p_gap_msg->u.param, sizeof(p_gap_msg->u.param));

        switch (p_gap_msg->subtype)
        {
            ......
            case GAP_MSG_LE_CONN_MTU_INFO:
            {
                app_handle_conn_mtu_info_evt(gap_msg.msg_data.gap_conn_mtu_info.conn_id,
                                            gap_msg.msg_data.gap_conn_mtu_info.mtu_size);
            }
            break;
            ......
        }
    }
 * \endcode
 */
T_GAP_CAUSE client_send_exchange_mtu_req(uint8_t conn_id);

/** End of GATT_Client_Exported_Functions
  * @}
  */

/** End of GATT_CLIENT_API
  * @}
  */



#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif /* PROFILE_CLIENT_H */

