#define _GNU_SOURCE
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include "sbox.h"
#include "iomux.h"
#include "util.h"
#include "timer.h"
#include <string.h>

#define MAX_SBOX_INST  8

static int nr_sbox_inst;
static io_inst_t sbox_io_inst[MAX_SBOX_INST];

typedef enum {
    EXP_IDEL,
    EXP_RUN,
    EXP_MAX,
} sbox_exp_state_t;

typedef struct {
    char             *exp_rsp;
    int              nr_recv_rsp;
    tmr_t            exp_tmr;
    sbox_exp_state_t state;
} sbox_exp_cb_t;

static sbox_exp_cb_t exp_cb;

typedef struct {
    void (*cb_func)(int code, void* priv);
    void *priv;
} sbox_cb_ctx_t;

static int sbox_set_attribs(int fd, int speed, int parity)
{
    struct termios tty;
    // get terminal's attributes
    tcgetattr(fd, &tty);
    /// set local mode options
    tty.c_lflag |= (ICANON);
    tty.c_lflag &= ~(ECHO | ECHOE | ISIG | IEXTEN);
    /// set control mode options
    tty.c_cflag |= (CLOCAL | CREAD);
    // disable hardware flow control
    tty.c_cflag &= ~CRTSCTS;
    // set how many bits in a character
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    // we don't need prity checking
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    /// set input mode options
    // disable input parity checking, this
    tty.c_iflag &= ~(INPCK | PARMRK | IGNBRK | BRKINT | ISTRIP
                     | INLCR | IGNCR | ICRNL | IXON);

    /// set output mode options
    tty.c_oflag &= ~OPOST;
    tty.c_cc[VMIN] = 1; 
    tty.c_cc[VTIME] = 0;
    cfsetspeed (&tty, speed);
    tcflush(fd, TCIFLUSH);
    if (tcsetattr (fd, TCSANOW, &tty) != 0) {
        RT_ERR("Failed to set attribute\n");
        return -1;
    }
    return 0;
}

static int sbox_set_blocking(int fd, int should_block)
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (fd, &tty) != 0) {
        RT_ERR("Failed to set attribute\n");
        return -1;
    }

    tty.c_cc[VMIN]  = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 5;           

    if (tcsetattr (fd, TCSANOW, &tty) != 0) {
        RT_ERR("Failed to set attribute\n");
        return -1;
    }
    return 0;
}

static int sbox_recv(io_inst_t* inst)
{
    char buf[32];
    int rc;
    sbox_cb_ctx_t* ctx = exp_cb.exp_tmr.data;

    bzero(buf, sizeof(buf));

    rc = read(inst->fd, buf, sizeof(buf));
    if (rc < 0) return -1;
    
    if (exp_cb.state == EXP_IDEL)
        return 0;

    if (strcasestr(buf, exp_cb.exp_rsp))
        exp_cb.nr_recv_rsp++;
    else
        return 0;

    if (exp_cb.nr_recv_rsp == nr_sbox_inst) {
        (*ctx->cb_func)(0, ctx->priv);
        del_timer(&(exp_cb.exp_tmr));
        exp_cb.nr_recv_rsp = 0;
        exp_cb.exp_rsp = NULL;
        exp_cb.state = EXP_IDEL;
        free(ctx);
    }

    return 0;
}

int sbox_send(char* command)
{
    int i, rc;

    for (i = 0; i < nr_sbox_inst; i++) {
        rc = write(sbox_io_inst[i].fd, command, strlen(command));
        if (rc < 0) {
            return rc;
        }
    }

    return 0;
}

static void sbox_rsp_timeout(int tid, void* data)
{
    sbox_cb_ctx_t* ctx = data;

    RT_ERR("%d shield box doesn't response in time.\n", nr_sbox_inst - exp_cb.nr_recv_rsp);

    (*ctx->cb_func)(-1, ctx->priv);
    exp_cb.nr_recv_rsp = 0;
    exp_cb.exp_rsp = NULL;
    exp_cb.state = EXP_IDEL;
    free(ctx);
}

int sbox_expect(char* rsp, void (*handler)(int rc, void*), void* priv)
{
    if (nr_sbox_inst == 0) {
        (*handler)(0, priv);
        return 0;
    }
    if (exp_cb.state != EXP_IDEL) {
        RT_ERR("Another shield box operation is onging\r\n");
        (*handler)(-1, priv);
        return 0;
    }

    sbox_cb_ctx_t* ctx = malloc(sizeof(sbox_cb_ctx_t));
    PANIC_IF(!ctx);

    ctx->cb_func = handler;
    ctx->priv = priv;

    exp_cb.exp_tmr.val = 8*10;
    exp_cb.exp_tmr.timer_id = 0;                 
    exp_cb.exp_tmr.fn = sbox_rsp_timeout;
    exp_cb.exp_tmr.repeat = 0; 
    exp_cb.exp_tmr.data = (void*)ctx;
    add_timer(&exp_cb.exp_tmr);
    exp_cb.state = EXP_RUN;
    exp_cb.nr_recv_rsp = 0;
    exp_cb.exp_rsp = rsp;
    
    return 0;
}

int sbox_connect()
{
    int i, rc;

    for (i = 0; i < nr_sbox_inst; i++) {
        rc = iomux_add(&(sbox_io_inst[i]));
        if (rc < 0) 
            return rc;
    }
    return rc;
}


int sbox_init(char* sbox)
{
    int fd;
    int rc;

    if (nr_sbox_inst >= MAX_SBOX_INST) {
        RT_ERR("Two many shield box\n");
        return -1;
    }

    fd = open (sbox, O_RDWR | O_NOCTTY );
    if (fd < 0) {
        RT_ERR("Failed to open %s\n", sbox);
        return -1;
    }

    rc = sbox_set_attribs(fd, B9600, 0); 
    if (rc < 0) 
        goto error;
    rc = sbox_set_blocking(fd, 1);                
    if (rc < 0) 
        goto error;


    IO_INST_INIT(&(sbox_io_inst[nr_sbox_inst]), 
                 fd,
                 IO_EVENT_IN,
                 0,         /* nonblocking */
                 0,         /* oneshot */
                 sbox_recv,
                 NULL, 
                 NULL,
                 NULL);
    nr_sbox_inst++;
    
    return 0;

 error:
    close(fd);
    return rc;
}

