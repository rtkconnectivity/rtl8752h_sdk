/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    vendor_tp_client.c
  * @brief
  * @details
  * @author  jane
  * @date    2016-02-18
  * @version v0.1
  ******************************************************************************
  */

/** Add Includes here **/
#include "trace.h"
#include <string.h>
#include "tp_client.h"
#include "tp_config.h"
#include "app_flags.h"

typedef struct
{
    T_TP_DISC_STATE   disc_state;
    uint16_t          hdl_cache[HDL_TP_CACHE_LEN];
} T_TP_LINK, *P_TP_LINK;

/** @brief  App link table */
T_TP_LINK tp_table[APP_MAX_LINKS];
static T_CLIENT_ID tp_client_id = CLIENT_PROFILE_GENERAL_ID;

static P_FUN_GENERAL_APP_CB tp_client_cb = NULL;

bool tp_client_start_discovery(uint8_t conn_id)
{
    PROFILE_PRINT_INFO0("tp_client_start_discovery");

    if (conn_id >= APP_MAX_LINKS)
    {
        PROFILE_PRINT_ERROR1("tp_client_start_discovery: failed invalid conn_id %d", conn_id);
        return false;
    }

    /* First clear handle cache. */
    memset(&tp_table[conn_id], 0, sizeof(T_TP_LINK));
    tp_table[conn_id].disc_state = DISC_TP_START;
    if (client_by_uuid_srv_discovery(conn_id, tp_client_id,
                                     GATT_UUID_VENDOR_TP_SERVICE) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    return false;
}

/**
  * @brief  Used by application, to get handle cache.
  * @param[in]  conn_id connection ID.
  * @param[in]  p_hdl_cache pointer of the handle cache table
  * @param[in]  len the length of handle cache table
  * @retval true success.
  * @retval false failed.
  */
bool tp_client_get_hdl_cache(uint8_t conn_id, uint16_t *p_hdl_cache, uint8_t len)
{
    if (conn_id >= APP_MAX_LINKS)
    {
        PROFILE_PRINT_ERROR1("tp_client_get_hdl_cache: failed invalid conn_id %d", conn_id);
        return false;
    }
    if (tp_table[conn_id].disc_state != DISC_TP_DONE)
    {
        PROFILE_PRINT_ERROR1("tp_client_get_hdl_cache: failed invalid state %d",
                             tp_table[conn_id].disc_state);
        return false;
    }
    if (len != sizeof(uint16_t) * HDL_TP_CACHE_LEN)
    {
        PROFILE_PRINT_ERROR1("tp_client_get_hdl_cache: failed invalid len %d", len);
        return false;
    }
    memcpy(p_hdl_cache, tp_table[conn_id].hdl_cache, len);
    return true;
}

/**
  * @brief  Used by application, to set handle cache.
  * @param[in]  conn_id connection ID.
  * @param[in]  p_hdl_cache pointer of the handle cache table
  * @param[in]  len the length of handle cache table
  * @retval true success.
  * @retval false failed.
  */
bool tp_client_set_hdl_cache(uint8_t conn_id, uint16_t *p_hdl_cache, uint8_t len)
{
    if (conn_id >= APP_MAX_LINKS)
    {
        PROFILE_PRINT_ERROR1("tp_client_set_hdl_cache: failed invalid conn_id %d", conn_id);
        return false;
    }
    if (tp_table[conn_id].disc_state != DISC_TP_IDLE)
    {
        PROFILE_PRINT_ERROR1("tp_client_set_hdl_cache: failed invalid state %d",
                             tp_table[conn_id].disc_state);
        return false;
    }
    if (len != sizeof(uint16_t) * HDL_TP_CACHE_LEN)
    {
        PROFILE_PRINT_ERROR1("tp_client_set_hdl_cache: failed invalid len %d", len);
        return false;
    }
    memcpy(tp_table[conn_id].hdl_cache, p_hdl_cache, len);
    tp_table[conn_id].disc_state = DISC_TP_DONE;
    return true;
}

static bool tp_client_start_char_discovery(uint8_t conn_id)
{
    uint16_t start_handle;
    uint16_t end_handle;

    APP_PRINT_INFO0("tp_client_start_tp_char_discovery");
    start_handle = tp_table[conn_id].hdl_cache[HDL_TP_SRV_START];
    end_handle = tp_table[conn_id].hdl_cache[HDL_TP_SRV_END];
    if (client_all_char_discovery(conn_id, tp_client_id, start_handle,
                                  end_handle) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    return false;
}

static bool tp_client_start_char_descriptor_discovery(uint8_t conn_id)
{
    uint16_t start_handle;
    uint16_t end_handle;

    PROFILE_PRINT_INFO0("tp_client_start_char_descriptor_discovery");
    start_handle = tp_table[conn_id].hdl_cache[HDL_TP_CT_TX_INDICATE];
    end_handle = tp_table[conn_id].hdl_cache[HDL_TP_SRV_END];
    if (client_all_char_descriptor_discovery(conn_id, tp_client_id, start_handle,
                                             end_handle) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    return false;
}

bool tp_client_write_notify_cccd(uint8_t conn_id, bool value)
{
    if (conn_id >= APP_MAX_LINKS)
    {
        PROFILE_PRINT_ERROR1("tp_client_write_notify_cccd: failed invalid conn_id %d", conn_id);
        return false;
    }

    if (tp_table[conn_id].hdl_cache[HDL_TP_DT_TX_NOTIFY_CCCD])
    {
        uint16_t handle = tp_table[conn_id].hdl_cache[HDL_TP_DT_TX_NOTIFY_CCCD];
        uint16_t length = sizeof(uint16_t);
        uint16_t cccd_bits = value ? 1 : 0;

        if (client_attr_write(conn_id, tp_client_id, GATT_WRITE_TYPE_REQ, handle,
                              length, (uint8_t *)&cccd_bits) == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
    }

    APP_PRINT_WARN0("tp_client_write_notify_cccd: Request fail! Please check!");
    return false;
}


bool tp_client_write_ind_cccd(uint8_t conn_id, bool value)
{
    if (conn_id >= APP_MAX_LINKS)
    {
        PROFILE_PRINT_ERROR1("tp_client_write_ind_cccd: failed invalid conn_id %d", conn_id);
        return false;
    }

    if (tp_table[conn_id].hdl_cache[HDL_TP_CT_TX_INDICATE_CCCD])
    {
        uint16_t handle = tp_table[conn_id].hdl_cache[HDL_TP_CT_TX_INDICATE_CCCD];
        uint16_t length = sizeof(uint16_t);
        uint16_t cccd_bits = value ? 2 : 0;

        if (client_attr_write(conn_id, tp_client_id, GATT_WRITE_TYPE_REQ, handle,
                              length, (uint8_t *)&cccd_bits) == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
    }

    APP_PRINT_WARN0("tp_client_write_ind_cccd: Request fail! Please check!");
    return false;
}

bool tp_client_write_value(uint8_t conn_id, uint16_t length, uint8_t *p_value)
{
    if (tp_table[conn_id].hdl_cache[HDL_TP_CT_RX_WRITE])
    {
        uint16_t handle = tp_table[conn_id].hdl_cache[HDL_TP_CT_RX_WRITE];

        if (client_attr_write(conn_id, tp_client_id, GATT_WRITE_TYPE_REQ, handle,
                              length, p_value) == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
    }

    APP_PRINT_WARN0("tp_client_write_value: Request fail! Please check!");
    return false;
}

bool tp_client_write_command(uint8_t conn_id, uint16_t length, uint8_t *p_value)
{
    if (tp_table[conn_id].hdl_cache[HDL_TP_DT_RX_WRITE])
    {
        uint16_t handle = tp_table[conn_id].hdl_cache[HDL_TP_DT_RX_WRITE];

        if (client_attr_write(conn_id, tp_client_id, GATT_WRITE_TYPE_CMD, handle,
                              length, p_value) == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
    }

    APP_PRINT_WARN0("tp_client_write_command: Request fail! Please check!");
    return false;
}

bool tp_client_read_prefer_param(uint8_t conn_id)
{
    if (client_attr_read_using_uuid(conn_id, tp_client_id, 0x01, 0xffff,
                                    GATT_UUID_CHAR_VENDOR_TP_CT_TX_NOTIFY_INDICATE, NULL) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }

    APP_PRINT_WARN0("tp_client_read_prefer_param: Request fail! Please check!");
    return false;
}

static void tp_client_discover_state_cb(uint8_t conn_id,  T_DISCOVERY_STATE discovery_state)
{
    bool cb_flag = false;
    T_TP_CB_DATA cb_data;
    cb_data.cb_type = TP_CLIENT_CB_TYPE_DISC_STATE;

    APP_PRINT_INFO1("tp_client_discover_state_cb: discovery_state %d", discovery_state);
    if (tp_table[conn_id].disc_state == DISC_TP_START)
    {
        uint16_t *hdl_cache;
        hdl_cache = tp_table[conn_id].hdl_cache;

        switch (discovery_state)
        {
        case DISC_STATE_SRV_DONE:
            /* Indicate that service handle found. Start discover characteristic. */
            if ((hdl_cache[HDL_TP_SRV_START] != 0)
                || (hdl_cache[HDL_TP_SRV_END] != 0))
            {
                if (tp_client_start_char_discovery(conn_id) == false)
                {
                    tp_table[conn_id].disc_state = DISC_TP_FAILED;
                    cb_flag = true;
                }
            }
            /* No Simple BLE service handle found. Discover procedure complete. */
            else
            {
                tp_table[conn_id].disc_state = DISC_TP_FAILED;
                cb_flag = true;
            }
            break;
        case DISC_STATE_CHAR_DONE:
            if (hdl_cache[HDL_TP_CT_TX_INDICATE] != 0)
            {
                if (tp_client_start_char_descriptor_discovery(conn_id) == false)
                {
                    tp_table[conn_id].disc_state = DISC_TP_FAILED;
                    cb_flag = true;
                }
            }
            else
            {
                tp_table[conn_id].disc_state = DISC_TP_FAILED;
                cb_flag = true;
            }
            break;
        case DISC_STATE_CHAR_DESCRIPTOR_DONE:
            tp_table[conn_id].disc_state = DISC_TP_DONE;
            cb_flag = true;
            break;
        case DISC_STATE_FAILED:
            tp_table[conn_id].disc_state = DISC_TP_FAILED;
            cb_flag = true;
            break;
        default:
            APP_PRINT_ERROR0("tp_handle_discover_state: Invalid Discovery State!");
            break;
        }
    }

    /* Send discover state to application if needed. */
    if (cb_flag && tp_client_cb)
    {
        cb_data.cb_content.disc_state = tp_table[conn_id].disc_state;
        (*tp_client_cb)(tp_client_id, conn_id, &cb_data);
    }
    return;
}

/**
  * @brief  Called by profile client layer, when discover result fetched.
  * @param  conn_id: connection ID.
  * @param  result_type: indicate which type of value discovered in service discovery procedure.
  * @param  result_data: value discovered.
  * @retval None
  */
static void tp_client_discover_result_cb(uint8_t conn_id,
                                         T_DISCOVERY_RESULT_TYPE result_type,
                                         T_DISCOVERY_RESULT_DATA result_data)
{
    APP_PRINT_INFO1("tp_client_discover_result_cb: result_type %d", result_type);
    if (tp_table[conn_id].disc_state == DISC_TP_START)
    {
        uint16_t handle;
        uint16_t *hdl_cache;
        hdl_cache = tp_table[conn_id].hdl_cache;

        switch (result_type)
        {
        case DISC_RESULT_SRV_DATA:
            hdl_cache[HDL_TP_SRV_START] = result_data.p_srv_disc_data->att_handle;
            hdl_cache[HDL_TP_SRV_END] = result_data.p_srv_disc_data->end_group_handle;
            break;

        case DISC_RESULT_CHAR_UUID16:
            handle = result_data.p_char_uuid16_disc_data->value_handle;
            switch (result_data.p_char_uuid16_disc_data->uuid16)
            {
            case GATT_UUID_CHAR_VENDOR_TP_CT_TX_NOTIFY_INDICATE:
                hdl_cache[HDL_TP_CT_TX_INDICATE] = handle;
                break;

            case GATT_UUID_CHAR_VENDOR_TP_CT_RX_WRITE_REQ:
                hdl_cache[HDL_TP_CT_RX_WRITE] = handle;
                break;

            case GATT_UUID_CHAR_VENDOR_TP_DT_TX_NOTIFY_INDICATE:
                hdl_cache[HDL_TP_DT_TX_NOTIFY] = handle;
                break;

            case GATT_UUID_CHAR_VENDOR_TP_DT_RX_WRITE_CMD:
                hdl_cache[HDL_TP_DT_RX_WRITE] = handle;
                break;

            default:
                /* have no intrest on this handle. */
                break;
            }

            break;

        case DISC_RESULT_CHAR_DESC_UUID16:
            /* When use client_all_char_descriptor_discovery. */
            if (result_data.p_char_desc_uuid16_disc_data->uuid16 == GATT_UUID_CHAR_CLIENT_CONFIG)
            {
                handle = result_data.p_char_desc_uuid16_disc_data->handle;
                if ((handle > hdl_cache[HDL_TP_CT_TX_INDICATE])
                    && (handle < hdl_cache[HDL_TP_CT_RX_WRITE]))
                {
                    hdl_cache[HDL_TP_CT_TX_INDICATE_CCCD] = handle;
                }
                else if ((handle > hdl_cache[HDL_TP_DT_TX_NOTIFY]) &&
                         (hdl_cache[HDL_TP_DT_TX_NOTIFY_CCCD] == 0))
                {
                    hdl_cache[HDL_TP_DT_TX_NOTIFY_CCCD] = handle;
                }
            }
            break;

        default:
            APP_PRINT_ERROR0("tp_handle_discover_result: Invalid Discovery Result Type!");
            break;
        }
    }

    return;
}

static void tp_client_write_cb(uint8_t conn_id, T_GATT_WRITE_TYPE type,
                               uint16_t handle, uint16_t cause,
                               uint8_t credits)
{
    T_TP_CB_DATA cb_data;
    cb_data.cb_type = TP_CLIENT_CB_TYPE_WRITE_RESULT;
    cb_data.cb_content.write_result.credits = credits;
    cb_data.cb_content.write_result.write_type = type;

    APP_PRINT_INFO1("tp_client_write_cb: result = 0x%x", cause);

    /* If write req success, branch to fetch value and send to application. */
    if ((handle == tp_table[conn_id].hdl_cache[HDL_TP_CT_RX_WRITE])
        || (handle == tp_table[conn_id].hdl_cache[HDL_TP_DT_RX_WRITE]))
    {
        cb_data.cb_content.write_result.type = TP_WRITE_CHAR_VALUE;
        cb_data.cb_content.write_result.cause = cause;
    }
    else if (handle == tp_table[conn_id].hdl_cache[HDL_TP_DT_TX_NOTIFY_CCCD])
    {
        cb_data.cb_content.write_result.type = TP_WRITE_NOTIFY_CCCD;
        cb_data.cb_content.write_result.cause = cause;
    }
    else if (handle == tp_table[conn_id].hdl_cache[HDL_TP_DT_TX_NOTIFY_CCCD])
    {
        cb_data.cb_content.write_result.type = TP_WRITE_IND_CCCD;
        cb_data.cb_content.write_result.cause = cause;
    }

    /* Inform application the write result. */
    if (tp_client_cb)
    {
        (*tp_client_cb)(tp_client_id, conn_id, &cb_data);
    }

    return;
}

static T_APP_RESULT tp_client_notify_ind_cb(uint8_t conn_id, bool notify_ind, uint16_t handle,
                                            uint16_t value_size, uint8_t *pValue)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;
    T_TP_CB_DATA cb_data;

    cb_data.cb_type = TP_CLIENT_CB_TYPE_NOTIF_IND_RESULT;

    if (handle == tp_table[conn_id].hdl_cache[HDL_TP_CT_TX_INDICATE])
    {
        cb_data.cb_content.notif_ind_data.notify_ind = notify_ind;
        cb_data.cb_content.notif_ind_data.value_size = value_size;
        cb_data.cb_content.notif_ind_data.pValue = pValue;
    }
    else
    {
        return app_result;
    }
    /* Inform application the notif/ind result. */
    if (tp_client_cb)
    {
        app_result = (*tp_client_cb)(tp_client_id, conn_id, &cb_data);
    }

    return app_result;
}

static void tp_client_read_result_cb(uint8_t conn_id,  uint16_t cause,
                                     uint16_t handle, uint16_t value_size, uint8_t *p_value)
{
    T_TP_CB_DATA cb_data;
    cb_data.cb_type = TP_CLIENT_CB_TYPE_READ_RESULT;

    PROFILE_PRINT_INFO2("tp_client_read_result_cb: handle 0x%x, cause 0x%x", handle, cause);
    cb_data.cb_content.read_result.cause = cause;
    cb_data.cb_content.read_result.type = TP_READ_PREFER_PARAM;
    if (cause == GAP_SUCCESS)
    {
        cb_data.cb_content.read_result.value_size = value_size;
        cb_data.cb_content.read_result.p_value = p_value;
    }

    if (tp_client_cb)
    {
        (*tp_client_cb)(tp_client_id, conn_id, &cb_data);
    }
    return;
}

static void tp_client_disc_cb(uint8_t conn_id)
{
    APP_PRINT_INFO0("tp_client_disc_cb.");
    memset(&tp_table[conn_id], 0, sizeof(T_TP_LINK));
    return;
}

const T_FUN_CLIENT_CBS TP_CLIENT_CBS =
{
    tp_client_discover_state_cb,   //!< Discovery State callback function pointer
    tp_client_discover_result_cb,  //!< Discovery result callback function pointer
    tp_client_read_result_cb,      //!< Read response callback function pointer
    tp_client_write_cb,            //!< Write result callback function pointer
    tp_client_notify_ind_cb,       //!< Notify Indicate callback function pointer
    tp_client_disc_cb              //!< Link disconnection callback function pointer
};


T_CLIENT_ID tp_client_add(P_FUN_GENERAL_APP_CB app_cb)
{
    T_CLIENT_ID client_id;
    if (false == client_register_spec_client_cb(&client_id, &TP_CLIENT_CBS))
    {
        tp_client_id = CLIENT_PROFILE_GENERAL_ID;
        APP_PRINT_ERROR0("tp_client_add_client Fail !!!");
        return tp_client_id;
    }
    tp_client_id = client_id;
    APP_PRINT_INFO1("tp_client_add_client: client ID = %d", tp_client_id);

    /* register callback for profile to inform application that some events happened. */
    tp_client_cb = app_cb;

    return client_id;
}

