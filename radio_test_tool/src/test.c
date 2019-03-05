#include "util.h"
#include "test.h"
#include "string.h"
#include "air.h"
#include "lib_md5.h"


#include <stdio.h>  
#include <stdarg.h>  
  
void st_print(int line,char *name,const char *fmt,...)  
{  
    int d;  
    char *s;  
    char c;  
    char buf[1024] = {0};  
    char buffer[10];  
    va_list ap;  
    va_start(ap,fmt);     
  
    while(*fmt != '\0')  
    {  
        if(*fmt == '%')  
        {  
            fmt++;  
            if(*fmt == 's')  
            {
                s = va_arg(ap,char *);
                strcat(buf,s);
            }
            else if(*fmt == 'd')
            {
                d = va_arg(ap,int);
                memset(buffer,0,10);
                sprintf(buffer,"%d",d);
                strcat(buf,buffer);
            }
        }
        else
        {
            memset(buffer,0,10);
            sprintf(buffer,"%c",*fmt);
            strcat(buf,buffer);
        }
        fmt++;
    }//end while
     
    time_t tt = time(0);
    struct tm ttt = *localtime(&tt);
    fprintf(stdout,"[%d:%d:%d],%s",ttt.tm_hour, ttt.tm_min, ttt.tm_sec, buf);  
    va_end(ap);
    return;
}  
  
#define message_r(...) st_print(__LINE__,__FILE__,##__VA_ARGS__)



static void rt_tmr_func(int tid, void* data);
static int dev_rpc_send(rt_dev_t* dev, char* method, char* param);
static void rt_miio_dev_hs(rt_dev_t* dev);
static void rt_dev_tmr_func(int tid, void* data);
int is_audio_test_pending();

int host_apd_running = 1;

#if DONUT_SUPPORT
void audio_test_tick_cb();
#endif

char universal_beacon_mac[128] = {0};
char universal_beacon_key[128] = {0};

char TOKEN[] = {
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF
};

#define TIME_SPACE_DURATION 40
#define TIME_REPEAT_SPACE_SCANIP 15//默认1.5秒再次去搜索一次 

static radio_test_t   rt_cb = {
    .state = RT_IDLE,
    .param = {{1, TIME_SPACE_DURATION}, {1, 1}, {1, -50}, {1, -60}, {1, -40}, {1, 10}},
    .dev_cnt = 0,
    .dev_list = {.prev = &(rt_cb.dev_list), .next = &(rt_cb.dev_list)}
};

int rt_running()
{
    return (rt_cb.state == RT_RUNNING);
}

int start_test()
{
    int rc;

    if (host_apd_running) {
        rc = rtt_exec("/etc/init.d/hostapd stop && sleep 5", 1);
    } else {
        rc = rtt_exec("/etc/init.d/hostapd stop && sleep 1", 1);
    }
    rc = rtt_exec("/etc/init.d/hostapd start", 1);
    if (rc != 0)
        return RT_ERR_START_HOSTAPD;

    host_apd_running = 1;

    rc = rtt_exec("dnsmasq --conf-file=/etc/dnsmasq.conf", 1);
    if (rc != 0)
        return RT_ERR_START_DNS;

    rc = rtt_exec("rngd -r /dev/urandom -o /dev/random", 1);
    if (rc != 0)
        return RT_ERR_START_DNS;

    /* need to call this here as we actually blocked 8 for seconds in above call */
    time_update();

    PANIC_IF(rt_cb.dev_cnt != 0);
    PANIC_IF(!list_is_empty(&(rt_cb.dev_list)));
    PANIC_IF(air_start());      /* start to monitor 802.11 traffic */

    rt_cb.state = RT_RUNNING;
    rt_start_tmr(tmr_duration, rt_cb.param.duration.val / 2, RT_TMR_DURATION, 0);
    rt_start_tmr(tmr_sample, rt_cb.param.sample_rate.val, RT_TMR_SAMPLE, 1);
#if DONUT_SUPPORT
    rt_start_tmr(tmr_audio_test, 1, RT_TMR_AUDIO_TEST, 1);
#endif

    return 0;
}

static void rt_tmr_func(int tid, void* data)
{
    /* time to stop this round of test and report result */
    if (tid == RT_TMR_DURATION) {
        message_r("RT_TMR_DURATION timeout stop_test >>>>>\n\n");
#if DONUT_SUPPORT
        if (is_audio_test_pending()) {
            del_timer(&rt_cb.tmr_duration);
            rt_start_tmr(tmr_duration, 1, RT_TMR_DURATION, 0);
            return;
        }
#endif
        report_status(NULL, RT_CMD_RESULT);
        stop_test();
    } else if (tid == RT_TMR_SAMPLE) {
        message_r("RT_TMR_SAMPLE timeout report_status >>>>>\n\n");
        report_status(NULL, RT_CMD_STATUS);
#if DONUT_SUPPORT
    } else if (tid == RT_TMR_AUDIO_TEST) {
        audio_test_tick_cb();
#endif
    } else {
        ;
    }
}

