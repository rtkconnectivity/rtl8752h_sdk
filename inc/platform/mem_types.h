/*
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef _MEM_TYPES_H_
#define _MEM_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup MEM_CONFIG Memory Configure
  * @{
  */

/*============================================================================*
 *                               Types
*============================================================================*/
/** @defgroup MEM_CONFIG_Exported_Types Memory Configure Exported Types
  * @{
  */

typedef enum
{
    RAM_TYPE_DATA_ON = 0,                      //!< DATA RAM TYPE.
    RAM_TYPE_BUFFER_ON = 1,                    //!< BUFFER RAM TYPE.
    RAM_TYPE_NUM = 2,                          //!< RAM TYPE NUM.
} RAM_TYPE;

/** @} */ /* End of group MEM_TYPES_Exported_Types */

/** @} */ /* End of group MEM_CONFIG */


#ifdef __cplusplus
}
#endif

#endif /* _MEM_TYPES_H_ */
