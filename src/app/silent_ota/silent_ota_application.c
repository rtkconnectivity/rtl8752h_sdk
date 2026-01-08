/*
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

#include <trace.h>
#include <string.h>
#include "gap.h"
#include "gap_adv.h"
#include "gap_msg.h"
#include "gap_bond_le.h"
#include "gap_conn_le.h"
#include "profile_server.h"
#include "app_msg.h"
#include "app_task.h"
#include "patch_header_check.h"
#include "rtl876x_wdg.h"
#include "dfu_flash.h"
#include "dfu_api.h"
#include "bas.h"
#include "dis.h"
#include "dfu_service.h"
#include "ota_service.h"
#include "silent_ota_application.h"
#include "otp_config.h"
#include "board.h"
#include "flash_nor_device.h"

/*============================================================================*
 *                              Macros
 *============================================================================*/
//convert img version bit map to deirectly copmare version value
#define IMG_VER_TO_LE_VALUE(p)       (((p & 0xf) << 28 ) + (((p & 0xff0) >> 4) << 20) + (((p & 0x07fff000) >> 12) << 5) + ((p & 0xf8000000) >> 27))
#define OTA_IMG_VER_TO_LE_VALUE(p)   (((p & 0xff) << 24 ) + (((p >> 8) & 0xff) << 16) + (((p >> 16) & 0xff) << 8) + ((p >> 24) & 0xff))

/*============================================================================*
 *                              external Variables
 *============================================================================*/
extern T_SERVER_ID g_bas_service_id;
extern T_SERVER_ID g_dis_service_id;
extern T_SERVER_ID g_ota_service_id;
extern T_SERVER_ID g_dfu_service_id;

uint16_t g_bat_vol;
uint16_t g_bas_battery_level;

/*============================================================================*
 *                              Local Variables
 *============================================================================*/
T_GAP_DEV_STATE gap_dev_state = {0, 0, 0, 0};                 /**< GAP device state */
T_GAP_CONN_STATE gap_conn_state = GAP_CONN_STATE_DISCONNECTED; /**< GAP connection state */


static bool switch_to_ota_mode_pending = false;
static bool dfu_active_rst_pending = false;
LantencyStatus g_lantency_status = LANTENCY_OFF;

/*============================================================================*
 *                              Functions
 *============================================================================*/
/*APP can define differtnt check rules of different images*/
bool dfu_soc_check_img_version(uint16_t image_id, uint32_t version)
{
    T_IMAGE_VERSION active_image_version;
    get_active_bank_image_version((T_IMG_ID)image_id, &active_image_version);

    APP_PRINT_INFO3("dfu_soc_check_img_version: img 0x%04x, dfu ver 0x%08x, active ver 0x%08x",
                    image_id, version, active_image_version.ver_info.version);

#if (OTA_ONLY_SUPPORT_UPGRADE == 1)
    if (image_id == OTA)
    {
        if (OTA_IMG_VER_TO_LE_VALUE(version) < OTA_IMG_VER_TO_LE_VALUE(
                active_image_version.ver_info.version))
        {
            return false;
        }
    }
    else
    {
        if (IMG_VER_TO_LE_VALUE(version) < IMG_VER_TO_LE_VALUE(active_image_version.ver_info.version))
        {
            return false;
        }
    }
#endif

    return true;
}

