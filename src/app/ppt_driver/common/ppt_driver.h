/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.

*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*****************************************************************************************
  * @file     ppt_driver.h
  * @brief    Head file for 2.4G module common driver.
  * @details  Data structs and external functions declaration.
  * @author   Bill
  * @date     2025-12-1
  * @version  v1.0
  ***************************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _PPT_DRIVER_H_
#define _PPT_DRIVER_H_

/* Add Includes here */
#include <stdbool.h>
#include <stdint.h>
#include "app_section.h"
//#include "DataType.h"
#include "ppt_hw_reg.h"
#include "ll_common.h"

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/** @addtogroup PPT_Driver
  * @{
  */

/** @defgroup PPT_Driver_Exported_Macros Exported Macros
  * @brief
  * @{
  */

/** @defgroup PPT_SECTION       Section
  * @brief Control the memory location of the function.
  * The processing can speed up if the code is in RAM region.
  * @{
  */
#define PPT_ISR_SECTION             DATA_RAM_FUNCTION
#define PPT_API_SECTION             DATA_RAM_FUNCTION
#define PPT_DAT_SECTION             //const

/** @} */

/** @defgroup PPT_Hardware_Channels     Hardware Channels
  * @brief Hardware trx channel related macros.
  * @{
  */
/** @brief Total channels number */
#define PPT_ENTRY_NUM               8
/** @brief Channels number for old design */
#define PPT_ENTRY_NUM_ORI           3
/** @brief The maximum rx channels */
#define PPT_ENTRY_RX_NUM            8
/** @brief Rx channels number for old design */
#define PPT_ENTRY_RX_NUM_ORI        2
/** @} */

/** @defgroup PPT_HW_MISC  Hardware Misc
  * @brief Hardware miscellaneous definition.
  * @{
  */
#define PPT_ADDR_LEN_MAX            5
#define PPT_TX_FIFO_SIZE            8

#define PRO_ONE_ENTRY_DW_SIZE               0x20
#define PRO_ENTRY_FIELD_BASE(entry)         ((entry) * PRO_ONE_ENTRY_DW_SIZE)
#define PRO_ONE_PREFIX_DW_SIZE              0x2
#define PRO_PREFIX_BASE(entry)              ((entry) * PRO_ONE_PREFIX_DW_SIZE)

#define PPT_ISR_NUMBER              7
#define PPT_ISR_VALID_BITS_MASK     0x7F
/** @} */

/** @defgroup PPT_REG_ACCESS     Register Access
  * @brief Macros used to access radio registers
  * @{
  */

/**
  * @brief Read radio register
  * @param[in] offset: the register address
  * @return the register value
  */
#define RD_PPT_REG(offset) ppt_read_reg(offset)

/**
  * @brief Write radio register
  * @param[in] offset: the register address
  * @param[in] data: the register value
  */
#define WR_PPT_REG(offset, data) ppt_write_reg(offset, data)

/**
  * @brief Update radio register
  * @param[in] offset: the register address
  * @param[in] mask: the register mask
  * @param[in] data: the register value
  */
#define UP_PPT_REG(offset, mask, data) do\
    {\
        uint16_t reg = RD_PPT_REG(offset);\
        reg &= ~(mask);\
        reg |= (data) & (mask);\
        WR_PPT_REG(offset, reg);\
    } while(0)

/**
  * @brief Read radio register by specific field
  * @param[in] name: the register name
  * @param[in] field: the specific field
  * @return the field value
  */
#define RD_PPT_REG_FIELD(name, field) ({name##_TYPE reg = {.d16 = RD_PPT_REG(name)}; reg.field;})

/**
  * @brief Write radio register by specific field
  * @param[in] name: the register name
  * @param[in] field: the specific field
  * @param[in] data: the field value
  * @return the field value
  */
#define WR_PPT_REG_FIELD(name, field, data) do\
    {\
        name##_TYPE reg = {.d16 = RD_PPT_REG(name)};\
        reg.field = data;\
        WR_PPT_REG(name, reg.d16);\
    } while (0)

/**
  * @brief Update radio register by specific field
  * @param[in] name: the register address
  * @param[in] field: the specific field
  * @param[in] mask: the field mask
  * @param[in] data: the field value
  */
#define UP_PPT_REG_FIELD(name, field, mask, data) do\
    {\
        name##_TYPE reg = {.d16 = RD_PPT_REG(name)};\
        reg.field &= ~(mask);\
        reg.field |= (data) & (mask);\
        WR_PPT_REG(name, reg.d16);\
    } while(0)

