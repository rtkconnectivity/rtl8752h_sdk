/**
 * @file at_zigbtest_cmd.c
 * @brief AT+ZIGBTEST command implementation for ZB MAC Test
 * @author
 * @date 2025-09-11
 *
 * This file implements the AT+ZIGBTEST command interface within the existing
 * zb-mac-test shell framework, providing chamber test commands compatibility.
 */

#include "shell.h"
#include "dbg_printf.h"
#include "strproc.h"
#include "at_zigbtest_mac_wrapper.h"
#include <stdio.h>
#include <string.h>

// AT+ZIGBTEST command constants
#define AT_CMD_PREFIX "AT+ZIGBTEST"
#define AT_CMD_MAX_LEN 128

// AT+ZIGBTEST command structure
typedef struct
{
    uint8_t a;      // A parameter (always 0)
    uint8_t p1;     // P1 parameter (command type)
    uint8_t p2;     // P2 parameter (sub-command)
    uint16_t data;  // Data parameter
} at_zigbtest_cmd_t;

// Test context
typedef struct
{
    bool mfg_mode_active;
    bool rx_mode_active;
    uint8_t current_channel;
    int8_t current_power;
    bool tx_running;
    bool rx_running;
    uint32_t rx_packet_count;
    uint32_t rx_error_count;
} at_test_context_t;

static at_test_context_t g_at_test_ctx = {0};

// External MAC test functions (from existing zb-mac-test)
extern int mac_test_set_channel(uint8_t channel);
extern int mac_test_set_tx_power(int8_t power_dbm);
extern int mac_test_start_tx_stream(void);
extern int mac_test_start_tx_tone(void);
extern int mac_test_stop_tx(void);
extern int mac_test_start_rx(void);
extern int mac_test_stop_rx(void);
extern int mac_test_reset_rx_stats(void);
extern int mac_test_get_rx_stats(uint32_t *good_count, uint32_t *error_count);

// External MP test functions for MFG mode control
extern uint8_t zb_mp_mac_init_cmd(uint8_t *param, uint8_t *send_cmd_comp_event_flag);
extern uint8_t zb_mp_mac_reset_cmd(uint8_t *param, uint8_t *send_cmd_comp_event_flag);

/**
 * @brief Parse AT+ZIGBTEST command string
 */
static bool parse_at_zigbtest_command(const char *cmd_str, at_zigbtest_cmd_t *cmd)
{
    // Skip "AT+ZIGBTEST" prefix
    const char *params = cmd_str + strlen(AT_CMD_PREFIX);

    // Skip whitespace
    while (*params == ' ' || *params == '\t') { params++; }

    // Parse A P1 P2 Data
    int parsed = sscanf(params, "%hhu %hhu %hhu %hu",
                        &cmd->a, &cmd->p1, &cmd->p2, &cmd->data);

    return (parsed == 4);
}

/**
 * @brief Handle MFG mode commands (P1=0)
 */
static bool handle_mfg_mode(uint8_t p2, uint16_t data, char *response, size_t resp_size)
{
    switch (p2)
    {
    case 0: // MFG mode control
        switch (data)
        {
        case 0: // MFG Mode End
            {
                uint8_t param[16] = {0};
                uint8_t send_cmd_comp_event_flag = 0;
                uint8_t status = zb_mp_mac_reset_cmd(param, &send_cmd_comp_event_flag);

                g_at_test_ctx.mfg_mode_active = false;
                g_at_test_ctx.rx_mode_active = false;
                if (g_at_test_ctx.tx_running)
                {
                    mac_test_stop_tx();
                    g_at_test_ctx.tx_running = false;
                }
                if (g_at_test_ctx.rx_running)
                {
                    mac_test_stop_rx();
                    g_at_test_ctx.rx_running = false;
                }
                snprintf(response, resp_size, "OK: MFG Mode End (status: %u)", status);
                return true;
            }

        case 1: // MFG Mode(Non RX) Start
            {
                uint8_t param[16] = {0};
                uint8_t send_cmd_comp_event_flag = 0;
                uint8_t status = zb_mp_mac_init_cmd(param, &send_cmd_comp_event_flag);

                if (status == 0)
                {
                    g_at_test_ctx.mfg_mode_active = true;
                    g_at_test_ctx.rx_mode_active = false;
                    snprintf(response, resp_size, "OK: MFG Mode Start (Non-RX)");
                    return true;
                }
                else
                {
                    snprintf(response, resp_size, "ERROR: Failed to start MFG mode (status: %u)", status);
                    return false;
                }
            }

        case 2: // Test Restart
            {
                uint8_t param[16] = {0};
                uint8_t send_cmd_comp_event_flag = 0;
                zb_mp_mac_reset_cmd(param, &send_cmd_comp_event_flag);

                // Reset all states
                memset(&g_at_test_ctx, 0, sizeof(g_at_test_ctx));
                g_at_test_ctx.current_channel = 11; // Default channel
                mac_test_stop_tx();
                mac_test_stop_rx();
                snprintf(response, resp_size, "OK: Test Restart");
                return true;
            }

        case 3: // Test Exit
            {
                uint8_t param[16] = {0};
                uint8_t send_cmd_comp_event_flag = 0;
                zb_mp_mac_reset_cmd(param, &send_cmd_comp_event_flag);

                g_at_test_ctx.mfg_mode_active = false;
                g_at_test_ctx.rx_mode_active = false;
                mac_test_stop_tx();
                mac_test_stop_rx();
                snprintf(response, resp_size, "OK: Test Exit");
                return true;
            }

        default:
            snprintf(response, resp_size, "ERROR: Invalid MFG mode data");
            return false;
        }
        break;

    default:
        snprintf(response, resp_size, "ERROR: Invalid MFG mode P2");
        return false;
    }
}

