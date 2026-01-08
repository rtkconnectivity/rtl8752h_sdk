/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

/* based on formal_reg_221109.xlsx */

/** @addtogroup PPT_Register
  * @{
  */

/** @defgroup PPT_REG_NAME_ADDR         Register Name & Address Offset
  * @brief Name and address declaration
  * @{
  */

#define PRO_INSTRUCTION                           0x0
#define PRO_BASE0_LOWER                           0x4
#define PRO_BASE0_UPPER                           0x6
#define PRO_BASE1_LOWER                           0x8
#define PRO_BASE1_UPPER                           0xa
#define PRO_PREFIX0_1                             0xc
#define PRO_PREFIX2_3                             0xe
#define PRO_PREFIX4_5                             0x10
#define PRO_PREFIX6_7                             0x12
#define PRO_TX_ADDRESS_LOWER                      0x14
#define PRO_TX_ADDRESS_MIDDLE                     0x16
#define PRO_TX_ADDRESS_UPPER                      0x18
#define PRO_TX_CONTROL                            0x20
#define PRO_PERIODIC_INTERVAL                     0x22
#define PRO_GPIO_TX_DELAY                         0x24
#define PRO_RX_CONTROL                            0x26
#define PRO_RX_MATCH_CONTROL                      0x28
#define PRO_TRX_CONTROL                           0x2a
#define PRO_FIELD_LENGTH2                         0x2c
#define PRO_FIELD_LENGTH                          0x30
#define PRO_PACKET_CONTROL                        0x32
#define PRO_GAIN                                  0x34
#define PRO_TX_PKT_NUM                            0x36
#define PRO_TX_PKT_NUM2                           0x38
#define PRO_HS_LENGTH                             0x3a
#define PRO_CHANNEL_BANK                          0x3c
#define PRO_GPIO_CONTROL                          0x3e
#define PRO_MISR                                  0x40
#define PRO_IMR                                   0x42
#define PRO_RSSI                                  0x4e
#define PRO_LENGTH_INCLUDE_ADDON                  0x50
#define PRO_LENGTH                                0x52
#define PRO_RX_STACK                              0x54
#define PRO_RX_HP                                 0x56
#define PRO_RX_CRC_LOWER                          0x58
#define PRO_RX_CRC_UPPER                          0x5a
#define PRO_ACCHIT_CLK_LOWER                      0x5c
#define PRO_ACCHIT_CLK_UPPER                      0x5e
#define PRO_RX_HS_LOWER                           0x60
#define PRO_RX_HS_UPPER                           0x62
#define PRO_TX_STACK                              0x64
#define PRO_TX_HS_LOWER                           0x66
#define PRO_TX_HS_UPPER                           0x68
#define PRO_TX_LENGTH                             0x6a
#define PRO_TX_CLK_LOWER                          0x6c
#define PRO_TX_CLK_UPPER                          0x6e
#define PRO_HP_0                                  0x80
#define PRO_HS_0_LOWER                            0x82
#define PRO_HS_0_UPPER                            0x84
#define PRO_LENGTH_0                              0x86
#define PRO_CRC_INIT0_LOWER                       0x88
#define PRO_CRC_INIT0_UPPER                       0x8a
#define PRO_CRC_POLYNOMIAL_REG0_LOWER             0x8c
#define PRO_CRC_POLYNOMIAL_REG0_UPPER             0x8e
#define PRO_PACKET_RELATED_CONTROL0               0x90
#define PRO_ADD_ON_LENGTH0                        0x92
#define PRO_WHITE_INIT0_LOWER                     0x94
#define PRO_WHITE_INIT0_UPPER                     0x96
#define PRO_WHITE_POLYNOMIAL_REG0_LOWER           0x98
#define PRO_WHITE_POLYNOMIAL_REG0_UPPER           0x9a
#define PRO_HP_1                                  0xa0
#define PRO_HS_1_LOWER                            0xa2
#define PRO_HS_1_UPPER                            0xa4
#define PRO_LENGTH_1                              0xa6
#define PRO_CRC_INIT1_LOWER                       0xa8
#define PRO_CRC_INIT1_UPPER                       0xaa
#define PRO_CRC_POLYNOMIAL_REG1_LOWER             0xac
#define PRO_CRC_POLYNOMIAL_REG1_UPPER             0xae
#define PRO_PACKET_RELATED_CONTROL1               0xb0
#define PRO_ADD_ON_LENGTH1                        0xb2
#define PRO_WHITE_INIT1_LOWER                     0xb4
#define PRO_WHITE_INIT1_UPPER                     0xb6
#define PRO_WHITE_POLYNOMIAL_REG1_LOWER           0xb8
#define PRO_WHITE_POLYNOMIAL_REG1_UPPER           0xba
#define PRO_HP_2                                  0xc0
#define PRO_HS_2_LOWER                            0xc2
#define PRO_HS_2_UPPER                            0xc4
#define PRO_LENGTH_2                              0xc6
#define PRO_CRC_INIT2_LOWER                       0xc8
#define PRO_CRC_INIT2_UPPER                       0xca
#define PRO_CRC_POLYNOMIAL_REG2_LOWER             0xcc
#define PRO_CRC_POLYNOMIAL_REG2_UPPER             0xce
#define PRO_PACKET_RELATED_CONTROL2               0xd0
#define PRO_ADD_ON_LENGTH2                        0xd2
#define PRO_WHITE_INIT2_LOWER                     0xd4
#define PRO_WHITE_INIT2_UPPER                     0xd6
#define PRO_WHITE_POLYNOMIAL_REG2_LOWER           0xd8
#define PRO_WHITE_POLYNOMIAL_REG2_UPPER           0xda
#define PRO_RESET_CTRL_REG                        0xdc
#define PRO_GPIO_CTRL_REG                         0xde
#define PRO_TX_ADDRESS2_LOWER                     0xe0
#define PRO_TX_ADDRESS2_MIDDLE                    0xe2
#define PRO_TX_ADDRESS2_UPPER                     0xe4
#define PRO_ORDER_CONTROL                         0xe6
#define PRO_GPIO_CTRL1_REG                        0xe8
#define PRO_GPIO_CTRL2_REG                        0xea
#define PRO_GPIO_NATIVE_CNT                       0xec
#define PRO_MAX_LENGTH                            0xee
#define PRO_RX_TURN_AROUND_DELAY                  0x100
#define PRO_2M_TRX_TURN_AROUND_DELAY              0x102
#define PRO_1M_2M_TRX_TURN_AROUND_DELAY           0x104
#define PRO_TX_TURN_AROUND_DELAY                  0x106
#define PRO_RX_TIMEOUT_DELAY                      0x108
#define PRO_TRX_ON_DELAY                          0x10a
#define PRO_2M_TRX_ON_DELAY                       0x10c
#define PRO_TRX_DEALY_MISC                        0x10e
#define PRO_PSD_RANGE0                            0x110
#define PRO_PSD_RANGE1                            0x112
#define PRO_PSD_TIMEOUT                           0x114
#define PRO_INT_CTRL                              0x118
#define PRO_GPIO_TX_DELAY1                        0x11a
#define PRO_INT_CTRL_TX                           0x11c
#define PRO_GPIO_TX_DELAY2                        0x11e
#define PRO_CBK_CTRL                              0x120
#define PRO_TRX_AFE                               0x122
#define PRO_DEBUG_SEL                             0x124
#define PRO_DEBUG_SEL2                            0x126
#define PRO_DSM_READ                              0x128
#define PRO_DSM_READ1                             0x12a
#define PRO_CRC_ENGINE_INIT                       0x12c
#define PRO_CRC_ENGINE_INIT1                      0x12e
#define PRO_CRC_ENGINE_POLY                       0x130
#define PRO_CRC_ENGINE_POLY1                      0x132
#define PRO_CRC_ENGINE_IN                         0x134
#define PRO_CRC_ENGINE_IN1                        0x136
#define PRO_CRC_ENGINE_IN2                        0x138
#define PRO_CRC_ENGINE_OUT                        0x13a
#define PRO_CRC_ENGINE_OUT1                       0x13c
#define PRO_CRC_ENGINE_EN                         0x13e
#define PRO_RX_CRC_INIT0_LOWER                    0x140
#define PRO_RX_CRC_INIT0_UPPER                    0x142
#define PRO_RX_CRC_INIT1_LOWER                    0x144
#define PRO_RX_CRC_INIT1_UPPER                    0x146
#define PRO_RX_CRC_INIT2_LOWER                    0x148
#define PRO_RX_CRC_INIT2_UPPER                    0x14a
#define PRO_HP_3                                  0x14c
#define PRO_HP_4                                  0x14e
#define PRO_HP_5                                  0x150
#define PRO_HS_3_LOWER                            0x152
#define PRO_HS_3_UPPER                            0x154
#define PRO_HS_4_LOWER                            0x156
#define PRO_HS_4_UPPER                            0x158
#define PRO_HS_5_LOWER                            0x15a
#define PRO_HS_5_UPPER                            0x15c
#define PRO_HS_6_LOWER                            0x15e
#define PRO_HS_6_UPPER                            0x160
#define PRO_HS_7_LOWER                            0x162
#define PRO_HS_7_UPPER                            0x164
#define PRO_LENGTH_3                              0x166
#define PRO_LENGTH_4                              0x168
#define PRO_LENGTH_5                              0x16a
#define PRO_LENGTH_6                              0x16c
#define PRO_LENGTH_7                              0x16e
#define PRO_ADD_ON_LENGTH3                        0x170
#define PRO_ADD_ON_LENGTH4                        0x172
#define PRO_ADD_ON_LENGTH5                        0x174
#define PRO_CRC_INIT3_LOWER                       0x176
#define PRO_CRC_INIT3_UPPER                       0x178
#define PRO_CRC_INIT4_LOWER                       0x17a
#define PRO_CRC_INIT4_UPPER                       0x17c
#define PRO_CRC_INIT5_LOWER                       0x17e
#define PRO_CRC_INIT5_UPPER                       0x180
#define PRO_CRC_INIT6_LOWER                       0x182
#define PRO_CRC_INIT6_UPPER                       0x184
#define PRO_CRC_INIT7_LOWER                       0x186
#define PRO_CRC_INIT7_UPPER                       0x188
#define PRO_RX_CRC_INIT3_LOWER                    0x18a
#define PRO_RX_CRC_INIT3_UPPER                    0x18c
#define PRO_RX_CRC_INIT4_LOWER                    0x18e
#define PRO_RX_CRC_INIT4_UPPER                    0x190
#define PRO_RX_CRC_INIT5_LOWER                    0x192
#define PRO_RX_CRC_INIT5_UPPER                    0x194
#define PRO_RX_CRC_INIT6_LOWER                    0x196
#define PRO_RX_CRC_INIT6_UPPER                    0x198
#define PRO_RX_CRC_INIT7_LOWER                    0x19a
#define PRO_RX_CRC_INIT7_UPPER                    0x19c
#define PRO_WHITE_INIT3_LOWER                     0x19e
#define PRO_WHITE_INIT3_UPPER                     0x1a0
#define PRO_WHITE_INIT4_LOWER                     0x1a2
#define PRO_WHITE_INIT4_UPPER                     0x1a4
#define PRO_WHITE_INIT5_LOWER                     0x1a6
#define PRO_WHITE_INIT5_UPPER                     0x1a8
#define PRO_WHITE_INIT6_LOWER                     0x1aa
#define PRO_WHITE_INIT6_UPPER                     0x1ac
#define PRO_WHITE_INIT7_LOWER                     0x1ae
#define PRO_WHITE_INIT7_UPPER                     0x1b0
#define PRO_WHITE_POLYNOMIAL_REG3_LOWER           0x1b2
#define PRO_WHITE_POLYNOMIAL_REG3_UPPER           0x1b4
#define PRO_WHITE_POLYNOMIAL_REG4_LOWER           0x1b6
#define PRO_WHITE_POLYNOMIAL_REG4_UPPER           0x1b8
#define PRO_WHITE_POLYNOMIAL_REG5_LOWER           0x1ba
#define PRO_WHITE_POLYNOMIAL_REG5_UPPER           0x1bc
#define PRO_WHITE_POLYNOMIAL_REG6_LOWER           0x1be
#define PRO_WHITE_POLYNOMIAL_REG6_UPPER           0x1c0
#define PRO_WHITE_POLYNOMIAL_REG7_LOWER           0x1c2
#define PRO_WHITE_POLYNOMIAL_REG7_UPPER           0x1c4
#define PRO_CRC_POLYNOMIAL_REG3_LOWER             0x1c6
#define PRO_CRC_POLYNOMIAL_REG3_UPPER             0x1c8
#define PRO_CRC_POLYNOMIAL_REG4_LOWER             0x1ca
#define PRO_CRC_POLYNOMIAL_REG4_UPPER             0x1cc
#define PRO_CRC_POLYNOMIAL_REG5_LOWER             0x1ce
#define PRO_CRC_POLYNOMIAL_REG5_UPPER             0x1d0
#define PRO_CRC_POLYNOMIAL_REG6_LOWER             0x1d2
#define PRO_CRC_POLYNOMIAL_REG6_UPPER             0x1d4
#define PRO_CRC_POLYNOMIAL_REG7_LOWER             0x1d6
#define PRO_CRC_POLYNOMIAL_REG7_UPPER             0x1d8
#define PRO_RX_PKT_NUM0                           0x1da
#define PRO_RX_PKT_NUM1                           0x1dc
#define PRO_RX_PKT_NUM2                           0x1de
#define PRO_TX_PKT_NUM3                           0x1e0
#define PRO_TX_PKT_NUM4                           0x1e2
#define PRO_TX_PKT_NUM5                           0x1e4
#define PRO_WHITE_FIELD_LENGTH_0                  0x1e6
#define PRO_WHITE_FIELD_LENGTH_1                  0x1e8
#define PRO_WHITE_FIELD_LENGTH_2                  0x1ea
#define PRO_WHITE_FIELD_LENGTH_3                  0x1ec
#define PRO_WHITE_FIELD_LENGTH_4                  0x1ee
#define PRO_FIELD_LENGTH3                         0x1f0
#define PRO_FIELD_LENGTH4                         0x1f2
#define PRO_FIELD_LENGTH5                         0x1f4
#define PRO_FIELD_LENGTH6                         0x1f6
#define PRO_FIELD_LENGTH7                         0x1f8
#define PRO_TX_ADDRESS1_LOWER                     0x1fa
#define PRO_TX_ADDRESS1_MIDDLE                    0x1fc
#define PRO_TX_ADDRESS1_UPPER                     0x1fe
#define PRO_TX_ADDRESS3_LOWER                     0x200
#define PRO_TX_ADDRESS3_MIDDLE                    0x202
#define PRO_TX_ADDRESS3_UPPER                     0x204
#define PRO_TX_ADDRESS4_LOWER                     0x206
#define PRO_TX_ADDRESS4_MIDDLE                    0x208
#define PRO_TX_ADDRESS4_UPPER                     0x20a
#define PRO_TX_ADDRESS5_LOWER                     0x20c
#define PRO_TX_ADDRESS5_MIDDLE                    0x20e
#define PRO_TX_ADDRESS5_UPPER                     0x210
#define PRO_TX_ADDRESS6_LOWER                     0x212
#define PRO_TX_ADDRESS6_MIDDLE                    0x214
#define PRO_TX_ADDRESS6_UPPER                     0x216
#define PRO_TX_ADDRESS7_LOWER                     0x218
#define PRO_TX_ADDRESS7_MIDDLE                    0x21a
#define PRO_TX_ADDRESS7_UPPER                     0x21c
#define PRO_RD_PTR_0                              0x21e
#define PRO_RD_PTR_1                              0x220
#define PRO_REG_TOTAL_SIZE                        (PRO_RD_PTR_1 + 2)

