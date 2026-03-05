/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_i2c.c
* @brief    This file provides all the I2C firmware functions.
* @details
* @author   elliot chen
* @date     2015-04-29
* @version  v0.1
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_rcc.h"
#include "rtl876x_i2c.h"
#include "os_sched.h"
#include "bmp80.h"
#include "trace.h"

uint32_t I2C_TimeOut = 0xFFFFF;
int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
uint16_t ac4, ac5, ac6;


/**
  * @brief  Initializes the I2Cx peripheral according to the specified
  *   parameters in the I2C_InitStruct.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  I2C_InitStruct: pointer to a I2C_InitTypeDef structure that
  *   contains the configuration information for the specified I2C peripheral.
  * @retval None
  */

uint16_t read16(uint8_t a) {
  uint8_t retbuf[2];
  uint16_t ret;

  // send 1 byte, reset i2c, read 2 bytes
  // we could typecast uint16_t as uint8_t array but would need to ensure proper
  // endianness
	I2C_Status status1, status2;
	status1 = I2C_MasterWrite(I2C0, &a, 1);
	status2 = I2C_MasterRead(I2C0, retbuf, 2);
	DBG_DIRECT("Read16 MasterWrite Status: %d MasterRead Status: %d", status1, status2);
  // write_then_read uses uint8_t array
  ret = retbuf[1] | (retbuf[0] << 8);

  return ret;
}

void write8(uint8_t a, uint8_t d) {
	I2C_Status status1, status2;
	status1 = I2C_MasterWrite(I2C0, &a, 1);
	status2 = I2C_MasterWrite(I2C0, &d, 1);
	DBG_DIRECT("Write8 MasterWrite Status: %d MasterRead Status: %d", status1, status2);
}

uint8_t read8(uint8_t a) {
  uint8_t ret;

  // send 1 byte, reset i2c, read 1 byte
	I2C_Status status1, status2;
	status1 = I2C_MasterWrite(I2C0, &a, 1);
	status2 = I2C_MasterRead(I2C0, &ret, 1);
	//DBG_DIRECT("Read8 value decimal: %d hex: %x", ret, ret);
	//DBG_DIRECT("Read8 MasterWrite Status: %d MasterRead Status: %d", status1, status2);
  return ret;
}

int32_t computeB5(int32_t UT) {
  int32_t X1 = (UT - (int32_t)ac6) * ((int32_t)ac5) >> 15;
  int32_t X2 = ((int32_t)mc << 11) / (X1 + (int32_t)md);
  return X1 + X2;
}

bool begin_bmp() {

  //i2c_dev = new Adafruit_I2CDevice(BMP085_I2CADDR, wire);

	uint8_t read_value = read8(0xD0);
	DBG_DIRECT("read_value = %x", read_value);

  if (read_value != 0x55)
    return false;
	
	//write8(0xE0, 0xB6);
	os_delay(10);
	
  /* read calibration data */
  ac1 = read16(BMP085_CAL_AC1);
  ac2 = read16(BMP085_CAL_AC2);
  ac3 = read16(BMP085_CAL_AC3);
  ac4 = read16(BMP085_CAL_AC4);
  ac5 = read16(BMP085_CAL_AC5);
  ac6 = read16(BMP085_CAL_AC6);

  b1 = read16(BMP085_CAL_B1);
  b2 = read16(BMP085_CAL_B2);

  mb = read16(BMP085_CAL_MB);
  mc = read16(BMP085_CAL_MC);
  md = read16(BMP085_CAL_MD);
	
  DBG_DIRECT("ac1 = %d", ac1);
  DBG_DIRECT("ac2 = %d", ac2);
  DBG_DIRECT("ac3 = %d", ac3);
  DBG_DIRECT("ac4 = %d", ac4);
  DBG_DIRECT("ac5 = %d", ac5);
  DBG_DIRECT("ac6 = %d", ac6);

  DBG_DIRECT("b1 = %d", b1);
  DBG_DIRECT("b2 = %d", b2);

  DBG_DIRECT("mb = %d", mb);
  DBG_DIRECT("mc = %d", mc);
  DBG_DIRECT("md = %d", md);

  return true;
}

