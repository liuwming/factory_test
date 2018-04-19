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
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "rtt_interface.h"

typedef struct {
    int fd;
    int proto;
    struct sockaddr_in udpaddr;
} rtt_conn_t;

rtt_handle_t rtt_connect(int timeout, int* err, int proto, char* ip)
{
    struct sockaddr_un servaddr;
    int rc, flag;
    fd_set wset; 
    struct timeval tv; 

    rtt_conn_t *conn = (rtt_conn_t*)malloc(sizeof(rtt_conn_t));
    if (conn == NULL) {
        *err = RT_ERR_NOMEM;
        return NULL;
    }
    bzero(conn, sizeof(rtt_conn_t));

    if (proto == RTT_PROTO_UNIX_SOCKET) {
        conn->proto = RTT_PROTO_UNIX_SOCKET;
        conn->fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
        if (conn->fd < 0) {
            goto error;
        }

        bzero(&servaddr, sizeof(servaddr));
        servaddr.sun_family = AF_LOCAL;
        strcpy(servaddr.sun_path, rt_cmd_interface);

        /* no timeout case */
        if (timeout == 0) {
            rc = connect(conn->fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
            if (rc < 0) {
                goto error;
            }
            return (rtt_handle_t)conn;
        }

        flag = fcntl(conn->fd, F_GETFL, NULL);
        if (flag < 0) {
            goto error;
        }
        rc = fcntl(conn->fd, F_SETFL, O_NONBLOCK | flag);
        if (rc < 0) {
            goto error;
        }

        rc = connect(conn->fd, (struct sockaddr*)&servaddr, sizeof(servaddr));

        if (rc < 0) {
            if (errno == EINPROGRESS) {
                do { 
                    tv.tv_sec = timeout; 
                    tv.tv_usec = 0; 
                    FD_ZERO(&wset); 
                    FD_SET(conn->fd, &wset); 

                    rc = select(conn->fd+1, NULL, &wset, NULL, &tv); 

                    if (rc < 0 && errno != EINTR) { 
                        goto error;
                    } else if (rc > 0) { 
                        int valopt; 
                        socklen_t sl = sizeof(int); 
                        if (getsockopt(conn->fd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &sl) < 0) { 
                            goto error;
                        } 
                        if (valopt) {
                            goto error;
                        } 
                        break; 
                    } else {          /* connect timeout */
                        goto error;
                    } 
                } while (1); 
            } else {
                goto error;
            }
        }
        rc = fcntl(conn->fd, F_SETFL, ~O_NONBLOCK & flag);
        if (rc < 0) {
            goto error;
        }
    } else {
        conn->proto = RTT_PROTO_UDP;
        conn->fd=socket(AF_INET, SOCK_DGRAM, 0);
        if (conn->fd < 0) {
            goto error;
        }
        bzero(&(conn->udpaddr),sizeof(conn->udpaddr));
        conn->udpaddr.sin_family = AF_INET;

        if (inet_aton(ip, &(conn->udpaddr.sin_addr)) == 0) 
            goto error;
        conn->udpaddr.sin_port=htons(UDP_PORT);
        rc = connect(conn->fd, (struct sockaddr*)&(conn->udpaddr), sizeof(conn->udpaddr));
        if (rc < 0)
            goto error;
    }

    return (rtt_handle_t)conn;

 error:
    *err = errno;
    if (conn->fd >= 0)
        close(conn->fd);
    free(conn);
    return NULL;
}

int rtt_discon(rtt_handle_t hdl)
{
    rtt_conn_t *con = hdl;

    if (con == NULL) return -1;
    
    close(con->fd);
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
        if (con->proto == RTT_PROTO_UNIX_SOCKET)
            rc = write(con->fd, cmd, sizeof(rt_cmd_t));
        else 
            rc = sendto(con->fd, cmd, sizeof(rt_cmd_t), 
                        0, (struct sockaddr *)&(con->udpaddr),
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
        if (con->proto == RTT_PROTO_UNIX_SOCKET)
            rc = read(con->fd, result, sizeof(rt_cmd_result_t));
        else 
            rc = recvfrom(con->fd, result, sizeof(rt_cmd_result_t),
                          0, NULL, NULL);
        printf("rtt_recv type, expecting %d, received = %d\r\n ", type, result->type);
    } while ((rc < 0 && errno == EINTR) || 
             (rc > 0 && type != RT_CMD_MAX && result->type != type));

    return rc;
}

/*
 get status of nth device
*/
rt_status_t* rtt_get_status(rt_cmd_result_t* result, int idx)
{
    return &(result->body.status[idx]);
}