#if DONUT_SUPPORT
extern char bin_dir[128];

void download_audio_test_results(rt_dev_t* dev)
{
    char * cmd[128];
    int fn_idx = 0;

    /* mkdir first */
    sprintf(cmd, "mkdir -p ~/audio_test_results/%02x/%02x/%02x/%02x/%02x/",
            dev->mac[0], dev->mac[1], dev->mac[2],
            dev->mac[3], dev->mac[4]);
    system(cmd);

    /* find a file name that's not occupied */
    for (fn_idx = 0; fn_idx < 100; fn_idx ++) {
        sprintf(cmd, "test -e ~/audio_test_results/%02x/%02x/%02x/%02x/%02x/%02x-%04d",
                dev->mac[0], dev->mac[1], dev->mac[2],
                dev->mac[3], dev->mac[4], dev->mac[5],
                fn_idx);

        if (0 != system(cmd)) {
            break;
        }
    }

    /* download file */
    sprintf(cmd, "%s/download %s root pass /tmp/aaz_result ~/audio_test_results/%02x/%02x/%02x/%02x/%02x/%02x-%04d &",
            bin_dir, dev->ip,
            dev->mac[0], dev->mac[1], dev->mac[2],
            dev->mac[3], dev->mac[4], dev->mac[5],
            fn_idx);
    system(cmd);
}
#endif

static char* parse_miio_rsp(rt_dev_t* dev, char* buf, int len)
{
    int info_size;
    unsigned int info_len;
    char* info;

    info_size = len - header_size;
    info = (char*)calloc(info_size*20, sizeof(char));
    info_len = decrypt(buf, len, dev->md5, info, info_size);

    RT_ERR("Info len: %d, content: %s\n", info_len, info);
    return info;
}

