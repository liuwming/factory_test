#ifndef TEST_H
#define TEST_H

#include "timer.h"
#include "iomux.h"
#include "osdep.h"
#include "miio_proto.h"
#include "rtt_interface.h"
#include "cmd.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#define CR                    0x0d
#define LF                    0x0a
#define MIIO_TRAFFIC_PORT     54321
#define QT                    "\""
#define METHOD                QT"method"QT
#define PARAMS                QT"params"QT
#define ID                    QT"id"QT
#define ROUND(x)              ((x+16)/16)*16
#define NOW()                                   \
    ({ struct timespec tp;                      \
    unsigned long now;                          \
    clock_gettime(CLOCK_REALTIME, &tp);         \
    now = (unsigned long)tp.tv_sec;             \
    now;})

extern char universal_beacon_mac[];
extern char universal_beacon_key[];

#define dev_connect(dev) \
    connect(dev->io.fd, (struct sockaddr*)&(dev->addr), sizeof(dev->addr));

#define dev_send(dev, msg, size) \
    sendto(dev->io.fd, msg, size, 0, (struct sockaddr*)&(dev->addr), sizeof(dev->addr))

#define dev_start_tmr(dev, to, id, rep)         \
    do {                                        \
        dev->tmr.val = to;                      \
        dev->tmr.timer_id = id;                 \
        dev->tmr.data = (void*)dev;             \
        dev->tmr.fn = rt_dev_tmr_func;          \
        dev->tmr.repeat = rep;                  \
        add_timer(&(dev->tmr));                 \
    } while (0)

#define rt_start_tmr(tmr, to, id, rep)          \
    do {                                        \
        rt_cb.tmr.val = (to)*10;                \
        rt_cb.tmr.timer_id = id;                \
        rt_cb.tmr.data = NULL;                  \
        rt_cb.tmr.fn = rt_tmr_func;             \
        rt_cb.tmr.repeat = rep;                 \
        add_timer(&(rt_cb.tmr));                \
    } while (0)

#define rt_set_param_field(dst, src, field) do {                      \
        if (src->field.prsnt) dst->field.val = src->field.val;  \
    } while (0)

#define RT_TMR_DURATION      0
#define RT_TMR_SAMPLE        1
#define RT_TMR_AUDIO_TEST    2
#define RT_TMR_REPEAT_SEND   3
#define RT_TMR_WAIT_SEND_ALL 5 //tmr id: waiting for miio_test_exit and miIO.bleEvtRuleAdd ack timeout

#define RT_DEV_TMR_SCANIP    0
#define RT_DEV_TMR_HANDSHAKE 1
#define RT_DEV_TMR_PROBE     2
#define RT_DEV_TMR_SENDMSG   3
#define RT_DEV_TMR_TEST_EXIT_OK 4 //waiting for miio_test_exit_ok timeout


typedef enum {
    RT_IDLE,
    RT_RUNNING,
    RT_MAX
} rt_state_t;


/* miio state */
typedef enum {
    STATE_IDLE = 0, /* always */
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_MAX
} miio_dev_state_t;

typedef enum {
    RT_DEV_START,
    RT_DEV_IP_ALLOCED,
    RT_DEV_IDENTIFIED,
    RT_DEV_CONN_FAILED,
    RT_DEV_MAX,
} rt_dev_state_t;

typedef enum {
    RTT_UNKN,
    RTT_PRINT, // for donut, wait for confirmation to print label
    RTT_PASS,
    RTT_FAIL,
    RTT_PRE_PASS,
    RTT_PRE_FAIL,
} test_result_t;

typedef struct {
    char beacon_param[10][128];//ble ctrl data,max 10
    char beacon_send_id[10];//send pkg id
    char beacon_confirmed[10];// if the pkg was confirmed this byte will be set to 1;
} ble_ctrls_data_t;
/*
 * wifi/miio devices under test
 */
typedef struct rt_dev {
    list_node_t         self;
    char                model[32];
    unsigned char       mac[6];
    char                ip[16];
    pid_t               async_pid;
    tmr_t               tmr;
    rt_dev_state_t      rt_dev_state;
    int                 rssi_calc_cnt;
    long long           total_rssi;
    int                 avg_rssi;
    int                 min_rssi;
    int                 max_rssi;
    unsigned            rate_to;
    unsigned            rate_from;
    /* miio related field starts */
    unsigned            packet_send;
    unsigned            packet_recv;
    struct sockaddr_in  addr;
    miio_dev_state_t    miio_dev_state;
    int                 hand_shake_cnt;
    int                 probe_cnt;
    char                fw_ver[16];
    int                 brs;
    int                 brf;
    int                 id;
    uint64_t            did;
    uint32_t            stamp;
    unsigned long       time;
    char                md5[md5_size];
    io_inst_t           io;
    int                 test_result; // test_result_t
    rt_client_t         *clnt_interested; // the client to send the confirmation to
#if BEACON_SUPPORT
    ble_ctrls_data_t  beacon_data;
#endif 
#if DONUT_SUPPORT
    int                 audio_test_result; // audio_test_result_t
    int                 ble_start_sent;
    int                 ssh_start_sent;
    int                 health_status; // health_status_t
#endif
#if FCC_SUPPORT
    int                 fcc;
#endif
    int                 id_test_exit;
    int                 test_exit_confirmed;
} rt_dev_t;

/*
 * radio test tool control struct
 */
typedef struct radio_test {
    rt_state_t   state;
    tmr_t        tmr_sample;
    tmr_t        tmr_duration;
    tmr_t        tmr_repeat_send;//used for repeat send miio_test_exit add add ble ctrl cmd
    tmr_t        tmr_wait_send_all;//tmr: waiting for miio_test_exit and miIO.bleEvtRuleAdd ack timeout
#if DONUT_SUPPORT
    tmr_t        tmr_audio_test;
#endif
    rt_param_t   param;
    int          dev_cnt;
    list_t       dev_list;
} radio_test_t;

typedef enum {
    CTRL_DEV_CMD_TEST_PASS  = 0,
    CTRL_DEV_CMD_TEST_FAIL  = 1,
    CTRL_DEV_CMD_OTA        = 2,
    CTRL_DEV_CMD_ADD_BEACON = 3,
    CTRL_DEV_CMD_AUDIO_TEST = 4,
    CTRL_DEV_CMD_BLE_TEST   = 5,
    CTRL_DEV_CMD_AUDIO_RES  = 6,
} ctrl_dev_cmd_t;

extern int start_test();
extern int clear_test();
extern int stop_test();
extern void ctrl_dev(char*, ctrl_dev_cmd_t);
extern void report_status(rt_client_t*, int);
extern void set_param(rt_param_t*);
extern rt_param_t* get_param();
extern int rt_running(void);
extern void rt_update_stats(unsigned char*, struct rx_info*, unsigned char*, unsigned char*);

#endif
