#include <time.h>
#include <stdlib.h>
#include "timer.h"

static tmr_cb_t       tmr_cb;
static tmr_cb_t       *tmr_cp = &tmr_cb;
static volatile ms_t  sys_msec = 0x7FFFFFFFFFFFFF00;

/* t1 is earlier than t2 */
#define time_before(t1, t2) ((long)((t1)-(t2)) < 0)
#define timer_before(tmr1, tmr2) (time_before(tmr1->expire, tmr2->expire))

/**
 * Update the current time.
 * Because all timer's expiring time is relative to current time, so we must
 * update current time after each time-consuming operations, e.g. epoll_wait.
 */
void time_update()
{
    int rc;
    static struct timespec tp;

    rc = clock_gettime(CLOCK_MONOTONIC, &tp);
    if (rc < 0) abort();

    sys_msec = (ms_t)(tp.tv_sec * 1000) + (ms_t)(tp.tv_nsec)/(1000*1000);

    return;
}


int timer_init()
{
    if (tmr_cp->init) {
        return -1;
    }
    list_init(&(tmr_cp->tmr_list));
    time_update();
    tmr_cp->tmr_cnt = 0;
    tmr_cp->init = 1;

    return 0;
}

void timer_deinit()
{
    list_node_t *node, *tmp;
    tmr_t *tmr;

    list_for_del(&(tmr_cp->tmr_list), node, tmp) {
        tmr = object_of(tmr_t, self, node);
        list_del(&(tmr->self));
        tmr_cp->tmr_cnt--;
    }
    tmr_cp->init = 0;
}

/**
 * Place the timer into timer queue.
 */
void add_timer(tmr_t* timer)
{
    list_node_t* node;
    tmr_t* tmr;

    del_timer(timer);

    timer->expire = sys_msec + timer->val * MS_PER_TICK;

    list_for(&(tmr_cp->tmr_list), node) {
        tmr = object_of(tmr_t, self, node);
        if (timer_before(timer, tmr)) {
            break;
        }
    }
    list_node_init(&(timer->self));
    list_ins_before(node, &(timer->self));

    tmr_cp->tmr_cnt++;
}


void del_timer(tmr_t* timer)
{
    if (timer->self.next != NULL && timer->self.prev != NULL) {
        list_del(&(timer->self));
        tmr_cp->tmr_cnt--;
    } else {
        return;  /* timer is not running */
    }
}


/**
 * Do callbacks for all the expired timer, restart the timer
 * if it's repeatitive.
 */
void proc_timer()
{
    tmr_t* tmr;
    list_node_t *node;

    for (;;) {
        if (tmr_cp->tmr_cnt == 0) {
            break;
        }

        node = list_first(&(tmr_cp->tmr_list));
        tmr = object_of(tmr_t, self, node);

        if (time_before(tmr->expire, sys_msec)) {
            del_timer(tmr);
            if (tmr->repeat) {
                add_timer(tmr);
            }
            tmr->fn(tmr->timer_id, tmr->data);
        } else {
            break;
        }
    }
}

/**
 * Find out how much time can we sleep before we need to
 * wake up to handle the timer.
 */
int get_next_timeout(ms_t* tick)
{
    list_node_t* node;
    tmr_t* tmr;

    if (tmr_cp->tmr_cnt == 0) {       /* no timer yet */
        return -1;
    }

    node = list_first(&(tmr_cp->tmr_list));

    tmr = object_of(tmr_t, self, node);

    if (time_before(tmr->expire, sys_msec)) {
        *tick = 0;
    } else {
        *tick = tmr->expire - sys_msec;
    }

    return 0;
}
