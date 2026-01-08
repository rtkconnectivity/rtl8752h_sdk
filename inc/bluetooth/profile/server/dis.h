/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     dis.h
  * @brief    Header file for using Device Information Service.
  * @details  DIS data structs and external functions declaration.
  * @author
  * @date
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _DEVICE_INFO_SERVICE_H_
#define _DEVICE_INFO_SERVICE_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profile_server.h"

/**
 * @defgroup DIS Device Information Service
 * @brief Device Information Service
 * @details
    The Device Information Service exposes manufacturer and/or vendor information about a device.
    This module implements the Device Information Service.
    The Device Information Service may expose one or more of the characteristics. It can be
    configured in file @ref dis_config.h.

    Applications shall register Device Information Service during initialization.

 * Sample code:
 * \code{.c}
    void app_le_profile_init(void)
    {
        dis_service_id = dis_add_service(app_profile_callback);
    }
 * \endcode

    When characteristic value was read, write or CCCD updated by remote device, the callback function will be
    called.
    Applications shall set DIS parameter value by call @ref dis_set_parameter.

 * Sample code:
 * \code{.c}
    T_APP_RESULT app_profile_callback(T_SERVER_ID service_id, void *p_data)
    {
    ...
        if (dis_service_id == service_id)
        {
            T_DIS_CALLBACK_DATA *p_dis_cb_data = (T_DIS_CALLBACK_DATA *)p_data;
            switch (p_dis_cb_data->msg_type)
            {
            case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
                {
                    if (p_dis_cb_data->msg_data.read_value_index == DIS_READ_MANU_NAME_INDEX)
                    {
                        const uint8_t dis_manufacture_name[] = "Realtek BT";
                        dis_set_parameter(DIS_PARAM_MANUFACTURER_NAME,
                                          sizeof(dis_manufacture_name),
                                          (void *)dis_manufacture_name);
                    }
                    else if (p_dis_cb_data->msg_data.read_value_index == DIS_READ_MODEL_NUM_INDEX)
                    {
                        const uint8_t dis_model_number[] = "Model Num 1.0";
                        dis_set_parameter(DIS_PARAM_MODEL_NUMBER,
                                          sizeof(dis_model_number),
                                          (void *)dis_model_number);
                    }
                    ...
                    ...

                }

                break;
            default:
                break;
            }
        }
    }
 * \endcode

 * @{
 */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup DIS_Exported_Macros DIS Exported Macros
  * @brief
  * @{
  */

/** @defgroup DIS_READ_INFO_INDEX DIS Read Information Index
  * @brief  Read characteristic value index.
  * @{
  */
#define DIS_READ_MANU_NAME_INDEX        1  /**< Manufacturer Name String characteristic read index. */
#define DIS_READ_MODEL_NUM_INDEX        2  /**< Model Number String characteristic read index. */
#define DIS_READ_SERIAL_NUM_INDEX       3  /**< Serial Number String characteristic read index. */
#define DIS_READ_HARDWARE_REV_INDEX     4  /**< Hardware Revision String characteristic read index. */
#define DIS_READ_FIRMWARE_REV_INDEX     5  /**< Firmware Revision String characteristic read index. */
#define DIS_READ_SOFTWARE_REV_INDEX     6  /**< Software Revision String characteristic read index. */
#define DIS_READ_SYSTEM_ID_INDEX        7  /**< System ID characteristic read index. */
#define DIS_READ_IEEE_CERT_STR_INDEX    8  /**< IEEE 11073-20601 Regulatory Certification Data List characteristic read index. */
#define DIS_READ_PNP_ID_INDEX           9  /**< PnP ID characteristic read index. */
/** @} */

/** @defgroup DIS_LENGTH_DEFs DIS Length Definitions
  * @{
  */
#define DIS_SYSTEM_ID_LENGTH                8  /**< Length of System ID characteristic value. */
#define DIS_PNP_ID_LENGTH                   7  /**< Length of PnP ID characteristic value. */
/** @} End of DIS_LENGTH_DEFs */

/** @} End of DIS_Exported_Macros */
/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup DIS_Exported_Types DIS Exported Types
  * @brief
  * @{
  */
/** @defgroup DIS_PARAM_TYPE DIS Parameter Type
* @brief  Type of parameters set/gotten from application.
* @{
*/
typedef enum
{
    DIS_PARAM_MANUFACTURER_NAME, /**< Parameter type for @ref dis_set_parameter to set
                                      Manufacturer Name String characteristic value. */
    DIS_PARAM_MODEL_NUMBER,      /**< Parameter type for @ref dis_set_parameter to set
                                      Model Number String characteristic value. */
    DIS_PARAM_SERIAL_NUMBER,     /**< Parameter type for @ref dis_set_parameter to set
                                      Serial Number String characteristic value. */
    DIS_PARAM_HARDWARE_REVISION, /**< Parameter type for @ref dis_set_parameter to set
                                      Hardware Revision String characteristic value. */
    DIS_PARAM_FIRMWARE_REVISION, /**< Parameter type for @ref dis_set_parameter to set
                                      Firmware Revision String characteristic value. */
    DIS_PARAM_SOFTWARE_REVISION, /**< Parameter type for @ref dis_set_parameter to set
                                      Software Revision String characteristic value. */
    DIS_PARAM_SYSTEM_ID,         /**< Parameter type for @ref dis_set_parameter to set
                                      System ID characteristic value. */
    DIS_PARAM_IEEE_DATA_LIST,    /**< Parameter type for @ref dis_set_parameter to set
                                      IEEE 11073-20601 Regulatory Certification Data List characteristic value. */
    DIS_PARAM_PNP_ID             /**< Parameter type for @ref dis_set_parameter to set
                                      PnP ID characteristic value. */
} T_DIS_PARAM_TYPE;
/** @} */

/** @defgroup DIS_Callback_Data DIS Callback Data
* @{
*/
/** @brief DIS upstream message data*/
typedef union
{
    uint8_t read_value_index;
} T_DIS_UPSTREAM_MSG_DATA;

/** @brief DIS callback data*/
typedef struct
{
    T_SERVICE_CALLBACK_TYPE     msg_type;
    uint8_t                     conn_id;
    T_DIS_UPSTREAM_MSG_DATA     msg_data;
} T_DIS_CALLBACK_DATA;
/** @} End of DIS_Callback_Data */

/** @} End of DIS_Exported_Types */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup DIS_Exported_Functions DIS Exported Functions
  * @brief
  * @{
  */

/**
 * @brief       Set a Device Information Service parameter.
 *
 * This function can be called with a DIS parameter type @ref T_DIS_PARAM_TYPE and it will set the DIS parameter.
 * The 'p_value' field must point to an appropriate data type that meets the requirements for the corresponding parameter type.
 * (For example, if required data length for parameter type is 1 octets, p_value should be cast to a pointer of uint8_t.)
 *
 * @param[in]   param_type   Device Information Service parameter type: @ref T_DIS_PARAM_TYPE.
 * @param[in]   length       Length of data to write.
 * @param[in]   p_value Pointer to data to write.
 *
 * @return Operation result.
 * @retval true Operation success.
 * @retval false Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        const uint8_t dis_manufacture_name[] = "Realtek BT";
        bool ret = dis_set_parameter(DIS_PARAM_MANUFACTURER_NAME,
                                     sizeof(dis_manufacture_name),
                                     (void *)dis_manufacture_name);
    }
 * \endcode
 */
bool dis_set_parameter(T_DIS_PARAM_TYPE param_type, uint8_t length, void *p_value);

/**
  * @brief Add Device Information Service to the Bluetooth Host.
  *
  * @param[in]   p_func  Callback when service attribute was read, write or CCCD update.
  * @return Service ID generated by the Bluetooth Host: @ref T_SERVER_ID.
  * @retval 0xFF Operation failure.
  * @retval others Service ID assigned by Bluetooth Host.
  *
  * <b>Example usage</b>
  * \code{.c}
    void profile_init()
    {
        server_init(service_num);
        dis_id = dis_add_service(app_handle_profile_message);
    }
  * \endcode
  */

T_SERVER_ID dis_add_service(void *p_func);

/** @} End of DIS_Exported_Functions */

/** @} End of DIS */


#ifdef  __cplusplus
}
#endif      /* __cplusplus */

#endif /* _DEVICE_INFO_SERVICE_H_ */
