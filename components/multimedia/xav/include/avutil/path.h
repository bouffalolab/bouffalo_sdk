/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __PATH_H__
#define __PATH_H__

#include "avutil/common.h"

__BEGIN_DECLS__

/**
 * @brief  get the file extension
 * @param  [in] filename
 * @return NULL when err
 */
const char *get_extension(const char *filename);

/**
 * @brief  get basename from the path
 * @param  [in] path
 * @return NULL when err
 */
const char *path_basename(const char *path);

__END_DECLS__

#endif