void app_handle_gap_msg(T_IO_MSG  *p_gap_msg);
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

    switch (msg_type)
    {
    case IO_MSG_TYPE_BT_STATUS:
        {
            app_handle_gap_msg(&io_msg);
        }
        break;
    case IO_MSG_TYPE_DFU_VALID_FW:
        {
            APP_PRINT_INFO0("IO_MSG_TYPE_DFU_VALID_FW");
            dfu_service_handle_valid_fw(io_msg.u.param);
        }
        break;
#if (ROM_WATCH_DOG_ENABLE == 1)
    case IO_MSG_TYPE_RESET_WDG_TIMER:
        {
            APP_PRINT_INFO0("[WDG] Watch Dog Rset Timer");
            WDG_Restart();
        }
        break;
#endif
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
    APP_PRINT_INFO3("app_handle_dev_state_evt: init state %d, adv state %d, cause 0x%x",
                    new_state.gap_init_state, new_state.gap_adv_state, cause);
    if (gap_dev_state.gap_init_state != new_state.gap_init_state)
    {
        if (new_state.gap_init_state == GAP_INIT_STATE_STACK_READY)
        {
            APP_PRINT_INFO0("GAP stack ready");
            /*stack ready*/
            le_adv_start();
        }
    }

    if (gap_dev_state.gap_adv_state != new_state.gap_adv_state)
    {
        if (new_state.gap_adv_state == GAP_ADV_STATE_IDLE)
        {
            if (new_state.gap_adv_sub_state == GAP_ADV_TO_IDLE_CAUSE_CONN)
            {
                APP_PRINT_INFO0("GAP adv stoped: because connection created");
            }
            else
            {
                APP_PRINT_INFO0("GAP adv stoped");
            }
        }
        else if (new_state.gap_adv_state == GAP_ADV_STATE_ADVERTISING)
        {

            APP_PRINT_INFO0("GAP adv start");
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
    APP_PRINT_INFO4("app_handle_conn_state_evt: conn_id %d old_state %d new_state %d, disc_cause 0x%x",
                    conn_id, gap_conn_state, new_state, disc_cause);
    switch (new_state)
    {
    case GAP_CONN_STATE_DISCONNECTED:
        {
            if ((disc_cause != (HCI_ERR | HCI_ERR_REMOTE_USER_TERMINATE))
                && (disc_cause != (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE)))
            {
                APP_PRINT_ERROR1("app_handle_conn_state_evt: connection lost cause 0x%x", disc_cause);
            }

            if (switch_to_ota_mode_pending)
            {
#if (SUPPORT_NORMAL_OTA == 1)
                switch_to_ota_mode_pending = false;
                extern void dfu_switch_to_ota_mode(void);
                dfu_switch_to_ota_mode();
#endif
            }
            else
            {
                if (dfu_active_rst_pending)
                {
                    dfu_active_rst_pending = false;
                    //unlock_flash_bp_all();   //GRACE TO CHECK
                    dfu_fw_reboot(true);
                }
                else
                {
                    le_adv_start();
                }
            }

            //le_adv_start();
        }
        break;

    case GAP_CONN_STATE_CONNECTED:
        {
            uint16_t conn_interval;
            uint16_t conn_latency;
            uint16_t conn_supervision_timeout;
            uint8_t  remote_bd[6];
            T_GAP_REMOTE_ADDR_TYPE remote_bd_type;

            le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &conn_interval, conn_id);
            le_get_conn_param(GAP_PARAM_CONN_LATENCY, &conn_latency, conn_id);
            le_get_conn_param(GAP_PARAM_CONN_TIMEOUT, &conn_supervision_timeout, conn_id);
            le_get_conn_addr(conn_id, remote_bd, &remote_bd_type);
            APP_PRINT_INFO5("GAP_CONN_STATE_CONNECTED:remote_bd %s, remote_addr_type %d, conn_interval 0x%x, conn_latency 0x%x, conn_supervision_timeout 0x%x",
                            TRACE_BDADDR(remote_bd), remote_bd_type,
                            conn_interval, conn_latency, conn_supervision_timeout);

            g_dfu_para.dfu_conn_para_update_in_progress = false;
            g_dfu_para.dfu_conn_interval = conn_interval;
            g_dfu_para.dfu_conn_lantency = conn_latency;

#if F_BT_LE_5_0_SET_PHY_SUPPORT
            uint8_t tx_phy;
            uint8_t rx_phy;
            le_get_conn_param(GAP_PARAM_CONN_RX_PHY_TYPE, &rx_phy, conn_id);
            le_get_conn_param(GAP_PARAM_CONN_TX_PHY_TYPE, &tx_phy, conn_id);
            APP_PRINT_INFO2("GAP_CONN_STATE_CONNECTED: tx_phy %d, rx_phy %d", tx_phy, rx_phy);

            uint8_t all_phys;
            uint8_t tx_phys;
            uint8_t rx_phys;
            T_GAP_PHYS_OPTIONS phy_options = GAP_PHYS_OPTIONS_CODED_PREFER_S8;

            all_phys = GAP_PHYS_PREFER_ALL;
            tx_phys = GAP_PHYS_PREFER_2M_BIT;
            rx_phys = GAP_PHYS_PREFER_2M_BIT;
            le_set_phy(conn_id, all_phys, tx_phys, rx_phys, phy_options);
#endif

#if (ENABLE_SLAVE_REQUEST_UPDATE_COON_PARA ==1)
            T_GAP_CAUSE update_conn_para_cause = le_update_conn_param(0, CONNECT_INTERVAL_MIN,
                                                                      CONNECT_INTERVAL_MAX, CONNECT_LATENCY,
                                                                      SUPERVISION_TIMEOUT, 2 * (CONNECT_INTERVAL_MIN - 1), 2 * (CONNECT_INTERVAL_MAX - 1));
            if (GAP_CAUSE_SUCCESS != update_conn_para_cause)
            {
                DFU_PRINT_INFO1("le_update_conn_param: send HCI command failed! cause=%d", update_conn_para_cause);
            }
#endif


        }
        break;

    default:
        break;
    }
    gap_conn_state = new_state;
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
                g_lantency_status = LANTENCY_UPDATING;
                APP_PRINT_INFO0("app_handle_authen_state_evt: GAP_AUTHEN_STATE_COMPLETE pair success");
            }
            else
            {
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
            if (conn_slave_latency)
            {
                g_lantency_status = LANTENCY_ON;
            }

            if (g_dfu_para.dfu_conn_para_update_in_progress == true)
            {
                g_dfu_para.dfu_conn_interval = conn_interval;
                g_dfu_para.dfu_conn_lantency = conn_slave_latency;
            }
            APP_PRINT_INFO3("app_handle_conn_param_update_evt update success: interval 0x%x, slave_latency 0x%x, supervision_timeout 0x%x",
                            conn_interval, conn_slave_latency, conn_supervision_timeout);
            dfu_notify_conn_para_update_req(conn_id, DFU_ARV_SUCCESS);
        }
        break;

    case GAP_CONN_PARAM_UPDATE_STATUS_FAIL:
        {
            APP_PRINT_ERROR1("app_handle_conn_param_update_evt update failed: cause 0x%x", cause);
            dfu_notify_conn_para_update_req(conn_id, DFU_ARV_FAIL_OPERATION);
        }
        break;

    case GAP_CONN_PARAM_UPDATE_STATUS_PENDING:
        {
            APP_PRINT_INFO0("app_handle_conn_param_update_evt update pending.");
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
            APP_PRINT_INFO1("GAP_MSG_LE_BOND_PASSKEY_DISPLAY:passkey %d", display_value);
            le_bond_passkey_display_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
        }
        break;

    case GAP_MSG_LE_BOND_USER_CONFIRMATION:
        {
            uint32_t display_value = 0;
            conn_id = gap_msg.msg_data.gap_bond_user_conf.conn_id;
            le_bond_get_display_key(conn_id, &display_value);
            APP_PRINT_INFO1("GAP_MSG_LE_BOND_USER_CONFIRMATION: passkey %d", display_value);
        }
        break;

    case GAP_MSG_LE_BOND_PASSKEY_INPUT:
        {
            uint32_t passkey = 888888;
            conn_id = gap_msg.msg_data.gap_bond_passkey_input.conn_id;
            APP_PRINT_INFO1("GAP_MSG_LE_BOND_PASSKEY_INPUT: conn_id %d", conn_id);
            le_bond_passkey_input_confirm(conn_id, passkey, GAP_CFM_CAUSE_ACCEPT);
        }
        break;

    case GAP_MSG_LE_BOND_OOB_INPUT:
        {
            uint8_t oob_data[GAP_OOB_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            conn_id = gap_msg.msg_data.gap_bond_oob_input.conn_id;
            APP_PRINT_INFO0("GAP_MSG_LE_BOND_OOB_INPUT");
            le_bond_set_param(GAP_PARAM_BOND_OOB_DATA, GAP_OOB_LEN, oob_data);
            le_bond_oob_input_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
        }
        break;

    default:
        APP_PRINT_ERROR1("app_handle_gap_msg: unknown subtype %d", p_gap_msg->subtype);
        break;
    }
}
/** @} */ /* End of group PERIPH_GAP_MSG */

