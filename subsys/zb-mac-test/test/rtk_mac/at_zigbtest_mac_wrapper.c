/**
 * @file at_zigbtest_mac_wrapper.c
 * @brief MAC test wrapper functions for AT+ZIGBTEST commands
 * @author
 * @date 2025-09-11
 *
 * This file provides wrapper functions that bridge AT+ZIGBTEST commands
 * to the existing MAC test infrastructure in zb-mac-test.
 */

#include "at_zigbtest_cmd.h"
#include "at_zigbtest_mac_wrapper.h"
#include "mac_test_common.h"
#include "dbg_printf.h"
#include "zb_mp_test.h"

// External MP test command functions (from zb_mp_test.c)
extern uint8_t zb_mp_mac_init_cmd(uint8_t *param, uint8_t *send_cmd_comp_event_flag);
extern uint8_t zb_mp_cont_tx_cmd(uint8_t *param, uint8_t *send_cmd_comp_event_flag);
extern uint8_t zb_mp_rx_cmd(uint8_t *param, uint8_t *send_cmd_comp_event_flag);
extern uint8_t zb_mp_tx_cmd(uint8_t *param, uint8_t *send_cmd_comp_event_flag);
extern zb_mp_test_adapter_t *pzb_mp_adapt;

// External functions for custom MP test initialization
extern void zb_mp_mac_phy_disable(void);
extern void zb_mp_test_timer_stop(void);
extern void zb_mp_mac_int_handler(uint32_t int_sts);
extern int32_t zb_mp_edscan_lv2dbm(int32_t level);
extern void (*modem_set_zb_cca_combination)(uint8_t comb);

// External MAC functions for lock management
extern void mpan_mac_lock(uint8_t pan_idx);
extern void mpan_mac_unlock(void);

// Simple global settings
static uint8_t g_channel = 11;          // Default channel
static uint8_t g_tx_gain = 30;          // Default tx_gain index (mid-range)
bool g_mp_test_initialized = false;  // Non-static for external access
static bool g_mp_test_init_attempted = false;  // Track if we've tried to init MP test

// Continuous TX control (keep minimal state only if needed later)
static bool g_cont_tx_active = false;

// Self-allocated MP test adapter for AT+ZIGBTEST mode
zb_mp_test_adapter_t g_mp_test_adapter;  // Non-static for external access

/**
 * @brief Initialize MP test on-demand (only when first needed)
 */
static bool ensure_mp_test_initialized(void)
{
    // If already successfully initialized, return true
    if (g_mp_test_initialized)
    {
        return true;
    }

    // If we already tried and failed, don't try again
    if (g_mp_test_init_attempted)
    {
        return false;
    }

    g_mp_test_init_attempted = true;

    // Self-allocate and initialize MP test adapter
    if (pzb_mp_adapt == NULL)
    {
        pzb_mp_adapt = &g_mp_test_adapter;
        // Initialize the MP test system with our adapter
        zb_mp_test_init(pzb_mp_adapt);
    }

    // Verify adapter is ready
    if (pzb_mp_adapt->mp_test_state != 1)    // ZB_MP_STATE_READY
    {
        // Try to set it to ready state
        pzb_mp_adapt->mp_test_state = 1;
    }

    g_mp_test_initialized = true;
    return true;
}

/**
 * @brief Set MAC channel (simplified)
 */
int mac_test_set_channel(uint8_t channel)
{
    // Simple validation
    if (channel < 11 || channel > 26)
    {
        return -1;
    }

    g_channel = channel;
    return 0;
}

/**
 * @brief Set TX power (simplified - direct tx_gain mapping)
 */
int mac_test_set_tx_power(int8_t power_dbm)
{
    // Simple tx_gain mapping without complex conversion
    // Direct mapping based on common power levels
    if (power_dbm >= 10)
    {
        g_tx_gain = 60;        // High power
    }
    else if (power_dbm >= 5)
    {
        g_tx_gain = 50;        // Medium-high power
    }
    else if (power_dbm >= 0)
    {
        g_tx_gain = 40;        // Medium power
    }
    else if (power_dbm >= -5)
    {
        g_tx_gain = 30;        // Medium-low power
    }
    else if (power_dbm >= -10)
    {
        g_tx_gain = 20;        // Low power
    }
    else
    {
        g_tx_gain = 10;        // Very low power
    }

    return 0;
}

/**
 * @brief Start TX stream
 */
int mac_test_start_tx_stream(void)
{
    if (!ensure_mp_test_initialized())
    {
        return -1;
    }

    // Delegate to existing MP TX start with infinite pkt_cnt (0)
    uint8_t param[16] = {0};
    uint8_t send_cmd_comp_event_flag = 0;

    // Parameters: start, channel, tx_gain, tx_len, pkt_cnt=0(infinite)
    param[0] = 1;           // start
    param[1] = g_channel;   // channel
    param[2] = g_tx_gain;   // tx_gain index
    param[3] = 50;          // tx_len fixed
    param[4] = 0;           // pkt_cnt infinite

    uint8_t status = zb_mp_tx_cmd(param, &send_cmd_comp_event_flag);
    return (status == 0) ? 0 : -1;
}

