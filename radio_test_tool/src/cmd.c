#include "cmd.h"
#include "iomux.h"
#include "util.h"
#include "test.h"
#include "sbox.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <string.h>
#include <sys/select.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <rtt_interface.h>

static io_inst_t cmd_inst;
int alive = 1;
static rt_client_t client_pool[MAX_RT_CLIENT];
static rt_client_t udp_clnt;

void clnt_result(rt_client_t* clnt, rt_cmd_type_t type_code, int code)
{
    rt_cmd_result_t res;

    res.type = type_code;
    res.ret_code = code;

    if (clnt->proto == RT_CMD_PROTO_UNIX)
        write(clnt->io.fd, &res, sizeof(res));
    else
        sendto((clnt)->io.fd, &res, sizeof(res), 0,
               (struct sockaddr *)&((clnt)->addr),
               sizeof((clnt)->addr));
}

void clnt_result2(rt_client_t* clnt, rt_cmd_result_t* res)
{
    if ((clnt)->proto == RT_CMD_PROTO_UNIX)
        write((clnt)->io.fd, res, sizeof(*res));
    else
        sendto((clnt)->io.fd, res, sizeof(*res), 0,
               (struct sockaddr *)&((clnt)->addr),
               sizeof((clnt)->addr));
}

static int rt_start(rt_client_t* clnt, void* arg)
{
    int rc = 0;

    UNUSED_PARAM(arg);
    TRACE_IN();

    if (rt_running()) {
        clnt_result(clnt, RT_CMD_START, RT_ERR_BUSY);
        return 0;
    }

    clear_test();
    rc = start_test();
    if (rc) {
        RT_ERR("start test failed.\n");
    }

    clnt_result(clnt, RT_CMD_START, rc);

    return rc;
}

static int rt_stop(rt_client_t* clnt, void* arg)
{
    TRACE_IN();
    if (!rt_running()) {
        RT_ERR("test already stopped\n");
        clnt_result(clnt, RT_CMD_STOP, RT_ERR_INVALID);
        return 0;
    }
    clnt_result(clnt, RT_CMD_STOP, RT_ERR_OK);
    report_status(NULL, RT_CMD_RESULT);
    stop_test();
    return 0;
}

static int rt_status(rt_client_t* clnt, void* arg)
{
    TRACE_IN();
    if (!rt_running()) {
        RT_ERR("test already stopped\n");
        clnt_result(clnt, RT_CMD_STATUS, RT_ERR_INVALID);
        return 0;
    }
    clnt_result(clnt, RT_CMD_STATUS, RT_ERR_OK);
    report_status(clnt, RT_CMD_STATUS);
    return 0;
}

static int rt_result(rt_client_t* clnt, void* arg)
{
    TRACE_IN();
    if (!rt_running()) {
        RT_ERR("test already stopped\n");
        clnt_result(clnt, RT_CMD_RESULT, RT_ERR_INVALID);
        return 0;
    }
    clnt_result(clnt, RT_CMD_RESULT, RT_ERR_OK);
    report_status(clnt, RT_CMD_RESULT);
    return 0;
}

static int rt_set_param(rt_client_t* clnt, void* arg)
{
    rt_param_t* param = arg;

    TRACE_IN();

    if (rt_running()) {
        RT_ERR("test already started\n");
        clnt_result(clnt, RT_CMD_SET_PARAM, RT_ERR_BUSY);
        return 0;
    }
    clnt_result(clnt, RT_CMD_SET_PARAM, RT_ERR_OK);
    set_param(param);

    return 0;
}

static int rt_get_param(rt_client_t* clnt, void* arg)
{
    rt_cmd_result_t cmd;
    rt_param_t* p = get_param();

    TRACE_IN();

    clnt_result(clnt, RT_CMD_GET_PARAM, RT_ERR_OK);

    cmd.type = RT_CMD_GET_PARAM;
    cmd.ret_code = RT_ERR_OK;
    memcpy(&(cmd.body.cmd_param), p, sizeof(rt_param_t));
    clnt_result2(clnt, &cmd);

    return 0;
}

static int rt_ctrl_dev(rt_client_t* clnt, void* arg)
{
    char* mac = arg;

    TRACE_IN();

    if (rt_running()) {
        clnt_result(clnt, RT_CMD_CTRL_DEV, RT_ERR_BUSY);
        return 0;
    }

    ctrl_dev3(mac, CTRL_DEV_CMD_TEST_PASS, clnt);

    return 0;
}

static int rt_fail_dev(rt_client_t* clnt, void* arg)
{
    char* mac = arg;

    TRACE_IN();

    if (rt_running()) {
        clnt_result(clnt, RT_CMD_FAIL_DEV, RT_ERR_BUSY);
        return 0;
    }

    ctrl_dev3(mac, CTRL_DEV_CMD_TEST_FAIL, clnt);

    return 0;
}

