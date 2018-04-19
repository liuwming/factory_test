#ifndef CMD_H
#define CMD_H

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "rtt_interface.h"
#include "iomux.h"

extern int alive;

typedef enum {
    RT_CMD_PROTO_UNIX,
    RT_CMD_PROTO_UDP,
} rt_cmd_proto_t;

#define rt_cmd_interface "/tmp/rt_cmd_interface"
#define CMD_INTERFACE_PORT 43210
#define MAX_RT_CLIENT   5


typedef struct client {
    io_inst_t           io;
    int                 busy;
    rt_cmd_proto_t      proto;
    struct sockaddr_in  addr;   /* client address */
} rt_client_t;

typedef int (*cmd_handler_t)(rt_client_t*, void*);

extern int cmd_init(char*);
extern void clnt_broadcast_res(rt_cmd_result_t* res);
extern void clnt_result(rt_client_t* clnt, rt_cmd_type_t type_code, int code);
extern void clnt_result2(rt_client_t* clnt, rt_cmd_result_t* res);

#endif
