/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-06-22     tyx          first
 * 2018-12-12     balanceTWK   first version
 * 2019-06-11     WillianChan  Add SD card hot plug detection
 * 2020-11-09     whj4674672   fix sdio non-aligned access problem
 */

#include "board.h"
#include "drv_sdio.h"
//#include "drv_config.h"
#ifdef OS_RTTHREAD
#ifdef BSP_USING_SDIO


#define DBG_ENABLE
#define DBG_TAG     "drv.sdio"
#define DBG_LVL     DBG_LOG
#include <rtdbg.h>


static struct rt_mmcsd_host *host;

#define SDIO_TX_RX_COMPLETE_TIMEOUT_LOOPS    (100000)

#define RTHW_SDIO_LOCK(_sdio)   rt_mutex_take(&_sdio->mutex, RT_WAITING_FOREVER)
#define RTHW_SDIO_UNLOCK(_sdio) rt_mutex_release(&_sdio->mutex);

struct sdio_pkg
{
    struct rt_mmcsd_cmd *cmd;
    void *buff;
    rt_uint32_t flag;
};

struct rthw_sdio
{
    struct rt_mmcsd_host *host;
    struct rtk_sdio_des sdio_des;
    struct rt_event event;
    struct rt_mutex mutex;
    struct sdio_pkg *pkg;
};


__attribute__((aligned(SDIO_ALIGN_LEN))) static rt_uint8_t cache_buf[SDIO_BUFF_SIZE];


static rt_uint32_t rtk_sdio_clk_get(struct rtk_sdio *hw_sdio)
{
    return SDIO_CLOCK_FREQ;
}
/**
  * @brief  This function get order from sdio.
  * @param  data
  * @retval sdio  order
  */
static int get_order(rt_uint32_t data)
{
    int order = 0;

    switch (data)
    {
    case 1:
        order = 0;
        break;
    case 2:
        order = 1;
        break;
    case 4:
        order = 2;
        break;
    case 8:
        order = 3;
        break;
    case 16:
        order = 4;
        break;
    case 32:
        order = 5;
        break;
    case 64:
        order = 6;
        break;
    case 128:
        order = 7;
        break;
    case 256:
        order = 8;
        break;
    case 512:
        order = 9;
        break;
    case 1024:
        order = 10;
        break;
    case 2048:
        order = 11;
        break;
    case 4096:
        order = 12;
        break;
    case 8192:
        order = 13;
        break;
    case 16384:
        order = 14;
        break;
    default :
        order = 0;
        break;
    }

    return order;
}

/**
  * @brief  This function wait sdio completed.
  * @param  sdio  rthw_sdio
  * @retval None
  */
static void rthw_sdio_wait_completed(struct rthw_sdio *sdio)
{
    rt_uint32_t status;
    struct rt_mmcsd_cmd *cmd = sdio->pkg->cmd;
    struct rt_mmcsd_data *data = cmd->data;
    struct rtk_sdio *hw_sdio = sdio->sdio_des.hw_sdio;

    if (rt_event_recv(&sdio->event, 0xffffffff, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                      rt_tick_from_millisecond(5000), &status) != RT_EOK)
    {
        LOG_E("wait completed timeout");
        cmd->err = -RT_ETIMEOUT;
        return;
    }

    if (sdio->pkg == RT_NULL)
    {
        return;
    }

    cmd->resp[0] = hw_sdio->RESP0;
    cmd->resp[1] = hw_sdio->RESP1;
    cmd->resp[2] = hw_sdio->RESP2;
    cmd->resp[3] = hw_sdio->RESP3;

    if (status)
    {
        LOG_E(" ERROR ");
    }
    else
    {
        cmd->err = RT_EOK;
        LOG_D("sta:0x%08X [%08X %08X %08X %08X]", status, cmd->resp[0], cmd->resp[1], cmd->resp[2],
              cmd->resp[3]);
    }
}

/**
  * @brief  This function transfer data by dma.
  * @param  sdio  rthw_sdio
  * @param  pkg   sdio package
  * @retval None
  */
static void rthw_sdio_transfer_by_dma(struct rthw_sdio *sdio, struct sdio_pkg *pkg)
{
    struct rt_mmcsd_data *data;
    int size;
    void *buff;
    struct rtk_sdio *hw_sdio;

    if ((RT_NULL == pkg) || (RT_NULL == sdio))
    {
        LOG_E("rthw_sdio_transfer_by_dma invalid args");
        return;
    }

    data = pkg->cmd->data;
    if (RT_NULL == data)
    {
        LOG_E("rthw_sdio_transfer_by_dma invalid args");
        return;
    }

    buff = pkg->buff;
    if (RT_NULL == buff)
    {
        LOG_E("rthw_sdio_transfer_by_dma invalid args");
        return;
    }
    hw_sdio = sdio->sdio_des.hw_sdio;
    size = data->blks * data->blksize;

    if (data->flags & DATA_DIR_WRITE)
    {
        sdio->sdio_des.txconfig((rt_uint32_t *)buff, (rt_uint32_t *)&hw_sdio->DATA, size);
        //hw_sdio->dctrl |= HW_SDIO_DMA_ENABLE;
    }
    else if (data->flags & DATA_DIR_READ)
    {
        sdio->sdio_des.rxconfig((rt_uint32_t *)&hw_sdio->DATA, (rt_uint32_t *)buff, size);
        //hw_sdio->dctrl |= HW_SDIO_DMA_ENABLE | HW_SDIO_DPSM_ENABLE;
    }
}

/**
  * @brief  This function send command.
  * @param  sdio  rthw_sdio
  * @param  pkg   sdio package
  * @retval None
  */
static void rthw_sdio_send_command(struct rthw_sdio *sdio, struct sdio_pkg *pkg)
{
    struct rt_mmcsd_cmd *cmd = pkg->cmd;
    struct rt_mmcsd_data *data = cmd->data;
    struct rtk_sdio *hw_sdio = sdio->sdio_des.hw_sdio;
    rt_uint32_t reg_cmd = 0;

    /* save pkg */
    sdio->pkg = pkg;

    LOG_D("CMD:%d ARG:0x%08x RES:%s%s%s%s%s%s%s%s%s rw:%c len:%d blksize:%d",
          cmd->cmd_code,
          cmd->arg,
          resp_type(cmd) == RESP_NONE ? "NONE"  : "",
          resp_type(cmd) == RESP_R1  ? "R1"  : "",
          resp_type(cmd) == RESP_R1B ? "R1B"  : "",
          resp_type(cmd) == RESP_R2  ? "R2"  : "",
          resp_type(cmd) == RESP_R3  ? "R3"  : "",
          resp_type(cmd) == RESP_R4  ? "R4"  : "",
          resp_type(cmd) == RESP_R5  ? "R5"  : "",
          resp_type(cmd) == RESP_R6  ? "R6"  : "",
          resp_type(cmd) == RESP_R7  ? "R7"  : "",
          data ? (data->flags & DATA_DIR_WRITE ?  'w' : 'r') : '-',
          data ? data->blks * data->blksize : 0,
          data ? data->blksize : 0
         );

    /* config cmd reg */


    //reg_cmd = cmd->cmd_code | HW_SDIO_CPSM_ENABLE;
    if (resp_type(cmd) == RESP_NONE)
    {
        reg_cmd |= HW_SDIO_RESPONSE_NO;
    }
    else if (resp_type(cmd) == RESP_R2)
    {
        reg_cmd |= HW_SDIO_RESPONSE_LONG;
    }
    else
    {
        reg_cmd |= HW_SDIO_RESPONSE_SHORT;
    }

    /* config data reg */
    if (data != RT_NULL)
    {
        rt_uint32_t dir = 0;
        rt_uint32_t size = data->blks * data->blksize;
        int order;

        //hw_sdio->dctrl = 0;
        hw_sdio->TMOUT = HW_SDIO_DATATIMEOUT;
        hw_sdio->BYTCNT = size;
        order = get_order(data->blksize);
        hw_sdio->BLKSIZ = order;
        //dir = (data->flags & DATA_DIR_READ) ? HW_SDIO_TO_HOST : 0;
        //hw_sdio->dctrl = HW_SDIO_IO_ENABLE | dir;
    }

    /* transfer config */
    if (data != RT_NULL)
    {
        rthw_sdio_transfer_by_dma(sdio, pkg);
    }

    /* open irq */
//    hw_sdio->INTMASK |= HW_SDIO_IT_CMDSENT | HW_SDIO_IT_CMDREND | HW_SDIO_ERRORS;
//    if (data != RT_NULL)
//    {
//        hw_sdio->INTMASK |= HW_SDIO_IT_DATAEND;
//    }

    /* send cmd */
    hw_sdio->CMDARG = cmd->arg;
    hw_sdio->CMD = reg_cmd | BIT31;


    /* wait completed */
    rthw_sdio_wait_completed(sdio);

    /* Waiting for data to be sent to completion */
    if (data != RT_NULL)
    {
        volatile rt_uint32_t count = SDIO_TX_RX_COMPLETE_TIMEOUT_LOOPS;

        while (count && (hw_sdio->STATUS & (BIT10)))
        {
            count--;
        }

        if ((count == 0) || (hw_sdio->STATUS & BIT10))
        {
            cmd->err = -RT_ERROR;
        }
    }

    /* close irq, keep sdio irq */
    //hw_sdio->INTMASK = hw_sdio->INTMASK & HW_SDIO_IT_SDIOIT ? HW_SDIO_IT_SDIOIT : 0x00;

    /* clear pkg */
    sdio->pkg = RT_NULL;
}

/**
  * @brief  This function send sdio request.
  * @param  host  rt_mmcsd_host
  * @param  req   request
  * @retval None
  */
static void rthw_sdio_request(struct rt_mmcsd_host *host, struct rt_mmcsd_req *req)
{
    struct sdio_pkg pkg;
    struct rthw_sdio *sdio = host->private_data;
    struct rt_mmcsd_data *data;

    RTHW_SDIO_LOCK(sdio);

    if (req->cmd != RT_NULL)
    {
        rt_memset(&pkg, 0, sizeof(pkg));
        data = req->cmd->data;
        pkg.cmd = req->cmd;

        if (data != RT_NULL)
        {
            rt_uint32_t size = data->blks * data->blksize;

            RT_ASSERT(size <= SDIO_BUFF_SIZE);

            pkg.buff = data->buf;
            if ((rt_uint32_t)data->buf & (SDIO_ALIGN_LEN - 1))
            {
                pkg.buff = cache_buf;
                if (data->flags & DATA_DIR_WRITE)
                {
                    rt_memcpy(cache_buf, data->buf, size);
                }
            }
        }

        rthw_sdio_send_command(sdio, &pkg);

        if ((data != RT_NULL) && (data->flags & DATA_DIR_READ) &&
            ((rt_uint32_t)data->buf & (SDIO_ALIGN_LEN - 1)))
        {
            rt_memcpy(data->buf, cache_buf, data->blksize * data->blks);
        }
    }

    if (req->stop != RT_NULL)
    {
        rt_memset(&pkg, 0, sizeof(pkg));
        pkg.cmd = req->stop;
        rthw_sdio_send_command(sdio, &pkg);
    }

    RTHW_SDIO_UNLOCK(sdio);

    mmcsd_req_complete(sdio->host);
}

/**
  * @brief  This function config sdio.
  * @param  host    rt_mmcsd_host
  * @param  io_cfg  rt_mmcsd_io_cfg
  * @retval None
  */
