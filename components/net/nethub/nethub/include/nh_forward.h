/**
 * @file nh_forward.h
 * @brief nethub转发规则配置系统
 */

#ifndef _NH_FORWARD_H
#define _NH_FORWARD_H

#include "nethub_types.h"


/**
 * @brief 设置转发规则配置
 * @param config 转发规则配置
 * @return 0表示成功，负数表示错误码
 */
int nh_forward_set_config(const nh_forward_config_t *config);

/**
 * @brief 获取当前转发规则配置
 * @return 当前转发规则配置指针
 */
const nh_forward_config_t* nh_forward_get_config(void);

/**
 * @brief 执行数据包转发
 * @param skb 数据包缓冲区
 * @param src_type 源接口类型
 * @return 转发决策结果
 */
nh_forward_result_t nh_forward_execute(nh_skb_t *skb, nhif_type_t src_type);

/**
 * @brief 获取指定接口的转发规则表
 * @param src_type 源接口类型
 * @return 转发规则表指针，未找到返回NULL
 */
const nh_if_forward_table_t* nh_forward_get_if_table(nhif_type_t src_type);

/**
 * @brief 打印当前转发规则表
 */
void nh_forward_print_rules(void);

/**
 * @brief 验证转发规则配置的有效性
 * @param config 转发规则配置
 * @return 0表示有效，负数表示错误码
 */
int nh_forward_validate_config(const nh_forward_config_t *config);

#endif /* _NH_FORWARD_H */