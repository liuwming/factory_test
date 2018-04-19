#ifndef AIR_H
#define AIR_H

#include "util.h"
#include "osdep.h"

#define default_rt_iface   "mon0"

extern struct wif *wi_alloc(int);
extern int init_cards(char*);
extern int air_init(int, char**);
extern int air_start(void);
extern int air_stop(void);

#endif
