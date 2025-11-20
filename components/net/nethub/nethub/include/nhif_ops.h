/**
 * @file nhif_ops.h
 * @brief nethub接口操作抽象层定义
 */

#ifndef _NHIF_OPS_H
#define _NHIF_OPS_H

#include "nethub_types.h"

/* Forward declaration */
struct nhif_ops;

/**
 * @brief 网络接口实例结构体
 */
typedef struct nhif_instance {
    nhif_type_t type;                          /**< 接口类型 */
    nhif_state_t state;                        /**< 接口状态 */
    const struct nhif_ops *ops;                /**< 接口操作函数指针 */
    void *private_data;                        /**< 接口私有数据 */
} nhif_instance_t;

/**
 * @brief 网络接口操作函数结构体
 * @note 每个接口类型都需要实现这个结构体
 */
struct nhif_ops {
    /**
     * @brief 接口类型
     */
    nhif_type_t type;

    /**
     * @brief 初始化接口
     * @return 0表示成功，负数表示错误码
     */
    int (*init)(void);

    /**
     * @brief 反初始化接口
     * @return 0表示成功，负数表示错误码
     */
    int (*deinit)(void);

    /**
     * @brief 数据输入回调函数
     * @details 当接口接收到数据时，调用此函数将数据传递给nethub
     * @param skb 数据包缓冲区
     * @param arg 用户自定义参数
     * @return 转发决策结果
     */
    nhforward_callback_t input;

    /**
     * @brief 数据输出回调函数
     * @details 当nethub需要向接口发送数据时，调用此函数
     * @param skb 数据包缓冲区
     * @param arg 用户自定义参数
     * @return 转发决策结果
     */
    nhforward_callback_t output;
};

/**
 * @brief 注册网络接口
 * @param ops 接口操作函数指针
 * @param private_data 私有数据
 * @return 0表示成功，负数表示错误码
 */
int nhif_register(const struct nhif_ops *ops, void *private_data);

/**
 * @brief 注销网络接口
 * @param type 接口类型
 * @return 0表示成功，负数表示错误码
 */
int nhif_unregister(nhif_type_t type);

/**
 * @brief 查找指定类型的接口实例
 * @param type 接口类型
 * @return 接口实例指针，未找到返回NULL
 */
nhif_instance_t* nhif_find_by_type(nhif_type_t type);

/**
 * @brief 初始化所有已注册的接口
 * @return 0表示成功，负数表示错误码
 */
int nhif_init_all(void);

#endif /* _NHIF_OPS_H */