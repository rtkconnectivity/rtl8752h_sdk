/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    gap_privacy.h
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
#ifndef GAP_PRIVACY_H
#define GAP_PRIVACY_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <gap_le.h>
#include <gap_conn_le.h>
#include <gap_storage_le.h>


/** @defgroup GAP_PRIVACY_MODULE GAP LE Privacy
  * @brief GAP LE privacy
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup Gap_Privacy_Exported_Types GAP Privacy Exported Types
  * @{
  */

/* Define the privacy callback type. */
#define GAP_MSG_LE_PRIVACY_RESOLUTION_STATUS_INFO       0x00 /**< Information msg type for resolution status.
                                                                  The structure of callback data is @ref T_LE_PRIVACY_RESOLUTION_STATUS_INFO. */
#define GAP_MSG_LE_PRIVACY_SET_RESOLV_PRIV_ADDR_TIMEOUT 0x01 /**< Response msg type for @ref le_privacy_set_resolv_priv_addr_timeout.
                                                                  The structure of callback data is @ref T_LE_PRIVACY_SET_RESOLV_PRIV_ADDR_TIMEOUT_RSP. */
#define GAP_MSG_LE_PRIVACY_MODIFY_RESOLV_LIST           0x02 /**< Response msg type for @ref le_privacy_modify_resolv_list.
                                                                  The structure of callback data is @ref T_LE_PRIVACY_MODIFY_RESOLV_LIST_RSP. */
#define GAP_MSG_LE_PRIVACY_READ_PEER_RESOLV_ADDR        0x03 /**< Response msg type for @ref le_privacy_read_peer_resolv_addr.
                                                                  The structure of callback data is @ref T_LE_PRIVACY_READ_PEER_RESOLV_ADDR_RSP. */
#define GAP_MSG_LE_PRIVACY_READ_LOCAL_RESOLV_ADDR       0x04 /**< Response msg type for @ref le_privacy_read_local_resolv_addr.
                                                                  The structure of callback data is @ref T_LE_PRIVACY_READ_LOCAL_RESOLV_ADDR_RSP. */
#define GAP_MSG_LE_PRIVACY_SET_MODE                     0x05 /**< Response msg type for @ref le_privacy_set_mode.
                                                                  The structure of callback data is @ref T_LE_PRIVACY_SET_MODE_RSP. */


/** @brief Define the privacy parameter type. */
typedef enum
{
    GAP_PARAM_PRIVACY_TIMEOUT           = 0x300, /**< Time Controller uses a Resolvable Private Address before a new resolvable
                                                      private address is generated and starts being used. Read/Write.
                                                      Size is 2 octets. Value range: 0x0001 to 0x0E10 (1 s to 1 hour). Units: Second.
                                                      Default value is 0x0384 (900 s or 15 minutes). */
    GAP_PARAM_PRIVACY_RESOLUTION_STATE  = 0x301, //!< Resolution state. Read only. Size is 1 octet. Value is @ref T_LE_PRIVACY_STATE.
    GAP_PARAM_PRIVACY_RESOLV_LIST_SIZE  = 0x302, /**< Total number of entries in the resolving list that can be
                                                      stored in Controller. Read only. Size is 1 octet. */
} T_LE_PRIVACY_PARAM_TYPE;

/** @brief Define the privacy state. */
typedef enum
{
    LE_PRIVACY_RESOLUTION_DISABLED,     //!< Resolution disabled.
    LE_PRIVACY_RESOLUTION_DISABLING,    //!< Resolution disabling.
    LE_PRIVACY_RESOLUTION_ENABLING,     //!< Resolution enabling.
    LE_PRIVACY_RESOLUTION_ENABLED       //!< Resolution enabled.
} T_LE_PRIVACY_STATE;

/** @brief LE resolution status info. */
typedef struct
{
    T_LE_PRIVACY_STATE   status;
    uint16_t             cause;
} T_LE_PRIVACY_RESOLUTION_STATUS_INFO;

/** @brief  Response of LE set resolvable private address timeout request.*/
typedef struct
{
    uint16_t             cause;
} T_LE_PRIVACY_SET_RESOLV_PRIV_ADDR_TIMEOUT_RSP;