/** @} */

/** @defgroup PPT_REG_DEF      Register Field Definition
  * @brief Field definition in detail
  * @{
  */

/* 0x0
    7:0     RW  instruction                     0x0
    15:8    RO  rsvd                            0x0
 */
typedef union _PRO_INSTRUCTION_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t instruction: 8;
        uint16_t rsvd: 8;
    };
} PRO_INSTRUCTION_TYPE;

/* 0x4
    15:0    RW  base0[15:0]                     0x0
 */
typedef union _PRO_BASE0_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t base0_15_0;
    };
} PRO_BASE0_LOWER_TYPE;

/* 0x6
    15:0    RW  base0[31:16]                    0x0
 */
typedef union _PRO_BASE0_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t base0_31_16;
    };
} PRO_BASE0_UPPER_TYPE;

/* 0x8
    15:0    RW  base1[15:0]                     0x0
 */
typedef union _PRO_BASE1_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t base1_15_0;
    };
} PRO_BASE1_LOWER_TYPE;

/* 0xa
    15:0    RW  base1[31:16]                    0x0
 */
typedef union _PRO_BASE1_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t base1_31_16;
    };
} PRO_BASE1_UPPER_TYPE;

/* 0xc
    7:0     RW  prefix0[7:0]                    0x0
    15:8    RW  prefix1[7:0]                    0x0
 */
typedef union _PRO_PREFIX0_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t prefix0_7_0: 8;
        uint16_t prefix1_7_0: 8;
    };
} PRO_PREFIX0_1_TYPE;

/* 0xe
    7:0     RW  prefix2[7:0]                    0x0
    15:8    RW  prefix3[7:0]                    0x0
 */
typedef union _PRO_PREFIX2_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t prefix2_7_0: 8;
        uint16_t prefix3_7_0: 8;
    };
} PRO_PREFIX2_3_TYPE;

/* 0x10
    7:0     RW  prefix4[7:0]                    0x0
    15:8    RW  prefix5[7:0]                    0x0
 */
typedef union _PRO_PREFIX4_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t prefix4_7_0: 8;
        uint16_t prefix5_7_0: 8;
    };
} PRO_PREFIX4_5_TYPE;

/* 0x12
    7:0     RW  prefix6[7:0]                    0x0
    8       RW  length_order_first_rx           0x0
    9       RW  length_split_first_rx           0x0
    10      RW  hp_order_first_rx               0x0
    11      RW  hp_split_first_rx               0x0
    12      RW  hs_order_first_rx               0x0
    13      RW  hs_split_first_rx               0x0
    15:14   RO  rsvd                            0x0
 */
typedef union _PRO_PREFIX6_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t prefix6_7_0: 8;
        uint16_t length_order_first_rx: 1;
        uint16_t length_split_first_rx: 1;
        uint16_t hp_order_first_rx: 1;
        uint16_t hp_split_first_rx: 1;
        uint16_t hs_order_first_rx: 1;
        uint16_t hs_split_first_rx: 1;
        uint16_t rsvd: 2;
    };
} PRO_PREFIX6_7_TYPE;

/* 0x14
    15:0    RW  tx_address[15:0]                0x0
 */
typedef union _PRO_TX_ADDRESS_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address_15_0;
    };
} PRO_TX_ADDRESS_LOWER_TYPE;

/* 0x16
    15:0    RW  tx_address[31:16]               0x0
 */
typedef union _PRO_TX_ADDRESS_MIDDLE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address_31_16;
    };
} PRO_TX_ADDRESS_MIDDLE_TYPE;

/* 0x18
    7:0     RW  tx_address[39:32]               0x0
    15:8    RW  prefix7[7:0]                    0x0
 */
typedef union _PRO_TX_ADDRESS_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address_39_32: 8;
        uint16_t prefix7_7_0: 8;
    };
} PRO_TX_ADDRESS_UPPER_TYPE;

/* 0x20
    0       RW  periodic_enable                 0x0
    1       RW  no_tx0                          0x0
    2       RW  auto_no_tx_option0              0x0
    3       RW  lock0                           0x0
    4       RW  fix_tx_address_option           0x0
    5       RW  empty_flag1                     0x0
    6       RW  clear_hw_no_tx1                 0x0
    7       RW  clear_hw_rd_ptr0                0x0
    8       RW  clear_hw_rd_ptr1                0x0
    9       RW  clear_hw_no_tx0                 0x0
    10      RO  read_clear_hw_rd_ptr0           0x0
    11      RO  read_clear_hw_rd_ptr1           0x0
    12      RO  read_clear_hw_no_tx0            0x0
    13      RO  read_clear_hw_no_tx1            0x0
    14      RW  empty_flag0                     0x0
    15      RW  lock1                           0x0
 */
typedef union _PRO_TX_CONTROL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t periodic_enable: 1;
        uint16_t no_tx0: 1;
        uint16_t auto_no_tx_option0: 1;
        uint16_t lock0: 1;
        uint16_t fix_tx_address_option: 1;
        uint16_t empty_flag1: 1;
        uint16_t clear_hw_no_tx1: 1;
        uint16_t clear_hw_rd_ptr0: 1;
        uint16_t clear_hw_rd_ptr1: 1;
        uint16_t clear_hw_no_tx0: 1;
        uint16_t read_clear_hw_rd_ptr0: 1;
        uint16_t read_clear_hw_rd_ptr1: 1;
        uint16_t read_clear_hw_no_tx0: 1;
        uint16_t read_clear_hw_no_tx1: 1;
        uint16_t empty_flag0: 1;
        uint16_t lock1: 1;
    };
} PRO_TX_CONTROL_TYPE;

/* 0x22
    15:0    RW  periodic_interval[15:0]         0x0
 */
typedef union _PRO_PERIODIC_INTERVAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t periodic_interval_15_0;
    };
} PRO_PERIODIC_INTERVAL_TYPE;

/* 0x24
    15:0    RW  gpio_tx_delay[15:0]             0x0
 */
typedef union _PRO_GPIO_TX_DELAY_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t gpio_tx_delay_15_0;
    };
} PRO_GPIO_TX_DELAY_TYPE;

/* 0x26
    6:0     RO  rsvd                            0x0
    7       RW  lna_en                          0x0
    8       RW  max_length_kept                 0x0
    9       RW  rx_phy                          0x0
    10      RW  continuous_rx                   0x1
    11      RW  rx2rx_option                    0x0
    12      RW  fix_entry0_option               0x0
    13      RW  fix_entry1_option               0x0
    14      RW  error_address_kept              0x0
    15      RW  acchit_time_enable              0x0
 */
typedef union _PRO_RX_CONTROL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rsvd: 7;
        uint16_t lna_en: 1;
        uint16_t max_length_kept: 1;
        uint16_t rx_phy: 1;
        uint16_t continuous_rx: 1;
        uint16_t rx2rx_option: 1;
        uint16_t fix_entry0_option: 1;
        uint16_t fix_entry1_option: 1;
        uint16_t error_address_kept: 1;
        uint16_t acchit_time_enable: 1;
    };
} PRO_RX_CONTROL_TYPE;

/* 0x28
    7:0     RW  rx_match_enable_bit[7:0]        0x0
    8       RW  compare_prefix_option           0x0
    9       RW  tx_sdcnt_enable                 0x0
    10      RW  rx_sdcnt_enable                 0x0
    11      RW  use_maxlength_compare           0x0
    12      RW  opt_crc_err_no_ack              0x0
    15:13   RO  rsvd                            0x0
 */
typedef union _PRO_RX_MATCH_CONTROL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_match_enable_bit_7_0: 8;
        uint16_t compare_prefix_option: 1;
        uint16_t tx_sdcnt_enable: 1;
        uint16_t rx_sdcnt_enable: 1;
        uint16_t use_maxlength_compare: 1;
        uint16_t opt_crc_err_no_ack: 1;
        uint16_t rsvd: 3;
    };
} PRO_RX_MATCH_CONTROL_TYPE;

/* 0x2a
    0       RW  ack_enable                      0x0
    4:1     RW  periodic_interval[19:16]        0x0
    5       RW  auto_no_tx_option1              0x0
    6       RW  empty_flag2                     0x0
    7       RW  auto_no_rdptr_option1           0x0
    15:8    RW  tifs_reg[7:0]                   0x0
 */
typedef union _PRO_TRX_CONTROL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t ack_enable: 1;
        uint16_t periodic_interval_19_16: 4;
        uint16_t auto_no_tx_option1: 1;
        uint16_t empty_flag2: 1;
        uint16_t auto_no_rdptr_option1: 1;
        uint16_t tifs_reg_7_0: 8;
    };
} PRO_TRX_CONTROL_TYPE;

/* 0x2c
    2:0     RW  preamble_field_length1[2:0]     0x0
    7:3     RW  ramp_time_minus                 0x18
    15:8    RW  early_interrupt_time            0xa
 */
typedef union _PRO_FIELD_LENGTH2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t preamble_field_length1_2_0: 3;
        uint16_t ramp_time_minus: 5;
        uint16_t early_interrupt_time: 8;
    };
} PRO_FIELD_LENGTH2_TYPE;

/* 0x30
    2:0     RW  preamble_field_length0[2:0]     0x0
    5:3     RW  address_field_length[2:0]       0x0
    9:6     RW  hp_field_length[3:0]            0x0
    13:10   RW  length_field_length[3:0]        0x0
    14      RW  auto_no_rdptr_option0           0x0
    15      RW  dma_length_mode                 0x1
 */
typedef union _PRO_FIELD_LENGTH_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t preamble_field_length0_2_0: 3;
        uint16_t address_field_length_2_0: 3;
        uint16_t hp_field_length_3_0: 4;
        uint16_t length_field_length_3_0: 4;
        uint16_t auto_no_rdptr_option0: 1;
        uint16_t dma_length_mode: 1;
    };
} PRO_FIELD_LENGTH_TYPE;

/* 0x32
    0       RW  crc_kept                        0x0
    1       RW  crc_include_addr_enable         0x0
    4:2     RW  crc_field_length[2:0]           0x0
    6:5     RO  rsvd                            0x0
    7       RW  force_null_packet               0x0
    8       RW  inval_crc                       0x0
    9       RW  empty_kept                      0x0
    10      RW  white_dis                       0x0
    11      RW  hp_split                        0x0
    12      RW  length_split                    0x0
    13      RW  hs_split                        0x0
    14      RW  white_include_addr_enable       0x0
    15      RW  rif_lbt_en_pro                  0x0
 */
typedef union _PRO_PACKET_CONTROL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_kept: 1;
        uint16_t crc_include_addr_enable: 1;
        uint16_t crc_field_length_2_0: 3;
        uint16_t rsvd: 2;
        uint16_t force_null_packet: 1;
        uint16_t inval_crc: 1;
        uint16_t empty_kept: 1;
        uint16_t white_dis: 1;
        uint16_t hp_split: 1;
        uint16_t length_split: 1;
        uint16_t hs_split: 1;
        uint16_t white_include_addr_enable: 1;
        uint16_t rif_lbt_en_pro: 1;
    };
} PRO_PACKET_CONTROL_TYPE;

/* 0x34
    7:0     RW  rif_tx_gain                     0x0
    8       RW  en_afc_opt0                     0x1
    9       RW  en_afc_opt1                     0x1
    10      RW  en_afc_opt2                     0x0
    11      RW  en_afc_opt3                     0x0
    15:12   RO  rsvd                            0x0
 */