/** @defgroup  PERIPH_GAP_CALLBACK GAP Callback Event Handler
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
#if F_BT_LE_5_0_SET_PHY_SUPPORT
    case GAP_MSG_LE_REMOTE_FEATS_INFO:
        {
            uint8_t remote_feats[8];
            APP_PRINT_INFO3("GAP_MSG_LE_REMOTE_FEATS_INFO: conn id %d, cause 0x%x, remote_feats %b",
                            p_data->p_le_remote_feats_info->conn_id,
                            p_data->p_le_remote_feats_info->cause,
                            TRACE_BINARY(8, p_data->p_le_remote_feats_info->remote_feats));
            if (p_data->p_le_remote_feats_info->cause == GAP_SUCCESS)
            {
                memcpy(remote_feats, p_data->p_le_remote_feats_info->remote_feats, 8);
                if (remote_feats[LE_SUPPORT_FEATURES_MASK_ARRAY_INDEX1] & LE_SUPPORT_FEATURES_LE_2M_MASK_BIT)
                {
                    APP_PRINT_INFO0("GAP_MSG_LE_REMOTE_FEATS_INFO: support 2M");
                }
                if (remote_feats[LE_SUPPORT_FEATURES_MASK_ARRAY_INDEX1] & LE_SUPPORT_FEATURES_LE_CODED_PHY_MASK_BIT)
                {
                    APP_PRINT_INFO0("GAP_MSG_LE_REMOTE_FEATS_INFO: support CODED");
                }
            }
        }
        break;
    case GAP_MSG_LE_PHY_UPDATE_INFO:
        APP_PRINT_INFO4("GAP_MSG_LE_PHY_UPDATE_INFO:conn_id %d, cause 0x%x, rx_phy %d, tx_phy %d",
                        p_data->p_le_phy_update_info->conn_id,
                        p_data->p_le_phy_update_info->cause,
                        p_data->p_le_phy_update_info->rx_phy,
                        p_data->p_le_phy_update_info->tx_phy);
        break;
#endif
    case GAP_MSG_LE_DATA_LEN_CHANGE_INFO:
        APP_PRINT_INFO3("GAP_MSG_LE_DATA_LEN_CHANGE_INFO: conn_id %d, tx octets 0x%x, max_tx_time 0x%x",
                        p_data->p_le_data_len_change_info->conn_id,
                        p_data->p_le_data_len_change_info->max_tx_octets,
                        p_data->p_le_data_len_change_info->max_tx_time);
        break;

    case GAP_MSG_LE_BOND_MODIFY_INFO:
        APP_PRINT_INFO1("GAP_MSG_LE_BOND_MODIFY_INFO: type 0x%x",
                        p_data->p_le_bond_modify_info->type);
        break;

    case GAP_MSG_LE_MODIFY_WHITE_LIST:
        APP_PRINT_INFO2("GAP_MSG_LE_MODIFY_WHITE_LIST: operation %d, cause 0x%x",
                        p_data->p_le_modify_white_list_rsp->operation,
                        p_data->p_le_modify_white_list_rsp->cause);
        break;

    default:
        APP_PRINT_ERROR1("app_gap_callback: unhandled cb_type 0x%x", cb_type);
        break;
    }
    return result;
}
/** @} */ /* End of group PERIPH_GAP_CALLBACK */

