/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-12-13     BalanceTWK   first version
 * 2019-06-11     WillianChan   Add SD card hot plug detection
 */

#ifndef _DRV_SDIO_H
#define _DRV_SDIO_H

#ifdef OS_RTTHREAD
#include <rtthread.h>
#include "rtdevice.h"
#include <rthw.h>
#include <drv_common.h>
#include <string.h>
#include <drivers/mmcsd_core.h>
#include <drivers/sdio.h>
#include "rtl876x.h"



#define SDIO_BUFF_SIZE       4096
#define SDIO_ALIGN_LEN       32

#ifndef SDIO_MAX_FREQ
#define SDIO_MAX_FREQ        (1000000)
#endif

#ifndef SDIO_CLOCK_FREQ
#define SDIO_CLOCK_FREQ      (40U * 1000 * 1000)
#endif



#define HW_SDIO_IT_CD                    (0x01U << 0)
#define HW_SDIO_IT_RE                    (0x01U << 1)
#define HW_SDIO_IT_CDDONE                    (0x01U << 2)
#define HW_SDIO_IT_DTO                    (0x01U << 3)
#define HW_SDIO_IT_TXDR                    (0x01U << 4)
#define HW_SDIO_IT_RXDR                     (0x01U << 5)
#define HW_SDIO_IT_RCRC                     (0x01U << 6)
#define HW_SDIO_IT_DCRC                     (0x01U << 7)
#define HW_SDIO_IT_RTO                     (0x01U << 8)
#define HW_SDIO_IT_DRTO                    (0x01U << 9)
#define HW_SDIO_IT_HTO                     (0x01U << 10)
#define HW_SDIO_IT_FRUN                      (0x01U << 11)
#define HW_SDIO_IT_HLE                       (0x01U << 12)
#define HW_SDIO_IT_BCI                       (0x01U << 13)
#define HW_SDIO_IT_ACD                    (0x01U << 14)
#define HW_SDIO_IT_EBE                    (0x01U << 15)


//#define HW_SDIO_ERRORS \
//    (HW_SDIO_IT_CCRCFAIL | HW_SDIO_IT_CTIMEOUT | \
//     HW_SDIO_IT_DCRCFAIL | HW_SDIO_IT_DTIMEOUT | \
//     HW_SDIO_IT_RXOVERR  | HW_SDIO_IT_TXUNDERR)

//#define HW_SDIO_POWER_OFF                      (0x00U)
//#define HW_SDIO_POWER_UP                       (0x02U)
//#define HW_SDIO_POWER_ON                       (0x03U)

//#define HW_SDIO_FLOW_ENABLE                    (0x01U << 14)
//#define HW_SDIO_BUSWIDE_1B                     (0x00U << 11)
//#define HW_SDIO_BUSWIDE_4B                     (0x01U << 11)
//#define HW_SDIO_BUSWIDE_8B                     (0x02U << 11)
//#define HW_SDIO_BYPASS_ENABLE                  (0x01U << 10)
//#define HW_SDIO_IDLE_ENABLE                    (0x01U << 9)
//#define HW_SDIO_CLK_ENABLE                     (0x01U << 8)

//#define HW_SDIO_SUSPEND_CMD                    (0x01U << 11)
//#define HW_SDIO_CPSM_ENABLE                    (0x01U << 10)
//#define HW_SDIO_WAIT_END                       (0x01U << 9)
//#define HW_SDIO_WAIT_INT                       (0x01U << 8)
#define HW_SDIO_RESPONSE_NO                    (0x00U << 6)
#define HW_SDIO_RESPONSE_SHORT                 (0x01U << 6)
#define HW_SDIO_RESPONSE_LONG                  (0x03U << 6)

//#define HW_SDIO_DATA_LEN_MASK                  (0x01FFFFFFU)

