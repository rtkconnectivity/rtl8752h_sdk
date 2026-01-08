/**
 * @file at_zigbtest_cmd.h
 * @brief AT+ZIGBTEST command header for ZB MAC Test
 * @author
 * @date 2025-09-11
 */

#ifndef AT_ZIGBTEST_CMD_H
#define AT_ZIGBTEST_CMD_H

#include <stdint.h>
#include <stdbool.h>
#include "shell.h"

// Function declarations
s32 cmd_at_zigbtest(u32 argc, char *argv[]);
s32 cmd_at_status(u32 argc, char *argv[]);
void at_zigbtest_init(void);

// Command table entry (for shell integration)
extern const shell_command_entry_t at_zigbtest_commands[];

#endif // AT_ZIGBTEST_CMD_H
