/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __SOCKET_RW_H__
#define __SOCKET_RW_H__

#include "avutil/common.h"

__BEGIN_DECLS__

/**
 * @brief  read n bytes from a sockfd with timeout
 * @param  [in] fd
 * @param  [in] buf
 * @param  [in] count
 * @param  [in] timeout_ms
 * @return -1 on err
 */
int sock_readn(int fd, char *buf, size_t count, int timeout_ms);

/**
 * @brief  write n bytes from a sockfd with timeout
 * @param  [in] fd
 * @param  [in] buf
 * @param  [in] count
 * @param  [in] timeout_ms
 * @return -1 on err
 */
int sock_writen(int fd, const char *buf, size_t count, int timeout_ms);

__END_DECLS__

#endif /* __SOCKET_RW_H__ */

