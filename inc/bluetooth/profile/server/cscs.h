/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     cscs.h
  * @brief    Variables and interfaces for Cycling Speed and Cadence Service.
  * @details  CSC Service data structs and functions.
  * @author   ethan_su
  * @date     2017-10-13
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _CSCS_SERVICE_DEF_H
#define _CSCS_SERVICE_DEF_H

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profile_server.h"
#include "cscs_config.h"


/** @defgroup CSCS Cycling Speed and Cadence Service
  * @brief  Cycling Speed and Cadence Service
   * @details

    The Cycling Speed and Cadence (CSC) Service exposes speed-related data and/or
    cadence-related data while using the Cycling Speed and Cadence sensor (Server).
    CSC Measurement, CSC Measurement Client Characteristic Configuration descriptor and CSC Feature characteristic are mandatory
    exposed in the Cycling Speed and Cadence Service.
    If the Multiple Sensor Locations feature is supported, CSC Feature will be mandatory, otherwise optional.
    If at least one SC Control Point procedure is supported, SC Control Point and SC Control Point Client
    Characteristic Configuration Descriptor will be mandatory, otherwise excluded.

    The CSC Measurement characteristic is used to send speed-related data and/or cadence-related data.
    The CSC Feature characteristic shall be used to describe the supported features of the Server.
    The Sensor Location characteristic of the device may be used to describe the physical location of the Server when correctly fitted.

    If the SC Control Point is supported, profiles utilizing this service are required to ensure that the Client configures the
    SC Control Point characteristic for indications (i.e. via the Client Characteristic Configuration descriptor) at the first connection.

    Applications shall register Cycling Speed and Cadence Service during initialization through @ref cscs_add_service function.

    Applications can set the Cycling Speed and Cadence parameters through @ref cscs_set_parameter function.

    Applications can send the measurement value through @ref cscs_meas_value_notify function.

  * @{
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup CSCS_Exported_Constants CSCS Exported Macros
  * @brief
  * @{
  */

/** @defgroup CSCS_Optional_Features CSC Optional Features
  * @brief CSC Service optional features bit mask
  * @{
  */

/** @defgroup CSCS_Measurement_Flags CSC Measurement Characteristic Flags
  * @brief CSC Measurement characteristic Flags present bit mask
  * @{
  */
#define CSCS_INC_WHEEL_REVOL_MASK            (1)     /**< Wheel Revolution Data Present. */
#define CSCS_INC_CRANK_REVOL_MASK            (1<<1)  /**< Crank Revolution Data Present. */
#define CSCS_INC_ALL_PRESENTS                (0x03)  /**< All flags are present. */
/** @} */

/** @defgroup CSCS_Features CSC Features Characteristic Bits
  * @brief CSC supported CSC Features characteristic bit mask
  * @{
  */
#define CSCS_SUPPORT_WHEEL_REVOL_MASK        (1)     /**< Wheel Revolution Data Supported. */
#define CSCS_SUPPORT_CRANK_REVOL_MASK        (1<<1)  /**< Crank Revolution Data Supported. */
#define CSCS_SUPPORT_MULTI_SENSOR_MASK       (1<<2)  /**< Multiple Sensor Locations Supported. */
#define CSCS_ALL_FEATURE_SUPPORTED           (0x07)  /**< All feature bits are supported. */
/** @} */

/** @defgroup CSCS_Sensor_Location Sensor Location Characteristic Bits
  * @brief CSC supported Sensor Location characteristic bit mask
  * @{
  */
