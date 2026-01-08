/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

#ifndef _DATATYPE_H_
#define _DATATYPE_H_

#include <stddef.h>
#include "mem_config.h"

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif

typedef unsigned char           BOOLEAN;
typedef void                    VOID;
typedef unsigned char           UCHAR;
typedef unsigned char           UINT8;
typedef unsigned short          UINT16;
typedef unsigned int            UINT32;
typedef unsigned long long      UINT64;
typedef signed char             CHAR;
typedef signed char             INT8;
typedef signed short            INT16;
typedef signed int              INT32;
typedef signed long long        INT64;

/** @brief The upper 8 bits of a 16 bit value */
#ifndef MSB
#define MSB(a) (((a) & 0xFF00) >> 8)
#endif

/** @brief The lower 8 bits of a 16 bit value */
#ifndef LSB
#define LSB(a) ((a) & 0xFF)
#endif

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef CEIL
#define CEIL(a,b)            (((a)+(b)-1)/(b))
#endif

#define KSEG1                      0
#define NON_DCATCH(ptr)            ((UINT32)((UINT32)(ptr) | KSEG1 | BUFFER_RAM_START_ADDR))
#define NON_DCATCH_PTR(ptr)        ((UINT8*)NON_DCATCH(ptr))

#define RD_32BIT_IO(base, offset) \
    (*(volatile UINT32*)((base) + (offset)))
#define RD_16BIT_IO(base, offset) \
    (*((volatile UINT16*)((base) + (offset))))
#define RD_8BIT_IO(base, offset) \
    (*((volatile UINT8*)((base) + (offset))))
#define WR_32BIT_IO(base, offset, val) \
    (*((volatile UINT32*)((base) + (offset))) = (val))
#define WR_16BIT_IO(base, offset, val) \
    (*((volatile UINT16*)((base) + (offset))) = (val))
#define WR_8BIT_IO(base, offset, val) \
    (*((volatile UINT8*)((base) + (offset))) = (val))

#define RD_REG_MMIO(type, addr) \
    (*((volatile type*) (addr)))
#define WR_REG_MMIO(type, addr, val) \
    (*((volatile type*) (addr)) = (val))


typedef union UINT32_S_
{
    UINT32 u4Byte;
    UINT16 u2Byte[2];
    UINT8 u1Byte[4];
} UINT32_S;

typedef union INT32_S_
{
    INT32 s4Byte;
    INT16 s2Byte[2];
    INT8 s1Byte[4];
} INT32_S;

typedef struct UINT32_BM_S_
{
    UINT32 bit0: 1;
    UINT32 bit1: 1;
    UINT32 bit2: 1;
    UINT32 bit3: 1;
    UINT32 bit4: 1;
    UINT32 bit5: 1;
    UINT32 bit6: 1;
    UINT32 bit7: 1;
    UINT32 bit8: 1;
    UINT32 bit9: 1;
    UINT32 bit10: 1;
    UINT32 bit11: 1;
    UINT32 bit12: 1;
    UINT32 bit13: 1;
    UINT32 bit14: 1;
    UINT32 bit15: 1;
    UINT32 bit16: 1;
    UINT32 bit17: 1;
    UINT32 bit18: 1;
    UINT32 bit19: 1;
    UINT32 bit20: 1;
    UINT32 bit21: 1;
    UINT32 bit22: 1;
    UINT32 bit23: 1;
    UINT32 bit24: 1;
    UINT32 bit25: 1;
    UINT32 bit26: 1;
    UINT32 bit27: 1;
    UINT32 bit28: 1;
    UINT32 bit29: 1;
    UINT32 bit30: 1;
    UINT32 bit31: 1;
} UINT32_BM_S;

typedef struct UINT16_BM_S_
{
    UINT16 bit0: 1;
    UINT16 bit1: 1;
    UINT16 bit2: 1;
    UINT16 bit3: 1;
    UINT16 bit4: 1;
    UINT16 bit5: 1;
    UINT16 bit6: 1;
    UINT16 bit7: 1;
    UINT16 bit8: 1;
    UINT16 bit9: 1;
    UINT16 bit10: 1;
    UINT16 bit11: 1;
    UINT16 bit12: 1;
    UINT16 bit13: 1;
    UINT16 bit14: 1;
    UINT16 bit15: 1;
} UINT16_BM_S;

typedef struct UINT8_BM_S_
{
    UINT8 bit0: 1;
    UINT8 bit1: 1;
    UINT8 bit2: 1;
    UINT8 bit3: 1;
    UINT8 bit4: 1;
    UINT8 bit5: 1;
    UINT8 bit6: 1;
    UINT8 bit7: 1;
} UINT8_BM_S;

#define PLATFORM_WAKEUP_TIME_INFINITE   0xffffffff

typedef enum
{
    ROUND                   = 0,
    ROUNDUP                 = 1,
    ROUNDDOWN               = 2,
} RoundType;

uint32_t us_to_rtc_clk(uint32_t us, RoundType round_type);

uint32_t rtc_clk_to_us(uint32_t rtc_clk, RoundType round_type);

uint64_t clk_cnt_diff(uint64_t clk_begin, uint64_t clk_end, uint64_t bit_mask);

#endif // _DATATYPE_H_

