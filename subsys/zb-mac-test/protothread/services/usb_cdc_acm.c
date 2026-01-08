/**************************************************************************//**
 * @file      usb_cdc_acm.c
 * @brief     Source file for usb cdc acm implementation
 * @author    felix
 * @version   V1.00
 * @date      2025-01-17
 *
 * @note
 *
 ******************************************************************************
 *
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

//#include <trace.h>
#include "protothread.h"
#include "platform_port.h"
#include "dbg_printf.h"
#include "shell.h"
#include "strproc.h"
#include "mac_test_common.h"

#include "usb_cdc_acm.h"
#include <string.h>

#if TEST_USB_CDC_ACM
/* ========= Realtek USB stack ========= */
#include "usb_spec20.h"
#include "usb_dm.h"
#include "usb_dev_driver.h"
#include "usb_cdc_driver.h"
#include "cdc.h"
#include "usb_isr.h"

/* ========= OS primitives (platform provided) ========= */
#include "os_sync.h"

/* ========= Compile-time constants ========= */

/* USB identity (default) */
#define CDC_ACM_VID            0x0BDA // Realtek VID
#define CDC_ACM_PID            0x8777
#define CDC_ACM_BCD_DEVICE     0x0200
#define CDC_ACM_BCD_CDC        0x0110

#define STR_MANUFACTURER_DEF   "Realtek"
#define STR_PRODUCT_DEF        "USB CDC-ACM"
#define STR_SERIAL_DEF         "0001"

/* Interfaces */
#define IF_CTRL                0
#define IF_DATA                1

/* Endpoints */
#define EP_INT_IN              0x81
#define EP_BULK_IN             0x82
#define EP_BULK_OUT            0x02

/* Packet sizes */
#define FS_BULK_MPS            64
#define HS_BULK_MPS            512

/* Internal pipe depth (not user-configurable) */
#define BULK_IN_PENDING        4
#define BULK_OUT_PENDING       2

/* ========= Internal state ========= */

static struct
{
    bool inited;
    bool ready;

    usb_cdc_acm_rx_cb_t rx_cb;
    void *rx_ctx;

    void *tx_mutex;
    void *tx_sem;

    void *inst_ctrl;
    void *inst_data;

    void *pipe_in;
    void *pipe_out;

    uint16_t bulk_mps;

    const char *str_manu;
    const char *str_prod;
    const char *str_ser;

} g;

/* ========= USB descriptors ========= */

typedef enum
{
    STRID_NONE = 0,
    STRID_MANUFACTURER,
    STRID_PRODUCT,
    STRID_SERIAL,
} str_id_t;

static T_USB_DEVICE_DESC dev_desc =
{
    .bLength            = sizeof(T_USB_DEVICE_DESC),
    .bDescriptorType    = USB_DESC_TYPE_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0x02,
    .bMaxPacketSize0    = 64,
    .idVendor           = CDC_ACM_VID,
    .idProduct          = CDC_ACM_PID,
    .bcdDevice          = CDC_ACM_BCD_DEVICE,
    .iManufacturer      = STRID_MANUFACTURER,
    .iProduct           = STRID_PRODUCT,
    .iSerialNumber      = STRID_SERIAL,
    .bNumConfigurations = 1,
};

static T_STRING strings[] =
{
    { STRID_MANUFACTURER, (char *)STR_MANUFACTURER_DEF },
    { STRID_PRODUCT, (char *)STR_PRODUCT_DEF      },
    { STRID_SERIAL, (char *)STR_SERIAL_DEF       },
    { STRID_NONE,         NULL },
};

static T_STRING_TAB string_tab =
{
    .language = 0x0409,
    .strings  = strings,
};

static T_STRING_TAB *string_tabs[] =
{
    &string_tab,
    NULL,
};

static T_USB_CONFIG_DESC cfg_desc =
{
    .bLength         = sizeof(T_USB_CONFIG_DESC),
    .bDescriptorType = USB_DESC_TYPE_CONFIG,
    .wTotalLength    = 0xFFFF,
    .bNumInterfaces  = 2,
    .bConfigurationValue = 1,
    .iConfiguration = 0,
    .bmAttributes    = USB_ATTRIBUTE_ALWAYS_ONE | USB_ATTRIBUTE_REMOTEWAKEUP,
    .bMaxPower       = 250,
};

static T_USB_INTERFACE_DESC if_ctrl =
{
    sizeof(T_USB_INTERFACE_DESC), USB_DESC_TYPE_INTERFACE,
    IF_CTRL, 0, 1,
    USB_CLASS_CODE_COMM, USB_CDC_SUBCLASS_ACM, 0, 0
};