#define CSCS_SENS_LOC_OTHER_MASK             (1)       /**< Other. */
#define CSCS_SENS_LOC_TOP_OF_SHOE_MASK       (1<<1)    /**< Top of shoe. */
#define CSCS_SENS_LOC_IN_SHOE_MASK           (1<<2)    /**< In shoe. */
#define CSCS_SENS_LOC_HIP_MASK               (1<<3)    /**< Hip. */
#define CSCS_SENS_LOC_FRONT_WHEEL_MASK       (1<<4)    /**< Front Wheel. */
#define CSCS_SENS_LOC_LEFT_CRANK_MASK        (1<<5)    /**< Left Crank. */
#define CSCS_SENS_LOC_RIGHT_CRANK_MASK       (1<<6)    /**< Right Crank. */
#define CSCS_SENS_LOC_LEFT_PEDAL_MASK        (1<<7)    /**< Left Pedal. */
#define CSCS_SENS_LOC_RIGHT_PEDAL_MASK       (1<<8)    /**< Right Pedal. */
#define CSCS_SENS_LOC_FRONT_HUB_MASK         (1<<9)    /**< Front Hub. */
#define CSCS_SENS_LOC_REAR_DROPOUT_MASK      (1<<10)   /**< Rear Dropout. */
#define CSCS_SENS_LOC_CHAINSTAY_MASK         (1<<11)   /**< Chainstay. */
#define CSCS_SENS_LOC_REAR_WHEEL_MASK        (1<<12)   /**< Rear Wheel. */
#define CSCS_SENS_LOC_REAR_HUB_MASK          (1<<13)   /**< Rear Hub. */
#define CSCS_SENS_LOC_CHEST_MASK             (1<<14)   /**< Chest. */
#define CSCS_ALL_SENS_LOC_SUPPORTED          (0x7FFF)  /**< All sensor location are supported. */
/** @} */

/** @} End of CSCS_Optional_Features */


/** @defgroup CSCS_Upstream_Message CSC Upstream Message
  * @brief  Upstream message used to inform application.
  * @{
  */

/** @defgroup CSCS_Read_Info CSC Read Info
  * @brief  Parameter for reading characteristic value.
  * @{
  */
#define CSCS_READ_CSCS_FEATURE                               1  /**< Request type for reading CSC Feature characteristic value.
                                                                    When receiving this type, APP may use @ref cscs_set_parameter (@ref CSCS_PARAM_CSCS_FEATURE, len, &p_data)
                                                                    to set the CSC Feature characteristic value. */
#define CSCS_READ_SENSOR_LOCATION                            2  /**< Request type for reading Sensor Location characteristic value.
                                                                    When receiving this type, APP may use @ref cscs_set_parameter (@ref CSCS_PARAM_SENSOR_LOC, len, &p_data)
                                                                    to set the Sensor Location characteristic value. */
/** @} */

/** @defgroup CSCS_Notify_Indicate_Info CSC Notify Indicate Info
  * @brief  Parameter for enable or disable notification or indication.
  * @{
  */
#define CSCS_NOTIFY_INDICATE_MEASUREMENT_ENABLE              1  /**< CSC Measurement characteristic notification enabled index. */
#define CSCS_NOTIFY_INDICATE_MEASUREMENT_DISABLE             2  /**< CSC Measurement characteristic notification disabled index. */
#define CSCS_NOTIFY_INDICATE_SC_CP_ENABLE                    3  /**< SC Control Point characteristic indication enabled index. */
#define CSCS_NOTIFY_INDICATE_SC_CP_DISABLE                   4  /**< SC Control Point characteristic indication disabled index. */
/** @} */

/** @} End of CSCS_Upstream_Message */

/** @defgroup CSCS_Control_Point CSC Control Point
  * @{
  */

/** @defgroup CSCS_Control_Point_OpCodes CSC Control Point OpCodes
  * @brief  Control Point OpCodes
  * @{
  */
#define CSCS_CP_OPCODE_RESERVED              0x00  /**< Reserved for Future Use. */
#define CSCS_CP_OPCODE_SET_CUMULATIVE        0x01  /**< Set Cumulative Value. */
#define CSCS_CP_OPCODE_UPDATE_SENS_LOC       0x03  /**< Update Sensor Location. */
#define CSCS_CP_OPCODE_REQ_SENS_LOC_LIST     0x04  /**< Request Supported Sensor Locations. */
#define CSCS_CP_OPCODE_RSP_CODE              0x10  /**< Response Code. */
/** @} */

