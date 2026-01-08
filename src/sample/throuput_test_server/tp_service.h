#ifndef _TP_SERVICE_H_
#define _TP_SERVICE_H_

#ifdef __cplusplus
extern "C"  {
#endif

#include "profile_server.h"

#define GATT_UUID_VENDOR_TP_SERVICE                     0xA00D
#define CT_TX                                           0xB001
#define CT_RX                                           0xB002
#define DT_TX                                           0xB003
#define DT_RX                                           0xB004

#define VENDOR_TP_SERVICE_CHAR_CT_TX_INDEX              0x02
#define VENDOR_TP_SERVICE_CHAR_CT_TX_CCCD_INDEX (VENDOR_TP_SERVICE_CHAR_CT_TX_INDEX + 1)
#define VENDOR_TP_SERVICE_CHAR_CT_RX_INDEX              0x05
#define VENDOR_TP_SERVICE_CHAR_DT_TX_INDEX              0x07
#define VENDOR_TP_SERVICE_CHAR_DT_TX_CCCD_INDEX (VENDOR_TP_SERVICE_CHAR_DT_TX_INDEX + 1)
#define VENDOR_TP_SERVICE_CHAR_DT_RX_INDEX              0x0a

typedef struct
{
    uint8_t                     *p_value;
    uint16_t                    value_len;
} T_TP_WRITE_DATA;

typedef struct
{
    uint16_t                    cccbits;
} T_TP_CCCD_DATA;

typedef struct
{
    uint8_t                     conn_id;
    uint16_t                    index;
    T_TP_WRITE_DATA             write_data;
    T_TP_CCCD_DATA              cccd_data;
} T_TP_CALLBACK_DATA;

extern bool ind_pending_flag;

T_SERVER_ID vendor_tp_service_add(void *p_func);
bool vendor_tp_service_indication(uint8_t conn_id, T_SERVER_ID service_id, void *p_value,
                                  uint16_t length);
bool vendor_tp_service_notification(uint8_t conn_id, T_SERVER_ID service_id, void *p_value,
                                    uint16_t length);

#ifdef __cplusplus
}
#endif

#endif
