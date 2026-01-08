/*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     hrs.h
  * @brief    Variables and interfaces for using Heart Rate Service.
  * @details  Heart Rate Service data structs and functions.
  * @author
  * @date     2017-09-21
  * @version  v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _HRS_SERVICE_DEF_H
#define _HRS_SERVICE_DEF_H

#ifdef __cplusplus
extern "C"  {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profile_server.h"
#include "hrs_define.h"

/** @defgroup HRS Heart Rate Service
  * @brief  Heart Rate Service
  * @details

    The Heart Rate Service exposes heart rate and other data related to a heart rate sensor intended for fitness applications.

    Applications shall register heart rate service during initialization through @ref hrs_add_service function.

    The Heart Rate Measurement characteristic is used to send a heart rate measurement. Included in the characteristic are a Flags field (for showing the presence of optional fields and features
    supported), a heart rate measurement value field and, depending upon the contents of the Flags field, an Energy Expended field and an RR-Interval field. The RR-Interval represents the time between
    two consecutive R waves in an Electrocardiogram (ECG) waveform.
    Applications can send heart rate measurement value through @ref hrs_heart_rate_measurement_value_notify function.

    The Body Sensor Location characteristic of the device is used to describe the intended location of the heart rate measurement for the device.
    The value of the Body Sensor Location characteristic is static while in a connection.

    The Heart Rate Control Point characteristic is used to enable a Client to write control points to a Server to control behavior.
    Support for this characteristic is mandatory if the Server supports the Energy Expended feature.

    Applications can set heart rate measurement parameters and the location of the heart rate measurement for the device through @ref hrs_set_parameter function.

  * @{
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup HRS_Exported_Macros HRS Exported Macros
  * @brief
  * @{
  */

/** @defgroup HRS_Notify_Indicate_Info HRS Notify Indicate Info
  * @brief  Parameter for enabling or disabling notification or indication.
  * @{
  */
#define HRS_NOTIFY_INDICATE_MEASUREMENT_VALUE_ENABLE        1
/**< Heart Rate Measurement characteristic notification enabled index. */
#define HRS_NOTIFY_INDICATE_MEASUREMENT_VALUE_DISABLE       2
/**< Heart Rate Measurement characteristic notification disabled index. */
/** @} */

/** End of HRS_Exported_Macros
* @}
*/