/**
  * @brief Overwrite radio field value
  * @param[in] offset: the register address
  * @param[in] field_offset: the field bit offset
  * @param[in] field_len: the field bit length
  * @param[in] data: the field value
  */
#define OV_PPT_REG_FIELD(offset, field_offset, field_len, data) do\
    {\
        uint16_t reg = RD_PPT_REG(offset);\
        reg &= ~(((0xffff >> (16 - (field_len)))) << (field_offset));\
        reg |= (data) << (field_offset);\
        WR_PPT_REG(offset, reg);\
    } while(0)
/** @} */

/** @defgroup PPT_SW_MISC  Software Misc
  * @brief Software miscellaneous definition.
  * @{
  */

/**
  * @brief Obtain the register offset of the specific entry
  * @param[in] entry: the entry of parameter
  * @param[in] offset0: the register offset0
  * @param[in] size0: the entry step size0
  * @param[in] offset1: the register offset1
  * @param[in] size1: the entry step size1
  * @return register offset
  */
#define PPT_REG_OFFSET(entry, offset0, size0, offset1, size1) (((entry) < PPT_ENTRY_NUM_ORI) ? ((size0)*(entry) + (offset0)) : ((size1) * ((entry) - PPT_ENTRY_NUM_ORI) + (offset1)))

/**
  * @brief Obtain the tx entry in the tx stack information
  * @param[in] tx_stack: the @ref PRO_TX_STACK_TYPE register
  * @param[in] tx_len: the @ref PRO_TX_LENGTH_TYPE register
  */
#define PPT_TX_STACK_ENTRY(tx_stack, tx_len) (tx_stack.tx_entry_1_0 + (tx_len.tx_entry_2 << 2))

/**
  * @brief Obtain the rx entry in the rx stack information
  * @param[in] rx_stack: the @ref PRO_RX_STACK_TYPE register
  */
#define PPT_RX_STACK_ENTRY(rx_stack) (rx_stack.entry_0 + (rx_stack.entry_2_1 << 1))

/**
  * @brief Obtain the header size pushed to the Rx FIFO
  *
  * The Rx FIFO is filled with the header and the payload of a packet.
  * The header contains the header prefix, length and header suffix.
  * The total size will be aligned to 16 bits when filling to the Rx FIFO.
  * @param[in] hp_size: header prefix size in unit of bit
  * @param[in] length_size: length size in unit of bit
  * @param[in] hs_size: header suffix size in unit of bit
  * @return header size in unit of byte
  */
#define PPT_RX_FIFO_HEADER_SIZE(hp_size, length_size, hs_size) ((((hp_size) + (length_size) + (hs_size) + 15) >> 4) << 1)

/**
  * @brief Obtain the header size pushed to the Rx FIFO
  *
  * The Rx FIFO is filled with the header and the payload of a packet.
  * The header contains the header prefix, length and header suffix.
  * The total size will be aligned to 16 bits when filling to the Rx FIFO.
  * @param[in] hp_size: header prefix size in unit of byte
  * @param[in] length_size: length size in unit of byte
  * @param[in] hs_size: header suffix size in unit of byte
  * @return header size in unit of byte
  */
#define PPT_RX_FIFO_HEADER_SIZE_BYTE(hp_size, length_size, hs_size) PPT_RX_FIFO_HEADER_SIZE(hp_size << 3, length_size << 3, hs_size << 3)

/**
  * @brief Enable radio interrupt
  */
#define PPT_ISR_ENABLE()        NVIC_EnableIRQ(BTMAC_IRQn)

/**
  * @brief Disable radio interrupt
  */
#define PPT_ISR_DISABLE()       NVIC_DisableIRQ(BTMAC_IRQn)

/** @} */

/** @} End of PPT_Driver_Exported_Macros */

/** @defgroup PPT_Driver_Exported_Types Exported Types
  * @brief
  * @{
  */

/** @brief Hw supported instructions */
typedef enum
{
    PPT_HW_INSTRUCTION_PTX_ENABLE           = 0,
    PPT_HW_INSTRUCTION_PRX_ENABLE           = 1,
    PPT_HW_INSTRUCTION_PTX_DISABLE          = 2,
    PPT_HW_INSTRUCTION_PRX_DISABLE          = 3,
    PPT_HW_INSTRUCTION_PSD_ENABLE           = 4,
    PPT_HW_INSTRUCTION_PSD_DISABLE          = 5,
    PPT_HW_INSTRUCTION_MAX
} ppt_hw_instruction_t;

/** @brief Hw supported states */
typedef enum
{
    PPT_FSM_STANDBY,
    PPT_FSM_PTX,
    PPT_FSM_PRX,
    PPT_FSM_PSD
} ppt_fsm_t;