static int rt_miio_recv(io_inst_t* inst)
{
    char buf[1024] = {0};
    char body[20] = {0};
    int valid_body = 0;
    char *rsp, *p1, *p2;
    int rc;
    uint16_t len;
    char * p_model_name;
    int off;

    int brf, brs, fcc;
    int id = -1;
    char mac_str[32] = {0};


    rt_dev_t* dev = inst->priv;

    do {
        rc = recv(inst->fd, buf, sizeof(buf), 0);
    } while (rc < 0 && errno == EINTR);

    if (rc <= 0) {
        RT_ERR("failed to receive message from Miio.\n");
        return -1;
    }

    /* miio device connect state machine */
    switch (dev->miio_dev_state) {

    case STATE_IDLE:
        parse_msg_head(buf, &len, &(dev->did), &(dev->stamp), dev->md5);

        dev->time = NOW();
        del_timer(&(dev->tmr));

        dev_rpc_send(dev, "miIO.info", "[]");
        dev_start_tmr(dev, 100, RT_DEV_TMR_PROBE, 0);

        dev->miio_dev_state = STATE_CONNECTING;
        break;

    case STATE_CONNECTING:
        rsp = parse_miio_rsp(dev, buf, rc);

#if !DONUT_SUPPORT
        p_model_name = strstr(rsp, "yeelink.");
        if(p_model_name == NULL){
            p_model_name = strstr(rsp, "yilai.");
        }
        if (p_model_name) {
#endif
            del_timer(&(dev->tmr));
            dev->miio_dev_state = STATE_CONNECTED;
            dev->rt_dev_state = RT_DEV_IDENTIFIED;
            message_r("STATE_CONNECTING recv data so rt_dev_state=RT_DEV_IDENTIFIED  st RT_DEV_TMR_SENDMSG rest RT_TMR_DURATION >>>>>\n\n");
            /* send message to miio dev every 0.5 sec */
            dev_start_tmr(dev, 10, RT_DEV_TMR_SENDMSG, 1);
            /* store model name */
            /* reset timer when first device is connected */
            rt_start_tmr(tmr_duration, rt_cb.param.duration.val / 2, RT_TMR_DURATION, 0);
#if !DONUT_SUPPORT
            strncpy(dev->model, p_model_name, 32);
            for (off = 0; off < 32; off++) {
                if(dev->model[off] == '"') {
                    dev->model[off] = 0;
                    break;
                }
            }
        }
#endif
        free(rsp);

        break;

    case STATE_CONNECTED:
        rsp = parse_miio_rsp(dev, buf, rc);

        if (strstr(rsp, "result")) {
            p1 = strchr(rsp, '[');
            p2 = strchr(rsp, ']');
            if (p1 && p2 && (p1 < p2)) {
                memcpy(body, p1+1, p2-p1-1);
                valid_body = 1;
            }
        }

        p1 = "\"id\":";
        p2 = strstr(rsp, p1);
        if (p2) {
            sscanf(p2 + strlen(p1), "%d", &id);
        }

        if (valid_body) {
            if (id == dev->id_test_exit || strstr(body, "miio_test_exit_ok")) {
                snprintf(mac_str, sizeof(mac_str), "%02x:%02x:%02x:%02x:%02x:%02x",
                dev->mac[0], dev->mac[1], dev->mac[2],
                dev->mac[3], dev->mac[4], dev->mac[5]);
            message_r("recv miio_test_exit_ok >>>>> mac:%s\n", mac_str);
            del_timer(&(dev->tmr));
            message_r("del dev-tmr ok >>>>>\n");
#if DONUT_SUPPORT
                downleoad_audio_test_results(dev);
#endif

                if (dev->clnt_interested) {
                    if (dev->test_result == RTT_PRE_FAIL) {
                        send_ctrl_dev_response(dev, RT_CMD_FAIL_DEV);
#if !BEACON_SUPPORT
                        dev->test_result = RTT_FAIL;
#endif
                    } else {
                        send_ctrl_dev_response(dev, RT_CMD_CTRL_DEV);
#if !BEACON_SUPPORT
                        dev->test_result = RTT_PASS;
#endif
                    }
                }

#if !BEACON_SUPPORT
                stop_hostapd_if_all_test_done();
#endif
            }
#if DONUT_SUPPORT
            else if (strstr(body, "audio_test_")) {
                if (strstr(body, "audio_test_pass")) {
                    dev->audio_test_result = AUDIO_TEST_PASS;
                } else if (strstr(body, "audio_test_fail")) {
                    dev->audio_test_result = AUDIO_TEST_PASS;
                } else {
                    dev->audio_test_result = AUDIO_TEST_UNKN;
                }
            } else if (strstr(body, "health_check")) {
                if (strstr(body, "health_check_pass")) {
                    dev->health_status = HEALTH_STATUS_PASS;
                } else if (strstr(body, "health_check_fail")) {
                    dev->health_status = HEALTH_STATUS_FAIL;
                } else {
                    dev->health_status = HEALTH_STATUS_UNKN;
                }
            } else if (strstr(body, "lable_print_")) {
                if (strstr(body, "lable_print_ok")) {
                    request_label_print(dev);
                    dev->test_result = RTT_PASS;
                    del_timer(&(dev->tmr)); /* stop sending message */
                    stop_hostapd_if_all_test_done();
                }
            }
#endif
            else if (strchr(body, ',')) {
                if (dev->brs == -1 || dev->brf == -1) {
#if FCC_SUPPORT
                    if (3 == sscanf(body, "%d,%d,%d", &brs, &brf, &fcc)) {
                        dev->brs = brs;
                        dev->brf = brf;
                        dev->fcc = fcc;
                    }
#else
                    if (2 == sscanf(body, "%d,%d", &brs, &brf)) {
                        dev->brs = brs;
                        dev->brf = brf;
                    }
#endif
                }
            } else {
                if (dev->fw_ver[0] == '\0')
                    memcpy(dev->fw_ver, body, strlen(body));
            }
        }

        free(rsp);
        dev->packet_recv ++;
        //message_r("recv frame serial count = %d >>>>>\n", dev->packet_recv);
        break;

    default:
        break;
    }

    return 0;
}

static void rt_dev_miio_start(rt_dev_t* dev)
{
    int sock, rc;

    memset(&(dev->addr), 0, sizeof(dev->addr));
    dev->addr.sin_family = AF_INET;
    inet_aton(dev->ip, &(dev->addr.sin_addr));
    dev->addr.sin_port = htons(MIIO_TRAFFIC_PORT);

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    /* add to io-mux poll list */
    IO_INST_INIT(&(dev->io),
                 sock,
                 IO_EVENT_IN,
                 1,
                 0,
                 rt_miio_recv,
                 NULL,
                 NULL,
                 dev);

    rc = iomux_add(&(dev->io));
    PANIC_IF(rc < 0);

    dev->miio_dev_state = STATE_IDLE;
    /* init and send handshake message */
    rt_miio_dev_hs(dev);
}

static void rt_miio_dev_hs(rt_dev_t* dev)
{
    char msg[header_size] = { 0 };
    init_msg_head(msg, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFF, TOKEN, header_size);
    dev_connect(dev);
    dev_send(dev, msg, header_size);
    dev_start_tmr(dev, 50, RT_DEV_TMR_HANDSHAKE, 0);
}

static int dev_raw_send(rt_dev_t* dev, char* raw)
{
    int rc, rpc_len, total_len;
    char* msg;
    char cmd_buf[1024] = {0};

    rc = snprintf(cmd_buf, sizeof(cmd_buf),
                  "{%s:%d, %s}",
                  ID, ++dev->id,
                  raw);

    rpc_len = ROUND(rc);
    total_len = rpc_len + header_size;

    msg = (char*)calloc(total_len*10, sizeof(char));

    encrypt(dev->did, dev->md5,
            dev->stamp + (NOW() - dev->time),
            cmd_buf, rc, msg, total_len);

    dev_send(dev, msg, total_len);

    free(msg);

    return dev->id;
}

