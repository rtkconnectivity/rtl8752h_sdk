/**************************************************************************//**
 * @file      pt_cmd_sec.c
 * @brief     Source file for security command implementation
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
#include "strproc.h"
#include "mac_test_common.h"
#include "mac_driver_interface.h"
#include "services/auto_test.h"

static int cmd_upper_sec_test(int argc, char *argv[])
{
    nonce_t nonce = {0};
    uint8_t data_len, max_data_len = MAC_MAX_TX_FRM_LEN;
    uint8_t sec_level = SEC_ENC;
    uint8_t unnecessary_enc_len = 0;
    pmac_txfifo_t pTxNFIFO = (pmac_txfifo_t)MAC_TXN_BASE_ADDR;
    tx_buf_t ciphertext_buf;

    if (argc > 0)
    {
        sec_level = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
    }

    uint8_t mic_len = mac_sec_mic_len_get(sec_level);
    max_data_len -= mic_len;
    data_len = max_data_len; // default data_len
    if (argc > 1)
    {
        data_len = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
    }

    if ((data_len > max_data_len || data_len < MAC_MIN_TX_FRM_LEN) ||
        (sec_level > SEC_ENC_MIC_128))
    {
        dbg_printf("Error: InvalidArgs\r\n");
        return FALSE;
    }

    dbg_printf("Start enc/dec test. data_len %u sec_level %u\r\n", data_len, sec_level);
    dbg_printf("Generate sequential test data\r\n");
    dbg_printf("plaintext:\r\n");
    BUF_RESET(g_tx_buf);
    GEN_SEQ_DATA_MV_PTR(g_tx_buf.buf, 1, data_len, g_tx_buf.len);
    dbg_mem_dump((const uint8_t *)g_tx_buf.buf, g_tx_buf.len);

    // prepare nonce
    nonce.sec_level = sec_level;
    nonce.frame_counter = 5;
    memcpy(&nonce.src_ext_addr, mac_long_addr_get(), sizeof(nonce.src_ext_addr));

    // load plaintext data
    mac_txn_payload_set(unnecessary_enc_len, g_tx_buf.len, g_tx_buf.buf);
#if 0
    mac_nonce_set((uint8_t *)&nonce);
    mac_txn_key_set(g_mac_key);
    mac_txn_cipher_set(nonce.sec_level);
    mac_upper_enc_trig(); // do encrypt
#else
    //mac_cs_enter();
    uint8_t ret = mac_upper_cipher(nonce.sec_level, g_mac_key, (uint8_t *)&nonce);
    //mac_cs_exit();
    if (ret != MAC_STS_SUCCESS && ret != MAC_STS_SECURITY_FAILED)
    {
        dbg_printf("mac_UpperCipher fail. ret %u\r\n", ret);
    }
#endif
    // store encrypted data with MIC
    dbg_printf("\r\ncipher text: %s", mic_len == 0 ? "\r\n" : "MIC-");
    dbg_mem_dump(pTxNFIFO->payload + pTxNFIFO->frm_len - mic_len, mic_len);
    dbg_mem_dump(pTxNFIFO->payload, pTxNFIFO->frm_len - mic_len);

    BUF_RESET(ciphertext_buf);
    CPY_MV_PTR(ciphertext_buf.buf, pTxNFIFO->payload, pTxNFIFO->frm_len, ciphertext_buf.len);

    // load ciphertext data
    mac_txn_payload_set(unnecessary_enc_len, ciphertext_buf.len, ciphertext_buf.buf);
#if 0
    mac_nonce_set((uint8_t *)&nonce);
    mac_txn_key_set(g_mac_key);
    mac_txn_cipher_set(nonce.sec_level);
    /* add crc-16 length, to make the security
       engine can fetch MIC from correct offset */
    if (mic_len)
    {
        pTxNFIFO->frm_len += 2;
    }
    mac_upper_dec_trig(); // do decrypt
    //restore frame length
    if (mic_len)
    {
        pTxNFIFO->frm_len -= 2;
    }
#else
    //mac_cs_enter();
    ret = mac_upper_decipher(nonce.sec_level, g_mac_key, (uint8_t *)&nonce);
    //mac_cs_exit();
    if (ret != MAC_STS_SUCCESS)
    {
        dbg_printf("mac_UpperDecipher fail. ret %u\r\n", ret);
    }
#endif

    dbg_printf("\r\ndecipher plain text: %s", mic_len == 0 ? "\r\n" : "MIC-");
    dbg_mem_dump(pTxNFIFO->payload + pTxNFIFO->frm_len - mic_len, mic_len);
    dbg_mem_dump(pTxNFIFO->payload, pTxNFIFO->frm_len - mic_len);

    if (memcmp(g_tx_buf.buf, pTxNFIFO->payload, g_tx_buf.len))
    {
        dbg_printf("plain text compare fail\r\n");
        return FALSE;
    }
    if (auto_test_is_enable() == 0)
    {
        dbg_printf("Done\r\n");
    }
    else
    {
        at_send(0, NULL);
    }
    return TRUE;
}

void shell_register_cmd_sec(void)
{
    shell_register_pt((shell_program_t)cmd_upper_sec_test, "upper_sec_test",
                      BRIEF("AES-CCM Security Test")
                      SYNOPSIS("upper_sec_test [<sec_level>] [<data_len>]")
                      DESCRIPTION(" data_len: plaintext data length")
                      DESCRIPTION("sec_level:      data_len max:")
                      DESCRIPTION("  1 - SEC_MIC_32          121")
                      DESCRIPTION("  2 - SEC_MIC_64          117")
                      DESCRIPTION("  3 - SEC_MIC_128         109")
                      DESCRIPTION("  4 - SEC_ENC             125")
                      DESCRIPTION("  5 - SEC_ENC_MIC_32      121")
                      DESCRIPTION("  6 - SEC_ENC_MIC_64      117")
                      DESCRIPTION("  7 - SEC_ENC_MIC_128     109"));
}
