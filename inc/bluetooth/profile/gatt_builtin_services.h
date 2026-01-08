/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     gatt_builtin_services.h
  * @brief    Header file for using built-in services, including GAP service and GATT service.
  * @details  GAPS data structures and external functions declaration.
  * @author   Jane
  * @date     2015-5-12
  * @version  v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _BUILTIN_SERVICES_H_
#define _BUILTIN_SERVICES_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include <profile_server.h>

/** @defgroup GAP_GATT_SERVICE GAP and GATT Inbox Services
  * @brief GAP and GATT inbox services
  * @{
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup GAP_GATT_SERVICE_Exported_Macros GAP and GATT Service Exported Macros
  * @brief
  * @{
  */

/** @defgroup GAPS_WRITE_PROPERTY GAP Service Write Property
  * @brief  GAP service write property.
  * @{
  */
#define GAPS_PROPERTY_WRITE_DISABLE                   0 //!< Characteristics contained in GAP Service do not support writable property.
#define GAPS_PROPERTY_WRITE_ENABLE                    1 //!< Characteristics contained in GAP Service support writable property.
/** @} */


/** @defgroup GAPS_WRITE_TYPE GAP and GATT Service Write Type
  * @brief  GAP and GATT Service Write Type.
  * @{
  */
#define GAPS_WRITE_DEVICE_NAME            1                 /**< Information for writing Device Name. The structure of p_value in callback data
                                                                 @ref T_GAPS_UPSTREAM_MSG_DATA is @ref T_LOCAL_NAME.
                                                                 When receiving this opcode, if returned value of callback is @ref APP_RESULT_SUCCESS,
                                                                 APP shall call @ref flash_save_local_name. */
#define GAPS_WRITE_APPEARANCE             2                 /**< Information for writing Appearance. The structure of p_value in callback data
                                                                 @ref T_GAPS_UPSTREAM_MSG_DATA is @ref T_LOCAL_APPEARANCE.
                                                                 When receiving this opcode, if returned value of callback is @ref APP_RESULT_SUCCESS,
                                                                 APP shall call @ref flash_save_local_appearance. */
#define GATT_SERVICE_CHANGE_CCCD_ENABLE   3                 /**< Information for enabling CCCD for Service Changed characteristic. */
#define GATT_SERVICE_CHANGE_CCCD_DISABLE  4                 /**< Information for disabling CCCD for Service Changed characteristic. */
#define GATT_SERVICE_WRITE_CLIENT_SUPPORTED_FEATURES 5      /**< Information for writing Client Supported Features characteristic. */
/** @} */

/** @defgroup GATT_Service_Client_Supported_Features_Bit GATT Service Client Supported Features Bit Field
  * @brief GATT Service Client Supported Features bit field.
  * @{
  */
#define GATTS_CLIENT_SUPPORTED_FEAUTRES_DEFAULT_VALUE       0x00 /**< Server shall not use any of the features associated with that bit when communicating with this client. */
#define GATTS_CLIENT_SUPPORTED_FEAUTRES_ROBUST_CACHING_BIT  0x01 /**< The client supports robust caching. */
#define GATTS_CLIENT_SUPPORTED_FEAUTRES_EATT_BEARER_BIT     0x02 /**< The client supports Enhanced ATT bearer. */
#define GATTS_CLIENT_SUPPORTED_FEAUTRES_MULTI_NOTIF_BIT     0x04 /**< The client supports receiving ATT_MULTIPLE_HANDLE_VALUE_NTF PDUs. */
/** @} */

/** End of GAP_GATT_SERVICE_Exported_Macros
* @}
*/