/** @defgroup CSCS_Control_Point_Response_Codes CSC Control Point Response Codes
  * @brief  Control Point Response Codes
  * @{
  */
#define CSCS_CP_RSPCODE_RESERVED             0x00  /**< Reserved For Future Use. */
#define CSCS_CP_RSPCODE_SUCCESS              0x01  /**< Success. */
#define CSCS_CP_RSPCODE_OPCODE_UNSUPPORT     0x02  /**< Op Code not supported. */
#define CSCS_CP_RSPCODE_INVALID_PARAMETER    0x03  /**< Invalid Operand. */
#define CSCS_CP_RSPCODE_OPERATION_FAILED     0x04  /**< Operation Failed. */
/** @} */

///@cond
/** @brief  Judge CSC Control Point operation is available or not. */
#define CSCS_CTL_PNT_OPERATE_ACTIVE(x)                     \
    ( (x == CSCS_CP_OPCODE_SET_CUMULATIVE)  || \
      (x == CSCS_CP_OPCODE_UPDATE_SENS_LOC)  || \
      (x == CSCS_CP_OPCODE_REQ_SENS_LOC_LIST) || \
      (x == CSCS_CP_OPCODE_RSP_CODE) )
///@endcond

/** @} End of CSCS_Control_Point */

/** @defgroup CSCS_Related_Def CSC Service Related Definitions
  * @{
  */

#define CSCS_ERR_PROC_ALREADY_IN_PROG        0x80  /**< Error code: Procedure Already In Progress. */
#define CSCS_ERR_CCCD_IMPROPERLY_CFG         0x81  /**< Error code: Client Characteristic Configuration
                                                       Descriptor Improperly Configured. */

/** @brief  Maximum octets of CSC Measurement data. */
#define CSCS_MAX_MEASUREMENT_VALUE           11
/** @brief  Maximum octets of CSC Control Point data. */
#define CSCS_MAX_CTL_PNT_VALUE               18
/** @} End of CSCS_Related_Def */

/** @} End of CSCS_Exported_Constants */