/** @brief The bit endianess */
typedef enum
{
    PPT_FIELD_BIT_ORDER_LSB,
    PPT_FIELD_BIT_ORDER_MSB
} ppt_field_bit_order_t;

/** @brief The packet format control parameters */
typedef struct
{
    uint8_t addr_len; //!< byte
    uint8_t hp_len; //!< bit
    uint8_t length_len; //!< bit
    uint8_t hs_len; //!< bit
    ppt_field_bit_order_t header_order;
    ppt_field_bit_order_t payload_order;
} ppt_pkt_format_t;

/** @brief The addon field length parameters */
typedef struct
{
    uint8_t len;
} ppt_addon_length_t;

/** @brief The crc formula parameters */
typedef struct
{
    uint32_t poly;
    uint32_t init;
} ppt_crc_value_t;

/** @brief The crc length parameters */
typedef struct
{
    uint8_t len; //!< byte
    bool include_addr; //!< not support feature
} ppt_crc_param_t;

/** @brief Crc parameters of each channel */
typedef struct
{
    ppt_crc_value_t value;
} ppt_crc_entry_param_t;

/** @brief The whitening formula parameters */
typedef struct
{
    uint32_t poly;
    uint32_t init;
} ppt_white_value_t;

/** @brief The whitening switch */
typedef struct
{
    bool enable;
} ppt_white_param_t;

/** @brief The whitening parameters of each channel */
typedef struct
{
    uint8_t len; //!< bit
    ppt_white_value_t value;
} ppt_white_entry_param_t;

/** @brief The supported PHY */
typedef enum
{
    PPT_PHY_TYPE_BLE_1M,
    PPT_PHY_TYPE_BLE_2M
} ppt_phy_type_t;

/** @brief The PHY common parameters */
typedef struct
{
    uint8_t bank;
    uint8_t channel;
    ppt_phy_type_t rx_phy;
} ppt_phy_param_t;

/** @brief The PHY special parameters of each channel */
typedef struct
{
    uint8_t preamble_len;
    ppt_phy_type_t tx_phy;
} ppt_phy_entry_param_t;

/** @brief Rf tx gain */
typedef enum
{
    PPT_TX_GAIN_8DBM = 0xe0,
    PPT_TX_GAIN_7P5DBM = 0xe0,
    PPT_TX_GAIN_4DBM = 0xb0,
    PPT_TX_GAIN_3DBM = 0xa0,
    PPT_TX_GAIN_0DBM = 0x70,
    PPT_TX_GAIN_N4DBM = 0x50,
    PPT_TX_GAIN_N8DBM = 0x40,
    PPT_TX_GAIN_N10DBM = 0x30,
    PPT_TX_GAIN_N20DBM = 0x10
} ppt_tx_gain_t;

/** @brief The tx access address of each channel */
typedef struct
{
    uint8_t tx_addr[PPT_ADDR_LEN_MAX];
} ppt_tx_addr_t;

/** @brief The rx access address of each channel */
typedef struct
{
    bool enable;
    uint8_t rx_addr[PPT_ADDR_LEN_MAX];
} ppt_rx_addr_t;

/** @brief The header fields of frame of each channel */
typedef struct
{
    uint8_t hp;
    uint16_t length; //!< this field set by DMA automatically
    uint32_t hs;
} ppt_header_t;

/** @brief The ptx mode parameters */
typedef struct
{
    bool ack_mode;
    bool periodic_mode;
    uint32_t periodic_interval; //!< 20 valid bits, units of 125us, (n+1)*125us
} ppt_ptx_mode_t;

/** @brief The prx mode parameters */
typedef struct
{
    bool ack_mode;
    bool continuous_mode;
} ppt_prx_mode_t;

/** @brief The psd mode parameters */
typedef struct
{
    uint8_t chann_start;
    uint8_t chann_stop;
    uint8_t chann_step;
    uint8_t mode; //!< useless
    uint16_t timeout; //!< value @ref PSD_TIMEOUT_DEFAULT is recommended
} ppt_psd_mode_t;

/** @brief The timing latch parameters */
typedef struct
{
    bool tx_on;
    bool acc_hit;
} ppt_timing_latch_t;

/** @brief The GPIO trigger parameters of each channel */
typedef struct
{
    uint16_t gpio_delay;
} ppt_gpio_entry_param_t;

/** @brief The GPIO trigger common parameters */
typedef struct
{
    bool enable;
    bool int_enable;
} ppt_gpio_param_t;

