/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef MSP_LIST_H
#define MSP_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Get offset of a member variable.
 *
 * @param[in]   type     the type of the struct this is embedded in.
 * @param[in]   member   the name of the variable within the struct.
 */
#define msp_offsetof(type, member)   ((size_t)&(((type *)0)->member))

/*
 * Get the struct for this entry.
 *
 * @param[in]   ptr     the list head to take the element from.
 * @param[in]   type    the type of the struct this is embedded in.
 * @param[in]   member  the name of the variable within the struct.
 */
#define msp_container_of(ptr, type, member) \
    ((type *) ((char *) (ptr) - msp_offsetof(type, member)))

/* for double link list */
typedef struct msp_dlist_s {
    struct msp_dlist_s *prev;
    struct msp_dlist_s *next;
} msp_dlist_t;

static inline void __msp_dlist_add(msp_dlist_t *node, msp_dlist_t *prev, msp_dlist_t *next)
{
    node->next = next;
    node->prev = prev;

    prev->next = node;
    next->prev = node;
}

/*
 * Get the struct for this entry.
 *
 * @param[in]   addr    the list head to take the element from.
 * @param[in]   type    the type of the struct this is embedded in.
 * @param[in]   member  the name of the msp_dlist_t within the struct.
 */
#define msp_dlist_entry(addr, type, member) \
    ((type *)((long)addr - msp_offsetof(type, member)))


static inline void msp_dlist_add(msp_dlist_t *node, msp_dlist_t *queue)
{
    __msp_dlist_add(node, queue, queue->next);
}

static inline void msp_dlist_add_tail(msp_dlist_t *node, msp_dlist_t *queue)
{
    __msp_dlist_add(node, queue->prev, queue);
}

static inline void msp_dlist_del(msp_dlist_t *node)
{
    msp_dlist_t *prev = node->prev;
    msp_dlist_t *next = node->next;

    prev->next = next;
    next->prev = prev;
}

static inline void msp_dlist_init(msp_dlist_t *node)
{
    node->next = node->prev = node;
}

static inline void INIT_MSP_DLIST_HEAD(msp_dlist_t *list)
{
    list->next = list;
    list->prev = list;
}

static inline int msp_dlist_empty(const msp_dlist_t *head)
{
    return head->next == head;
}

/*
 * Initialise the list.
 *
 * @param[in]   list    the list to be inited.
 */
#define MSP_DLIST_INIT(list)  {&(list), &(list)}

/*
 * Get the first element from a list
 *
 * @param[in]   ptr     the list head to take the element from.
 * @param[in]   type    the type of the struct this is embedded in.
 * @param[in]   member  the name of the msp_dlist_t within the struct.
 */
#define msp_dlist_first_entry(ptr, type, member) \
    msp_dlist_entry((ptr)->next, type, member)

/*
 * Iterate over a list.
 *
 * @param[in]   pos     the &struct msp_dlist_t to use as a loop cursor.
 * @param[in]   head    he head for your list.
 */
#define msp_dlist_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/*
 * Iterate over a list safe against removal of list entry.
 *
 * @param[in]   pos     the &struct msp_dlist_t to use as a loop cursor.
 * @param[in]   n       another &struct msp_dlist_t to use as temporary storage.
 * @param[in]   head    he head for your list.
 */
#define msp_dlist_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
    pos = n, n = pos->next)

/*
 * Iterate over list of given type.
 *
 * @param[in]   queue   he head for your list.
 * @param[in]   node    the &struct msp_dlist_t to use as a loop cursor.
 * @param[in]   type    the type of the struct this is embedded in.
 * @param[in]   member  the name of the msp_dlist_t within the struct.
 */
#define msp_dlist_for_each_entry(queue, node, type, member) \
    for (node = msp_container_of((queue)->next, type, member); \
         &node->member != (queue); \
         node = msp_container_of(node->member.next, type, member))

/*
 * Iterate over list of given type safe against removal of list entry.
 *
 * @param[in]   queue   the head for your list.
 * @param[in]   n       the type * to use as a temp.
 * @param[in]   node    the type * to use as a loop cursor.
 * @param[in]   type    the type of the struct this is embedded in.
 * @param[in]   member  the name of the msp_dlist_t within the struct.
 */
#define msp_dlist_for_each_entry_safe(queue, n, node, type, member) \
    for (node = msp_container_of((queue)->next, type, member),  \
         n = (queue)->next ? (queue)->next->next : NULL;        \
         &node->member != (queue);                              \
         node = msp_container_of(n, type, member), n = n ? n->next : NULL)

/*
 * Get the struct for this entry.
 * @param[in]   ptr     the list head to take the element from.
 * @param[in]   type    the type of the struct this is embedded in.
 * @param[in]   member  the name of the variable within the struct.
 */
#define msp_list_entry(ptr, type, member) \
        msp_container_of(ptr, type, member)


/*
 * Iterate backwards over list of given type.
 *
 * @param[in]   pos     the type * to use as a loop cursor.
 * @param[in]   head    he head for your list.
 * @param[in]   member  the name of the msp_dlist_t within the struct.
 * @param[in]   type    the type of the struct this is embedded in.
 */