typedef union _PRO_GAIN_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rif_tx_gain: 8;
        uint16_t en_afc_opt0: 1;
        uint16_t en_afc_opt1: 1;
        uint16_t en_afc_opt2: 1;
        uint16_t en_afc_opt3: 1;
        uint16_t rsvd: 4;
    };
} PRO_GAIN_TYPE;

/* 0x36
    7:0     RO  tx_pkt_num0                     0x0
    15:8    RO  tx_pkt_num1                     0x0
 */
typedef union _PRO_TX_PKT_NUM_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_pkt_num0: 8;
        uint16_t tx_pkt_num1: 8;
    };
} PRO_TX_PKT_NUM_TYPE;

/* 0x38
    7:0     RO  tx_pkt_num2                     0x0
    8       RW  modem_compare2_addr             0x0
    15:9    RO  rsvd                            0x0
 */
typedef union _PRO_TX_PKT_NUM2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_pkt_num2: 8;
        uint16_t modem_compare2_addr: 1;
        uint16_t rsvd: 7;
    };
} PRO_TX_PKT_NUM2_TYPE;

/* 0x3a
    4:0     RW  hs_field_length[4:0]            0x0
    15:5    RO  rsvd                            0x0
 */
typedef union _PRO_HS_LENGTH_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hs_field_length_4_0: 5;
        uint16_t rsvd: 11;
    };
} PRO_HS_LENGTH_TYPE;

/* 0x3c
    6:0     RW  pro_channel                     0x0
    12:7    RO  rsvd2                           0x0
    15:13   RW  bank                            0x0
 */
typedef union _PRO_CHANNEL_BANK_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t pro_channel: 7;
        uint16_t rsvd2: 6;
        uint16_t bank: 3;
    };
} PRO_CHANNEL_BANK_TYPE;

/* 0x3e
    0       RW  fix_entry2_option               0x0
    1       RW  lock2                           0x0
    2       RO  rsvd1                           0x0
    3       RW  clear_hw_no_tx2                 0x0
    4       RW  auto_no_tx_option2              0x0
    5       RW  no_tx2                          0x0
    8:6     RW  preamble_field_length2          0x0
    9       RW  clear_hw_rd_ptr2                0x0
    10      RW  auto_no_rdptr_option2           0x0
    11      RW  reset_tx_pktnum2                0x0
    12      RO  read_clear_hw_no_tx2            0x0
    13      RO  read_clear_hw_rd_ptr2           0x0
    15:14   RO  rsvd                            0x0
 */
typedef union _PRO_GPIO_CONTROL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t fix_entry2_option: 1;
        uint16_t lock2: 1;
        uint16_t rsvd1: 1;
        uint16_t clear_hw_no_tx2: 1;
        uint16_t auto_no_tx_option2: 1;
        uint16_t no_tx2: 1;
        uint16_t preamble_field_length2: 3;
        uint16_t clear_hw_rd_ptr2: 1;
        uint16_t auto_no_rdptr_option2: 1;
        uint16_t reset_tx_pktnum2: 1;
        uint16_t read_clear_hw_no_tx2: 1;
        uint16_t read_clear_hw_rd_ptr2: 1;
        uint16_t rsvd: 2;
    };
} PRO_GPIO_CONTROL_TYPE;

/* 0x40
    0       RO  tx_int                          0x0
    1       RO  rx_int                          0x0
    2       RO  tx_early_int                    0x0
    3       RO  gpio_int                        0x0
    4       RO  kill_ptx_int                    0x0
    5       RO  kill_prx_int                    0x0
    6       RO  reset_trig_int                  0x0
    15:7    RO  rsvd                            0x0
 */
typedef union _PRO_MISR_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_int: 1;
        uint16_t rx_int: 1;
        uint16_t tx_early_int: 1;
        uint16_t gpio_int: 1;
        uint16_t kill_ptx_int: 1;
        uint16_t kill_prx_int: 1;
        uint16_t reset_trig_int: 1;
        uint16_t rsvd: 9;
    };
} PRO_MISR_TYPE;

/* 0x42
    0       RW  tx_int                          0x0
    1       RW  rx_int                          0x0
    2       RW  tx_early_int                    0x0
    3       RW  gpio_int                        0x0
    4       RW  kill_ptx_int                    0x0
    5       RW  kill_prx_int                    0x0
    6       RW  reset_trig_int                  0x0
    15:7    RW  imr[15:7](rsvd)                 0x0
 */
typedef union _PRO_IMR_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_int: 1;
        uint16_t rx_int: 1;
        uint16_t tx_early_int: 1;
        uint16_t gpio_int: 1;
        uint16_t kill_ptx_int: 1;
        uint16_t kill_prx_int: 1;
        uint16_t reset_trig_int: 1;
        uint16_t imr_15_7rsvd: 9;
    };
} PRO_IMR_TYPE;

/* 0x4e
    15:0    RO  rssi                            0x0
 */
typedef union _PRO_RSSI_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rssi;
    };
} PRO_RSSI_TYPE;

/* 0x50
    15:0    RO  length_include_addon            0x0
 */
typedef union _PRO_LENGTH_INCLUDE_ADDON_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t length_include_addon;
    };
} PRO_LENGTH_INCLUDE_ADDON_TYPE;

/* 0x52
    14:0    RO  rx_length                       0x0
    15      RO  rsvd                            0x0
 */
typedef union _PRO_LENGTH_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_length: 15;
        uint16_t rsvd: 1;
    };
} PRO_LENGTH_TYPE;

/* 0x54
    0       RO  is_max_length                   0x0
    1       RO  is_crc_error                    0x0
    2       RO  entry[0]                        0x0
    3       RO  rx_hit                          0x0
    4       RO  gpio_flag                       0x0
    5       RO  rx_time_out                     0x0
    6       RO  rx_abort_rd                     0x0
    8:7     RO  entry[2:1]                      0x0
    15:9    RO  rsvd                            0x0
 */
typedef union _PRO_RX_STACK_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t is_max_length: 1;
        uint16_t is_crc_error: 1;
        uint16_t entry_0: 1;
        uint16_t rx_hit: 1;
        uint16_t gpio_flag: 1;
        uint16_t rx_time_out: 1;
        uint16_t rx_abort_rd: 1;
        uint16_t entry_2_1: 2;
        uint16_t rsvd: 7;
    };
} PRO_RX_STACK_TYPE;

/* 0x56
    7:0     RO  hp                              0x0
    15:8    RO  rsvd                            0x0
 */
typedef union _PRO_RX_HP_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hp: 8;
        uint16_t rsvd: 8;
    };
} PRO_RX_HP_TYPE;

/* 0x58
    15:0    RO  rx_crc[15:0]                    0x0
 */
typedef union _PRO_RX_CRC_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_crc_15_0;
    };
} PRO_RX_CRC_LOWER_TYPE;

/* 0x5a
    15:0    RO  rx_crc[31:16]                   0x0
 */
typedef union _PRO_RX_CRC_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_crc_31_16;
    };
} PRO_RX_CRC_UPPER_TYPE;

/* 0x5c
    15:0    RO  acchit_clk[15:0]                0x0
 */
typedef union _PRO_ACCHIT_CLK_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t acchit_clk_15_0;
    };
} PRO_ACCHIT_CLK_LOWER_TYPE;

/* 0x5e
    15:0    RO  acchit_clk[31:16]               0x0
 */
typedef union _PRO_ACCHIT_CLK_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t acchit_clk_31_16;
    };
} PRO_ACCHIT_CLK_UPPER_TYPE;

/* 0x60
    15:0    RO  hs[15:0]                        0x0
 */
typedef union _PRO_RX_HS_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hs_15_0;
    };
} PRO_RX_HS_LOWER_TYPE;

/* 0x62
    14:0    RO  hs[30:16]                       0x0
    15      RO  rsvd                            0x0
 */
typedef union _PRO_RX_HS_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hs_30_16: 15;
        uint16_t rsvd: 1;
    };
} PRO_RX_HS_UPPER_TYPE;

/* 0x64
    0       RO  is_no_tx                        0x0
    1       RO  is_empty                        0x0
    5:2     RO  tx_ptr                          0x0
    7:6     RO  tx_entry[1:0]                   0x0
    15:8    RO  tx_hp                           0x0
 */
typedef union _PRO_TX_STACK_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t is_no_tx: 1;
        uint16_t is_empty: 1;
        uint16_t tx_ptr: 4;
        uint16_t tx_entry_1_0: 2;
        uint16_t tx_hp: 8;
    };
} PRO_TX_STACK_TYPE;

/* 0x66
    15:0    RO  tx_hs[15:0]                     0x0
 */
typedef union _PRO_TX_HS_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_hs_15_0;
    };
} PRO_TX_HS_LOWER_TYPE;

/* 0x68
    14:0    RO  tx_hs[30:16]                    0x0
    15      RO  rsvd                            0x0
 */
typedef union _PRO_TX_HS_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_hs_30_16: 15;
        uint16_t rsvd: 1;
    };
} PRO_TX_HS_UPPER_TYPE;

/* 0x6a
    10:0    RO  tx_length                       0x0
    11      RO  tx_entry[2]                     0x0
    15:12   RO  rsvd                            0x0
 */
typedef union _PRO_TX_LENGTH_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_length: 11;
        uint16_t tx_entry_2: 1;
        uint16_t rsvd: 4;
    };
} PRO_TX_LENGTH_TYPE;

/* 0x6c
    15:0    RO  tx_clk[15:0]                    0x0
 */
typedef union _PRO_TX_CLK_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_clk_15_0;
    };
} PRO_TX_CLK_LOWER_TYPE;

/* 0x6e
    15:0    RO  tx_clk[31:16]                   0x0
 */
typedef union _PRO_TX_CLK_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_clk_31_16;
    };
} PRO_TX_CLK_UPPER_TYPE;

/* 0x80
    7:0     RW  hp_0[7:0]                       0x0
    15:8    RO  rsvd                            0x0
 */
typedef union _PRO_HP_0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hp_0_7_0: 8;
        uint16_t rsvd: 8;
    };
} PRO_HP_0_TYPE;

/* 0x82
    15:0    RW  hs_0[15:0]                      0x0
 */
typedef union _PRO_HS_0_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hs_0_15_0;
    };
} PRO_HS_0_LOWER_TYPE;

/* 0x84
    14:0    RW  hs_0[30:16]                     0x0
    15      RO  rsvd                            0x0
 */
typedef union _PRO_HS_0_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hs_0_30_16: 15;
        uint16_t rsvd: 1;
    };
} PRO_HS_0_UPPER_TYPE;

/* 0x86
    14:0    RW  length_0[14:0]                  0x0
    15      RO  rsvd                            0x0
 */
typedef union _PRO_LENGTH_0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t length_0_14_0: 15;
        uint16_t rsvd: 1;
    };
} PRO_LENGTH_0_TYPE;

/* 0x88
    15:0    RW  crc_init0[15:0]                 0x0
 */
typedef union _PRO_CRC_INIT0_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_init0_15_0;
    };
} PRO_CRC_INIT0_LOWER_TYPE;

/* 0x8a
    15:0    RW  crc_init0[31:16]                0x0
 */
typedef union _PRO_CRC_INIT0_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_init0_31_16;
    };
} PRO_CRC_INIT0_UPPER_TYPE;

/* 0x8c
    15:0    RW  crc_polynomial_reg0[15:0]       0x0
 */
typedef union _PRO_CRC_POLYNOMIAL_REG0_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_polynomial_reg0_15_0;
    };
} PRO_CRC_POLYNOMIAL_REG0_LOWER_TYPE;

/* 0x8e
    15:0    RW  crc_polynomial_reg0[31:16]      0x0
 */
typedef union _PRO_CRC_POLYNOMIAL_REG0_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_polynomial_reg0_31_16;
    };
} PRO_CRC_POLYNOMIAL_REG0_UPPER_TYPE;

/* 0x90
    0       RW  tx_phy0                         0x0
    1       RW  auto_add_rptr_option0           0x0
    4:2     RW  fw_rd_ptr0                      0x0
    5       RW  fw_ack0                         0x0
    11:6    RW  white_field_length0             0x0
    15:12   RO  rsvd                            0x0
 */
typedef union _PRO_PACKET_RELATED_CONTROL0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_phy0: 1;
        uint16_t auto_add_rptr_option0: 1;
        uint16_t fw_rd_ptr0: 3;
        uint16_t fw_ack0: 1;
        uint16_t white_field_length0: 6;
        uint16_t rsvd: 4;
    };
} PRO_PACKET_RELATED_CONTROL0_TYPE;

/* 0x92
    7:0     RW  add_on_length0[7:0]             0x0
    15:8    RO  rx_pkt_num0                     0x0
 */
typedef union _PRO_ADD_ON_LENGTH0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t add_on_length0_7_0: 8;
        uint16_t rx_pkt_num0: 8;
    };
} PRO_ADD_ON_LENGTH0_TYPE;

/* 0x94
    15:0    RW  white_init0[15:0]               0x0
 */
typedef union _PRO_WHITE_INIT0_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_init0_15_0;
    };
} PRO_WHITE_INIT0_LOWER_TYPE;

/* 0x96
    15:0    RW  white_init0[31:16]              0x0
 */
typedef union _PRO_WHITE_INIT0_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_init0_31_16;
    };
} PRO_WHITE_INIT0_UPPER_TYPE;

/* 0x98
    15:0    RW  white_polynomial_reg0[15:0]     0x0
 */
typedef union _PRO_WHITE_POLYNOMIAL_REG0_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_polynomial_reg0_15_0;
    };
} PRO_WHITE_POLYNOMIAL_REG0_LOWER_TYPE;

/* 0x9a
    15:0    RW  white_polynomial_reg0[31:16]    0x0
 */
