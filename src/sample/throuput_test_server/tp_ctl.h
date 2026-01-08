#ifndef _TP_CTL_H_
#define _TP_CTL_H_

#include <stdint.h>
#include "profile_server.h"

extern T_SERVER_ID tp_svc_id;

extern uint8_t para_mode;
extern uint8_t leng_mode;
extern uint8_t phy_mode;

void tp_data_init(void);
void tp_update_complete(uint8_t conn_id, uint16_t credit);
void tp_recv_ct_rx(uint8_t conn_id, uint8_t *p_value, uint16_t length);
void tp_recv_dt_rx(uint8_t conn_id, uint8_t *p_value, uint16_t length);
void update_conn_para_callback(uint8_t conn_id);

#endif