static int dev_rpc_send(rt_dev_t* dev, char* method, char* param)
{
    char rpc_buf[1024] = {0};

    snprintf(rpc_buf, sizeof(rpc_buf),
             "%s:%s%s%s,%s:%s",
             METHOD, QT, method, QT,
             PARAMS, param);

    message_r("in dev_rpc_send ip[%s] data[%s] >>>>>\n", dev->ip, rpc_buf);
    if (strstr(rpc_buf, "miio_test_exit")) {//avoid to loss miio_test_exit
        message_r("in dev_rpc_send find miio_test_exit >>>>>\n");
        dev_raw_send(dev, rpc_buf);
        usleep(300*1000);
        message_r("in dev_rpc_send send miio_test_exit twice>>>>>\n");
        return dev_raw_send(dev, rpc_buf);
    }
    return dev_raw_send(dev, rpc_buf);
}


void str2hex(char *ds, unsigned char *bs, unsigned int n)
{
    int i;
    for (i = 0; i < n; i++)
        sprintf(ds + 2 * i, "%02x", bs[i]);

}

static void get_md5(unsigned char *buf, uint32_t len, unsigned char *des)
{
    unsigned char tmp[16];
    //    MD5_CTX md5;
    //
    //    MD5InitA(&md5);
    //    MD5UpdateA(&md5, buf, len);
    //    MD5FinalA(&md5, tmp);
    md5(buf, len, tmp);
    str2hex((char *)des, tmp, 16);
}

static char* rt_dev_scan_ip(unsigned char* mac)
{
    char* ip = xalloc(16);
    FILE* fp;
    char mac_str[32] = {0};
    char cmd[128] = {0};
    int item;

    snprintf(mac_str, sizeof(mac_str), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2],
             mac[3], mac[4], mac[5]);

    snprintf(cmd, sizeof(cmd), "grep \"%s\" /var/lib/misc/dnsmasq.leases | cut -d \" \" -f3", mac_str);

    fp = popen(cmd, "r");
    if (fp == NULL)
        goto err;

    item = fscanf(fp, "%s", ip);
    if (item != 1)
        goto err;

    RT_ERR("Found ip address %s for mac %s\n", ip, mac_str);

    fclose(fp);
    return ip;
 err:
    free(ip);
    if (fp) fclose(fp);
    return NULL;
}