/**
 * @brief Handle configuration commands (P1=1)
 */
static bool handle_config(uint8_t p2, uint16_t data, char *response, size_t resp_size)
{
    switch (p2)
    {
    case 0: // Power Set
        // Validate power range (-90 to +14 dBm) according to uart_test_interface.c
        if (data > 104)   // Convert from unsigned to handle negative values
        {
            snprintf(response, resp_size, "ERROR: Power range -90 to +14 dBm");
            return false;
        }

        int8_t power_dbm;
        if (data > 14)
        {
            // Handle negative values: data > 14 means negative power
            power_dbm = (int8_t)(data - 256); // Convert from unsigned to signed
        }
        else
        {
            power_dbm = (int8_t)data;
        }

        if (power_dbm < -90 || power_dbm > 14)
        {
            snprintf(response, resp_size, "ERROR: Power range -90 to +14 dBm");
            return false;
        }

        g_at_test_ctx.current_power = power_dbm;
        if (mac_test_set_tx_power(power_dbm) == 0)
        {
            snprintf(response, resp_size, "OK: Power set to %d dBm", power_dbm);
            return true;
        }
        else
        {
            snprintf(response, resp_size, "ERROR: Failed to set power");
            return false;
        }

    case 1: // Channel Set
        if (data < 11 || data > 26)
        {
            snprintf(response, resp_size, "ERROR: Channel range 11-26");
            return false;
        }
        g_at_test_ctx.current_channel = (uint8_t)data;
        if (mac_test_set_channel(g_at_test_ctx.current_channel) == 0)
        {
            snprintf(response, resp_size, "OK: Channel set to %u", data);
            return true;
        }
        else
        {
            snprintf(response, resp_size, "ERROR: Failed to set channel");
            return false;
        }

    default:
        snprintf(response, resp_size, "ERROR: Invalid config P2");
        return false;
    }
}

/**
 * @brief Handle TX commands (P1=2)
 */
static bool handle_tx_cmd(uint8_t p2, uint16_t data, char *response, size_t resp_size)
{
    if (!g_at_test_ctx.mfg_mode_active || g_at_test_ctx.rx_mode_active)
    {
        snprintf(response, resp_size, "ERROR: Must be in MFG mode (Non-RX)");
        return false;
    }

    switch (p2)
    {
    case 1: // TX Tone Start
        if (g_at_test_ctx.tx_running)
        {
            mac_test_stop_tx();
        }
        if (mac_test_start_tx_tone() == 0)
        {
            g_at_test_ctx.tx_running = true;
            snprintf(response, resp_size, "OK: TX Tone started");
            return true;
        }
        else
        {
            snprintf(response, resp_size, "ERROR: Failed to start TX Tone");
            return false;
        }

    case 2: // TX Stream Start
        if (g_at_test_ctx.tx_running)
        {
            mac_test_stop_tx();
        }
        if (mac_test_start_tx_stream() == 0)
        {
            g_at_test_ctx.tx_running = true;
            snprintf(response, resp_size, "OK: TX Stream started");
            return true;
        }
        else
        {
            snprintf(response, resp_size, "ERROR: Failed to start TX Stream");
            return false;
        }

    case 3: // TX Stop
        if (mac_test_stop_tx() == 0)
        {
            g_at_test_ctx.tx_running = false;
            snprintf(response, resp_size, "OK: TX stopped");
            return true;
        }
        else
        {
            snprintf(response, resp_size, "ERROR: Failed to stop TX");
            return false;
        }

    default:
        snprintf(response, resp_size, "ERROR: Invalid TX P2");
        return false;
    }
}

/**
 * @brief Handle RX commands (P1=3)
 */
