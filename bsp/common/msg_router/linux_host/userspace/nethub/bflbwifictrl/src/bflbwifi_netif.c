/**
 * @file bflbwifi_netif.c
 * @brief Linux 网卡配置模块实现
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>

#include "../include/bflbwifi_log.h"
#include "../include/bflbwifi_netif.h"

#define BFLBWIFI_NETIF_DEFAULT_ROUTE_METRIC 700

static int parse_ipv4_addr(const char *addr, uint32_t *value)
{
    unsigned int parts[4];
    int count;

    if (!addr || !value) {
        return -1;
    }

    count = sscanf(addr, "%u.%u.%u.%u", &parts[0], &parts[1], &parts[2], &parts[3]);
    if (count != 4) {
        return -1;
    }

    for (int i = 0; i < 4; i++) {
        if (parts[i] > 255) {
            return -1;
        }
    }

    *value = ((uint32_t)parts[0] << 24) |
             ((uint32_t)parts[1] << 16) |
             ((uint32_t)parts[2] << 8) |
             (uint32_t)parts[3];
    return 0;
}

/**
 * @brief 验证子网掩码是否有效
 * @param mask 子网掩码（如 "255.255.255.0"）
 * @return 1=有效, 0=无效
 */
static int is_valid_mask(const char *mask)
{
    uint32_t m;
    uint32_t inverted;

    if (!mask) {
        BFLB_LOGE("子网掩码验证失败：mask 为 NULL");
        return 0;
    }

    if (parse_ipv4_addr(mask, &m) != 0) {
        BFLB_LOGE("子网掩码解析失败：%s", mask);
        return 0;
    }

    BFLB_LOGD("子网掩码 32 位整数值：0x%08x", m);

    /* 有效掩码应满足 ~mask 为连续的低位 1。 */
    inverted = ~m;
    if ((inverted & (inverted + 1U)) != 0) {
        BFLB_LOGE("子网掩码格式无效：0x%08x", m);
        return 0;
    }

    BFLB_LOGD("子网掩码验证通过：%s", mask);
    return 1;
}

/**
 * @brief 将子网掩码转换为 CIDR 格式
 * @param mask 子网掩码（如 "255.255.255.0"）
 * @return CIDR 前缀长度（如 24），错误返回 -1
 */
static int mask_to_cidr(const char *mask)
{
    uint32_t m;
    int cidr = 0;

    if (!mask) {
        return -1;
    }

    if (parse_ipv4_addr(mask, &m) != 0) {
        return -1;
    }

    /* 计算连续的 1 的个数 */
    while (m) {
        if (m & 0x80000000) {
            cidr++;
        } else if ((m & 0x7FFFFFFF) != 0) {
            /* 遇到 0 后还有 1，说明掩码格式错误 */
            return -1;
        }
        m <<= 1;
    }

    return cidr;
}

/**
 * @brief 计算广播地址
 * @param ip IP 地址（如 "192.168.50.97"）
 * @param mask 子网掩码（如 "255.255.255.0"）
 * @param broadcast 输出广播地址缓冲区（至少 16 字节）
 * @return 0=成功, -1=失败
 */
static int calculate_broadcast(const char *ip, const char *mask, char *broadcast)
{
    uint32_t ip_addr;
    uint32_t mask_addr;
    uint32_t broadcast_addr;

    if (!ip || !mask || !broadcast) {
        return -1;
    }

    if (parse_ipv4_addr(ip, &ip_addr) != 0 || parse_ipv4_addr(mask, &mask_addr) != 0) {
        return -1;
    }

    /* 计算广播地址：IP | ~mask */
    broadcast_addr = ip_addr | (~mask_addr);

    /* 转换回字符串格式 */
    snprintf(broadcast, 16, "%u.%u.%u.%u",
             (broadcast_addr >> 24) & 0xFF,
             (broadcast_addr >> 16) & 0xFF,
             (broadcast_addr >> 8) & 0xFF,
             broadcast_addr & 0xFF);

    return 0;
}