static void rt_dev_tmr_func(int tid, void* data)
{
    rt_dev_t* dev = data;
    char* ip;
    char cmd[128] = {0};

    if (tid == RT_DEV_TMR_SCANIP) {
        ip = rt_dev_scan_ip(dev->mac);
        if (ip) {
            strncpy(dev->ip, ip, sizeof(dev->ip));
            free(ip);
            snprintf(cmd, sizeof(cmd), "ping -i 0.5 %s", dev->ip);
            dev->async_pid = rtt_exec2(cmd);
            dev->rt_dev_state = RT_DEV_IP_ALLOCED;
            message_r("RT_DEV_TMR_SCANIP timeout scaned ip  rt_dev_state = RT_DEV_IP_ALLOCED >>>>>\n");
            rt_dev_miio_start(dev);
        } else {
            RT_ERR("No ip found.\n");
            dev_start_tmr(dev, TIME_REPEAT_SPACE_SCANIP, RT_DEV_TMR_SCANIP, 0);
        }
    } else if (tid == RT_DEV_TMR_SENDMSG) {
        message_r("RT_DEV_TMR_SENDMSG timeout dev_rpc_send [get_prop] >>>>>\n");
        dev_rpc_send(dev, "get_prop", "[\"fw_ver\"]");
        dev->packet_send ++;
        if (dev->brs == -1 || dev->brf == -1) {
            dev_rpc_send(dev, "get_aging_status", "[]");
            dev->packet_send ++;
        }
#if DONUT_SUPPORT
        if (dev->health_status == HEALTH_STATUS_UNKN) {
            dev_rpc_send(dev, "get_health_status", "[]");
            dev->packet_send ++;
        }
        if (!dev->ble_start_sent) {
            dev_rpc_send(dev, "start_ble_test", "[]");
            dev->ble_start_sent = 1;
            dev->packet_send ++;
        }

        char auth_code[33] = {0};
        char plain_text[128] = {0};

        if (!dev->ssh_start_sent) {
            sprintf(plain_text, "%s%02X:%02X:%02X:%02X:%02X:%02X",
                    "1c8e9d6568c73add89c2ec3ea2ce2a1e",
                    dev->mac[0], dev->mac[1], dev->mac[2],
                    dev->mac[3], dev->mac[4], dev->mac[5]);
            get_md5(plain_text, strlen(plain_text), auth_code);
            sprintf(cmd, "[\"pass\",\"%s\"]", auth_code);
            dev_rpc_send(dev, "enable_ssh", cmd);
            dev->ssh_start_sent = 1;
            dev->packet_send ++;
        }
        if (dev->test_result == RTT_PRINT) {
            dev_rpc_send(dev, "query_label_print", "[]");
            dev->packet_send ++;
        }
#endif
    } else if (tid == RT_DEV_TMR_HANDSHAKE) {
        RT_ERR("RT_DEV_TMR_HANDSHAKE timeout for dev %s\n", dev->ip);
        if (dev->hand_shake_cnt++ > 3) {
            RT_ERR("handshake failure for dev %s\n", dev->ip);
            goto fail_miio_dev;
        }
        rt_miio_dev_hs(dev);

    } else if (tid == RT_DEV_TMR_PROBE) {
        RT_ERR("RT_DEV_TMR_PROBE timeout for dev %s\n", dev->ip);
        if (dev->probe_cnt++ > 3) {
            RT_ERR("probe failure for dev %s\n", dev->ip);
            goto fail_miio_dev;
        }
        dev_rpc_send(dev, "miIO.info", "[]");
        dev_start_tmr(dev, 100, RT_DEV_TMR_PROBE, 0);
    } else if (tid == RT_DEV_TMR_TEST_EXIT_OK) {
        message_r("RT_DEV_TMR_TEST_EXIT_OK timeout waiting for test_exit_ok timeout >>>>>\n");
#if DONUT_SUPPORT
        downleoad_audio_test_results(dev);
#endif
        if (dev->clnt_interested) {
            if (dev->test_result == RTT_PRE_FAIL) {
                send_ctrl_dev_response(dev, RT_CMD_FAIL_DEV);
                dev->test_result = RTT_FAIL;
            } else {
                send_ctrl_dev_response(dev, RT_CMD_CTRL_DEV);
                dev->test_result = RTT_PASS;
            }
        }
#if !BEACON_SUPPORT
        stop_hostapd_if_all_test_done();
#endif
    } else if(tid == RT_DEV_TMR_DELAY_BEACON_SEND_ALL){
         message_r("RT_DEV_TMR_DELAY_BEACON_SEND_ALL timeout try stop_hostapd_if_all_test_done >>>>>\n\n");
         stop_hostapd_if_all_test_done();
    }
    return;

 fail_miio_dev:
    dev->rt_dev_state = RT_DEV_CONN_FAILED;
    message_r("RT_DEV_TMR_HANDSHAKE or RT_DEV_TMR_PROBE timeout for 3 times >>>>>\n");
    rtt_wait_pid(dev->async_pid);
    iomux_del(&(dev->io));
    close(dev->io.fd);
    return;
}

int stop_test()
{
    list_node_t *node;
    rt_dev_t* dev;

    air_stop();
    del_timer(&(rt_cb.tmr_duration));
    del_timer(&(rt_cb.tmr_sample));
#if DONUT_SUPPORT
    del_timer(&(rt_cb.tmr_audio_test));
#endif
    rt_cb.state = RT_IDLE;


#if !DONUT_SUPPORT
    /* gather information and clear */
    list_for(&(rt_cb.dev_list), node) {
        dev = object_of(rt_dev_t, self, node);
        del_timer(&(dev->tmr)); /* stop sending message */
    }
#endif

    return 0;
}

int clear_test()
{
    list_node_t *node, *tmp;
    rt_dev_t* dev;

    /* gather information and clear */
    list_for_del(&(rt_cb.dev_list), node, tmp) {
        list_del(node);
        dev = object_of(rt_dev_t, self, node);
        del_timer(&(dev->tmr));
        if (dev->rt_dev_state == RT_DEV_IP_ALLOCED ||
            dev->rt_dev_state == RT_DEV_IDENTIFIED) {
            rtt_wait_pid(dev->async_pid);
            iomux_del(&(dev->io));
            close(dev->io.fd);
        }
        free(dev);
        rt_cb.dev_cnt--;
    }

    /* clear dns leases */
    rtt_exec("pkill dnsmasq", 1);
    rtt_exec("rm -f /var/lib/misc/dnsmasq.leases", 1);

    return 0;
}

void set_param(rt_param_t* p)
{
    rt_set_param_field((&(rt_cb.param)), p, duration);
    rt_set_param_field((&(rt_cb.param)), p, sample_rate);
    rt_set_param_field((&(rt_cb.param)), p, thresh_avg_rssi);
    rt_set_param_field((&(rt_cb.param)), p, thresh_min_rssi);
    rt_set_param_field((&(rt_cb.param)), p, thresh_max_rssi);
    rt_set_param_field((&(rt_cb.param)), p, thresh_packet_loss);
}

rt_param_t* get_param()
{
    return &(rt_cb.param);
}