static int rt_ota_dev(rt_client_t* clnt, void* arg)
{
    char* mac = arg;

    TRACE_IN();

    if (rt_running()) {
        clnt_result(clnt, RT_CMD_OTA_DEV, RT_ERR_BUSY);
        return 0;
    }
    clnt_result(clnt, RT_CMD_OTA_DEV, RT_ERR_OK);

    ctrl_dev(mac, CTRL_DEV_CMD_OTA);

    return 0;
}

/* attach remote controller*/
static int rt_add_beacon(rt_client_t* clnt, void* arg)
{
    beacon_cmd_param_t *beacon = arg;

    TRACE_IN();

    strcpy(universal_beacon_mac, beacon->mac_beacon); // TODO: refactor when multi-thread is enabled
    strcpy(universal_beacon_key, beacon->key); // TODO: refactor if multi-thread is enabled
    ctrl_dev(beacon->mac_dev, CTRL_DEV_CMD_ADD_BEACON);
    clnt_result(clnt, RT_CMD_ADD_BEACON, RT_ERR_OK);

    return 0;
}

static int rt_quit(rt_client_t* clnt, void* arg)
{
    TRACE_IN();
    clnt_result(clnt, RT_CMD_QUIT, RT_ERR_OK);
    alive = 0;
    return 0;
}


static int rt_start_audio_test(rt_client_t* clnt, void* arg)
{
    char* mac = arg;

    TRACE_IN();
    clnt_result(clnt, RT_CMD_START_AUDIO_TEST, RT_ERR_OK);
    ctrl_dev(mac, CTRL_DEV_CMD_AUDIO_TEST);

    return 0;
}


static int rt_start_ble_test(rt_client_t* clnt, void* arg)
{
    char* mac = arg;

    TRACE_IN();
    clnt_result(clnt, RT_CMD_START_BLE_TEST, RT_ERR_OK);
    ctrl_dev(mac, CTRL_DEV_CMD_BLE_TEST);

    return 0;
}

static int rt_get_audio_result(rt_client_t* clnt, void* arg)
{
    char* mac = arg;

    TRACE_IN();
    clnt_result(clnt, RT_CMD_GET_AUDIO_TEST, RT_ERR_OK);
    ctrl_dev(mac, CTRL_DEV_CMD_AUDIO_RES);

    return 0;
}

static void _rt_close_sbox(int code, void* priv)
{
    rt_client_t* clnt = priv;

    if (code < 0) {
        RT_ERR("ERROR ERROR Failed to receive response from shield box\n");
        clnt_result(clnt, RT_CMD_CLOSE_SBOX, RT_ERR_FAILURE);
    } else {
        clnt_result(clnt, RT_CMD_CLOSE_SBOX, RT_ERR_OK);
    }
}

static int rt_close_sbox(rt_client_t* clnt, void* arg)
{
    TRACE_IN();

    RT_ERR("closing the shield box ...\n");

    sbox_send("close\r\n");
    sbox_expect("ready", _rt_close_sbox, (void*)clnt);

    return 0;
}

static void _rt_open_sbox(int code, void* priv)
{
    rt_client_t* clnt = priv;

    if (code < 0) {
        RT_ERR("ERROR ERROR Failed to receive response from shield box\n");
        clnt_result(clnt, RT_CMD_OPEN_SBOX, RT_ERR_FAILURE);
    } else {
        clnt_result(clnt, RT_CMD_OPEN_SBOX, RT_ERR_OK);
    }
}

static int rt_open_sbox(rt_client_t* clnt, void* arg)
{
    TRACE_IN();

    RT_ERR("opening the shield box ...\n");

    sbox_send("open\r\n");
    sbox_expect("ok", _rt_open_sbox, (void*)clnt);

    return 0;
}


static cmd_handler_t rt_cmd_handler[] = {
    rt_start,
    rt_stop,
    rt_status,
    rt_result,
    rt_set_param,
    rt_get_param,
    rt_ctrl_dev,
    rt_fail_dev,
    rt_ota_dev,
    rt_quit,
    rt_open_sbox,
    rt_close_sbox,
    rt_add_beacon,
    rt_start_audio_test,
    rt_start_ble_test,
    rt_get_audio_result,
};


static void cmd_close_client(rt_client_t* clnt)
{
    iomux_del(&(clnt->io));
    close(clnt->io.fd);
    clnt->busy = 0;
    clnt->proto = 0;
}

static int cmd_exe(rt_client_t* clnt, rt_cmd_t *cmd)
{
    return rt_cmd_handler[cmd->type](clnt, &(cmd->body));
}

static rt_client_t* find_rt_client()
{
    int i;
    for (i = 0; i < ARR_SIZE(client_pool); i++) {
        if (client_pool[i].busy == 0)
            return &client_pool[i];
    }
    return NULL;
}

