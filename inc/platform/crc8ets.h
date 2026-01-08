/*
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

#if ! defined (__CRC8ETS_H)
#define        __CRC8ETS_H

/*
#if ! defined (__BASETYPE_H)
#include <basetype.h>
#endif
*/
#include <stdint.h>

/** @defgroup CRC_FCS CRC Implementation
  * @brief CRC implementation for specified polynomial
  * @{
  */

/*============================================================================*
  *                                Functions
  *============================================================================*/
/** @defgroup CRC_FCS_Exported_Functions CRC Implementation Exported Functions
    * @brief
    * @{
    */
/**
* @brief  Generate FCS field.
*
* @param[in]  p  input data pointer
* @param[in]  len  input data length
*
* @return FCS field.
*
*/
uint8_t crc8EtsGen(uint8_t *p, uint16_t len);

/**
* @brief  Check if FCS is okay.
*
* @param[in]  p  input data pointer
* @param[in]  len   input data length
* @param[in]  rfcs FCS field to validate.
*
* @return Status of validating.
* @retval true    Validation pass.
* @retval false   Validation not pass.
*
*/
uint8_t crc8EtsCheck(uint8_t *p, uint16_t len, uint8_t rfcs);
/** @} */ /* End of group CRC_FCS_Exported_Functions */

/** @} */ /* End of group CRC_FCS */

#endif /**< ! defined (__CRC8ETS_H) */
