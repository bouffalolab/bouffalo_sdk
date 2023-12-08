/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AVFILTER_H__
#define __AVFILTER_H__

#include "avfilter/avfilter_cls.h"

__BEGIN_DECLS__

/**
 * @brief  init inner resource of the avfilter
 * @param  [in] inst_name : the name of avfilter_t instance
 * @param  [in] avf
 * @return 0/-1
 */
int avf_init(const char *inst_name, avfilter_t *avf);

/**
 * @brief  control the filter
 * @param  [in] avf
 * @param  [in] cmd          : command id of the filter
 * @param  [in] arg
 * @param  [in/out] arg_size
 * @return 0/-1
 */
int avf_control(avfilter_t *avf, int cmd, void *arg, size_t *arg_size);

/**
 * @brief  bypass enable
 * @param  [in] avf
 * @param  [in] bypass : 0 or 1
 * @return 0/-1
 */
int avf_set_bypass(avfilter_t *avf, int bypass);

/**
 * @brief  link the two filters, to make a filter-chain
 * @param  [in] src : source filter
 * @param  [in] dst : the next/sink filter
 * @return 0/-1
 */
int avf_link(avfilter_t *src, avfilter_t *dst);

/**
 * @brief  link the tail to head filter
 * @param  [in] head : head of the filter chain
 * @param  [in] tail
 * @return 0/-1
 */
int avf_link_tail(avfilter_t *head, avfilter_t *tail);

/**
 * @brief  filter frame over the avfilter or filter-chain(if had linked)
 * @param  [in] avf : the avfilter
 * @param  [in] in
 * @param  [in] out
 * @return number of samples output per channel, -1 on error
 */
int avf_filter_frame(avfilter_t *avf, const avframe_t *in, avframe_t *out);

/**
* @brief  free inner resource of the avfilter
* @param  [in] avf
* @return 0/-1
*/
int avf_uninit(avfilter_t *avf);

/**
* @brief  close & free the avfilter
* @param  [in] avf
* @return 0/-1
*/
int avf_close(avfilter_t *avf);

/**
* @brief  close the filter-chain
* @param  [in] avf : header avfilter of the filter-chain
* @return 0/-1
*/
int avf_chain_close(avfilter_t *avf);

__END_DECLS__

#endif /* __AVFILTER_H__ */