/** @brief The crc engine common parameters */
typedef struct
{
    uint32_t poly;
    uint32_t init;
    uint8_t length; //!< length in bits
    uint8_t input_endian : 1; //!< 0: lsb, 1: msb
    uint8_t output_endian : 1; //!< 0: lsb, 1: msb
    uint8_t bit_mode : 1; //!< 0: byte, 1: bit
} ppt_crc_eng_param_t;

/** @brief Hw register field descriptor */
typedef struct
{
    uint16_t reg_offset;
    uint8_t field_offset;
} ppt_reg_field_t;

/** @brief Flag to tell driver whether it can sleep */
extern volatile bool ppt_dlps_mac_idle;

/** @} End of PPT_Driver_Exported_Types */

/** @defgroup PPT_Driver_Exported_Functions Exported Functions
  * @brief
  * @{
  */

/**
  * @brief Read radio register
  * @param[in] offset: the register address
  * @return the register value
  */
extern uint16_t (*ppt_read_reg)(uint16_t offset);

/**
  * @brief Write radio register
  * @param[in] offset: the register address
  * @param[in] data: the register value
  */
extern void (*ppt_write_reg)(uint16_t offset, uint16_t data);

/**
  * @brief Swap bit order of one byte data
  * @param[in] inp: the data to be swapped
  * @return the new value after swapped bit order
  */
uint8_t ppt_swap_bits8(uint8_t inp);

/**
  * @brief Swap bit order of two bytes data
  * @param[in] inp: the data to be swapped
  * @return the new value after swapped bit order
  */
uint16_t ppt_swap_bits16(uint16_t inp);

/**
  * @brief Swap bit order of three bytes data
  * @param[in] inp: the data to be swapped
  * @return the new value after swapped bit order
  */
uint32_t ppt_swap_bits24(uint32_t inp);

/**
  * @brief Swap bit order of four bytes data
  * @param[in] inp: the data to be swapped
  * @return the new value after swapped bit order
  */
uint32_t ppt_swap_bits32(uint32_t inp);

/**
  * @brief Initialize the DMA of radio
  */
void ppt_init_dma(void);

/**
  * @brief Workaround for reset when bzdma start which results in dma carry error later.
  * @deprecated hardware has fixed this issue.
  */
#define ppt_clear_dma_tx_done()

/**
  * @brief Deinitialize the DMA of radio
  */
void ppt_deinit_dma(void);

/**
  * @brief Initialize the miscellaneous functions
  */
void ppt_init_misc(void);

/**
  * @brief Deinitialize the miscellaneous functions
  */
void ppt_deinit_misc(void);

/**
  * @brief Reset trx packet number counter of hardware
  */
void ppt_reset_trx_pkt_num(void);

/**
  * @brief Enable the clock of radio
  * @param[in] ppt_mode: true for 2.4G, false for ble
  */
void ppt_reset_core_domain(bool ppt_mode);

/**
  * @brief Execute instruction to switch the radio state
  * @param[in] instruction_code: instruction opcode @ref ppt_hw_instruction_t
  */
void ppt_execute_instruction(uint8_t instruction_code);

/**
  * @brief Clear radio state
  * @deprecated not used
  */
#define ppt_clear_radio()

/**
  * @brief Clear radio state
  * @deprecated not used
  */
#define ppt_clear_radio_quick(...)

/**
  * @brief Lock/unlock the header fields value
  *
  * Since the header has multiple fields, they can be written in an atomic way after lock.
  * @param[in] entry: channel index
  * @param[in] lock: true for lock, false for unlock
  */
void ppt_lock(uint8_t entry, bool lock);

/**
  * @brief Set whether to hold tx automatically
  * @param[in] entry: channel index
  * @param[in] enable: feature value
  */
void ppt_set_auto_no_tx(uint8_t entry, bool enable);

/**
  * @brief Clear hardware holding tx once
  * @param[in] entry: channel index
  */
void ppt_clear_hw_no_tx(uint8_t entry);

/**
  * @brief Set the feature of hardware auto ack the transmitted packet
  * @param[in] entry: channel index
  * @param[in] enable: feature value
  */
void ppt_set_auto_ack(uint8_t entry, bool enable);

/**
  * @brief Firmware ack the transmitted packet once with lock flag
  * @param[in] entry: channel index
  * @param[in] lock: lock the header and tx fifo
  * @return tx fifo read pointer
  */
uint8_t ppt_trigger_fw_ack_lock(uint8_t entry, bool lock);

/**
  * @brief Firmware ack the transmitted packet once
  * @param[in] entry: channel index
  * @return tx fifo read pointer
  */