/** @brief  Response of LE modify resolvable list request.*/
typedef struct
{
    T_GAP_RESOLV_LIST_OP operation;
    uint16_t             cause;
} T_LE_PRIVACY_MODIFY_RESOLV_LIST_RSP;

/** @brief  Response of LE read peer resolvable address request.*/
typedef struct
{
    uint16_t             cause;
    uint8_t              peer_resolv_addr[6];
} T_LE_PRIVACY_READ_PEER_RESOLV_ADDR_RSP;

/** @brief  Response of LE read local resolvable address request.*/
typedef struct
{
    uint16_t             cause;
    uint8_t              local_resolv_addr[6];
} T_LE_PRIVACY_READ_LOCAL_RESOLV_ADDR_RSP;

/** @brief  Response of set privacy mode.*/
typedef struct
{
    uint16_t             cause;
} T_LE_PRIVACY_SET_MODE_RSP;

/** @brief LE privacy callback data.  */
typedef union
{
    T_LE_PRIVACY_RESOLUTION_STATUS_INFO            le_privacy_resolution_status_info;
    T_LE_PRIVACY_SET_RESOLV_PRIV_ADDR_TIMEOUT_RSP *p_le_privacy_set_resolv_priv_addr_timeout_rsp;
    T_LE_PRIVACY_MODIFY_RESOLV_LIST_RSP           *p_le_privacy_modify_resolv_list_rsp;
    T_LE_PRIVACY_READ_PEER_RESOLV_ADDR_RSP        *p_le_privacy_read_peer_resolv_addr_rsp;
    T_LE_PRIVACY_READ_LOCAL_RESOLV_ADDR_RSP       *p_le_privacy_read_local_resolv_addr_rsp;
    T_LE_PRIVACY_SET_MODE_RSP                     *p_le_privacy_set_mode_rsp;
} T_LE_PRIVACY_CB_DATA;