typedef union _PRO_WHITE_POLYNOMIAL_REG0_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_polynomial_reg0_31_16;
    };
} PRO_WHITE_POLYNOMIAL_REG0_UPPER_TYPE;

/* 0xa0
    7:0     RW  hp_1[7:0]                       0x0
    15:8    RO  rsvd                            0x0
 */
typedef union _PRO_HP_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hp_1_7_0: 8;
        uint16_t rsvd: 8;
    };
} PRO_HP_1_TYPE;

/* 0xa2
    15:0    RW  hs_1[15:0]                      0x0
 */
typedef union _PRO_HS_1_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hs_1_15_0;
    };
} PRO_HS_1_LOWER_TYPE;

/* 0xa4
    14:0    RW  hs_1[30:16]                     0x0
    15      RO  rsvd                            0x0
 */
typedef union _PRO_HS_1_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hs_1_30_16: 15;
        uint16_t rsvd: 1;
    };
} PRO_HS_1_UPPER_TYPE;

/* 0xa6
    14:0    RW  length_1[14:0]                  0x0
    15      RO  rsvd                            0x0
 */
typedef union _PRO_LENGTH_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t length_1_14_0: 15;
        uint16_t rsvd: 1;
    };
} PRO_LENGTH_1_TYPE;

/* 0xa8
    15:0    RW  crc_init1[15:0]                 0x0
 */
typedef union _PRO_CRC_INIT1_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_init1_15_0;
    };
} PRO_CRC_INIT1_LOWER_TYPE;

/* 0xaa
    15:0    RW  crc_init1[31:16]                0x0
 */
typedef union _PRO_CRC_INIT1_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_init1_31_16;
    };
} PRO_CRC_INIT1_UPPER_TYPE;

/* 0xac
    15:0    RW  crc_polynomial_reg1[15:0]       0x0
 */
typedef union _PRO_CRC_POLYNOMIAL_REG1_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_polynomial_reg1_15_0;
    };
} PRO_CRC_POLYNOMIAL_REG1_LOWER_TYPE;

/* 0xae
    15:0    RW  crc_polynomial_reg1[31:16]      0x0
 */
typedef union _PRO_CRC_POLYNOMIAL_REG1_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_polynomial_reg1_31_16;
    };
} PRO_CRC_POLYNOMIAL_REG1_UPPER_TYPE;

/* 0xb0
    0       RW  tx_phy1                         0x0
    1       RW  auto_add_rptr_option1           0x0
    4:2     RW  fw_rd_ptr1                      0x0
    5       RW  fw_ack1                         0x0
    11:6    RW  white_field_length1             0x0
    15:12   RO  rsvd                            0x0
 */
typedef union _PRO_PACKET_RELATED_CONTROL1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_phy1: 1;
        uint16_t auto_add_rptr_option1: 1;
        uint16_t fw_rd_ptr1: 3;
        uint16_t fw_ack1: 1;
        uint16_t white_field_length1: 6;
        uint16_t rsvd: 4;
    };
} PRO_PACKET_RELATED_CONTROL1_TYPE;

/* 0xb2
    7:0     RW  add_on_length1[7:0]             0x0
    15:8    RO  rx_pkt_num1                     0x0
 */
typedef union _PRO_ADD_ON_LENGTH1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t add_on_length1_7_0: 8;
        uint16_t rx_pkt_num1: 8;
    };
} PRO_ADD_ON_LENGTH1_TYPE;

/* 0xb4
    15:0    RW  white_init1[15:0]               0x0
 */
typedef union _PRO_WHITE_INIT1_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_init1_15_0;
    };
} PRO_WHITE_INIT1_LOWER_TYPE;

/* 0xb6
    15:0    RW  white_init1[31:16]              0x0
 */
typedef union _PRO_WHITE_INIT1_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_init1_31_16;
    };
} PRO_WHITE_INIT1_UPPER_TYPE;

/* 0xb8
    15:0    RW  white_polynomial_reg1[15:0]     0x0
 */
typedef union _PRO_WHITE_POLYNOMIAL_REG1_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_polynomial_reg1_15_0;
    };
} PRO_WHITE_POLYNOMIAL_REG1_LOWER_TYPE;

/* 0xba
    15:0    RW  white_polynomial_reg1[31:16]    0x0
 */
typedef union _PRO_WHITE_POLYNOMIAL_REG1_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_polynomial_reg1_31_16;
    };
} PRO_WHITE_POLYNOMIAL_REG1_UPPER_TYPE;

/* 0xc0
    7:0     RW  hp_2[7:0]                       0x0
    15:8    RO  rsvd                            0x0
 */
typedef union _PRO_HP_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hp_2_7_0: 8;
        uint16_t rsvd: 8;
    };
} PRO_HP_2_TYPE;

/* 0xc2
    15:0    RW  hs_2[15:0]                      0x0
 */
typedef union _PRO_HS_2_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hs_2_15_0;
    };
} PRO_HS_2_LOWER_TYPE;

/* 0xc4
    14:0    RW  hs_2[30:16]                     0x0
    15      RO  rsvd                            0x0
 */
typedef union _PRO_HS_2_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hs_2_30_16: 15;
        uint16_t rsvd: 1;
    };
} PRO_HS_2_UPPER_TYPE;

/* 0xc6
    14:0    RW  length_2[14:0]                  0x0
    15      RO  rsvd                            0x0
 */
typedef union _PRO_LENGTH_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t length_2_14_0: 15;
        uint16_t rsvd: 1;
    };
} PRO_LENGTH_2_TYPE;

/* 0xc8
    15:0    RW  crc_init2[15:0]                 0x0
 */
typedef union _PRO_CRC_INIT2_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_init2_15_0;
    };
} PRO_CRC_INIT2_LOWER_TYPE;

/* 0xca
    15:0    RW  crc_init2[31:16]                0x0
 */
typedef union _PRO_CRC_INIT2_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_init2_31_16;
    };
} PRO_CRC_INIT2_UPPER_TYPE;

/* 0xcc
    15:0    RW  crc_polynomial_reg2[15:0]       0x0
 */
typedef union _PRO_CRC_POLYNOMIAL_REG2_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_polynomial_reg2_15_0;
    };
} PRO_CRC_POLYNOMIAL_REG2_LOWER_TYPE;

/* 0xce
    15:0    RW  crc_polynomial_reg2[31:16]      0x0
 */
typedef union _PRO_CRC_POLYNOMIAL_REG2_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_polynomial_reg2_31_16;
    };
} PRO_CRC_POLYNOMIAL_REG2_UPPER_TYPE;

/* 0xd0
    0       RW  tx_phy2                         0x0
    1       RW  auto_add_rptr_option2           0x0
    4:2     RW  fw_rd_ptr2                      0x0
    5       RW  fw_ack2                         0x0
    11:6    RW  white_field_length2             0x0
    15:12   RO  rsvd                            0x0
 */
typedef union _PRO_PACKET_RELATED_CONTROL2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_phy2: 1;
        uint16_t auto_add_rptr_option2: 1;
        uint16_t fw_rd_ptr2: 3;
        uint16_t fw_ack2: 1;
        uint16_t white_field_length2: 6;
        uint16_t rsvd: 4;
    };
} PRO_PACKET_RELATED_CONTROL2_TYPE;

/* 0xd2
    7:0     RW  add_on_length2[7:0]             0x0
    15:8    RW  pro_rx_timeout_delay_ms         0x0
 */
typedef union _PRO_ADD_ON_LENGTH2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t add_on_length2_7_0: 8;
        uint16_t pro_rx_timeout_delay_ms: 8;
    };
} PRO_ADD_ON_LENGTH2_TYPE;

/* 0xd4
    15:0    RW  white_init2[15:0]               0x0
 */
typedef union _PRO_WHITE_INIT2_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_init2_15_0;
    };
} PRO_WHITE_INIT2_LOWER_TYPE;

/* 0xd6
    15:0    RW  white_init2[31:16]              0x0
 */
typedef union _PRO_WHITE_INIT2_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_init2_31_16;
    };
} PRO_WHITE_INIT2_UPPER_TYPE;

/* 0xd8
    15:0    RW  white_polynomial_reg2[15:0]     0x0
 */
typedef union _PRO_WHITE_POLYNOMIAL_REG2_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_polynomial_reg2_15_0;
    };
} PRO_WHITE_POLYNOMIAL_REG2_LOWER_TYPE;

/* 0xda
    15:0    RW  white_polynomial_reg2[31:16]    0x0
 */
typedef union _PRO_WHITE_POLYNOMIAL_REG2_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_polynomial_reg2_31_16;
    };
} PRO_WHITE_POLYNOMIAL_REG2_UPPER_TYPE;

/* 0xdc
    4:0     RW  pro_gpio_native_counter_reg     0x18
    5       RW  gpio_tx_enable                  0x0
    6       RW  gpio_tx_enable1                 0x0
    7       RW  gpio_tx_enable2                 0x0
    8       RW  fw_rst_trigger                  0x0
    9       RW  fw_rst_enable                   0x0
    10      RW  hw_rst_enable                   0x0
    11      RW  gpio_in_enable                  0x0
    12      RW  reset_trig_int_enable           0x1
    15:13   RW  dma_early_delay                 0x0
 */
typedef union _PRO_RESET_CTRL_REG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t pro_gpio_native_counter_reg: 5;
        uint16_t gpio_tx_enable: 1;
        uint16_t gpio_tx_enable1: 1;
        uint16_t gpio_tx_enable2: 1;
        uint16_t fw_rst_trigger: 1;
        uint16_t fw_rst_enable: 1;
        uint16_t hw_rst_enable: 1;
        uint16_t gpio_in_enable: 1;
        uint16_t reset_trig_int_enable: 1;
        uint16_t dma_early_delay: 3;
    };
} PRO_RESET_CTRL_REG_TYPE;

/* 0xde
    15:0    RW  gpio_cnt_reg                    0x1
 */
typedef union _PRO_GPIO_CTRL_REG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t gpio_cnt_reg;
    };
} PRO_GPIO_CTRL_REG_TYPE;

/* 0xe0
    15:0    RW  tx_address2[15:0]               0x0
 */
typedef union _PRO_TX_ADDRESS2_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address2_15_0;
    };
} PRO_TX_ADDRESS2_LOWER_TYPE;

/* 0xe2
    15:0    RW  tx_address2[31:16]              0x0
 */
typedef union _PRO_TX_ADDRESS2_MIDDLE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address2_31_16;
    };
} PRO_TX_ADDRESS2_MIDDLE_TYPE;

/* 0xe4
    7:0     RW  tx_address2[39:32]              0x0
    15:8    RO  rsvd                            0x0
 */
typedef union _PRO_TX_ADDRESS2_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address2_39_32: 8;
        uint16_t rsvd: 8;
    };
} PRO_TX_ADDRESS2_UPPER_TYPE;

/* 0xe6
    0       RW  preamble_order                  0x0
    1       RW  txfifo_out_order                0x0
    2       RW  txclk_order                     0x0
    3       RW  rxfifo_in_order                 0x0
    4       RW  tx_address_order                0x0
    5       RW  length_order                    0x0
    6       RW  hp_order                        0x0
    7       RW  hs_order                        0x0
    8       RW  length_order_first              0x0
    9       RW  length_split_first              0x0
    10      RW  hp_order_first                  0x0
    11      RW  hp_split_first                  0x0
    12      RW  hs_order_first                  0x0
    13      RW  hs_split_first                  0x0
    15:14   RO  rsvd                            0x0
 */
typedef union _PRO_ORDER_CONTROL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t preamble_order: 1;
        uint16_t txfifo_out_order: 1;
        uint16_t txclk_order: 1;
        uint16_t rxfifo_in_order: 1;
        uint16_t tx_address_order: 1;
        uint16_t length_order: 1;
        uint16_t hp_order: 1;
        uint16_t hs_order: 1;
        uint16_t length_order_first: 1;
        uint16_t length_split_first: 1;
        uint16_t hp_order_first: 1;
        uint16_t hp_split_first: 1;
        uint16_t hs_order_first: 1;
        uint16_t hs_split_first: 1;
        uint16_t rsvd: 2;
    };
} PRO_ORDER_CONTROL_TYPE;

/* 0xe8
    15:0    RW  gpio_cnt1_reg                   0x1
 */
typedef union _PRO_GPIO_CTRL1_REG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t gpio_cnt1_reg;
    };
} PRO_GPIO_CTRL1_REG_TYPE;

/* 0xea
    15:0    RW  gpio_cnt2_reg                   0x1
 */
typedef union _PRO_GPIO_CTRL2_REG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t gpio_cnt2_reg;
    };
} PRO_GPIO_CTRL2_REG_TYPE;

/* 0xec
    4:0     RW  pro_gpio_native_counter1_reg    0x18
    5       RW  gpio_rst_enable1                0x0
    7:6     RO  rsvd1                           0x0
    12:8    RW  pro_gpio_native_counter2_reg    0x18
    13      RW  gpio_rst_enable2                0x0
    15:14   RO  rsvd2                           0x0
 */
typedef union _PRO_GPIO_NATIVE_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t pro_gpio_native_counter1_reg: 5;
        uint16_t gpio_rst_enable1: 1;
        uint16_t rsvd1: 2;
        uint16_t pro_gpio_native_counter2_reg: 5;
        uint16_t gpio_rst_enable2: 1;
        uint16_t rsvd2: 2;
    };
} PRO_GPIO_NATIVE_CNT_TYPE;

/* 0xee
    9:0     RW  max_length                      0xff
    10      RW  txclk_split                     0x0
    11      RW  txfifo_out_split                0x0
    12      RW  rxfifo_in_split                 0x0
    15:13   RO  rsvd                            0x0
 */
