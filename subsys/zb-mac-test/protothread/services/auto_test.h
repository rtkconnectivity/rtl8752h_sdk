/**
*****************************************************************************************
*     Copyright(c) 2024 - 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      auto_test.h
   * @brief     Header file for auto test function implementation
   * @author    felix
   * @date      2025-01-17
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2024 - 2025 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

#ifndef _AUTO_TEST_H_
#define _AUTO_TEST_H_

#define AT_NOTIFY_EV_ACK_TO 1   // sub event: ack timeout
#define AT_NOTIFY_EV_RX 2       // sub event: packet rx

#if TEST_AUTO_TEST_EN
void auto_test_init(void);
void auto_test_enable(void);
void auto_test_disable(void);
bool auto_test_is_enable(void);
void at_send_first(uint8_t len, void *pvalue);
void at_send_more(uint8_t len, void *pvalue);
void at_send_last(uint8_t len, void *pvalue);
void at_send(uint8_t len, void *pvalue);
void at_send_enh_ack_timeout(void);
#else
static inline void auto_test_init(void) { return; }
static inline void auto_test_enable(void) { return; }
static inline void auto_test_disable(void) { return; }
static inline bool auto_test_is_enable(void) { return 0; }
static inline void at_send_first(uint8_t len, void *pvalue) { return; }
static inline void at_send_more(uint8_t len, void *pvalue) { return; }
static inline void at_send_last(uint8_t len, void *pvalue) { return; }
static inline void at_send(uint8_t len, void *pvalue) { return; }
static inline void at_send_enh_ack_timeout(void) { return; }
#endif /* TEST_AUTO_TEST_EN */

/*
#define at_send_and_exit(len, pvalue) \
    { \
        at_send(len, pvalue); \
        return TRUE; \
    }
#define at_send_last_and_exit(len, pvalue) \
    { \
        at_send_last(len, pvalue); \
        return TRUE; \
    }
*/
#endif /* _AUTO_TEST_H_ */