static rt_dev_t* rt_find_dev(unsigned char* mac)
{
    list_node_t* node;
    rt_dev_t* dev;

    list_for(&(rt_cb.dev_list), node) {
        dev = object_of(rt_dev_t, self, node);
        if (memcmp(dev->mac, mac, 6) == 0)
            return dev;
    }
    return NULL;
}

static rt_dev_t* rt_add_dev(unsigned char* mac)
{
    rt_dev_t* dev= xalloc(sizeof(rt_dev_t));

    memcpy(dev->mac, mac, 6);
    list_node_init(&(dev->self));
    list_ins_back(&(rt_cb.dev_list), &(dev->self));
    rt_cb.dev_cnt ++;

    dev->min_rssi = 1;
    dev->max_rssi = -1000;
    dev->rt_dev_state = RT_DEV_START;
    message_r("rt_add_dev rt_dev_state = RT_DEV_START \n");
    dev_start_tmr(dev, TIME_REPEAT_SPACE_SCANIP, RT_DEV_TMR_SCANIP, 0); /* scan for IP every 3 secs */
    dev->brs = -1;
    dev->brf = -1;
    return dev;
}

static void rt_update_dev_rssi(rt_dev_t* dev, int power)
{
    dev->total_rssi += power;
    dev->rssi_calc_cnt++;
    dev->avg_rssi = dev->total_rssi / dev->rssi_calc_cnt;

    if (power < dev->min_rssi)
        dev->min_rssi = power;
    if (power > dev->max_rssi)
        dev->max_rssi = power;
}

// this function will be called only when exit_pass or exit_fail is received from client
// at this time, all dev that have not been identified will be ignored
void stop_hostapd_if_all_test_done()
{
    message_r("stop_hostapd_if_all_test_done \n\n");
    list_node_t* node;
    rt_dev_t* dev;
    list_for(&(rt_cb.dev_list), node) {
        dev = object_of(rt_dev_t, self, node);
        if (dev->rt_dev_state == RT_DEV_IDENTIFIED &&
            dev->test_result != RTT_PASS &&
            dev->test_result != RTT_FAIL) {
            message_r("stop_hostapd_if_all_test_done not all test is done return\n\n");
            // if any identified device has not received test_exit command, then wait
            return;
        }
    }

    sleep(8);

    int rc = rtt_exec("/etc/init.d/hostapd stop", 1);
    if (rc != 0)
    {
        RT_ERR("failed to stop hostapd after all devices exited test\n");
    }
    host_apd_running = 0;
}