#define  ppt_trigger_fw_ack(entry) ppt_trigger_fw_ack_lock(entry, true)

/**
  * @brief Firmware force ack the first packet
  *
  * PRX role may use this API to quit data pushed previously.
  * @param[in] entry: channel index
  * @return TX FIFO read pointer
  */
uint8_t ppt_force_fw_ack(uint8_t entry);

/**
  * @brief Register the interrupt handler
  * @param[in] handler: the function pointer of ISR
  */
void ppt_reg_handler(void (*handler)(void));

/**
  * @brief Register the PSD interrupt handler
  * @param[in] handler: the function pointer of ISR
  */
void ppt_reg_psd_handler(void (*handler)(void));

/**
  * @brief Set CRC common parameters
  *
  * @note CRC including address field is not supported.
  * It can be as a workaround to replace the CRC init by a new value which is calculated by CRC check the address with the CRC init.
  * @param[in] param: parameters
  */
void ppt_set_crc_param(ppt_crc_param_t *param);

/**
  * @brief Set CRC channel specific parameters
  * @param[in] entry: channel index
  * @param[in] param: parameters
  */
void ppt_set_crc_entry_param(uint8_t entry, ppt_crc_entry_param_t *param);

/**
  * @brief Set whitening common parameters
  * @param[in] param: parameters
  */
void ppt_set_white_param(ppt_white_param_t *param);

/**
  * @brief Set whitening channel specific parameters
  * @param[in] entry: channel index
  * @param[in] param: parameters
  */
void ppt_set_white_entry_param(uint8_t entry, ppt_white_entry_param_t *param);

/**
  * @brief Set RX PHY
  * @param[in] rx_phy: PHY type
  */
void ppt_set_phy_rx_type(ppt_phy_type_t rx_phy);

/**
  * @brief Set TX PHY
  * @param[in] entry: channel index
  * @param[in] tx_phy: PHY type
  */
void ppt_set_phy_tx_type(uint8_t entry, ppt_phy_type_t tx_phy);

/**
  * @brief Set PHY common parameters
  * @param[in] param: parameters
  */
void ppt_set_phy_param(ppt_phy_param_t *param);

/**
  * @brief Set modulation index of 2M phy
  * @param[in] change: change default value
  */
void ppt_set_phy_mod_index_2m(bool change);

/**
  * @brief Set phy parameters for different rf bank
  *
  * Currently modulation tx parameters are updated by this function, so only tx needs these settings.
  *
  * @param[in] bank_idx: 0 for 2402~2480MHz, 1 for 2348~2426MHz, 2 for 2452~2530MHz
  */
void ppt_set_phy_bank(uint8_t bank_idx);

/**
  * @brief Set phy channel
  *
  * It will call ppt_set_phy_bank to update phy parameters at the same time.
  *
  * @param[in] freq_mhz: 2348~2530MHz
  * @return set result
  */
bool ppt_set_phy_channel(uint16_t freq_mhz);

/**
  * @brief Set phy channel specific parameters
  * @param[in] entry: channel index
  * @param[in] param: parameters
  */
void ppt_set_phy_entry_param(uint8_t entry, ppt_phy_entry_param_t *param);

/**
  * @brief Set packet format parameters
  * @param[in] param: parameters
  */
void ppt_set_pkt_format(ppt_pkt_format_t *param);

/**
  * @brief Set preamble length
  * @param[in] entry: channel index
  * @param[in] preamble_len: length in unit of byte
  */
void ppt_set_preamble_len(uint8_t entry, uint8_t preamble_len);

/**
  * @brief Set addon field parameters
  * @param[in] entry: channel index
  * @param[in] param: parameters
  */
void ppt_set_addon_len(uint8_t entry, ppt_addon_length_t *param);

/**
  * @brief Set rf tx power
  * @param[in] tx_gain: tx gain value @ref ppt_tx_gain_t
  */
void ppt_set_tx_power(uint8_t tx_gain);

/**
  * @brief Set rf tx power
  * @param[in] tx_power_dbm: tx power in unit of dbm
  */
void ppt_set_tx_power_dbm(float tx_power_dbm);

/**
  * @brief Set tx access address
  * @param[in] entry: channel index
  * @param[in] param: parameters
  */
void ppt_set_tx_addr(uint8_t entry, ppt_tx_addr_t *param);

/**
  * @brief Set rx access address
  *
  * The caller shall check whether any address value is 0xff or two rx address values are similar.
  * The access address shall be chaotic as the definition in BT specification.
  * If not, the radio parameter needs to be changed to receive the packets successfully.
  * But this will decrease the rx sensitivity.
  *
  * The radio parameter can be updated by @ref ppt_check_rx_addr.
  * @param[in] entry: channel index
  * @param[in] param: parameters
  */
