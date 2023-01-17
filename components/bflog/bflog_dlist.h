/**
 * @file bflog_dlist.h
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

#ifndef _BFLOG_DLIST_H
#define _BFLOG_DLIST_H

#include <stdint.h>

/*!< 初始化链表 */
#define bflog_dlist_init(_plist) \
    (_plist)->next = (_plist);   \
    (_plist)->prev = (_plist)

/*!< 后插 */
#define bflog_dlist_insert_after(_plist, _pnode) \
    (_plist)->next->prev = (_pnode);             \
    (_pnode)->next = (_plist)->next;             \
    (_plist)->next = (_pnode);                   \
    (_pnode)->prev = (_plist)

/*!< 前插 */
#define bflog_dlist_insert_before(_plist, _pnode) \
    (_plist)->prev->next = (_pnode);              \
    (_pnode)->prev = (_plist)->prev;              \
    (_plist)->prev = (_pnode);                    \
    (_pnode)->next = (_plist)

/*!< 删除 */
#define bflog_dlist_remove(_pnode)         \
    (_pnode)->next->prev = (_pnode)->prev; \
    (_pnode)->prev->next = (_pnode)->next; \
    (_pnode)->next = (_pnode);             \
    (_pnode)->prev = (_pnode)

/*!< 移动到链表头 */
#define bflog_dlist_move_head(_plist, _pnode) \
    bflog_dlist_remove(_pnode);               \
    bflog_dlist_insert_after(_plist, _pnode)

/*!< 移动到链表尾 */
#define bflog_dlist_move_tail(_plist, _pnode) \
    bflog_dlist_remove(_pnode);               \
    bflog_dlist_insert_before(_plist, _pnode)

/*!< 链表是否为空 */
#define bflog_dlist_is_empty(_plist) \
    ((void *)((_plist)->next) == (void *)(_plist))

#define BFLOG_DLIST_CONTAINER_OF(_ptr, _type, _member) \
    ((_type *)((char *)(_ptr) - (unsigned int)(&((_type *)0)->_member)))

/*!< 定义新链表 */
#define BFLOG_DLIST_NEW(_list_name) \
    struct _bflog_list _list_name = { &(_list_name), &(_list_name) }

/*!< 获取链表容器 */
#define BFLOG_DLIST_ENTRY(_pnode, _type, _member) \
    BFLOG_DLIST_CONTAINER_OF(_pnode, _type, _member)

/*!< 获取链表下一个节点容器 */
#define BFLOG_DLIST_ENTRY_FIRST(_pnode, _type, _member) \
    BFLOG_DLIST_CONTAINER_OF((_pnode)-next, _type, _member)

// /*!< 获取链表容器 */
// #define BFLOG_DLIST_ENTRY(_pnode) (void *)(_pnode)

// /*!< 获取链表下一个节点容器 */
// #define BFLOG_DLIST_ENTRY_FIRST(_pnode, _type, _member) (void *)((_pnode)->next)

/*!< 获取链表下一个节点容器或NULL */
#define BFLOG_DLIST_ENTRY_FIRST_ORNULL(_pnode, _type, _member) \
    (bflog_dlist_is_empty(_pnode) ? NULL : BFLOG_DLIST_ENTRY_FIRST(_pnode, _type, _member))

/*!< 正向遍历链表 */
#define BFLOG_DLIST_FOREACH(_pos, _head)            for ((_pos) = (_head)->next; (_pos) != (_head); (_pos) = (_pos)->next)

/*!< 正向遍历链表 */
#define BFLOG_DLIST_FOREACH_NEXT                    BFLOG_DLIST_FOREACH

/*!< 逆向遍历链表 */
#define BFLOG_DLIST_FOREACH_PREV(_pos, _head)       for ((_pos) = (_head)->prev; (_pos) != (_head); (_pos) = (_pos)->prev)

/*!< 安全正向遍历链表 */
#define BFLOG_DLIST_FOREACH_S(_pos, _n, _head)      for ((_pos) = (_head)->next, (_n) = (_pos)->next; (_pos) != (_head); (_pos) = (_n), (_n) = (_pos)->next)

/*!< 安全正向遍历链表 */
#define BFLOG_DLIST_FOREACH_NEXT_S                  BFLOG_DLIST_FOREACH_S

/*!< 安全逆向遍历链表 */
#define BFLOG_DLIST_FOREACH_PREV_S(_pos, _n, _head) for ((_pos) = (_head)->prev, (_n) = (_pos)->prev; (_pos) != (_head); (_pos) = (_n), (_n) = (_pos)->prev)

#endif
