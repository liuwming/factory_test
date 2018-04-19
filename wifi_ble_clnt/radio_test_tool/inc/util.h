#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ARR_SIZE(array)        (sizeof(array)/sizeof(array[0]))
#define RT_ERR(fmt, msg...)    printf(fmt, ##msg);
#define PANIC_IF(cond)                          \
    do {                                        \
        if (cond) abort();                      \
    } while (0)

#define UNUSED_PARAM(p) if (p==p)
#define TRACE_IN()  RT_ERR("Function %s entered.\n", __FUNCTION__)
#define TRACE_OUT() RT_ERR("Function %s left.\n", __FUNCTION__)

extern int rtt_exec(char*, int);
extern int rtt_exec2(char*);
extern void rtt_wait_pid(pid_t);
extern void* xalloc(size_t);

#endif