static int cmd_recv_udp(io_inst_t* inst)
{
    int rc = 0;
    rt_cmd_t cmd;
    socklen_t len = sizeof(udp_clnt.addr);

    rc = recvfrom(inst->fd, &cmd, sizeof(cmd), 0, (struct sockaddr *)&(udp_clnt.addr), &len);
    if (rc <= 0) {
        RT_ERR("Failed to receive message from UDP peer.\n");
        return -1;
    }
    udp_clnt.io.fd = inst->fd;
    udp_clnt.proto = RT_CMD_PROTO_UDP;
    udp_clnt.busy = 1;

    assert(rc == sizeof(cmd));
    assert(cmd.type >=0 && cmd.type < RT_CMD_MAX);

    rc = cmd_exe(&udp_clnt, &cmd);

    return rc;
}


static int cmd_recv(io_inst_t* inst)
{
    int rc = 0;
    int fd;
    rt_client_t *clnt;
    rt_cmd_t cmd;

    /* new client connected */
    if (inst->fd == cmd_inst.fd) {
        fd = accept(inst->fd, NULL, NULL);
        if (fd < 0) {
            RT_ERR("interface connected failed .\n");
            return -1;
        }

        clnt = find_rt_client();
        if (!clnt) {
            RT_ERR("max client reached, reject.\n");
            close(fd);
            return 0;
        } else {
            clnt->busy = 1;
            clnt->proto = RT_CMD_PROTO_UNIX;

            IO_INST_INIT(&(clnt->io),
                         fd,
                         IO_EVENT_IN,
                         1,         /* nonblocking */
                         0,         /* oneshot */
                         cmd_recv,
                         NULL,
                         NULL,
                         clnt
                        );
            rc = iomux_add(&(clnt->io));
        }
    } else {  /* recv message from client */
        clnt = inst->priv;
        do {
            rc = recv(inst->fd, &cmd, sizeof(cmd), 0);
        } while (rc < 0 && errno == EINTR);

        if (rc < 0) {
            cmd_close_client(clnt);
        } else if (rc == 0) {
            RT_ERR("Client leaved.\n");
            cmd_close_client(clnt);
        } else {
            assert(rc == sizeof(cmd));
            assert(cmd.type >=0 && cmd.type < RT_CMD_MAX);
            rc = cmd_exe(clnt, &cmd);
        }
    }
    return rc;
}

static int cmd_creat_interface(rt_cmd_proto_t proto, const char* server)
{
    struct sockaddr_un servaddr;
    struct sockaddr_in udpaddr;

    int rc, fd;

    if (proto == RT_CMD_PROTO_UNIX) {
        fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
        if (fd < 0) {
            goto err;
        }
        unlink(server);

        bzero(&servaddr, sizeof(servaddr));
        servaddr.sun_family = AF_LOCAL;
        strcpy(servaddr.sun_path, server);

        rc = bind(fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
        if (rc < 0) {
            goto err;
        }

        rc = listen(fd, 15);
        if (rc < 0) {
            goto err;
        }

        IO_INST_INIT(&(cmd_inst),
                     fd,
                     IO_EVENT_IN,
                     1,         /* nonblocking */
                     0,         /* oneshot */
                     cmd_recv,
                     NULL,
                     NULL,
                     &(cmd_inst)
                    );
    } else {
        fd=socket(AF_INET, SOCK_DGRAM, 0);
        if (fd < 0) {
            goto err;
        }
        bzero(&udpaddr,sizeof(udpaddr));
        udpaddr.sin_family = AF_INET;
        if (inet_aton(server, &(udpaddr.sin_addr)) == 0)
            goto err;
        udpaddr.sin_port=htons(CMD_INTERFACE_PORT);

        rc = bind(fd, (struct sockaddr *)&udpaddr, sizeof(udpaddr));
        if (rc < 0) {
            RT_ERR("Failed to bind UDP socket.\n");
            goto err;
        }
        IO_INST_INIT(&(cmd_inst),
                     fd,
                     IO_EVENT_IN,
                     1,         /* nonblocking */
                     0,         /* oneshot */
                     cmd_recv_udp,
                     NULL,
                     NULL,
                     &(cmd_inst)
                    );
    }
    rc = iomux_add(&(cmd_inst));
    if (rc < 0) {
        return -1;
    }

    return 0;
 err:
    return -1;
}

void clnt_broadcast_res(rt_cmd_result_t* res)
{
    int i;
    for (i = 0; i < ARR_SIZE(client_pool); i++) {
        if (client_pool[i].busy == 1)
            clnt_result2(&(client_pool[i]), res);
    }
    if (udp_clnt.busy)
        clnt_result2(&(udp_clnt), res);
}

int cmd_init(char *ip)
{
    int rc;

    /* must be domain socket */
    if (!ip) {
        rc = cmd_creat_interface(RT_CMD_PROTO_UNIX, rt_cmd_interface);
        if (rc < 0) {
            RT_ERR("failed to create Unix domian socket interface.\n");
            return rc;
        }
    } else {
        rc = cmd_creat_interface(RT_CMD_PROTO_UDP, ip);
        if (rc < 0) {
            RT_ERR("failed to create UDP socket interface.\n");
            return rc;
        }
    }

    return 0;
}
