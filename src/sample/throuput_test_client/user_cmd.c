/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      user_cmd.c
   * @brief     User defined test commands.
   * @details  User command interfaces.
   * @author    jane
   * @date      2017-06-06
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */
/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <string.h>
#include <trace.h>
#include <profile_client.h>
#include <gap_bond_le.h>
#include <gap_scan.h>
#include <user_cmd.h>
#include <gap.h>
#include <gap_conn_le.h>
#include <link_mgr.h>
#include <gaps_client.h>
#include <tp_test_client_app.h>


/** @defgroup  TP_CLIENT_CMD Throughput Client User Command
    * @brief This file handles Throughput Client User Command.
    * @{
    */
/*============================================================================*
 *                              Variables
 *============================================================================*/
/** @brief User command interface data, used to parse the commands from Data UART. */
T_USER_CMD_IF    user_cmd_if;

/*============================================================================*
 *                              Functions
 *============================================================================*/
/**
 * @brief Show all devices connecting status
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "showcon",
        "showcon\n\r",
        "Show all devices connecting status\n\r",
        cmd_showcon
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_showcon(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t conn_id;
    T_GAP_CONN_INFO conn_info;
    for (conn_id = 0; conn_id < APP_MAX_LINKS; conn_id++)
    {
        if (le_get_conn_info(conn_id, &conn_info))
        {
            data_uart_print("ShowCon conn_id %d state 0x%x role %d\r\n", conn_id,
                            conn_info.conn_state, conn_info.role);
            data_uart_print("RemoteBd = [%02x:%02x:%02x:%02x:%02x:%02x] type = %d\r\n",
                            conn_info.remote_bd[5], conn_info.remote_bd[4],
                            conn_info.remote_bd[3], conn_info.remote_bd[2],
                            conn_info.remote_bd[1], conn_info.remote_bd[0],
                            conn_info.remote_bd_type);
        }
    }
    data_uart_print("active link num %d,  idle link num %d\r\n",
                    le_get_active_link_num(), le_get_idle_link_num());
    return (RESULT_SUCESS);
}

/**
 * @brief LE connection param update request
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "conupdreq",
        "conupdreq [conn_id] [interval_min] [interval_max] [latency] [supervision_timeout]\n\r",
        "LE connection param update request\r\n\
        sample: conupdreq 0 0x30 0x40 0 500\n\r",
        cmd_conupdreq
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_conupdreq(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    T_GAP_CAUSE cause;
    uint8_t  conn_id = p_parse_value->dw_param[0];
    uint16_t conn_interval_min = p_parse_value->dw_param[1];
    uint16_t conn_interval_max = p_parse_value->dw_param[2];
    uint16_t conn_latency = p_parse_value->dw_param[3];
    uint16_t supervision_timeout = p_parse_value->dw_param[4];


    cause = le_update_conn_param(conn_id,
                                 conn_interval_min,
                                 conn_interval_max,
                                 conn_latency,
                                 supervision_timeout,
                                 2 * (conn_interval_min - 1),
                                 2 * (conn_interval_max - 1)
                                );
    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Disconnect to remote device
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "disc",
        "disc [conn_id]\n\r",
        "Disconnect to remote device\n\r",
        cmd_disc
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_disc(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t conn_id = p_parse_value->dw_param[0];
    T_GAP_CAUSE cause;
    cause = le_disconnect(conn_id);
    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Config authentication mode
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "authmode",
        "authmode [auth_flags] [io_cap] [sec_enable] [oob_enable]\n\r",
        "Config authentication mode\r\n\
        [auth_flags]:authentication req bit field: bit0-(bonding), bit2-(MITM), bit3-(SC)\r\n\
        [io_cap]:set io Capabilities: 0-(display only), 1-(display yes/no), 2-(keyboard noly), 3-(no IO), 4-(keyboard display)\r\n\
        [sec_enable]:Start smp pairing procedure when connected: 0-(disable), 1-(enable)\r\n\
        [oob_enable]:Enable oob flag: 0-(disable), 1-(enable)\r\n\
        sample: authmode 0x5 2 1 0\n\r",
        cmd_authmode
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_authmode(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    T_GAP_CAUSE cause;
    uint8_t  auth_pair_mode = GAP_PAIRING_MODE_PAIRABLE;
    uint16_t auth_flags = GAP_AUTHEN_BIT_BONDING_FLAG;
    uint8_t  auth_io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
    uint8_t  oob_enable = false;
    uint8_t  auth_sec_req_enable = false;
    uint16_t auth_sec_req_flags = GAP_AUTHEN_BIT_BONDING_FLAG;

    if (p_parse_value->param_count > 0)
    {
        auth_flags = p_parse_value->dw_param[0];
        auth_sec_req_flags = p_parse_value->dw_param[0];
    }
    if (p_parse_value->param_count > 1)
    {
        auth_io_cap = p_parse_value->dw_param[1];
    }
    if (p_parse_value->param_count > 2)
    {
        auth_sec_req_enable = p_parse_value->dw_param[2];
    }
    if (p_parse_value->param_count > 3)
    {
        oob_enable = p_parse_value->dw_param[3];
    }
    gap_set_param(GAP_PARAM_BOND_PAIRING_MODE, sizeof(auth_pair_mode), &auth_pair_mode);
    gap_set_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(auth_flags), &auth_flags);
    gap_set_param(GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(auth_io_cap), &auth_io_cap);
    gap_set_param(GAP_PARAM_BOND_OOB_ENABLED, sizeof(uint8_t), &oob_enable);
    le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_ENABLE, sizeof(auth_sec_req_enable), &auth_sec_req_enable);
    le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_REQUIREMENT, sizeof(auth_sec_req_flags),
                      &auth_sec_req_flags);
    cause = gap_set_pairable_mode();
    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Send authentication request
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "sauth",
        "sauth [conn_id]\n\r",
        "Send authentication request\n\r",
        cmd_sauth
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_sauth(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t conn_id = p_parse_value->dw_param[0];
    T_GAP_CAUSE cause;
    cause = le_bond_pair(conn_id);
    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Send user confirmation when show GAP_MSG_LE_BOND_USER_CONFIRMATION
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "userconf",
        "userconf [conn_id] [conf]\n\r",
        "Send user confirmation when show GAP_MSG_LE_BOND_USER_CONFIRMATION\r\n\
        [conf]: 0-(Reject), 1-(Accept)\r\n\
        sample: userconf 0 1\n\r",
        cmd_userconf
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_userconf(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t conn_id = p_parse_value->dw_param[0];
    T_GAP_CFM_CAUSE confirm = GAP_CFM_CAUSE_ACCEPT;
    T_GAP_CAUSE cause;
    if (p_parse_value->dw_param[1] == 0)
    {
        confirm = GAP_CFM_CAUSE_REJECT;
    }
    cause = le_bond_user_confirm(conn_id, confirm);
    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Input passkey when show GAP_MSG_LE_BOND_PASSKEY_INPUT
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "authkey",
        "authkey [conn_id] [passkey]\n\r",
        "Input passkey when show GAP_MSG_LE_BOND_PASSKEY_INPUT\r\n\
        [passkey]: 0 - 999999\r\n\
        sample: authkey 0 123456\n\r",
        cmd_authkey
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_authkey(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t conn_id = p_parse_value->dw_param[0];
    uint32_t passcode = p_parse_value->dw_param[1];
    T_GAP_CAUSE cause;
    T_GAP_CFM_CAUSE confirm = GAP_CFM_CAUSE_ACCEPT;
    if (passcode > GAP_PASSCODE_MAX)
    {
        confirm = GAP_CFM_CAUSE_REJECT;
    }
    cause = le_bond_passkey_input_confirm(conn_id, passcode,
                                          confirm);
    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Clear all bonded devices information
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "bondclear",
        "bondclear\n\r",
        "Clear all bonded devices information\n\r",
        cmd_bondclear
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_bondclear(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    le_bond_clear_all_keys();
    return (RESULT_SUCESS);
}

/**
 * @brief Get all Bonded devices information
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "bondinfo",
        "bondinfo\n\r",
        "Get all Bonded devices information\n\r",
        cmd_bondinfo
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_bondinfo(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t i;
    T_LE_KEY_ENTRY *p_entry;
    uint8_t bond_storage_num = le_get_max_le_paired_device_num();
    for (i = 0; i < bond_storage_num; i++)
    {
        p_entry = le_find_key_entry_by_idx(i);
        if (p_entry != NULL)
        {
            data_uart_print("bond_dev[%d]: bd 0x%02x%02x%02x%02x%02x%02x, addr_type %d, flags 0x%x\r\n",
                            p_entry->idx,
                            p_entry->remote_bd.addr[5],
                            p_entry->remote_bd.addr[4],
                            p_entry->remote_bd.addr[3],
                            p_entry->remote_bd.addr[2],
                            p_entry->remote_bd.addr[1],
                            p_entry->remote_bd.addr[0],
                            p_entry->remote_bd.remote_bd_type,
                            p_entry->flags);
        }
    }

    return (RESULT_SUCESS);
}
/************************** Central only *************************************/
/**
 * @brief Start scan
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "scan",
        "scan [filter_policy] [filter_duplicate]\n\r",
        "Start scan\r\n\
        [filter_policy]: 0-(any), 1-(whitelist), 2-(any RPA), 3-(whitelist RPA) \r\n\
        [filter_duplicate]: 0-(disable), 1-(enable) \n\r",
        cmd_scan
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_scan(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    T_GAP_CAUSE cause;
    uint8_t scan_filter_policy = GAP_SCAN_FILTER_ANY;
    uint8_t scan_filter_duplicate = GAP_SCAN_FILTER_DUPLICATE_ENABLE;

    if (p_parse_value->param_count > 0)
    {
        scan_filter_policy = p_parse_value->dw_param[0];
    }
    if (p_parse_value->param_count > 1)
    {
        scan_filter_duplicate = p_parse_value->dw_param[1];
    }

    link_mgr_clear_device_list();
    le_scan_set_param(GAP_PARAM_SCAN_FILTER_POLICY, sizeof(scan_filter_policy),
                      &scan_filter_policy);
    le_scan_set_param(GAP_PARAM_SCAN_FILTER_DUPLICATES, sizeof(scan_filter_duplicate),
                      &scan_filter_duplicate);
    cause = le_scan_start();
    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Stop scan
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "stopscan",
        "stopscan\n\r",
        "Stop scan\n\r",
        cmd_stopscan
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_stopscan(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    T_GAP_CAUSE cause;
    cause = le_scan_stop();
    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Connect to remote device: use address
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "con",
        "con [BD0] [BD1] [BD2] [BD3] [BD4] [BD5] [addr_type]\n\r",
        "Connect to remote device: use address\r\n\
        [BD0] [BD1] [BD2] [BD3] [BD4] [BD5]: remote device address\r\n\
        [addr_type]: 0-(public), 1-(random)\r\n\
        sample: con x11 x22 x33 x44 x55 x66 0 \n\r",
        cmd_con
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_con(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    T_GAP_CAUSE cause;
    uint8_t addr[6] = {0};
    uint8_t addr_len;
    uint8_t addr_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    T_GAP_LE_CONN_REQ_PARAM conn_req_param;

    conn_req_param.scan_interval = 0x10;
    conn_req_param.scan_window = 0x10;
    conn_req_param.conn_interval_min = 80;
    conn_req_param.conn_interval_max = 80;
    conn_req_param.conn_latency = 0;
    conn_req_param.supv_tout = 1000;
    conn_req_param.ce_len_min = 2 * (conn_req_param.conn_interval_min - 1);
    conn_req_param.ce_len_max = 2 * (conn_req_param.conn_interval_max - 1);
    le_set_conn_param(GAP_CONN_PARAM_1M, &conn_req_param);

    for (addr_len = 0; addr_len < GAP_BD_ADDR_LEN; addr_len++)
    {
        addr[addr_len] = p_parse_value->dw_param[GAP_BD_ADDR_LEN - addr_len - 1];
    }
    if (p_parse_value->param_count >= 7)
    {
        addr_type = p_parse_value->dw_param[6];
    }

    cause = le_connect(GAP_PHYS_CONN_INIT_1M_BIT, addr, (T_GAP_REMOTE_ADDR_TYPE)addr_type,
                       GAP_LOCAL_ADDR_LE_PUBLIC,
                       1000);

    return (T_USER_CMD_PARSE_RESULT)cause;
}

/**
 * @brief Connect to remote device: use showdev to show idx
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "condev",
        "condev [idx]\n\r",
        "Connect to remote device: use showdev to show idx\r\n\
        [idx]: use cmd showdev to show idx before use this cmd\r\n\
        sample: condev 0\n\r",
        cmd_condev
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_condev(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t dev_idx = p_parse_value->dw_param[0];
    if (dev_idx < dev_list_count)
    {
        T_GAP_CAUSE cause;
        T_GAP_LE_CONN_REQ_PARAM conn_req_param;

        conn_req_param.scan_interval = 0x10;
        conn_req_param.scan_window = 0x10;
        conn_req_param.conn_interval_min = 80;
        conn_req_param.conn_interval_max = 80;
        conn_req_param.conn_latency = 0;
        conn_req_param.supv_tout = 1000;
        conn_req_param.ce_len_min = 2 * (conn_req_param.conn_interval_min - 1);
        conn_req_param.ce_len_max = 2 * (conn_req_param.conn_interval_max - 1);
        le_set_conn_param(GAP_CONN_PARAM_1M, &conn_req_param);
        cause = le_connect(GAP_PHYS_CONN_INIT_1M_BIT,
                           dev_list[dev_idx].bd_addr,
                           (T_GAP_REMOTE_ADDR_TYPE)dev_list[dev_idx].bd_type,
                           GAP_LOCAL_ADDR_LE_PUBLIC,
                           1000);
        return (T_USER_CMD_PARSE_RESULT)cause;
    }
    else
    {
        return RESULT_ERR;
    }
}
/************************** GATT client *************************************/
/**
 * @brief Read GAP service characteristic value
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "gapread",
        "gapread [conn_id] [type]\n\r",
        "Read GAP service characteristic value\r\n\
        [type]: 0-(read device name), 1-(read appearance)\r\n\
        simple: gapread 0 0\n\r",
        cmd_gapread
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_gapread(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t conn_id = p_parse_value->dw_param[0];
    T_GAPS_READ_TYPE read_type = (T_GAPS_READ_TYPE)p_parse_value->dw_param[1];
    bool ret = gaps_read(conn_id, read_type);
    if (ret)
    {
        return (RESULT_SUCESS);
    }
    else
    {
        return (RESULT_ERR);
    }
}

/**
 * @brief List GAP service handle cache
 *
 * <b>Command table define</b>
 * \code{.c}
    {
        "gaphdl",
        "gaphdl [conn_id]\n\r",
        "List GAP service handle cache\n\r",
        cmd_gaphdl
    },
 * \endcode
 */