/******************************************************************
 * @fn        app_general_srv_cb
 * @brief     General service callbacks are handled in this function.
 * @param     p_data  - pointer to callback data
 * @return    cb_result
 * @retval    T_APP_RESULT
 */
static T_APP_RESULT app_general_srv_cb(T_SERVER_APP_CB_DATA *p_data)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;
    switch (p_data->eventId)
    {
    case PROFILE_EVT_SRV_REG_COMPLETE:// srv register result event.
        APP_PRINT_INFO1("PROFILE_EVT_SRV_REG_COMPLETE: result %d",
                        p_data->event_data.service_reg_result);
        break;

    case PROFILE_EVT_SEND_DATA_COMPLETE:
        APP_PRINT_INFO5("PROFILE_EVT_SEND_DATA_COMPLETE: conn_id %d, cause 0x%x, service_id %d, attrib_idx 0x%x, credits = %d",
                        p_data->event_data.send_data_result.conn_id,
                        p_data->event_data.send_data_result.cause,
                        p_data->event_data.send_data_result.service_id,
                        p_data->event_data.send_data_result.attrib_idx,
                        p_data->event_data.send_data_result.credits);
        if (p_data->event_data.send_data_result.cause == GAP_SUCCESS)
        {
            APP_PRINT_INFO0("PROFILE_EVT_SEND_DATA_COMPLETE success");
        }
        else
        {
            APP_PRINT_ERROR0("PROFILE_EVT_SEND_DATA_COMPLETE failed");
        }
        break;

    default:
        break;
    }
    return app_result;
}