/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup CSCS_Exported_Types CSCS Exported Types
  * @brief
  * @{
  */

/* Add all public types here */

/** @defgroup CSCS_TYPES_DEFs CSC Service Types Definitions
  * @{
  */
/**
*  @brief CSC Service parameter type
*/
typedef enum
{
    CSCS_PARAM_CSCS_FEATURE = 0x01,       /**< Parameter type for @ref cscs_set_parameter to set CSC Feature characteristic value. */
    CSCS_PARAM_INC_FLAG = 0x11,           /**< Parameter type for @ref cscs_set_parameter or @ref cscs_get_parameter
                                              to set or get CSC Measurement characteristic Flags field value. */
    CSCS_PARAM_WHEEL_REVOL,               /**< Parameter type for @ref cscs_set_parameter or @ref cscs_get_parameter to set or get
                                              Cumulative Wheel Revolutions field value in the Wheel Revolution Data field
                                              of CSC Measurement characteristic. */
    CSCS_PARAM_WHEEL_EVT_TIME,            /**< Parameter type for @ref cscs_set_parameter or @ref cscs_get_parameter to set or get
                                              Last Wheel Event Time field value in the Wheel Revolution Data field
                                              of CSC Measurement characteristic. */
    CSCS_PARAM_CRANK_REVOL,               /**< Parameter type for @ref cscs_set_parameter or @ref cscs_get_parameter to set or get
                                              Cumulative Crank Revolutions field value in the Crank Revolution Data field
                                              of CSC Measurement characteristic. */
    CSCS_PARAM_CRANK_EVT_TIME,            /**< Parameter type for @ref cscs_set_parameter or @ref cscs_get_parameter to set or get
                                              Last Crank Event Time field value in the Crank Revolution Data field
                                              of CSC Measurement characteristic. */
    CSCS_PARAM_CTL_PNT_PROG_CLR = 0x17,   /**< Parameter type for @ref cscs_set_parameter to clear SC Control Point characteristic progress flag. */
    CSCS_PARAM_SENSOR_LOC = 0x21,         /**< Parameter type for @ref cscs_set_parameter or @ref cscs_get_parameter
                                              to set or get Sensor Location characteristic value. */
} T_CSCS_PARAM_TYPE;

/**
 * @brief CSC sensor locations.
 *
 * All CSC sensor locations defined in spec.
*/
typedef enum
{
    CSCS_SENSOR_LOC_OTHER            = 0,    /**< Other. */
    CSCS_SENSOR_LOC_TOP_OF_SHOE      = 1,    /**< Top of shoe. */
    CSCS_SENSOR_LOC_IN_SHOE          = 2,    /**< In shoe. */
    CSCS_SENSOR_LOC_HIP              = 3,    /**< Hip. */
    CSCS_SENSOR_LOC_FRONT_WHEEL      = 4,    /**< Front Wheel. */
    CSCS_SENSOR_LOC_LEFT_CRANK       = 5,    /**< Left Crank. */
    CSCS_SENSOR_LOC_RIGHT_CRANK      = 6,    /**< Right Crank. */
    CSCS_SENSOR_LOC_LEFT_PEDAL       = 7,    /**< Left Pedal. */
    CSCS_SENSOR_LOC_RIGHT_PEDAL      = 8,    /**< Right Pedal. */
    CSCS_SENSOR_LOC_FRONT_HUB        = 9,    /**< Front Hub. */
    CSCS_SENSOR_LOC_REAR_DROPOUT     = 10,   /**< Rear Dropout. */
    CSCS_SENSOR_LOC_CHAINSTAY        = 11,   /**< Chainstay. */
    CSCS_SENSOR_LOC_REAL_WHEEL       = 12,   /**< Rear Wheel. */
    CSCS_SENSOR_LOC_REAL_HUB         = 13,   /**< Rear Hub. */
    CSCS_SENSOR_LOC_CHEST            = 14,   /**< Chest. */
    CSCS_SENSOR_LOC_MAX                      /**< Maximum number of sensor locations. */
} T_CSCS_SENSOR_LOCATION;

/** @brief CSC measurement data, variable length during connection, maximum can reach 11 octets */
typedef struct
{
    uint8_t    cur_length;          /**<  Length of current CSC measurement data. */
    uint8_t    value[CSCS_MAX_MEASUREMENT_VALUE];/**<  Value of current CSC measurement data. */
} T_CSCS_MEASUREMENT;

/**
 * @brief CSC Control Point data, variable length during connection, maximum can reach 17 octets.
 *
 * CSC Control Point data used to store the Control Point Command received from the client.
*/
typedef struct
{
    uint8_t    cur_length;            /**<  Length of current CSC Control Point data. */
    uint8_t
    value[CSCS_MAX_CTL_PNT_VALUE];            /**<  Value of current CSC Control Point data. */
} T_CSCS_CONTROL_POINT;
/** @} End of CSCS_TYPES_DEFs */

/** @defgroup CSCS_Callback_Data CSC Callback Data
  * @brief CSC data struct for notification data to application.
  * @{
  */
typedef union
{
    uint8_t  sensor_location_value;
    uint32_t cumulative_value;
} T_CSCS_CP_PARAMETER;

typedef struct
{
    uint8_t opcode; //!<  @ref CSCS_Control_Point_OpCodes.
    T_CSCS_CP_PARAMETER cp_parameter;
} T_CSCS_WRITE_MSG;

typedef union
{
    uint8_t notification_indification_index; //!<  @ref CSCS_Notify_Indicate_Info.
    uint8_t read_value_index; //!<  @ref CSCS_Read_Info.
    T_CSCS_WRITE_MSG write;
} T_CSCS_UPSTREAM_MSG_DATA;

/** CSC Service data to inform application. */
typedef struct
{
    T_SERVICE_CALLBACK_TYPE     msg_type;
    T_CSCS_UPSTREAM_MSG_DATA    msg_data;
} T_CSCS_CALLBACK_DATA;
/** @} */

/** @} End of CSCS_Exported_Types */


/*============================================================================*
 *                         Functions
 *============================================================================*/

/** @defgroup CSCS_Exported_Functions CSCS Exported Functions
  * @brief
  * @{
  */


/**
  * @brief       Add cycling Speed and Cadence Service to the Bluetooth Host.
  *
  *
  * @param[in]   p_func  Callback when service attribute was read, written or CCCD updated.
  * @return Service ID generated by the Bluetooth Host: @ref T_SERVER_ID.
  * @retval 0xFF Operation failure.
  * @retval Others Service ID assigned by Bluetooth Host.
  *
  * <b>Example usage</b>
  * \code{.c}
     void profile_init()
     {
         server_init(service_num);
         cscs_id = cscs_add_service(app_handle_profile_message);
     }
  * \endcode
  */
T_SERVER_ID cscs_add_service(void *p_func);

/**
 * @brief       Set a cycling Speed and Cadence Service parameter.
 *
 * This function can be called with a CSCS parameter type @ref T_CSCS_PARAM_TYPE and it will set the CSCS parameter.
 * The 'p_value' field must point to an appropriate data type that meets the requirements for the corresponding parameter type.
 * (For example, if required data length for parameter type is 1 octets, p_value should be cast to a pointer of uint8_t.)
 *
 * @param[in]   param_type   Cycling Speed and Cadence Service parameter type: @ref T_CSCS_PARAM_TYPE.
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
        bool ret = cscs_set_parameter(CSCS_PARAM_CTL_PNT_PROG_CLR, 0, NULL);
    }
 * \endcode
 */
bool cscs_set_parameter(T_CSCS_PARAM_TYPE param_type,  uint8_t len, void *p_value);

/**
 * @brief   Get a cycling speed and cadence parameter.
 *
 * This function can be called with a cycling speed and cadence parameter type and it will get a
 *          cycling speed and cadence parameter. Cycling Speed and Cadence Service parameters are defined in @ref T_CSCS_PARAM_TYPE.
 *
 * @param[in]   param_type Cycling speed and cadence parameter type: @ref T_CSCS_PARAM_TYPE.
 * @param[in,out]  p_value Pointer to the location to get the parameter value.  This is dependent on
 *                      the parameter type and will be cast to the appropriate
 *                      data type (For example, if the data type of param is uint16_t, p_value will be cast to
 *                      a pointer of uint16_t).
 *
 * @return Operation result.
 * @retval true Operation success.
 * @retval false Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        bool ret = cscs_get_parameter(CSCS_PARAM_WHEEL_REVOL, &cscs_cul_value);
    }
 * \endcode
 */
bool cscs_get_parameter(T_CSCS_PARAM_TYPE param_type, void *p_value);


/**
 * @brief       Send measurement value notification data.
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
        bool op_result = cscs_meas_value_notify(conn_id, cscs_id);
    }
 * \endcode
 */
bool cscs_meas_value_notify(uint8_t conn_id, T_SERVER_ID service_id);



/**
 * @brief       Send control point indication.
 *
 *
 * @param[in]   conn_id     Connection ID.
 * @param[in]   service_id  Service ID.
 * @param[in]   op_code     Opcode.
 * @param[in]   rsp_code    Response code.
 * @return Operation result.
 * @retval true Operation success.
 * @retval false Operation failure.
 *
 */
bool cscs_ctl_pnt_indicate(uint8_t conn_id, T_SERVER_ID service_id, uint8_t op_code,
                           uint8_t rsp_code);




/** @} End of CSCS_Exported_Functions */

/** @} End of CSC */


#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif  /* _CSCS_DEF_H */

