#ifndef _CSCS_CONFIG_H_
#define _CSCS_CONFIG_H_



/** @defgroup CSCS Cycling Speed and Cadence Service
  * @brief Cycling Speed and Cadence Service
  * @{
  */

/** @defgroup CSCS_CONFIG CSCS Config
  * @brief Cycling Speed and Cadence Service configuration file
  * @{
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup CSCS_Common_Exported_Macros CSCS Common Exported Macros
  * @{
  */

/** @defgroup CSCS_Char_Sup CSCS Characteristic Support Configuration
  * @{
  */
#define CSCS_MULTIPLE_SENSOR_LOCATIONS_SUPPORT   /**< Support Multiple Sensor Location feature. */
#define CSCS_SC_CONTROL_POINT_SUPPORT            /**< Support SC Control Point characteristic. */
/** @} End of CSCS_Char_Sup */

/** @defgroup CSCS_Char_Index CSCS Characteristic Index
  * @brief Index of each characteristic in CSC service database.
  * @{
  */
#define GATT_SVC_CSCS_MEASUREMENT_INDEX      2   /**< CSC Measurement characteristic index. */
#define GATT_SVC_CSCS_FEATURE_INDEX          5   /**< CSC Feature characteristic index. */
#define GATT_SVC_CSCS_SENS_LOC_INDEX         7   /**< Sensor Location characteristic index. */
#define GATT_SVC_CSCS_CTL_PNT_INDEX          9   /**< SC Control Point characteristic index. */
#define GATT_SVC_CSCS_MEAS_CCCD_INDEX        (GATT_SVC_CSCS_MEASUREMENT_INDEX + 1)    /**< CSC Measurement characteristic CCCD index. */
#define GATT_SVC_CSCS_CTL_PNT_CCCD_INDEX     (GATT_SVC_CSCS_CTL_PNT_INDEX + 1)   /**< SC Control Point characteristic CCCD index. */
/** @} End of CSCS_Char_Index */

/** @} End of CSCS_Common_Exported_Macros */

/** @} End of CSCS_CONFIG */

/** @} End of CSCS */

#endif