static bool handle_rx_cmd(uint8_t p2, uint16_t data, char *response, size_t resp_size)
{
    switch (p2)
    {
    case 1: // RX Start (also enables MFG RX mode)
        g_at_test_ctx.mfg_mode_active = true;
        g_at_test_ctx.rx_mode_active = true;
        if (mac_test_start_rx() == 0)
        {
            g_at_test_ctx.rx_running = true;
            snprintf(response, resp_size, "OK: RX started, MFG RX mode enabled");
            return true;
        }
        else
        {
            snprintf(response, resp_size, "ERROR: Failed to start RX");
            return false;
        }

    case 2: // RX Frame Receive Good (get count)
        if (mac_test_get_rx_stats(&g_at_test_ctx.rx_packet_count,
                                  &g_at_test_ctx.rx_error_count) == 0)
        {
            snprintf(response, resp_size, "OK: RX Good Count: %u",
                     g_at_test_ctx.rx_packet_count);
            return true;
        }
        else
        {
            snprintf(response, resp_size, "ERROR: Failed to get RX stats");
            return false;
        }

    case 3: // RX Frame Receive Count Reset
        if (mac_test_reset_rx_stats() == 0)
        {
            g_at_test_ctx.rx_packet_count = 0;
            g_at_test_ctx.rx_error_count = 0;
            snprintf(response, resp_size, "OK: RX count reset");
            return true;
        }
        else
        {
            snprintf(response, resp_size, "ERROR: Failed to reset RX count");
            return false;
        }

    default:
        snprintf(response, resp_size, "ERROR: Invalid RX P2");
        return false;
    }
}

/**
 * @brief Execute AT+ZIGBTEST command
 */
static bool execute_at_zigbtest(const at_zigbtest_cmd_t *cmd, char *response, size_t resp_size)
{
    // A parameter should always be 0
    if (cmd->a != 0)
    {
        snprintf(response, resp_size, "ERROR: A parameter must be 0");
        return false;
    }

    switch (cmd->p1)
    {
    case 0: // MFG mode
        return handle_mfg_mode(cmd->p2, cmd->data, response, resp_size);

    case 1: // Configuration
        return handle_config(cmd->p2, cmd->data, response, resp_size);

    case 2: // TX commands
        return handle_tx_cmd(cmd->p2, cmd->data, response, resp_size);

    case 3: // RX commands
        return handle_rx_cmd(cmd->p2, cmd->data, response, resp_size);

    default:
        snprintf(response, resp_size, "ERROR: Invalid P1 parameter");
        return false;
    }
}

/**
 * @brief AT+ZIGBTEST shell command handler
 */
s32 cmd_at_zigbtest(u32 argc, char *argv[])
{
    if (argc < 4)
    {
        dbg_printf("AT+ZIGBTEST NG\r\n");
        return 1;
    }

    at_zigbtest_cmd_t cmd;
    cmd.a = (uint8_t)_strtoul((char *)argv[0], NULL, 10);
    cmd.p1 = (uint8_t)_strtoul((char *)argv[1], NULL, 10);
    cmd.p2 = (uint8_t)_strtoul((char *)argv[2], NULL, 10);
    cmd.data = (uint16_t)_strtoul((char *)argv[3], NULL, 10);

    // Special case: AT+ZIGBTEST 0 3 2 0 (RX Frame Receive Good query) - return packet count
    if (cmd.a == 0 && cmd.p1 == 3 && cmd.p2 == 2 && cmd.data == 0)
    {
        if (mac_test_get_rx_stats(&g_at_test_ctx.rx_packet_count,
                                  &g_at_test_ctx.rx_error_count) == 0)
        {
            dbg_printf("AT+ZIGBTEST %u\r\n", g_at_test_ctx.rx_packet_count);
        }
        return 0;
    }

    char response[128];
    bool result = execute_at_zigbtest(&cmd, response, sizeof(response));

    // Standard OK/NG response for all other commands
    dbg_printf("AT+ZIGBTEST %s\r\n", result ? "OK" : "NG");

    return result ? 0 : 1;
}

/**
 * @brief AT+ZIGBTEST status command
 */
s32 cmd_at_status(u32 argc, char *argv[])
{
    // Simple status response showing RX packet count
    dbg_printf("AT+ZIGBTEST %u\r\n", g_at_test_ctx.rx_packet_count);
    return 0;
}

/**
 * @brief Initialize AT+ZIGBTEST command context
 */
void at_zigbtest_init(void)
{
    memset(&g_at_test_ctx, 0, sizeof(g_at_test_ctx));
    g_at_test_ctx.current_channel = 11; // Default channel
    g_at_test_ctx.current_power = 0;    // Default power
}

// Command table entries (to be added to the shell command table)
const shell_command_entry_t at_zigbtest_commands[] =
{
    {"AT+ZIGBTEST", (shell_program_t)cmd_at_zigbtest, "AT+ZIGBTEST A P1 P2 Data - Chamber test command", 0},
    {"atstat", (shell_program_t)cmd_at_status, "Show AT+ZIGBTEST status", 0}
};
