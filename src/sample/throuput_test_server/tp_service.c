#include <string.h>
#include "trace.h"
#include "tp_service.h"
#include "gap.h"
#include "os_sched.h"

T_SERVER_ID vendor_tp_service_id;

static P_FUN_SERVER_GENERAL_CB pfn_vendor_tp_service_cb = NULL;

const T_ATTRIB_APPL vendor_tp_service_tbl[] =
{
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),  /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_VENDOR_TP_SERVICE),      /* service UUID */
            HI_WORD(GATT_UUID_VENDOR_TP_SERVICE)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    },
    /*  <<charateristic CT_TX >> */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                         /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_INDICATE | GATT_CHAR_PROP_WRITE)    /* characteristic properties */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                         /* type_value */
            LO_WORD(CT_TX),
            HI_WORD(CT_TX)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)                            /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                 /* flags */
        {                                         /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* permissions */
    },
    /*  <<charateristic CT_RX >> */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                         /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_WRITE                    /* characteristic properties */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                         /* type_value */
            LO_WORD(CT_RX),
            HI_WORD(CT_RX)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)                            /* permissions */
    },
    /*  <<charateristic DT_TX >> */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                         /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_NOTIFY | GATT_CHAR_PROP_WRITE)             /* characteristic properties */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                         /* type_value */
            LO_WORD(DT_TX),
            HI_WORD(DT_TX)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)                            /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                 /* flags */
        {                                         /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* permissions */
    },
    /*  <<charateristic DT_RX >> */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                         /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_WRITE_NO_RSP                    /* characteristic properties */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                         /* type_value */
            LO_WORD(DT_RX),
            HI_WORD(DT_RX)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)                            /* permissions */
    },

};


/**
 * @brief write characteristic data from service.
*/
T_APP_RESULT vendor_tp_service_attr_write_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                             uint16_t attr_index, T_WRITE_TYPE write_type,
                                             uint16_t length, uint8_t *p_value, P_FUN_WRITE_IND_POST_PROC *p_write_ind_post_proc)
{
    T_TP_CALLBACK_DATA callback_data;
    T_APP_RESULT cause = APP_RESULT_SUCCESS;

    if (attr_index == VENDOR_TP_SERVICE_CHAR_CT_RX_INDEX && write_type == WRITE_REQUEST)
    {
        callback_data.conn_id = conn_id;
        callback_data.index = VENDOR_TP_SERVICE_CHAR_CT_RX_INDEX;
        callback_data.write_data.p_value = p_value;
        callback_data.write_data.value_len = length;

        if (pfn_vendor_tp_service_cb)
        {
            pfn_vendor_tp_service_cb(service_id, (void *)&callback_data);
        }
    }
    else if (attr_index == VENDOR_TP_SERVICE_CHAR_DT_RX_INDEX && write_type == WRITE_WITHOUT_RESPONSE)
    {
        callback_data.conn_id = conn_id;
        callback_data.index = VENDOR_TP_SERVICE_CHAR_DT_RX_INDEX;
        callback_data.write_data.p_value = p_value;
        callback_data.write_data.value_len = length;

        if (pfn_vendor_tp_service_cb)
        {
            pfn_vendor_tp_service_cb(service_id, (void *)&callback_data);
        }

    }
    else
    {
        APP_PRINT_ERROR2("vendor_tp_service_attr_write_cb error attr_index = 0x%x length=%d", attr_index,
                         length);
        cause = APP_RESULT_ATTR_NOT_FOUND;
    }
    return cause;
}

#include "os_sched.h"
bool ind_pending_flag;

bool vendor_tp_service_indication(uint8_t conn_id, T_SERVER_ID service_id, void *p_value,
                                  uint16_t length)
{
    uint8_t *p_data = (uint8_t *)p_value;
    uint16_t data_len = length;

    uint8_t pending_num = 0;
    while (ind_pending_flag && (pending_num < 10))
    {
        os_delay(50);
        APP_PRINT_INFO0("vendor_tp_service_indication pending");
        pending_num++;
    }

    APP_PRINT_INFO0("vendor_tp_service_indication");
    ind_pending_flag = true;
    return server_send_data(conn_id, service_id, VENDOR_TP_SERVICE_CHAR_CT_TX_INDEX, p_data, data_len,
                            GATT_PDU_TYPE_INDICATION);
}

bool vendor_tp_service_notification(uint8_t conn_id, T_SERVER_ID service_id, void *p_value,
                                    uint16_t length)
{
    uint8_t *p_data = (uint8_t *)p_value;
    uint16_t data_len = length;
    APP_PRINT_INFO0("vendor_tp_service_notification");
    return server_send_data(conn_id, service_id, VENDOR_TP_SERVICE_CHAR_DT_TX_INDEX, p_data, data_len,
                            GATT_PDU_TYPE_NOTIFICATION);
}

/**
 * @brief update CCCD bits from stack.
*/
void vendor_tp_service_cccd_update_cb(uint8_t conn_id, T_SERVER_ID service_id, uint16_t index,
                                      uint16_t cccbits)
{
    T_TP_CALLBACK_DATA callback_data;

    APP_PRINT_INFO2("vendor_tp_service_cccd_update_cb index = %d cccbits %x", index, cccbits);

    if (index == VENDOR_TP_SERVICE_CHAR_CT_TX_CCCD_INDEX)
    {
        callback_data.conn_id = conn_id;
        callback_data.index = VENDOR_TP_SERVICE_CHAR_CT_TX_CCCD_INDEX;
        callback_data.cccd_data.cccbits = cccbits;

        if (pfn_vendor_tp_service_cb)
        {
            pfn_vendor_tp_service_cb(service_id, (void *)&callback_data);
        }
    }
    else if (index == VENDOR_TP_SERVICE_CHAR_DT_TX_CCCD_INDEX)
    {
        callback_data.conn_id = conn_id;
        callback_data.index = VENDOR_TP_SERVICE_CHAR_DT_TX_CCCD_INDEX;
        callback_data.cccd_data.cccbits = cccbits;

        if (pfn_vendor_tp_service_cb)
        {
            pfn_vendor_tp_service_cb(service_id, (void *)&callback_data);
        }
    }
    else
    {
        APP_PRINT_ERROR1("vendor_tp_service_cccd_update_cb error attr_index = 0x%x ", index);
    }
}


/**
 * @brief Simple ble Service Callbacks.
*/
const T_FUN_GATT_SERVICE_CBS vendor_tp_service_cbs =
{
    NULL,                               // Read callback function pointer
    vendor_tp_service_attr_write_cb,    // Write callback function pointer
    vendor_tp_service_cccd_update_cb    // CCCD update callback function pointer
};



/**
  * @brief add Simple BLE service to application.
  */
T_SERVER_ID vendor_tp_service_add(void *p_func)
{
    if (false == server_add_service(&vendor_tp_service_id,
                                    (uint8_t *)vendor_tp_service_tbl,
                                    sizeof(vendor_tp_service_tbl),
                                    vendor_tp_service_cbs))
    {
        APP_PRINT_ERROR1("vendor_tp_service_add: service_id %d", vendor_tp_service_id);
        vendor_tp_service_id = 0xff;
        return vendor_tp_service_id;
    }

    pfn_vendor_tp_service_cb = (P_FUN_SERVER_GENERAL_CB)p_func;
    return vendor_tp_service_id;
}