#define BMP180_ADDR 0x77
#define BITSHIFT_BMP180_ADDR (0x77 << 1)

uint16_t readRawTemperature(void) {
	I2C_SendCmd(I2C0, I2C_WRITE_CMD, 0xF4, I2C_STOP_DISABLE);
	I2C_SendCmd(I2C0, I2C_WRITE_CMD, 0x2E, I2C_STOP_DISABLE);
	os_delay(50);
	I2C_SendCmd(I2C0, I2C_WRITE_CMD, 0xF6, I2C_STOP_DISABLE);
  return read16(BMP085_TEMPDATA);
}

float readTemperature(void) {
  int32_t UT, B5; // following ds convention
  float temp;

  UT = readRawTemperature();
	DBG_DIRECT("RawTemperature: %d", UT);
  B5 = computeB5(UT);
	DBG_DIRECT("computeB5: %d", B5);
  temp = (B5 + 8) >> 4;
  temp /= 10;
	DBG_DIRECT("temp: %f", temp);
  return temp;
}

void I2C_Init(I2C_TypeDef *I2Cx, I2C_InitTypeDef *I2C_InitStruct)
{
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));
    assert_param(IS_I2C_CLOCK_SPEED(I2C_InitStruct->I2C_ClockSpeed));

    volatile uint32_t I2CSrcClk;

    I2CSrcClk = I2C_InitStruct->I2C_Clock;
    /* Disable I2C device before change configuration */
    I2Cx->IC_ENABLE &= ~0x0001;

    /* ------------------------------ Initialize I2C device ------------------------------*/
    if (I2C_DeviveMode_Master == I2C_InitStruct->I2C_DeviveMode)
    {
        /*configure I2C device mode which can be selected for master or slave*/
        I2Cx->IC_CON = I2C_InitStruct->I2C_DeviveMode | (I2C_InitStruct->I2C_AddressMode << 4) | BIT(5);

        /*set target address*/
        I2Cx->IC_TAR = (I2C_InitStruct->I2C_SlaveAddress & 0x3ff)
                       | (I2C_InitStruct->I2C_AddressMode << 12);
        /*set SDA hold time in master mode*/
        I2Cx->IC_SDA_HOLD = 0x01;

    }
    else
    {
        /* set to slave mode */
        I2Cx->IC_CON = (I2C_InitStruct->I2C_DeviveMode) | (I2C_InitStruct->I2C_AddressMode << 3);
        /* set Ack in slave mode */
        I2Cx->IC_ACK_GENERAL_CALL &= I2C_InitStruct->I2C_Ack;
        /* set slave address */
        I2Cx->IC_SAR = I2C_InitStruct->I2C_SlaveAddress;
        /* set SDA hold time in slave mode */
         I2Cx->IC_SDA_SETUP = 0x02;
    }

#if 1
    /*set Tx empty level*/
    I2Cx->IC_TX_TL = I2C_InitStruct->I2C_TxThresholdLevel;
    /*set Rx full level*/
    I2Cx->IC_RX_TL = I2C_InitStruct->I2C_RxThresholdLevel;