typedef union _PRO_MAX_LENGTH_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t max_length: 10;
        uint16_t txclk_split: 1;
        uint16_t txfifo_out_split: 1;
        uint16_t rxfifo_in_split: 1;
        uint16_t rsvd: 3;
    };
} PRO_MAX_LENGTH_TYPE;

/* 0x100
    7:0     RW  pro_rx2tx_delay_base            0x83
    15:8    RW  pro_rx2rx_delay                 0x32
 */
typedef union _PRO_RX_TURN_AROUND_DELAY_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t pro_rx2tx_delay_base: 8;
        uint16_t pro_rx2rx_delay: 8;
    };
} PRO_RX_TURN_AROUND_DELAY_TYPE;

/* 0x102
    3:0     RW  pro_rx2tx_delay_1m              0x3
    7:4     RW  pro_tx2rx_delay_1m              0x0
    11:8    RW  pro_rx2tx_delay_2m              0x8
    15:12   RW  pro_tx2rx_delay_2m              0x0
 */
typedef union _PRO_2M_TRX_TURN_AROUND_DELAY_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t pro_rx2tx_delay_1m: 4;
        uint16_t pro_tx2rx_delay_1m: 4;
        uint16_t pro_rx2tx_delay_2m: 4;
        uint16_t pro_tx2rx_delay_2m: 4;
    };
} PRO_2M_TRX_TURN_AROUND_DELAY_TYPE;

/* 0x104
    3:0     RW  pro_rx2tx_delay_1m_2m           0x3
    7:4     RW  pro_tx2rx_delay_1m_2m           0x0
    11:8    RW  pro_rx2tx_delay_2m_1m           0x7
    15:12   RW  pro_tx2rx_delay_2m_1m           0x0
 */
typedef union _PRO_1M_2M_TRX_TURN_AROUND_DELAY_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t pro_rx2tx_delay_1m_2m: 4;
        uint16_t pro_tx2rx_delay_1m_2m: 4;
        uint16_t pro_rx2tx_delay_2m_1m: 4;
        uint16_t pro_tx2rx_delay_2m_1m: 4;
    };
} PRO_1M_2M_TRX_TURN_AROUND_DELAY_TYPE;

/* 0x106
    7:0     RW  pro_tx2rx_delay_base            0x83
    13:8    RW  pro_txon_early_1m               0x10
    15:14   RO  rsvd                            0x0
 */
typedef union _PRO_TX_TURN_AROUND_DELAY_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t pro_tx2rx_delay_base: 8;
        uint16_t pro_txon_early_1m: 6;
        uint16_t rsvd: 2;
    };
} PRO_TX_TURN_AROUND_DELAY_TYPE;

/* 0x108
    7:0     RW  pro_rx_timeout_delay_us         0x64
    11:8    RW  no_tx_early[3:0]                0x2
    15:12   RW  pro_txon_early_2m               0x0
 */
typedef union _PRO_RX_TIMEOUT_DELAY_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t pro_rx_timeout_delay_us: 8;
        uint16_t no_tx_early_3_0: 4;
        uint16_t pro_txon_early_2m: 4;
    };
} PRO_RX_TIMEOUT_DELAY_TYPE;

/* 0x10a
    7:0     RW  pro_txon_delay                  0xdc
    15:8    RW  pro_rxon_delay                  0xdc
 */
typedef union _PRO_TRX_ON_DELAY_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t pro_txon_delay: 8;
        uint16_t pro_rxon_delay: 8;
    };
} PRO_TRX_ON_DELAY_TYPE;

/* 0x10c
    7:0     RW  pro_txon_delay_2m               0xdc
    15:8    RW  pro_rxon_delay_2m               0xdc
 */
typedef union _PRO_2M_TRX_ON_DELAY_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t pro_txon_delay_2m: 8;
        uint16_t pro_rxon_delay_2m: 8;
    };
} PRO_2M_TRX_ON_DELAY_TYPE;

/* 0x10e
    3:0     RW  txon_ext                        0x4
    4       RW  en_early_txon_tgl               0x1
    7:5     RW  no_tx_early[6:4]                0x0
    15:8    RW  get_ack_before_send             0x1
 */
typedef union _PRO_TRX_DEALY_MISC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t txon_ext: 4;
        uint16_t en_early_txon_tgl: 1;
        uint16_t no_tx_early_6_4: 3;
        uint16_t get_ack_before_send: 8;
    };
} PRO_TRX_DEALY_MISC_TYPE;

/* 0x110
    6:0     RW  pro_psd_start                   0x0
    7       RO  rsvd0                           0x0
    14:8    RW  pro_psd_end                     0x0
    15      RO  rsvd1                           0x0
 */
typedef union _PRO_PSD_RANGE0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t pro_psd_start: 7;
        uint16_t rsvd0: 1;
        uint16_t pro_psd_end: 7;
        uint16_t rsvd1: 1;
    };
} PRO_PSD_RANGE0_TYPE;

/* 0x112
    6:0     RW  pro_psd_step                    0x0
    7       RW  pro_psd_mode                    0x0
    8       RW  psd_rst_enable                  0x0
    13:9    RW  psd_rst_cnt_reg                 0x2
    15:14   RO  rsvd                            0x0
 */
typedef union _PRO_PSD_RANGE1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t pro_psd_step: 7;
        uint16_t pro_psd_mode: 1;
        uint16_t psd_rst_enable: 1;
        uint16_t psd_rst_cnt_reg: 5;
        uint16_t rsvd: 2;
    };
} PRO_PSD_RANGE1_TYPE;

/* 0x114
    9:0     RW  pro_psd_timeout                 0x114
    15:10   RO  rsvd                            0x0
 */
typedef union _PRO_PSD_TIMEOUT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t pro_psd_timeout: 10;
        uint16_t rsvd: 6;
    };
} PRO_PSD_TIMEOUT_TYPE;

/* 0x118
    14:0    RO  rsvd                            0x0
    15      RW  rx_int_stack_ovf                0x0
 */
typedef union _PRO_INT_CTRL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rsvd: 15;
        uint16_t rx_int_stack_ovf: 1;
    };
} PRO_INT_CTRL_TYPE;

/* 0x11a
    15:0    RW  gpio_tx_delay1[15:0]            0x0
 */
typedef union _PRO_GPIO_TX_DELAY1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t gpio_tx_delay1_15_0;
    };
} PRO_GPIO_TX_DELAY1_TYPE;

/* 0x11c
    14:0    RO  rsvd                            0x0
    15      RW  tx_int_stack_ovf                0x0
 */
typedef union _PRO_INT_CTRL_TX_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rsvd: 15;
        uint16_t tx_int_stack_ovf: 1;
    };
} PRO_INT_CTRL_TX_TYPE;

/* 0x11e
    15:0    RW  gpio_tx_delay2[15:0]            0x0
 */
typedef union _PRO_GPIO_TX_DELAY2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t gpio_tx_delay2_15_0;
    };
} PRO_GPIO_TX_DELAY2_TYPE;

/* 0x120
    0       RW  opt_pro_ce_end                  0x0
    1       RW  reset_tx_pktnum0                0x0
    2       RW  reset_tx_pktnum1                0x0
    3       RW  reset_rx_pktnum0                0x0
    4       RW  reset_rx_pktnum1                0x0
    5       RW  tx_time_enable                  0x0
    6       RW  no_tx1                          0x0
    7       RW  cbk_pcd_cg                      0x1
    8       RW  cbk_radio_cg                    0x1
    9       RW  cbk_txrx_time_cg                0x1
    10      RW  sd_cnt_enable                   0x0
    11      RW  no_tx_trig_stack_enable         0x0
    15:12   RW  dma_cnt_reg                     0x7
 */
typedef union _PRO_CBK_CTRL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t opt_pro_ce_end: 1;
        uint16_t reset_tx_pktnum0: 1;
        uint16_t reset_tx_pktnum1: 1;
        uint16_t reset_rx_pktnum0: 1;
        uint16_t reset_rx_pktnum1: 1;
        uint16_t tx_time_enable: 1;
        uint16_t no_tx1: 1;
        uint16_t cbk_pcd_cg: 1;
        uint16_t cbk_radio_cg: 1;
        uint16_t cbk_txrx_time_cg: 1;
        uint16_t sd_cnt_enable: 1;
        uint16_t no_tx_trig_stack_enable: 1;
        uint16_t dma_cnt_reg: 4;
    };
} PRO_CBK_CTRL_TYPE;

/* 0x122
    7:0     RW  txon_early_afe                  0x6
    15:8    RW  rxon_early_afe                  0x6
 */
typedef union _PRO_TRX_AFE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t txon_early_afe: 8;
        uint16_t rxon_early_afe: 8;
    };
} PRO_TRX_AFE_TYPE;

/* 0x124
    3:0     RW  ptx_dbg_sel                     0x0
    5:4     RW  control_dbg_sel                 0x0
    8:6     RW  trx_dbg_sel                     0x0
    11:9    RW  radio_dbg_sel                   0x0
    15:12   RW  dma_dbg_sel[3:0]                0x0
 */
typedef union _PRO_DEBUG_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t ptx_dbg_sel: 4;
        uint16_t control_dbg_sel: 2;
        uint16_t trx_dbg_sel: 3;
        uint16_t radio_dbg_sel: 3;
        uint16_t dma_dbg_sel_3_0: 4;
    };
} PRO_DEBUG_SEL_TYPE;

/* 0x126
    3:0     RW  pcd_dbg_sel                     0x0
    5:4     RW  stack_dbg_sel                   0x0
    8:6     RW  top_dbg_sel                     0x0
    9       RW  dma_dbg_sel[4]                  0x0
    15:10   RO  rsvd                            0x0
 */
typedef union _PRO_DEBUG_SEL2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t pcd_dbg_sel: 4;
        uint16_t stack_dbg_sel: 2;
        uint16_t top_dbg_sel: 3;
        uint16_t dma_dbg_sel_4: 1;
        uint16_t rsvd: 6;
    };
} PRO_DEBUG_SEL2_TYPE;

/* 0x128
    5:0     RO  radio_cst                       0x0
    6       RO  radio_idle                      0x1
    10:7    RO  timing_cst                      0x0
    11      RO  timing_idle                     0x1
    12      RO  prx_fsm_idle                    0x1
    14:13   RO  prx_cst                         0x0
    15      RO  rsvd                            0x0
 */
typedef union _PRO_DSM_READ_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t radio_cst: 6;
        uint16_t radio_idle: 1;
        uint16_t timing_cst: 4;
        uint16_t timing_idle: 1;
        uint16_t prx_fsm_idle: 1;
        uint16_t prx_cst: 2;
        uint16_t rsvd: 1;
    };
} PRO_DSM_READ_TYPE;

/* 0x12a
    0       RO  ptx_fsm_idle                    0x1
    2:1     RO  ptx_cst                         0x0
    6:3     RO  p2s_cst                         0x0
    7       RO  pcd_p2s_idle                    0x1
    12:8    RO  pcd_ctrl_cst                    0x0
    13      RO  pcd_idle                        0x1
    15:14   RO  rsvd                            0x0
 */
typedef union _PRO_DSM_READ1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t ptx_fsm_idle: 1;
        uint16_t ptx_cst: 2;
        uint16_t p2s_cst: 4;
        uint16_t pcd_p2s_idle: 1;
        uint16_t pcd_ctrl_cst: 5;
        uint16_t pcd_idle: 1;
        uint16_t rsvd: 2;
    };
} PRO_DSM_READ1_TYPE;

/* 0x12c
    15:0    RW  crc_engine_init[15:0]           0x0
 */
typedef union _PRO_CRC_ENGINE_INIT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_engine_init_15_0;
    };
} PRO_CRC_ENGINE_INIT_TYPE;

/* 0x12e
    15:0    RW  crc_engine_init[31:16]          0x0
 */
typedef union _PRO_CRC_ENGINE_INIT1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_engine_init_31_16;
    };
} PRO_CRC_ENGINE_INIT1_TYPE;

/* 0x130
    15:0    RW  crc_engine_polynomial[15:0]     0x0
 */
typedef union _PRO_CRC_ENGINE_POLY_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_engine_polynomial_15_0;
    };
} PRO_CRC_ENGINE_POLY_TYPE;

/* 0x132
    15:0    RW  crc_engine_polynomial[31:16]    0x0
 */
typedef union _PRO_CRC_ENGINE_POLY1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_engine_polynomial_31_16;
    };
} PRO_CRC_ENGINE_POLY1_TYPE;

/* 0x134
    15:0    RW  crc_engine_input_data[15:0]     0x0
 */
typedef union _PRO_CRC_ENGINE_IN_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_engine_input_data_15_0;
    };
} PRO_CRC_ENGINE_IN_TYPE;

/* 0x136
    15:0    RW  crc_engine_input_data[31:16]    0x0
 */
typedef union _PRO_CRC_ENGINE_IN1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_engine_input_data_31_16;
    };
} PRO_CRC_ENGINE_IN1_TYPE;

/* 0x138
    7:0     RW  crc_engine_input_data[39:32]    0x0
    15:8    RO  rsvd                            0x0
 */
typedef union _PRO_CRC_ENGINE_IN2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_engine_input_data_39_32: 8;
        uint16_t rsvd: 8;
    };
} PRO_CRC_ENGINE_IN2_TYPE;

/* 0x13a
    15:0    RO  crc_engine_output[15:0]         0x0
 */
typedef union _PRO_CRC_ENGINE_OUT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_engine_output_15_0;
    };
} PRO_CRC_ENGINE_OUT_TYPE;

/* 0x13c
    15:0    RO  crc_engine_output[31:16]        0x0
 */