void ppt_set_rx_addr(uint8_t entry, ppt_rx_addr_t *param);

/**
  * @brief Check rx access address and set radio parameters
  *
  * If any address value is 0xff or two rx address values are similar, this function will set the radio parameters.
  * Since the parameters are set according to the chosen rx PHY, this function shall be called
  * after both the address and rx PHY are set.
  * @param[in] bit_th: bit difference threshold, recommend to use 5
  */
void ppt_check_rx_addr(uint8_t bit_th);

/**
  * @brief Set ptx modes
  * @param[in] param: parameters
  */
void ppt_set_ptx_mode(ppt_ptx_mode_t *param);

/**
  * @brief Set prx modes
  * @param[in] param: parameters
  */
void ppt_set_prx_mode(ppt_prx_mode_t *param);

/**
  * @brief Set psd modes
  * @param[in] param: psd parameters
  */
void ppt_set_psd_mode(ppt_psd_mode_t *param);

/**
  * @brief Set tx header prefix field
  * @param[in] entry: channel index
  * @param[in] hp: header prefix
  */
void ppt_set_hp(uint8_t entry, uint8_t hp);

/**
  * @brief Set tx header length field
  *
  * The value of the length field indicates the octet length of the payload.
  * But this value is useless if dma_length_mode is enabled (which is default on) which uses dma length instead.
  * @param[in] entry: channel index
  * @param[in] length: payload length
  */
void ppt_set_length(uint8_t entry, uint16_t length);

/**
  * @brief Set tx header suffix field
  * @param[in] entry: channel index
  * @param[in] hs: header suffix
  */
void ppt_set_hs(uint8_t entry, uint32_t hs);

/**
  * @brief Set tx header field value
  * @param[in] entry: channel index
  * @param[in] param: parameters
  */
void ppt_set_tx_header(uint8_t entry, ppt_header_t *param);

/**
  * @brief Set rf turn around time
  *
  * If not set, the default value is 150us.
  * @param[in] tifs_us: tifs value in microseconds
  */
void ppt_set_tifs(uint8_t tifs_us);

/**
  * @brief Set timing latch parameters
  * @param[in] param: parameters
  */
void ppt_set_timing_latch(ppt_timing_latch_t *param);

/**
  * @brief Set gpio trigger common parameters
  * @param[in] param: parameters
  */
void ppt_set_gpio_param(ppt_gpio_param_t *param);

/**
  * @brief Set gpio trigger channel specific parameters
  * @param[in] entry: channel index
  * @param[in] param: parameters
  */
void ppt_set_gpio_entry_param(uint8_t entry, ppt_gpio_entry_param_t *param);

/**
  * @brief Push the payload field to the tx fifo
  *
  * The hardware won't copy the data, so the data needs to be valid until it is acknowledged.
  * The other fields will be filled to the transmitted frame automatically by the hardware.
  * @param[in] entry: channel index
  * @param[in] payload: data pointer
  * @param[in] payload_len: length of payload
  */
bool ppt_push_tx_fifo(uint8_t entry, uint8_t *payload, uint16_t payload_len);

/**
  * @brief Update the payload field to the tx fifo
  *
  * The hardware won't copy the data, so the data needs to be valid until it is acknowledged.
  * The other fields will be filled to the transmitted frame automatically by the hardware.
  * @param[in] entry: channel index
  * @param[in] payload: data pointer
  * @param[in] payload_len: length of payload
  */
bool ppt_update_tx_fifo(uint8_t entry, uint8_t *payload, uint16_t payload_len);

/**
  * @brief Flush the tx fifo
  * @param[in] entry: channel index
  * @return read pointer
  */
uint8_t ppt_flush_tx_fifo(uint8_t entry);

/**
  * @brief Get the tx fifo read pointer
  * @param[in] entry: channel index
  * @return read pointer
  */
uint8_t ppt_get_tx_fifo_rdptr(uint8_t entry);

/**
  * @brief Set the tx fifo read pointer
  * @param[in] entry: channel index
  * @param[in] ptr: where to start tx next time
  */
void ppt_set_tx_fifo_rdptr(uint8_t entry, uint8_t ptr);

/**
  * @brief Get the tx fifo write pointer
  * @param[in] entry: channel index
  * @return write pointer
  */
uint8_t ppt_get_tx_fifo_wrptr(uint8_t entry);