#endif

    /*------------------------------ configure I2C speed ------------------------------*/
    /*Configure I2C speed in standard mode*/
    if (I2C_InitStruct->I2C_ClockSpeed <= 100000)
    {
        I2Cx->IC_CON |= (0x3 << 1);
        I2Cx->IC_CON &= 0xfffb;
        /*configure I2C speed*/
        I2Cx->IC_SS_SCL_HCNT = 20 + (4000 * (I2CSrcClk / 10000)) / (I2C_InitStruct->I2C_ClockSpeed);
        I2Cx->IC_SS_SCL_LCNT = 20 + (4700 * (I2CSrcClk / 10000)) / (I2C_InitStruct->I2C_ClockSpeed);
    }
    /*Configure I2C speed in fast mode*/
    else if (I2C_InitStruct->I2C_ClockSpeed <= 400000)
    {

        I2Cx->IC_CON |= (0x3 << 1);
        I2Cx->IC_CON &= 0xfffd;
        if (I2C_InitStruct->I2C_ClockSpeed == 200000)
        {
            /*configure I2C speed*/
            I2Cx->IC_FS_SCL_HCNT = 32 + (600 * (I2CSrcClk / 10000) * 4) / (I2C_InitStruct->I2C_ClockSpeed);
            I2Cx->IC_FS_SCL_LCNT = (1300 * (I2CSrcClk / 10000) * 4) / (I2C_InitStruct->I2C_ClockSpeed);
        }
        else if (I2C_InitStruct->I2C_ClockSpeed == 400000)
        {
            /*configure I2C speed*/
            I2Cx->IC_FS_SCL_HCNT = 8 + (600 * (I2CSrcClk / 10000) * 4) / (I2C_InitStruct->I2C_ClockSpeed);
            I2Cx->IC_FS_SCL_LCNT = 1 + (1300 * (I2CSrcClk / 10000) * 4) / (I2C_InitStruct->I2C_ClockSpeed);
        }
        else
        {
            I2Cx->IC_FS_SCL_HCNT = 20 + (600 * (I2CSrcClk / 10000) * 4) / (I2C_InitStruct->I2C_ClockSpeed);
            I2Cx->IC_FS_SCL_LCNT = (1300 * (I2CSrcClk / 10000) * 4) / (I2C_InitStruct->I2C_ClockSpeed);
        }
    }
    /*Configure I2C speed in high mode*/
    else
    {
        if (I2C_InitStruct->I2C_ClockSpeed > 3400000)
        {
            I2C_InitStruct->I2C_ClockSpeed = 3400000;
        }

        I2Cx->IC_CON |= (0x3 << 1);
        /*configure I2C speed*/
        I2Cx->IC_HS_SCL_HCNT = 8 + (60 * (I2CSrcClk / 10000) * 30) / (I2C_InitStruct->I2C_ClockSpeed);
        I2Cx->IC_HS_SCL_LCNT = 1 + (120 * (I2CSrcClk / 10000) * 30) / (I2C_InitStruct->I2C_ClockSpeed);

    }

    /*Config I2C dma mode*/
    I2Cx->IC_DMA_CR = ((I2C_InitStruct->I2C_RxDmaEn)\
                       | ((I2C_InitStruct->I2C_TxDmaEn) << 1));

    /*Config I2C waterlevel*/
    I2Cx->IC_DMA_RDLR = I2C_InitStruct->I2C_RxWaterlevel;
    I2Cx->IC_DMA_TDLR = I2C_InitStruct->I2C_TxWaterlevel;

    I2Cx->IC_INTR_MASK = 0;
}

/**
  * @brief  Deinitializes the I2Cx peripheral registers to their default reset values.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @retval None
  */
void I2C_DeInit(I2C_TypeDef *I2Cx)
{
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));

    /*Disable I2C IP*/
    if (I2Cx == I2C0)
    {
        RCC_PeriphClockCmd(APBPeriph_I2C0, APBPeriph_I2C0_CLOCK, DISABLE);
    }
    else if (I2Cx == I2C1)
    {
        RCC_PeriphClockCmd(APBPeriph_I2C1, APBPeriph_I2C1_CLOCK, DISABLE);
    }
}

/**
  * @brief  Fills each I2C_InitStruct member with its default value.
  * @param  I2C_InitStruct : pointer to a I2C_InitTypeDef structure which will be initialized.
  * @retval None
  */