/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup HRS_Exported_Types HRS Exported Types
  * @brief
  * @{
  */

typedef enum
{
    HRS_HEART_RATE_MEASUREMENT_PARAM_FLAG = 0x01,
    /**< Parameter type for @ref hrs_set_parameter to set Heart Rate Measurement characteristic
     *   Flags field value. Size is 1 octet. Value is @ref T_HEART_RATE_MEASUREMENT_VALUE_FLAG. */
    HRS_HEART_RATE_MEASUREMENT_PARAM_MEASUREMENT_VALUE,
    /**< Parameter type for @ref hrs_set_parameter to set Heart Rate Measurement characteristic
     *   Heart Rate Measurement Value field value. Size is 2 octets.
     *   Unit: org.bluetooth.unit.period.beats_per_minute.
     *   - When heart_rate_value_format_bit of @ref HRS_HEART_RATE_MEASUREMENT_PARAM_FLAG is set to 0,
     *     value is 8 bit resolution.
     *   - When heart_rate_value_format_bit of @ref HRS_HEART_RATE_MEASUREMENT_PARAM_FLAG is set to 1,
     *     value is 16 bit resolution. */
    HRS_HEART_RATE_MEASUREMENT_PARAM_ENERGY_EXPENDED,
    /**< Parameter type for @ref hrs_set_parameter to set Heart Rate Measurement characteristic
     *   Energy Expended field value. Size is 2 octets.
     *   Unit: org.bluetooth.unit.energy.joule. Present if energy_expended_status_bit
     *   of @ref HRS_HEART_RATE_MEASUREMENT_PARAM_FLAG is set to 1. */
    HRS_HEART_RATE_MEASUREMENT_PARAM_RR_INTERVAL,
    /**< Parameter type for @ref hrs_set_parameter to set Heart Rate Measurement characteristic
     *   RR-interval field value. Size is N * 2 octets.
     *   N: Number of RR-Interval sub-fields. Present if rr_interval_bit
     *   of @ref HRS_HEART_RATE_MEASUREMENT_PARAM_FLAG is set to 1. */
    HRS_BODY_SENSOR_LOCATION_PARAM_VALUE = 0x10,
    /**< Parameter type for @ref hrs_set_parameter to set Body Sensor Location characteristic value.
     *   Size is 1 octet. Value is @ref HRS_Sensor_Location. */
} T_HRS_PARAM_TYPE;

typedef struct
{
    T_HRS_HEART_RATE_CP_OPCODE opcode;
} T_HRS_WRITE_MSG;

typedef union
{
    uint8_t notification_indification_index;
    uint8_t read_value_index;
    T_HRS_WRITE_MSG write;
} T_HRS_UPSTREAM_MSG_DATA;

typedef struct
{
    T_SERVICE_CALLBACK_TYPE     msg_type;
    T_HRS_UPSTREAM_MSG_DATA    msg_data;
} T_HRS_CALLBACK_DATA;
/** End of HRS_Exported_Types
* @}
*/

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup HRS_Exported_Functions HRS Exported Functions
  * @brief
  * @{
  */

/**
 * @brief       Add heart rate service to the Bluetooth Host.
 *
 *
 * @param[in]   p_func  Callback when service attribute was read, written, or CCCD updated.
 * @return Service ID generated by the Bluetooth Host: @ref T_SERVER_ID.
 * @retval 0xFF Operation failure.
 * @retval others Service ID assigned by Bluetooth Host.
 *
 * <b>Example usage</b>
 * \code{.c}
    void profile_init()
    {
        server_init(service_num);
        hrs_id = hrs_add_service(app_handle_profile_message);
    }
 * \endcode
 */
uint8_t hrs_add_service(void *p_func);


/**
 * @brief       Set a heart rate service parameter.
 *
 * This function can be called with a HRS parameter type @ref T_HRS_PARAM_TYPE and it will set the HRS parameter.
 * The 'p_value' field must point to an appropriate data type that meets the requirements for the corresponding parameter type.
 * (For example: if required data length for parameter type is 1 octets, p_value should be cast to a pointer of uint8_t.)
 *
 * @param[in]   param_type   Health rate service parameter type: @ref T_HRS_PARAM_TYPE.
 * @param[in]   len       Length of data to write.
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
        T_HEART_RATE_MEASUREMENT_VALUE_FLAG flag;
        flag.heart_rate_value_format_bit = 1;
        flag.sensor_contact_status_bits = 3;
        if (p_parse_value->param_count >= 1)
        {
            flag.sensor_contact_status_bits = p_parse_value->dw_param[1];
        }

        flag.energy_expended_status_bit = 1;
        flag.rr_interval_bit = 1;
        flag.rfu = 0;

        hrs_set_parameter(HRS_HEART_RATE_MEASUREMENT_PARAM_FLAG, 1, &flag);
    }
 * \endcode
 */

bool hrs_set_parameter(T_HRS_PARAM_TYPE param_type, uint8_t len, void *p_value);

/**
 * @brief       Send heart rate measurement value notification data.
 *              Applications shall call @ref hrs_set_parameter to set heart rate measurement value first,
 *              and the call this api to send the notification value.
 *
 * @param[in]   conn_id  Connection ID.
 * @param[in]   service_id  Service ID.
 *
 * @return Operation result.
 * @retval true Operation success.
 * @retval false Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        bool result = hrs_heart_rate_measurement_value_notify(conn_id, service_id);
    }
 * \endcode
 */
bool hrs_heart_rate_measurement_value_notify(uint8_t conn_id, T_SERVER_ID service_id);

/** @} End of HRS_Exported_Functions */

/** @} End of HRS */


#ifdef __cplusplus
}
#endif

#endif /* _HRS_SERVICE_DEF_H */