/**
 * @brief Start TX tone (simplified)
 */
int mac_test_start_tx_tone(void)
{
    if (!ensure_mp_test_initialized())
    {
        return -1;
    }

    uint8_t param[16] = {0};
    uint8_t send_cmd_comp_event_flag = 0;

    // Simple continuous TX parameters
    param[0] = 1;           // tx_start: 1=Start TX
    param[1] = g_channel;   // channel
    param[2] = g_tx_gain;   // tx_gain index
    param[3] = 7;           // payload_pattern: 7=PRBS9
    param[4] = 1;           // rf_mode: 1=IQM_MODE

    uint8_t status = zb_mp_cont_tx_cmd(param, &send_cmd_comp_event_flag);

    return (status == 0) ? 0 : -1;
}

/**
 * @brief Stop TX (simplified)
 */
int mac_test_stop_tx(void)
{
    if (!ensure_mp_test_initialized())
    {
        return -1;
    }

    // Stop any MP test TX modes
    uint8_t param[16] = {0};
    uint8_t send_cmd_comp_event_flag = 0;

    // Stop continuous TX
    param[0] = 0;           // tx_start: 0=Stop TX
    param[1] = g_channel;   // channel
    param[2] = g_tx_gain;   // tx_gain
    param[3] = 0;           // payload_pattern
    param[4] = 0;           // rf_mode
    zb_mp_cont_tx_cmd(param, &send_cmd_comp_event_flag);

    // Stop packet TX
    param[0] = 0;           // tx_start: 0=Stop TX
    param[1] = g_channel;   // channel
    param[2] = g_tx_gain;   // tx_gain
    param[3] = 50;          // tx_len
    param[4] = 0;           // pkt_cnt
    zb_mp_tx_cmd(param, &send_cmd_comp_event_flag);

    return 0;
}

/**
 * @brief Start RX (simplified)
 */
int mac_test_start_rx(void)
{
    if (!ensure_mp_test_initialized())
    {
        return -1;
    }

    uint8_t param[16] = {0};
    uint8_t send_cmd_comp_event_flag = 0;

    // Simple RX parameters
    param[0] = 1;           // rx_start: 1=Start RX
    param[1] = g_channel;   // channel

    uint8_t status = zb_mp_rx_cmd(param, &send_cmd_comp_event_flag);

    return (status == 0) ? 0 : -1;
}

/**
 * @brief Stop RX (simplified)
 */
int mac_test_stop_rx(void)
{
    if (!ensure_mp_test_initialized())
    {
        return -1;
    }

    uint8_t param[16] = {0};
    uint8_t send_cmd_comp_event_flag = 0;

    // Simple RX stop
    param[0] = 0;           // rx_start: 0=Stop RX
    param[1] = g_channel;   // channel

    uint8_t status = zb_mp_rx_cmd(param, &send_cmd_comp_event_flag);

    return (status == 0) ? 0 : -1;
}

/**
 * @brief Reset RX statistics (simplified)
 */
int mac_test_reset_rx_stats(void)
{

    if (!ensure_mp_test_initialized())
    {
        return -1;
    }

    // Reset all RX counters
    if (pzb_mp_adapt)
    {
        pzb_mp_adapt->mp_rx_state.rx_ok_cnt = 0;
        pzb_mp_adapt->mp_rx_state.rx_err_cnt = 0;
        pzb_mp_adapt->mp_rx_state.rx_running_state = 0;  // Reset to stopped state
    }

    return 0;
}

/**
 * @brief Get RX statistics (simplified)
 */
int mac_test_get_rx_stats(uint32_t *good_count, uint32_t *error_count)
{
    if (!good_count || !error_count)
    {
        return -1;
    }

    if (!ensure_mp_test_initialized())
    {
        return -1;
    }

    // Get stats from MP test adapter if available
    if (pzb_mp_adapt)
    {
        *good_count = pzb_mp_adapt->mp_rx_state.rx_ok_cnt;
        *error_count = pzb_mp_adapt->mp_rx_state.rx_err_cnt;
    }
    else
    {
        // Fallback values
        *good_count = 0;
        *error_count = 0;
    }

    return 0;
}

/**
 * @brief Initialize MAC test wrapper (simplified)
 */
void mac_test_wrapper_init(void)
{
    // Initialize defaults
    g_channel = 11;
    g_tx_gain = 30;
    g_cont_tx_active = false;

    // Reset initialization flags
    g_mp_test_initialized = false;
    g_mp_test_init_attempted = false;

    // Enable AT+ZIGBTEST command structure
    at_zigbtest_init();  // Initialize command context (safe)
}
