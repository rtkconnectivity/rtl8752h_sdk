#include "trace.h"
#include "gap_bond_le.h"
#include "user_cmd.h"
#include "gap.h"

T_USER_CMD_IF user_cmd_if;

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
/** @} */ /* End of group CENTRAL_CMD */


