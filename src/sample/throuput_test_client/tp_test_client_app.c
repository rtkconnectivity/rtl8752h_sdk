/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      tp_test_client_app.c
   * @brief     This file handles Throughput Client application routines.
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
#include <app_msg.h>
#include <string.h>
#include <trace.h>
#include <gap_scan.h>
#include <gap.h>
#include <gap_msg.h>
#include <gap_bond_le.h>
#include <tp_test_client_app.h>
#include <link_mgr.h>
#include <user_cmd.h>
#include <user_cmd_parse.h>
#include <gaps_client.h>
#include <tp_client.h>
#include "os_sched.h"
#include "os_timer.h"
#include "otp_config.h"
#if SYSTEM_TRACE_ENABLE
#include "trace_system_loading.h"
#endif

/** @defgroup  THROUPUT_TEST_CLIENT_APP Throughput Client Application
    * @brief This file handles Throughput Client application routines.
    * @{
    */
/*============================================================================*
 *                              Variables
 *============================================================================*/
/** @addtogroup  THROUPUT_TEST_CLIENT_CALLBACK
    * @{
    */
T_CLIENT_ID   gaps_client_id;        /**< gap service client id*/
T_CLIENT_ID   tp_client_id;   /**< throughput test client id*/
/** @} */ /* End of group THROUPUT_TEST_CLIENT_CALLBACK */

/** @defgroup  THROUPUT_TEST_CLIENT_GAP_MSG GAP Message Handler
    * @brief Handle GAP Message
    * @{
    */
T_GAP_DEV_STATE gap_dev_state = {0, 0, 0, 0};                 /**< GAP device state */

T_TP_TEST_PARAM tp_test_param = {0};
uint8_t tp_test_direction = TP_TEST_DIR_SERVER_TO_CLIENT;
uint8_t tp_test_phy = GAP_PHYS_PREFER_1M_BIT;
uint8_t tp_write_cmd_init_value = 0;
uint64_t tp_write_cmd_begin_time = 0;
uint8_t tp_send_opcode = 0;

void *tp_send_data_complete_timer_handle = NULL;

/*============================================================================*
 *                              Functions
 *============================================================================*/
void app_discov_services(uint8_t conn_id, bool start);
void app_handle_gap_msg(T_IO_MSG  *p_gap_msg);

void tp_send_data_complete_timeout_handler(void *p_handle)
{
    APP_PRINT_ERROR0("tp_send_data_complete_timeout_handler");
    uint8_t conn_id;
    uint32_t timer_id;
    os_timer_id_get(&p_handle, &timer_id);
    conn_id = (uint8_t)timer_id;

    tp_send_opcode = GATT_OPCODE_CHAR_CT_RX_GET_TEST_REPORT;
    tp_client_write_value(conn_id, sizeof(uint8_t), &tp_send_opcode);
}

void tp_set_test_param(uint8_t direction, uint32_t con_interval_us, uint16_t length, uint8_t phy,
                       uint16_t time)
{
    APP_PRINT_INFO5("direction %d, conn_interval_us %d, length %d, phy %d, time %d",
                    direction, con_interval_us, length, phy, time);

    tp_test_direction = direction;
    tp_test_param.con_interval_max = (uint16_t)(con_interval_us / 1250);
    tp_test_param.con_interval_min = (uint16_t)(con_interval_us / 1250);
    tp_test_param.length = length;
    tp_test_param.time = time;
    tp_test_phy = phy;

    if (phy == GAP_PHYS_PREFER_1M_BIT)
    {
        tp_test_param.flag = TP_FLAG_1M_WITH_DATA_LEN_EXTENSION;
    }
    else if (phy == GAP_PHYS_PREFER_2M_BIT)
    {
        tp_test_param.flag = TP_FLAG_2M_WITH_DATA_LEN_EXTENSION;
    }
}

