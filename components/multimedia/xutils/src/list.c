/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <xutils/list.h>

void msp_slist_add_tail(msp_slist_t *node, msp_slist_t *head)
{
    while (head->next) {
        head = head->next;
    }

    msp_slist_add(node, head);
}

void msp_slist_del(msp_slist_t *node, msp_slist_t *head)
{
    while (head->next) {
        if (head->next == node) {
            head->next = node->next;
            break;
        }

        head = head->next;
    }
}

int msp_slist_entry_number(msp_slist_t *queue)
{
    int num;
    msp_slist_t *cur = queue;
    for (num=0; cur->next; cur=cur->next, num++)
        ;

    return num;
}

int msp_dlist_entry_number(msp_dlist_t *queue)
{
    int num;
    msp_dlist_t *cur = queue;
    for (num=0; cur->next != queue; cur=cur->next, num++)
        ;

    return num;
}

