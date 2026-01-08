/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     gls_config.h
  * @brief    Glucose Service configuration file.
  * @details  Configure the optional characteristic and fields in the Glucose Service.
  * @author   bill
  * @date     2017-6-8
  * @version  v1.0
  * *************************************************************************************
  */

#ifndef _GLS_CONFIG_H_
#define _GLS_CONFIG_H_

/** @defgroup GLS Glucose Service
  * @brief Glucose Service
  * @{
  */


/** @defgroup GLS_CONFIG GLS Config
  * @brief Glucose Service configuration file
  * @{
  */


/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup GLS_Common_Exported_Macros GLS Common Exported Macros
  * @{
  */
/** @defgroup GLS_Common_DEFs GLS Common Definitions
  * @{
  */
/** @brief Glucose maximum number of records in database. */
#define GLC_RACP_MAX_NBR_OF_STORED_RECS             10
/** @} */

/** @defgroup GLS_Optional_Characteristic GLS Optional Characteristic Configuration
  * @{
  */
#define GLC_MEASUREMENT_CONTEXT_SUPPORT             1 /**< Whether to support Glucose Measurement Context characteristic.
                                                           - 0: Do not support Glucose Measurement Context characteristic.
                                                           - 1: Support Glucose Measurement Context characteristic. */
/** @} */

/** @defgroup GLS_Optional_Field GLS Optional Field
  * @brief  Glucose optional field.
  * @{
  */

/** @defgroup GLS_Measurement_Optional_Field GLS Measurement Characteristic Optional Field Configuration
  * @{
  */
#define GLC_INCLUDE_TIME_OFFSET                     1  /**< Whether Time Offset field is present. */
#define GLC_INCLUDE_CONC_TS_LOC                     1  /**< Whether Type-Sample Location field is present. */
#define GLC_INCLUDE_SS_ANNUNC                       1  /**< Whether Sensor Status Annunciation field is present. */
/** @} End of GLS_Measurement_Optional_Field*/

/** @defgroup GLS_Measurement_Context_Optional_Field GLS Measurement Context Characteristic Optional Field Configuration
  * @{
  */
#if (GLC_MEASUREMENT_CONTEXT_SUPPORT)
#define GLC_INCLUDE_CARBOHYDRATE                    1  /**< Whether Carbohydrate field is present. */
#define GLC_INCLUDE_MEAL                            1  /**< Whether Meal field is present. */
#define GLC_INCLUDE_TESTER_HEALTH                   1  /**< Whether Tester-Health field is present. */
#define GLC_INCLUDE_EXCERCISE                       1  /**< Whether Exercise Duration and Exercise Intensity fields are present. */
#define GLC_INCLUDE_MEDICATION                      1  /**< Whether Medication ID and Medication fields are present. */
#define GLC_INCLUDE_HbA1c                           1  /**< Whether HbA1c field is present. */
#define GLC_INCLUDE_EXT_FLAGS                       0  /**< Whether Extended Flags field is present. */

/** @brief Used to verify if the GLS Measurement Context Characteristic meets the requirement of supporting at least one Optional Field. */
#define GLC_MS_CTXT_INCLUDE_XXX (GLC_INCLUDE_CARBOHYDRATE + \
                                 GLC_INCLUDE_MEAL + \
                                 GLC_INCLUDE_TESTER_HEALTH + \
                                 GLC_INCLUDE_EXCERCISE + \
                                 GLC_INCLUDE_MEDICATION + \
                                 GLC_INCLUDE_HbA1c)
#if (GLC_MS_CTXT_INCLUDE_XXX == 0)
#error "Glucose measurement context characteristic value shall include at least one field, in addition to the Flags field and Sequence Number field!"
#endif
#endif
/** @} End of GLS_Measurement_Context_Optional_Field*/

/** @} */

/** @defgroup GLS_Attribute_Index GLS Attribute Index
  * @brief  Index of each characteristic in service database
  * @{
  */
#define INDEX_OFFSET0                               (0)  /**< GLS attribute index 0. */
#define GLS_CHAR_GLC_MEASUREMENT_INDEX              (INDEX_OFFSET0+0x02)  /**< Glucose Measurement characteristic index. */
#define GLS_CHAR_GLC_MEASUREMENT_CCCD_INDEX         (GLS_CHAR_GLC_MEASUREMENT_INDEX + 1)  /**< Glucose Measurement characteristic CCCD index. */

#if (GLC_MEASUREMENT_CONTEXT_SUPPORT)
#define GLS_CHAR_GLC_MEASUREMENT_CONTEXT_INDEX      (INDEX_OFFSET0+0x05)  /**< Glucose Measurement Context characteristic index. */
#define GLS_CHAR_GLC_MEASUREMENT_CONTEXT_CCCD_INDEX (GLS_CHAR_GLC_MEASUREMENT_CONTEXT_INDEX + 1)  /**< Glucose Measurement Context characteristic CCCD index. */
#endif

#define INDEX_OFFSET1                               (INDEX_OFFSET0+GLC_MEASUREMENT_CONTEXT_SUPPORT*3)  /**< GLS attribute index 1. */
#define GLS_CHAR_GLC_FEATURE_INDEX                  (INDEX_OFFSET1+0x05)  /**< Glucose Feature characteristic index. */
#define GLS_CHAR_GLC_RACP_INDEX                     (INDEX_OFFSET1+0x07)  /**< Record Access Control Point characteristic index. */
#define GLS_CHAR_GLC_RACP_CCCD_INDEX                (GLS_CHAR_GLC_RACP_INDEX + 1)  /**< Record Access Control Point characteristic CCCD index. */
/** @} */


/** @} End of GLS_Common_Exported_Macros */

/** @} End of GLS_CONFIG */

/** @} End of GLS */
#endif