static T_USER_CMD_PARSE_RESULT cmd_gaphdl(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t conn_id = p_parse_value->dw_param[0];
    uint16_t hdl_cache[HDL_GAPS_CACHE_LEN];
    uint8_t hdl_idx;
    bool ret = gaps_get_hdl_cache(conn_id, hdl_cache,
                                  sizeof(uint16_t) * HDL_GAPS_CACHE_LEN);

    if (ret)
    {
        for (hdl_idx = HDL_GAPS_SRV_START; hdl_idx < HDL_GAPS_CACHE_LEN; hdl_idx++)
        {
            data_uart_print("-->Index %d -- Handle 0x%x\r\n", hdl_idx, hdl_cache[hdl_idx]);
        }
        return (RESULT_SUCESS);
    }
    else
    {
        return (RESULT_ERR);
    }
}

static T_USER_CMD_PARSE_RESULT cmd_set_tp_param(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    uint8_t direction = p_parse_value->dw_param[0];
    uint32_t con_interval_us = p_parse_value->dw_param[1];
    uint8_t phy = p_parse_value->dw_param[2];
    uint16_t length = 244;
    uint16_t time = 30;

    if (p_parse_value->param_count > 3)
    {
        length = p_parse_value->dw_param[3];
    }

    if (p_parse_value->param_count > 4)
    {
        time = p_parse_value->dw_param[4];
    }

    tp_set_test_param(direction, con_interval_us, length, phy, time);

    data_uart_print("TP Set Test Param \r\n");
    return (RESULT_SUCESS);
}