/**
 * @brief 执行系统命令并获取输出
 * @param command 命令字符串
 * @return 0=成功, -1=失败
 */
static int execute_command(const char *command)
{
    int ret;

    if (!command) {
        return -1;
    }

    BFLB_LOGD("执行命令: %s", command);

    ret = system(command);
    if (ret == -1) {
        BFLB_LOGE("命令执行失败: %s", command);
        return -1;
    }

    /* 检查命令退出状态 */
    if (WIFEXITED(ret) && WEXITSTATUS(ret) != 0) {
        BFLB_LOGE("命令返回错误: %s (exit=%d)", command, WEXITSTATUS(ret));
        return -1;
    }

    return 0;
}

static int execute_command_ignore_exit(const char *command, int ignored_exit)
{
    int ret;

    if (!command) {
        return -1;
    }

    BFLB_LOGD("执行命令: %s", command);

    ret = system(command);
    if (ret == -1) {
        BFLB_LOGE("命令执行失败: %s", command);
        return -1;
    }

    if (!WIFEXITED(ret)) {
        BFLB_LOGE("命令异常退出: %s", command);
        return -1;
    }

    if (WEXITSTATUS(ret) == ignored_exit) {
        BFLB_LOGD("命令返回已忽略的退出码: %s (exit=%d)", command, ignored_exit);
        return 0;
    }

    if (WEXITSTATUS(ret) != 0) {
        BFLB_LOGE("命令返回错误: %s (exit=%d)", command, WEXITSTATUS(ret));
        return -1;
    }

    return 0;
}

