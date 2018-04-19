#ifndef IOMUX_H
#define IOMUX_H

typedef struct iomux_cb {
    int                init;
    int                epollfd;
} iomux_cb_t;

#define MAX_EVENTS 32

#define IO_EVENT_IN     0x1
#define IO_EVENT_OUT    0x2
#define IO_EVENT_ERR    0x4

#define IO_INST_INIT(inst, f, et, nblk, os, rv, wr, er, p)  \
    do {                                                    \
        (inst)->fd = f;                                     \
        (inst)->evt = et;                                   \
        (inst)->nonblock = nblk;                            \
        (inst)->oneshot = os;                               \
        (inst)->on_recv = rv;                               \
        (inst)->on_writable = wr;                           \
        (inst)->on_err = er;                                \
        (inst)->priv = p;                                   \
    } while (0)

typedef struct io_inst {
    int           fd;
    int           evt;
    char          nonblock;
    char          oneshot;
    char          error;        /* used internally */
    int           (*on_recv)(struct io_inst*);
    int           (*on_writable)(struct io_inst*);
    int           (*on_err)(struct io_inst*);
    void*         priv;
} io_inst_t;

extern int  iomux_init(void);
extern int  iomux_deinit(void);
extern int  iomux_add(io_inst_t* new);
extern int  iomux_del(io_inst_t* inst);
extern int  iomux_mod(io_inst_t* change);
extern void iomux_loop(void);

#endif
