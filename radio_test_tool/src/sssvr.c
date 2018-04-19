/* simple stupid server to server http file downloading */

#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>

#define MSS         1024        /* bytes per send */
#define SENT_DELAY  70000       /* microseconds */

static char *file_contents;
static int file_len;
const char* resp_header = 
    "HTTP/1.1 200 OK\r\n"
    "Server: sss/1.8.0\r\n"
    "Content-Type: application/octet-stream\r\n"
    "Content-Length: %d\r\n"
    "Connection: keep-alive\r\n"
    "Accept-Ranges: bytes\r\n\r\n";


void sig_handler(int signum) 
{
    printf("receive signal %d\n", signum);
    return;
}


static void err_exit(char *msg, int code)
{
    printf("%s\n", msg);
    exit(code);
}

static void* srv_clnt(void* arg)
{
    int fd;
    int rc;
    char buf[512];
    int offset = 0;
    int cnt, to_send;
    int i = 1;

    fd = (int)arg;
    printf("client thread created for fd %d\n", fd);    

    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&i, sizeof(i));

    bzero(buf, sizeof(buf));    

 again:
    do {
        rc = recv(fd, buf + offset, sizeof(buf) - offset, 0);
    } while (rc < 0 && errno == EINTR);
    
    if (rc <= 0) {
        printf("failed to get client request, or client closed\n");
        goto exit;
    }

    offset += rc;

    if (strstr(buf, "\r\n\r\n")) {
        printf("begin to serve\n");
        rc = sprintf(buf, resp_header, file_len);
        cnt = send(fd, buf, rc, 0);
        if (cnt <= 0) {
            printf("failed to send http response header\n");
            goto exit;
        }
        
        for (cnt = 0; cnt < file_len; cnt += to_send) {
            to_send = file_len - cnt;
            if (to_send > MSS) to_send = MSS;
            rc = send(fd, file_contents + cnt, to_send, 0);
            if (rc <= 0) {
                printf("failed to send contents\n");
                goto exit;
            }
            usleep(SENT_DELAY);
        }
        printf("all data served, totoal sent %d\n", cnt);
    } else {
        goto again;
    }

 exit:
    close(fd);
    printf("client thread completed\n");
}

static void srv_file()
{
    int clnt_fd;
    int listen_fd;
    int optval = 1;
    int clientlen; 
    struct sockaddr_in clientaddr; 
    struct sockaddr_in serveraddr;
    pthread_t tid;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        err_exit("failed to open socket\n", -1);
    }

    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, 
               (const void *)&optval , sizeof(int));
    
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET; 
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(80);
    
    if (bind(listen_fd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
        err_exit("failed to bind 80 port\n", -1);
    }

    if (listen(listen_fd, 5) < 0) {
        err_exit("failed to listen\n", -1);
    }

    while (1) {
        clnt_fd = accept(listen_fd, (struct sockaddr *) &clientaddr, &clientlen);
        if (clnt_fd < 0) {
            printf("failed to accept new connection\n");
            continue;
        }
        if (pthread_create(&tid, NULL, srv_clnt, (void*)clnt_fd) < 0) {
            printf("failed to create thread to serve client\n");
            close(clnt_fd);
            continue;
        }
        pthread_detach(tid);    /* to avoid resource leakage since we won't call pthread_join */
    }
}

int main(int argc, char** argv)
{
    struct stat st;
    int fd;

    if (argc < 2) {
        err_exit("no file speficied\n", -1);
    }

    if (stat(argv[1], &st) < 0) {
        err_exit("file not found\n", -1);
    }

    fd = open(argv[1], 0);
    if (fd < 0) {
        err_exit("failed to open file\n", -1);
    }

    file_contents = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_contents == NULL) {
        err_exit("failed to read file\n", -1);
    }
    file_len = st.st_size;
    
    printf("file len %d\n", file_len);

    signal(SIGPIPE, sig_handler);

    srv_file();

    return 0;
}
