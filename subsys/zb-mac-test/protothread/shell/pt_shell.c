/**************************************************************************//**
 * @file      pt_shell.c
 * @brief     Source file for protothread shell process implementation
 * @author    felix
 * @version   V1.00
 * @date      2025-01-17
 *
 * @note
 *
 ******************************************************************************
 *
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

#include "protothread.h"
#include "dbg_printf.h"
#include "shell.h"
#include <osif.h>
static struct shell_input_param_s
{
    shell_program_t program;
    int argc;
    char **argv;
} shell_input_param;

static void *shell_sem;

PROCESS(shell_process, "shell");
PROCESS_THREAD(shell_process, ev, data)
{
    PROCESS_BEGIN();
    osif_sem_create(&shell_sem, "shell_sem", 0, 1);
    memset(&shell_input_param, 0, sizeof(shell_input_param));
    dbg_printf("%s start\r\n", PROCESS_NAME_STRING(PROCESS_CURRENT()));
    while (1)
    {
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_POLL);
        if (shell_input_param.program)
        {
            shell_input_param.program(shell_input_param.argc, shell_input_param.argv);
            shell_input_param.program = NULL;
            osif_sem_give(shell_sem);
        }
    }
    PROCESS_END();
}

int pt_shell_input(const char *name, shell_program_t program, int argc, char *argv[])
{
    if (name && program)
    {
        //dbg_printf("pt shell exec [%s]\r\n", name);
        shell_input_param.program = program;
        shell_input_param.argc = argc;
        shell_input_param.argv = argv;
        process_poll(&shell_process);
        osif_sem_take(shell_sem, 0xffffffff);
        return 0;
    }
    else
    {
        return -1;
    }
}