void I2C_StructInit(I2C_InitTypeDef *I2C_InitStruct)
{
    I2C_InitStruct->I2C_Clock             = 40000000;               /* depend on clock divider */
    I2C_InitStruct->I2C_ClockSpeed        = 400000;
    I2C_InitStruct->I2C_DeviveMode        = I2C_DeviveMode_Master;  /* Master mode */
    I2C_InitStruct->I2C_AddressMode       = I2C_AddressMode_7BIT;   /* 7-bit address mode */
    I2C_InitStruct->I2C_SlaveAddress      = 0;
    I2C_InitStruct->I2C_Ack               = I2C_Ack_Enable;
    I2C_InitStruct->I2C_TxThresholdLevel  = 0x00;                 /* tx fifo depth: 24 * 8bits */
    I2C_InitStruct->I2C_RxThresholdLevel  = 0x00;                 /* rx fifo depth: 40 * 8bits */
    I2C_InitStruct->I2C_TxDmaEn           = DISABLE;
    I2C_InitStruct->I2C_RxDmaEn           = DISABLE;
    I2C_InitStruct->I2C_RxWaterlevel      = 1;                    /* Best to equal GDMA Source MSize */
    I2C_InitStruct->I2C_TxWaterlevel      = 15;                   /* Best to equal Tx fifo minus
                                                                      GDMA Dest MSize */
}


/**
  * @brief  Enables or disables the specified I2C peripheral.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  NewState: new state of the I2Cx peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void I2C_Cmd(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        /* Enable the selected I2C peripheral */
        I2Cx->IC_ENABLE |= 0x0001;
    }
    else
    {
        /* Disable the selected I2C peripheral */
        I2Cx->IC_ENABLE &= ~0x0001;
    }
}


/**
  * @brief  Checks whether the last I2Cx abort status.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @retval the status of I2Cx.
  */
I2C_Status I2C_CheckAbortStatus(I2C_TypeDef *I2Cx)
{
    uint32_t abort_status = 0;

    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));

    /* Get abort status */
    abort_status = I2Cx->IC_TX_ABRT_SOURCE;

    if (abort_status & MS_ALL_ABORT)
    {
        /* Clear abort status */
        (void)I2Cx->IC_CLR_TX_ABRT;

        /* Check abort type */
        if (abort_status & ABRT_TXDATA_NOACK)
        {
            return I2C_ABRT_TXDATA_NOACK;
        }

        if (abort_status & ABRT_7B_ADDR_NOACK)
        {
            return I2C_ABRT_7B_ADDR_NOACK;
        }

        if (abort_status & ARB_LOST)
        {
            return I2C_ARB_LOST;
        }

        if (abort_status & ABRT_MASTER_DIS)
        {
            return I2C_ABRT_MASTER_DIS;
        }

        if (abort_status & ABRT_10ADDR1_NOACK)
        {
            return I2C_ABRT_10ADDR1_NOACK;
        }

        if (abort_status & ABRT_10ADDR2_NOACK)
        {
            return I2C_ABRT_10ADDR2_NOACK;
        }
    }

    return I2C_Success;
}

/**
  * @brief  Send data in master mode through the I2Cx peripheral.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  pBuf: bytes to be transmitted.
  * @param  len: length of bytes to be tranamitted.
  * @retval the status of I2Cx.
  */
I2C_Status I2C_MasterWrite(I2C_TypeDef *I2Cx, uint8_t *pBuf, uint16_t len)
{
    uint16_t cnt = 0;
    uint32_t time_out = I2C_TimeOut;
    I2C_Status abort_status = I2C_Success;

    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));

    /* Write in the DR register the data to be sent */
    for (cnt = 0; cnt < len; cnt++)
    {
        if (cnt >= len - 1)
        {
            /*generate stop signal*/
            I2Cx->IC_DATA_CMD = (*pBuf++) | (1 << 9);
        }
        else
        {
            I2Cx->IC_DATA_CMD = *pBuf++;
        }

        /* wait for flag of I2C_FLAG_TFNF */
        time_out = I2C_TimeOut;
        while (((I2Cx->IC_STATUS & (1 << 1)) == 0) && (time_out != 0))
        {
            /* Check abort status */
            abort_status = I2C_CheckAbortStatus(I2Cx);
            if (abort_status != I2C_Success)
            {
                return abort_status;
            }

            time_out--;
            if (time_out == 0)
            {
                return I2C_ERR_TIMEOUT;
            }
        }

        /* Check abort status */
        abort_status = I2C_CheckAbortStatus(I2Cx);
        if (abort_status != I2C_Success)
        {
            return abort_status;
        }
    }

    return abort_status;
}

/**
  * @brief  Read data in master mode through the I2Cx peripheral.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  pBuf: bytes to be received.
  * @param  len: length of bytes to be receiveed.
  * @retval the status of I2Cx.
  */