//#define HW_SDIO_IO_ENABLE                      (0x01U << 11)
//#define HW_SDIO_RWMOD_CK                       (0x01U << 10)
//#define HW_SDIO_RWSTOP_ENABLE                  (0x01U << 9)
//#define HW_SDIO_RWSTART_ENABLE                 (0x01U << 8)
//#define HW_SDIO_DBLOCKSIZE_1                   (0x00U << 4)
//#define HW_SDIO_DBLOCKSIZE_2                   (0x01U << 4)
//#define HW_SDIO_DBLOCKSIZE_4                   (0x02U << 4)
//#define HW_SDIO_DBLOCKSIZE_8                   (0x03U << 4)
//#define HW_SDIO_DBLOCKSIZE_16                  (0x04U << 4)
//#define HW_SDIO_DBLOCKSIZE_32                  (0x05U << 4)
//#define HW_SDIO_DBLOCKSIZE_64                  (0x06U << 4)
//#define HW_SDIO_DBLOCKSIZE_128                 (0x07U << 4)
//#define HW_SDIO_DBLOCKSIZE_256                 (0x08U << 4)
//#define HW_SDIO_DBLOCKSIZE_512                 (0x09U << 4)
//#define HW_SDIO_DBLOCKSIZE_1024                (0x0AU << 4)
//#define HW_SDIO_DBLOCKSIZE_2048                (0x0BU << 4)
//#define HW_SDIO_DBLOCKSIZE_4096                (0x0CU << 4)
//#define HW_SDIO_DBLOCKSIZE_8192                (0x0DU << 4)
//#define HW_SDIO_DBLOCKSIZE_16384               (0x0EU << 4)
//#define HW_SDIO_DMA_ENABLE                     (0x01U << 3)
//#define HW_SDIO_STREAM_ENABLE                  (0x01U << 2)
//#define HW_SDIO_TO_HOST                        (0x01U << 1)
//#define HW_SDIO_DPSM_ENABLE                    (0x01U << 0)

#define HW_SDIO_DATATIMEOUT                    (0xF0000000U)


struct rtk_sdio
{
    __IO uint32_t CTRL;                   /*!< 0x00 */
    __IO uint32_t PWREN;                  /*!< 0x04 */
    __IO uint32_t CLKDIV;                 /*!< 0x08 */
    __IO uint32_t CLKSRC;                 /*!< 0x0C */
    __IO uint32_t CLKENA;                 /*!< 0x10 */
    __IO uint32_t TMOUT;                  /*!< 0x14 */
    __IO uint32_t CTYPE;                  /*!< 0x18 */
    __IO uint32_t BLKSIZ;                 /*!< 0x1C */
    __IO uint32_t BYTCNT;                 /*!< 0x20 */
    __IO uint32_t INTMASK;                /*!< 0x24 */
    __IO uint32_t CMDARG;                 /*!< 0x28 */
    __IO uint32_t CMD;                    /*!< 0x2C */
    __I  uint32_t RESP0;                  /*!< 0x30 */
    __I  uint32_t RESP1;                  /*!< 0x34 */
    __I  uint32_t RESP2;                  /*!< 0x38 */
    __I  uint32_t RESP3;                  /*!< 0x3C */
    __I  uint32_t MINTSTS;                /*!< 0x40 */
    __IO uint32_t RINTSTS;                /*!< 0x44 */
    __I  uint32_t STATUS;                 /*!< 0x48 */
    __IO uint32_t FIFOTH;                 /*!< 0x4C */
    __I  uint32_t CDETECT;                /*!< 0x50 */
    __I  uint32_t WRTPRT;                 /*!< 0x54 */
    __IO uint32_t GPIO;                   /*!< 0x58 */
    __I  uint32_t TCBCNT;                 /*!< 0x5C */
    __I  uint32_t TBBCNT;                 /*!< 0x60 */
    __IO uint32_t DEBNCE;                 /*!< 0x64 */
    __IO uint32_t USRID;                  /*!< 0x68 */
    __I  uint32_t VERID;                  /*!< 0x6C */
    __I  uint32_t HCON;                   /*!< 0x70 */
    __IO uint32_t UHS_REG;                /*!< 0x74 */
    __IO uint32_t RST_n;                  /*!< 0x78 */
    __I  uint32_t Reserved0;              /*!< 0x7C */
    __IO uint32_t BMOD;                   /*!< 0x80 */
    __O  uint32_t PLDMND;                 /*!< 0x84 */
    __IO uint32_t DBADDR;                 /*!< 0x88 */
    __IO uint32_t IDSTS;                  /*!< 0x8C */
    __IO uint32_t IDINTEN;                /*!< 0x90 */
    __I  uint32_t DSCADDR;                /*!< 0x94 */
    __I  uint32_t BUFADDR;                /*!< 0x98 */
    __I  uint8_t Reserved1[0x100 - 0x9C];            /*!< 0x9C */
    __IO uint32_t CardThrCtl;             /*!< 0x100 */
    __IO uint32_t Back_end_power;         /*!< 0x104 */
    __IO uint32_t UHS_REG_EXT;            /*!< 0x108 */
    __IO uint32_t EMMC_DDR_REG;           /*!< 0x10C */
    __IO uint32_t ENABLE_SHIFT;           /*!< 0x110 */
    __IO uint8_t Reserved2[0x100 - 0x14];        /*!< 0x114 */
    __IO uint32_t DATA;                   /*!< >=0x200 */
};

typedef rt_err_t (*dma_txconfig)(rt_uint32_t *src, rt_uint32_t *dst, int size);
typedef rt_err_t (*dma_rxconfig)(rt_uint32_t *src, rt_uint32_t *dst, int size);
typedef rt_uint32_t (*sdio_clk_get)(struct rtk_sdio *hw_sdio);