static T_CDC_HEADER_FUNC_DESC cdc_hdr =
{
    sizeof(T_CDC_HEADER_FUNC_DESC), USB_DESCTYPE_CLASS_INTERFACE, 0, CDC_ACM_BCD_CDC
};

static T_CM_FUNC_DESC cm_desc =
{
    sizeof(T_CM_FUNC_DESC), USB_DESCTYPE_CLASS_INTERFACE,
    USB_CDC_SUBCLASS_CM, 0x03, IF_DATA
};

static T_CDC_ACM_FUNC_DESC acm_desc =
{
    sizeof(T_CDC_ACM_FUNC_DESC), USB_DESCTYPE_CLASS_INTERFACE,
    USB_CDC_SUBCLASS_ACM, 0x02
};

static T_CDC_UNION_FUNC_DESC union_desc =
{
    sizeof(T_CDC_UNION_FUNC_DESC), USB_DESCTYPE_CLASS_INTERFACE,
    0x06, IF_CTRL, IF_DATA
};

static T_USB_ENDPOINT_DESC int_in_fs =
{
    sizeof(T_USB_ENDPOINT_DESC), USB_DESC_TYPE_ENDPOINT,
    EP_INT_IN, USB_EP_TYPE_INT, 64, 1
};

static T_USB_ENDPOINT_DESC int_in_hs =
{
    sizeof(T_USB_ENDPOINT_DESC), USB_DESC_TYPE_ENDPOINT,
    EP_INT_IN, USB_EP_TYPE_INT, 64, 4
};

static T_USB_INTERFACE_DESC if_data =
{
    sizeof(T_USB_INTERFACE_DESC), USB_DESC_TYPE_INTERFACE,
    IF_DATA, 0, 2,
    USB_CLASS_CDC_DATA, 0, 0, 0
};

static T_USB_ENDPOINT_DESC bulk_in_fs =
{
    sizeof(T_USB_ENDPOINT_DESC), USB_DESC_TYPE_ENDPOINT,
    EP_BULK_IN, USB_EP_TYPE_BULK, FS_BULK_MPS, 0
};

static T_USB_ENDPOINT_DESC bulk_in_hs =
{
    sizeof(T_USB_ENDPOINT_DESC), USB_DESC_TYPE_ENDPOINT,
    EP_BULK_IN, USB_EP_TYPE_BULK, HS_BULK_MPS, 0
};

static T_USB_ENDPOINT_DESC bulk_out_fs =
{
    sizeof(T_USB_ENDPOINT_DESC), USB_DESC_TYPE_ENDPOINT,
    EP_BULK_OUT, USB_EP_TYPE_BULK, FS_BULK_MPS, 0
};

static T_USB_ENDPOINT_DESC bulk_out_hs =
{
    sizeof(T_USB_ENDPOINT_DESC), USB_DESC_TYPE_ENDPOINT,
    EP_BULK_OUT, USB_EP_TYPE_BULK, HS_BULK_MPS, 0
};

static void *ctrl_descs_fs[] =
{
    &if_ctrl, &cdc_hdr, &cm_desc, &acm_desc, &union_desc, &int_in_fs, NULL
};

static void *ctrl_descs_hs[] =
{
    &if_ctrl, &cdc_hdr, &cm_desc, &acm_desc, &union_desc, &int_in_hs, NULL
};

static void *data_descs_fs[] =
{
    &if_data, &bulk_in_fs, &bulk_out_fs, NULL
};

static void *data_descs_hs[] =
{
    &if_data, &bulk_in_hs, &bulk_out_hs, NULL
};

/* ========= Callbacks ========= */
static void cdc_set_line_coding_cb(void)
{
}

static void cdc_get_line_coding_cb(void)
{
}

static void cdc_set_control_line_state_cb(uint16_t wValue)
{
    (void)wValue;
}

static long unsigned int tx_done_cb(void *handle, void *buf, long unsigned int len, int status)
{
    (void)handle; (void)buf; (void)len; (void)status;
    os_sem_give(g.tx_sem);
    return 0;
}

static long unsigned int rx_cb(void *handle, void *buf, long unsigned int len, int status)
{
    (void)handle; (void)status;

    if (g.rx_cb && buf && len)
    {
        g.rx_cb((const uint8_t *)buf, (uint16_t)len, g.rx_ctx);
    }
    return 0;
}

