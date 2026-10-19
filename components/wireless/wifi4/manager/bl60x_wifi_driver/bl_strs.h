
/**
 ****************************************************************************************
 *
 * @file bl_strs.h
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */


#ifndef _RWNX_STRS_H_
#define _RWNX_STRS_H_

#include "lmac_msg.h"
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define RWNX_ID2STR(tag) (((MSG_T(tag) < ARRAY_SIZE(bl_id2str)) &&        \
                           (bl_id2str[MSG_T(tag)]) &&          \
                           ((bl_id2str[MSG_T(tag)])[MSG_I(tag)])) ?   \
                          (bl_id2str[MSG_T(tag)])[MSG_I(tag)] : "unknown")

extern const char *const *bl_id2str[TASK_LAST_EMB + 1];

#endif /* _RWNX_STRS_H_ */
