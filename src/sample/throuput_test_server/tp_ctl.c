#include <string.h>
#include "trace.h"
#include "user_cmd.h"
#include "tp_service.h"
#include "os_sched.h"
#include "tp_ctl.h"
#include "gap_conn_le.h"
#include "os_timer.h"
#include "otp_config.h"
#if SYSTEM_TRACE_ENABLE
#include "trace_system_loading.h"
#endif

void *update_para_len_phy_timer_handle = NULL;

uint64_t tx_begin_time;
uint64_t tx_end_time;
uint64_t tx_data_count;
uint32_t tx_data_rate;

uint64_t rx_begin_time;
uint64_t rx_end_time;
uint64_t rx_data_count;
uint32_t rx_data_rate;

//test mode : 1 - tx, 2 - rx, 3 - trx
uint8_t test_mode = 0;
//update parameter result : 1 - reject, 2 - accept with no change, 3 - accept with new params
uint8_t para_mode = 0;
//set data length result : 1 - not support, 2 - accept with no change, 3 - accept with new length
uint8_t leng_mode = 0;
//updata 2m PHY result : 1 - 1m, 2 - 2m;
uint8_t phy_mode = 0;

uint16_t pkt_len;
uint16_t pkt_time;
uint16_t int_min;
uint16_t int_max;
uint8_t len_ext;
uint8_t phy_2m;

uint16_t init_val = 0;

enum tx_state
{
    S_TX_READY,
    S_TX_SEND_CONFIG_RSP,
    S_TX_SEND_NOTIFY,
    S_TX_SEND_TEST_END
};

enum rx_state
{
    S_RX_READY,
    S_RX_RECV_WRITE_CMD,
    S_RX_RECV_TEST_END
};

uint8_t tx_state_machine = S_TX_READY;
uint8_t rx_state_machine = S_RX_READY;


void tp_recv_test_complete(uint8_t conn_id)
{
    rx_state_machine = S_RX_RECV_TEST_END;
    rx_end_time = os_sys_time_get();
}

void tp_recv_dt_rx(uint8_t conn_id, uint8_t *p_value, uint16_t length)
{
    if (rx_state_machine == S_RX_READY)
    {
        rx_state_machine = S_RX_RECV_WRITE_CMD;
        rx_begin_time = os_sys_time_get();
    }
    if (rx_state_machine == S_RX_RECV_WRITE_CMD)
    {
        rx_data_count += length;
    }
}

static void tp_send_mtu_size(uint8_t conn_id)
{
    uint8_t rsp[3];
    uint16_t mtu_size;
    if (le_get_conn_param(GAP_PARAM_CONN_MTU_SIZE, &mtu_size, conn_id) == GAP_CAUSE_SUCCESS)
    {
        APP_PRINT_INFO1("tp_send_mtu_size %d", mtu_size);
        rsp[0] = 0x02;
        memcpy(&rsp[1], &mtu_size, 2);
    }
    else
    {
        APP_PRINT_INFO0("tp_send_mtu_size failed");
    }

    vendor_tp_service_indication(conn_id, tp_svc_id, rsp, sizeof(rsp));
}

void tp_send_test_complete(uint8_t conn_id)
{
    uint8_t rsp[1] = {0x22};
    APP_PRINT_INFO0("tp_send_test_complete");
    vendor_tp_service_indication(conn_id, tp_svc_id, rsp, sizeof(rsp));
}

static uint64_t os_time_get_elapsed(uint64_t begin, uint64_t end)
{
    if (end >= begin)
    {
        return end - begin;
    }
    else
    {
        return ((uint64_t)(0xFFFFFFFFFFFFFFFF) - begin + end);
    }
}