static void rthw_sdio_iocfg(struct rt_mmcsd_host *host, struct rt_mmcsd_io_cfg *io_cfg)
{
    rt_uint32_t clkcr, div;
    rt_uint32_t clk_src;
    rt_uint32_t clk = io_cfg->clock;
    struct rthw_sdio *sdio = host->private_data;
    struct rtk_sdio *hw_sdio = sdio->sdio_des.hw_sdio;

    clk_src = 4000000;
    if (clk_src < 400 * 1000)
    {
        LOG_E("The clock rate is too low! rata:%d", clk_src);
        return;
    }

    if (clk > host->freq_max) { clk = host->freq_max; }

    if (clk > clk_src)
    {
        LOG_W("Setting rate is greater than clock source rate.");
        clk = clk_src;
    }

    LOG_D("clk:%d width:%s%s%s power:%s%s%s",
          clk,
          io_cfg->bus_width == MMCSD_BUS_WIDTH_8 ? "8" : "",
          io_cfg->bus_width == MMCSD_BUS_WIDTH_4 ? "4" : "",
          io_cfg->bus_width == MMCSD_BUS_WIDTH_1 ? "1" : "",
          io_cfg->power_mode == MMCSD_POWER_OFF ? "OFF" : "",
          io_cfg->power_mode == MMCSD_POWER_UP ? "UP" : "",
          io_cfg->power_mode == MMCSD_POWER_ON ? "ON" : ""
         );

    RTHW_SDIO_LOCK(sdio);

    div = clk_src / clk;
    if ((clk == 0) || (div == 0))
    {
        clkcr = 0;
    }
    else
    {
        if (div < 2)
        {
            div = 2;
        }
        else if (div > 0xFF)
        {
            div = 0xFF;
        }
        div -= 2;
        //clkcr = div | HW_SDIO_CLK_ENABLE;
    }

    if (io_cfg->bus_width == MMCSD_BUS_WIDTH_8)
    {
        //clkcr |= HW_SDIO_BUSWIDE_8B;
    }
    else if (io_cfg->bus_width == MMCSD_BUS_WIDTH_4)
    {
        //clkcr |= HW_SDIO_BUSWIDE_4B;
    }
    else
    {
        //clkcr |= HW_SDIO_BUSWIDE_1B;
    }

    if (io_cfg->bus_mode == MMCSD_BUSMODE_OPENDRAIN)
    {
        hw_sdio->CTRL |= BIT24;
    }
    else if (io_cfg->bus_mode == MMCSD_BUSMODE_PUSHPULL)
    {
        hw_sdio->CTRL &= ~BIT24;
    }

    ///hw_sdio->clkcr = clkcr;

    switch (io_cfg->power_mode)
    {
    case MMCSD_POWER_OFF:
        hw_sdio->PWREN = 0;
        break;
    case MMCSD_POWER_UP:
        hw_sdio->PWREN = 1;
        break;
    case MMCSD_POWER_ON:
        hw_sdio->PWREN = 1;
        break;
    default:
        LOG_W("unknown power_mode %d", io_cfg->power_mode);
        break;
    }

    RTHW_SDIO_UNLOCK(sdio);
}

/**
  * @brief  This function update sdio interrupt.
  * @param  host    rt_mmcsd_host
  * @param  enable
  * @retval None
  */
void rthw_sdio_irq_update(struct rt_mmcsd_host *host, rt_int32_t enable)
{
    struct rthw_sdio *sdio = host->private_data;
    struct rtk_sdio *hw_sdio = sdio->sdio_des.hw_sdio;

    if (enable)
    {
        LOG_D("enable sdio irq");
        //hw_sdio->mask |= HW_SDIO_IT_SDIOIT;
    }
    else
    {
        LOG_D("disable sdio irq");
        //hw_sdio->mask &= ~HW_SDIO_IT_SDIOIT;
    }
}

/**
  * @brief  This function detect sdcard.
  * @param  host    rt_mmcsd_host
  * @retval 0x01
  */
static rt_int32_t rthw_sd_detect(struct rt_mmcsd_host *host)
{
    LOG_D("try to detect device");
    return 0x01;
}

/**
  * @brief  This function interrupt process function.
  * @param  host  rt_mmcsd_host
  * @retval None
  */
void rthw_sdio_irq_process(struct rt_mmcsd_host *host)
{
//    int complete = 0;
//    struct rthw_sdio *sdio = host->private_data;
//    struct rtk_sdio *hw_sdio = sdio->sdio_des.hw_sdio;
//    rt_uint32_t intstatus = hw_sdio->STATUS;

//    if (intstatus & HW_SDIO_ERRORS)
//    {
//        hw_sdio->icr = HW_SDIO_ERRORS;
//        complete = 1;
//    }
//    else
//    {
//        if (intstatus & HW_SDIO_IT_CMDREND)
//        {
//            hw_sdio->icr = HW_SDIO_IT_CMDREND;

//            if (sdio->pkg != RT_NULL)
//            {
//                if (!sdio->pkg->cmd->data)
//                {
//                    complete = 1;
//                }
//                else if ((sdio->pkg->cmd->data->flags & DATA_DIR_WRITE))
//                {
//                    hw_sdio->dctrl |= HW_SDIO_DPSM_ENABLE;
//                }
//            }
//        }

//        if (intstatus & HW_SDIO_IT_CMDSENT)
//        {
//            hw_sdio->icr = HW_SDIO_IT_CMDSENT;

//            if (resp_type(sdio->pkg->cmd) == RESP_NONE)
//            {
//                complete = 1;
//            }
//        }

//        if (intstatus & HW_SDIO_IT_DATAEND)
//        {
//            hw_sdio->icr = HW_SDIO_IT_DATAEND;
//            complete = 1;
//        }
//    }

//    if ((intstatus & HW_SDIO_IT_SDIOIT) && (hw_sdio->mask & HW_SDIO_IT_SDIOIT))
//    {
//        hw_sdio->icr = HW_SDIO_IT_SDIOIT;
//        sdio_irq_wakeup(host);
//    }

//    if (complete)
//    {
//        hw_sdio->mask &= ~HW_SDIO_ERRORS;
//        rt_event_send(&sdio->event, intstatus);
//    }
}

static const struct rt_mmcsd_host_ops ops =
{
    rthw_sdio_request,
    rthw_sdio_iocfg,
    rthw_sd_detect,
    rthw_sdio_irq_update,
};


/**
  * @brief  This function get stm32 sdio clock.
  * @param  hw_sdio: rtk_sdio
  * @retval PCLK2Freq
  */
static rt_uint32_t rtk_sdio_clock_get(struct rtk_sdio *hw_sdio)
{
    //return HAL_RCC_GetPCLK2Freq();
    return 0;
}

static rt_err_t DMA_TxConfig(rt_uint32_t *src, rt_uint32_t *dst, int Size)
{
    //SD_LowLevel_DMA_TxConfig((uint32_t *)src, (uint32_t *)dst, Size / 4);
    return RT_EOK;
}

static rt_err_t DMA_RxConfig(rt_uint32_t *src, rt_uint32_t *dst, int Size)
{
    //SD_LowLevel_DMA_RxConfig((uint32_t *)src, (uint32_t *)dst, Size / 4);
    return RT_EOK;
}

/**
  * @brief  This function create mmcsd host.
  * @param  sdio_des  rtk_sdio_des
  * @retval rt_mmcsd_host
  */
struct rt_mmcsd_host *sdio_host_create(struct rtk_sdio_des *sdio_des)
{
    struct rt_mmcsd_host *host;
    struct rthw_sdio *sdio = RT_NULL;

    if ((sdio_des == RT_NULL) || (sdio_des->txconfig == RT_NULL) || (sdio_des->rxconfig == RT_NULL))
    {
        LOG_E("L:%d F:%s %s %s %s",
              (sdio_des == RT_NULL ? "sdio_des is NULL" : ""),
              (sdio_des ? (sdio_des->txconfig ? "txconfig is NULL" : "") : ""),
              (sdio_des ? (sdio_des->rxconfig ? "rxconfig is NULL" : "") : "")
             );
        return RT_NULL;
    }

    sdio = rt_malloc(sizeof(struct rthw_sdio));
    if (sdio == RT_NULL)
    {
        LOG_E("L:%d F:%s malloc rthw_sdio fail");
        return RT_NULL;
    }
    rt_memset(sdio, 0, sizeof(struct rthw_sdio));

    host = mmcsd_alloc_host();
    if (host == RT_NULL)
    {
        LOG_E("L:%d F:%s mmcsd alloc host fail");
        rt_free(sdio);
        return RT_NULL;
    }

    rt_memcpy(&sdio->sdio_des, sdio_des, sizeof(struct rtk_sdio_des));
    sdio->sdio_des.hw_sdio = (sdio_des->hw_sdio == RT_NULL ? (struct rtk_sdio *)
                              SDIO_HOST1_CFG_REG_BASE : sdio_des->hw_sdio);
    sdio->sdio_des.clk_get = (sdio_des->clk_get == RT_NULL ? rtk_sdio_clk_get : sdio_des->clk_get);

    rt_event_init(&sdio->event, "sdio", RT_IPC_FLAG_FIFO);
    rt_mutex_init(&sdio->mutex, "sdio", RT_IPC_FLAG_PRIO);

    /* set host defautl attributes */
    host->ops = &ops;
    host->freq_min = 400 * 1000;
    host->freq_max = SDIO_MAX_FREQ;
    host->valid_ocr = 0X00FFFF80;/* The voltage range supported is 1.65v-3.6v */
#ifndef SDIO_USING_1_BIT
    host->flags = MMCSD_BUSWIDTH_4 | MMCSD_MUTBLKWRITE | MMCSD_SUP_SDIO_IRQ;
#else
    host->flags = MMCSD_MUTBLKWRITE | MMCSD_SUP_SDIO_IRQ;
#endif
    host->max_seg_size = SDIO_BUFF_SIZE;
    host->max_dma_segs = 1;
    host->max_blk_size = 512;
    host->max_blk_count = 512;

    /* link up host and sdio */
    sdio->host = host;
    host->private_data = sdio;

    rthw_sdio_irq_update(host, 1);

    /* ready to change */
    mmcsd_change(host);

    return host;
}



void SDIO_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();
    /* Process All SDIO Interrupt Sources */
    rthw_sdio_irq_process(host);

    /* leave interrupt */
    rt_interrupt_leave();
}

#if 0
#include "rtl_pinmux.h"
#include "rtl876x_sdio.h"
#define CLK_40M_FREQ_kHz        (20*1000)//40M
#define ROUND_UP_DIVIDE(a, b)   (((a)+(b)-1)/(b))

static void Sdio_InitClk(SDIO_TypeDef *Sdio)
{

    Sdh0Phy0_t sdh0phy0 = {.d32 = SDH0_PHY0_REG};
    sdh0phy0.b.r_sdh0_clk_sel_drv = 1;
    sdh0phy0.b.r_sdh0_clk_sel_sample = 1;
    SDH0_PHY0_REG = sdh0phy0.d32;

    SdhCtl_t sdhctl = {.d32 = SDH_CTL_REG};
    sdhctl.b.sdio0_ck_en = 1;
    sdhctl.b.sdio0_func_en = 1;
    sdhctl.b.r_sdio0_clk_src_en = 1;
    sdhctl.b.r_sdio0_div_sel = 0;
    sdhctl.b.r_sdio0_div_en = 0;
    sdhctl.b.r_sdio0_mux_clk_cg_en = 1;
    sdhctl.b.r_sdio0_clk_src_sel = 0;
    SDH_CTL_REG = sdhctl.d32;

    rt_kprintf("InitClk, 0x400E_21E8: 0x%x, 0x400E_21EC: 0x%x", SDH_CTL_REG, SDH0_PHY0_REG);
}

static void Sdio_ResetAll(SDIO_TypeDef *Sdio)
{
    CTRL_t ctrl = {.d32 = Sdio->CTRL};
    ctrl.b.controller_reset = 1;
    ctrl.b.fifo_reset = 1;
    ctrl.b.dma_reset = 1;
    Sdio->CTRL = ctrl.d32;
    do
    {
        ctrl.d32 = Sdio->CTRL;
    }
    while (ctrl.b.controller_reset == 1 || ctrl.b.fifo_reset == 1 || ctrl.b.dma_reset == 1);
}

void Sdio_SetClk(SDIO_TypeDef *Sdio, uint32_t Freq_kHz)
{
    uint32_t Divider = ROUND_UP_DIVIDE(CLK_40M_FREQ_kHz, Freq_kHz);
    Divider = ROUND_UP_DIVIDE(Divider, 2);

    Sdio->CLKDIV = Divider;

    Sdio->CLKSRC = 0;

    static const CLKENA_t clkena = {.b = {.cclk_enable = 1, .cclk_low_power = 1}};
    Sdio->CLKENA = clkena.d32;

    CMD_t cmd = {.d32 = Sdio->CMD};
    cmd.b.start_cmd = 1;
    cmd.b.update_clock_registers_only = 1;
    cmd.b.wait_prvdata_complete = 1;
    Sdio->CMD = cmd.d32;
    do
    {
        cmd.d32 = Sdio->CMD;
        // TODO: Support HLE handling. P219
    }
    while (cmd.b.start_cmd == 1);
}