static T_APP_RESULT app_bas_srv_cb(T_BAS_CALLBACK_DATA *p_data)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;
    switch (p_data->msg_type)
    {
    case SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION:
        {
            if (p_data->msg_data.notification_indification_index == BAS_NOTIFY_BATTERY_LEVEL_ENABLE)
            {
                APP_PRINT_INFO0("Battery level notification enable");
            }
            else if (p_data->msg_data.notification_indification_index ==
                     BAS_NOTIFY_BATTERY_LEVEL_DISABLE)
            {
                APP_PRINT_INFO0("Battery level notification disable");
            }
        }
        break;
    case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
        {
            /* update RCU battery level */
            g_bat_vol = 150;
            g_bas_battery_level = 100;

            APP_PRINT_INFO2("Battery_Level_Update BatVol is %d , BASBatteryLevel is %d", g_bat_vol,
                            g_bas_battery_level);

            bas_set_parameter(BAS_PARAM_BATTERY_LEVEL, 1, (uint8_t *)&g_bas_battery_level);
        }
        break;
    default:
        break;
    }
    return app_result;
}

static T_APP_RESULT app_dis_srv_cb(T_DIS_CALLBACK_DATA *p_data)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;
    switch (p_data->msg_type)
    {
    case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
        {
            if (p_data->msg_data.read_value_index == DIS_READ_MANU_NAME_INDEX)
            {
                const uint8_t DISManufacturerName[] = "Realtek BT";
                dis_set_parameter(DIS_PARAM_MANUFACTURER_NAME,
                                  sizeof(DISManufacturerName),
                                  (void *)DISManufacturerName);
            }
            else if (p_data->msg_data.read_value_index == DIS_READ_MODEL_NUM_INDEX)
            {
                const uint8_t DISModelNumber[] = "Model Nbr 0.9";
                dis_set_parameter(DIS_PARAM_MODEL_NUMBER,
                                  sizeof(DISModelNumber),
                                  (void *)DISModelNumber);
            }
            else if (p_data->msg_data.read_value_index == DIS_READ_SERIAL_NUM_INDEX)
            {
                const uint8_t DISSerialNumber[] = "RTKBeeSerialNum";
                dis_set_parameter(DIS_PARAM_SERIAL_NUMBER,
                                  sizeof(DISSerialNumber),
                                  (void *)DISSerialNumber);
            }
            else if (p_data->msg_data.read_value_index == DIS_READ_HARDWARE_REV_INDEX)
            {
                const uint8_t DISHardwareRev[] = "RTKBeeHardwareRev";
                dis_set_parameter(DIS_PARAM_HARDWARE_REVISION,
                                  sizeof(DISHardwareRev),
                                  (void *)DISHardwareRev);
            }
            else if (p_data->msg_data.read_value_index == DIS_READ_FIRMWARE_REV_INDEX)
            {
                const uint8_t DISFirmwareRev[] = "RTKBeeFirmwareRev";
                dis_set_parameter(DIS_PARAM_FIRMWARE_REVISION,
                                  sizeof(DISFirmwareRev),
                                  (void *)DISFirmwareRev);
            }
            else if (p_data->msg_data.read_value_index == DIS_READ_SOFTWARE_REV_INDEX)
            {
                const uint8_t DISSoftwareRev[] = "RTKBeeSoftwareRev";
                dis_set_parameter(DIS_PARAM_SOFTWARE_REVISION,
                                  sizeof(DISSoftwareRev),
                                  (void *)DISSoftwareRev);
            }
            else if (p_data->msg_data.read_value_index == DIS_READ_SYSTEM_ID_INDEX)
            {
                const uint8_t DISSystemID[DIS_SYSTEM_ID_LENGTH] = {0, 1, 2, 0, 0, 3, 4, 5};
                dis_set_parameter(DIS_PARAM_SYSTEM_ID,
                                  sizeof(DISSystemID),
                                  (void *)DISSystemID);
            }
            else if (p_data->msg_data.read_value_index == DIS_READ_IEEE_CERT_STR_INDEX)
            {
                const uint8_t DISIEEEDataList[] = "RTKBeeIEEEDatalist";
                dis_set_parameter(DIS_PARAM_IEEE_DATA_LIST,
                                  sizeof(DISIEEEDataList),
                                  (void *)DISIEEEDataList);
            }
            else if (p_data->msg_data.read_value_index == DIS_READ_PNP_ID_INDEX)
            {
                //uint8_t DISPnpID[DIS_PNP_ID_LENGTH] = {0x01,0xDA,0x0B,0x62,0x87,0x01,0x00};
                uint16_t version = 0x03; //VERSION_BUILD;

                uint8_t DISPnpID[DIS_PNP_ID_LENGTH] = {0x01, 0x5D, 0x00, 0x01, 0x00, (uint8_t)version, (uint8_t)(version >> 8)}; //VID_005D&PID_0001?

                dis_set_parameter(DIS_PARAM_PNP_ID,
                                  sizeof(DISPnpID),
                                  DISPnpID);
            }
        }
        break;
    default:
        break;
    }

    return app_result;
}