void ctrl_dev3(char* dev_mac, ctrl_dev_cmd_t cmd, rt_client_t* clnt)
{
    list_node_t* node;
    rt_dev_t* dev;
    char mac[32] = {0};
    char *p;
    char ip[16];
    char cmd_param[128];

    list_for(&(rt_cb.dev_list), node) {
        dev = object_of(rt_dev_t, self, node);

        snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x",
                 dev->mac[0], dev->mac[1], dev->mac[2],
                 dev->mac[3], dev->mac[4], dev->mac[5]);

        RT_ERR("Compare mac [%s] with target [%s]\n", mac, dev_mac);

        if (!strcmp(dev_mac, mac)) {
            dev->clnt_interested = clnt;
            switch(cmd)
            {
            case CTRL_DEV_CMD_TEST_PASS:
                message_r("cmd = CTRL_DEV_CMD_TEST_PASS dev_rpc_send [miio_test_exit0] >>>>>\n\n");
                dev->id_test_exit = dev_rpc_send(dev, "miio_test_exit", "[0]");
                dev->test_result = RTT_PRE_PASS;
                dev_start_tmr(dev, 50, RT_DEV_TMR_TEST_EXIT_OK, 0);
                #if !BEACON_SUPPORT
                    message_r("cmd = CTRL_DEV_CMD_TEST_PASS but not beacon_support >>>>>\n\n");
                    stop_hostapd_if_all_test_done();
                #endif
                break;
            case CTRL_DEV_CMD_TEST_FAIL:
                message_r("cmd = CTRL_DEV_CMD_TEST_FAIL dev_rpc_send [miio_test_exit1] >>>>>\n\n");
                dev->id_test_exit = dev_rpc_send(dev, "miio_test_exit", "[1]");
                dev->test_result = RTT_PRE_FAIL;
                dev_start_tmr(dev, 50, RT_DEV_TMR_TEST_EXIT_OK, 0);
                #if !BEACON_SUPPORT
                    message_r("cmd = CTRL_DEV_CMD_TEST_FAIL but not beacon_support >>>>>\n\n");
                    stop_hostapd_if_all_test_done();
                #endif
                break;
            case CTRL_DEV_CMD_OTA:
                if (strcmp(dev->fw_ver, "\"17\"") == 0
                    || strcmp(dev->fw_ver, "\"9\"") == 0
                    || strcmp(dev->fw_ver, "\"22\"") == 0) {
                    strcpy(ip, dev->ip);
                    p = rindex(ip, '.');
                    if (!p)
                        return;
                    *(p+1) = '1';
                    *(p+2) = '\0';
                    //sprintf(cmd_param, "{\"app_url\":\"http://%s/lamp.bin\"}", ip);
                    sprintf(cmd_param, "{\"app_url\":\"http://www.yeelight.com/bindir/upgrade.bin\"}");
                    dev_rpc_send(dev, "miIO.ota", cmd_param);
                }
                break;
            case CTRL_DEV_CMD_ADD_BEACON:
                sprintf(cmd_param, "{\"mac\":\"%s\",\"pid\":69,\"eid\":4097,\"beaconKey\":\"%s\"}",
                        universal_beacon_mac, universal_beacon_key);
                message_r("cmd = CTRL_DEV_CMD_ADD_BEACON dev_rpc_send [miIO.bleEvtRuleAdd] >>>>>\n\n");
                dev_rpc_send(dev, "miIO.bleEvtRuleAdd", cmd_param);
                #if BEACON_SUPPORT
                //if (dev->clnt_interested) {
                    //message_r("cmd = CTRL_DEV_CMD_ADD_BEACON clnt_interested >>>>>\n\n");
                    if (dev->test_result == RTT_PRE_FAIL) {
                        message_r("cmd = CTRL_DEV_CMD_ADD_BEACON  test_result = RTT_PRE_FAIL >>>>>\n\n");
                        dev->test_result = RTT_FAIL;
                    } else {
                        message_r("cmd = CTRL_DEV_CMD_ADD_BEACON  test_result = RTT_PRE_PASS >>>>>\n\n");
                        dev->test_result = RTT_PASS;
                    }
                //}
                //else{
                //    message_r("cmd = CTRL_DEV_CMD_ADD_BEACON not clnt_interested >>>>>\n\n");
                //}
                //message_r("cmd = CTRL_DEV_CMD_ADD_BEACON  stop_hostapd_if_all_test_done >>>>>\n\n");
                //stop_hostapd_if_all_test_done();
                dev_start_tmr(dev, 50, RT_DEV_TMR_DELAY_BEACON_SEND_ALL, 0);
                #endif
                break;
            case CTRL_DEV_CMD_AUDIO_TEST:
                dev_rpc_send(dev, "start_audio_test", "[80]");
                dev->packet_send ++;
                break;
            case CTRL_DEV_CMD_BLE_TEST:
                dev_rpc_send(dev, "start_ble_test", "[]");
                dev->packet_send ++;
                break;
            case CTRL_DEV_CMD_AUDIO_RES:
                dev_rpc_send(dev, "get_audio_test", "[]");
                break;
            }

            return;
        }
    }
    RT_ERR("No device found for mac [%s], how ???\n", dev_mac);
}

void ctrl_dev(char* dev_mac, ctrl_dev_cmd_t cmd)
{
    ctrl_dev3(dev_mac, cmd, NULL);
}

void rt_update_stats(unsigned char* mac, struct rx_info *ri, unsigned char* h80211, unsigned char* bssid)
{
    if (!rt_running()) return;

    rt_dev_t* dev = rt_find_dev(mac);
    if (dev == NULL) {
        RT_ERR("Found new device.\n");
        dev = rt_add_dev(mac);
    }

    //update bitrate to station
    if(( h80211[1] & 3 ) == 2)
        dev->rate_to = ri->ri_rate;

    /* only update power if packets comes from the
     * client: either type == Mgmt and SA != BSSID,
     * or FromDS == 0 and ToDS == 1 */
    if( ( ( h80211[1] & 3 ) == 0 &&
          memcmp( h80211 + 10, bssid, 6 ) != 0 ) ||
        ( ( h80211[1] & 3 ) == 1 ) )
    {
        rt_update_dev_rssi(dev, ri->ri_power);
        dev->rate_from = ri->ri_rate;
    }

    return ;
}

#define rt_res(field) result.body.status[result.dev_count].field
#if DONUT_SUPPORT
void request_label_print(rt_dev_t* dev) //(rt_client_t* clnt, char* mac, char* did)
{
    rt_cmd_result_t result = {0};

    char mac[64] = {0};
    char did[20] = {0};

    snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x",
             dev->mac[0], dev->mac[1], dev->mac[2],
             dev->mac[3], dev->mac[4], dev->mac[5]);
    sprintf(did, "%llu", dev->did);

    result.type = RT_CMD_PRINT_LABEL;

    strcpy(result.body.label_info.mac, mac);
    strcpy(result.body.label_info.did, did);

    clnt_broadcast_res(&result);
}
#endif

