#ifndef LIST_H
#define LIST_H

#include <stddef.h>

struct list_node;
typedef struct list_node list_node_t;
typedef struct list_node list_t;

struct list_node {
    list_node_t*     next;
    list_node_t*     prev;
};

/*
 * For macros.
 */
#define list_for(l, nd_var) for (nd_var = (void*)((l)->next); \
                                    (nd_var != (void*)(l)) && (nd_var != NULL); \
                                    nd_var = (void*)((list_node_t*)nd_var)->next)

#define list_for_del(l, nd_var, tmp_var) for (nd_var = (void*)((l)->next); \
                                                 tmp_var = (void*)((list_node_t*)nd_var)->next, \
                                                     nd_var != (void*)(l) || (nd_var = NULL); \
                                                 nd_var = tmp_var)


/*
 * Basic node level operations.
 */
static inline void
list_ins_after(list_node_t* nd, list_node_t* new_nd)
{
    // printf("list_ins_after %p %p\n", nd, new_nd);
    // list_print(nd, "ins_after in");
    new_nd->next = nd->next;
    new_nd->prev = nd;
    nd->next->prev = new_nd;
    nd->next = new_nd;
    // list_print(nd, "ins_after out");
}

static inline void
list_ins_before(list_node_t* nd, list_node_t* new_nd)
{
    // list_print(nd, "ins_before in");
    new_nd->prev = nd->prev;
    new_nd->next = nd;
    nd->prev->next = new_nd;
    nd->prev = new_nd;
    // list_print(nd, "ins_before out");
}

static inline void*
list_del(list_node_t* nd)
{
    // list_print(nd, "del in");
    nd->prev->next = nd->next;
    nd->next->prev = nd->prev;
    // list_print(nd->next, "del out");
    nd->next = NULL;    // just to be safe
    nd->prev = NULL;    // just to be safe

    return nd;
}

/*
 * High-level operations.
 */
static inline void
list_init(list_t* l)
{
    l->next = l;
    l->prev = l;
}

static inline void
list_node_init(list_node_t* nd)
{
    nd->next = nd;
    nd->prev = nd;
}

static inline int
list_is_empty(list_t* l)
{
    return (l == l->next);
}

static inline void*
list_next(list_t* l)
{
    return (list_is_empty(l) ? NULL : l->next);
}

static inline void*
list_prev(list_t* l)
{
    return (list_is_empty(l) ? NULL : l->prev);
}

static inline void*
list_first(list_t* l)
{
    return list_next(l);
}

static inline void*
list_last(list_t* l)
{
    return list_prev(l);
}

static inline void
list_ins_front(list_t* l, list_node_t* nd)
{
    list_ins_after(l, nd);
}

static inline void
list_ins_back(list_t* l, list_node_t* nd)
{
    list_ins_before(l, nd);
}

static inline void*
list_del_front(list_t* l)
{
    return (list_is_empty(l) ? NULL : list_del(l->next));
}

static inline void*
list_del_back(list_t* l)
{
    return (list_is_empty(l) ? NULL : list_del(l->prev));
}

/*
 *
 */
static inline void
list_enqueue(list_t* l, list_node_t* nd)
{
    list_ins_back(l, nd);
}

static inline void*
list_dequeue(list_t* l)
{
    return list_del_front(l);
}

/*
 *
 */
static inline void
list_push(list_t* l, list_node_t* nd)
{
    list_ins_front(l, nd);
}

static inline void*
list_pop(list_t* l)
{
    return list_del_front(l);
}


/*
 * 
 */
static inline void
list_transfer(list_t *src, list_t *dst)
{
    list_node_t *nd;

    while ((nd = list_dequeue(src)) != NULL) {
        list_enqueue(dst, nd);
    }
}

/*
 *
 */
static inline unsigned int
list_count_slowly(list_t *l)
{
    list_node_t  *nd;
    unsigned int    cnt = 0;

    list_for(l, nd) {
        cnt++;
    }

    return cnt;
}


/**
 * list_splice - join two lists. result list would be (src + dst) and
 * src list head will be re-initialized.
 * @src:  the new list to add.
 * @dest: the place to add it in the first list.
 */
static inline void
list_splice(list_t *src, list_t *dst)
{
    if (!list_is_empty(src)) {
        list_t *first = src->next;
        list_t *last = src->prev;
        list_t *at = dst->next;

        first->prev = dst;
        dst->next = first;

        last->next = at;
        at->prev = last;

        list_init(src);
    }
}


#define object_of(type, member, ptr) ({                                 \
            const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
            (type *)( (char *)__mptr - offsetof(type,member) );})

#endif