#define msp_dlist_for_each_entry_reverse(pos, head, member, type) \
    for (pos = msp_list_entry((head)->prev, type, member);        \
         &pos->member != (head);                              \
         pos = msp_list_entry(pos->member.prev, type, member))


/*
 * Get the list length.
 *
 * @param[in]  queue  the head for your list.
 */
int msp_dlist_entry_number(msp_dlist_t *queue);



/*
 * Initialise the list.
 *
 * @param[in]   name    the list to be initialized.
 */
#define MSP_DLIST_HEAD_INIT(name) { &(name), &(name) }

/*
 * Initialise the list.
 *
 * @param[in]   name    the list to be initialized.
 */
#define MSP_DLIST_HEAD(name) \
        msp_dlist_t name = MSP_DLIST_HEAD_INIT(name)

/* for single link list */
typedef struct msp_slist_s {
    struct msp_slist_s *next;
} msp_slist_t;

static inline void msp_slist_add(msp_slist_t *node, msp_slist_t *head)
{
    node->next = head->next;
    head->next = node;
}

void msp_slist_add_tail(msp_slist_t *node, msp_slist_t *head);

void msp_slist_del(msp_slist_t *node, msp_slist_t *head);

static inline int msp_slist_empty(const msp_slist_t *head)
{
    return !head->next;
}

static inline void msp_slist_init(msp_slist_t *head)
{
    head->next = 0;
}

static inline msp_slist_t *msp_slist_remove(msp_slist_t *l, msp_slist_t *n)
{
    /* remove slist head */
    struct msp_slist_s *node = l;
    while (node->next && node->next != n) node = node->next;

    /* remove node */
    if (node->next != (msp_slist_t *)0) node->next = node->next->next;

    return l;
}

static inline msp_slist_t *msp_slist_first(msp_slist_t *l)
{
    return l->next;
}

static inline msp_slist_t *msp_slist_tail(msp_slist_t *l)
{
    while (l->next) l = l->next;

    return l;
}

static inline msp_slist_t *msp_slist_next(msp_slist_t *n)
{
    return n->next;
}

/*
* Iterate over list of given type.
*
* @param[in]   queue   he head for your list.
* @param[in]   node    the type * to use as a loop cursor.
* @param[in]   type    the type of the struct this is embedded in.
* @param[in]   member  the name of the msp_slist_t within the struct.
*/
#define msp_slist_for_each_entry(queue, node, type, member)        \
    for (node = (queue)->next? msp_container_of((queue)->next, type, member) : NULL; \
         node;                                        \
         node = node->member.next ? msp_container_of(node->member.next, type, member) : NULL)

/*
 * Iterate over list of given type safe against removal of list entry.
 *
 * @param[in]   queue   the head for your list.
 * @param[in]   tmp     the type * to use as a temp.
 * @param[in]   node    the type * to use as a loop cursor.
 * @param[in]   type    the type of the struct this is embedded in.
 * @param[in]   member  the name of the msp_slist_t within the struct.
 */
#define msp_slist_for_each_entry_safe(queue, tmp, node, type, member) \
    for (node = (queue)->next? msp_container_of((queue)->next, type, member) : NULL, \
         tmp = (queue)->next ? (queue)->next->next : NULL;        \
         node;                                           \
         node = tmp ? msp_container_of(tmp, type, member) : NULL, tmp = tmp ? tmp->next : NULL)

/*
 * Initialise the list.
 *
 * @param[in]   name    the list to be initialized.
 */
#define MSP_SLIST_HEAD_INIT(name) {0}

/*
 * Initialise the list.
 *
 * @param[in]   name    the list to be initialized.
 */
#define MSP_SLIST_HEAD(name) \
        msp_slist_t name = MSP_SLIST_HEAD_INIT(name)

/*
 * Get the struct for this entry.
 * @param[in]   addr     the list head to take the element from.
 * @param[in]   type     the type of the struct this is embedded in.
 * @param[in]   member   the name of the msp_slist_t within the struct.
 */
#define msp_slist_entry(addr, type, member) (                                   \
    addr ? (type *)((long)addr - msp_offsetof(type, member)) : (type *)addr \
)

/*
* Get the first element from a list.
*
* @param[in]   ptr     the list head to take the element from.
* @param[in]   type    the type of the struct this is embedded in.
* @param[in]   member  the name of the msp_slist_t within the struct.
*/
#define msp_slist_first_entry(ptr, type, member) \
    msp_slist_entry((ptr)->next, type, member)

/**
 * msp_slist_tail_entry - get the tail element from a slist
 * @ptr:    the slist head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define msp_slist_tail_entry(ptr, type, member) \
    msp_slist_entry(msp_slist_tail(ptr), type, member)

/*
 * Get the list length.
 *
 * @param[in]   queue    the head for your list.
 */
int msp_slist_entry_number(msp_slist_t *queue);

#ifdef __cplusplus
}
#endif

#endif /* MSP_LIST_H */