typedef union _PRO_CRC_ENGINE_OUT1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_engine_output_31_16;
    };
} PRO_CRC_ENGINE_OUT1_TYPE;

/* 0x13e
    0       RW  crc_engine_en                   0x0
    1       RW  crc_engine_rst                  0x0
    4:2     RW  crc_engine_field_length         0x0
    10:5    RW  crc_engine_data_length          0x0
    11      RW  crc_engine_cg                   0x1
    12      RW  crc_engine_input_endian         0x0
    13      RW  crc_engine_output_endian        0x0
    14      RW  crc_engine_data_length_in_bit   0x1
    15      RO  rsvd                            0x0
 */
typedef union _PRO_CRC_ENGINE_EN_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_engine_en: 1;
        uint16_t crc_engine_rst: 1;
        uint16_t crc_engine_field_length: 3;
        uint16_t crc_engine_data_length: 6;
        uint16_t crc_engine_cg: 1;
        uint16_t crc_engine_input_endian: 1;
        uint16_t crc_engine_output_endian: 1;
        uint16_t crc_engine_data_length_in_bit: 1;
        uint16_t rsvd: 1;
    };
} PRO_CRC_ENGINE_EN_TYPE;

/* 0x140
    15:0    RW  rx_crc_init0[15:0]              0x0
 */
typedef union _PRO_RX_CRC_INIT0_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_crc_init0_15_0;
    };
} PRO_RX_CRC_INIT0_LOWER_TYPE;

/* 0x142
    15:0    RW  rx_crc_init0[31:16]             0x0
 */
typedef union _PRO_RX_CRC_INIT0_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_crc_init0_31_16;
    };
} PRO_RX_CRC_INIT0_UPPER_TYPE;

/* 0x144
    15:0    RW  rx_crc_init1[15:0]              0x0
 */
typedef union _PRO_RX_CRC_INIT1_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_crc_init1_15_0;
    };
} PRO_RX_CRC_INIT1_LOWER_TYPE;

/* 0x146
    15:0    RW  rx_crc_init1[31:16]             0x0
 */
typedef union _PRO_RX_CRC_INIT1_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_crc_init1_31_16;
    };
} PRO_RX_CRC_INIT1_UPPER_TYPE;

/* 0x148
    15:0    RW  rx_crc_init2[15:0]              0x0
 */
typedef union _PRO_RX_CRC_INIT2_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_crc_init2_15_0;
    };
} PRO_RX_CRC_INIT2_LOWER_TYPE;

/* 0x14a
    15:0    RW  rx_crc_init2[31:16]             0x0
 */
typedef union _PRO_RX_CRC_INIT2_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_crc_init2_31_16;
    };
} PRO_RX_CRC_INIT2_UPPER_TYPE;

/* 0x14c
    7:0     RW  hp_3[7:0]                       0x0
    15:8    RW  hp_4[7:0]                       0x0
 */
typedef union _PRO_HP_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hp_3_7_0: 8;
        uint16_t hp_4_7_0: 8;
    };
} PRO_HP_3_TYPE;

/* 0x14e
    7:0     RW  hp_5[7:0]                       0x0
    15:8    RW  hp_6[7:0]                       0x0
 */
typedef union _PRO_HP_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hp_5_7_0: 8;
        uint16_t hp_6_7_0: 8;
    };
} PRO_HP_4_TYPE;

/* 0x150
    7:0     RW  hp_7[7:0]                       0x0
    8       RW  fix_entry_option                0x0
    11:9    RW  fix_ptx_entry[2:0]              0x0
    12      RW  fix_entry_rx_option             0x0
    15:13   RW  fix_prx_tx_entry[2:0]           0x0
 */
typedef union _PRO_HP_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hp_7_7_0: 8;
        uint16_t fix_entry_option: 1;
        uint16_t fix_ptx_entry_2_0: 3;
        uint16_t fix_entry_rx_option: 1;
        uint16_t fix_prx_tx_entry_2_0: 3;
    };
} PRO_HP_5_TYPE;

/* 0x152
    15:0    RW  hs_3[15:0]                      0x0
 */
typedef union _PRO_HS_3_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hs_3_15_0;
    };
} PRO_HS_3_LOWER_TYPE;

/* 0x154
    14:0    RW  hs_3[30:16]                     0x0
    15      RO  rsvd                            0x0
 */
typedef union _PRO_HS_3_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hs_3_30_16: 15;
        uint16_t rsvd: 1;
    };
} PRO_HS_3_UPPER_TYPE;

/* 0x156
    15:0    RW  hs_4[15:0]                      0x0
 */
typedef union _PRO_HS_4_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hs_4_15_0;
    };
} PRO_HS_4_LOWER_TYPE;

/* 0x158
    14:0    RW  hs_4[30:16]                     0x0
    15      RO  rsvd                            0x0
 */
typedef union _PRO_HS_4_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hs_4_30_16: 15;
        uint16_t rsvd: 1;
    };
} PRO_HS_4_UPPER_TYPE;

/* 0x15a
    15:0    RW  hs_5[15:0]                      0x0
 */
typedef union _PRO_HS_5_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hs_5_15_0;
    };
} PRO_HS_5_LOWER_TYPE;

/* 0x15c
    14:0    RW  hs_5[30:16]                     0x0
    15      RO  rsvd                            0x0
 */
typedef union _PRO_HS_5_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hs_5_30_16: 15;
        uint16_t rsvd: 1;
    };
} PRO_HS_5_UPPER_TYPE;

/* 0x15e
    15:0    RW  hs_6[15:0]                      0x0
 */
typedef union _PRO_HS_6_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hs_6_15_0;
    };
} PRO_HS_6_LOWER_TYPE;

/* 0x160
    14:0    RW  hs_6[30:16]                     0x0
    15      RO  rsvd                            0x0
 */
typedef union _PRO_HS_6_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hs_6_30_16: 15;
        uint16_t rsvd: 1;
    };
} PRO_HS_6_UPPER_TYPE;

/* 0x162
    15:0    RW  hs_7[15:0]                      0x0
 */
typedef union _PRO_HS_7_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hs_7_15_0;
    };
} PRO_HS_7_LOWER_TYPE;

/* 0x164
    14:0    RW  hs_7[30:16]                     0x0
    15      RO  rsvd                            0x0
 */
typedef union _PRO_HS_7_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t hs_7_30_16: 15;
        uint16_t rsvd: 1;
    };
} PRO_HS_7_UPPER_TYPE;

/* 0x166
    14:0    RW  length_3[14:0]                  0x0
    15      RO  rsvd                            0x0
 */
typedef union _PRO_LENGTH_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t length_3_14_0: 15;
        uint16_t rsvd: 1;
    };
} PRO_LENGTH_3_TYPE;

/* 0x168
    14:0    RW  length_4[14:0]                  0x0
    15      RO  rsvd                            0x0
 */
typedef union _PRO_LENGTH_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t length_4_14_0: 15;
        uint16_t rsvd: 1;
    };
} PRO_LENGTH_4_TYPE;

/* 0x16a
    14:0    RW  length_5[14:0]                  0x0
    15      RO  rsvd                            0x0
 */
typedef union _PRO_LENGTH_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t length_5_14_0: 15;
        uint16_t rsvd: 1;
    };
} PRO_LENGTH_5_TYPE;

/* 0x16c
    14:0    RW  length_6[14:0]                  0x0
    15      RO  rsvd                            0x0
 */
typedef union _PRO_LENGTH_6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t length_6_14_0: 15;
        uint16_t rsvd: 1;
    };
} PRO_LENGTH_6_TYPE;

/* 0x16e
    14:0    RW  length_7[14:0]                  0x0
    15      RO  rsvd                            0x0
 */
typedef union _PRO_LENGTH_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t length_7_14_0: 15;
        uint16_t rsvd: 1;
    };
} PRO_LENGTH_7_TYPE;

/* 0x170
    7:0     RW  add_on_length3[7:0]             0x0
    15:8    RW  add_on_length4[7:0]             0x0
 */
typedef union _PRO_ADD_ON_LENGTH3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t add_on_length3_7_0: 8;
        uint16_t add_on_length4_7_0: 8;
    };
} PRO_ADD_ON_LENGTH3_TYPE;

/* 0x172
    7:0     RW  add_on_length5[7:0]             0x0
    15:8    RW  add_on_length6[7:0]             0x0
 */
typedef union _PRO_ADD_ON_LENGTH4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t add_on_length5_7_0: 8;
        uint16_t add_on_length6_7_0: 8;
    };
} PRO_ADD_ON_LENGTH4_TYPE;

/* 0x174
    7:0     RW  add_on_length7[7:0]             0x0
    15:8    RO  rsvd                            0x0
 */
typedef union _PRO_ADD_ON_LENGTH5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t add_on_length7_7_0: 8;
        uint16_t rsvd: 8;
    };
} PRO_ADD_ON_LENGTH5_TYPE;

/* 0x176
    15:0    RW  crc_init3[15:0]                 0x0
 */
typedef union _PRO_CRC_INIT3_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_init3_15_0;
    };
} PRO_CRC_INIT3_LOWER_TYPE;

/* 0x178
    15:0    RW  crc_init3[31:16]                0x0
 */
typedef union _PRO_CRC_INIT3_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_init3_31_16;
    };
} PRO_CRC_INIT3_UPPER_TYPE;

/* 0x17a
    15:0    RW  crc_init4[15:0]                 0x0
 */
typedef union _PRO_CRC_INIT4_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_init4_15_0;
    };
} PRO_CRC_INIT4_LOWER_TYPE;

/* 0x17c
    15:0    RW  crc_init4[31:16]                0x0
 */
typedef union _PRO_CRC_INIT4_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_init4_31_16;
    };
} PRO_CRC_INIT4_UPPER_TYPE;

/* 0x17e
    15:0    RW  crc_init5[15:0]                 0x0
 */
typedef union _PRO_CRC_INIT5_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_init5_15_0;
    };
} PRO_CRC_INIT5_LOWER_TYPE;

/* 0x180
    15:0    RW  crc_init5[31:16]                0x0
 */
typedef union _PRO_CRC_INIT5_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_init5_31_16;
    };
} PRO_CRC_INIT5_UPPER_TYPE;

/* 0x182
    15:0    RW  crc_init6[15:0]                 0x0
 */
typedef union _PRO_CRC_INIT6_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_init6_15_0;
    };
} PRO_CRC_INIT6_LOWER_TYPE;

/* 0x184
    15:0    RW  crc_init6[31:16]                0x0
 */
typedef union _PRO_CRC_INIT6_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_init6_31_16;
    };
} PRO_CRC_INIT6_UPPER_TYPE;

/* 0x186
    15:0    RW  crc_init7[15:0]                 0x0
 */
typedef union _PRO_CRC_INIT7_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_init7_15_0;
    };
} PRO_CRC_INIT7_LOWER_TYPE;

/* 0x188
    15:0    RW  crc_init7[31:16]                0x0
 */
typedef union _PRO_CRC_INIT7_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_init7_31_16;
    };
} PRO_CRC_INIT7_UPPER_TYPE;

/* 0x18a
    15:0    RW  rx_crc_init3[15:0]              0x0
 */
typedef union _PRO_RX_CRC_INIT3_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_crc_init3_15_0;
    };
} PRO_RX_CRC_INIT3_LOWER_TYPE;

/* 0x18c
    15:0    RW  rx_crc_init3[31:16]             0x0
 */
typedef union _PRO_RX_CRC_INIT3_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_crc_init3_31_16;
    };
} PRO_RX_CRC_INIT3_UPPER_TYPE;

/* 0x18e
    15:0    RW  rx_crc_init4[15:0]              0x0
 */
typedef union _PRO_RX_CRC_INIT4_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_crc_init4_15_0;
    };
} PRO_RX_CRC_INIT4_LOWER_TYPE;

/* 0x190
    15:0    RW  rx_crc_init4[31:16]             0x0
 */
typedef union _PRO_RX_CRC_INIT4_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_crc_init4_31_16;
    };
} PRO_RX_CRC_INIT4_UPPER_TYPE;

/* 0x192
    15:0    RW  rx_crc_init5[15:0]              0x0
 */
typedef union _PRO_RX_CRC_INIT5_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_crc_init5_15_0;
    };
} PRO_RX_CRC_INIT5_LOWER_TYPE;

/* 0x194
    15:0    RW  rx_crc_init5[31:16]             0x0
 */
typedef union _PRO_RX_CRC_INIT5_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_crc_init5_31_16;
    };
} PRO_RX_CRC_INIT5_UPPER_TYPE;

/* 0x196
    15:0    RW  rx_crc_init6[15:0]              0x0
 */
typedef union _PRO_RX_CRC_INIT6_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_crc_init6_15_0;
    };
} PRO_RX_CRC_INIT6_LOWER_TYPE;

/* 0x198
    15:0    RW  rx_crc_init6[31:16]             0x0
 */
typedef union _PRO_RX_CRC_INIT6_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_crc_init6_31_16;
    };
} PRO_RX_CRC_INIT6_UPPER_TYPE;

/* 0x19a
    15:0    RW  rx_crc_init7[15:0]              0x0
 */
typedef union _PRO_RX_CRC_INIT7_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_crc_init7_15_0;
    };
} PRO_RX_CRC_INIT7_LOWER_TYPE;

/* 0x19c
    15:0    RW  rx_crc_init7[31:16]             0x0
 */
typedef union _PRO_RX_CRC_INIT7_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_crc_init7_31_16;
    };
} PRO_RX_CRC_INIT7_UPPER_TYPE;

/* 0x19e
    15:0    RW  white_init3[15:0]               0x0
 */