/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup GAP_GATT_SERVICE_Exported_Types GAP and GATT Service Exported Types
  * @brief
  * @{
  */

/** @brief GAPS parameter type */
typedef enum
{
    GAPS_PARAM_DEVICE_NAME = 0x00,                  /**< Device Name characteristic value. Write only. Range of value length is from 0 to (@ref GAP_DEVICE_NAME_LEN - 1). */
    GAPS_PARAM_APPEARANCE  = 0x01,                  /**< Appearance characteristic value. Write only. Size is 2 octets. Value is @ref GAP_LE_APPEARANCE_VALUES.
                                                         Default value is @ref GAP_GATT_APPEARANCE_UNKNOWN. */
    GAPS_PARAM_CENTRAL_ADDRESS_RESOLUTION = 0x02,   /**< Central Address Resolution characteristic value. Whether to support Central Address Resolution.
                                                         Write only. Size is 1 octet.
                                                         Default value is 0 (disabled). */
    GAPS_PARAM_DEVICE_NAME_PROPERTY = 0x03,         /**< Whether Device Name characteristic value can be writable. Write only. Size is 1 octet.
                                                         Value is @ref GAPS_WRITE_PROPERTY. Default value is @ref GAPS_PROPERTY_WRITE_DISABLE. */
    GAPS_PARAM_APPEARANCE_PROPERTY = 0x04,          /**< Whether Appearance characteristic value can be writable. Write only. Size is 1 octet.
                                                         Value is @ref GAPS_WRITE_PROPERTY. Default value is @ref GAPS_PROPERTY_WRITE_DISABLE. */
} T_GAPS_PARAM_TYPE;

/** @brief Builtin services data struct for notification data to application. */
typedef struct
{
    uint8_t  opcode; //!<  @ref GAPS_WRITE_TYPE.
    uint16_t len;    //!<  Length of data to be sent to APP.
    uint8_t  *p_value;  //!<  Pointer to data to be sent to APP.
} T_GAPS_UPSTREAM_MSG_DATA;

/** @brief Builtin services callback data to inform application. */
typedef struct
{
    T_SERVICE_CALLBACK_TYPE     msg_type; //!<  @ref T_SERVICE_CALLBACK_TYPE.
    uint8_t conn_id;     /**< Connection ID. */
    T_GAPS_UPSTREAM_MSG_DATA    msg_data; //!<  @ref T_GAPS_UPSTREAM_MSG_DATA.
} T_GAPS_CALLBACK_DATA;

/** End of GAP_GATT_SERVICE_Exported_Types
* @}
*/
/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup GAP_GATT_SERVICE_Exported_Functions GAP and GATT Service Exported Functions
  * @brief
  * @{
  */

/**
 * @brief  Register callback to builtin services.
 *
 * @param[in] p_func   Pointer to callback to notify APP.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        uint8_t appearance_prop = GAPS_PROPERTY_WRITE_ENABLE;
        uint8_t device_name_prop = GAPS_PROPERTY_WRITE_ENABLE;
        gaps_set_parameter(GAPS_PARAM_APPEARANCE_PROPERTY, sizeof(appearance_prop), &appearance_prop);
        gaps_set_parameter(GAPS_PARAM_DEVICE_NAME_PROPERTY, sizeof(device_name_prop), &device_name_prop);
        gatt_register_callback(gap_service_callback);
    }
    T_APP_RESULT gap_service_callback(T_SERVER_ID service_id, void *p_para)
    {
        T_APP_RESULT  result = APP_RESULT_SUCCESS;
        T_GAPS_CALLBACK_DATA *p_gap_data = (T_GAPS_CALLBACK_DATA *)p_para;
        APP_PRINT_INFO2("gap_service_callback conn_id %d msg_type %d", p_gap_data->conn_id,
                        p_gap_data->msg_type);
        if (p_gap_data->msg_type == SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE)
        {
            switch (p_gap_data->msg_data.opcode)
            {
            case GAPS_WRITE_DEVICE_NAME:
                {
                    T_LOCAL_NAME device_name;
                    memcpy(device_name.local_name, p_gap_data->msg_data.p_value, p_gap_data->msg_data.len);
                    device_name.local_name[p_gap_data->msg_data.len] = 0;
                    flash_save_local_name(&device_name);
                }
                break;

            case GAPS_WRITE_APPEARANCE:
                {
                    uint16_t appearance_val;
                    T_LOCAL_APPEARANCE appearance;

                    LE_ARRAY_TO_UINT16(appearance_val, p_gap_data->msg_data.p_value);
                    appearance.local_appearance = appearance_val;
                    flash_save_local_appearance(&appearance);
                }
                break;

            default:
                break;
            }
        }
        else if (p_gap_data->msg_type == SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION)
        {
            if (p_gap_data->msg_data.opcode == GATT_SERVICE_CHANGE_CCCD_ENABLE)
            {
                APP_PRINT_INFO0("GATT_SERVICE_CHANGE_CCCD_ENABLE");
            }
        }
        return result;
    }
 * \endcode
 */
void gatt_register_callback(void *p_func);

/**
 * @brief  Set GAP service parameter.
 *
 * This function can be called with a GAP service parameter type @ref T_GAPS_PARAM_TYPE and it will set the GAP service parameter.
 * The 'p_value' field must point to an appropriate data type that meets the requirements for the corresponding parameter type.
 * (For example, if required data length for parameter type is 2 octets, p_value should be cast to a pointer of uint16_t.)
 *
 * @param[in] param_type   GAP service parameter type @ref T_GAPS_PARAM_TYPE.
 * @param[in] length       Length of data to write.
 * @param[in] p_value      Pointer to data to write.
 * @return Operation result.
 * @retval true    Operation success.
 * @retval false   Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        uint8_t appearance_prop = GAPS_PROPERTY_WRITE_ENABLE;
        uint8_t device_name_prop = GAPS_PROPERTY_WRITE_ENABLE;
        gaps_set_parameter(GAPS_PARAM_APPEARANCE_PROPERTY, sizeof(appearance_prop), &appearance_prop);
        gaps_set_parameter(GAPS_PARAM_DEVICE_NAME_PROPERTY, sizeof(device_name_prop), &device_name_prop);
    }
 * \endcode
 */
bool gaps_set_parameter(T_GAPS_PARAM_TYPE param_type, uint8_t length, void *p_value);


/**
  * @brief  Set the preferred connection parameter of the Peripheral.
  *
  * @param[in] conn_interval_min    Value:
  *                                 - 0xXXXX: Minimum value of connection interval.
  *                                   Range: 0x0006 to 0x0C80. Units: 1.25 ms.
  *                                 - 0xFFFF: No specific value is requested.
  * @param[in] conn_interval_max    Value:
  *                                 - 0xXXXX: Maximum value of connection interval.
  *                                   Range: 0x0006 to 0x0C80. Units: 1.25 ms.
  *                                 - 0xFFFF: No specific value is requested.
  * @param[in] slave_latency        Value:
  *                                 - 0xXXXX: Peripheral latency for the connection in number of
  *                                           connection events. Range: 0x0000 to 0x01F3.
  *                                 - 0xFFFF: No specific value is requested.
  * @param[in] supervision_timeout  Value:
  *                                 - 0xXXXX: Supervision timeout for the connection.
  *                                   Range: 0x000A to 0x0C80. Units: 10 ms.
  *                                 - 0xFFFF: No specific value is requested.
  */
void gaps_set_peripheral_preferred_conn_param(uint16_t conn_interval_min,
                                              uint16_t conn_interval_max,
                                              uint16_t slave_latency,
                                              uint16_t supervision_timeout);

/**
 * @brief  Send indication for Service Changed characteristic.
 *
 * If sending request operation is successful, the sending result will be returned by callback
 * registered by @ref server_register_app_cb with event ID @ref PROFILE_EVT_SEND_DATA_COMPLETE.
 *
 * @param[in] conn_id      Connection ID.
 * @param[in] start_handle Start of Affected Attribute Handle Range.
 * @param[in] end_handle   End of Affected Attribute Handle Range.
 * @return The result of sending request.
 * @retval true Sending request operation is successful.
 * @retval false Sending request operation is failed.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        uint16_t start_handle = 0x0001;
        uint16_t end_handle = 0xFFFF;
        bool ret = gatts_service_changed_indicate(conn_id, start_handle, end_handle);
    }
 * \endcode
 */
bool gatts_service_changed_indicate(uint8_t conn_id, uint16_t start_handle, uint16_t end_handle);

/** End of GAP_GATT_SERVICE_Exported_Functions
* @}
*/

/** End of GAP_GATT_SERVICE
* @}
*/


#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif  /* _BUILTIN_SERVICES_H_ */
