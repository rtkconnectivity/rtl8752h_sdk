/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2012-11-23     Yihui        The first version
 * 2013-11-24     aozima       fixed _sys_read()/_sys_write() issues.
 * 2014-08-03     bernard      If using msh, use system() implementation
 *                             in msh.
 * 2020-08-05     Meco Man     fixed _sys_flen() compiling-warning when
 *                             RT_USING_DFS is not defined
 */

#include <string.h>
#include <stdlib.h>


/* Standard IO device handles. */
#define STDIN       0
#define STDOUT      1
#define STDERR      2

typedef int FILEHANDLE;

/*
 * These names are used during library initialization as the
 * file names opened for stdin, stdout, and stderr.
 * As we define _sys_open() to always return the same file handle,
 * these can be left as their default values.
 */
const char __stdin_name[] =  ":tt";
const char __stdout_name[] =  ":tt";
const char __stderr_name[] =  ":tt";

FILEHANDLE _sys_open(const char *name, int openmode)
{
    return 1; /* everything goes to the same output */
}
int _sys_close(FILEHANDLE fh)
{
    return 0;
}
int _sys_write(FILEHANDLE fh, const unsigned char *buf,
               unsigned len, int mode)
{
//  your_device_write(buf, len);
    return 0;
}
int _sys_read(FILEHANDLE fh, unsigned char *buf,
              unsigned len, int mode)
{
    return -1; /* not supported */
}
void _ttywrch(int ch)
{
    return;
}
int _sys_istty(FILEHANDLE fh)
{
    return 0; /* buffered output */
}
int _sys_seek(FILEHANDLE fh, long pos)
{
    return -1; /* not supported */
}
long _sys_flen(FILEHANDLE fh)
{
    return -1; /* not supported */
}

__attribute__((weak)) void _sys_exit(int return_code)
{
    /* TODO: perhaps exit the thread which is invoking this function */
    while (1);
}

/**
 * used by tmpnam() or tmpfile()
 */
int _sys_tmpnam(char *name, int fileno, unsigned maxlength)
{
    return -1;
}

char *_sys_command_string(char *cmd, int len)
{
    /* no support */
    return NULL;
}





