// UDP_client.cpp 
// environment：VC2013 + Win7 64bit
//

#include "stdafx.h"
#include "windows.h"

#include <stdio.h>
#include <string.h>
#include "../rtt/rtt_interface.h"



#define res(field) result->body.status[i].field

static void print_test_status(rt_cmd_result_t* result)
{
    int i;
    printf("--------%s--------\n" , result->type == RT_CMD_STATUS ? 
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
    } else {
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


int _tmain(int argc, char* argv[])
{
    rtt_handle_t hdl;
    int err;
    int c;
    char mac[32];

    setvbuf(stdout, NULL, _IONBF, 0);

    hdl = rtt_connect(5, &err, RTT_PROTO_UDP, "192.168.1.1");
    if (!hdl) {
        printf("connect to RTT server failed %d, ip=%s.\n", err, argv[1]);
        system("pause");
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

#if 0
int _tmain(int argc, _TCHAR* argv[])
{
    //加载套接字库
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        return -1;
    }

    /* Confirm that the WinSock DLL supports 2.2.*/
    /* Note that if the DLL supports versions greater    */
    /* than 2.2 in addition to 2.2, it will still return */
    /* 2.2 in wVersion since that is the version we      */
    /* requested.                                        */

    if (LOBYTE(wsaData.wVersion) != 2 ||
        HIBYTE(wsaData.wVersion) != 2) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        WSACleanup();
        return -1;
    }

    //创建套接字
    SOCKET sockClient = socket(AF_INET, SOCK_DGRAM, 0);
    if (INVALID_SOCKET == sockClient)
    {
        printf("socket() called failed! The error code is: %d\n", WSAGetLastError());
        return -1;
    }
    else
    {
        printf("socket() called succesful!\n");
    }

    SOCKADDR_IN addrServer;
    addrServer.sin_addr.S_un.S_addr = inet_addr("192.168.1.100");
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(SERVERPORT);

    //发送数据
    char send_str[] = "Hello yeelight";
    err = sendto(sockClient, send_str, strlen(send_str) + 1, 0, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
    if (SOCKET_ERROR == err)
    {
        printf("sendto() called failed! The error code is: %s\n", WSAGetLastError());
        return -1;
    }
    else
    {
        printf("sendto() called successful!\n");
    }

    //关闭套接字
    closesocket(sockClient);

    //终止套接字库的使用
    WSACleanup();

    system("pause");
    return 0;
}
#endif
