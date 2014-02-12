
/* 
 * Ridiculously good and elegant list implementation shamelessly yanked out 
 * of the Linux kernel source tree, I believe it was also part of the BSD
 */

#ifndef _LIST_H_
#define _LIST_H_

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifdef WIN32
#define INLINE __inline
#else
#define INLINE inline
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((unsigned long) &((TYPE *)0)->MEMBER)
#endif

static INLINE void prefetch(const void *x)
{
    /* NOOP, instead of using arch dependent assembly */
}

#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

struct list_head
{
    struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define DEFINE_LIST_HEAD(name) \
        struct list_head name = LIST_HEAD_INIT(name)

static INLINE void INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list;
    list->prev = list;
}

static INLINE void __list_del(struct list_head * prev, 
        struct list_head * next)
{
    next->prev = prev;
    prev->next = next;
}

static INLINE void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = NULL;
    entry->prev = NULL;
}

static INLINE void __list_add(struct list_head *new_,
        struct list_head *prev,
        struct list_head *next)
 {
    next->prev = new_;
    new_->next = next;
    new_->prev = prev;
    prev->next = new_;
 }

static INLINE void list_add(struct list_head *new_, 
        struct list_head *head)
{
    __list_add(new_, head, head->next);
}

static INLINE void list_add_tail(struct list_head *new_, 
        struct list_head *head)
{
    __list_add(new_, head->prev, head);
}

static INLINE int list_empty(const struct list_head *head)
{
    return head->next == head;
}

static INLINE int list_is_last(const struct list_head *list, 
        const struct list_head *head)
{
    return list->next == head;
}

static INLINE void list_replace(struct list_head *old, 
        struct list_head *new_)
{
    new_->next = old->next;
    new_->next->prev = new_;
    new_->prev = old->prev;
    new_->prev->next = new_;
}

static INLINE void __list_splice(struct list_head *list, 
        struct list_head *head)
{
    struct list_head *first = list->next;
    struct list_head *last = list->prev;
    struct list_head *at = head->next;

    first->prev = head;
    head->next = first;

    last->next = at;
    at->prev = last;
}

/**
 * join two lists
 * @list: the new list being added. The @list element is unlinked and will
 *          not be part of the result list.
 * @head: the place to add it in the first list (after this element).
 */
static INLINE void list_splice(struct list_head *list, 
        struct list_head *head)
{
    if (!list_empty(list))
        __list_splice(list, head);
}

#define list_for_each(pos, head) \
        for (pos = (head)->next; prefetch(pos->next), pos != (head); \
            pos = pos->next)

#define list_for_each_prev(pos, head) \
        for (pos = (head)->prev; prefetch(pos->prev), pos != (head); \
            pos = pos->prev)

#define list_for_each_entry(pos, head, member)                          \
        for (pos = list_entry((head)->next, typeof(*pos), member);      \
            prefetch(pos->member.next), &pos->member != (head);         \
            pos = list_entry(pos->member.next, typeof(*pos), member))

#define list_for_each_entry_reverse(pos, head, member)                  \
        for (pos = list_entry((head)->prev, typeof(*pos), member);      \
            prefetch(pos->member.prev), &pos->member != (head);        \
            pos = list_entry(pos->member.prev, typeof(*pos), member))

#define list_for_each_entry_from(pos, head, member)                     \
        for (; prefetch(pos->member.next), &pos->member != (head);      \
            pos = list_entry(pos->member.next, typeof(*pos), member))

#define list_for_each_entry_safe(pos, n, head, member)                  \
    for (pos = list_entry((head)->next, typeof(*pos), member),      \
            n = list_entry(pos->member.next, typeof(*pos), member); \
            &pos->member != (head);                                    \
            pos = n, n = list_entry(n->member.next, typeof(*n), member))

#define list_for_each_entry_safe_from(pos, n, head, member)                 \
        for (n = list_entry(pos->member.next, typeof(*pos), member);        \
            &pos->member != (head);                                         \
            pos = n, n = list_entry(n->member.next, typeof(*n), member))

#define list_for_each_entry_safe_reverse(pos, n, head, member)          \
        for (pos = list_entry((head)->prev, typeof(*pos), member),      \
            n = list_entry(pos->member.prev, typeof(*pos), member); \
            &pos->member != (head);                                    \
            pos = n, n = list_entry(n->member.prev, typeof(*n), member))

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif //_LIST_H_

