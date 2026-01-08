/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     hts.h
  * @brief    Variables and interfaces for using Health Thermometer Service.
  * @details  Health Thermometer Service data structs and functions.
  * @author
  * @date     2017-09-20
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _HTS_SERVICE_DEF_H
#define _HTS_SERVICE_DEF_H

#ifdef __cplusplus
extern "C"  {
#endif      /* __cplusplus */

/* Add Includes here */
#include "stdint.h"
#include "profile_server.h"
#include "rtl876x.h"


/** @defgroup HTS Health Thermometer Service
  * @brief  Health Thermometer Service
   * @details

     The Health Thermometer Service exposes temperature and other data related to a thermometer used for healthcare applications.
     Applications shall register Health Thermometer Service during initialization through @ref hts_add_service function.

     The Temperature Measurement characteristic is used to send a temperature measurement.
     Applications can send a temperature measurement through @ref hts_measurement_value_indicate function.

     The Intermediate Temperature characteristic is used to send intermediate temperature values to a device
     for display purposes while the measurement is in progress.
     Applications can send intermediate temperature values through @ref hts_intermediate_temperature_value_notify function.

     The Measurement Interval characteristic is used to enable and control the interval
     between consecutive temperature measurements.
     Applications can send interval through @ref hts_measurement_interval_notify function.



  * @{
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup HTS_Exported_Macros HTS Exported Macros
  * @brief
  * @{
  */
/** @defgroup HTS_DEFs HTS Definitions
  * @{
  */
#define HTS_READ_TEMPERATURE_TYPE_VALUE                     1  /**< Temperature Type characteristic value read index. */
#define HTS_READ_MEASUREMENT_INTERVAL_VALUE                 2  /**< Measurement Interval characteristic value read index. */
#define HTS_READ_VALID_RANGE_VALUE                          3  /**< Valid Range descriptor value of Measurement Interval characteristic read index. */

#define HTS_WRITE_MEASUREMENT_INTERVAL_VALUE                1  /**< Measurement Interval characteristic value write index. */
/** End of HTS_DEFs
* @}
*/


/** @defgroup HTS_Notify_Indicate_Info HTS Notify Indicate Info
  * @brief  Parameter for enable or disable notification or indication.
  * @{
  */
#define HTS_NOTIFY_INDICATE_TEMPERATURE_MEASUREMENT_VALUE_ENABLE     1 /**< Temperature Measurement characteristic indication enabled index. */
#define HTS_NOTIFY_INDICATE_TEMPERATURE_MEASUREMENT_VALUE_DISABLE    2 /**< Temperature Measurement characteristic indication disabled index. */
#define HTS_NOTIFY_INDICATE_INTERMEDIATE_TEMPERATURE_VALUE_ENABLE    3 /**< Intermediate Temperature characteristic notification enabled index. */
#define HTS_NOTIFY_INDICATE_INTERMEDIATE_TEMPERATURE_VALUE_DISABLE   4 /**< Intermediate Temperature characteristic notification disable index. */
#define HTS_NOTIFY_INDICATE_MEASUREMENT_INTERVAL_VALUE_ENABLE        5 /**< Measurement Interval characteristic indication enabled index. */
#define HTS_NOTIFY_INDICATE_MEASUREMENT_INTERVAL_VALUE_DISABLE       6 /**< Measurement Interval characteristic indication disable index. */
/** @} */

/** @defgroup HTS_Measurement_Flag Temperature Measurement Characteristic Flag Field
  * @{
  */
#define HTS_FLAG_MEASUREMENT_UINT_BIT                       BIT0  /**< Temperature Units Flag.
                                                                       - 0: Temperature Measurement Value in units of Celsius.
                                                                       - 1: Temperature Measurement Value in units of Fahrenheit. */
#define HTS_FLAG_MEASUREMENT_TIMESTAMP_PRESENT_BIT          BIT1  /**< Time Stamp Flag.
                                                                       - 0: Time Stamp field not present.
                                                                       - 1: Time Stamp field present. */
#define HTS_FLAG_MEASUREMENT_TYPE_PRESNET_BIT               BIT2  /**< Temperature Type Flag.
                                                                       - 0: Temperature Type field not present.
                                                                       - 1: Temperature Type field present. */
/** @} */

/** End of HTS_Exported_Macros
* @}
*/

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup HTS_Exported_Types HTS Exported Types
  * @brief
  * @{
  */

/**
*  @brief  Health Thermometer parameter type
*/
typedef enum
{
    HTS_PARAM_MEASUREMENT_TEMPPARAM_FLAG = 1,  /**< Parameter type for @ref hts_set_parameter to set Temperature Measurement
                                                    characteristic Flags field. */
    HTS_PARAM_MEASUREMENT_TEMPERATUER_VALUE,   /**< Parameter type for @ref hts_set_parameter to set Temperature Measurement
                                                    characteristic Temperature Measurement Value field. */
    HTS_PARAM_MEASUREMENT_TIME_STAMP,          /**< Parameter type for @ref hts_set_parameter to set Temperature Measurement
                                                    characteristic Time Stamp field. */
    HTS_PARAM_MEASUREMENT_TEMPERATURE_TYPE,    /**< Parameter type for @ref hts_set_parameter to set Temperature Measurement
                                                    characteristic Temperature Type field. */
    HTS_PARAM_MEASUREMENT_INTERVAL,            /**< Parameter type for @ref hts_set_parameter to set Temperature Interval
                                                    characteristic value. */
    HTS_PARAM_VALID_RANGE                      /**< Parameter type for @ref hts_set_parameter to set Valid Range
                                                    descriptor value of Measurement Interval characteristic. */
} T_HTS_PARAM_TYPE;

/**
*  @brief  Health Thermometer temperature type
*/
typedef enum
{
    HTS_TEMPERATURE_TYPE_ARMPIT = 1,               /**< Armpit. */
    HTS_TEMPERATURE_TYPE_BODY,                     /**< Body (general). */
    HTS_TEMPERATURE_TYPE_EAR,                      /**< Ear (usually earlobe). */
    HTS_TEMPERATURE_TYPE_FINGER,                   /**< Finger. */
    HTS_TEMPERATURE_TYPE_GASTRO_INTESTINAL_TRACT,  /**< Gastrointestinal Tract. */
    HTS_TEMPERATURE_TYPE_MOUTH,                    /**< Mouth. */
    HTS_TEMPERATURE_TYPE_RECTUM,                   /**< Rectum. */
    HTS_TEMPERATURE_TYPE_TOE,                      /**< Toe. */
    HTS_TEMPERATURE_TYPE_TYMPANUM,                 /**< Tympanum (ear drum). */
    HTS_TEMPERATURE_TYPE_RESERVED,                 /**< Reserved for Future Use. */
} T_HTS_TEMPERATURE_TYPE;

/**
*  @brief  Health Thermometer Measurement Value Flag
*/
typedef struct
{
    uint8_t temp_value_units_bit: 1;
    uint8_t temp_time_stamp_present_bit: 1;
    uint8_t temp_type_present_bit: 1;
    uint8_t rfu: 5;
} T_HEALTH_THERMOMETER_MEASUREMENT_VALUE_FLAG;

typedef struct
{
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hours;
    uint8_t  minutes;
    uint8_t  seconds;
} T_TIME_STAMP;

typedef struct
{
    uint16_t lower_value;
    uint16_t upper_value;
} T_HTS_MEASUREMENT_INTERVAL_VALID_RANGE;

typedef struct
{
    uint8_t write_index;
    uint16_t measurement_interval;
} T_HTS_WRITE_MSG;

/**
*  @brief  Health Thermometer Service Bluetooth Host stream message data
*/
typedef union
{
    uint8_t notification_indification_index;
    uint8_t read_value_index;
    T_HTS_WRITE_MSG write;
} T_HTS_UPSTREAM_MSG_DATA;

/**
*  @brief  Health Thermometer Service callback data
*/
typedef struct
{
    T_SERVICE_CALLBACK_TYPE     msg_type;
    T_HTS_UPSTREAM_MSG_DATA     msg_data;
} T_HTS_CALLBACK_DATA;

/** End of HTS_Exported_Types
* @}
*/

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup HTS_Exported_Functions HTS Exported Functions
  * @brief
  * @{
  */

/**
  * @brief       Add Health Thermometer Service to the Bluetooth Host.
  *
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
         hts_id = hts_add_service(app_handle_profile_message);
     }
  * \endcode
  */
uint8_t hts_add_service(void *p_func);


/**
 * @brief       Set a Health Thermometer Service parameter.
 *
 * This function can be called with a HTS parameter type @ref T_HTS_PARAM_TYPE and it will set the HTS parameter.
 * The 'p_value' field must point to an appropriate data type that meets the requirements for the corresponding parameter type.
 * (For example, if required data length for parameter type is 1 octets, p_value should be cast to a pointer of uint8_t.)
 *
 * @param[in]   param_type   Health Thermometer Service parameter type: @ref T_HTS_PARAM_TYPE.
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
        uint8_t hts_flag = HTS_FLAG_MEASUREMENT_UINT_BIT | HTS_FLAG_MEASUREMENT_TIMESTAMP_PRESENT_BIT |
                           HTS_FLAG_MEASUREMENT_TYPE_PRESNET_BIT;
        T_HTS_TEMPERATURE_TYPE temperature_type = HTS_TEMPERATURE_TYPE_ARMPIT;

        hts_measure_time_stamp.seconds += hts_measurement_interval;

        hts_set_parameter(HTS_PARAM_MEASUREMENT_TEMPPARAM_FLAG, sizeof(hts_flag), &hts_flag);
        hts_set_parameter(HTS_PARAM_MEASUREMENT_TIME_STAMP, sizeof(hts_measure_time_stamp),
                          &hts_measure_time_stamp);
        hts_set_parameter(HTS_PARAM_MEASUREMENT_TEMPERATURE_TYPE, 1, &temperature_type);
    }
 * \endcode
 */

bool hts_set_parameter(T_HTS_PARAM_TYPE param_type, uint8_t len, void *p_value);

/**
 * @brief       Send measurement value indication data.
 *              Applications shall call @ref hts_set_parameter to set intermediate temperature value first,
 *              and then call this API to send the indication value.
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
        uint8_t hts_flag = HTS_FLAG_MEASUREMENT_UINT_BIT | HTS_FLAG_MEASUREMENT_TIMESTAMP_PRESENT_BIT |
                           HTS_FLAG_MEASUREMENT_TYPE_PRESNET_BIT;
        T_HTS_TEMPERATURE_TYPE temperature_type = HTS_TEMPERATURE_TYPE_ARMPIT;

        hts_measure_time_stamp.seconds += hts_measurement_interval;

        hts_set_parameter(HTS_PARAM_MEASUREMENT_TEMPPARAM_FLAG, sizeof(hts_flag), &hts_flag);
        hts_set_parameter(HTS_PARAM_MEASUREMENT_TIME_STAMP, sizeof(hts_measure_time_stamp),
                          &hts_measure_time_stamp);
        hts_set_parameter(HTS_PARAM_MEASUREMENT_TEMPERATURE_TYPE, 1, &temperature_type);

        bool ret = hts_measurement_value_indicate(conn_id, hts_id);
    }
 * \endcode
 */
bool hts_measurement_value_indicate(uint8_t conn_id, uint8_t service_id);

/**
 * @brief       Send intermediate temperature notification data.
 *              Applications shall call @ref hts_set_parameter to set intermediate temperature value first,
 *              and then call this API to send the notification value.
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
        uint8_t hts_flag = HTS_FLAG_MEASUREMENT_UINT_BIT
                           | HTS_FLAG_MEASUREMENT_TIMESTAMP_PRESENT_BIT
                           | HTS_FLAG_MEASUREMENT_TYPE_PRESNET_BIT;
        T_HTS_TEMPERATURE_TYPE temperature_type = HTS_TEMPERATURE_TYPE_ARMPIT;

        hts_set_parameter(HTS_PARAM_MEASUREMENT_TEMPPARAM_FLAG, sizeof(hts_flag), &hts_flag);
        hts_set_parameter(HTS_PARAM_MEASUREMENT_TIME_STAMP, sizeof(hts_measure_time_stamp),
                          &hts_measure_time_stamp);
        hts_set_parameter(HTS_PARAM_MEASUREMENT_TEMPERATURE_TYPE, 1, &temperature_type);

        bool ret = hts_intermediate_temperature_value_notify(conn_id, hts_id);
    }
 * \endcode
 */
bool hts_intermediate_temperature_value_notify(uint8_t conn_id, uint8_t service_id);


/**
 * @brief       Send measurement interval notification data.
 *
 *
 * @param[in]   conn_id     Connection ID.
 * @param[in]   service_id  Service ID.
 * @param[in]   seconds     Measurement interval.
 *
 * @return Operation result.
 * @retval true Operation success.
 * @retval false Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        bool ret = hts_measurement_interval_notify(conn_id, hts_id, interval);
    }
 * \endcode
 */
bool hts_measurement_interval_notify(uint8_t conn_id, uint8_t service_id, uint16_t seconds);

/** @} End of HTS_Exported_Functions */

/** @} End of HTS */


#ifdef __cplusplus
}
#endif

#endif // _HTS_SERVICE_DEF_H