static T_USER_CMD_PARSE_RESULT UserCmdLogOn(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    log_module_bitmap_trace_set(0xFFFFFFFFFFFFFFFF, LEVEL_TRACE, 1);
    log_module_bitmap_trace_set(0xFFFFFFFFFFFFFFFF, LEVEL_INFO, 1);
    log_module_bitmap_trace_set(0xFFFFFFFFFFFFFFFF, LEVEL_WARN, 1);
    log_module_bitmap_trace_set(0xFFFFFFFFFFFFFFFF, LEVEL_ERROR, 1);
    data_uart_print("Log On Sucess\r\n");
    return (RESULT_SUCESS);
}

static T_USER_CMD_PARSE_RESULT UserCmdLogOff(T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    log_module_bitmap_trace_set(0xFFFFFFFFFFFFFFFF, LEVEL_TRACE, 0);
    log_module_bitmap_trace_set(0xFFFFFFFFFFFFFFFF, LEVEL_INFO, 0);
    log_module_bitmap_trace_set(0xFFFFFFFFFFFFFFFF, LEVEL_WARN, 0);
    log_module_bitmap_trace_set(0xFFFFFFFFFFFFFFFF, LEVEL_ERROR, 0);
    data_uart_print("Log Off Sucess\r\n");
    return (RESULT_SUCESS);
}