void tp_send_notification(uint8_t conn_id, uint16_t credit)
{
    uint8_t value[250];
    uint64_t cur_time = os_sys_time_get();
    uint64_t elapsed_time = os_time_get_elapsed(tx_begin_time, cur_time);
    APP_PRINT_INFO2("tp_send_notification elapsed_time %d ms pkt_time %d 000ms",
                    (uint32_t)elapsed_time & 0xFFFFFFFF, pkt_time);

    if (elapsed_time < pkt_time * 1000 && rx_state_machine != S_RX_RECV_TEST_END)
    {
        while (credit)
        {
            memset(value, init_val, pkt_len);
            if (vendor_tp_service_notification(conn_id, tp_svc_id, value, pkt_len))
            {
                tx_data_count += pkt_len;
                init_val++;
                credit--;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        tx_end_time = os_sys_time_get();
        tx_state_machine = S_TX_SEND_TEST_END;
        tp_send_test_complete(conn_id);
    }

}

void tp_update_complete(uint8_t conn_id, uint16_t credit)
{
//    APP_PRINT_INFO1("tp_update_complete tx_state_machine=%d", tx_state_machine);
    if (tx_state_machine == S_TX_SEND_CONFIG_RSP)
    {
        if (test_mode == 1 || test_mode == 3)
        {
            tx_begin_time = os_sys_time_get();
            tx_state_machine = S_TX_SEND_NOTIFY;
            tp_send_notification(conn_id, credit);
        }
    }
    if (tx_state_machine == S_TX_SEND_NOTIFY)
    {
        tp_send_notification(conn_id, credit);
    }
    if (tx_state_machine == S_TX_SEND_TEST_END)
    {
    }
}

static void tp_cal_rst(void)
{
    if (test_mode == 1)
    {
        uint64_t elapsed_time = os_time_get_elapsed(tx_begin_time, tx_end_time);
        tx_data_rate = tx_data_count * 1000 / elapsed_time;
    }
    if (test_mode == 2)
    {
        uint64_t elapsed_time = os_time_get_elapsed(rx_begin_time, rx_end_time);
        rx_data_rate = rx_data_count * 1000 / elapsed_time;
    }
    if (test_mode == 3)
    {
        uint64_t begin_time = tx_begin_time > rx_begin_time ? tx_begin_time : rx_begin_time;
        uint64_t end_time = tx_end_time > rx_end_time ? rx_end_time : tx_end_time;
        uint64_t elapsed_time = os_time_get_elapsed(begin_time, end_time);
        tx_data_rate = tx_data_count * 1000 / elapsed_time;
        rx_data_rate = rx_data_count * 1000 / elapsed_time;
    }
    tp_data_init();
}

void tp_data_init(void)
{
    //reset global params;
    tx_begin_time = 0;
    tx_end_time = 0;
    tx_data_count = 0;

    rx_begin_time = 0;
    rx_end_time = 0;
    rx_data_count = 0;

    tx_state_machine = S_TX_READY;
    rx_state_machine = S_RX_READY;
}

static void tp_send_test_report(uint8_t conn_id)
{
    tp_cal_rst();

    uint8_t rsp[9];
    rsp[0] = 0x32;
    memcpy(rsp + 1, &tx_data_rate, 4);
    memcpy(rsp + 5, &rx_data_rate, 4);

    data_uart_print("tx rate (bytes/s) %d\r\n", tx_data_rate);
    data_uart_print("rx rate (bytes/s) %d\r\n", rx_data_rate);

    APP_PRINT_INFO2("tp_send_test_report tx rate %d(bytes/s) rx rate %d(bytes/s)",
                    tx_data_rate, rx_data_rate);

    vendor_tp_service_indication(conn_id, tp_svc_id, rsp, sizeof(rsp));
#if SYSTEM_TRACE_ENABLE
    APP_PRINT_INFO0("CPU loading test end");
    get_system_loading();
#endif
}


static void tp_send_config_rsp(uint8_t conn_id, bool para_accp, uint8_t len_res, uint8_t phy_res)
{
    APP_PRINT_INFO4("tp_send_config_rsp conn_id=%d, para_accp=%d, len_res=%d, phy_res=%d",
                    conn_id, para_accp, len_res, phy_res);

    uint8_t rsp[5];
    rsp[0] = 0x21;

    uint8_t interval[2];
    le_get_conn_param(GAP_PARAM_CONN_INTERVAL, interval, conn_id);
    rsp[2] = interval[0];
    rsp[3] = interval[1];
    rsp[4] = len_res | ((phy_res - 1) << 1);

    if (para_accp == false || len_res != len_ext || phy_res != phy_2m)
    {
        test_mode = 0;
        rsp[1] = 1;
    }
    else
    {
#if SYSTEM_TRACE_ENABLE
        APP_PRINT_INFO0("CPU loading test start");
        reset_system_loading();
#endif
        rsp[1] = 0;
    }

    tx_state_machine = S_TX_SEND_CONFIG_RSP;
    vendor_tp_service_indication(conn_id, tp_svc_id, rsp, sizeof(rsp));
}

void update_conn_para_callback(uint8_t conn_id)
{
    bool para_accp;
    uint8_t len_res;
    uint8_t phy_res;

    os_timer_stop(&update_para_len_phy_timer_handle);

    if (para_mode == 1)
    {
        para_accp = false;
    }
    else
    {
        uint8_t interval[2];
        le_get_conn_param(GAP_PARAM_CONN_INTERVAL, interval, conn_id);
        uint16_t conn_interval = interval[1] << 8 | interval[0];
        if (conn_interval >= int_min && conn_interval <= int_max)
        {
            para_accp = true;
        }
        else
        {
            para_accp = false;
        }
    }

    if (leng_mode == 1)
    {
        len_res = 0;
    }
    else
    {
        len_res = len_ext;
    }

    if (phy_mode == 1)
    {
        phy_res = 1;
    }
    else
    {
        phy_res = 2;
    }

    APP_PRINT_INFO1("update_conn_para_callback para_accp=%d", para_accp);
    tp_send_config_rsp(conn_id, para_accp, len_res, phy_res);
}

void update_para_len_phy_timeout_handler(void *p_handle)
{
    APP_PRINT_INFO0("update_para_len_phy_timeout_handler");
    update_conn_para_callback(0);
}

static void tp_recv_params_config(uint8_t opcode, uint8_t conn_id, uint8_t *p_value,
                                  uint16_t length)
{
    pkt_len = p_value[2] << 8 | p_value[1];
    pkt_time = p_value[4] << 8 | p_value[3];
    int_min = p_value[6] << 8 | p_value[5];
    int_max = p_value[8] << 8 | p_value[7];
    len_ext = p_value[9] & 0x01;
    phy_2m = ((p_value[9] & 0x02) >> 1) + 1;

    APP_PRINT_INFO7("opcode %d, pkt_len %d, pkt_time %d, int_min %d, int_max %d, len_ext %d, phy_2m %d",
                    opcode, pkt_len, pkt_time, int_min, int_max, len_ext, phy_2m);
    data_uart_print("opcode %d, pkt_len %d, pkt_time %d, int_min %d, int_max %d, len_ext %d, phy_2m %d\r\n",
                    opcode, pkt_len, pkt_time, int_min, int_max, len_ext, phy_2m);

    tx_data_rate = 0;
    rx_data_rate = 0;

    le_update_conn_param(conn_id, int_min, int_max, 0, 500, 2 * (int_min - 1), 2 * (int_max - 1));

    uint16_t tx_octs = len_ext ? 251 : 27;
    le_set_data_len(conn_id, tx_octs, 0x0848);

    uint8_t phy = phy_2m == 2 ? GAP_PHYS_PREFER_2M_BIT : GAP_PHYS_PREFER_1M_BIT;
    le_set_phy(conn_id, 0, phy, phy, GAP_PHYS_OPTIONS_CODED_PREFER_NO);

    if (NULL == update_para_len_phy_timer_handle)
    {
        os_timer_create(&update_para_len_phy_timer_handle, "update_para_len_phy_timer",
                        0, 5000, false, update_para_len_phy_timeout_handler);
    }
    os_timer_start(&update_para_len_phy_timer_handle);
}


void tp_recv_ct_rx(uint8_t conn_id, uint8_t *p_value, uint16_t length)
{
    uint8_t opcode = p_value[0];

    switch (opcode)
    {
    case 0x01:
        tp_send_mtu_size(conn_id);
        break;
    case 0x22:
        tp_recv_test_complete(conn_id);
        break;
    case 0x31:
        tp_send_test_report(conn_id);
        break;
    case 0x11:
        test_mode = 1;
        tp_recv_params_config(opcode, conn_id, p_value, length);
        break;
    case 0x12:
        test_mode = 2;
        tp_recv_params_config(opcode, conn_id, p_value, length);
        break;
    case 0x13:
        test_mode = 3;
        tp_recv_params_config(opcode, conn_id, p_value, length);
        break;
    default:
        APP_PRINT_WARN1("tp_recv_ct_rx invalid opcode=%#x", opcode);
        break;
    }
}

