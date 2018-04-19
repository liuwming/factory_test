#ifndef TIMER_H
#define TIMER_H

#include "list.h"

#define MS_PER_TICK      100         /* milli-seconds per tick */

typedef unsigned long ms_t;

typedef void (*timer_func_t)(int timer_id, void* data);

typedef struct {
    list_node_t    self;
    int            timer_id;
    unsigned       val;           /* how many ticks? */
    ms_t           expire;
    int            repeat;
    timer_func_t   fn;
    void           *data;
} tmr_t;

typedef struct tmr_cb {
    list_t         tmr_list;
    int            init;
    int            tmr_cnt;
} tmr_cb_t;

extern int timer_init(void);
extern void timer_deinit(void);
extern void add_timer(tmr_t* timer);
extern void del_timer(tmr_t* timer);
extern void proc_timer(void);
extern int get_next_timeout(ms_t* ms);
extern void time_update(void);

#endif