/**
  * @brief Get the pend packet number of tx fifo
  * @param[in] entry: channel index
  * @return the number of packet pending at the tx fifo
  */
uint8_t ppt_get_tx_fifo_pend_pkt_num(uint8_t entry);

/**
  * @brief Get the free packet number of tx fifo
  * @param[in] entry: channel index
  * @return the number of packets that can be sent to the tx fifo
  */
uint8_t ppt_get_tx_fifo_free_pkt_num(uint8_t entry);

/**
  * @brief Reset the tx fifo sw & hw state
  *
  * The read and write pointer will be set to zero, and the fifo will be empty.
  * @param[in] entry: channel index
  */
void ppt_reset_tx_fifo(uint8_t entry);

/**
  * @brief Pop the received pdu from the rx fifo
  *
  * The rx fifo contains the PDU data including the header fields and payload field.
  * @param[in] entry: channel index
  * @param[out] pdu: data pointer
  * @param[in] pdu_len: length of pdu, attained from the rx stack information
  * @return operation result
  */
bool ppt_pop_rx_fifo(uint8_t entry, uint8_t *pdu, uint16_t pdu_len);

/**
  * @brief Pop the received pdu from the rx fifo with wait option
  *
  * The rx fifo contains the PDU data including the header fields and payload field.
  * When choosing to wait, this api equals @ref ppt_pop_rx_fifo.
  * When choosing not to wait, this api returns without waiting for the DMA move operation to be done.
  * The user can use @ref ppt_pop_rx_fifo_wait_done to check & wait for the DMA operation later.
  * @param[in] entry: channel index
  * @param[out] pdu: data pointer
  * @param[in] pdu_len: length of pdu, attained from the rx stack information
  * @param[in] wait: choose whether to wait for DMA move done
  * @return operation result
  */
bool ppt_pop_rx_fifo_with_wait_option(uint8_t entry, uint8_t *pdu, uint16_t pdu_len, bool wait);

/**
  * @brief Wait for pop the received pdu from the rx fifo operation to be done
  * @return operation result
  */
bool ppt_pop_rx_fifo_wait_done(void);

/**
  * @brief Flush the rx fifo
  *
  * Since the receiving state may be disrupted, some residual data needs to be flushed from the rx fifo.
  */
void ppt_flush_rx_fifo(void);

/**
  * @brief Flush the pending tx & rx interrupt
  *
  * The tx & rx interrupt isn't cared about, so flush them.
  */
void ppt_flush_trx_int(void);

/**
  * @brief Get rssi dbm value
  * @param[in] rssi_raw: rssi register value
  * @return the value
  */
int8_t ppt_get_rssi(uint16_t rssi_raw);

/**
  * @brief Get channel setting from radio frequency
  * @param[in] frequency: radio frequency in MHz
  * @param[out] bank: frequency band index
  * @param[out] channel: frequency offset
  * @return get result
  * @retval true: the input radio frequency is supported by the module
  * @retval false: the input radio frequency isn't supported by the module
  */
bool ppt_get_bank_channel(uint16_t frequency, uint8_t *bank, uint8_t *channel);

/**
  * @brief Get ble logic channel
  * @param[in] bank: frequency band index
  * @param[in] channel: frequency offset
  * @return logic channel index
  * @retval >=0: the input channel is supported by ble
  * @retval <0: the input channel isn't supported by ble
  */
int8_t ppt_get_ble_logic_chan(uint8_t bank, uint8_t channel);

/**
  * @brief Get ble logic channel
  * @param[in] frequency: frequency in MHz
  * @return logic channel index
  * @retval >=0: the input channel is supported by ble
  * @retval <0: the input channel isn't supported by ble
  */
int8_t ppt_get_ble_logic_chan_via_freq(uint16_t frequency);

/**
  * @brief Get whitening init value from ble logic channel
  * @param[in] le_logic_chan: ble logic channel
  * @return whitening init value
  */
uint32_t ppt_get_ble_white_init(uint8_t le_logic_chan);

/**
  * @brief Format diagnose output with raw data
  * @param[in] data: raw data
  * @param[in] data_len: raw data length
  * @param[in] fmt: format string
  */
void ppt_dump_log(uint8_t *data, uint32_t data_len, char *fmt, ...);

/**
  * @brief Dump all non-zero register
  */
void ppt_dump_nonzero_reg(void);

/**
  * @brief Calculate the crc of the designated data
  * @param[in] param: the crc parameters
  * @param[in] data: raw data
  * @param[in] data_len: the data length in bytes for byte mode and in bits for bit mode
  * @return the CRC result
  */