void tp_send_write_cmd(uint8_t conn_id)
{
    uint8_t value[250] = {0};
    uint8_t credits = 10;

    if ((os_sys_time_get() - tp_write_cmd_begin_time) < (tp_test_param.time * 1000))
    {
        while (credits)
        {
            value[0] = tp_write_cmd_init_value;
            if (tp_client_write_command(conn_id, tp_test_param.length, value))
            {
                tp_write_cmd_init_value++;
                credits--;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        tp_send_opcode = GATT_OPCODE_CHAR_CT_TX_DATA_SEND_COMPLETE;
        tp_client_write_value(conn_id, sizeof(uint8_t), &tp_send_opcode);
    }
}

void tp_config_param_write_req(uint8_t conn_id, uint8_t opcode, uint8_t direction)
{
    APP_PRINT_INFO5("tp_config_param_write_req: opcode %d, direction %d, con_interval %d, flag %d, length %d",
                    opcode, tp_test_direction, tp_test_param.con_interval_min, tp_test_param.flag,
                    tp_test_param.length);

    T_TP_WRITE_REQ req = {0};

    req.opcode = opcode;
    LE_UINT16_TO_ARRAY(req.length, tp_test_param.length);
    LE_UINT16_TO_ARRAY(req.time, tp_test_param.time);
    LE_UINT16_TO_ARRAY(req.con_interval_min, tp_test_param.con_interval_min);
    LE_UINT16_TO_ARRAY(req.con_interval_max, tp_test_param.con_interval_max);
    req.flag = tp_test_param.flag;

    os_delay(1000);

    if (tp_client_write_value(conn_id, sizeof(T_TP_WRITE_REQ),
                              (uint8_t *)&req))
    {
        tp_send_opcode = req.opcode;
        return;
    }
}

/**
 * @brief    All the application messages are pre-handled in this function
 * @note     All the IO MSGs are sent to this function, then the event handling
 *           function shall be called according to the MSG type.
 * @param[in] io_msg  IO message data
 * @return   void
 */
void app_handle_io_msg(T_IO_MSG io_msg)
{
    uint16_t msg_type = io_msg.type;
    uint8_t rx_char;

    switch (msg_type)
    {
    case IO_MSG_TYPE_BT_STATUS:
        {
            app_handle_gap_msg(&io_msg);
        }
        break;
    case IO_MSG_TYPE_UART:
        /* We handle user command informations from Data UART in this branch. */
        rx_char = (uint8_t)io_msg.subtype;
        user_cmd_collect(&user_cmd_if, &rx_char, sizeof(rx_char), user_cmd_table);
        break;
    default:
        break;
    }
}

/**
 * @brief    Handle msg GAP_MSG_LE_DEV_STATE_CHANGE
 * @note     All the gap device state events are pre-handled in this function.
 *           Then the event handling function shall be called according to the new_state
 * @param[in] new_state  New gap device state
 * @param[in] cause GAP device state change cause
 * @return   void
 */
void app_handle_dev_state_evt(T_GAP_DEV_STATE new_state, uint16_t cause)
{
    APP_PRINT_INFO3("app_handle_dev_state_evt: init state  %d, scan state %d, cause 0x%x",
                    new_state.gap_init_state,
                    new_state.gap_scan_state, cause);
    if (gap_dev_state.gap_init_state != new_state.gap_init_state)
    {
        if (new_state.gap_init_state == GAP_INIT_STATE_STACK_READY)
        {
            uint8_t bt_addr[6];
            APP_PRINT_INFO0("GAP stack ready");
            /*stack ready*/
            gap_get_param(GAP_PARAM_BD_ADDR, bt_addr);
            data_uart_print("local bd addr: 0x%2x:%2x:%2x:%2x:%2x:%2x\r\n",
                            bt_addr[5],
                            bt_addr[4],
                            bt_addr[3],
                            bt_addr[2],
                            bt_addr[1],
                            bt_addr[0]);
        }
    }

    if (gap_dev_state.gap_scan_state != new_state.gap_scan_state)
    {
        if (new_state.gap_scan_state == GAP_SCAN_STATE_IDLE)
        {
            APP_PRINT_INFO0("GAP scan stop");
            data_uart_print("GAP scan stop\r\n");
        }
        else if (new_state.gap_scan_state == GAP_SCAN_STATE_SCANNING)
        {
            APP_PRINT_INFO0("GAP scan start");
            data_uart_print("GAP scan start\r\n");
        }
    }

    gap_dev_state = new_state;
}

/**
 * @brief    Handle msg GAP_MSG_LE_CONN_STATE_CHANGE
 * @note     All the gap conn state events are pre-handled in this function.
 *           Then the event handling function shall be called according to the new_state
 * @param[in] conn_id Connection ID
 * @param[in] new_state  New gap connection state
 * @param[in] disc_cause Use this cause when new_state is GAP_CONN_STATE_DISCONNECTED
 * @return   void
 */
void app_handle_conn_state_evt(uint8_t conn_id, T_GAP_CONN_STATE new_state, uint16_t disc_cause)
{
    if (conn_id >= APP_MAX_LINKS)
    {
        return;
    }

    APP_PRINT_INFO4("app_handle_conn_state_evt: conn_id %d, conn_state(%d -> %d), disc_cause 0x%x",
                    conn_id, app_link_table[conn_id].conn_state, new_state, disc_cause);

    app_link_table[conn_id].conn_state = new_state;
    switch (new_state)
    {
    case GAP_CONN_STATE_DISCONNECTED:
        {
            if ((disc_cause != (HCI_ERR | HCI_ERR_REMOTE_USER_TERMINATE))
                && (disc_cause != (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE)))
            {
                APP_PRINT_ERROR2("app_handle_conn_state_evt: connection lost, conn_id %d, cause 0x%x", conn_id,
                                 disc_cause);
            }

            memset(&tp_test_param, 0, sizeof(T_TP_TEST_PARAM));
            tp_test_direction = TP_TEST_DIR_SERVER_TO_CLIENT;
            tp_test_phy = GAP_PHYS_PREFER_1M_BIT;
            tp_write_cmd_init_value = 0;
            tp_write_cmd_begin_time = 0;
            tp_send_opcode = 0;

            data_uart_print("Disconnect conn_id %d\r\n", conn_id);
            memset(&app_link_table[conn_id], 0, sizeof(T_APP_LINK));
        }
        break;

    case GAP_CONN_STATE_CONNECTED:
        {
            le_get_conn_addr(conn_id, app_link_table[conn_id].bd_addr,
                             &app_link_table[conn_id].bd_type);
            data_uart_print("Connected success conn_id %d\r\n", conn_id);
        }
        break;

    default:
        break;

    }
}

/**
 * @brief    Handle msg GAP_MSG_LE_AUTHEN_STATE_CHANGE
 * @note     All the gap authentication state events are pre-handled in this function.
 *           Then the event handling function shall be called according to the new_state
 * @param[in] conn_id Connection ID
 * @param[in] new_state  New authentication state
 * @param[in] cause Use this cause when new_state is GAP_AUTHEN_STATE_COMPLETE
 * @return   void
 */
void app_handle_authen_state_evt(uint8_t conn_id, uint8_t new_state, uint16_t cause)
{
    APP_PRINT_INFO2("app_handle_authen_state_evt:conn_id %d, cause 0x%x", conn_id, cause);

    switch (new_state)
    {
    case GAP_AUTHEN_STATE_STARTED:
        {
            APP_PRINT_INFO0("app_handle_authen_state_evt: GAP_AUTHEN_STATE_STARTED");
        }
        break;

    case GAP_AUTHEN_STATE_COMPLETE:
        {
            if (cause == GAP_SUCCESS)
            {
                data_uart_print("Pair success\r\n");
                APP_PRINT_INFO0("app_handle_authen_state_evt: GAP_AUTHEN_STATE_COMPLETE pair success");

            }
            else
            {
                data_uart_print("Pair failed: cause 0x%x\r\n", cause);
                APP_PRINT_INFO0("app_handle_authen_state_evt: GAP_AUTHEN_STATE_COMPLETE pair failed");
            }
        }
        break;

    default:
        {
            APP_PRINT_ERROR1("app_handle_authen_state_evt: unknown newstate %d", new_state);
        }
        break;
    }
}

/**
 * @brief    Handle msg GAP_MSG_LE_CONN_MTU_INFO
 * @note     This msg is used to inform APP that exchange mtu procedure is completed.
 * @param[in] conn_id Connection ID
 * @param[in] mtu_size  New mtu size
 * @return   void
 */
void app_handle_conn_mtu_info_evt(uint8_t conn_id, uint16_t mtu_size)
{
    APP_PRINT_INFO2("app_handle_conn_mtu_info_evt: conn_id %d, mtu_size %d", conn_id, mtu_size);
    app_discov_services(conn_id, true);
}

/**
 * @brief    Handle msg GAP_MSG_LE_CONN_PARAM_UPDATE
 * @note     All the connection parameter update change  events are pre-handled in this function.
 * @param[in] conn_id Connection ID
 * @param[in] status  New update state
 * @param[in] cause Use this cause when status is GAP_CONN_PARAM_UPDATE_STATUS_FAIL
 * @return   void
 */
void app_handle_conn_param_update_evt(uint8_t conn_id, uint8_t status, uint16_t cause)
{
    switch (status)
    {
    case GAP_CONN_PARAM_UPDATE_STATUS_SUCCESS:
        {
            uint16_t conn_interval;
            uint16_t conn_slave_latency;
            uint16_t conn_supervision_timeout;

            le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &conn_interval, conn_id);
            le_get_conn_param(GAP_PARAM_CONN_LATENCY, &conn_slave_latency, conn_id);
            le_get_conn_param(GAP_PARAM_CONN_TIMEOUT, &conn_supervision_timeout, conn_id);
            APP_PRINT_INFO4("app_handle_conn_param_update_evt update success:conn_id %d, conn_interval 0x%x, conn_slave_latency 0x%x, conn_supervision_timeout 0x%x",
                            conn_id, conn_interval, conn_slave_latency, conn_supervision_timeout);
        }
        break;

    case GAP_CONN_PARAM_UPDATE_STATUS_FAIL:
        {
            APP_PRINT_ERROR2("app_handle_conn_param_update_evt update failed: conn_id %d, cause 0x%x",
                             conn_id, cause);
        }
        break;

    case GAP_CONN_PARAM_UPDATE_STATUS_PENDING:
        {
            APP_PRINT_INFO1("app_handle_conn_param_update_evt update pending: conn_id %d", conn_id);
        }
        break;

    default:
        break;
    }
}

/**
 * @brief    All the BT GAP MSG are pre-handled in this function.
 * @note     Then the event handling function shall be called according to the
 *           subtype of T_IO_MSG
 * @param[in] p_gap_msg Pointer to GAP msg
 * @return   void
 */
void app_handle_gap_msg(T_IO_MSG *p_gap_msg)
{
    T_LE_GAP_MSG gap_msg;
    uint8_t conn_id;
    memcpy(&gap_msg, &p_gap_msg->u.param, sizeof(p_gap_msg->u.param));

    APP_PRINT_TRACE1("app_handle_gap_msg: subtype %d", p_gap_msg->subtype);
    switch (p_gap_msg->subtype)
    {
    case GAP_MSG_LE_DEV_STATE_CHANGE:
        {
            app_handle_dev_state_evt(gap_msg.msg_data.gap_dev_state_change.new_state,
                                     gap_msg.msg_data.gap_dev_state_change.cause);
        }
        break;

    case GAP_MSG_LE_CONN_STATE_CHANGE:
        {
            app_handle_conn_state_evt(gap_msg.msg_data.gap_conn_state_change.conn_id,
                                      (T_GAP_CONN_STATE)gap_msg.msg_data.gap_conn_state_change.new_state,
                                      gap_msg.msg_data.gap_conn_state_change.disc_cause);
        }
        break;

    case GAP_MSG_LE_CONN_MTU_INFO:
        {
            app_handle_conn_mtu_info_evt(gap_msg.msg_data.gap_conn_mtu_info.conn_id,
                                         gap_msg.msg_data.gap_conn_mtu_info.mtu_size);
        }
        break;

    case GAP_MSG_LE_CONN_PARAM_UPDATE:
        {
            app_handle_conn_param_update_evt(gap_msg.msg_data.gap_conn_param_update.conn_id,
                                             gap_msg.msg_data.gap_conn_param_update.status,
                                             gap_msg.msg_data.gap_conn_param_update.cause);
        }
        break;

    case GAP_MSG_LE_AUTHEN_STATE_CHANGE:
        {
            app_handle_authen_state_evt(gap_msg.msg_data.gap_authen_state.conn_id,
                                        gap_msg.msg_data.gap_authen_state.new_state,
                                        gap_msg.msg_data.gap_authen_state.status);
        }
        break;

    case GAP_MSG_LE_BOND_JUST_WORK:
        {
            conn_id = gap_msg.msg_data.gap_bond_just_work_conf.conn_id;
            le_bond_just_work_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
            APP_PRINT_INFO0("GAP_MSG_LE_BOND_JUST_WORK");
        }
        break;

    case GAP_MSG_LE_BOND_PASSKEY_DISPLAY:
        {
            uint32_t display_value = 0;
            conn_id = gap_msg.msg_data.gap_bond_passkey_display.conn_id;
            le_bond_get_display_key(conn_id, &display_value);
            APP_PRINT_INFO2("GAP_MSG_LE_BOND_PASSKEY_DISPLAY: conn_id %d, passkey %d",
                            conn_id, display_value);
            le_bond_passkey_display_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
            data_uart_print("GAP_MSG_LE_BOND_PASSKEY_DISPLAY: conn_id %d, passkey %d\r\n",
                            conn_id,
                            display_value);
        }
        break;

    case GAP_MSG_LE_BOND_USER_CONFIRMATION:
        {
            uint32_t display_value = 0;
            conn_id = gap_msg.msg_data.gap_bond_user_conf.conn_id;
            le_bond_get_display_key(conn_id, &display_value);
            APP_PRINT_INFO2("GAP_MSG_LE_BOND_USER_CONFIRMATION: conn_id %d, passkey %d",
                            conn_id, display_value);
            data_uart_print("GAP_MSG_LE_BOND_USER_CONFIRMATION: conn_id %d, passkey %d\r\n",
                            conn_id,
                            display_value);
            //le_bond_user_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
        }
        break;

    case GAP_MSG_LE_BOND_PASSKEY_INPUT:
        {
            //uint32_t passkey = 888888;
            conn_id = gap_msg.msg_data.gap_bond_passkey_input.conn_id;
            APP_PRINT_INFO1("GAP_MSG_LE_BOND_PASSKEY_INPUT: conn_id %d", conn_id);
            data_uart_print("GAP_MSG_LE_BOND_PASSKEY_INPUT: conn_id %d\r\n", conn_id);
            //le_bond_passkey_input_confirm(conn_id, passkey, GAP_CFM_CAUSE_ACCEPT);
        }
        break;

    case GAP_MSG_LE_BOND_OOB_INPUT:
        {
            uint8_t oob_data[GAP_OOB_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            conn_id = gap_msg.msg_data.gap_bond_oob_input.conn_id;
            APP_PRINT_INFO1("GAP_MSG_LE_BOND_OOB_INPUT: conn_id %d", conn_id);
            le_bond_set_param(GAP_PARAM_BOND_OOB_DATA, GAP_OOB_LEN, oob_data);
            le_bond_oob_input_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
        }
        break;

    default:
        APP_PRINT_ERROR1("app_handle_gap_msg: unknown subtype %d", p_gap_msg->subtype);
        break;
    }
}

/** @defgroup  THROUPUT_TEST_CLIENT_GAP_CALLBACK GAP Callback Event Handler
    * @brief Handle GAP callback event
    * @{
    */
/**
  * @brief Callback for gap le to notify app
  * @param[in] cb_type callback msy type @ref GAP_LE_MSG_Types.
  * @param[in] p_cb_data point to callback data @ref T_LE_CB_DATA.
  * @retval result @ref T_APP_RESULT
  */
T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
{
    T_APP_RESULT result = APP_RESULT_SUCCESS;
    T_LE_CB_DATA *p_data = (T_LE_CB_DATA *)p_cb_data;

    switch (cb_type)
    {
    case GAP_MSG_LE_SCAN_INFO:
        APP_PRINT_INFO5("GAP_MSG_LE_SCAN_INFO:adv_type 0x%x, bd_addr %s, remote_addr_type %d, rssi %d, data_len %d",
                        p_data->p_le_scan_info->adv_type,
                        TRACE_BDADDR(p_data->p_le_scan_info->bd_addr),
                        p_data->p_le_scan_info->remote_addr_type,
                        p_data->p_le_scan_info->rssi,
                        p_data->p_le_scan_info->data_len);
        /* If you want to parse the scan info, please reference function app_parse_scan_info in observer app. */
        break;

    case GAP_MSG_LE_CONN_UPDATE_IND:
        APP_PRINT_INFO5("GAP_MSG_LE_CONN_UPDATE_IND: conn_id %d, conn_interval_max 0x%x, conn_interval_min 0x%x, conn_latency 0x%x,supervision_timeout 0x%x",
                        p_data->p_le_conn_update_ind->conn_id,
                        p_data->p_le_conn_update_ind->conn_interval_max,
                        p_data->p_le_conn_update_ind->conn_interval_min,
                        p_data->p_le_conn_update_ind->conn_latency,
                        p_data->p_le_conn_update_ind->supervision_timeout);
        /* if reject the proposed connection parameter from peer device, use APP_RESULT_REJECT. */
        result = APP_RESULT_ACCEPT;
        break;

    case GAP_MSG_LE_PHY_UPDATE_INFO:
        {
            APP_PRINT_INFO4("GAP_MSG_LE_PHY_UPDATE_INFO:conn %d, cause 0x%x, rx_phy %d, tx_phy %d",
                            p_data->p_le_phy_update_info->conn_id,
                            p_data->p_le_phy_update_info->cause,
                            p_data->p_le_phy_update_info->rx_phy,
                            p_data->p_le_phy_update_info->tx_phy);

        }
        break;

    default:
        APP_PRINT_ERROR1("app_gap_callback: unhandled cb_type 0x%x", cb_type);
        break;
    }
    return result;
}
/** @} */ /* End of group THROUPUT_TEST_CLIENT_GAP_CALLBACK */

/** @defgroup  THROUPUT_TEST_CLIENT_SRV_DIS GATT Services discovery and storage
    * @brief GATT Services discovery and storage
    * @{
    */
/**
 * @brief  Discovery GATT services
 * @param  conn_id connection ID.
 * @param  start first call. true - first call this function after conncection, false - not first
 * @retval None
 */
void app_discov_services(uint8_t conn_id, bool start)
{
    if (app_link_table[conn_id].conn_state != GAP_CONN_STATE_CONNECTED)
    {
        APP_PRINT_ERROR1("app_discov_services: conn_id %d not connected ", conn_id);
        return;
    }
    if (start)
    {
        if (gaps_start_discovery(conn_id) == false)
        {
            APP_PRINT_ERROR1("app_discov_services: discover gaps failed conn_id %d", conn_id);
        }
        return;
    }
    if ((app_link_table[conn_id].discovered_flags & APP_DISCOV_TPS_FLAG) == 0)
    {
        if (tp_client_start_discovery(conn_id) == false)
        {
            APP_PRINT_ERROR1("app_discov_services: discover tps failed conn_id %d", conn_id);
        }
    }
    else
    {
        APP_PRINT_INFO2("app_discov_services: discover complete, conn_id %d, srv_found_flags 0x%x",
                        conn_id, app_link_table[conn_id].srv_found_flags);
    }

    return;
}
/** @} */ /* End of group THROUPUT_TEST_CLIENT_SRV_DIS */

/** @defgroup  THROUPUT_TEST_CLIENT_CALLBACK Profile Client Callback Event Handler
    * @brief Handle profile client callback event
    * @{
    */

/**
 * @brief  Callback will be called when data sent from profile client layer.
 * @param  client_id the ID distinguish which module sent the data.
 * @param  conn_id connection ID.
 * @param  p_data  pointer to data.
 * @retval   result @ref T_APP_RESULT
 */
T_APP_RESULT app_client_callback(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    APP_PRINT_INFO2("app_client_callback: client_id %d, conn_id %d",
                    client_id, conn_id);
    if (client_id == gaps_client_id)
    {
        T_GAPS_CLIENT_CB_DATA *p_gaps_cb_data = (T_GAPS_CLIENT_CB_DATA *)p_data;
        switch (p_gaps_cb_data->cb_type)
        {
        case GAPS_CLIENT_CB_TYPE_DISC_STATE:
            switch (p_gaps_cb_data->cb_content.disc_state)
            {
            case DISC_GAPS_DONE:
                app_link_table[conn_id].discovered_flags |= APP_DISCOV_GAPS_FLAG;
                app_link_table[conn_id].srv_found_flags |= APP_DISCOV_GAPS_FLAG;
                app_discov_services(conn_id, false);
                /* Discovery Simple BLE service procedure successfully done. */
                APP_PRINT_INFO0("app_client_callback: discover gaps procedure done.");
                break;
            case DISC_GAPS_FAILED:
                app_link_table[conn_id].discovered_flags |= APP_DISCOV_GAPS_FLAG;
                app_discov_services(conn_id, false);
                /* Discovery Request failed. */
                APP_PRINT_INFO0("app_client_callback: discover gaps request failed.");
                break;
            default:
                break;
            }
            break;
        case GAPS_CLIENT_CB_TYPE_READ_RESULT:
            switch (p_gaps_cb_data->cb_content.read_result.type)
            {
            case GAPS_READ_DEVICE_NAME:
                if (p_gaps_cb_data->cb_content.read_result.cause == GAP_SUCCESS)
                {
                    APP_PRINT_INFO1("GAPS_READ_DEVICE_NAME: device name %s.",
                                    TRACE_STRING(p_gaps_cb_data->cb_content.read_result.data.device_name.p_value));
                }
                else
                {
                    APP_PRINT_INFO1("GAPS_READ_DEVICE_NAME: failded cause 0x%x",
                                    p_gaps_cb_data->cb_content.read_result.cause);
                }
                break;
            case GAPS_READ_APPEARANCE:
                if (p_gaps_cb_data->cb_content.read_result.cause == GAP_SUCCESS)
                {
                    APP_PRINT_INFO1("GAPS_READ_APPEARANCE: appearance %d",
                                    p_gaps_cb_data->cb_content.read_result.data.appearance);
                }
                else
                {
                    APP_PRINT_INFO1("GAPS_READ_APPEARANCE: failded cause 0x%x",
                                    p_gaps_cb_data->cb_content.read_result.cause);
                }
                break;
            case GAPS_READ_CENTRAL_ADDR_RESOLUTION:
                if (p_gaps_cb_data->cb_content.read_result.cause == GAP_SUCCESS)
                {
                    APP_PRINT_INFO1("GAPS_READ_THROUPUT_TEST_CLIENT_ADDR_RESOLUTION: central_addr_res %d",
                                    p_gaps_cb_data->cb_content.read_result.data.central_addr_res);
                }
                else
                {
                    APP_PRINT_INFO1("GAPS_READ_THROUPUT_TEST_CLIENT_ADDR_RESOLUTION: failded cause 0x%x",
                                    p_gaps_cb_data->cb_content.read_result.cause);
                }
                break;
            default:
                break;
            }
            break;

        default:
            break;
        }
    }
    else if (client_id == tp_client_id)
    {
        T_TP_CB_DATA *p_tp_cb_data = (T_TP_CB_DATA *)p_data;
        switch (p_tp_cb_data->cb_type)
        {
        case TP_CLIENT_CB_TYPE_DISC_STATE:
            switch (p_tp_cb_data->cb_content.disc_state)
            {
            case DISC_TP_DONE:
                /* Discovery Throughput Service procedure successfully done. */
                app_link_table[conn_id].discovered_flags |= APP_DISCOV_TPS_FLAG;
                app_link_table[conn_id].srv_found_flags |= APP_DISCOV_TPS_FLAG;
                app_discov_services(conn_id, false);
                APP_PRINT_INFO0("app_client_callback: discover tp service procedure done.");
                tp_client_write_notify_cccd(conn_id, true);
                break;
            case DISC_TP_FAILED:
                /* Discovery Request failed. */
                app_link_table[conn_id].discovered_flags |= APP_DISCOV_TPS_FLAG;
                app_discov_services(conn_id, false);
                APP_PRINT_INFO0("app_client_callback: discover tp service request failed.");
                break;
            default:
                break;
            }
            break;

        case TP_CLIENT_CB_TYPE_READ_RESULT:
            switch (p_tp_cb_data->cb_content.read_result.type)
            {
            case TP_READ_PREFER_PARAM:
                if (p_tp_cb_data->cb_content.read_result.cause != GAP_SUCCESS)
                {
                    APP_PRINT_ERROR1("TP_READ_PREFER_PARAM: failed cause 0x%x",
                                     p_tp_cb_data->cb_content.read_result.cause);
                }
                break;

            default:
                break;
            }
            break;

        case TP_CLIENT_CB_TYPE_WRITE_RESULT:
            APP_PRINT_INFO1("TP_CLIENT_CB_TYPE_WRITE_RESULT: type %d",
                            p_tp_cb_data->cb_content.write_result.type);
            switch (p_tp_cb_data->cb_content.write_result.type)
            {
            case TP_WRITE_NOTIFY_CCCD:
                if (p_tp_cb_data->cb_content.write_result.cause == GAP_SUCCESS)
                {
                    tp_client_write_ind_cccd(conn_id, true);
                }
                else
                {
                    APP_PRINT_INFO1("TP_WRITE_NOTIFY_CCCD: cause 0x%x.", p_tp_cb_data->cb_content.write_result.cause);
                }
                break;

            case TP_WRITE_IND_CCCD:
                if (p_tp_cb_data->cb_content.write_result.cause == GAP_SUCCESS)
                {
                    data_uart_print("TP_TEST_MODE_ONE_SET \r\n");

                    if (tp_test_phy == GAP_PHYS_PREFER_2M_BIT)
                    {
                        le_set_phy(conn_id, GAP_PHYS_PREFER_ALL, tp_test_phy, tp_test_phy,
                                   GAP_PHYS_OPTIONS_CODED_PREFER_NO);
                    }

                    data_uart_print("tp_test_direction,  phy, conn_interval_us, pkt_len, tx data rate(Bytes/s), rx data rate(Bytes/s) \r\n");

                    uint8_t opcode = 0;
                    switch (tp_test_direction)
                    {
                    case TP_TEST_DIR_SERVER_TO_CLIENT:
                        opcode = GATT_OPCODE_CHAR_CT_RX_CONFIG_DT_TX_PREFER_PARAM;
                        break;

                    case TP_TEST_DIR_CLIENT_TO_SERVER:
                        opcode = GATT_OPCODE_CHAR_CT_RX_CONFIG_DT_RX_PREFER_PARAM;
                        break;

                    case TP_TEST_DIR_BOTH_WAYS:
                        opcode = GATT_OPCODE_CHAR_CT_RX_CONFIG_DT_TX_RX_PREFER_PARAM;
                        break;

                    default:
                        break;
                    }

                    tp_config_param_write_req(conn_id, opcode, opcode);
                }
                else
                {
                    APP_PRINT_INFO1("TP_WRITE_IND_CCCD: cause 0x%x.", p_tp_cb_data->cb_content.write_result.cause);
                }
                break;

            case TP_WRITE_CHAR_VALUE:
                if (p_tp_cb_data->cb_content.write_result.cause == GAP_SUCCESS)
                {
                    switch (p_tp_cb_data->cb_content.write_result.write_type)
                    {
                    case GATT_WRITE_TYPE_CMD:
                        tp_send_write_cmd(conn_id);
                        break;

                    case GATT_WRITE_TYPE_REQ:
                        {
                            if (tp_send_opcode == GATT_OPCODE_CHAR_CT_TX_DATA_SEND_COMPLETE)
                            {
                                if (NULL == tp_send_data_complete_timer_handle)
                                {
                                    os_timer_create(&tp_send_data_complete_timer_handle, "tp_send_data_complete_timer",
                                                    conn_id, 2000, false, tp_send_data_complete_timeout_handler);
                                }
                                os_timer_start(&tp_send_data_complete_timer_handle);
                            }
                        }
                        break;

                    default:
                        break;
                    }
                }
                break;

            default:
                break;
            }
            break;

        case TP_CLIENT_CB_TYPE_NOTIF_IND_RESULT:
            {
                uint8_t tp_recv_opcode = 0;

                memcpy(&tp_recv_opcode, p_tp_cb_data->cb_content.notif_ind_data.pValue, 1);
                APP_PRINT_INFO1("TP_CLIENT_CB_TYPE_NOTIF_IND_RESULT: tp_recv_opcode %d", tp_recv_opcode);

                switch (tp_recv_opcode)
                {
                case GATT_OPCODE_CHAR_CT_TX_DATA_SEND_COMPLETE:
                    {
                        if (NULL == tp_send_data_complete_timer_handle)
                        {
                            os_timer_create(&tp_send_data_complete_timer_handle, "tp_send_data_complete_timer",
                                            conn_id, 2000, false, tp_send_data_complete_timeout_handler);
                        }
                        os_timer_start(&tp_send_data_complete_timer_handle);
                    }
                    break;

                case GATT_OPCODE_CHAR_CT_TX_CONFIG_PREFER_PARAM_RSP:
                    {
                        uint8_t status = 0;
                        memcpy(&status, p_tp_cb_data->cb_content.notif_ind_data.pValue + 1, 1);

                        if (status)
                        {
                            data_uart_print("Config Param fail status %d \r\n", status);
                            le_disconnect(conn_id);
                            break;
                        }

#if SYSTEM_TRACE_ENABLE
                        APP_PRINT_INFO0("CPU loading test start");
                        reset_system_loading();
#endif
                        switch (tp_test_direction)
                        {
                        case TP_TEST_DIR_CLIENT_TO_SERVER:
                        case TP_TEST_DIR_BOTH_WAYS:
                            {
                                tp_write_cmd_begin_time = os_sys_time_get();
                                APP_PRINT_INFO0("TP_TEST_DIR_CLIENT_TO_SERVER");
                                tp_send_write_cmd(conn_id);
                            }
                            break;

                        default:
                            break;
                        }

                    }
                    break;

                case GATT_OPCODE_CHAR_CT_TX_DATA_RATE_REPORT:
                    {
                        LE_ARRAY_TO_UINT32(tp_test_param.tx_rate,
                                           p_tp_cb_data->cb_content.notif_ind_data.pValue + 1);
                        LE_ARRAY_TO_UINT32(tp_test_param.rx_rate,
                                           p_tp_cb_data->cb_content.notif_ind_data.pValue + 5);


                        if (tp_test_direction == TP_TEST_DIR_SERVER_TO_CLIENT)
                        {
                            data_uart_print("  server->client,");
                        }
                        else if (tp_test_direction == TP_TEST_DIR_CLIENT_TO_SERVER)
                        {
                            data_uart_print("  client->server,");
                        }
                        else
                        {
                            data_uart_print("  client<->server,");
                        }

                        if (tp_test_phy == GAP_PHYS_PREFER_1M_BIT)
                        {
                            data_uart_print("   1M,");
                        }
                        else if (tp_test_phy == GAP_PHYS_PREFER_2M_BIT)
                        {
                            data_uart_print("   2M,");
                        }

                        data_uart_print("       %d,        %d,            %d,                 %d \r\n",
                                        (tp_test_param.con_interval_min) * 1250,
                                        tp_test_param.length,
                                        tp_test_param.tx_rate, tp_test_param.rx_rate);

                        os_delay(1000);
                        le_disconnect(conn_id);
#if SYSTEM_TRACE_ENABLE
                        APP_PRINT_INFO0("CPU loading test end");
                        get_system_loading();
#endif
                    }
                    break;

                default:
                    break;
                }
            }
            break;

        default:
            break;
        }
    }

    return result;
}

/** @} */ /* End of group THROUPUT_TEST_CLIENT_CLIENT_CALLBACK */
/** @} */ /* End of group THROUPUT_TEST_CLIENT_APP */

