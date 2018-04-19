/************************************************************************
** App Name: Yeelight WiFi Test App
** Environment: Win7(64bit) + VC2013
** Note: This program use yeelight_wifi_test_dll.dll¡±£¬you can find this
         file in folder "yl_rtt_dll".
         the App code all in this file. 
         note that the generated exe file correct running need a 
         "yeelight_wifi_test_dll.dll" copied to the same directory.
** Support: weiwei@yeelink.net
            songyuanrui@yeelink.net
************************************************************************/

#pragma comment(lib,"./yl_rtt_dll/yeelight_wifi_test_dll.lib")

#include "stdafx.h"
#include "./yl_rtt_dll/rtt_interface.h"
#include "windows.h"

#include <stdio.h>
#include <string.h>

#define res(field) result->body.status[i].field

static void print_test_status(rt_cmd_result_t* result)
{
    int i;
    printf("--------%s--------\n", result->type == RT_CMD_STATUS ?
        "status" : "result");
    for (i = 0; i < result->dev_count; i++) {
        printf("\t+Device: Status[%s] MAC[%s] IP[%s]\n", res(state), res(mac), res(ip));
        if (strcmp(res(state), STATE_DEV_CONN) == 0) {
            printf("\t\t-firmware_ver: %s\n", res(fw_ver));
            printf("\t\t-avg_rssi: %d\n", res(avg_rssi));
            printf("\t\t-min_rssi: %d\n", res(min_rssi));
            printf("\t\t-max_rssi: %d\n", res(max_rssi));
            printf("\t\t-packet_send: %d\n", res(packet_send));
            printf("\t\t-packet_recv: %d\n", res(packet_recv));
            printf("\t\t-brs:%d\n", res(brs));
            printf("\t\t-brf:%d\n", res(brf));
        }
        printf("\n");
    }
}

static void test_stop(rtt_handle_t hdl)
{
    rt_cmd_result_t res;
    rt_cmd_t cmd;
    int rc;

    cmd.type = RT_CMD_STOP;

    rtt_send(hdl, &cmd);
    rc = rtt_recv(hdl, &res, RT_CMD_STOP);

    if (rc < 0 || (rc > 0 && res.ret_code != RT_ERR_OK)) {
        printf("Failed to exe command %d.\n", res.ret_code);
        return;
    }
    else {
        rc = rtt_recv(hdl, &res, RT_CMD_STOP);
        if (rc <= 0) return;
        print_test_status(&res);
    }

}

static void test_start(rtt_handle_t hdl)
{
    rt_cmd_result_t res;
    rt_cmd_t cmd;
    int rc;

    cmd.type = RT_CMD_START;

    rtt_send(hdl, &cmd);
    rc = rtt_recv(hdl, &res, RT_CMD_START);

    if (rc < 0 || (rc > 0 && res.ret_code != RT_ERR_OK)) {
        printf("Failed to exe command %d.\n", res.ret_code);
        return;
    }

    printf("Command Begin.\n");
    while (1) {
        rc = rtt_recv(hdl, &res, RT_CMD_MAX); /* receive all messages from daemon */
        if (rc <= 0) break;
        if (res.type == RT_CMD_STATUS) {
            printf("Test status update.\n");
            print_test_status(&res);
        }
        if (res.type == RT_CMD_RESULT) {
            printf("Test Result.\n");
            print_test_status(&res);
            break;
        }
    }
}

static void control_dev(rtt_handle_t hdl, char* mac)
{
    rt_cmd_result_t res;
    rt_cmd_t cmd;
    int rc;

    cmd.type = RT_CMD_CTRL_DEV;
    strcpy_s(cmd.body.mac, mac);

    rtt_send(hdl, &cmd);
    rc = rtt_recv(hdl, &res, RT_CMD_CTRL_DEV);

    if (rc < 0 || (rc > 0 && res.ret_code != RT_ERR_OK)) {
        printf("Failed to exe command %d.\n", res.ret_code);
        return;
    }
    else {
        printf("command execute successfully.\n");
    }
}

static void ota_dev(rtt_handle_t hdl, char* mac)
{
    rt_cmd_result_t res;
    rt_cmd_t cmd;
    int rc;

    cmd.type = RT_CMD_OTA_DEV;
    strcpy_s(cmd.body.mac, mac);

    rtt_send(hdl, &cmd);
    rc = rtt_recv(hdl, &res, RT_CMD_OTA_DEV);

    if (rc < 0 || (rc > 0 && res.ret_code != RT_ERR_OK)) {
        printf("Failed to exe command %d.\n", res.ret_code);
        return;
    }
    else {
        printf("command execute successfully.\n");
    }
}

int _tmain(int argc, _TCHAR* argv[])
{

    rtt_handle_t hdl;
    int err;
    int c;
    char mac[32];

    setvbuf(stdout, NULL, _IONBF, 0);

    hdl = rtt_connect(5, &err, RTT_PROTO_UDP, "192.168.1.1");
    if (!hdl) {
        printf("connect to RTT server failed %d, ip=%s.\n", err, argv[1]);
        //system("pause");
        return -1;
    }

    while ((c = getchar()) != 'q') {
        switch (c) {
        case '1':
            test_start(hdl);
            break;
        case '2':
            printf("input mac address:");
            scanf_s("%s", mac, sizeof(mac));
            printf("control mac %s\n", mac);
            control_dev(hdl, mac);
            break;
        case '3':
            test_stop(hdl);
            break;
        case '4':
            printf("input mac address:");
            scanf_s("%s", mac, sizeof(mac));
            printf("control mac %s\n", mac);
            ota_dev(hdl, mac);
            break;
        case '5':
            break;
        case '6':
            break;
        default:
            break;
        }
    }

	return 0;
}

