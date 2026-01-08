/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     wss.h
  * @brief    Header file for using weight scale service.
  * @details  Weight scale data types and external functions declaration.
  * @author   Vera
  * @date
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _WSS_H_
#define _WSS_H_

#ifdef __cplusplus
extern "C"  {
#endif      /* __cplusplus */


/* Add Includes here */
#include "stdint.h"
#include "profile_server.h"



/** @defgroup WSS Weight Scale Service
  * @brief Weight Scale Service
   * @details

    The Weight Scale (WS) Service exposes weight and related data from a weight scale (Server) intended for
    consumer healthcare as well as sports/fitness applications.

    Weight Scale Feature and Weight Measurement are exposed in the Weight Scale Service.
    Only one instance of each characteristic is permitted within this service.
    The Weight Scale Feature characteristic shall be used to describe the supported features of the Server.
    When read, the Weight Scale Feature characteristic returns a value that is used by a Client to determine
    the supported features of the Server.
    The Weight Measurement characteristic is used to send weight-related data to the Client.
    Included in the characteristic value are a Flags field (for showing the presence of optional fields and
    measurement units), a Weight field, and depending upon the contents of the Flags field, may include one or more optional fields.

    Applications shall register weight scale service during initialization through @ref wss_add_service function.

    Applications can set the weight scale service parameters through @ref wss_set_parameter function.

    Applications can send the weight-related data of WSS to the client through @ref wss_measurement_indicate function.

  * @{
  */


/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup WSS_Exported_Macros WSS Exported Macros
  * @brief
  * @{
  */

/** @defgroup WSS_Weight_Measurement_Char_Flags WSS Weight Measurement Characteristic Flags Field
  * @{
  */
#define WSS_FLAG_MEASUREMENT_UINT_BIT                       BIT0
/**< Measurement Units:
 *   - 0: SI (Weight and Mass in units of kilogram (kg) and Height in units of meter).
 *   - 1: Imperial (Weight and Mass in units of pound (lb) and Height in units of inch (in)). */
#define WSS_FLAG_MEASUREMENT_TIMESTAMP_PRESENT_BIT          BIT1
/**< Time Stamp present.
 *   - 0: Not present.
 *   - 1: Present.
 */
#define WSS_FLAG_MEASUREMENT_USERID_PRESNET_BIT             BIT2
/**< User ID present.
 *   - 0: Not present.
 *   - 1: Present.
 */
#define WSS_FLAG_MEASUREMENT_BMI_PRESNET_BIT                BIT3
/**< BMI and Height present.
 *   - 0: Not present.
 *   - 1: Present.
 */

/** @} */



/** @defgroup WSS_Service_Read_Info WSS Service Read Info
  * @brief  Parameter for reading characteristic value.
  * @{
  */
#define WEIGHT_SCALE_READ_FEATURE                   1   /**< Read Weight Scale Feature characteristic value index. */
/** @} End of WSS_Service_Read_Info*/



/** @defgroup WSS_Service_Notify_Indicate_Info WSS Service Notify Indicate Info
  * @brief  Parameter for enable or disable notification or indication.
  * @{
  */
#define WSS_INDICATE_WEIGHT_MEASUREMENT_ENABLE     1    /**< Weight Measurement characteristic indication enabled index. */
#define WSS_INDICATE_WEIGHT_MEASUREMENT_DISABLE    2    /**< Weight Measurement characteristic indication disabled index. */
/** @} */

/** @} End of WSS_Exported_Macros */


/*============================================================================*
 *                         Types
 *============================================================================*/

/** @defgroup WSS_Service_Exported_Types WSS Exported Types
  * @brief
  * @{
  */

/** @defgroup T_WSS_PARAM_TYPE WSS Parameter Type
  * @brief Weight scale service parameter type
  * @{
  */
typedef enum
{
    WSS_PARAM_MEASUREMENT_WEIGHTPARAM_FLAG = 0x01,  /**< Parameter type for @ref wss_set_parameter to set
                                                         Weight Measurement characteristic Flags field value.
                                                         Size is 1 octet.
                                                         Use the combination of macros defined in
                                                         @ref WSS_Weight_Measurement_Char_Flags. */
    WSS_PARAM_MEASUREMENT_WEIGHT_VALUE, /**< Parameter type for @ref wss_set_parameter to set Weight Measurement
                                             characteristic Weight field value. Size is 2 octets.
                                             - When @ref WSS_FLAG_MEASUREMENT_UINT_BIT of
                                               @ref WSS_PARAM_MEASUREMENT_WEIGHTPARAM_FLAG is 0,
                                               value is in kilograms with resolution 0.005.
                                             - When @ref WSS_FLAG_MEASUREMENT_UINT_BIT of
                                               @ref WSS_PARAM_MEASUREMENT_WEIGHTPARAM_FLAG is 1,
                                               value is in pounds with resolution 0.01. */
    WSS_PARAM_MEASUREMENT_TIME_STAMP, /**< Parameter type for @ref wss_set_parameter to set Weight Measurement
                                           characteristic Time Stamp field value. Value is @ref T_WSS_TIME_STAMP. */
    WSS_PARAM_MEASUREMENT_USERID, /**< Parameter type for @ref wss_set_parameter to set Weight Measurement
                                       characteristic User ID field value. Size is 1 octet. */
    WSS_PARAM_MEASUREMENT_BMI,  /**< Parameter type for @ref wss_set_parameter to set Weight Measurement
                                     characteristic BMI field value. Size is 2 octets.
                                     Unit is 0.1 kg/m2, or org.bluetooth.unit.kilogram_per_square_metre
                                     represented values: M = 1, d = -1, b = 0. */
    WSS_PARAM_MEASUREMENT_HEIGHT_VALUE, /**< Parameter type for @ref wss_set_parameter to set Weight Measurement
                                             characteristic Height field value. Size is 2 octets.
                                             - When @ref WSS_FLAG_MEASUREMENT_UINT_BIT of
                                               @ref WSS_PARAM_MEASUREMENT_WEIGHTPARAM_FLAG is 0,
                                               value is in meters with a resolution of 0.001.
                                             - When @ref WSS_FLAG_MEASUREMENT_UINT_BIT of
                                               @ref WSS_PARAM_MEASUREMENT_WEIGHTPARAM_FLAG is 0,
                                               value is in inches with a resolution of 0.1. */
    WSS_PARAM_FEATURE_READ_CHAR_VAL, /**< Parameter type for @ref wss_set_parameter to set Weight Scale Feature
                                          characteristic value. Size is 4 octets.
                                          Value is @ref T_WEIGHT_SCALE_FEATURE_VALUE. */
} T_WSS_PARAM_TYPE;
/** @} End of T_WSS_PARAM_TYPE */

/** @defgroup T_WSS_UPSTREAM_MSG_DATA WSS Upstream MSG Data
  * @brief Weight scale service callback message content.
  * @{
  */
typedef union
{
    uint8_t notification_indication_index; //!<  @ref WSS_Service_Notify_Indicate_Info.
    uint8_t read_value_index; //!<  @ref WSS_Service_Read_Info.
} T_WSS_UPSTREAM_MSG_DATA;
/** @} End of T_WSS_UPSTREAM_MSG_DATA */

/** @defgroup T_WSS_CALLBACK_DATA WSS Callback Data
  * @brief Weight scale service data to inform application.
  * @{
  */
typedef struct
{
    T_SERVICE_CALLBACK_TYPE     msg_type;
    T_WSS_UPSTREAM_MSG_DATA    msg_data;
} T_WSS_CALLBACK_DATA;
/** @} End of T_WSS_CALLBACK_DATA */


/** @defgroup T_WEIGHT_SCALE_FEATURE_VALUE Weight Scale Feature Value
  * @brief Weight Scale Feature Value.
  * @{
  */
typedef struct
{
    uint32_t time_stamp_supported: 1;
    uint32_t multiple_users_supported: 1;
    uint32_t bmi_supported: 1;
    uint32_t weight_measurement_resolution: 4;
    uint32_t height_measurement_resolution: 3;
    uint32_t rfu: 22;
} T_WEIGHT_SCALE_FEATURE_VALUE;
/** @} End of T_WEIGHT_SCALE_FEATURE_VALUE */

/** @defgroup T_WEIGHT_MEASUREMENT_VALUE_FLAG Weight Measurement Value Flag
  * @brief Weight Measurement Value Flag.
  * @{
  */
typedef struct
{
    uint8_t measurement_units: 1;
    uint8_t time_stamp_present: 1;
    uint8_t user_id_present: 1;
    uint8_t bmi_and_height_present: 1;
    uint8_t rfu: 4;
} T_WEIGHT_MEASUREMENT_VALUE_FLAG;
/** @} End of T_WEIGHT_MEASUREMENT_VALUE_FLAG */

/** @defgroup T_WSS_TIME_STAMP WSS Time Stamp
  * @brief Weight Scale Feature Time Stamp.
  * @{
  */
typedef struct
{
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hours;
    uint8_t  minutes;
    uint8_t  seconds;
} T_WSS_TIME_STAMP;
/** @} End of T_WSS_TIME_STAMP */

/** @defgroup T_WSS_MEASUREMENT WSS Measurement
  * @brief Weight Measurement Value.
  * @{
  */
typedef struct
{
    T_WEIGHT_MEASUREMENT_VALUE_FLAG flag;
    uint16_t weight_si_value;
    uint16_t weight_imperial_value;
    T_WSS_TIME_STAMP time_stamp;
    uint8_t user_id;
    uint16_t bmi;
    uint16_t height_si_value;
    uint16_t height_imperial_value;
} T_WSS_MEASUREMENT;
/** @} End of T_WSS_MEASUREMENT */

/** @} End of WSS_Service_Exported_Types */


/*============================================================================*
 *                         Functions
 *============================================================================*/

/** @defgroup WSS_Service_Exported_Functions WSS Exported Functions
  * @brief
  * @{
  */

/**
  * @brief       Add Weight Scale service to the Bluetooth Host.
  *
  *
  * @param[in]   p_func  Callback when service attribute is read, written, or CCCD updated.
  * @return Service ID generated by the Bluetooth Host: @ref T_SERVER_ID.
  * @retval 0xFF Operation failure.
  * @retval Others Service ID assigned by Bluetooth Host.
  *
  * <b>Example usage</b>
  * \code{.c}
     void profile_init()
     {
         server_init(service_num);
         wss_id = wss_add_service(app_handle_profile_message);
     }
  * \endcode
  */
T_SERVER_ID wss_add_service(void *p_func);

/**
 * @brief       Set a Weight Scale service parameter.
 *
 * This function can be called with a WSS parameter type @ref T_WSS_PARAM_TYPE and it will set the WSS parameter.
 * The 'p_value' field must point to an appropriate data type that meets the requirements for the corresponding parameter type.
 * (For example, if required data length for parameter type is 1 octets, p_value should be cast to a pointer of uint8_t.)
 *
 * @param[in]   param_type   Weight Scale service parameter type: @ref T_WSS_PARAM_TYPE.
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
        bool ret = wss_set_parameter(WSS_PARAM_FEATURE_READ_CHAR_VAL, 4, &weight_feature);
    }
 * \endcode
 */
bool wss_set_parameter(T_WSS_PARAM_TYPE param_type, uint8_t len, void *p_value);


/**
 * @brief       Send measurement indication data.
 *
 *
 * @param[in]   conn_id  Connection ID.
 * @param[in]   service_id  Service ID.
 * @return Operation result.
 * @retval true Operation success.
 * @retval false Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        uint8_t wm_flag = WSS_FLAG_MEASUREMENT_UINT_BIT
                          | WSS_FLAG_MEASUREMENT_TIMESTAMP_PRESENT_BIT
                          | WSS_FLAG_MEASUREMENT_USERID_PRESNET_BIT
                          | WSS_FLAG_MEASUREMENT_BMI_PRESNET_BIT;

        wss_set_parameter(WSS_PARAM_MEASUREMENT_WEIGHTPARAM_FLAG, sizeof(wm_flag), &wm_flag);
        wss_set_parameter(WSS_PARAM_MEASUREMENT_TIME_STAMP, sizeof(wss_measure_time_stamp),
                          &wss_measure_time_stamp);
        wss_measurement_indicate(conn_id, wss_id);
    }
 * \endcode
 */
bool wss_measurement_indicate(uint8_t conn_id, T_SERVER_ID service_id);
/** @} End of WSS_Service_Exported_Functions */

/** @} End of WSS */



#ifdef __cplusplus
}
#endif

#endif /* _WSS_H_ */
