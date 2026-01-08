/**
 * @file at_zigbtest_mac_wrapper.h
 * @brief MAC test wrapper functions header
 * @author
 * @date 2025-09-11
 */

#ifndef AT_ZIGBTEST_MAC_WRAPPER_H
#define AT_ZIGBTEST_MAC_WRAPPER_H

#include <stdint.h>

// MAC test wrapper functions
int mac_test_set_channel(uint8_t channel);
int mac_test_set_tx_power(int8_t power_dbm);
int mac_test_start_tx_stream(void);
int mac_test_start_tx_tone(void);
int mac_test_stop_tx(void);
int mac_test_start_rx(void);
int mac_test_stop_rx(void);
int mac_test_reset_rx_stats(void);
int mac_test_get_rx_stats(uint32_t *good_count, uint32_t *error_count);

// Initialization
void mac_test_wrapper_init(void);

#endif // AT_ZIGBTEST_MAC_WRAPPER_H