static bool usb_speed_cb(unsigned char speed)
{
    g.bulk_mps = (speed == USB_SPEED_HIGH) ? HS_BULK_MPS : FS_BULK_MPS;

    T_USB_CDC_DRIVER_ATTR attr =
    {
        .zlp = 1,
        .high_throughput = 0,
        .congestion_ctrl = CDC_DRIVER_CONGESTION_CTRL_DROP_CUR,
        .mtu = g.bulk_mps,
    };

    if (!g.pipe_in)
        g.pipe_in = usb_cdc_driver_data_pipe_open(
                        EP_BULK_IN, attr, BULK_IN_PENDING, tx_done_cb);

    if (!g.pipe_out)
        g.pipe_out = usb_cdc_driver_data_pipe_open(
                         EP_BULK_OUT, attr, BULK_OUT_PENDING, rx_cb);

    g.ready = true;
    return 0;
}

/* ========= Public API ========= */

bool usb_cdc_acm_init(const usb_cdc_acm_config_t *cfg)
{
    if (!cfg || !cfg->rx_cb || g.inited)
    {
        return false;
    }

    memset(&g, 0, sizeof(g));
    g.rx_cb  = cfg->rx_cb;
    g.rx_ctx = cfg->rx_cb_ctx;

    g.str_manu = cfg->manufacturer ? cfg->manufacturer : STR_MANUFACTURER_DEF;
    g.str_prod = cfg->product      ? cfg->product      : STR_PRODUCT_DEF;
    g.str_ser  = cfg->serial       ? cfg->serial       : STR_SERIAL_DEF;

    strings[0].s = (char *)g.str_manu;
    strings[1].s = (char *)g.str_prod;
    strings[2].s = (char *)g.str_ser;

    os_mutex_create(&g.tx_mutex);
    os_sem_create(&g.tx_sem, "cdc_tx", 0, 1);
    usb_spd_cb_register(usb_speed_cb);
    T_USB_CORE_CONFIG core =
    {
        .speed = USB_SPEED_FULL,
        .class_set = { 0 },
    };
    usb_dm_core_init(core);
    usb_dev_driver_dev_desc_register(&dev_desc);
    usb_dev_driver_cfg_desc_register(&cfg_desc);
    usb_dev_driver_string_desc_register(string_tabs);
    g.inst_ctrl = usb_cdc_driver_inst_alloc();
    usb_cdc_driver_if_desc_register(g.inst_ctrl, ctrl_descs_hs, ctrl_descs_fs);
    g.inst_data = usb_cdc_driver_inst_alloc();
    usb_cdc_driver_if_desc_register(g.inst_data, data_descs_hs, data_descs_fs);
    T_LINE_CODING lc = { 115200, 0, 0, 8 };
    usb_cdc_driver_linecoding(g.inst_ctrl, lc);
    T_USB_CDC_DRIVER_CBS cbs = {0};
    cbs.pfnSetLineCondigCb        = cdc_set_line_coding_cb;
    cbs.pfnGetLineCondigCb        = cdc_get_line_coding_cb;
    cbs.pfnSetControlLineStateCb  = cdc_set_control_line_state_cb;
    usb_cdc_driver_cbs_register(g.inst_ctrl, &cbs);
    usb_cdc_driver_init();

    g.inited = true;
    //DBG_DIRECT("usb_cdc_acm_init");
    return true;
}

bool usb_cdc_acm_start(void)
{
    if (!g.inited)
    {
        return false;
    }

    usb_isr_set_priority(2);
    usb_dm_start(false);
    //DBG_DIRECT("usb_cdc_acm_start");
    return true;
}

bool usb_cdc_acm_stop(void)
{
    if (!g.inited)
    {
        return false;
    }

    usb_dm_stop();
    g.ready = false;
    return true;
}

bool usb_cdc_acm_is_ready(void)
{
    return g.inited && g.ready && g.pipe_in;
}

static uint8_t g_acm_tx_buf[512] __ALIGNED(4);
bool usb_cdc_acm_send(const uint8_t *data, uint16_t len)
{
    if (!usb_cdc_acm_is_ready() || !data || len == 0)
    {
        return false;
    }

    os_mutex_take(g.tx_mutex, 0xFFFFFFFF);

    if (len > 512)
    {
        len = 512;
    }

    mac_memcpy(g_acm_tx_buf, data, len);

    uint16_t off = 0;
    while (off < len)
    {
        uint16_t n = (len - off > g.bulk_mps) ? g.bulk_mps : (len - off);

        usb_cdc_driver_data_pipe_send(g.pipe_in, (void *)&g_acm_tx_buf[off], n);
        os_sem_take(g.tx_sem, 0xFFFFFFFF);
        off += n;
    }

    os_mutex_give(g.tx_mutex);
    return true;
}
#endif /*TEST_USB_CDC_ACM*/