typedef union _PRO_WHITE_INIT3_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_init3_15_0;
    };
} PRO_WHITE_INIT3_LOWER_TYPE;

/* 0x1a0
    15:0    RW  white_init3[31:16]              0x0
 */
typedef union _PRO_WHITE_INIT3_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_init3_31_16;
    };
} PRO_WHITE_INIT3_UPPER_TYPE;

/* 0x1a2
    15:0    RW  white_init4[15:0]               0x0
 */
typedef union _PRO_WHITE_INIT4_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_init4_15_0;
    };
} PRO_WHITE_INIT4_LOWER_TYPE;

/* 0x1a4
    15:0    RW  white_init4[31:16]              0x0
 */
typedef union _PRO_WHITE_INIT4_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_init4_31_16;
    };
} PRO_WHITE_INIT4_UPPER_TYPE;

/* 0x1a6
    15:0    RW  white_init5[15:0]               0x0
 */
typedef union _PRO_WHITE_INIT5_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_init5_15_0;
    };
} PRO_WHITE_INIT5_LOWER_TYPE;

/* 0x1a8
    15:0    RW  white_init5[31:16]              0x0
 */
typedef union _PRO_WHITE_INIT5_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_init5_31_16;
    };
} PRO_WHITE_INIT5_UPPER_TYPE;

/* 0x1aa
    15:0    RW  white_init6[15:0]               0x0
 */
typedef union _PRO_WHITE_INIT6_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_init6_15_0;
    };
} PRO_WHITE_INIT6_LOWER_TYPE;

/* 0x1ac
    15:0    RW  white_init6[31:16]              0x0
 */
typedef union _PRO_WHITE_INIT6_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_init6_31_16;
    };
} PRO_WHITE_INIT6_UPPER_TYPE;

/* 0x1ae
    15:0    RW  white_init7[15:0]               0x0
 */
typedef union _PRO_WHITE_INIT7_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_init7_15_0;
    };
} PRO_WHITE_INIT7_LOWER_TYPE;

/* 0x1b0
    15:0    RW  white_init7[31:16]              0x0
 */
typedef union _PRO_WHITE_INIT7_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_init7_31_16;
    };
} PRO_WHITE_INIT7_UPPER_TYPE;

/* 0x1b2
    15:0    RW  white_polynomial_reg3[15:0]     0x0
 */
typedef union _PRO_WHITE_POLYNOMIAL_REG3_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_polynomial_reg3_15_0;
    };
} PRO_WHITE_POLYNOMIAL_REG3_LOWER_TYPE;

/* 0x1b4
    15:0    RW  white_polynomial_reg3[31:16]    0x0
 */
typedef union _PRO_WHITE_POLYNOMIAL_REG3_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_polynomial_reg3_31_16;
    };
} PRO_WHITE_POLYNOMIAL_REG3_UPPER_TYPE;

/* 0x1b6
    15:0    RW  white_polynomial_reg4[15:0]     0x0
 */
typedef union _PRO_WHITE_POLYNOMIAL_REG4_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_polynomial_reg4_15_0;
    };
} PRO_WHITE_POLYNOMIAL_REG4_LOWER_TYPE;

/* 0x1b8
    15:0    RW  white_polynomial_reg4[31:16]    0x0
 */
typedef union _PRO_WHITE_POLYNOMIAL_REG4_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_polynomial_reg4_31_16;
    };
} PRO_WHITE_POLYNOMIAL_REG4_UPPER_TYPE;

/* 0x1ba
    15:0    RW  white_polynomial_reg5[15:0]     0x0
 */
typedef union _PRO_WHITE_POLYNOMIAL_REG5_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_polynomial_reg5_15_0;
    };
} PRO_WHITE_POLYNOMIAL_REG5_LOWER_TYPE;

/* 0x1bc
    15:0    RW  white_polynomial_reg5[31:16]    0x0
 */
typedef union _PRO_WHITE_POLYNOMIAL_REG5_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_polynomial_reg5_31_16;
    };
} PRO_WHITE_POLYNOMIAL_REG5_UPPER_TYPE;

/* 0x1be
    15:0    RW  white_polynomial_reg6[15:0]     0x0
 */
typedef union _PRO_WHITE_POLYNOMIAL_REG6_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_polynomial_reg6_15_0;
    };
} PRO_WHITE_POLYNOMIAL_REG6_LOWER_TYPE;

/* 0x1c0
    15:0    RW  white_polynomial_reg6[31:16]    0x0
 */
typedef union _PRO_WHITE_POLYNOMIAL_REG6_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_polynomial_reg6_31_16;
    };
} PRO_WHITE_POLYNOMIAL_REG6_UPPER_TYPE;

/* 0x1c2
    15:0    RW  white_polynomial_reg7[15:0]     0x0
 */
typedef union _PRO_WHITE_POLYNOMIAL_REG7_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_polynomial_reg7_15_0;
    };
} PRO_WHITE_POLYNOMIAL_REG7_LOWER_TYPE;

/* 0x1c4
    15:0    RW  white_polynomial_reg7[31:16]    0x0
 */
typedef union _PRO_WHITE_POLYNOMIAL_REG7_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_polynomial_reg7_31_16;
    };
} PRO_WHITE_POLYNOMIAL_REG7_UPPER_TYPE;

/* 0x1c6
    15:0    RW  crc_polynomial_reg3[15:0]       0x0
 */
typedef union _PRO_CRC_POLYNOMIAL_REG3_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_polynomial_reg3_15_0;
    };
} PRO_CRC_POLYNOMIAL_REG3_LOWER_TYPE;

/* 0x1c8
    15:0    RW  crc_polynomial_reg3[31:16]      0x0
 */
typedef union _PRO_CRC_POLYNOMIAL_REG3_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_polynomial_reg3_31_16;
    };
} PRO_CRC_POLYNOMIAL_REG3_UPPER_TYPE;

/* 0x1ca
    15:0    RW  crc_polynomial_reg4[15:0]       0x0
 */
typedef union _PRO_CRC_POLYNOMIAL_REG4_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_polynomial_reg4_15_0;
    };
} PRO_CRC_POLYNOMIAL_REG4_LOWER_TYPE;

/* 0x1cc
    15:0    RW  crc_polynomial_reg4[31:16]      0x0
 */
typedef union _PRO_CRC_POLYNOMIAL_REG4_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_polynomial_reg4_31_16;
    };
} PRO_CRC_POLYNOMIAL_REG4_UPPER_TYPE;

/* 0x1ce
    15:0    RW  crc_polynomial_reg5[15:0]       0x0
 */
typedef union _PRO_CRC_POLYNOMIAL_REG5_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_polynomial_reg5_15_0;
    };
} PRO_CRC_POLYNOMIAL_REG5_LOWER_TYPE;

/* 0x1d0
    15:0    RW  crc_polynomial_reg5[31:16]      0x0
 */
typedef union _PRO_CRC_POLYNOMIAL_REG5_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_polynomial_reg5_31_16;
    };
} PRO_CRC_POLYNOMIAL_REG5_UPPER_TYPE;

/* 0x1d2
    15:0    RW  crc_polynomial_reg6[15:0]       0x0
 */
typedef union _PRO_CRC_POLYNOMIAL_REG6_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_polynomial_reg6_15_0;
    };
} PRO_CRC_POLYNOMIAL_REG6_LOWER_TYPE;

/* 0x1d4
    15:0    RW  crc_polynomial_reg6[31:16]      0x0
 */
typedef union _PRO_CRC_POLYNOMIAL_REG6_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_polynomial_reg6_31_16;
    };
} PRO_CRC_POLYNOMIAL_REG6_UPPER_TYPE;

/* 0x1d6
    15:0    RW  crc_polynomial_reg7[15:0]       0x0
 */
typedef union _PRO_CRC_POLYNOMIAL_REG7_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_polynomial_reg7_15_0;
    };
} PRO_CRC_POLYNOMIAL_REG7_LOWER_TYPE;

/* 0x1d8
    15:0    RW  crc_polynomial_reg7[31:16]      0x0
 */
typedef union _PRO_CRC_POLYNOMIAL_REG7_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t crc_polynomial_reg7_31_16;
    };
} PRO_CRC_POLYNOMIAL_REG7_UPPER_TYPE;

/* 0x1da
    7:0     RO  rx_pkt_num2                     0x0
    15:8    RO  rx_pkt_num3                     0x0
 */
typedef union _PRO_RX_PKT_NUM0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_pkt_num2: 8;
        uint16_t rx_pkt_num3: 8;
    };
} PRO_RX_PKT_NUM0_TYPE;

/* 0x1dc
    7:0     RO  rx_pkt_num4                     0x0
    15:8    RO  rx_pkt_num5                     0x0
 */
typedef union _PRO_RX_PKT_NUM1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_pkt_num4: 8;
        uint16_t rx_pkt_num5: 8;
    };
} PRO_RX_PKT_NUM1_TYPE;

/* 0x1de
    7:0     RO  rx_pkt_num6                     0x0
    15:8    RO  rx_pkt_num7                     0x0
 */
typedef union _PRO_RX_PKT_NUM2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rx_pkt_num6: 8;
        uint16_t rx_pkt_num7: 8;
    };
} PRO_RX_PKT_NUM2_TYPE;

/* 0x1e0
    7:0     RO  tx_pkt_num3                     0x0
    15:8    RO  tx_pkt_num4                     0x0
 */
typedef union _PRO_TX_PKT_NUM3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_pkt_num3: 8;
        uint16_t tx_pkt_num4: 8;
    };
} PRO_TX_PKT_NUM3_TYPE;

/* 0x1e2
    7:0     RO  tx_pkt_num5                     0x0
    15:8    RO  tx_pkt_num6                     0x0
 */
typedef union _PRO_TX_PKT_NUM4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_pkt_num5: 8;
        uint16_t tx_pkt_num6: 8;
    };
} PRO_TX_PKT_NUM4_TYPE;

/* 0x1e4
    7:0     RO  tx_pkt_num7                     0x0
    15:8    RO  rsvd                            0x0
 */
typedef union _PRO_TX_PKT_NUM5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_pkt_num7: 8;
        uint16_t rsvd: 8;
    };
} PRO_TX_PKT_NUM5_TYPE;

/* 0x1e6
    5:0     RW  white_field_length3             0x0
    6       RW  reset_rx_pktnum2                0x0
    7       RW  lock3                           0x0
    8       RW  no_tx3                          0x0
    9       RW  auto_no_tx_option3              0x0
    10      RW  clear_hw_no_tx3                 0x0
    11      RO  read_clear_hw_no_tx3            0x0
    12      RW  empty_flag3                     0x0
    13      RW  fw_ack3                         0x0
    14      RW  tx_phy3                         0x0
    15      RW  reset_rx_pktnum3                0x0
 */
typedef union _PRO_WHITE_FIELD_LENGTH_0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_field_length3: 6;
        uint16_t reset_rx_pktnum2: 1;
        uint16_t lock3: 1;
        uint16_t no_tx3: 1;
        uint16_t auto_no_tx_option3: 1;
        uint16_t clear_hw_no_tx3: 1;
        uint16_t read_clear_hw_no_tx3: 1;
        uint16_t empty_flag3: 1;
        uint16_t fw_ack3: 1;
        uint16_t tx_phy3: 1;
        uint16_t reset_rx_pktnum3: 1;
    };
} PRO_WHITE_FIELD_LENGTH_0_TYPE;

/* 0x1e8
    5:0     RW  white_field_length4             0x0
    6       RO  rsvd                            0x0
    7       RW  lock4                           0x0
    8       RW  no_tx4                          0x0
    9       RW  auto_no_tx_option4              0x0
    10      RW  clear_hw_no_tx4                 0x0
    11      RO  read_clear_hw_no_tx4            0x0
    12      RW  empty_flag4                     0x0
    13      RW  fw_ack4                         0x0
    14      RW  tx_phy4                         0x0
    15      RW  reset_rx_pktnum4                0x0
 */
typedef union _PRO_WHITE_FIELD_LENGTH_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_field_length4: 6;
        uint16_t rsvd: 1;
        uint16_t lock4: 1;
        uint16_t no_tx4: 1;
        uint16_t auto_no_tx_option4: 1;
        uint16_t clear_hw_no_tx4: 1;
        uint16_t read_clear_hw_no_tx4: 1;
        uint16_t empty_flag4: 1;
        uint16_t fw_ack4: 1;
        uint16_t tx_phy4: 1;
        uint16_t reset_rx_pktnum4: 1;
    };
} PRO_WHITE_FIELD_LENGTH_1_TYPE;

/* 0x1ea
    5:0     RW  white_field_length5             0x0
    6       RO  rsvd                            0x0
    7       RW  lock5                           0x0
    8       RW  no_tx5                          0x0
    9       RW  auto_no_tx_option5              0x0
    10      RW  clear_hw_no_tx5                 0x0
    11      RO  read_clear_hw_no_tx5            0x0
    12      RW  empty_flag5                     0x0
    13      RW  fw_ack5                         0x0
    14      RW  tx_phy5                         0x0
    15      RW  reset_rx_pktnum5                0x0
 */
typedef union _PRO_WHITE_FIELD_LENGTH_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_field_length5: 6;
        uint16_t rsvd: 1;
        uint16_t lock5: 1;
        uint16_t no_tx5: 1;
        uint16_t auto_no_tx_option5: 1;
        uint16_t clear_hw_no_tx5: 1;
        uint16_t read_clear_hw_no_tx5: 1;
        uint16_t empty_flag5: 1;
        uint16_t fw_ack5: 1;
        uint16_t tx_phy5: 1;
        uint16_t reset_rx_pktnum5: 1;
    };
} PRO_WHITE_FIELD_LENGTH_2_TYPE;