struct rtk_sdio_des
{
    struct rtk_sdio *hw_sdio;
    dma_txconfig txconfig;
    dma_rxconfig rxconfig;
    sdio_clk_get clk_get;
};

extern void stm32_mmcsd_change(void);
#else
#include <stdint.h>
#include <stdbool.h>
#include "rtl_sdio_reg.h"



#define BYTES_PER_BLOCK  512


typedef enum
{
    SDEMMCRES_OK = 0,

    SDEMMCRES_ILLEGAL_PARM,

    SDEMMCRES_CMD0_ERROR,
    SDEMMCRES_CMD8_ERROR, // 3
    SDEMMCRES_CMD55_ERROR,
    SDEMMCRES_ACMD41_ERROR,
    SDEMMCRES_ACMD41_TIMEOUT, // 6
    SDEMMCRES_CMD2_ERROR,
    SDEMMCRES_CMD3_ERROR,
    SDEMMCRES_CMD7_ERROR, // 9
    SDEMMCRES_CMD9_ERROR,
    SDEMMCRES_CMD13_ERROR,
    SDEMMCRES_ACMD6_ERROR, // 12
    SDEMMCRES_CMD16_ERROR,
    SDEMMCRES_ACMD42_ERROR,
    SDEMMCRES_CMD18_ERROR, // 15
    SDEMMCRES_CMD13_TIMEOUT,
    SDEMMCRES_ACMD51_ERROR,
    SDEMMCRES_CMD6_ERROR,  // 18
    SDEMMCRES_CMD25_ERROR,
    SDEMMCRES_CMD1_ERROR,
    SDEMMCRES_CMD1_TIMEOUT, // 21
    SDEMMCRES_CMD6_TIMEOUT,

    SDEMMCRES_WRITE_TIMEOUT,
    SDEMMCRES_MALLOC_FAILED, // 24
} SdEmmcRes_t;

typedef enum
{
    CARDTYPE_SD = 1,
    CARDTYPE_EMMC,
} CardType_t;

typedef enum
{
    DATAWIDTH_1BIT,
    DATAWIDTH_4BIT,
    DATAWIDTH_8BIT,
    DATAWIDTH_4BIT_DDR,
    DATAWIDTH_8BIT_DDR,
} DataWidth_t;

typedef struct
{
    // Pin group.
    // Power enable pin and active level.

    // Enter DLPS callback;
    // Power on/off callback;
    // Auto power off;
    // ...

    CardType_t CardType;
    DataWidth_t DataWidth;
    uint32_t ClkOutFreq_kHz;
} SdEmmcInitParm_t;


//** All the following APIs must run in task environment.
SdEmmcRes_t SdEmmc_Init(SDIO_TypeDef *Sdio, const SdEmmcInitParm_t *pParm);
// A block is always set to 512 bytes.
SdEmmcRes_t SdEmmc_Read(SDIO_TypeDef *Sdio, uint32_t StartBlock, uint32_t BlockCnt, void *pBuf);
SdEmmcRes_t SdEmmc_Write(SDIO_TypeDef *Sdio, uint32_t StartBlock, uint32_t BlockCnt,
                         const void *pBuf);
uint32_t SdEmmc_GetBlockCnt(SDIO_TypeDef *Sdio);

#include <string.h>
#include "utils.h"
#include "trace.h"