void send_ctrl_dev_response(rt_dev_t* dev, rt_cmd_type_t type) //(rt_client_t* clnt, char* mac, char* did)
{
    rt_cmd_result_t result = {0};

    char mac[64] = {0};
    char did[20] = {0};

    snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x",
             dev->mac[0], dev->mac[1], dev->mac[2],
             dev->mac[3], dev->mac[4], dev->mac[5]);
    sprintf(did, "%llu", dev->did);

    result.type = type;
    result.ret_code = RT_ERR_OK;

    strcpy(result.body.label_info.mac, mac);
    strcpy(result.body.label_info.did, did);

    clnt_result2(dev->clnt_interested, &result);
}

void report_status(rt_client_t* clnt, int type)
{
    list_node_t* node;
    rt_dev_t* dev;
    rt_cmd_result_t result;
    char mac[32] = {0};

    bzero(&result, sizeof(result));

    result.type = type;

    list_for(&(rt_cb.dev_list), node) {
        if (result.dev_count >= MAX_TEST_DEV)
            break;
        dev = object_of(rt_dev_t, self, node);

        snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x",
                 dev->mac[0], dev->mac[1], dev->mac[2],
                 dev->mac[3], dev->mac[4], dev->mac[5]);

        memcpy(rt_res(mac), mac, sizeof(mac));

        if (dev->rt_dev_state == RT_DEV_START) {
            sprintf(rt_res(ip), "Allocating");
            sprintf(rt_res(state), "Acquiring IP");
        } else if (dev->rt_dev_state == RT_DEV_IP_ALLOCED) {
            sprintf(rt_res(ip), "%s", dev->ip);
            sprintf(rt_res(state), "IP Allocated");
        } else if (dev->rt_dev_state == RT_DEV_IDENTIFIED) {
            sprintf(rt_res(ip), "%s", dev->ip);
            switch (dev->miio_dev_state) {
            case STATE_IDLE:
                sprintf(rt_res(state), "Probing Device");
                break;
            case STATE_CONNECTING:
                sprintf(rt_res(state), "Hand-shaking with Device");
                break;
            case STATE_CONNECTED:
                sprintf(rt_res(state), "Device Connected");
                memcpy(rt_res(fw_ver), dev->fw_ver, sizeof(dev->fw_ver));
                memcpy(rt_res(model), dev->model, sizeof(dev->model));
                rt_res(did) = dev->did;
                rt_res(rssi_calc_cnt) = dev->rssi_calc_cnt;
                rt_res(avg_rssi) = dev->avg_rssi;
                rt_res(min_rssi) = dev->min_rssi;
                rt_res(max_rssi) = dev->max_rssi;
#if DONUT_SUPPORT
                rt_res(audio_test_result) = dev->audio_test_result;
                rt_res(health_status) = dev->health_status;
#endif
                rt_res(packet_send) = dev->packet_send;
                rt_res(packet_recv) = dev->packet_recv;
                rt_res(brs) = dev->brs;
                rt_res(brf) = dev->brf;
#if FCC_SUPPORT
                rt_res(fcc) = dev->fcc;
#endif
                break;
            default:
                break;
            }
        } else if (dev->rt_dev_state == RT_DEV_CONN_FAILED) {
            sprintf(rt_res(ip), "%s", dev->ip);
            sprintf(rt_res(state), "Probing Failed");
        } else {
            RT_ERR("Unexpected state.\n");
            continue;
        }
        result.dev_count++;
    }

    if (clnt)
        clnt_result2(clnt, &result);
    else
        clnt_broadcast_res(&result);
}

#if DONUT_SUPPORT

int g_audio_test_pending = 0;

int is_audio_test_pending()
{
    return g_audio_test_pending;
}

void audio_test_tick_cb()
{
    list_node_t* node;
    rt_dev_t* dev;
    rt_dev_t* dev_to_start = NULL, *dev_started = NULL;
    rt_cmd_result_t result;
    int done = 0;

    bzero(&result, sizeof(result));

    list_for(&(rt_cb.dev_list), node) {
        dev = object_of(rt_dev_t, self, node);
        if (dev->miio_dev_state != STATE_CONNECTED) {
            continue;
        }
        switch (dev->audio_test_result) {
        case AUDIO_TEST_IDLE:
            if (!dev_to_start)
                dev_to_start = dev;
            break;

        case AUDIO_TEST_UNKN:
            dev_started = dev;
            break;

        default:
            break;
        }
    }

    if (dev_started) {
        /* a dev has started audio test and result is pending  */
        dev_rpc_send(dev_started, "get_audio_test", "[]");
        dev_started->packet_send ++;
        g_audio_test_pending = 1;
        return;
    } else if (dev_to_start) {
        /* no audio test running and not all test done */
        dev_rpc_send(dev_to_start, "start_audio_test", "[80]");
        dev_to_start->audio_test_result = AUDIO_TEST_UNKN;
        dev_to_start->packet_send ++;
        g_audio_test_pending = 1;
        return;
    } else {
        g_audio_test_pending = 0;
        /* do nothing: no dev or all done */
    }

    return;
}

#endif