/** End of Gap_Privacy_Exported_Types
* @}
*/

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup Gap_Privacy_Exported_Functions GAP Privacy Exported Functions
  * @{
  */

/**
  * @brief Callback for GAP to notify APP.
  * @param[in] msg_type Callback msg type.
  * @param[in] msg_data Message data @ref T_LE_PRIVACY_CB_DATA.
  * @return    Result.
  * @retval    result @ref T_APP_RESULT.
  */
typedef T_APP_RESULT(*P_FUN_PRIVACY_CB)(uint8_t msg_type, T_LE_PRIVACY_CB_DATA msg_data);

/**
  * @brief  Set a LE privacy parameter.
  *
  * This function can be called with a LE privacy parameter type @ref T_LE_PRIVACY_PARAM_TYPE and it will set the privacy parameter.
  * The 'p_value' field must point to an appropriate data type that meets the requirements for the corresponding parameter type.
  * (For example, if required data length for parameter type is 2 octets, p_value should be cast to a pointer of uint16_t.)
  *
  * @param[in]  param LE privacy parameter type @ref T_LE_PRIVACY_PARAM_TYPE.
  * @param[in]  len Length of data to write.
  * @param[in]  p_value Pointer to data to write.
  * @return Operation result.
  * @retval GAP_CAUSE_SUCCESS Operation success.
  * @retval Others Operation failure.
  */
T_GAP_CAUSE le_privacy_set_param(T_LE_PRIVACY_PARAM_TYPE param, uint8_t len, void *p_value);

/**
  * @brief  Get an LE privacy parameter.
  *
  * This function can be called with a LE privacy parameter type @ref T_LE_PRIVACY_PARAM_TYPE and it will get the privacy parameter.
  * The 'p_value' field must point to an appropriate data type that meets the requirements for the corresponding parameter type.
  * (For example, if required data length for parameter type is 2 octets, p_value should be cast to a pointer of uint16_t.)
  *
  * @param[in]  param LE privacy parameter type @ref T_LE_PRIVACY_PARAM_TYPE.
  * @param[in,out]  p_value Pointer to location to get the parameter value.
  *
  * @return Operation result.
  * @retval GAP_CAUSE_SUCCESS Operation success.
  * @retval Others Operation failure.
  */
T_GAP_CAUSE le_privacy_get_param(T_LE_PRIVACY_PARAM_TYPE param, void *p_value);

/**
  * @brief  Register privacy callback to Bluetooth Host.
  * @param[in] p_privacy_cb Callback function provided by the APP to handle LE privacy messages sent from the Bluetooth Host.
  *              - NULL -> Not send GAP privacy messages to APP.
  *              - Other -> Use application-defined callback function.
  */
void        le_privacy_register_cb(P_FUN_PRIVACY_CB p_privacy_cb);

/**
  * @brief  Enable or disable LE privacy address resolution mode.
  *
  * If sending request operation is successful, the enabling or disabling result will be returned by callback
  * registered by @ref le_privacy_register_cb with msg type @ref GAP_MSG_LE_PRIVACY_RESOLUTION_STATUS_INFO.
  *
  * @param[in] enable Enable or disable address resolution.
  *                   - true: Enable address resolution.
  *                   - false: Disable address resolution.
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  */
T_GAP_CAUSE le_privacy_set_addr_resolution(bool enable);

/**
  * @brief  Read peer Resolvable Private Address.
  *
  * If sending request operation is successful, the reading result will be returned by callback
  * registered by @ref le_privacy_register_cb with msg type @ref GAP_MSG_LE_PRIVACY_READ_PEER_RESOLV_ADDR.
  *
  * @param[in] peer_identity_address_type Peer identity address type @ref T_GAP_IDENT_ADDR_TYPE.
  * @param[in] peer_identity_address Pointer to peer identity address.
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  */
T_GAP_CAUSE le_privacy_read_peer_resolv_addr(T_GAP_IDENT_ADDR_TYPE peer_identity_address_type,
                                             uint8_t *peer_identity_address);

/**
  * @brief  Read local Resolvable Private Address.
  *
  * If sending request operation is successful, the reading result will be returned by callback
  * registered by @ref le_privacy_register_cb with msg type @ref GAP_MSG_LE_PRIVACY_READ_LOCAL_RESOLV_ADDR.
  *
  * @param[in] peer_identity_address_type Peer identity address type @ref T_GAP_IDENT_ADDR_TYPE.
  * @param[in] peer_identity_address Pointer to peer identity address.
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  */
T_GAP_CAUSE le_privacy_read_local_resolv_addr(T_GAP_IDENT_ADDR_TYPE peer_identity_address_type,
                                              uint8_t *peer_identity_address);

/**
  * @brief  Set resolvable private address timeout with value set by @ref le_privacy_set_param
  *         with parameter @ref GAP_PARAM_PRIVACY_TIMEOUT.
  *
  * If sending request operation is successful, the setting result will be returned by callback
  * registered by @ref le_privacy_register_cb with msg type @ref GAP_MSG_LE_PRIVACY_SET_RESOLV_PRIV_ADDR_TIMEOUT.
  *
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  */
T_GAP_CAUSE le_privacy_set_resolv_priv_addr_timeout(void);


/**
  * @brief  Modify local resolving list.
  *
  * When add device to local resolving list, APP shall select API based on the situation:
  * - In the default situation or when @ref gap_config_le_key_storage_flag is called to set
  *   @ref LE_KEY_STORE_LOCAL_BD_BIT to 1, @ref le_privacy_modify_resolv_list with parameter
  *   @ref GAP_RESOLV_LIST_OP_ADD shall not be called, and @ref le_privacy_add_resolv_list shall be called.
  * - When @ref gap_config_le_key_storage_flag is called to set @ref LE_KEY_STORE_LOCAL_BD_BIT to 0,
  *   @ref le_privacy_modify_resolv_list with parameter @ref GAP_RESOLV_LIST_OP_ADD
  *   or @ref le_privacy_add_resolv_list can be called.
  *
  * If sending request operation is successful, the modifying result will be returned by callback
  * registered by @ref le_privacy_register_cb with msg type @ref GAP_MSG_LE_PRIVACY_MODIFY_RESOLV_LIST.
  *
  * @param[in] operation @ref T_GAP_RESOLV_LIST_OP.
  * @param[in] peer_identity_address_type Peer identity address type @ref T_GAP_IDENT_ADDR_TYPE.
  * @param[in] peer_identity_address Pointer to peer identity address.
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  */
T_GAP_CAUSE le_privacy_modify_resolv_list(T_GAP_RESOLV_LIST_OP operation,
                                          T_GAP_IDENT_ADDR_TYPE peer_identity_address_type,
                                          uint8_t *peer_identity_address);

/**
  * @brief  Add device to local resolving list.
  *
  * If sending request operation is successful, the adding result will be returned by callback
  * registered by @ref le_privacy_register_cb with msg type @ref GAP_MSG_LE_PRIVACY_MODIFY_RESOLV_LIST.
  *
  * @param[in] peer_identity_address_type Peer identity address type @ref T_GAP_IDENT_ADDR_TYPE.
  * @param[in] peer_identity_address Pointer to peer identity address.
  * @param[in] peer_irk              Pointer to peer IRK (16 octets).
                                     - NULL: No peer IRK, use all-zero IRK.
  * @param[in] use_local_irk         Indicates whether local IRK has been distributed to the peer device.
                                     - false: Local IRK has not been distributed to the peer device.
                                     - true: Local IRK has been distributed to the peer device.
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  */
T_GAP_CAUSE le_privacy_add_resolv_list(T_GAP_IDENT_ADDR_TYPE peer_identity_address_type,
                                       uint8_t *peer_identity_address, uint8_t *peer_irk, bool use_local_irk);

/**
  * @brief  Set privacy mode.
  *
  * If sending request operation is successful, the setting result will be returned by callback
  * registered by @ref le_privacy_register_cb with msg type @ref GAP_MSG_LE_PRIVACY_SET_MODE.
  *
  * @param[in] peer_identity_address_type Peer identity address type @ref T_GAP_IDENT_ADDR_TYPE.
  * @param[in] peer_identity_address Pointer to peer identity address.
  * @param[in] privacy_mode Privacy mode @ref T_GAP_PRIVACY_MODE.
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  */
T_GAP_CAUSE le_privacy_set_mode(T_GAP_IDENT_ADDR_TYPE peer_identity_address_type,
                                uint8_t *peer_identity_address,
                                T_GAP_PRIVACY_MODE privacy_mode);

/**
  * @brief  Convert remote Bluetooth device address type to identity address type.
  * @param[in] type Bluetooth device address type @ref T_GAP_REMOTE_ADDR_TYPE.
  * @return Identity address type @ref T_GAP_IDENT_ADDR_TYPE.
  * @retval GAP_IDENT_ADDR_PUBLIC  Public identity address.
  * @retval GAP_IDENT_ADDR_RAND    Random identity address.
  */
T_GAP_IDENT_ADDR_TYPE le_privacy_convert_addr_type(T_GAP_REMOTE_ADDR_TYPE type);

/**
 * @brief Check if a given Resolvable Private Address (RPA) can be resolved using a specified Identity Resolving Key (IRK).
 *
 * This function determines if a provided Resolvable Private Address (RPA) associated with Bluetooth Low Energy (BLE) privacy
 * mechanisms can be resolved using the specified Identity Resolving Key (IRK). It achieves this by utilizing encryption and
 * comparison methods as defined in the Bluetooth specification for privacy address resolution.
 *
 * @param[in] rpa Pointer to a 6-byte Resolvable Private Address that needs to be resolved. This address is typically a random
 *                address generated for privacy protection during BLE operations.
 * @param[in] irk Pointer to a 16-byte Identity Resolving Key used for resolving the Resolvable Private Address.
 *
 * @return Check result.
 * @retval true The Resolvable Private Address matches the IRK and successfully resolves. This indicates that the provided RPA
 *              is associated with the given IRK and should be accepted as a legitimate identity.
 * @retval false The Resolvable Private Address does not match the IRK and fails to resolve. This means that the RPA is either
 *               not associated with the given IRK or could not be resolved, indicating it is not recognized as a valid identity
 *               with the provided key.
 */
bool le_privacy_check_resolvable_private_address(uint8_t *rpa, uint8_t *irk);

/** @} */ /* End of group Gap_Privacy_Exported_Functions */
/** @} */ /* End of group GAP_PRIVACY_MODULE */


#ifdef __cplusplus
}
#endif

#endif /* GAP_PRIVACY_H */