I2C_Status I2C_MasterRead(I2C_TypeDef *I2Cx, uint8_t *pBuf, uint16_t len)
{
    uint16_t cnt = 0;
    uint32_t reg_value = 0;
    uint32_t time_out = I2C_TimeOut;
    I2C_Status abort_status = I2C_Success;

    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));

    /* read in the DR register the data to be sent */
    for (cnt = 0; cnt < len; cnt++)
    {
        if (cnt >= len - 1)
        {
            /* generate stop singal */
            I2Cx->IC_DATA_CMD = (reg_value) | (0x0003 << 8);
        }
        else
        {
            I2Cx->IC_DATA_CMD = (reg_value) | (0x0001 << 8);
        }

        /* read data */
        if (cnt > 0)
        {
            /* wait for I2C_FLAG_RFNE flag */
            time_out = I2C_TimeOut;
            while (((I2Cx->IC_STATUS & (1 << 3)) == 0) && (time_out != 0))
            {
                /* Check abort status */
                abort_status = I2C_CheckAbortStatus(I2Cx);
                if (abort_status != I2C_Success)
                {
                    return abort_status;
                }

                time_out--;
                if (time_out == 0)
                {
                    return I2C_ERR_TIMEOUT;
                }
            }

            *pBuf++ = (uint8_t)I2Cx->IC_DATA_CMD;
        }
    }

    /* wait for I2C_FLAG_RFNE flag */
    time_out = I2C_TimeOut;
    while (((I2Cx->IC_STATUS & (1 << 3)) == 0) && (time_out != 0))
    {
        /* Check abort status */
        abort_status = I2C_CheckAbortStatus(I2Cx);
        if (abort_status != I2C_Success)
        {
            return abort_status;
        }

        time_out--;
        if (time_out == 0)
        {
            return I2C_ERR_TIMEOUT;
        }
    }

    *pBuf = (uint8_t)I2Cx->IC_DATA_CMD;

    return abort_status;
}

/**
  * @brief  Sends data and read data in master mode through the I2Cx peripheral.Attention:Read data with time out mechanism.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  pWriteBuf: byte to be transmitted.
  * @param  Writelen: length of bytes to be tranamitted.
  * @param  pReadBuf: byte to be received.
  * @param  Readlen: length of bytes to be receiveed.
  * @retval the status of I2Cx.
  */
I2C_Status I2C_RepeatRead(I2C_TypeDef *I2Cx, uint8_t *pWriteBuf, uint16_t Writelen,
                          uint8_t *pReadBuf, uint16_t Readlen)
{
    uint16_t cnt = 0;
    uint32_t reg_value = 0;
    uint32_t time_out = I2C_TimeOut;
    I2C_Status abort_status = I2C_Success;

    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));

    /*------------------------------ write data section ------------------------------*/
    /* write data in the IC_DATA_CMD register */
    for (cnt = 0; cnt < Writelen; cnt++)
    {
        I2Cx->IC_DATA_CMD = *pWriteBuf++;

        /*wait for I2C_FLAG_TFNF flag that Tx FIFO is not full*/
        time_out = I2C_TimeOut;
        while (((I2Cx->IC_STATUS & BIT(1)) == 0) && (time_out != 0))
        {
            /* Check abort status */
            abort_status = I2C_CheckAbortStatus(I2Cx);
            if (abort_status != I2C_Success)
            {
                return abort_status;
            }

            time_out--;
            if (time_out == 0)
            {
                return I2C_ERR_TIMEOUT;
            }
        }

        /* Check abort status */
        abort_status = I2C_CheckAbortStatus(I2Cx);
        if (abort_status != I2C_Success)
        {
            return abort_status;
        }
    }

    /*------------------------------ read data section ------------------------------*/
    for (cnt = 0; cnt < Readlen; cnt++)
    {
        if (cnt >= Readlen - 1)
        {
            /*generate stop singal in last byte which to be sent*/
            I2Cx->IC_DATA_CMD = reg_value | BIT(8) | BIT(9);
        }
        else
        {
            I2Cx->IC_DATA_CMD = reg_value | BIT(8);
        }

        /*read data */
        if (cnt > 0)
        {
            /*wait for I2C_FLAG_RFNE flag*/
            time_out = I2C_TimeOut;
            while (((I2Cx->IC_STATUS & BIT(3)) == 0) && (time_out != 0))
            {
                /* Check abort status */
                abort_status = I2C_CheckAbortStatus(I2Cx);
                if (abort_status != I2C_Success)
                {
                    return abort_status;
                }

                time_out--;
                if (time_out == 0)
                {
                    return I2C_ERR_TIMEOUT;
                }
            }

            *pReadBuf++ = (uint8_t)I2Cx->IC_DATA_CMD;
        }
    }

    /*read data*/
    time_out = I2C_TimeOut;
    while (((I2Cx->IC_STATUS & BIT(3)) == 0) && (time_out != 0))
    {
        /* Check abort status */
        abort_status = I2C_CheckAbortStatus(I2Cx);
        if (abort_status != I2C_Success)
        {
            return abort_status;
        }

        time_out--;
        if (time_out == 0)
        {
            return I2C_ERR_TIMEOUT;
        }
    }

    *pReadBuf = (uint8_t)I2Cx->IC_DATA_CMD;

    return abort_status;
}