/* 0x1ec
    5:0     RW  white_field_length6             0x0
    6       RO  rsvd                            0x0
    7       RW  lock6                           0x0
    8       RW  no_tx6                          0x0
    9       RW  auto_no_tx_option6              0x0
    10      RW  clear_hw_no_tx6                 0x0
    11      RO  read_clear_hw_no_tx6            0x0
    12      RW  empty_flag6                     0x0
    13      RW  fw_ack6                         0x0
    14      RW  tx_phy6                         0x0
    15      RW  reset_rx_pktnum6                0x0
 */
typedef union _PRO_WHITE_FIELD_LENGTH_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_field_length6: 6;
        uint16_t rsvd: 1;
        uint16_t lock6: 1;
        uint16_t no_tx6: 1;
        uint16_t auto_no_tx_option6: 1;
        uint16_t clear_hw_no_tx6: 1;
        uint16_t read_clear_hw_no_tx6: 1;
        uint16_t empty_flag6: 1;
        uint16_t fw_ack6: 1;
        uint16_t tx_phy6: 1;
        uint16_t reset_rx_pktnum6: 1;
    };
} PRO_WHITE_FIELD_LENGTH_3_TYPE;

/* 0x1ee
    5:0     RW  white_field_length7             0x0
    6       RO  rsvd                            0x0
    7       RW  lock7                           0x0
    8       RW  no_tx7                          0x0
    9       RW  auto_no_tx_option7              0x0
    10      RW  clear_hw_no_tx7                 0x0
    11      RO  read_clear_hw_no_tx7            0x0
    12      RW  empty_flag7                     0x0
    13      RW  fw_ack7                         0x0
    14      RW  tx_phy7                         0x0
    15      RW  reset_rx_pktnum7                0x0
 */
typedef union _PRO_WHITE_FIELD_LENGTH_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t white_field_length7: 6;
        uint16_t rsvd: 1;
        uint16_t lock7: 1;
        uint16_t no_tx7: 1;
        uint16_t auto_no_tx_option7: 1;
        uint16_t clear_hw_no_tx7: 1;
        uint16_t read_clear_hw_no_tx7: 1;
        uint16_t empty_flag7: 1;
        uint16_t fw_ack7: 1;
        uint16_t tx_phy7: 1;
        uint16_t reset_rx_pktnum7: 1;
    };
} PRO_WHITE_FIELD_LENGTH_4_TYPE;

/* 0x1f0
    2:0     RW  preamble_field_length3[2:0]     0x0
    3       RW  clear_hw_rd_ptr3                0x0
    4       RO  read_clear_hw_rd_ptr3           0x0
    5       RW  auto_add_rptr_option3           0x0
    8:6     RW  fw_rd_ptr3                      0x0
    9       RW  auto_no_rdptr_option3           0x0
    10      RW  reset_tx_pktnum3                0x0
    15:11   RO  rsvd                            0x0
 */
typedef union _PRO_FIELD_LENGTH3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t preamble_field_length3_2_0: 3;
        uint16_t clear_hw_rd_ptr3: 1;
        uint16_t read_clear_hw_rd_ptr3: 1;
        uint16_t auto_add_rptr_option3: 1;
        uint16_t fw_rd_ptr3: 3;
        uint16_t auto_no_rdptr_option3: 1;
        uint16_t reset_tx_pktnum3: 1;
        uint16_t rsvd: 5;
    };
} PRO_FIELD_LENGTH3_TYPE;

/* 0x1f2
    2:0     RW  preamble_field_length4[2:0]     0x0
    3       RW  clear_hw_rd_ptr4                0x0
    4       RO  read_clear_hw_rd_ptr4           0x0
    5       RW  auto_add_rptr_option4           0x0
    8:6     RW  fw_rd_ptr4                      0x0
    9       RW  auto_no_rdptr_option4           0x0
    10      RW  reset_tx_pktnum4                0x0
    15:11   RO  rsvd                            0x0
 */
typedef union _PRO_FIELD_LENGTH4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t preamble_field_length4_2_0: 3;
        uint16_t clear_hw_rd_ptr4: 1;
        uint16_t read_clear_hw_rd_ptr4: 1;
        uint16_t auto_add_rptr_option4: 1;
        uint16_t fw_rd_ptr4: 3;
        uint16_t auto_no_rdptr_option4: 1;
        uint16_t reset_tx_pktnum4: 1;
        uint16_t rsvd: 5;
    };
} PRO_FIELD_LENGTH4_TYPE;

/* 0x1f4
    2:0     RW  preamble_field_length5[2:0]     0x0
    3       RW  clear_hw_rd_ptr5                0x0
    4       RO  read_clear_hw_rd_ptr5           0x0
    5       RW  auto_add_rptr_option5           0x0
    8:6     RW  fw_rd_ptr5                      0x0
    9       RW  auto_no_rdptr_option5           0x0
    10      RW  reset_tx_pktnum5                0x0
    15:11   RO  rsvd                            0x0
 */
typedef union _PRO_FIELD_LENGTH5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t preamble_field_length5_2_0: 3;
        uint16_t clear_hw_rd_ptr5: 1;
        uint16_t read_clear_hw_rd_ptr5: 1;
        uint16_t auto_add_rptr_option5: 1;
        uint16_t fw_rd_ptr5: 3;
        uint16_t auto_no_rdptr_option5: 1;
        uint16_t reset_tx_pktnum5: 1;
        uint16_t rsvd: 5;
    };
} PRO_FIELD_LENGTH5_TYPE;

/* 0x1f6
    2:0     RW  preamble_field_length6[2:0]     0x0
    3       RW  clear_hw_rd_ptr6                0x0
    4       RO  read_clear_hw_rd_ptr6           0x0
    5       RW  auto_add_rptr_option6           0x0
    8:6     RW  fw_rd_ptr6                      0x0
    9       RW  auto_no_rdptr_option6           0x0
    10      RW  reset_tx_pktnum6                0x0
    15:11   RO  rsvd                            0x0
 */
typedef union _PRO_FIELD_LENGTH6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t preamble_field_length6_2_0: 3;
        uint16_t clear_hw_rd_ptr6: 1;
        uint16_t read_clear_hw_rd_ptr6: 1;
        uint16_t auto_add_rptr_option6: 1;
        uint16_t fw_rd_ptr6: 3;
        uint16_t auto_no_rdptr_option6: 1;
        uint16_t reset_tx_pktnum6: 1;
        uint16_t rsvd: 5;
    };
} PRO_FIELD_LENGTH6_TYPE;

/* 0x1f8
    2:0     RW  preamble_field_length7[2:0]     0x0
    3       RW  clear_hw_rd_ptr7                0x0
    4       RO  read_clear_hw_rd_ptr7           0x0
    5       RW  auto_add_rptr_option7           0x0
    8:6     RW  fw_rd_ptr7                      0x0
    9       RW  auto_no_rdptr_option7           0x0
    10      RW  reset_tx_pktnum7                0x0
    15:11   RO  rsvd                            0x0
 */
typedef union _PRO_FIELD_LENGTH7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t preamble_field_length7_2_0: 3;
        uint16_t clear_hw_rd_ptr7: 1;
        uint16_t read_clear_hw_rd_ptr7: 1;
        uint16_t auto_add_rptr_option7: 1;
        uint16_t fw_rd_ptr7: 3;
        uint16_t auto_no_rdptr_option7: 1;
        uint16_t reset_tx_pktnum7: 1;
        uint16_t rsvd: 5;
    };
} PRO_FIELD_LENGTH7_TYPE;

/* 0x1fa
    15:0    RW  tx_address1[15:0]               0x0
 */
typedef union _PRO_TX_ADDRESS1_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address1_15_0;
    };
} PRO_TX_ADDRESS1_LOWER_TYPE;

/* 0x1fc
    15:0    RW  tx_address1[31:16]              0x0
 */
typedef union _PRO_TX_ADDRESS1_MIDDLE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address1_31_16;
    };
} PRO_TX_ADDRESS1_MIDDLE_TYPE;

/* 0x1fe
    7:0     RW  tx_address1[39:32]              0x0
    15:8    RO  rsvd                            0x0
 */
typedef union _PRO_TX_ADDRESS1_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address1_39_32: 8;
        uint16_t rsvd: 8;
    };
} PRO_TX_ADDRESS1_UPPER_TYPE;

/* 0x200
    15:0    RW  tx_address3[15:0]               0x0
 */
typedef union _PRO_TX_ADDRESS3_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address3_15_0;
    };
} PRO_TX_ADDRESS3_LOWER_TYPE;

/* 0x202
    15:0    RW  tx_address3[31:16]              0x0
 */
typedef union _PRO_TX_ADDRESS3_MIDDLE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address3_31_16;
    };
} PRO_TX_ADDRESS3_MIDDLE_TYPE;

/* 0x204
    7:0     RW  tx_address3[39:32]              0x0
    15:8    RO  rsvd                            0x0
 */
typedef union _PRO_TX_ADDRESS3_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address3_39_32: 8;
        uint16_t rsvd: 8;
    };
} PRO_TX_ADDRESS3_UPPER_TYPE;

/* 0x206
    15:0    RW  tx_address4[15:0]               0x0
 */
typedef union _PRO_TX_ADDRESS4_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address4_15_0;
    };
} PRO_TX_ADDRESS4_LOWER_TYPE;

/* 0x208
    15:0    RW  tx_address4[31:16]              0x0
 */
typedef union _PRO_TX_ADDRESS4_MIDDLE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address4_31_16;
    };
} PRO_TX_ADDRESS4_MIDDLE_TYPE;

/* 0x20a
    7:0     RW  tx_address4[39:32]              0x0
    15:8    RO  rsvd                            0x0
 */
typedef union _PRO_TX_ADDRESS4_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address4_39_32: 8;
        uint16_t rsvd: 8;
    };
} PRO_TX_ADDRESS4_UPPER_TYPE;

/* 0x20c
    15:0    RW  tx_address5[15:0]               0x0
 */
typedef union _PRO_TX_ADDRESS5_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address5_15_0;
    };
} PRO_TX_ADDRESS5_LOWER_TYPE;

/* 0x20e
    15:0    RW  tx_address5[31:16]              0x0
 */
typedef union _PRO_TX_ADDRESS5_MIDDLE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address5_31_16;
    };
} PRO_TX_ADDRESS5_MIDDLE_TYPE;

/* 0x210
    7:0     RW  tx_address5[39:32]              0x0
    15:8    RO  rsvd                            0x0
 */
typedef union _PRO_TX_ADDRESS5_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address5_39_32: 8;
        uint16_t rsvd: 8;
    };
} PRO_TX_ADDRESS5_UPPER_TYPE;

/* 0x212
    15:0    RW  tx_address6[15:0]               0x0
 */
typedef union _PRO_TX_ADDRESS6_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address6_15_0;
    };
} PRO_TX_ADDRESS6_LOWER_TYPE;

/* 0x214
    15:0    RW  tx_address6[31:16]              0x0
 */
typedef union _PRO_TX_ADDRESS6_MIDDLE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address6_31_16;
    };
} PRO_TX_ADDRESS6_MIDDLE_TYPE;

/* 0x216
    7:0     RW  tx_address6[39:32]              0x0
    15:8    RO  rsvd                            0x0
 */
typedef union _PRO_TX_ADDRESS6_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address6_39_32: 8;
        uint16_t rsvd: 8;
    };
} PRO_TX_ADDRESS6_UPPER_TYPE;

/* 0x218
    15:0    RW  tx_address7[15:0]               0x0
 */
typedef union _PRO_TX_ADDRESS7_LOWER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address7_15_0;
    };
} PRO_TX_ADDRESS7_LOWER_TYPE;

/* 0x21a
    15:0    RW  tx_address7[31:16]              0x0
 */
typedef union _PRO_TX_ADDRESS7_MIDDLE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address7_31_16;
    };
} PRO_TX_ADDRESS7_MIDDLE_TYPE;

/* 0x21c
    7:0     RW  tx_address7[39:32]              0x0
    15:8    RO  rsvd                            0x0
 */
typedef union _PRO_TX_ADDRESS7_UPPER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t tx_address7_39_32: 8;
        uint16_t rsvd: 8;
    };
} PRO_TX_ADDRESS7_UPPER_TYPE;

/* 0x21e
    2:0     RO  rd_ptr0[2:0]                    0x0
    5:3     RO  rd_ptr1[2:0]                    0x0
    8:6     RO  rd_ptr2[2:0]                    0x0
    11:9    RO  rd_ptr3[2:0]                    0x0
    14:12   RO  rd_ptr4[2:0]                    0x0
    15      RO  rsvd                            0x0
 */
typedef union _PRO_RD_PTR_0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rd_ptr0_2_0: 3;
        uint16_t rd_ptr1_2_0: 3;
        uint16_t rd_ptr2_2_0: 3;
        uint16_t rd_ptr3_2_0: 3;
        uint16_t rd_ptr4_2_0: 3;
        uint16_t rsvd: 1;
    };
} PRO_RD_PTR_0_TYPE;

/* 0x220
    2:0     RO  rd_ptr5[2:0]                    0x0
    5:3     RO  rd_ptr6[2:0]                    0x0
    8:6     RO  rd_ptr7[2:0]                    0x0
    15:9    RO  rsvd                            0x0
 */
typedef union _PRO_RD_PTR_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rd_ptr5_2_0: 3;
        uint16_t rd_ptr6_2_0: 3;
        uint16_t rd_ptr7_2_0: 3;
        uint16_t rsvd: 7;
    };
} PRO_RD_PTR_1_TYPE;

/** @} */

/** @} End of PPT_Register */