int rt_hw_sdio_init(void)
{
    /**********for test**************/
#define DRV_SDIO_CMD  P3_6
#define DRV_SDIO_CLK  P3_7
#define DRV_SDIO_D0  P4_0
#define DRV_SDIO_D1  P4_1
#define DRV_SDIO_D2  P4_2
#define DRV_SDIO_D3  P4_3

    Pad_Config(DRV_SDIO_CMD, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
    Pad_Config(DRV_SDIO_CLK, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
    Pad_Config(DRV_SDIO_D0, PAD_PINMUX_MODE, PAD_IS_PWRON,  PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
    Pad_Config(DRV_SDIO_D1, PAD_PINMUX_MODE, PAD_IS_PWRON,  PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
    Pad_Config(DRV_SDIO_D2, PAD_PINMUX_MODE, PAD_IS_PWRON,  PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
    Pad_Config(DRV_SDIO_D3, PAD_PINMUX_MODE, PAD_IS_PWRON,  PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);

    Pad_Dedicated_Config(DRV_SDIO_CMD, ENABLE);
    Pad_Dedicated_Config(DRV_SDIO_CLK, ENABLE);
    Pad_Dedicated_Config(DRV_SDIO_D0, ENABLE);
    Pad_Dedicated_Config(DRV_SDIO_D1, ENABLE);
    Pad_Dedicated_Config(DRV_SDIO_D2, ENABLE);
    Pad_Dedicated_Config(DRV_SDIO_D3, ENABLE);

    struct rtk_sdio_des sdio_des;
    sdio_des.clk_get = rtk_sdio_clock_get;
    sdio_des.hw_sdio = (struct rtk_sdio *)SDIO_HOST0_CFG_REG_BASE;
    sdio_des.rxconfig = DMA_RxConfig;
    sdio_des.txconfig = DMA_TxConfig;

    struct rtk_sdio *hw_sdio = sdio_des.hw_sdio;

    Sdio_InitClk(NULL);

    Sdio_ResetAll((SDIO_TypeDef *)hw_sdio);

    hw_sdio->RINTSTS = 0xffffffff;
    hw_sdio->INTMASK = 0;

    Sdio_SetClk((SDIO_TypeDef *)hw_sdio, 400);

    /* cmd0 with send_initialization */
    hw_sdio->CMDARG = 0;
    hw_sdio->CMD = (BIT13 | BIT15 | BIT29 | BIT31);

    /**********for test**************/


    host = sdio_host_create(&sdio_des);
    if (host == RT_NULL)
    {
        LOG_E("host create fail");
        return -1;
    }

    return 0;
}
INIT_DEVICE_EXPORT(rt_hw_sdio_init);
#endif



#endif
#else
#include "drv_sdio.h"
#include <string.h>
#include "app_section.h"
#include "rtl_nvic.h"
//#include "cachel1_armv7.h"
#include "rtl_pinmux.h"

#include "os_mem.h"
#include "os_sync.h"
#include "os_sched.h"
#include "trace.h"



//// Abbrevations:
// xfer = transfer          rsp = response          freq = frequency
// intr = interrupt         sem = semaphore         inst = instance
// res = result


//// Prefix:
// g = global variable      k = global const
// p = pointer              pfn = function pointer  a = array


//// Configurable macro
#define SDIO_CLK_SRC  CLKSRC_CLK_40M_VCORE4 // CLKSRC_CLK_40M_VCORE4, CLKSRC_CKO1_PLL4_VCORE4

#define SDIO_INTR_PRIORITY  5


//// Non-configurable macro
#define CLKIN_FREQ_kHz  ((SDIO_CLK_SRC == CLKSRC_CLK_40M_VCORE4) ? (20*1000) : (50*1000))

#define DEFAULT_CLK_OUT_FREQ_kHz  400

#define CLK_OUT_MAX_FREQ_kHz  CLKIN_FREQ_kHz
#define CLK_OUT_MIN_FREQ_kHz  (CLKIN_FREQ_kHz / (255*2))



#define SDIO0_CLK  P3_7
#define SDIO0_CMD  P3_6
#define SDIO0_D0  P4_0
#define SDIO0_D1  P4_1
#define SDIO0_D2  P4_2
#define SDIO0_D3  P4_3
#define SDIO0_D4  P4_4
#define SDIO0_D5  P4_5
#define SDIO0_D6  P4_6
#define SDIO0_D7  P4_7

#define SDIO1_CLK  P5_7
#define SDIO1_CMD  P5_6
#define SDIO1_D0  P19_0
#define SDIO1_D1  P19_1
#define SDIO1_D2  P19_2
#define SDIO1_D3  P19_3
#define SDIO1_D4  P19_4
#define SDIO1_D5  P19_5
#define SDIO1_D6  P19_6
#define SDIO1_D7  P19_7

static const uint8_t aSdio0Pin[10] = {SDIO0_CLK, SDIO0_CMD, SDIO0_D0, SDIO0_D1, SDIO0_D2, SDIO0_D3, SDIO0_D4, SDIO0_D5, SDIO0_D6, SDIO0_D7};
static const uint8_t aSdio1Pin[10] = {SDIO1_CLK, SDIO1_CMD, SDIO1_D0, SDIO1_D1, SDIO1_D2, SDIO1_D3, SDIO1_D4, SDIO1_D5, SDIO1_D6, SDIO1_D7};



static void *gSdio0Sem = NULL;
static void *gSdio1Sem = NULL;

// Only use interrupt-driven DMA mode. Polling mode and reading/writing data by CPU is not used.

static SdioRes_t CheckResponseByIntr(SDIO_TypeDef *Sdio);

static SdioRes_t SendCmdWithRxDataByDma(SDIO_TypeDef *Sdio,
                                        const CmdInfo_t *pCmdInfo, void *pRspBuf,
                                        const DataInfo_t *pDataInfo, void *pRxDataBuf);
static SdioRes_t SendCmdWithTxDataByDma(SDIO_TypeDef *Sdio,
                                        const CmdInfo_t *pCmdInfo, void *pRspBuf,
                                        const DataInfo_t *pDataInfo, const void *pDataToTx);

static void InitDmaDesc(SDIO_TypeDef *Sdio, uint32_t DataAddr, uint32_t DataBytes);
static SdioRes_t CheckRxDataXferByIntr(SDIO_TypeDef *Sdio);
static SdioRes_t CheckTxDataXferByIntr(SDIO_TypeDef *Sdio);
static SdioRes_t CheckDmaStateByIntr(SDIO_TypeDef *Sdio);


static void ResetAll(SDIO_TypeDef *Sdio);
static void EnableIntrByNvic(SDIO_TypeDef *Sdio);
static void DisableIntrByNvic(SDIO_TypeDef *Sdio);

static void SetFieldByCmdInfo(CMD_t *pCmd, const CmdInfo_t *pCmdInfo);
static void GetResponse(SDIO_TypeDef *Sdio, bool IsR2Rsp, void *pBuf);

static void InitClk(SDIO_TypeDef *Sdio);
static void DeInitClk(SDIO_TypeDef *Sdio);

static uint8_t GetPinCnt(DataWidth_t DataWidth);



void Sdio_InitPad(SDIO_TypeDef *Sdio, DataWidth_t DataWidth)
{
    uint8_t PinCnt = GetPinCnt(DataWidth);
    const uint8_t *pSdioPin = (Sdio == SDIO0) ? aSdio0Pin : aSdio1Pin;

    for (uint8_t i = 0; i < PinCnt; ++i)
    {
        Pad_Config(pSdioPin[i], PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
        Pad_Dedicated_Config(pSdioPin[i], ENABLE);
//        Pad_SetDrivingCurrent(pSdioPin[i], PAD_DRIVING_CURRENT_8_16mA);
    }

}

void Sdio_DeInitPad(SDIO_TypeDef *Sdio, DataWidth_t DataWidth)
{
    uint8_t PinCnt = GetPinCnt(DataWidth);
    const uint8_t *pSdioPin = (Sdio == SDIO0) ? aSdio0Pin : aSdio1Pin;

    for (uint8_t i = 0; i < PinCnt; ++i)
    {
        Pad_Dedicated_Config(pSdioPin[i], DISABLE);
        Pad_Config(pSdioPin[i], PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    }
}

void Sdio_Init(SDIO_TypeDef *Sdio)
{
    InitClk(Sdio);

    ResetAll(Sdio);

    Sdio->RINTSTS = 0xffffffff;
    Sdio->INTMASK = 0;

    CTRL_t ctrl = {.d32 = Sdio->CTRL};
    ctrl.b.int_enable = 1;
    ctrl.b.use_internal_dmac = 1;
    Sdio->CTRL = ctrl.d32;

    Sdio_SetClkOutFreq(Sdio, DEFAULT_CLK_OUT_FREQ_kHz);

    if (Sdio == SDIO0)
    {
        ASSERT(gSdio0Sem == NULL);
        os_sem_create(&gSdio0Sem, "gSdio0Sem", 0, 1);
        ASSERT(gSdio0Sem != NULL);
    }
    else
    {
        ASSERT(gSdio1Sem == NULL);
        os_sem_create(&gSdio1Sem, "gSdio1Sem", 0, 1);
        ASSERT(gSdio1Sem != NULL);
    }

    EnableIntrByNvic(Sdio);
}


void Sdio_DeInit(SDIO_TypeDef *Sdio)
{
    DisableIntrByNvic(Sdio);

    if (Sdio == SDIO0)
    {
        ASSERT(gSdio0Sem != NULL);
        os_sem_delete(gSdio0Sem);
        gSdio0Sem = NULL;
    }
    else
    {
        ASSERT(gSdio1Sem != NULL);
        os_sem_delete(gSdio1Sem);
        gSdio1Sem = NULL;
    }

    DeInitClk(Sdio);
}

#define ROUND_UP_DIV(a, b)  (((a)+(b)-1)/(b))

uint32_t Sdio_SetClkOutFreq(SDIO_TypeDef *Sdio, uint32_t Freq_kHz)
{
    if (Freq_kHz >= CLK_OUT_MAX_FREQ_kHz)
    {
        Sdio->CLKDIV = 0;
    }
    else if (Freq_kHz <= CLK_OUT_MIN_FREQ_kHz)
    {
        Sdio->CLKDIV = 0xff;
    }
    else
    {
        uint32_t Divider = ROUND_UP_DIV(CLK_OUT_MAX_FREQ_kHz, Freq_kHz);
        Divider = ROUND_UP_DIV(Divider, 2);

        ASSERT(0 < Divider && Divider <= 0xff);
        Sdio->CLKDIV = Divider;
    }

    Sdio->CLKSRC = 0;

    static const CLKENA_t clkena = {.b = {.cclk_enable = 1, .cclk_low_power = 1}};
    Sdio->CLKENA = clkena.d32;

    CMD_t cmd = {.d32 = Sdio->CMD};
    cmd.b.start_cmd = 1;
    cmd.b.update_clock_registers_only = 1;
    cmd.b.wait_prvdata_complete = 1;
    Sdio->CMD = cmd.d32;
    do
    {
        cmd.d32 = Sdio->CMD;
        // TODO: Support HLE handling. P219
    }
    while (cmd.b.start_cmd == 1);

    return Sdio_GetClkOutFreq_kHz(Sdio);
}


uint32_t Sdio_GetClkOutFreq_kHz(SDIO_TypeDef *Sdio)
{
    CLKDIV_t clkdiv = {.d32 = Sdio->CLKDIV};
    uint32_t Div = clkdiv.b.clk_divider0 * 2;
    return (Div == 0) ? CLKIN_FREQ_kHz : (CLKIN_FREQ_kHz / Div);
}

void Sdio_SetHostDataWidth(SDIO_TypeDef *Sdio, DataWidth_t Width)
{
    switch (Width)
    {
    case DATAWIDTH_1BIT:
        Sdio->CTYPE = 0;
        break;
    case DATAWIDTH_4BIT:
    case DATAWIDTH_4BIT_DDR:
        Sdio->CTYPE = 1;
        break;
    case DATAWIDTH_8BIT: // Only 4 pins is available in Bee4.
    case DATAWIDTH_8BIT_DDR:
        Sdio->CTYPE = 1 << 16;
        break;
    default:
        ASSERT(0);
        break;
    }

    UHS_REG_t uhs_reg = {.d32 = Sdio->UHS_REG};
    uhs_reg.b.DDR_REG = (Width == DATAWIDTH_4BIT_DDR || Width == DATAWIDTH_8BIT_DDR) ? 1 : 0;
    Sdio->UHS_REG = uhs_reg.d32;
}


// TODO: Add comment to inform user that pRspBuf must be 4 bytes aligned.
SdioRes_t Sdio_SendNoDataCmd(SDIO_TypeDef *Sdio, const CmdInfo_t *pCmdInfo, void *pRspBuf)
{
    ASSERT(pCmdInfo->CmdIdx <= 63);

    Sdio->CMDARG = pCmdInfo->CmdArg;

    CMD_t cmd = {0};
    SetFieldByCmdInfo(&cmd, pCmdInfo);
    cmd.b.data_expected = 0;
    Sdio->CMD = cmd.d32;

    SdioRes_t Res = CheckResponseByIntr(Sdio);
    if (Res != SDIORES_OK)
    {
        return Res;
    }
    if (pCmdInfo->IsRspExpected)
    {
        ASSERT(pRspBuf != NULL);
        GetResponse(Sdio, pCmdInfo->IsR2Rsp, pRspBuf);
    }

    return SDIORES_OK;
}

SdioRes_t Sdio_SendCmdWithRxData(SDIO_TypeDef *Sdio,
                                 const CmdInfo_t *pCmdInfo, void *pRspBuf,
                                 const DataInfo_t *pDataInfo, void *pRxDataBuf)
{
    return SendCmdWithRxDataByDma(Sdio, pCmdInfo, pRspBuf, pDataInfo, pRxDataBuf);
}

SdioRes_t Sdio_SendCmdWithTxData(SDIO_TypeDef *Sdio,
                                 const CmdInfo_t *pCmdInfo, void *pRspBuf,
                                 const DataInfo_t *pDataInfo, const void *pDataToTx)
{
    return SendCmdWithTxDataByDma(Sdio, pCmdInfo, pRspBuf, pDataInfo, pDataToTx);
}


SdioRes_t Sdio_WaitData0Idle(SDIO_TypeDef *Sdio, uint32_t Timeout_ms)
{
    for (uint32_t i = 0; i < Timeout_ms; ++i)
    {
        STATUS_t status = {.d32 = Sdio->STATUS};
        if (!status.b.data_busy)
        {
            return SDIORES_OK;
        }
        os_delay(1);
    }
    return SDIORES_WAIT_DATA0_IDLE_TIMEOUT;
}


static SdioRes_t CheckResponseByIntr(SDIO_TypeDef *Sdio)
{
    SdioRes_t Res = SDIORES_OK;

    RINTSTS_t rintsts = {.d32 = Sdio->RINTSTS};
    if (rintsts.b.HardwareLockedWriteError)
    {
        Res = SDIORES_HARDWARE_LOCKED_ERROR;
        goto Exit1;
    }

    static const INTMASK_t IntrToCtrl = {.b = {.CommandDone = 1}};
    Sdio->INTMASK |= IntrToCtrl.d32;

    if ((Sdio->RINTSTS & Sdio->INTMASK) == 0)
    {
        // Wait for an interrupt.
        EnableIntrByNvic(Sdio);
        void *Sem = (Sdio == SDIO0) ? gSdio0Sem : gSdio1Sem;
        bool xRes = os_sem_take(Sem, 0xffffffff);
        ASSERT(xRes == true);
    }

    rintsts.d32 = Sdio->RINTSTS;
    if (rintsts.b.ResponseError)
    {
        Res = SDIORES_RESPONSE_ERROR;
        goto Exit2;
    }
    if (rintsts.b.ResponseCrcError)
    {
        Res = SDIORES_RESPONSE_CRC_ERROR;
        goto Exit2;
    }
    if (rintsts.b.ResponseTimeout)
    {
        Res = SDIORES_RESPONSE_TIMEOUT;
        goto Exit2;
    }

Exit2:
    Sdio->INTMASK &= ~IntrToCtrl.d32;

    static const RINTSTS_t IntrToClear = {.b = {
            .HardwareLockedWriteError = 1,
            .CommandDone = 1,
            .ResponseError = 1,
            .ResponseCrcError = 1,
            .ResponseTimeout = 1,
        }
    };
Exit1:
    Sdio->RINTSTS = IntrToClear.d32;
    return Res;
}


static SdioRes_t SendCmdWithRxDataByDma(SDIO_TypeDef *Sdio,
                                        const CmdInfo_t *pCmdInfo, void *pRspBuf,
                                        const DataInfo_t *pDataInfo, void *pRxDataBuf)
{
    ASSERT(pRxDataBuf != NULL && (size_t)pRxDataBuf % 4 == 0);

    uint32_t RxBytes = pDataInfo->BlockSize * pDataInfo->BlockCount;
    ASSERT(RxBytes <= MAX_BYTES_PER_XFER);

    InitDmaDesc(Sdio, (uint32_t)pRxDataBuf, RxBytes);

    CardThrCtl_t card_threshold = {.d32 = Sdio->CardThrCtl};
    card_threshold.b.CardRdThrEn = 1;
    card_threshold.b.CardThreshold = BYTES_PER_BLOCK;
    Sdio->CardThrCtl = card_threshold.d32;

    Sdio->BYTCNT = RxBytes;
    Sdio->BLKSIZ = pDataInfo->BlockSize;

    Sdio->CMDARG = pCmdInfo->CmdArg;

    CMD_t cmd = {0};
    SetFieldByCmdInfo(&cmd, pCmdInfo);
    cmd.b.data_expected = 1;
    cmd.b.send_auto_stop = pDataInfo->SendAutoStop ? 1 : 0;
    cmd.b.transfer_mode = BLOCK_TRANSFER;
    cmd.b.read_write = READ_DATA;
    Sdio->CMD = cmd.d32;

    SdioRes_t Res = CheckResponseByIntr(Sdio);
    if (Res != SDIORES_OK)
    {
        return Res;
    }
    if (pCmdInfo->IsRspExpected)
    {
        ASSERT(pRspBuf != NULL);
        GetResponse(Sdio, pCmdInfo->IsR2Rsp, pRspBuf);
    }

    Res = CheckRxDataXferByIntr(Sdio);
    if (Res != SDIORES_OK)
    {
        return Res;
    }

    Res = CheckDmaStateByIntr(Sdio);
    if (Res != SDIORES_OK)
    {
        return Res;
    }

    SCB_InvalidateDCache_by_Addr(pRxDataBuf, RxBytes);

    return SDIORES_OK;
}


// pDataToTx must be on EXT_DATA_SRAM.
static SdioRes_t SendCmdWithTxDataByDma(SDIO_TypeDef *Sdio,
                                        const CmdInfo_t *pCmdInfo, void *pRspBuf,
                                        const DataInfo_t *pDataInfo, const void *pDataToTx)
{
    ASSERT(pDataToTx != NULL && (size_t)pDataToTx % 4 == 0);

    uint32_t TxBytes = pDataInfo->BlockSize * pDataInfo->BlockCount;
    ASSERT(TxBytes <= MAX_BYTES_PER_XFER);

    SCB_CleanDCache_by_Addr((volatile void *)pDataToTx, TxBytes);

    InitDmaDesc(Sdio, (uint32_t)pDataToTx, TxBytes);

    Sdio->BYTCNT = TxBytes;
    Sdio->BLKSIZ = pDataInfo->BlockSize;

    Sdio->CMDARG = pCmdInfo->CmdArg;

    CMD_t cmd = {0};
    SetFieldByCmdInfo(&cmd, pCmdInfo);
    cmd.b.data_expected = 1;
    cmd.b.send_auto_stop = pDataInfo->SendAutoStop ? 1 : 0;
    cmd.b.transfer_mode = BLOCK_TRANSFER;
    cmd.b.read_write = WRITE_DATA;
    Sdio->CMD = cmd.d32;

    SdioRes_t Res = CheckResponseByIntr(Sdio);
    if (Res != SDIORES_OK)
    {
        return Res;
    }

    if (pCmdInfo->IsRspExpected)
    {
        ASSERT(pRspBuf != NULL);
        GetResponse(Sdio, pCmdInfo->IsR2Rsp, pRspBuf);
    }

    Res = CheckDmaStateByIntr(Sdio);
    if (Res != SDIORES_OK)
    {
        return Res;
    }

    Res = CheckTxDataXferByIntr(Sdio);
    if (Res != SDIORES_OK)
    {
        return Res;
    }

    return SDIORES_OK;
}


static void InitDmaDesc(SDIO_TypeDef *Sdio, uint32_t DataAddr, uint32_t DataBytes)
{
    ASSERT(DataBytes <= MAX_BYTES_PER_XFER);

    BMOD_t bmod = {.d32 = Sdio->BMOD};
    bmod.b.DE = 1;
    bmod.b.DSL = 0; //sizeof(DmaDesc_t) / 4;
    bmod.b.FB = 0;
    bmod.b.SWR = 0;
    Sdio->BMOD = bmod.d32;

    Sdio->IDINTEN = 0x3ff; // Enable, not unmask!!!

    __ALIGNED(4) static volatile DmaDesc_t aDmaDesc0[DESC_CNT] EXT_RAM_DATA = {0};
    __ALIGNED(4) static volatile DmaDesc_t aDmaDesc1[DESC_CNT] EXT_RAM_DATA = {0};
    volatile DmaDesc_t *pDmaDesc = (Sdio == SDIO0) ? aDmaDesc0 : aDmaDesc1;

    uint32_t CurrDescBytes;
    for (uint32_t RemainBytes = DataBytes, DescIdx = 0; RemainBytes > 0;
         RemainBytes -= CurrDescBytes, ++DescIdx)
    {
        ASSERT(DescIdx < DESC_CNT);

        CurrDescBytes = MIN2(RemainBytes, MAX_BYTES_PER_DESC);

        bool IsLastDesc = (CurrDescBytes == RemainBytes);

        pDmaDesc[DescIdx].DES0.DisableInterruptOnCompletion = !IsLastDesc;
        pDmaDesc[DescIdx].DES0.LastDescriptor = IsLastDesc;
        pDmaDesc[DescIdx].DES0.FirstDescriptor = (DescIdx == 0);
        pDmaDesc[DescIdx].DES0.SecondAddressChained = 0;
        pDmaDesc[DescIdx].DES0.EndOfRing = 0;
        pDmaDesc[DescIdx].DES0.CardErrorSummary = 0;
        pDmaDesc[DescIdx].DES0.OWN = OWN_BY_DMA;
        pDmaDesc[DescIdx].DES0.Reserved0 = 0;
        pDmaDesc[DescIdx].DES0.Reserved1 = 0;

        static const uint32_t MaxBytesHalfDesc = MAX_BYTES_PER_DESC / 2;
        if (CurrDescBytes > MaxBytesHalfDesc)
        {
            pDmaDesc[DescIdx].DES1.Buffer1Size = MaxBytesHalfDesc;
            pDmaDesc[DescIdx].DES2.BufferAddressPointer1 = DataAddr;
            DataAddr += pDmaDesc[DescIdx].DES1.Buffer1Size;

            pDmaDesc[DescIdx].DES1.Buffer2Size = CurrDescBytes - MaxBytesHalfDesc;
            pDmaDesc[DescIdx].DES3.BufferAddressPointer2 = DataAddr;
            DataAddr += pDmaDesc[DescIdx].DES1.Buffer2Size;
        }
        else
        {
            pDmaDesc[DescIdx].DES1.Buffer1Size = CurrDescBytes;
            pDmaDesc[DescIdx].DES2.BufferAddressPointer1 = DataAddr;
            DataAddr += pDmaDesc[DescIdx].DES1.Buffer1Size;

            pDmaDesc[DescIdx].DES1.Buffer2Size = 0;
            pDmaDesc[DescIdx].DES3.BufferAddressPointer2 = 0;
        }

        // volatile uint32_t *pu = (volatile uint32_t *)&pDmaDesc[DescIdx];
        // DBG_DIRECT("pDmaDesc[%d]: 0x%x, 0x%x, 0x%x, 0x%x", DescIdx, pu[0], pu[1], pu[2], pu[3]);
    }

    Sdio->DBADDR = (uint32_t)pDmaDesc;

    FIFOTH_t fifoth = {.d32 = Sdio->FIFOTH};
    fifoth.b.TX_WMark = BYTES_PER_BLOCK / 4 / 2; // Half block.
    fifoth.b.RX_WMark = BYTES_PER_BLOCK / 4 / 2;
    Sdio->FIFOTH = fifoth.d32;
}

static SdioRes_t CheckRxDataXferByIntr(SDIO_TypeDef *Sdio)
{
    SdioRes_t Res = SDIORES_OK;

    static const RINTSTS_t xIntrToCtrl =
    {
        .b = {
            .DataCrcError = 1,
            .DataReadTimeout = 1,
            .StartBitError_BusyClearInterrupt = 1,
            .EndBitError_WriteNoCrc = 1,
            .DataTransferOver = 1,
        }
    };

    RINTSTS_t rintsts;
    do
    {
        Sdio->INTMASK |= xIntrToCtrl.d32;

        void *Sem = (Sdio == SDIO0) ? gSdio0Sem : gSdio1Sem;
        if ((Sdio->RINTSTS & Sdio->INTMASK) == 0)
        {
            // Wait for an interrupt.
            EnableIntrByNvic(Sdio);
            bool xRes = os_sem_take(Sem, 0xffffffff);
            ASSERT(xRes == true);
        }

        rintsts.d32 = Sdio->RINTSTS;
        if (rintsts.b.DataCrcError)
        {
            Res = SDIORES_DATA_CRC_ERROR;
            goto Exit;
        }
        if (rintsts.b.DataReadTimeout)
        {
            Res = SDIORES_DATA_READ_TIMEOUT;
            goto Exit;
        }
        if (rintsts.b.StartBitError_BusyClearInterrupt)
        {
            Res = SDIORES_DATA_START_BIT_ERROR;
            goto Exit;
        }
        if (rintsts.b.EndBitError_WriteNoCrc)
        {
            Res = SDIORES_DATA_END_BIT_ERROR;
            goto Exit;
        }
        rintsts.d32 = Sdio->RINTSTS;
    }
    while (rintsts.b.DataTransferOver != 1);
Exit:
    Sdio->INTMASK &= ~xIntrToCtrl.d32;

    // DBG_DIRECT("Sdio->RINTSTS: 0x%x", rintsts.d32);
    Sdio->RINTSTS = xIntrToCtrl.d32;

    return Res;
}

static SdioRes_t CheckTxDataXferByIntr(SDIO_TypeDef *Sdio)
{
    SdioRes_t Res = SDIORES_OK;

    static const RINTSTS_t xIntrToCtrl = {.b = {
            .DataCrcError = 1,
            .DataReadTimeout = 1,
            .EndBitError_WriteNoCrc = 1,
            .DataTransferOver = 1,
        }
    };

    void *Sem = (Sdio == SDIO0) ? gSdio0Sem : gSdio1Sem;
    RINTSTS_t rintsts;
    do
    {
        Sdio->INTMASK |= xIntrToCtrl.d32;

        if ((Sdio->RINTSTS & Sdio->INTMASK) == 0)
        {
            // Wait for an interrupt.
            EnableIntrByNvic(Sdio);
            bool xRes = os_sem_take(Sem, 0xffffffff);
            ASSERT(xRes == true);
        }

        rintsts.d32 = Sdio->RINTSTS;
        if (rintsts.b.DataCrcError)
        {
            Res = SDIORES_DATA_CRC_ERROR;
            goto Exit;
        }
        if (rintsts.b.DataReadTimeout)
        {
            Res = SDIORES_DATA_READ_TIMEOUT;
            goto Exit;
        }
        if (rintsts.b.EndBitError_WriteNoCrc)
        {
            Res = SDIORES_WRITE_NO_CRC;
            goto Exit;
        }
        rintsts.d32 = Sdio->RINTSTS;
    }
    while (rintsts.b.DataTransferOver != 1);
Exit:
    Sdio->INTMASK &= ~xIntrToCtrl.d32;;

    // DBG_DIRECT("Sdio->RINTSTS: 0x%x", rintsts.d32);
    Sdio->RINTSTS = xIntrToCtrl.d32;

    return Res;
}

static SdioRes_t CheckDmaStateByIntr(SDIO_TypeDef *Sdio)
{
    SdioRes_t Res = SDIORES_OK;

    void *Sem = (Sdio == SDIO0) ? gSdio0Sem : gSdio1Sem;
    IDSTS_t idsts;
    do
    {
        if ((Sdio->IDSTS & 0x3ff) == 0)
        {
            // Wait for an interrupt.
            EnableIntrByNvic(Sdio);
            bool xRes = os_sem_take(Sem, 0xffffffff);
            ASSERT(xRes == true);
        }

        idsts.d32 = Sdio->IDSTS;
        // DBG_DIRECT("IDSTS: 0x%x", idsts.d32);
        if (idsts.b.FBE)
        {
            Res = SDIORES_FATAL_BUS_ERROR;
            goto Exit;
        }
        if (idsts.b.DU)
        {
            Res = SDIORES_DESCRIPTOR_UNAVAILABLE;
            goto Exit;
        }
    }
    while (idsts.b.NIS != 1);

Exit:
    Sdio->IDSTS = 0xffffffff;
    return Res;
}


static void ResetAll(SDIO_TypeDef *Sdio)
{
    CTRL_t ctrl = {.d32 = Sdio->CTRL};
    ctrl.b.controller_reset = 1;
    ctrl.b.fifo_reset = 1;
    ctrl.b.dma_reset = 1;
    Sdio->CTRL = ctrl.d32;
    do
    {
        ctrl.d32 = Sdio->CTRL;
    }
    while (ctrl.b.controller_reset == 1 || ctrl.b.fifo_reset == 1 || ctrl.b.dma_reset == 1);
}


static void EnableIntrByNvic(SDIO_TypeDef *Sdio)
{
    NVIC_InitTypeDef Nvic =
    {
        .NVIC_IRQChannel = (Sdio == SDIO0) ? SDIO0_IRQn : SDIO1_IRQn,
        .NVIC_IRQChannelPriority = SDIO_INTR_PRIORITY,
        .NVIC_IRQChannelCmd = ENABLE,
    };
    NVIC_Init(&Nvic);
}


static void DisableIntrByNvic(SDIO_TypeDef *Sdio)
{
    NVIC_InitTypeDef Nvic =
    {
        .NVIC_IRQChannel = (Sdio == SDIO0) ? SDIO0_IRQn : SDIO1_IRQn,
        .NVIC_IRQChannelPriority = SDIO_INTR_PRIORITY,
        .NVIC_IRQChannelCmd = DISABLE,
    };
    NVIC_Init(&Nvic);
}

static void SetFieldByCmdInfo(CMD_t *pCmd, const CmdInfo_t *pCmdInfo)
{
    pCmd->b.start_cmd = 1;
    pCmd->b.use_hold_reg = 1;
    pCmd->b.update_clock_registers_only = 0;
    pCmd->b.card_number = 0;
    pCmd->b.wait_prvdata_complete = 1;

    pCmd->b.cmd_index = pCmdInfo->CmdIdx;
    pCmd->b.send_initialization = pCmdInfo->IsResetCmd;
    pCmd->b.stop_abort_cmd = pCmdInfo->IsStopCmd;
    pCmd->b.response_expect = pCmdInfo->IsRspExpected;
    pCmd->b.response_length = pCmdInfo->IsR2Rsp;
    pCmd->b.check_response_crc = pCmdInfo->CheckRspCrc;
}


static void GetResponse(SDIO_TypeDef *Sdio, bool IsR2Rsp, void *pBuf)
{
    uint32_t aBuf[4];
    if (IsR2Rsp)
    {
        aBuf[0] = Sdio->RESP0;
        aBuf[1] = Sdio->RESP1;
        aBuf[2] = Sdio->RESP2;
        aBuf[3] = Sdio->RESP3;
        // To prevent unaligned access.
        memcpy(pBuf, aBuf, 4 * 4);
    }
    else
    {
        aBuf[0] = Sdio->RESP0;
        memcpy(pBuf, aBuf, 4);
    }
}

static void InitClk(SDIO_TypeDef *Sdio)
{
    Sdh0Phy0_t sdh0phy0 = {.d32 = SDH0_PHY0_REG};
    sdh0phy0.b.r_sdh0_clk_sel_drv = 1;
    sdh0phy0.b.r_sdh0_clk_sel_sample = 1;
    SDH0_PHY0_REG = sdh0phy0.d32;

    SdhCtl_t sdhctl = {.d32 = SDH_CTL_REG};
    if (Sdio == SDIO0)
    {
        sdhctl.b.sdio0_ck_en = 1;
        sdhctl.b.sdio0_func_en = 1;
        sdhctl.b.r_sdio0_clk_src_en = 1;
        sdhctl.b.r_sdio0_div_sel = 0;
        sdhctl.b.r_sdio0_div_en = 0;
        sdhctl.b.r_sdio0_mux_clk_cg_en = 1;
        sdhctl.b.r_sdio0_clk_src_sel = SDIO_CLK_SRC;
    }
    else
    {
        sdhctl.b.sdio1_ck_en = 1;
        sdhctl.b.sdio1_func_en = 1;
        sdhctl.b.r_sdio1_clk_src_en = 1;
        sdhctl.b.r_sdio1_div_sel = 0;
        sdhctl.b.r_sdio1_div_en = 0;
        sdhctl.b.r_sdio1_mux_clk_cg_en = 1;
        sdhctl.b.r_sdio1_clk_src_sel = SDIO_CLK_SRC;
    }
    SDH_CTL_REG = sdhctl.d32;
}


static void DeInitClk(SDIO_TypeDef *Sdio)
{
    SdhCtl_t sdhctl = {.d32 = SDH_CTL_REG};
    if (Sdio == SDIO0)
    {
        sdhctl.b.sdio0_ck_en = 0;
        sdhctl.b.sdio0_func_en = 0;
        sdhctl.b.r_sdio0_clk_src_en = 0;
        sdhctl.b.r_sdio0_div_sel = 0;
        sdhctl.b.r_sdio0_div_en = 1;
        sdhctl.b.r_sdio0_mux_clk_cg_en = 0;
        sdhctl.b.r_sdio0_clk_src_sel = 0;
    }
    else
    {
        sdhctl.b.sdio1_ck_en = 0;
        sdhctl.b.sdio1_func_en = 0;
        sdhctl.b.r_sdio1_clk_src_en = 0;
        sdhctl.b.r_sdio1_div_sel = 0;
        sdhctl.b.r_sdio1_div_en = 0;
        sdhctl.b.r_sdio1_mux_clk_cg_en = 0;
        sdhctl.b.r_sdio1_clk_src_sel = 0;
    }
    SDH_CTL_REG = sdhctl.d32;
}

static uint8_t GetPinCnt(DataWidth_t DataWidth)
{
    switch (DataWidth)
    {
    case DATAWIDTH_1BIT:
        return 3;
    case DATAWIDTH_4BIT:
    case DATAWIDTH_4BIT_DDR:
        return 6;
    case DATAWIDTH_8BIT: // Only 4 pins is available in Bee4.
    case DATAWIDTH_8BIT_DDR:
        return 10;
    default:
        ASSERT(0);
        return 0;
    }
}





void SDIO0_Handler(void)
{
    DisableIntrByNvic(SDIO0);
    os_sem_give(gSdio0Sem);
}

void SDIO1_Handler(void)
{
    DisableIntrByNvic(SDIO1);
    os_sem_give(gSdio1Sem);
}
#include "os_mem.h"
#include "os_sched.h"


// SD 3.01 Command Index
#define SD_GO_IDLE_STATE        0
#define SD_ALL_SEND_CID         2
#define SD_SEND_RELATIVE_ADDR   3
#define SD_SET_DSR              4
#define SD_IO_SEND_OP_COND      5
#define SD_SWITCH_FUNC          6
#define SD_SELECT_DESELECT_CARD 7
#define SD_SEND_IF_COND         8
#define SD_SEND_CSD             9
#define SD_SEND_CID             10
#define SD_VOLTAGE_SWITCH       11
#define SD_READ_DAT_UTIL_STOP   11
#define SD_STOP_TRANSMISSION    12
#define SD_SEND_STATUS          13
#define SD_GO_INACTIVE_STATE    15

#define SD_SET_BLOCKLEN         16
#define SD_READ_SINGLE_BLOCK    17
#define SD_READ_MULTIPLE_BLOCK  18
#define SD_SEND_TUNING_PATTERN  19

#define SD_SET_BLOCK_COUNT      23
#define SD_WRITE_BLOCK          24
#define SD_WRITE_MULTIPLE_BLOCK 25
#define SD_PROGRAM_CSD          27

#define SD_ERASE_WR_BLK_START   32
#define SD_ERASE_WR_BLK_END     33
#define SD_ERASE_CMD            38

#define SD_LOCK_UNLOCK          42
#define SD_IO_RW_DIRECT         52

#define SD_APP_CMD              55
#define SD_GEN_CMD              56

// ACMD
#define SD_SET_BUS_WIDTH        6
#define SD_SD_STATUS            13
#define SD_SEND_NUM_WR_BLOCKS   22
#define SD_SET_WR_BLK_ERASE_COUNT  23
#define SD_SD_APP_OP_COND       41
#define SD_SET_CLR_CARD_DETECT  42
#define SD_SEND_SCR             51


#define SDR12_CLKFREQ_kHz  (25 * 1000)
#define CARD_IDENTIFICATION_MODE_CLK_FREQ_kHz  400

#define CMD6_STATUS_DATA_BYTES  (512/8)


typedef struct
{
    uint32_t Reserved0: 3;
    uint32_t AkeSeqError: 1;

    uint32_t Reserved1: 1;
    uint32_t AppCmd: 1;

    uint32_t Reserved2 : 2;
    uint32_t ReadyForData: 1;
    uint32_t CurrentState: 4;
    uint32_t EraseReset: 1;
    uint32_t CardEccDisabled: 1;
    uint32_t WpErarseSkip: 1;
    uint32_t CsdOverwrite: 1;

    uint32_t Reserved3: 2;
    uint32_t Error: 1;
    uint32_t CcError: 1;
    uint32_t CardEccFailed: 1;
    uint32_t IllegalCommand: 1;
    uint32_t ComCrcError: 1;
    uint32_t LockUnlockFailure: 1;
    uint32_t CardIsLocked: 1;
    uint32_t WpViolation: 1;
    uint32_t EraseParam: 1;
    uint32_t EraseSeqError: 1;
    uint32_t BlockLenError: 1;
    uint32_t AddressError: 1;
    uint32_t OutOfRange: 1;
} R1Rsp_t, CardStatus_t;

#define CARDSTATE_IDLE  0
#define CARDSTATE_READY  1
#define CARDSTATE_IDENT  2
#define CARDSTATE_STBY  3
#define CARDSTATE_TRAN  4
#define CARDSTATE_DATA  5
#define CARDSTATE_RCV  6
#define CARDSTATE_PRG  7
#define CARDSTATE_DIS  8


#define CSD_CID_BYTES  16

typedef struct
{
    uint8_t aCidOrCsd[CSD_CID_BYTES];
} R2Rsp_t;


typedef struct
{
    uint32_t Reserved0: 15;
    uint32_t _2p7_2p8: 1;
    uint32_t _2p8_2p9: 1;
    uint32_t _2p9_3p0: 1;
    uint32_t _3p0_3p1: 1;
    uint32_t _3p1_3p2: 1;
    uint32_t _3p2_3p3: 1;
    uint32_t _3p3_3p4: 1;
    uint32_t _3p4_3p5: 1;
    uint32_t _3p5_3p6: 1;
    uint32_t SwitchingTo1p8VAccepted_S18A: 1;
    uint32_t Reserved1: 5;
    uint32_t CardCapacityStatus_Ccs: 1;
    uint32_t CardPowerUpStatusBit_Busy: 1;
} R3Rsp_t, Ocr_t;

#define R3RSP_BUSY  0
#define R3RSP_NOT_BUSY  1


typedef struct
{
    uint32_t Reserved0: 3;
    uint32_t AkeSeqError: 1;
    uint32_t Reserved1: 1;
    uint32_t AppCmd: 1;
    uint32_t Reserved2 : 2;
    uint32_t ReadyForData: 1;
    uint32_t CurrentState: 4;
    uint32_t Error: 1;
    uint32_t IllegalCommand: 1;
    uint32_t ComCrcError: 1;

    uint32_t NewPublishedRca: 16;
} R6Rsp_t;


typedef struct
{
    uint32_t EchoBackPattern: 8;
    uint32_t VoltageAccepted: 4;
    uint32_t Reserved: 20;
} R7Rsp_t;


typedef struct
{
    uint64_t Reserved0: 32;
    uint64_t CmdSupport: 2;
    uint64_t Reserved1: 9;
    uint64_t ExSecurity: 4;
    uint64_t SdSpec3: 1;
    uint64_t SdBusWidth: 4;
    uint64_t SdSecurity: 3;
    uint64_t DataStatAfterErase: 1;
    uint64_t SdSpec: 4;
    uint64_t ScrStructure: 4;
} Scr_t;
STATIC_ASSERT(sizeof(Scr_t) == 8);


typedef enum
{
    SDTYPE_SDSC = 1,
    SDTYPE_SDHC_OR_SDXC,
} SdCardType_t;


typedef struct
{
    SdCardType_t CardType;
    uint16_t Rca;
    uint8_t aCsd[CSD_CID_BYTES];
    Scr_t Scr;
} SdDb_t;

// To deal with multiple IP instances.
static inline SdDb_t *GetSdDb(SDIO_TypeDef *Sdio)
{
    static SdDb_t SdDb0, SdDb1;
    return (Sdio == SDIO0) ? &SdDb0 : &SdDb1;
}



static SdEmmcRes_t Cmd0_GoIdleState(SDIO_TypeDef *Sdio);
static SdEmmcRes_t Cmd8_SendIfCond(SDIO_TypeDef *Sdio, R7Rsp_t *pR7Rsp);
static SdEmmcRes_t Cmd55_AppCmd(SDIO_TypeDef *Sdio, R1Rsp_t *pR1Rsp);
static SdEmmcRes_t Acmd41_SdSendOpCond(SDIO_TypeDef *Sdio, bool IsHcsSet, R3Rsp_t *pR3Rsp);
static SdEmmcRes_t Cmd2_AllSendCid(SDIO_TypeDef *Sdio, R2Rsp_t *pR2Rsp);
static SdEmmcRes_t Cmd3_SendRelativeAddr(SDIO_TypeDef *Sdio, R6Rsp_t *pR6Rsp);
static SdEmmcRes_t Cmd7_SelectCard(SDIO_TypeDef *Sdio, R1Rsp_t *pR1Rsp);
static SdEmmcRes_t Cmd9_SendCsd(SDIO_TypeDef *Sdio, R2Rsp_t *pR2Rsp);
static SdEmmcRes_t Cmd13_SendStatus(SDIO_TypeDef *Sdio, R1Rsp_t *pR1Rsp);
static SdEmmcRes_t Acmd6_SetBusWidth(SDIO_TypeDef *Sdio, DataWidth_t DataWidth, R1Rsp_t *pR1Rsp);
static SdEmmcRes_t Cmd16_SetBlockLen(SDIO_TypeDef *Sdio, uint32_t BlockLen, R1Rsp_t *pR1Rsp);
static SdEmmcRes_t Acmd42_SetClrCardDetect(SDIO_TypeDef *Sdio, bool IsCdSet, R1Rsp_t *pR1Rsp);
static SdEmmcRes_t Acmd51_SendScr(SDIO_TypeDef *Sdio, R1Rsp_t *pR1Rsp, void *pScr);
static SdEmmcRes_t Cmd6_SwitchFunc(SDIO_TypeDef *Sdio, uint32_t CmdArg, R1Rsp_t *pR1Rsp,
                                   void *pStatusData);

static SdEmmcRes_t RepeatAcmd41UntilNotBusy(SDIO_TypeDef *Sdio, bool IsHcsSet, R3Rsp_t *pR3Rsp,
                                            uint32_t Interval_ms, uint32_t MaxRepeatCnt);
static SdEmmcRes_t RepeatCmd13UntillIntoXferState(SDIO_TypeDef *Sdio, R1Rsp_t *pR1Rsp,
                                                  uint32_t Interval_ms, uint32_t MaxRepeatCnt);




SdEmmcRes_t Sd_Init(SDIO_TypeDef *Sdio, const SdEmmcInitParm_t *pParm)
{
    DBG_DIRECT("%s:%d", __func__, __LINE__);
    if (pParm == NULL || pParm->CardType != CARDTYPE_SD)
    {
        ASSERT(0);
        return SDEMMCRES_ILLEGAL_PARM;
    }
    DBG_DIRECT("%s:%d", __func__, __LINE__);
    Sdio_InitPad(Sdio, pParm->DataWidth);
    Sdio_Init(Sdio);
    Sdio_SetClkOutFreq(Sdio,
                       pParm->ClkOutFreq_kHz /*MIN2(CARD_IDENTIFICATION_MODE_CLK_FREQ_kHz, pParm->ClkOutFreq_kHz)*/);
    DBG_DIRECT("%s:%d", __func__, __LINE__);
    uint8_t *pBuf = os_mem_alloc(RAM_TYPE_EXT_DATA_SRAM, CMD6_STATUS_DATA_BYTES);
    uint8_t *pSwap = os_mem_alloc(RAM_TYPE_EXT_DATA_SRAM, sizeof(Scr_t));
    if (pBuf == NULL || pSwap == NULL)
    {
        ASSERT(0);
        return SDEMMCRES_MALLOC_FAILED;
    }
    DBG_DIRECT("%s:%d", __func__, __LINE__);
    SdEmmcRes_t Res = SDEMMCRES_OK;
    R1Rsp_t R1Rsp;
    R2Rsp_t R2Rsp;
    R3Rsp_t R3Rsp;
    R6Rsp_t R6Rsp;
    R7Rsp_t R7Rsp;
    SdDb_t *pSdDb = GetSdDb(Sdio);
    DBG_DIRECT("%s:%d", __func__, __LINE__);
    // The following steps are based on SD spec V3.01 figure 4-2 and figure 4-9.
    Res = Cmd0_GoIdleState(Sdio);
    if (Res != SDEMMCRES_OK)
    {
        goto Exit;
    }
    DBG_DIRECT("%s:%d", __func__, __LINE__);
    Res = Cmd8_SendIfCond(Sdio, &R7Rsp);
    if (Res == SDEMMCRES_OK)
    {
        Res = RepeatAcmd41UntilNotBusy(Sdio, true, &R3Rsp, 2, 500);
        if (Res != SDEMMCRES_OK)
        {
            goto Exit;
        }
        pSdDb->CardType = (R3Rsp.CardCapacityStatus_Ccs == 1) ? SDTYPE_SDHC_OR_SDXC : SDTYPE_SDSC;
    }
    else
    {
        Res = RepeatAcmd41UntilNotBusy(Sdio, false, &R3Rsp, 2, 500);
        if (Res != SDEMMCRES_OK)
        {
            goto Exit;
        }
        pSdDb->CardType = SDTYPE_SDSC;
    }
    DBG_DIRECT("%s:%d", __func__, __LINE__);
    Res = Cmd2_AllSendCid(Sdio, &R2Rsp);
    if (Res != SDEMMCRES_OK)
    {
        goto Exit;
    }

    Res = Cmd3_SendRelativeAddr(Sdio, &R6Rsp);
    if (Res != SDEMMCRES_OK || R6Rsp.NewPublishedRca == 0)
    {
        goto Exit;
    }
    pSdDb->Rca = R6Rsp.NewPublishedRca;

    Res = Cmd9_SendCsd(Sdio, &R2Rsp);
    if (Res != SDEMMCRES_OK)
    {
        goto Exit;
    }
    memcpy(pSdDb->aCsd, &R2Rsp, CSD_CID_BYTES);

    Res = Cmd7_SelectCard(Sdio, &R1Rsp);
    if (Res != SDEMMCRES_OK)
    {
        goto Exit;
    }
    DBG_DIRECT("%s:%d", __func__, __LINE__);
    Res = Acmd42_SetClrCardDetect(Sdio, false, &R1Rsp);
    if (Res != SDEMMCRES_OK)
    {
        goto Exit;
    }

    Res = Cmd16_SetBlockLen(Sdio, BYTES_PER_BLOCK, &R1Rsp);
    if (Res != SDEMMCRES_OK)
    {
        goto Exit;
    }

    Sdio_SetClkOutFreq(Sdio, MIN2(SDR12_CLKFREQ_kHz, pParm->ClkOutFreq_kHz));

    Res = Acmd51_SendScr(Sdio, &R1Rsp, pBuf);
    if (Res != SDEMMCRES_OK)
    {
        goto Exit;
    }
    for (uint8_t i = 0; i < sizeof(Scr_t); i++)
    {
        *(pSwap + sizeof(Scr_t) - 1 - i) = *(pBuf + i);
    }
    memcpy(&pSdDb->Scr, pSwap, sizeof(Scr_t));

    // Refer to SD spec V3.01 4.3.10
    if (pParm->ClkOutFreq_kHz > SDR12_CLKFREQ_kHz && pSdDb->Scr.SdSpec >= 1)
    {
        Res = Cmd6_SwitchFunc(Sdio, 0x00fffff1, &R1Rsp, pBuf);
        if (Res != SDEMMCRES_OK)
        {
            goto Exit;
        }
        // StatusData[379:376] == 0x1, which means switch to SDR25 is allowed.
        if ((pBuf[16] & 0xf) == 0x1)
        {
            Res = Cmd6_SwitchFunc(Sdio, 0x80fffff1, &R1Rsp, pBuf);
            if (Res != SDEMMCRES_OK)
            {
                goto Exit;
            }

            //Check transfer state before switching host to SDR25
            Res = RepeatCmd13UntillIntoXferState(Sdio, &R1Rsp, 1, 1000);
            if (Res != SDEMMCRES_OK)
            {
                goto Exit;
            }
            Sdio_SetClkOutFreq(Sdio, pParm->ClkOutFreq_kHz);
        }
    }

    Res = Acmd6_SetBusWidth(Sdio, pParm->DataWidth, &R1Rsp);
    if (Res != SDEMMCRES_OK)
    {
        goto Exit;
    }
    Sdio_SetHostDataWidth(Sdio, pParm->DataWidth);
    DBG_DIRECT("%s:%d", __func__, __LINE__);
Exit:
    os_mem_free(pBuf);
    os_mem_free(pSwap); DBG_DIRECT("%s:%d", __func__, __LINE__);
    return Res;
}


SdEmmcRes_t Sd_Read(SDIO_TypeDef *Sdio, uint32_t StartBlock, uint32_t BlockCnt, void *pBuf)
{

    uint8_t *puBuf = pBuf;
    uint32_t BlockAddr = StartBlock;
    uint32_t RemainBlock = BlockCnt;

    while (RemainBlock > 0)
    {
        uint32_t BlockCntSend = MIN2(MAX_BLOCK_PER_XFER, RemainBlock);

        const CmdInfo_t Cmd18 =
        {
            .CmdIdx = SD_READ_MULTIPLE_BLOCK,
            .CmdArg = (GetSdDb(Sdio)->CardType == SDTYPE_SDHC_OR_SDXC) ? BlockAddr : BlockAddr * BYTES_PER_BLOCK,
            .IsResetCmd = false,
            .IsStopCmd = false,
            .IsRspExpected = true,
            .IsR2Rsp = false,
            .CheckRspCrc = true,
        };
        const DataInfo_t DataInfo =
        {
            .BlockSize = BYTES_PER_BLOCK,
            .BlockCount = BlockCntSend,
            .SendAutoStop = true,
        };
        R1Rsp_t R1Rsp;
        SdioRes_t SdioRes = Sdio_SendCmdWithRxData(Sdio, &Cmd18, &R1Rsp, &DataInfo, puBuf);
        if (SdioRes != SDIORES_OK || R1Rsp.Error)
        {
            DBG_DIRECT("SdioRes: %d", SdioRes);
            return SDEMMCRES_CMD18_ERROR;
        }

        puBuf += (BlockCntSend * BYTES_PER_BLOCK);
        BlockAddr += BlockCntSend;
        RemainBlock -= BlockCntSend;
    }

    return SDEMMCRES_OK;
}

SdEmmcRes_t Sd_Write(SDIO_TypeDef *Sdio, uint32_t StartBlock, uint32_t BlockCnt, const void *pBuf)
{
    const uint8_t *puBuf = pBuf;
    uint32_t BlockAddr = StartBlock;
    uint32_t RemainBlock = BlockCnt;

    while (RemainBlock > 0)
    {
        uint32_t BlockCntSend = MIN2(MAX_BLOCK_PER_XFER, RemainBlock);

        const CmdInfo_t Cmd25 =
        {
            .CmdIdx = SD_WRITE_MULTIPLE_BLOCK,
            .CmdArg = (GetSdDb(Sdio)->CardType == SDTYPE_SDHC_OR_SDXC) ? BlockAddr : BlockAddr * BYTES_PER_BLOCK,
            .IsResetCmd = false,
            .IsStopCmd = false,
            .IsRspExpected = true,
            .IsR2Rsp = false,
            .CheckRspCrc = true,
        };
        const DataInfo_t DataInfo =
        {
            .BlockSize = BYTES_PER_BLOCK,
            .BlockCount = BlockCntSend,
            .SendAutoStop = true,
        };
        R1Rsp_t R1Rsp;
        SdioRes_t SdioRes = Sdio_SendCmdWithTxData(Sdio, &Cmd25, &R1Rsp, &DataInfo, puBuf);
        if (SdioRes != SDIORES_OK || R1Rsp.Error)
        {
            DBG_DIRECT("SdioRes: %d", SdioRes);
            return SDEMMCRES_CMD25_ERROR;
        }

        SdioRes = Sdio_WaitData0Idle(Sdio, 2000);
        if (SdioRes != SDIORES_OK)
        {
            return SDEMMCRES_WRITE_TIMEOUT;
        }

        SdEmmcRes_t SdEmmcRes = RepeatCmd13UntillIntoXferState(Sdio, &R1Rsp, 1, 1000);
        if (SdEmmcRes != SDEMMCRES_OK)
        {
            return SdEmmcRes;
        }

        puBuf += (BlockCntSend * BYTES_PER_BLOCK);
        BlockAddr += BlockCntSend;
        RemainBlock -= BlockCntSend;
    }

    return SDEMMCRES_OK;
}

uint32_t Sd_GetBlockCnt(SDIO_TypeDef *Sdio)
{
    SdDb_t *pSdDb = GetSdDb(Sdio);

    uint64_t Capacity_Byte = 0;
    if (pSdDb->CardType == SDTYPE_SDHC_OR_SDXC)
    {
        uint32_t CSize = BitsToU32(pSdDb->aCsd, 48, 69);
        Capacity_Byte = (CSize + 1ULL) * 1024 * 512;
    }
    else
    {
        uint32_t CSize = BitsToU32(pSdDb->aCsd, 62, 73);
        uint32_t CSizeMult = BitsToU32(pSdDb->aCsd, 47, 49);
        uint32_t ReadBlLen = BitsToU32(pSdDb->aCsd, 80, 83);

        uint32_t Mult = 1UL << (CSizeMult + 2);
        uint32_t BlockLen = 1UL << ReadBlLen;
        uint32_t Blocknr = (CSize + 1) * Mult;

        Capacity_Byte = Blocknr * BlockLen;
    }

    return Capacity_Byte / BYTES_PER_BLOCK;
}


static SdEmmcRes_t Cmd0_GoIdleState(SDIO_TypeDef *Sdio)
{
    static const CmdInfo_t Cmd0 =
    {
        .CmdIdx = SD_GO_IDLE_STATE,
        .CmdArg = 0,
        .IsResetCmd = true,
        .IsStopCmd = false,
        .IsRspExpected = false,
        .IsR2Rsp = false,
        .CheckRspCrc = false,
    };
    SdioRes_t Res = Sdio_SendNoDataCmd(Sdio, &Cmd0, NULL);
    if (Res != SDIORES_OK)
    {
        return SDEMMCRES_CMD0_ERROR;
    }
    return SDEMMCRES_OK;
}

static SdEmmcRes_t Cmd8_SendIfCond(SDIO_TypeDef *Sdio, R7Rsp_t *pR7Rsp)
{
    static const CmdInfo_t Cmd8 =
    {
        .CmdIdx = SD_SEND_IF_COND,
        .CmdArg = 0x1aa,
        .IsResetCmd = false,
        .IsStopCmd = false,
        .IsRspExpected = true,
        .IsR2Rsp = false,
        .CheckRspCrc = true,
    };

    uint32_t R7Rsp;
    SdioRes_t Res = Sdio_SendNoDataCmd(Sdio, &Cmd8, pR7Rsp);
    if (Res != SDIORES_OK)
    {
        return SDEMMCRES_CMD8_ERROR;
    }

    return SDEMMCRES_OK;
}

static SdEmmcRes_t Cmd55_AppCmd(SDIO_TypeDef *Sdio, R1Rsp_t *pR1Rsp)
{
    const CmdInfo_t Cmd55 =
    {
        .CmdIdx = SD_APP_CMD,
        .CmdArg = GetSdDb(Sdio)->Rca << 16,
                                     .IsResetCmd = false,
                                     .IsStopCmd = false,
                                     .IsRspExpected = true,
                                     .IsR2Rsp = false,
                                     .CheckRspCrc = false,
    };

    R1Rsp_t R1Rsp;
    SdioRes_t Res = Sdio_SendNoDataCmd(Sdio, &Cmd55, pR1Rsp);
    if (Res != SDIORES_OK)
    {
        return SDEMMCRES_CMD55_ERROR;
    }

    return SDEMMCRES_OK;
}

static SdEmmcRes_t Acmd41_SdSendOpCond(SDIO_TypeDef *Sdio, bool IsHcsSet, R3Rsp_t *pR3Rsp)
{
    R1Rsp_t R1Rsp;
    SdEmmcRes_t SdEmmcRes = Cmd55_AppCmd(Sdio, &R1Rsp);
    if (SdEmmcRes != SDEMMCRES_OK || R1Rsp.Error)
    {
        return SDEMMCRES_CMD55_ERROR;
    }

    CmdInfo_t Acmd41 =
    {
        .CmdIdx = SD_SD_APP_OP_COND,
        .CmdArg = BIT28 | BIT15 | BIT16 | BIT17 | BIT18 | BIT19 | BIT20 | BIT21 | BIT22 | BIT23,
        .IsResetCmd = false,
        .IsStopCmd = false,
        .IsRspExpected = true,
        .IsR2Rsp = false,
        .CheckRspCrc = false,
    };
    if (IsHcsSet)
    {
        Acmd41.CmdArg |= BIT30;
    }

    SdioRes_t Res = Sdio_SendNoDataCmd(Sdio, &Acmd41, pR3Rsp);
    if (Res != SDIORES_OK)
    {
        return SDEMMCRES_ACMD41_ERROR;
    }

    return SDEMMCRES_OK;
}

static SdEmmcRes_t RepeatAcmd41UntilNotBusy(SDIO_TypeDef *Sdio, bool IsHcsSet, R3Rsp_t *pR3Rsp,
                                            uint32_t Interval_ms, uint32_t MaxRepeatCnt)
{
    for (uint32_t i = 0; i < MaxRepeatCnt; ++i)
    {
        os_delay(Interval_ms);

        SdEmmcRes_t Res = Acmd41_SdSendOpCond(Sdio, IsHcsSet, pR3Rsp);
        if (Res != SDEMMCRES_OK)
        {
            return Res;
        }
        if (pR3Rsp->CardPowerUpStatusBit_Busy == R3RSP_NOT_BUSY)
        {
            return SDEMMCRES_OK;
        }
    }

    return SDEMMCRES_ACMD41_TIMEOUT;
}


static SdEmmcRes_t RepeatCmd13UntillIntoXferState(SDIO_TypeDef *Sdio, R1Rsp_t *pR1Rsp,
                                                  uint32_t Interval_ms, uint32_t MaxRepeatCnt)
{
    for (uint32_t i = 0; i < MaxRepeatCnt; ++i)
    {
        SdEmmcRes_t Res = Cmd13_SendStatus(Sdio, pR1Rsp);
        if (Res != SDEMMCRES_OK)
        {
            return Res;
        }
        if (pR1Rsp->CurrentState == CARDSTATE_TRAN && pR1Rsp->ReadyForData)
        {
            return SDEMMCRES_OK;
        }
        os_delay(Interval_ms);
    }

    return SDEMMCRES_CMD13_TIMEOUT;
}

static SdEmmcRes_t Cmd2_AllSendCid(SDIO_TypeDef *Sdio, R2Rsp_t *pR2Rsp)
{
    static const CmdInfo_t Cmd2 =
    {
        .CmdIdx = SD_ALL_SEND_CID,
        .CmdArg = 0,
        .IsResetCmd = false,
        .IsStopCmd = false,
        .IsRspExpected = true,
        .IsR2Rsp = true,
        .CheckRspCrc = false,
    };

    SdioRes_t Res = Sdio_SendNoDataCmd(Sdio, &Cmd2, pR2Rsp);
    if (Res != SDIORES_OK)
    {
        return SDEMMCRES_CMD2_ERROR;
    }

    return SDEMMCRES_OK;
}


static SdEmmcRes_t Cmd3_SendRelativeAddr(SDIO_TypeDef *Sdio, R6Rsp_t *pR6Rsp)
{
    static const CmdInfo_t Cmd3 =
    {
        .CmdIdx = SD_SEND_RELATIVE_ADDR,
        .CmdArg = 0,
        .IsResetCmd = false,
        .IsStopCmd = false,
        .IsRspExpected = true,
        .IsR2Rsp = false,
        .CheckRspCrc = true,
    };
    SdioRes_t Res = Sdio_SendNoDataCmd(Sdio, &Cmd3, pR6Rsp);
    if (Res != SDIORES_OK)
    {
        return SDEMMCRES_CMD3_ERROR;
    }

    return SDEMMCRES_OK;
}


static SdEmmcRes_t Cmd7_SelectCard(SDIO_TypeDef *Sdio, R1Rsp_t *pR1Rsp)
{
    const CmdInfo_t Cmd7 =
    {
        .CmdIdx = SD_SELECT_DESELECT_CARD,
        .CmdArg = GetSdDb(Sdio)->Rca << 16,
                                     .IsResetCmd = false,
                                     .IsStopCmd = false,
                                     .IsRspExpected = true,
                                     .IsR2Rsp = false,
                                     .CheckRspCrc = true,
    };
    SdioRes_t Res = Sdio_SendNoDataCmd(Sdio, &Cmd7, pR1Rsp);
    if (Res != SDIORES_OK)
    {
        return SDEMMCRES_CMD7_ERROR;
    }

    return SDEMMCRES_OK;
}


static SdEmmcRes_t Cmd9_SendCsd(SDIO_TypeDef *Sdio, R2Rsp_t *pR2Rsp)
{
    const CmdInfo_t Cmd9 =
    {
        .CmdIdx = SD_SEND_CSD,
        .CmdArg = GetSdDb(Sdio)->Rca << 16,
                                     .IsResetCmd = false,
                                     .IsStopCmd = false,
                                     .IsRspExpected = true,
                                     .IsR2Rsp = true,
                                     .CheckRspCrc = false,
    };

    SdioRes_t Res = Sdio_SendNoDataCmd(Sdio, &Cmd9, pR2Rsp);
    if (Res != SDIORES_OK)
    {
        return SDEMMCRES_CMD9_ERROR;
    }

    return SDEMMCRES_OK;
}

static SdEmmcRes_t Cmd13_SendStatus(SDIO_TypeDef *Sdio, R1Rsp_t *pR1Rsp)
{
    const CmdInfo_t Cmd13 =
    {
        .CmdIdx = SD_SEND_STATUS,
        .CmdArg = GetSdDb(Sdio)->Rca << 16,
                                     .IsResetCmd = false,
                                     .IsStopCmd = false,
                                     .IsRspExpected = true,
                                     .IsR2Rsp = false,
                                     .CheckRspCrc = true,
    };
    SdioRes_t Res = Sdio_SendNoDataCmd(Sdio, &Cmd13, pR1Rsp);
    if (Res != SDIORES_OK)
    {
        return SDEMMCRES_CMD13_ERROR;
    }

    return SDEMMCRES_OK;
}

static SdEmmcRes_t Acmd6_SetBusWidth(SDIO_TypeDef *Sdio, DataWidth_t DataWidth, R1Rsp_t *pR1Rsp)
{
    if (DataWidth != DATAWIDTH_1BIT && DataWidth != DATAWIDTH_4BIT)
    {
        return SDEMMCRES_ILLEGAL_PARM;
    }

    SdEmmcRes_t SdEmmcRes = Cmd55_AppCmd(Sdio, pR1Rsp);
    if (SdEmmcRes != SDEMMCRES_OK || pR1Rsp->Error)
    {
        return SDEMMCRES_CMD55_ERROR;
    }

    const CmdInfo_t Acmd6 =
    {
        .CmdIdx = SD_SET_BUS_WIDTH,
        .CmdArg = (DataWidth == DATAWIDTH_4BIT) ? 0x2 : 0x0,
        .IsResetCmd = false,
        .IsStopCmd = false,
        .IsRspExpected = true,
        .IsR2Rsp = false,
        .CheckRspCrc = false,
    };

    SdioRes_t Res = Sdio_SendNoDataCmd(Sdio, &Acmd6, pR1Rsp);
    if (Res != SDIORES_OK)
    {
        return SDEMMCRES_ACMD6_ERROR;
    }

    return SDEMMCRES_OK;
}

static SdEmmcRes_t Cmd16_SetBlockLen(SDIO_TypeDef *Sdio, uint32_t BlockLen, R1Rsp_t *pR1Rsp)
{
    const CmdInfo_t Cmd16 =
    {
        .CmdIdx = SD_SET_BLOCKLEN,
        .CmdArg = BlockLen,
        .IsResetCmd = false,
        .IsStopCmd = false,
        .IsRspExpected = true,
        .IsR2Rsp = false,
        .CheckRspCrc = true,
    };
    SdioRes_t Res = Sdio_SendNoDataCmd(Sdio, &Cmd16, pR1Rsp);
    if (Res != SDIORES_OK)
    {
        return SDEMMCRES_CMD16_ERROR;
    }

    return SDEMMCRES_OK;
}


static SdEmmcRes_t Acmd42_SetClrCardDetect(SDIO_TypeDef *Sdio, bool IsCdSet, R1Rsp_t *pR1Rsp)
{
    SdEmmcRes_t SdEmmcRes = Cmd55_AppCmd(Sdio, pR1Rsp);
    if (SdEmmcRes != SDEMMCRES_OK || pR1Rsp->Error)
    {
        return SDEMMCRES_CMD55_ERROR;
    }

    const CmdInfo_t Acmd42 =
    {
        .CmdIdx = SD_SET_CLR_CARD_DETECT,
        .CmdArg = IsCdSet,
        .IsResetCmd = false,
        .IsStopCmd = false,
        .IsRspExpected = true,
        .IsR2Rsp = false,
        .CheckRspCrc = true,
    };
    SdioRes_t Res = Sdio_SendNoDataCmd(Sdio, &Acmd42, pR1Rsp);
    if (Res != SDIORES_OK)
    {
        return SDEMMCRES_ACMD42_ERROR;
    }

    return SDEMMCRES_OK;
}


static SdEmmcRes_t Acmd51_SendScr(SDIO_TypeDef *Sdio, R1Rsp_t *pR1Rsp, void *pScr)
{
    SdEmmcRes_t SdEmmcRes = Cmd55_AppCmd(Sdio, pR1Rsp);
    if (SdEmmcRes != SDEMMCRES_OK || pR1Rsp->Error)
    {
        return SDEMMCRES_CMD55_ERROR;
    }

    static const CmdInfo_t Acmd51 =
    {
        .CmdIdx = SD_SEND_SCR,
        .CmdArg = 0,
        .IsResetCmd = false,
        .IsStopCmd = false,
        .IsRspExpected = true,
        .IsR2Rsp = false,
        .CheckRspCrc = true,
    };
    static const DataInfo_t DataInfo =
    {
        .BlockSize = sizeof(Scr_t),
        .BlockCount = 1,
        .SendAutoStop = false,
    };
    SdioRes_t Res = Sdio_SendCmdWithRxData(Sdio, &Acmd51, pR1Rsp, &DataInfo, pScr);
    if (Res != SDIORES_OK)
    {
        return SDEMMCRES_ACMD51_ERROR;
    }

    return SDEMMCRES_OK;
}


static SdEmmcRes_t Cmd6_SwitchFunc(SDIO_TypeDef *Sdio, uint32_t CmdArg, R1Rsp_t *pR1Rsp,
                                   void *pStatusData)
{
    const CmdInfo_t Cmd6 =
    {
        .CmdIdx = SD_SWITCH_FUNC,
        .CmdArg = CmdArg,
        .IsResetCmd = false,
        .IsStopCmd = false,
        .IsRspExpected = true,
        .IsR2Rsp = false,
        .CheckRspCrc = true,
    };
    static const DataInfo_t DataInfo =
    {
        .BlockSize = CMD6_STATUS_DATA_BYTES,
        .BlockCount = 1,
        .SendAutoStop = false,
    };
    SdioRes_t Res = Sdio_SendCmdWithRxData(Sdio, &Cmd6, pR1Rsp, &DataInfo, pStatusData);
    if (Res != SDIORES_OK)
    {
        return SDEMMCRES_CMD6_ERROR;
    }

    return SDEMMCRES_OK;
}

static inline void SetCardType(SDIO_TypeDef *Sdio, CardType_t CardType);
static inline CardType_t GetCardType(SDIO_TypeDef *Sdio);



//SdEmmcRes_t SdEmmc_Init(SDIO_TypeDef *Sdio, const SdEmmcInitParm_t *pParm)
//{
//    return (GetCardType(Sdio) == CARDTYPE_SD) ? Sd_Init(Sdio, pParm) : Emmc_Init(Sdio, pParm);
//}

//SdEmmcRes_t SdEmmc_Read(SDIO_TypeDef *Sdio, uint32_t StartBlock, uint32_t BlockCnt, void *pBuf)
//{
//    return (GetCardType(Sdio) == CARDTYPE_SD) ?
//           Sd_Read(Sdio, StartBlock, BlockCnt, pBuf) :
//           Emmc_Read(Sdio, StartBlock, BlockCnt, pBuf);
//}

//SdEmmcRes_t SdEmmc_Write(SDIO_TypeDef *Sdio, uint32_t StartBlock, uint32_t BlockCnt,
//                         const void *pBuf)
//{
//    return (GetCardType(Sdio) == CARDTYPE_SD) ?
//           Sd_Write(Sdio, StartBlock, BlockCnt, pBuf) :
//           Emmc_Write(Sdio, StartBlock, BlockCnt, pBuf);
//}

//uint32_t SdEmmc_GetBlockCnt(SDIO_TypeDef *Sdio)
//{
//    return (GetCardType(Sdio) == CARDTYPE_SD) ? Sd_GetBlockCnt(Sdio) : Emmc_GetBlockCnt(Sdio);
//}



static CardType_t gSdio0CardType, gSdio1CardType;

static inline void SetCardType(SDIO_TypeDef *Sdio, CardType_t CardType)
{
    if (Sdio == SDIO0)
    {
        gSdio0CardType = CardType;
    }
    else
    {
        gSdio1CardType = CardType;
    }
}

static inline CardType_t GetCardType(SDIO_TypeDef *Sdio)
{
    return (Sdio == SDIO0) ? gSdio0CardType : gSdio1CardType;
}



#endif
