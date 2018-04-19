/*#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <string.h>
#include <sys/select.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>*/
#include "rtt_interface.h"
#include <WinSock2.h>
#include <stdio.h>
#include <errno.h>

#pragma comment(lib, "WS2_32.lib")

#define SERVERPORT 43210	//服务端口号


typedef struct {
    SOCKET fd;
    int proto;
    SOCKADDR_IN udpaddr;
} rtt_conn_t;

rtt_handle_t rtt_connect(int timeout, int* err, int proto, char* ip)
{
    //struct sockaddr_un servaddr;
    int rc;
    //fd_set wset; 
    //struct timeval tv; 

    rtt_conn_t *conn = (rtt_conn_t*)malloc(sizeof(rtt_conn_t));
    if (conn == NULL) {
        *err = RT_ERR_NOMEM;
        return NULL;
    }
    memset(conn, sizeof(rtt_conn_t), 0);

    if (proto == RTT_PROTO_UNIX_SOCKET) {
        goto error;
    } else {
        //加载套接字库
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;

        wVersionRequested = MAKEWORD(2, 2);

        err = WSAStartup(wVersionRequested, &wsaData);
        if (err != 0) {
            /* Tell the user that we could not find a usable */
            /* WinSock DLL.                                  */
            goto error;
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
            goto error;
        }

        conn->proto = RTT_PROTO_UDP;
        conn->fd=socket(AF_INET, SOCK_DGRAM, 0);
        if (conn->fd < 0) {
            goto error;
        }
        memset(&(conn->udpaddr),sizeof(conn->udpaddr), 0);
        //conn->udpaddr.sin_family = AF_INET;

        //SOCKADDR_IN conn->udpaddr;// *.h
        conn->udpaddr.sin_addr.S_un.S_addr = inet_addr(ip);
        conn->udpaddr.sin_family = AF_INET;
        conn->udpaddr.sin_port = htons(SERVERPORT);

        /*if (inet_aton(ip, &(conn->udpaddr.sin_addr)) == 0) 
            goto error;
            
        conn->udpaddr.sin_port=htons(UDP_PORT);*/
        rc = connect(conn->fd, (SOCKADDR*)&(conn->udpaddr), sizeof(conn->udpaddr));
        if (rc < 0)
            goto error;
    }

    /*rc = fcntl(conn->fd, F_SETFL, ~O_NONBLOCK & flag);
    if (rc < 0) {
        goto error;
    }*/

    return (rtt_handle_t)conn;

 error:
    free(conn);
    *err = errno;
    //关闭套接字
    closesocket(conn->fd);

    //终止套接字库的使用
    WSACleanup();
    return NULL;
}

int rtt_discon(rtt_handle_t hdl)
{
    rtt_conn_t *con = hdl;

    if (con == NULL) return -1;
    
    closesocket(con->fd);
    free(con);
    return 0;
}

/* 
 * send command to Radio Test Tool
 */
int rtt_send(rtt_handle_t hdl, rt_cmd_t* cmd)
{
    rtt_conn_t *con = hdl;
    int rc;

    if (!con || !cmd) return -1;

    do {
        rc = sendto(con->fd, (const char*)cmd, sizeof(rt_cmd_t), 
                        0, (SOCKADDR *)&(con->udpaddr),
                        sizeof(con->udpaddr));
    } while (rc < 0 && errno == EINTR);

    return rc;
}

/* 
 * recv result from Radio Test Tool
 */
int rtt_recv(rtt_handle_t hdl, rt_cmd_result_t* result, rt_cmd_type_t type)
{
    rtt_conn_t *con = hdl;
    int rc;

    if (!con || !result) return -1;
    
    do {
        rc = recvfrom(con->fd, (char *)result, sizeof(rt_cmd_result_t),
                          0, NULL, NULL);
    } while ((rc < 0 && errno == EINTR) || 
             (rc > 0 && type != RT_CMD_MAX && result->type != type));

    return rc;
}