/**
  * @brief mask the specified I2C interrupt.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  I2C_INT
  * This parameter can be one of the following values:
  *     @arg I2C_INT_GEN_CALL: When a General Call address is received and it is acknowledged.
  *     @arg I2C_INT_START_DET: When a START or RESTART condition has occurred on the I2C interface
  *                            regardless of whether I2C is operating in slave or master mode.
  *     @arg I2C_INT_STOP_DET: When a STOP condition has occurred on the I2C interface
  *                            regardless of whether I2C is operating in slave or master mode.
  *     @arg I2C_INT_ACTIVITY: When I2C is activity on the bus. Stays set until it is cleared.
  *     @arg I2C_INT_RX_DONE: When the I2C is acting as a slave-transmitter and the master does
  *                           not acknowledge a transmitted byte.
  *                           This occurs on the last byte of the transmission,
  *                           indicating that the transmission is done.
  *     @arg I2C_INT_TX_ABRT: When an I2C transmitter is unable to complete the intended actions
  *                           on the contents of the transmit FIFO.
  *     @arg I2C_INT_RD_REQ:  When I2C is acting as a slave and another I2C master is attempting to read data from I2C.
  *     @arg I2C_INT_TX_EMPTY: When the transmit buffer is at or below the threshold value set in the REG_IC_TXFLR register.
  *     @arg I2C_INT_TX_OVER: When transmit buffer is filled to IC_TX_BUFFER_DEPTH and
  *                           the processor attempts to issue another I2C command by writing to the REG_IC_DATA_CMD register.
  *     @arg I2C_INT_RX_FULL: When the receive buffer reaches or goes above the RX_TL threshold in the REG_IC_RX_TL register.
  *                           A value of 0 sets the threshold for 1 entry, and a value of 255 sets the threshold for 256 entries.
  *     @arg I2C_INT_RX_OVER: When the receive buffer is completely filled to IC_RX_BUFFER_DEPTH and
  *                           an additional byte is received from an external I2C device.
  *     @arg I2C_INT_RX_UNDER: When the processor attempts to read the receive buffer
  *                            when it is empty by reading from the REG_IC_DATA_CMD register.
  * @retval None.
  */
void I2C_INTConfig(I2C_TypeDef *I2Cx, uint16_t I2C_INT, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));
    assert_param(I2C_GET_INT(I2C_INT));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        /* Enable the selected I2C interrupts */
        I2Cx->IC_INTR_MASK |= I2C_INT;
    }
    else
    {
        /* Disable the selected I2C interrupts */
        I2Cx->IC_INTR_MASK &= (uint16_t)~I2C_INT;
    }
}

