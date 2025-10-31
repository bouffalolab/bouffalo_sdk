/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __RECORD_H__
#define __RECORD_H__

#include <stdio.h>
#include <stdint.h>
#include <xutils/debug.h>

typedef int * rec_hdl_t;
typedef int (*data_ready_func)(void *arg, void *data, size_t len);
typedef void (*data_release_func)(void *arg);

/**
 * @brief      register a record session
 *
 * @param[in]  from   the source data from, e.g. 
 *                                          ringbuffer://handle=403913384
 *                                          mem://address=403913384&size=1024
 * @param[in]  to     the dst data to, e.g. 
 *                                          ringbuffer://handle=403913384
 *                                          ws://192.168.1.102:9090/mic.pcm
 * @return
 *     - `rec_hdl_t`
 *     - NULL if any errors
 */
rec_hdl_t record_register(const char *from, const char *to);

/**
 * @brief      set data ready event callback, to notify read data in record func.
 * 
 * @note       must call before `record_start`
 *
 * @param[in]  hdl  see `rec_hdl_t` 
 * 
 * @param[in]  cb   data_ready_func callback 
 * 
 * @param[in]  arg  the argument for callback
 *  
 * @return None
 */
void record_set_data_ready_cb(rec_hdl_t hdl, data_ready_func cb, void *arg);

/**
 * @brief      set data release event callback, the callback whill be called in `record_stop`
 *
 * @note       must call before `record_start`
 *
 * @param[in]  hdl  see `rec_hdl_t`
 *
 * @param[in]  cb   data_release_func callback
 *
 * @param[in]  arg  the argument for callback
 *
 * @return None
 */
void record_set_data_release_cb(rec_hdl_t hdl, data_release_func cb, void *arg);

/**
 * @brief      set data chunk size to read
 * 
 * @note       must call before `record_start`
 *
 * @param[in]  hdl  see `rec_hdl_t` 
 * 
 * @param[in]  chunk_size   chunk size
 *  
 * @return None
 */
void record_set_chunk_size(rec_hdl_t hdl, int chunk_size);

/**
 * @brief      get current read and send bytes count
 * 
 * @param[in]  hdl  see `rec_hdl_t` 
 * 
 * @param[out]  read_bytes  bytes read
 *  
 * @param[out]  sent_bytes  bytes sent
 *  
 * @return None
 */
int record_get_count(rec_hdl_t hdl, int *read_bytes, int *sent_bytes);

/**
 * @brief      start a record session
 *
 * @param[in]  hdl  see `rec_hdl_t` 
 * 
 * @return
 *     - 0 OK
 *     - -1 if any errors
 */
int record_start(rec_hdl_t hdl);

/**
 * @brief      stop a record session
 *
 * @param[in]  hdl  see `rec_hdl_t` 
 * 
 * @return
 *     - 0 OK
 *     - -1 if any errors
 */
int record_stop(rec_hdl_t hdl);
int record_pre_stop(rec_hdl_t hdl);

/**
 * @brief      remove a record session
 *
 * @param[in]  hdl  see `rec_hdl_t` 
 * 
 * @return
 *     - 0 OK
 *     - -1 if any errors
 */
int record_unregister(rec_hdl_t hdl);


#endif