#ifndef NDEBUG
#define ASSERT(e) \
    do { \
        if(!(e)) { \
            platform_delay_ms(1000); \
            DBG_DIRECT("(" #e ") assert failed! Func: %s. Line: %d.", __func__, __LINE__); \
            DBG_DIRECT("(" #e ") assert failed! Func: %s. Line: %d.", __func__, __LINE__); \
            *(volatile int *)0x1 = 0; \
        } \
    } while(0)
#else
#define ASSERT(e) ((void)0)
#endif // NDEBUG

#define STATIC_ASSERT(e)  typedef char StaticAssertOn##__LINE__[(e) ? 1 : -1]

#define IS_ADDR_ALIGNED(Addr)  ((uint32_t)(Addr) % 4 == 0)

#define MIN2(a, b)  ((a) < (b) ? (a) : (b))


static inline uint32_t BitsToU32(const void *pBits, uint32_t FirstBit, uint32_t LastBit)
{
    uint32_t FirstByte = FirstBit / 8;
    uint32_t FirstOfs = FirstBit % 8;
    uint32_t LastByte = LastBit / 8;

    uint32_t ByteCnt = LastByte + 1 - FirstByte;
    uint32_t BitsCnt = LastBit + 1 - FirstBit;
    ASSERT(BitsCnt <= 32);

    uint64_t u64;
    const uint8_t *p = pBits;
    memcpy(&u64, p + FirstByte, ByteCnt);

    return (uint32_t)(u64 >> FirstOfs) & (~0UL >> (32 - BitsCnt));
}


typedef enum
{
    SDIORES_OK = 0,

    SDIORES_HARDWARE_LOCKED_ERROR, // 1
    SDIORES_RESPONSE_ERROR,
    SDIORES_RESPONSE_CRC_ERROR,
    SDIORES_RESPONSE_TIMEOUT,

    SDIORES_DATA_CRC_ERROR, // 5
    SDIORES_DATA_READ_TIMEOUT,
    SDIORES_DATA_START_BIT_ERROR,
    SDIORES_DATA_END_BIT_ERROR,
    SDIORES_WRITE_NO_CRC, // 9

    SDIORES_FATAL_BUS_ERROR,
    SDIORES_DESCRIPTOR_UNAVAILABLE,
    SDIORES_CARD_ERROR, // 12

    SDIORES_WAIT_DATA0_IDLE_TIMEOUT,
} SdioRes_t;


typedef struct
{
    // 0 ~ 63
    uint8_t CmdIdx;
    uint32_t CmdArg;

    // Such as CMD0.
    bool IsResetCmd;
    // Such as CMD12.
    bool IsStopCmd;

    // CMD0, CMD4, CMD15... have no response.
    bool IsRspExpected;
    // R2 is long response.
    bool IsR2Rsp;
    // Some CMD do not have CRC, such as ACMD41 for eMMC and CMD8 for SD.
    bool CheckRspCrc;
} CmdInfo_t;


typedef struct
{
    // Bytes per block.
    uint32_t BlockSize;
    // Block counts you want to Tx/Rx.
    uint32_t BlockCount;
    // If set, stop cmd (CMD12) will be sent automatically after data transfer.
    bool SendAutoStop;
} DataInfo_t;




// All the APIs must run in task environment.

// pParm is shallow copied, and must keep valid during the whole SDIO session.
void Sdio_InitPad(SDIO_TypeDef *Sdio, DataWidth_t DataWidth);
void Sdio_DeInitPad(SDIO_TypeDef *Sdio, DataWidth_t DataWidth);

void Sdio_Init(SDIO_TypeDef *Sdio);
void Sdio_DeInit(SDIO_TypeDef *Sdio);

uint32_t Sdio_SetClkOutFreq(SDIO_TypeDef *Sdio, uint32_t Freq_kHz);
uint32_t Sdio_GetClkOutFreq_kHz(SDIO_TypeDef *Sdio);

void Sdio_SetHostDataWidth(SDIO_TypeDef *Sdio, DataWidth_t Width);

SdioRes_t Sdio_SendNoDataCmd(SDIO_TypeDef *Sdio, const CmdInfo_t *pCmdInfo, void *pRspBuf);

// At most (DESC_CNT * MAX_BLOCK_PER_DESC * BYTES_PER_BLOCK) bytes can be send per transfer.
SdioRes_t Sdio_SendCmdWithRxData(SDIO_TypeDef *Sdio,
                                 const CmdInfo_t *pCmdInfo, void *pRspBuf,
                                 const DataInfo_t *pDataInfo, void *pRxDataBuf);
SdioRes_t Sdio_SendCmdWithTxData(SDIO_TypeDef *Sdio,
                                 const CmdInfo_t *pCmdInfo, void *pRspBuf,
                                 const DataInfo_t *pDataInfo, const void *pDataToTx);

SdioRes_t Sdio_WaitData0Idle(SDIO_TypeDef *Sdio, uint32_t Timeout_ms);

#define DESC_CNT  4
// TODO: It seems that the maximum can only be set to 16. why?
#define MAX_BLOCK_PER_DESC  16

#define MAX_BYTES_PER_DESC  (MAX_BLOCK_PER_DESC * BYTES_PER_BLOCK)
#define MAX_BLOCK_PER_XFER  (MAX_BLOCK_PER_DESC * DESC_CNT)
#define MAX_BYTES_PER_XFER  (MAX_BYTES_PER_DESC * DESC_CNT)



//// All the following APIs must run in task environment.
SdEmmcRes_t Sd_Init(SDIO_TypeDef *Sdio, const SdEmmcInitParm_t *pParm);
// A block is always set to 512 bytes.
SdEmmcRes_t Sd_Read(SDIO_TypeDef *Sdio, uint32_t StartBlock, uint32_t BlockCnt, void *pBuf);
SdEmmcRes_t Sd_Write(SDIO_TypeDef *Sdio, uint32_t StartBlock, uint32_t BlockCnt, const void *pBuf);
uint32_t Sd_GetBlockCnt(SDIO_TypeDef *Sdio);





#endif
#endif