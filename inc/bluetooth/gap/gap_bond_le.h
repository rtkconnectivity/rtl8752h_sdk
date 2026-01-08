/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    gap_bond_le.h
  * @brief   This file contains all the function prototypes for the GAP bond and pairing
  *          related functions.
  * @details
  * @author  ranhui
  * @date    2016-02-18
  * @version v1.0
  * *************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef GAP_BOND_LE_H
#define GAP_BOND_LE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <gap_le.h>

/** @defgroup GAP_LE_Bond_Manager GAP LE Bond Manager
  * @brief GAP LE Bond Manager.
  * @{
  */
/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup GAP_LE_BOND_MANAGER_Exported_Types GAP LE Bond Manager Exported Types
  * @{
  */

/** @brief LE bond parameter types */
typedef enum
{
    GAP_PARAM_BOND_OOB_DATA             = 0x210,//!< Out of Band data. Read/Write. Size is 16 octets @ref GAP_OOB_LEN. Default value is all zero.
    GAP_PARAM_BOND_FIXED_PASSKEY        = 0x211,//!< The fixed passkey for MITM protection. Read/Write. Size is 4 octets. Range is 000,000 - 999,999. Default value is 0.
    GAP_PARAM_BOND_FIXED_PASSKEY_ENABLE = 0x212,//!< The fixed passkey available for pairing. Read/Write. Size is 1 octet. Default value is 0 (disabled).
    GAP_PARAM_BOND_SEC_REQ_ENABLE       = 0x213,//!< Whether to start pairing procedure or encryption procedure when connected. Read/Write. Size is 1 octet. Default value is 0 (disabled).
    GAP_PARAM_BOND_SEC_REQ_REQUIREMENT  = 0x214,/**< Authentication requirements. Read/Write. Size is 2 octets. Value is @ref BOND_AUTHEN_REQ.
                                                     Default value is @ref GAP_AUTHEN_BIT_BONDING_FLAG. */
    GAP_PARAM_BOND_MIN_KEY_SIZE         = 0x215,//!< Minimum LTK size required. Read/Write. Size is 1 octet. Range is 7 - 16. Default value is 7.
    GAP_PARAM_BOND_GEN_LOCAL_IRK_AUTO   = 0x216,//!< Whether to auto generate local IRK. Write only. Size is 1 octet. Default value is 0 (disabled).
    GAP_PARAM_BOND_SET_LOCAL_IRK        = 0x217,//!< Set local IRK. Write only. Size is 16 octets. Default value is all zero.
    GAP_PARAM_BOND_KEY_MANAGER          = 0x218,/**< The way to implement bonding key management. Write only. Size is 1 octet. Value range:
                                                     - 0: GAP implements bonding key management.
                                                     - 1: GAP does not implement bonding key management.
                                                     - 2: GAP implements bonding key management. When receiving @ref GAP_MSG_LE_BOND_KEY_REQ,
                                                          APP shall handle the message.
                                                     Default value is 0. */
    GAP_PARAM_BOND_SIGN_KEY_FLAG        = 0x219,//!< Whether to distribute CSRK when pairing. Write only. Size is 1 octet. Default value is 0 (disabled).
    GAP_PARAM_BOND_CCCD_STORAGE         = 0x21A,//!< Whether to store CCCD information. Write only. Size is 1 octet. Default value is 1 (enabled).
} T_LE_BOND_PARAM_TYPE;


/** @brief LE local Out of Band data */
typedef struct
{
    uint8_t     present;
    uint8_t     rand[16];
    uint8_t     confirm[16];
    uint8_t     bd_addr_to[6];
    uint8_t     local_sk[32];
    uint8_t     local_pk[64];
} T_GAP_LE_LOCAL_OOB_DATA;

/** @brief LE peer Out of Band data */
typedef struct
{
    uint8_t     present;
    uint8_t     rand[16];
    uint8_t     confirm[16];
    uint8_t     bd_addr_from[6];
} T_GAP_LE_PEER_OOB_DATA;

/** End of GAP_LE_BOND_MANAGER_Exported_Types
* @}
*/

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup GAP_LE_BONDMGR_Exported_Functions GAP LE Bond Manager Exported Functions
  * @{
  */

/**
 * @brief   Set a GAP Bond Manager Parameter.
 *
 * This function can be called with a bond parameter type @ref T_LE_BOND_PARAM_TYPE and it will set the bond parameter.
 * The 'p_value' field must point to an appropriate data type that meets the requirements for the corresponding parameter type.
 * (For example, if required data length for parameter type is 2 octets, p_value should be cast to a pointer of uint16_t.)
 *
 * @param[in]   param   @ref T_LE_BOND_PARAM_TYPE.
 * @param[in]   len     Length of data to write.
 * @param[in]   p_value Pointer to data to write.
 *
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval Others   Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        ...
        uint8_t  auth_use_fix_passkey = false;
        uint32_t auth_fix_passkey = 0;
        uint8_t  auth_sec_req_enable = false;
        uint16_t auth_sec_req_flags = GAP_AUTHEN_BIT_NONE;

        le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY, sizeof(auth_fix_passkey), &auth_fix_passkey);
        le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY_ENABLE, sizeof(auth_use_fix_passkey),
                         &auth_use_fix_passkey);
        le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_ENABLE, sizeof(auth_sec_req_enable), &auth_sec_req_enable);
        le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_REQUIREMENT, sizeof(auth_sec_req_flags),
                         &auth_sec_req_flags);
        ....
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_set_param(T_LE_BOND_PARAM_TYPE param, uint8_t len, void *p_value);

/**
 * @brief   Get a GAP Bond Manager Parameter.
 *
 * This function can be called with a bond parameter type @ref T_LE_BOND_PARAM_TYPE and it will get the bond parameter.
 * The 'p_value' field must point to an appropriate data type that meets the requirements for the corresponding parameter type.
 * (For example, if required data length for parameter type is 2 octets, p_value should be cast to a pointer of uint16_t.)
 *
 * @param[in]   param   @ref T_LE_BOND_PARAM_TYPE.
 * @param[in,out]  p_value Pointer to location to get the value.
 *
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval Others   Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        uint8_t seq_req;
        le_bond_get_param(GAP_PARAM_BOND_SEC_REQ_ENABLE, &seq_req);
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_get_param(T_LE_BOND_PARAM_TYPE param, void *p_value);

/**
 * @brief   Start pairing procedure or encryption procedure.
 *
 * If sending request operation is successful, result will be returned in one of the following ways:
 * - In the default situation, or when @ref le_gap_msg_info_way (true) has been called, APP will be notified
 *   by message @ref GAP_MSG_LE_AUTHEN_STATE_CHANGE with new_state @ref BOND_PAIRING_STATE_DEFINES.
 * - When @ref le_gap_msg_info_way (false) has been called, APP will be notified with the callback registered
 *   by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_GAP_STATE_MSG.
 *
 * @param[in] conn_id Connection ID.
 *
 * @return The result of sending request.
 * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
 * @retval Others Sending request operation is failed.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
       uint8_t conn_id = 0;
       le_bond_pair(conn_id);
    }

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
 * \endcode
 */
T_GAP_CAUSE le_bond_pair(uint8_t conn_id);

/**
 * @brief   Get type of LE pair procedure.
 *
 * @note The API should only be used when receive @ref GAP_MSG_LE_BOND_JUST_WORK,
 * @ref GAP_MSG_LE_BOND_PASSKEY_DISPLAY, @ref GAP_MSG_LE_BOND_PASSKEY_INPUT,
 * @ref GAP_MSG_LE_BOND_USER_CONFIRMATION, @ref GAP_MSG_LE_BOND_OOB_INPUT.
 *
 * \xrefitem Added_API_2_13_1_0 "Added Since 2.13.1.0" "Added API"
 * @param[in]     conn_id       Connection ID.
 * @param[in,out] p_value       Pointer to location to get the value.
 *                              Value of LE pair procedure type:
 *                              - 0: LE Legacy Pairing.
 *                              - 1: LE Secure Connections.
 *                              - All other values: Reserved for future use.
 *
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval Others               Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_handle_gap_msg(T_IO_MSG *p_gap_msg)
    {
      ...
    case GAP_MSG_LE_BOND_JUST_WORK:
        {
            ...
            if (le_bond_get_pair_procedure_type(conn_id, &le_pair_proc_type) == GAP_CAUSE_SUCCESS)
            {
            ...
        }
        break;

    case GAP_MSG_LE_BOND_PASSKEY_DISPLAY:
        {
            ...
            if (le_bond_get_pair_procedure_type(conn_id, &le_pair_proc_type) == GAP_CAUSE_SUCCESS)
            {
            ...
        }
        break;

    case GAP_MSG_LE_BOND_PASSKEY_INPUT:
        {
            ...
            if (le_bond_get_pair_procedure_type(conn_id, &le_pair_proc_type) == GAP_CAUSE_SUCCESS)
            {
            ...
        }
        break;

    case GAP_MSG_LE_BOND_USER_CONFIRMATION:
        {
            ...
            if (le_bond_get_pair_procedure_type(conn_id, &le_pair_proc_type) == GAP_CAUSE_SUCCESS)
            {
            ...
        }
        break;

    case GAP_MSG_LE_BOND_OOB_INPUT:
        {
            ...
            if (le_bond_get_pair_procedure_type(conn_id, &le_pair_proc_type) == GAP_CAUSE_SUCCESS)
            {
            ...
        }
        break;
      ...
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_get_pair_procedure_type(uint8_t conn_id, uint8_t *p_value);

/**
 * @brief   When receiving @ref GAP_MSG_LE_BOND_PASSKEY_DISPLAY or @ref GAP_MSG_LE_BOND_USER_CONFIRMATION,
 *          APP shall call this API to get the passkey information.
 *
 * @param[in] conn_id   Connection ID.
 * @param[in,out] p_key    Pointer to passkey value to be displayed as a 6 digit decimal number.
 *
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval Others               Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_handle_gap_msg(T_IO_MSG *p_io_msg)
    {
      T_LE_GAP_MSG bt_msg;
      uint8_t conn_id;

      memcpy(&bt_msg, &p_io_msg->u.param, sizeof(p_io_msg->u.param));

      switch (p_io_msg->subtype)
      {
      ...
      case GAP_MSG_LE_BOND_PASSKEY_DISPLAY:
          {
            uint32_t display_value = 0;
            conn_id = gap_msg.msg_data.gap_bond_passkey_display.conn_id;
            le_bond_get_display_key(conn_id, &display_value);
            APP_PRINT_INFO1("GAP_MSG_LE_BOND_PASSKEY_DISPLAY: passkey %d", display_value);
            le_bond_passkey_display_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
          }
          break;

        case GAP_MSG_LE_BOND_USER_CONFIRMATION:
          {
            uint32_t display_value = 0;
            conn_id = gap_msg.msg_data.gap_bond_user_conf.conn_id;
            le_bond_get_display_key(conn_id, &display_value);
            APP_PRINT_INFO1("GAP_MSG_LE_BOND_USER_CONFIRMATION: passkey %d", display_value);
            le_bond_user_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
          }
          break;
      ...
      }
      ...
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_get_display_key(uint8_t conn_id, uint32_t *p_key);

/**
 * @brief   When receiving @ref GAP_MSG_LE_BOND_PASSKEY_INPUT,
 *          APP shall call this API to decide whether to pair with Passkey Entry and input passkey.
 *
 * @param[in] conn_id Connection ID.
 * @param[in] passcode  User input passkey.
 * @param[in] cause     @ref T_GAP_CFM_CAUSE.
 *
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval Others               Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_handle_gap_msg(T_IO_MSG *p_io_msg)
    {
      T_LE_GAP_MSG bt_msg;
      uint8_t conn_id;

      memcpy(&bt_msg, &p_io_msg->u.param, sizeof(p_io_msg->u.param));

      switch (p_io_msg->subtype)
      {
      ...
      case GAP_MSG_LE_BOND_PASSKEY_INPUT:
          {
              conn_id = bt_msg.msg_data.gap_bond_passkey_input.conn_id;
              APP_PRINT_INFO0("GAP_MSG_LE_BOND_PASSKEY_INPUT");
              uint32_t passKey = 888888;
              le_bond_passkey_input_confirm(conn_id, passKey, GAP_CFM_CAUSE_ACCEPT);
          }
          break;
      ...
      }
      ...
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_passkey_input_confirm(uint8_t conn_id, uint32_t passcode,
                                          T_GAP_CFM_CAUSE cause);

/**
 * @brief   When receiving @ref GAP_MSG_LE_BOND_OOB_INPUT,
 *          APP shall call @ref le_bond_set_param with parameter @ref GAP_PARAM_BOND_OOB_DATA
 *          and call this API to decide whether to pair with OOB and input OOB data.
 *
 * @param[in] conn_id Connection ID.
 * @param[in] cause   @ref T_GAP_CFM_CAUSE.
 *
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval Others               Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_handle_gap_msg(T_IO_MSG *p_io_msg)
    {
      T_LE_GAP_MSG bt_msg;
      uint8_t conn_id;

      memcpy(&bt_msg, &p_io_msg->u.param, sizeof(p_io_msg->u.param));

      switch (p_io_msg->subtype)
      {
      ...
      case GAP_MSG_LE_BOND_OOB_INPUT:
          {
              conn_id = bt_msg.msg_data.gap_bond_oob_input.conn_id;
              APP_PRINT_INFO0("GAP_MSG_LE_BOND_OOB_INPUT");
              uint8_t ooBData[GAP_OOB_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
              le_bond_set_param(GAP_PARAM_BOND_OOB_DATA, GAP_OOB_LEN, ooBData);
              le_bond_oob_input_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
          }
          break;
      ...
      }
      ...
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_oob_input_confirm(uint8_t conn_id, T_GAP_CFM_CAUSE cause);

/**
 * @brief   When receiving @ref GAP_MSG_LE_BOND_JUST_WORK,
 *          APP shall call this API to decide whether to pair with Just Works.
 *
 * @param[in] conn_id   Connection ID.
 * @param[in] cause     @ref T_GAP_CFM_CAUSE.
 *
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval Others               Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_handle_gap_msg(T_IO_MSG *p_io_msg)
    {
        T_LE_GAP_MSG bt_msg;
        uint8_t conn_id;

        memcpy(&bt_msg, &p_io_msg->u.param, sizeof(p_io_msg->u.param));

        switch (p_io_msg->subtype)
        {
        ...
        case GAP_MSG_LE_BOND_JUST_WORK:
        {
            conn_id = gap_msg.msg_data.gap_bond_just_work_conf.conn_id;
            le_bond_just_work_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
            APP_PRINT_INFO0("GAP_MSG_LE_BOND_JUST_WORK");
        }
        break;
        ...
        }
        ...
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_just_work_confirm(uint8_t conn_id, T_GAP_CFM_CAUSE cause);

/**
 * @brief   When receiving @ref GAP_MSG_LE_BOND_PASSKEY_DISPLAY,
 *          APP shall call @ref le_bond_get_display_key and call this API to decide
 *          whether to pair with Passkey Entry.
 *
 * @param[in] conn_id   Connection ID.
 * @param[in] cause     @ref T_GAP_CFM_CAUSE.
 *
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval Others               Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_handle_gap_msg(T_IO_MSG *p_io_msg)
    {
        T_LE_GAP_MSG bt_msg;
        uint8_t conn_id;

        memcpy(&bt_msg, &p_io_msg->u.param, sizeof(p_io_msg->u.param));

        switch (p_io_msg->subtype)
        {
        ...
        case GAP_MSG_LE_BOND_PASSKEY_DISPLAY:
        {
        uint32_t display_value = 0;
        conn_id = gap_msg.msg_data.gap_bond_passkey_display.conn_id;
        le_bond_get_display_key(conn_id, &display_value);
        APP_PRINT_INFO1("GAP_MSG_LE_BOND_PASSKEY_DISPLAY: passkey %d", display_value);
        le_bond_passkey_display_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
        }
        break;
        ...
        }
        ...
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_passkey_display_confirm(uint8_t conn_id, T_GAP_CFM_CAUSE cause);

/**
 * @brief   When receiving @ref GAP_MSG_LE_BOND_USER_CONFIRMATION,
 *          APP shall call @ref le_bond_get_display_key and call this API to decide
 *          whether to pair with Numeric Comparison.
 *
 * @param[in] conn_id   Connection ID.
 * @param[in] cause     @ref T_GAP_CFM_CAUSE.
 *
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval Others               Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_handle_gap_msg(T_IO_MSG *p_io_msg)
    {
        T_LE_GAP_MSG bt_msg;
        uint8_t conn_id;

        memcpy(&bt_msg, &p_io_msg->u.param, sizeof(p_io_msg->u.param));

        switch (p_io_msg->subtype)
        {
        ...
        case GAP_MSG_LE_BOND_USER_CONFIRMATION:
          {
            uint32_t display_value = 0;
            conn_id = gap_msg.msg_data.gap_bond_user_conf.conn_id;
            le_bond_get_display_key(conn_id, &display_value);
            APP_PRINT_INFO1("GAP_MSG_LE_BOND_USER_CONFIRMATION: passkey %d", display_value);
            le_bond_user_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
          }
          break;
        ...
        }
        ...
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_user_confirm(uint8_t conn_id, T_GAP_CFM_CAUSE cause);

/**
 * @brief   Send keypress notification.
 *
 * If sending request operation is successful, the sending result will be returned by callback
 * registered by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_KEYPRESS_NOTIFY.
 *
 * @param[in] conn_id   Connection ID.
 * @param[in] type      @ref T_GAP_KEYPRESS_NOTIF_TYPE.
 *
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval Others               Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_KEYPRESS_NOTIFY:
           APP_PRINT_INFO2("GAP_MSG_LE_KEYPRESS_NOTIFY:conn %d, cause 0x%x",
                           cb_data.p_le_keypress_notify_rsp->conn_id, cb_data.p_le_keypress_notify_rsp->cause);
           break;
        ...
        }
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_keypress_notify(uint8_t conn_id, T_GAP_KEYPRESS_NOTIF_TYPE type);

/**
 * @brief   Set local key distribution field.
 *
 * @param[in] init_dist   Initiator key distribution.
 * @param[in] rsp_dist    Responder key distribution.
 *
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval Others               Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        uint8_t init_dist;
        uint8_t rsp_dist;
        le_bond_cfg_local_key_distribute(init_dist, rsp_dist);
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_cfg_local_key_distribute(uint8_t init_dist, uint8_t rsp_dist);

/**
 * @brief   Initialize local Out of Band data.
 *
 * @note APP should call this API if OOB for LE Secure Connections pairing will be used.
 *
 * @param[in] p_ecc_rand_in     Pointer to ecc rand.
 * @param[in] local_rand        Pointer to local rand.
 * @param[in,out] p_local_oob_data  Pointer to local OOB data.
 *
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval Others               Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        T_GAP_CAUSE ret;
        uint64_t ecc_rand_in[4];
        uint8_t local_rand[16];

        // APP set parameters value according to requirements

        if (idx == 1)
        {
            ret = le_bond_sc_local_oob_init(NULL, local_rand, &local_oob_data);
        }
        else
        {
            ret = le_bond_sc_local_oob_init(ecc_rand_in, local_rand, &local_oob_data);
        }
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_sc_local_oob_init(uint64_t *p_ecc_rand_in, uint8_t *local_rand,
                                      T_GAP_LE_LOCAL_OOB_DATA *p_local_oob_data);

/**
 * @brief   Initialize peer Out of Band data.
 *
 * @param[in] p_peer_oob_data  Pointer to peer OOB data.
 *
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval Others               Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        T_GAP_CAUSE ret;
        T_GAP_LE_PEER_OOB_DATA peer_oob_data;
        uint8_t bd_addr[6];
        uint8_t confirmation[16];
        uint8_t random_number[16];

        // APP set parameters value according to requirements

        peer_oob_data.present = peer_oob_data_present_flag;

        memcpy(peer_oob_data.rand, random_number, 16);
        memcpy(peer_oob_data.confirm, confirmation, 16);
        memcpy(peer_oob_data.bd_addr_from, bd_addr, 6);

        ret = le_bond_sc_peer_oob_init(&peer_oob_data);

        // APP call gap_set_param(GAP_PARAM_BOND_OOB_ENABLED,), and value is same as value of peer_oob_data.present
        // APP call gap_set_pairable_mode()
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_sc_peer_oob_init(T_GAP_LE_PEER_OOB_DATA *p_peer_oob_data);

/**
 * @brief   Erase all link keys of bonded devices.
 *
 * If erase operation is successful, the operation result will be returned by callback
 * registered by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_BOND_MODIFY_INFO.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        le_bond_clear_all_keys();
    }

    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA *p_data = (T_LE_CB_DATA *)p_cb_data;

        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_BOND_MODIFY_INFO:
            APP_PRINT_INFO1("GAP_MSG_LE_BOND_MODIFY_INFO: type 0x%x",
                            p_data->p_le_bond_modify_info->type);
            break;
        ...
        }
    }
 * \endcode
 */
void le_bond_clear_all_keys(void);

/**
 * @brief   Delete bond information by index.
 *
 * If delete operation is successful, the operation result will be returned by callback
 * registered by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_BOND_MODIFY_INFO.
 *
 * @param[in] idx   Index of key entry.
 *
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval Others               Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        T_GAP_CAUSE cause = le_bond_delete_by_idx(idx);
    }

    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA *p_data = (T_LE_CB_DATA *)p_cb_data;

        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_BOND_MODIFY_INFO:
            APP_PRINT_INFO1("GAP_MSG_LE_BOND_MODIFY_INFO: type 0x%x",
                            p_data->p_le_bond_modify_info->type);
            break;
        ...
        }
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_delete_by_idx(uint8_t idx);

/**
 * @brief   Delete bond information by Bluetooth device address and address type.
 *
 * If delete operation is successful, the operation result will be returned by callback
 * registered by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_BOND_MODIFY_INFO.
 *
 * @param[in] bd_addr   Bonded Bluetooth device address.
 * @param[in] bd_type   @ref T_GAP_REMOTE_ADDR_TYPE.
 *
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval Others               Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        T_GAP_CAUSE cause = le_bond_delete_by_bd(bd_addr, bd_type);
    }

    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA *p_data = (T_LE_CB_DATA *)p_cb_data;

        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_BOND_MODIFY_INFO:
            APP_PRINT_INFO1("GAP_MSG_LE_BOND_MODIFY_INFO: type 0x%x",
                            p_data->p_le_bond_modify_info->type);
            break;
        ...
        }
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_delete_by_bd(uint8_t *bd_addr, T_GAP_REMOTE_ADDR_TYPE bd_type);

/**
 * @brief   Get the security level.
 *
 * @param[in] conn_id   Connection ID.
 * @param[in,out] p_type   Pointer to location to get the security level.
 *
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval Others   Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        T_GAP_CAUSE cause;
        T_GAP_SEC_LEVEL sec_level;
        uint8_t conn_id;

        cause = le_bond_get_sec_level(conn_id, &sec_level);
        if(sec_level == GAP_SEC_LEVEL_NO)
        {
            data_uart_print("GAP_SEC_LEVEL_NO\r\n");
        }
        else if(sec_level == GAP_SEC_LEVEL_UNAUTHEN)
        {
            data_uart_print("GAP_SEC_LEVEL_UNAUTHEN\r\n");
        }
        else if(sec_level == GAP_SEC_LEVEL_AUTHEN)
        {
            data_uart_print("GAP_SEC_LEVEL_AUTHEN\r\n");
        }
        else if(sec_level == GAP_SEC_LEVEL_SC_UNAUTHEN)
        {
            data_uart_print("GAP_SEC_LEVEL_SC_UNAUTHEN\r\n");
        }
        else if(sec_level == GAP_SEC_LEVEL_SC_AUTHEN)
        {
            data_uart_print("GAP_SEC_LEVEL_SC_AUTHEN\r\n");
        }
    }
 * \endcode
 */
T_GAP_CAUSE le_bond_get_sec_level(uint8_t conn_id, T_GAP_SEC_LEVEL *p_type);

/**
 * @brief   When receiving @ref GAP_MSG_LE_AUTHEN_RESULT_IND,
 *          APP can call this API to confirm authentication result.
 *
 * @param[in] bd_addr          Remote bluetooth device address.
 * @param[in] remote_addr_type Remote bluetooth device address type.
 * @param[in] key_type         Key type.
 * @param[in] cause            User confirm result.
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval GAP_CAUSE_NON_CONN   Operation failure. No connection.
 */
T_GAP_CAUSE le_bond_authen_result_confirm(uint8_t *bd_addr, T_GAP_REMOTE_ADDR_TYPE remote_addr_type,
                                          T_GAP_KEY_TYPE key_type, T_GAP_CFM_CAUSE cause);

/**
 * @brief   When receiving @ref GAP_MSG_LE_AUTHEN_KEY_REQ_IND,
 *          APP can call this API to confirm authentication key request.
 *
 * @param[in] bd_addr          Remote bluetooth device address.
 * @param[in] remote_addr_type Remote bluetooth device address type.
 * @param[in] key_len          Key length to set.
 * @param[in] p_key            Pointer to key to set.
 * @param[in] key_type         Key type.
 * @param[in] cause            User confirm result.
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval GAP_CAUSE_NON_CONN   Operation failure. No connection.
 */
T_GAP_CAUSE le_bond_authen_key_req_confirm(uint8_t *bd_addr,
                                           T_GAP_REMOTE_ADDR_TYPE remote_addr_type,
                                           uint8_t key_len, uint8_t *p_key, T_GAP_KEY_TYPE key_type,
                                           T_GAP_CFM_CAUSE cause);

/**
 * @brief   When receiving @ref GAP_MSG_LE_GATT_SERVER_STORE_IND,
 *          APP can call this API to confirm storage indication.
 *
 * @param[in] bd_addr          Operation @ref T_GATT_STORE_OPCODE.
 * @param[in] bd_addr          Remote bluetooth device address.
 * @param[in] remote_addr_type Remote bluetooth device address type.
 * @param[in] data_len         Data length to set.
 * @param[in] data             Pointer to Data to set.
 * @param[in] cause            User confirm result.
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval GAP_CAUSE_NON_CONN   Operation failure. No connection.
 */
T_GAP_CAUSE le_bond_gatt_server_store_confirm(T_GATT_STORE_OPCODE op, uint8_t *bd_addr,
                                              T_GAP_REMOTE_ADDR_TYPE remote_addr_type,
                                              uint8_t data_len, uint8_t *data, T_GAP_CFM_CAUSE cause);

/** End of GAP_LE_BONDMGR_Exported_Functions
  * @}
  */

/** End of GAP_LE_Bond_Manager
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* GAP_BOND_LE_H */
