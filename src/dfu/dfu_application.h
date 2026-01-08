/*
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

#ifndef _PERIPHERAL_APPLICATION__
#define _PERIPHERAL_APPLICATION__

#ifdef __cplusplus
extern "C" {
#endif
#include "app_msg.h"
//#include <gap_le.h>
#include "profile_server.h"

void  dfu_handle_io_msg(T_IO_MSG io_driver_msg_recv);
T_APP_RESULT dfu_profile_callback(T_SERVER_ID service_id, void *p_data);
T_APP_RESULT dfu_gap_callback(uint8_t cb_type, void *p_cb_data);

#ifdef __cplusplus
}
#endif

#endif