/**
  * @brief Clear the specified I2C interrupt.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  I2C_INT
  * This parameter can be one of the following values:
  *     @arg I2C_INT_GEN_CALL: When a General Call address is received and it is acknowledged.
  *     @arg I2C_INT_START_DET: When a START or RESTART condition has occurred on the I2C interface
  *                            regardless of whether I2C is operating in slave or master mode.
  *     @arg I2C_INT_STOP_DET: When a STOP condition has occurred on the I2C interface
  *                            regardless of whether I2C is operating in slave or master mode.
  *     @arg I2C_INT_ACTIVITY: When I2C is activity on the bus. Stays set until it is cleared.
  *     @arg I2C_INT_RX_DONE: When the I2C is acting as a slave-transmitter and the master does
  *                           not acknowledge a transmitted byte.
  *                           This occurs on the last byte of the transmission,
  *                           indicating that the transmission is done.
  *     @arg I2C_INT_TX_ABRT: When an I2C transmitter is unable to complete the intended actions
  *                           on the contents of the transmit FIFO.
  *     @arg I2C_INT_RD_REQ:  When I2C is acting as a slave and another I2C master is attempting to read data from I2C.
  *     @arg I2C_INT_TX_EMPTY: When the transmit buffer is at or below the threshold value set in the REG_IC_TXFLR register.
  *     @arg I2C_INT_TX_OVER: When transmit buffer is filled to IC_TX_BUFFER_DEPTH and
  *                           the processor attempts to issue another I2C command by writing to the REG_IC_DATA_CMD register.
  *     @arg I2C_INT_RX_FULL: When the receive buffer reaches or goes above the RX_TL threshold in the REG_IC_RX_TL register.
  *                           A value of 0 sets the threshold for 1 entry, and a value of 255 sets the threshold for 256 entries.
  *     @arg I2C_INT_RX_OVER: When the receive buffer is completely filled to IC_RX_BUFFER_DEPTH and
  *                           an additional byte is received from an external I2C device.
  *     @arg I2C_INT_RX_UNDER: When the processor attempts to read the receive buffer
  *                            when it is empty by reading from the REG_IC_DATA_CMD register.
  * @retval None.
  */
void I2C_ClearINTPendingBit(I2C_TypeDef *I2Cx, uint16_t I2C_IT)
{
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));
    assert_param(I2C_GET_INT(I2C_IT));

    switch (I2C_IT)
    {
    case I2C_INT_RX_UNDER:
        {
            (void)I2Cx->IC_CLR_RX_UNDER;
            break;
        }
    case I2C_INT_RX_OVER:
        {
            (void)I2Cx->IC_CLR_RX_OVER;
            break;
        }
    case I2C_INT_TX_OVER:
        {
            (void)I2Cx->IC_CLR_TX_OVER;
            break;
        }
    case I2C_INT_RD_REQ:
        {
            (void)I2Cx->IC_CLR_RD_REQ;
            break;
        }
    case I2C_INT_TX_ABRT:
        {
            (void)I2Cx->IC_CLR_TX_ABRT;
            break;
        }
    case I2C_INT_RX_DONE:
        {
            (void)I2Cx->IC_CLR_RX_DONE;
            break;
        }
    case I2C_INT_ACTIVITY:
        {
            (void)I2Cx->IC_CLR_ACTIVITY;
            break;
        }
    case I2C_INT_STOP_DET:
        {
            (void)I2Cx->IC_CLR_STOP_DET;
            break;
        }
    case I2C_INT_START_DET:
        {
            (void)I2Cx->IC_CLR_START_DET;
            break;
        }
    case I2C_INT_GEN_CALL:
        {
            (void)I2Cx->IC_CLR_GEN_CALL;
            break;
        }
    default:
        {
            break;
        }
    }
}

void i2c_tempdemo(void)
{
	while(!begin_bmp())
		{
				DBG_DIRECT("Failed to connect to BMP80");
				//os_delay(10);
		}
		
		//while(true)	
		for(int i = 0; i < 100; i++)
		{
			float temp = readTemperature();
			DBG_DIRECT("Temperature read: %f", temp);
		}

}

/******************* (C) COPYRIGHT 2015 Realtek Semiconductor Corporation *****END OF FILE****/