static T_APP_RESULT app_ota_srv_cb(T_OTA_CALLBACK_DATA *p_data)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;
    switch (p_data->msg_type)
    {
    case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
        {
            if (OTA_WRITE_CHAR_VAL == p_data->msg_data.write.opcode &&
                OTA_VALUE_ENTER == p_data->msg_data.write.u.value)
            {
                /*battery level is above 60 percent*/
                APP_PRINT_INFO0("Preparing switch into OTA mode");
                /*prepare to enter OTA mode, before switch action, we should disconnect first.*/
                switch_to_ota_mode_pending = true;
                le_disconnect(0);
            }
        }
        break;
    default:
        break;
    }
    return app_result;
}

static T_APP_RESULT app_dfu_srv_cb(T_DFU_CALLBACK_DATA *p_data)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;
    switch (p_data->msg_type)
    {
    case SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION:
        {
            if (p_data->msg_data.notification_indification_index == DFU_NOTIFY_ENABLE)
            {
                APP_PRINT_INFO0("dfu notification enable");
            }
            else if (p_data->msg_data.notification_indification_index ==
                     DFU_NOTIFY_DISABLE)
            {
                APP_PRINT_INFO0("dfu notification disable");
            }
        }
        break;
    case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
        {
            uint8_t dfu_write_opcode = p_data->msg_data.write.opcode;
            if (dfu_write_opcode == DFU_WRITE_ATTR_EXIT)
            {
                if (p_data->msg_data.write.write_attrib_index == INDEX_DFU_CONTROL_POINT_CHAR_VALUE)
                {
                    uint8_t control_point_opcode = *p_data->msg_data.write.p_value;
                    switch (control_point_opcode)
                    {
                    case DFU_OPCODE_VALID_FW:
                        {
                            T_IO_MSG dfu_valid_fw_msg;
                            dfu_valid_fw_msg.type = IO_MSG_TYPE_DFU_VALID_FW;
                            dfu_valid_fw_msg.u.param = p_data->conn_id;
                            if (app_send_msg_to_apptask(&dfu_valid_fw_msg) == false)
                            {
                                DBG_DIRECT("DFU send Valid FW msg fail!");
                            }
                        }
                        break;
                    case DFU_OPCODE_ACTIVE_IMAGE_RESET:
                        {
#if (ENABLE_AUTO_BANK_SWITCH == 1)
                            if (is_ota_support_bank_switch())
                            {
                                uint32_t ota_addr;
                                ota_addr = get_header_addr_by_img_id(OTA);
                                DFU_PRINT_INFO1("DFU_OPCODE_ACTIVE_IMAGE_RESET: Bank switch erase ota_addr=0x%x", ota_addr);
                                unlock_flash_bp_all();
                                flash_nor_erase_locked(ota_addr, FLASH_NOR_ERASE_SECTOR);
                                lock_flash_bp();
                            }
#endif
                            le_disconnect(0);
                            dfu_active_rst_pending = true;
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (dfu_write_opcode == DFU_WRITE_FAIL)
            {
                DBG_DIRECT("DFU FAIL!");
            }
            else if (dfu_write_opcode == DFU_WRITE_ATTR_ENTER)
            {
                // application can add check conditions before start dfu procefure
                //if check fail, return some error code except APP_RESULT_SUCCESS to exit dfu procedure
                //app_result = APP_RESULT_REJECT;
                //APP_PRINT_INFO0("exit dfu procedure");
            }
            else
            {
            }
        }
        break;
    default:
        break;
    }

    return app_result;
}

/** @defgroup  PERIPH_SEVER_CALLBACK Profile Server Callback Event Handler
    * @brief Handle profile server callback event
    * @{
    */
/**
    * @brief    All the BT Profile service callback events are handled in this function
    * @note     Then the event handling function shall be called according to the
    *           service_id
    * @param    service_id  Profile service ID
    * @param    p_data      Pointer to callback data
    * @return   T_APP_RESULT, which indicates the function call is successful or not
    * @retval   APP_RESULT_SUCCESS  Function run successfully
    * @retval   others              Function run failed, and return number indicates the reason
    */
T_APP_RESULT app_profile_callback(T_SERVER_ID service_id, void *p_data)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;
    if (service_id == SERVICE_PROFILE_GENERAL_ID)
    {
        app_result = app_general_srv_cb((T_SERVER_APP_CB_DATA *)p_data);
    }
    else if (service_id == g_bas_service_id)
    {
        app_result = app_bas_srv_cb((T_BAS_CALLBACK_DATA *)p_data);
    }
    else if (service_id == g_dis_service_id)
    {
        app_result = app_dis_srv_cb((T_DIS_CALLBACK_DATA *)p_data);
    }
    else if (service_id == g_ota_service_id)
    {
        app_result = app_ota_srv_cb((T_OTA_CALLBACK_DATA *)p_data);
    }
    else if (service_id == g_dfu_service_id)
    {
        app_result = app_dfu_srv_cb((T_DFU_CALLBACK_DATA *)p_data);
    }

    return app_result;
}

/** @} */ /* End of group PERIPH_SEVER_CALLBACK */
/** @} */ /* End of group PERIPH_APP */
