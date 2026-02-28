/**
 * @file bflbwifi_netif.h
 * @brief Linux 网卡配置模块
 */

#ifndef BFLBWIFI_NETIF_H
#define BFLBWIFI_NETIF_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 配置网卡 IP 地址和子网掩码
 * @param ifname 网卡名称（如 "mr_eth0"）
 * @param ip IP 地址（如 "192.168.50.1"）
 * @param mask 子网掩码（如 "255.255.255.0"）
 * @return 0=成功, -1=失败
 */
int bflbwifi_netif_set_addr(const char *ifname, const char *ip, const char *mask);

/**
 * @brief 配置默认网关
 * @param ifname 网卡名称（如 "mr_eth0"）
 * @param gw 网关地址（如 "192.168.50.1"）
 * @return 0=成功, -1=失败
 */
int bflbwifi_netif_set_gateway(const char *ifname, const char *gw);

/**
 * @brief 配置 DNS 服务器
 * @param dns DNS 服务器地址（如 "192.168.50.1"）
 * @return 0=成功, -1=失败
 */
int bflbwifi_netif_set_dns(const char *dns);

/**
 * @brief 一键配置网卡（IP、掩码、网关、DNS）
 * @param ifname 网卡名称（如 "mr_eth0"）
 * @param ip IP 地址
 * @param mask 子网掩码
 * @param gw 网关地址
 * @param dns DNS 服务器地址
 * @return 0=成功, -1=失败
 */
int bflbwifi_netif_config(const char *ifname,
                          const char *ip,
                          const char *mask,
                          const char *gw,
                          const char *dns);

#ifdef __cplusplus
}
#endif

#endif /* BFLBWIFI_NETIF_H */