/** @brief  User command table */
const T_USER_CMD_TABLE_ENTRY user_cmd_table[] =
{
    /************************** Common cmd *************************************/
    {
        "conupdreq",
        "conupdreq [conn_id] [interval_min] [interval_max] [latency] [supervision_timeout]\n\r",
        "LE connection param update request\r\n\
        sample: conupdreq 0 0x30 0x40 0 500\n\r",
        cmd_conupdreq
    },
    {
        "showcon",
        "showcon\n\r",
        "Show all devices connecting status\n\r",
        cmd_showcon
    },
    {
        "disc",
        "disc [conn_id]\n\r",
        "Disconnect to remote device\n\r",
        cmd_disc
    },
    {
        "authmode",
        "authmode [auth_flags] [io_cap] [sec_enable] [oob_enable]\n\r",
        "Config authentication mode\r\n\
        [auth_flags]:authentication req bit field: bit0-(bonding), bit2-(MITM), bit3-(SC)\r\n\
        [io_cap]:set io Capabilities: 0-(display only), 1-(display yes/no), 2-(keyboard noly), 3-(no IO), 4-(keyboard display)\r\n\
        [sec_enable]:Start smp pairing procedure when connected: 0-(disable), 1-(enable)\r\n\
        [oob_enable]:Enable oob flag: 0-(disable), 1-(enable)\r\n\
        sample: authmode 0x5 2 1 0\n\r",
        cmd_authmode
    },
    {
        "sauth",
        "sauth [conn_id]\n\r",
        "Send authentication request\n\r",
        cmd_sauth
    },
    {
        "userconf",
        "userconf [conn_id] [conf]\n\r",
        "Send user confirmation when show GAP_MSG_LE_BOND_USER_CONFIRMATION\r\n\
        [conf]: 0-(Reject), 1-(Accept)\r\n\
        sample: userconf 0 1\n\r",
        cmd_userconf
    },
    {
        "authkey",
        "authkey [conn_id] [passkey]\n\r",
        "Input passkey when show GAP_MSG_LE_BOND_PASSKEY_INPUT\r\n\
        [passkey]: 0 - 999999\r\n\
        sample: authkey 0 123456\n\r",
        cmd_authkey
    },
    {
        "bondinfo",
        "bondinfo\n\r",
        "Get all Bonded devices information\n\r",
        cmd_bondinfo
    },
    {
        "bondclear",
        "bondclear\n\r",
        "Clear all bonded devices information\n\r",
        cmd_bondclear
    },
    /************************** Central only *************************************/
    {
        "scan",
        "scan [filter_policy] [filter_duplicate]\n\r",
        "Start scan\r\n\
        [filter_policy]: 0-(any), 1-(whitelist), 2-(any RPA), 3-(whitelist RPA) \r\n\
        [filter_duplicate]: 0-(disable), 1-(enable) \n\r",
        cmd_scan
    },
    {
        "stopscan",
        "stopscan\n\r",
        "Stop scan\n\r",
        cmd_stopscan
    },
    {
        "con",
        "con [BD0] [BD1] [BD2] [BD3] [BD4] [BD5] [addr_type]\n\r",
        "Connect to remote device: use address\r\n\
        [BD0] [BD1] [BD2] [BD3] [BD4] [BD5]: remote device address\r\n\
        [addr_type]: 0-(public), 1-(random)\r\n\
        sample: con x11 x22 x33 x44 x55 x66 0 \n\r",
        cmd_con
    },
    {
        "condev",
        "condev [idx]\n\r",
        "Connect to remote device: use showdev to show idx\r\n\
        [idx]: use cmd showdev to show idx before use this cmd\r\n\
        sample: condev 0\n\r",
        cmd_condev
    },
    /************************** GATT client *************************************/
    /*GAPS client*/
    {
        "gapread",
        "gapread [conn_id] [type]\n\r",
        "Read GAP service characteristic value\r\n\
        [type]: 0-(read device name), 1-(read appearance)\r\n\
        simple: gapread 0 0\n\r",
        cmd_gapread
    },
    {
        "gaphdl",
        "gaphdl [conn_id]\n\r",
        "List GAP service handle cache\n\r",
        cmd_gaphdl
    },
    {
        "tpparam",
        "tpparam [direction] [conn_interval_us] [phy] [length] [time]\n\r",
        "tp client set test param\n\r",
        cmd_set_tp_param
    },
    {
        "logon",
        "logon\n\r",
        "logon\n\r",
        UserCmdLogOn
    },
    {
        "logoff",
        "logoff\n\r",
        "logoff\n\r",
        UserCmdLogOff
    },
    /* MUST be at the end: */
    {
        0,
        0,
        0,
        0
    }
};
/** @} */ /* End of group TP_CLIENT_CMD */