static int resolv_conf_has_nameserver(void)
{
    FILE *fp;
    char line[256];

    fp = fopen("/etc/resolv.conf", "r");
    if (!fp) {
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        char *cursor = line;

        while (*cursor == ' ' || *cursor == '\t') {
            cursor++;
        }

        if (*cursor == '#' || *cursor == '\n' || *cursor == '\0') {
            continue;
        }

        if (strncmp(cursor, "nameserver", strlen("nameserver")) == 0) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

int bflbwifi_netif_set_addr(const char *ifname, const char *ip, const char *mask)
{
    char cmd[256];
    char broadcast[16];
    int cidr;
    const char *actual_mask = mask;

    if (!ifname || !ip || !mask) {
        BFLB_LOGE("参数错误");
        return -1;
    }

    /* 验证子网掩码是否有效 */
    if (!is_valid_mask(mask)) {
        BFLB_LOGW("无效的子网掩码: %s，使用默认值 255.255.255.0", mask);
        actual_mask = "255.255.255.0";
    }

    /* 将子网掩码转换为 CIDR 格式 */
    cidr = mask_to_cidr(actual_mask);
    if (cidr < 0) {
        BFLB_LOGE("子网掩码转换失败: %s", actual_mask);
        return -1;
    }

    /* 计算广播地址 */
    if (calculate_broadcast(ip, actual_mask, broadcast) != 0) {
        BFLB_LOGW("广播地址计算失败，使用默认值");
        snprintf(broadcast, sizeof(broadcast), "%s", "0.0.0.0");
    } else {
        BFLB_LOGD("计算得到的广播地址: %s", broadcast);
    }

    /* 先清理旧的 IP 地址配置 */
    snprintf(cmd, sizeof(cmd), "ip addr flush dev %s 2>/dev/null", ifname);
    execute_command(cmd);

    /* 配置新的 IP 地址（显式指定广播地址） */
    if (strcmp(broadcast, "0.0.0.0") != 0) {
        snprintf(cmd, sizeof(cmd), "ip addr add %s/%d brd %s dev %s", ip, cidr, broadcast, ifname);
    } else {
        snprintf(cmd, sizeof(cmd), "ip addr add %s/%d dev %s", ip, cidr, ifname);
    }

    if (execute_command(cmd) != 0) {
        BFLB_LOGE("配置 IP 地址失败");
        return -1;
    }

    /* 启用网卡 */
    snprintf(cmd, sizeof(cmd), "ip link set %s up", ifname);
    if (execute_command(cmd) != 0) {
        BFLB_LOGE("启用网卡失败");
        return -1;
    }

    BFLB_LOGI("网卡 IP 配置成功: %s -> %s/%d (broadcast: %s)", ifname, ip, cidr, broadcast);
    return 0;
}

int bflbwifi_netif_set_gateway(const char *ifname, const char *gw)
{
    char cmd[256];

    if (!ifname || !gw) {
        BFLB_LOGE("参数错误");
        return -1;
    }

    /*
     * 仅维护目标接口上的默认路由，避免破坏宿主机现有管理链路
     * （例如 eth0 的默认路由和 SSH 出口）。
     */
    snprintf(cmd, sizeof(cmd), "ip route del default dev %s 2>/dev/null", ifname);
    execute_command_ignore_exit(cmd, 2);

    /*
     * 使用较高 metric 挂载 nethub 路由：
     * 1. 没有其他默认路由时，仍可通过 device Wi-Fi 出网；
     * 2. 已有 eth0 等管理链路时，不会抢占系统默认出口。
     */
    snprintf(cmd, sizeof(cmd),
             "ip route add default via %s dev %s metric %d",
             gw,
             ifname,
             BFLBWIFI_NETIF_DEFAULT_ROUTE_METRIC);
    if (execute_command(cmd) != 0) {
        BFLB_LOGE("配置网关失败");
        return -1;
    }

    BFLB_LOGI("默认网关配置成功: via %s dev %s metric %d",
              gw,
              ifname,
              BFLBWIFI_NETIF_DEFAULT_ROUTE_METRIC);
    return 0;
}

int bflbwifi_netif_set_dns(const char *dns)
{
    FILE *fp;

    if (!dns) {
        BFLB_LOGE("参数错误");
        return -1;
    }

    /*
     * 不覆盖系统已经存在的 DNS 配置，避免影响宿主机原有管理网络。
     * 仅在系统尚无 nameserver 时，使用 device 提供的 DNS 作为兜底。
     */
    if (resolv_conf_has_nameserver()) {
        BFLB_LOGI("保留现有 DNS 配置，跳过覆盖 device DNS: %s", dns);
        return 0;
    }

    fp = fopen("/etc/resolv.conf", "w");
    if (!fp) {
        BFLB_LOGE("无法打开 /etc/resolv.conf");
        return -1;
    }

    /* 写入 DNS 服务器 */
    fprintf(fp, "nameserver %s\n", dns);
    fclose(fp);

    BFLB_LOGI("DNS 配置成功: %s", dns);
    return 0;
}

int bflbwifi_netif_config(const char *ifname,
                          const char *ip,
                          const char *mask,
                          const char *gw,
                          const char *dns)
{
    int ret;

    if (!ifname || !ip || !mask || !gw || !dns) {
        BFLB_LOGE("参数错误");
        return -1;
    }

    BFLB_LOGI("开始配置网卡: %s (IP:%s, MASK:%s, GW:%s, DNS:%s)",
              ifname, ip, mask, gw, dns);

    /* 配置 IP 地址和掩码 */
    ret = bflbwifi_netif_set_addr(ifname, ip, mask);
    if (ret != 0) {
        BFLB_LOGE("配置 IP 地址失败");
        return -1;
    }

    /* 配置网关 */
    ret = bflbwifi_netif_set_gateway(ifname, gw);
    if (ret != 0) {
        BFLB_LOGE("配置网关失败");
        return -1;
    }

    /* 配置 DNS */
    ret = bflbwifi_netif_set_dns(dns);
    if (ret != 0) {
        BFLB_LOGE("配置 DNS 失败");
        return -1;
    }

    BFLB_LOGI("网卡配置完成: %s", ifname);
    return 0;
}