uint32_t ppt_crc_eng_cal(ppt_crc_eng_param_t *param, uint8_t *data, uint32_t data_len);

/**
  * @brief Initialize the dlps setting
  *
  * This api shall be called early before entering dlps, for example in the pwr_mgr_init.
  */
void ppt_dlps_init(void);

/**
  * @brief Deinitialize the dlps setting when disabling 2.4g module
  */
void ppt_dlps_deinit(void);

/**
  * @brief Register function that checks whether the 2.4g module can enter sleep mode
  * @param[in] check_cb: dlps enter check callback function pointer
  */
void ppt_dlps_reg_mac_check_cb(bool (*check_cb)(uint32_t *wakeup_time_diff));

/**
  * @brief Register function that stores some setting when mac enters dlps
  * @param[in] enter_cb: dlps enter store function pointer
  */
void ppt_dlps_reg_mac_enter_cb(void (*enter_cb)(void));

/**
  * @brief Register function that restores some setting when mac exits dlps
  * @param[in] exit_cb: dlps exit restore function pointer
  */
void ppt_dlps_reg_mac_exit_cb(void (*exit_cb)(void));

/**
  * @brief Register function that stores some settings when entering dlps
  * @param[in] enter_cb: dlps enter store function pointer
  */
void ppt_dlps_reg_pf_enter_cb(void (*enter_cb)(void));

/**
  * @brief Register function that restores some settings when exiting dlps
  * @param[in] exit_cb: dlps exit restore function pointer
  */
void ppt_dlps_reg_pf_exit_cb(void (*exit_cb)(void));

/** @} End of PPT_Driver_Exported_Functions */

///@cond
#define TIME_DEBUG                  0

#if TIME_DEBUG
#include "rtl876x_rcc.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_gpio.h"

#define TEST_PIN                    P2_2, P2_3, P2_4, P2_5
#define TIME_DEBUG_CHAN_INVALID     -1
#define TIME_DEBUG_CHAN_THREAD      0
#define TIME_DEBUG_CHAN_ISR         1
#define TIME_DEBUG_CHAN_ISR_TX      1
#define TIME_DEBUG_CHAN_ISR_RX      1
#define TIME_DEBUG_CHAN_ISR_GPIO    2
#define TIME_DEBUG_CHAN_TIMING      3
#define TIME_DEBUG_CHAN_DLPS        -1

extern uint8_t test_pin[];
extern uint32_t gpio_test_pin[];
extern bool level_test_pin[];
#define TIME_DEBUG_CHAN_NUMBER      COUNT_ARG(TEST_PIN)
void ppt_init_time_debug(void);

#define time_debug_level_high(ch)   do \
    { \
        if(ch >= 0 && ch < TIME_DEBUG_CHAN_NUMBER) \
        {\
            GPIO->DATAOUT |= gpio_test_pin[ch]; \
            level_test_pin[ch] = TRUE; \
        }\
    }while(0)
#define time_debug_level_low(ch)    do \
    { \
        if(ch >= 0 && ch < TIME_DEBUG_CHAN_NUMBER) \
        {\
            GPIO->DATAOUT &= ~gpio_test_pin[ch]; \
            level_test_pin[ch] = FALSE; \
        }\
    }while(0)
#define time_debug_edge_up(ch)      do \
    { \
        if(ch >= 0 && ch < TIME_DEBUG_CHAN_NUMBER) \
        {\
            GPIO->DATAOUT &= ~gpio_test_pin[ch]; \
            GPIO->DATAOUT |= gpio_test_pin[ch]; \
            level_test_pin[ch] = TRUE; \
        }\
    }while(0)
#define time_debug_edge_down(ch)      do \
    { \
        if(ch >= 0 && ch < TIME_DEBUG_CHAN_NUMBER) \
        {\
            GPIO->DATAOUT |= gpio_test_pin[ch]; \
            GPIO->DATAOUT &= ~gpio_test_pin[ch]; \
            level_test_pin[ch] = FALSE; \
        }\
    }while(0)
#else
#define time_debug_level_high(...)
#define time_debug_level_low(...)
#define time_debug_edge_up(...)
#define time_debug_edge_down(...)
#endif
void ppt_dbg_port_open(uint16_t bt_port, uint16_t ppt_port, uint16_t ppt_port2);
void ppt_dbg_port_set_pin(uint8_t pin_group, uint32_t dbg_bitmap);
void ppt_dbg_port(uint16_t bt_port, uint16_t ppt_port, uint16_t ppt_port2, uint32_t dbg_bitmap);
///@endcond

/** @} End of PPT_Driver */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
