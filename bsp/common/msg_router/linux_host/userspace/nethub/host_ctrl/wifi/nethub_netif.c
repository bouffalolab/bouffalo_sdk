#include "nethub_netif.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "nethub_log.h"

#define NETHUB_NETIF_DEFAULT_NAME "mr_eth0"
#define NETHUB_NETIF_ENV_NAME "NETHUB_NETIF_NAME"
#define NETHUB_NETIF_NAME_MAX 15
#define NETHUB_NETIF_ROUTE_METRIC 700
#define NETHUB_NETIF_ROUTE_TABLE 100
#define NETHUB_NETIF_RULE_PRIORITY 10000

static int netif_name_char_is_valid(char ch)
{
    return (ch >= 'A' && ch <= 'Z') ||
           (ch >= 'a' && ch <= 'z') ||
           (ch >= '0' && ch <= '9') ||
           ch == '_' || ch == '-' || ch == '.' || ch == ':';
}

static int netif_name_is_valid(const char *name)
{
    size_t len;

    if (!name || name[0] == '\0') {
        return 0;
    }

    len = strlen(name);
    if (len > NETHUB_NETIF_NAME_MAX) {
        return 0;
    }

    for (size_t i = 0; i < len; i++) {
        if (!netif_name_char_is_valid(name[i])) {
            return 0;
        }
    }

    return 1;
}

static const char *netif_name_from_env(void)
{
    const char *name = getenv(NETHUB_NETIF_ENV_NAME);

    if (!name || name[0] == '\0') {
        return NETHUB_NETIF_DEFAULT_NAME;
    }

    if (!netif_name_is_valid(name)) {
        NETHUB_LOGE("invalid %s: %s", NETHUB_NETIF_ENV_NAME, name);
        return NULL;
    }

    return name;
}

static int parse_ipv4_addr(const char *addr, uint32_t *value)
{
    unsigned int parts[4];
    char tail[2];

    if (!addr || !value) {
        return -1;
    }

    if (sscanf(addr, "%u.%u.%u.%u%1s", &parts[0], &parts[1], &parts[2], &parts[3], tail) != 4) {
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

static int mask_to_cidr(const char *mask)
{
    uint32_t value;
    int cidr = 0;

    if (parse_ipv4_addr(mask, &value) != 0) {
        return -1;
    }

    while (value & 0x80000000U) {
        cidr++;
        value <<= 1;
    }

    if (value != 0) {
        return -1;
    }

    return cidr;
}

static int calculate_broadcast(const char *ip, const char *mask, char *broadcast, size_t broadcast_size)
{
    uint32_t ip_addr;
    uint32_t mask_addr;
    uint32_t broadcast_addr;

    if (!broadcast || broadcast_size < 16 ||
        parse_ipv4_addr(ip, &ip_addr) != 0 ||
        parse_ipv4_addr(mask, &mask_addr) != 0) {
        return -1;
    }

    broadcast_addr = ip_addr | ~mask_addr;
    snprintf(broadcast, broadcast_size, "%u.%u.%u.%u",
             (broadcast_addr >> 24) & 0xFF,
             (broadcast_addr >> 16) & 0xFF,
             (broadcast_addr >> 8) & 0xFF,
             broadcast_addr & 0xFF);
    return 0;
}

static int calculate_network(const char *ip, const char *mask, char *network, size_t network_size)
{
    uint32_t ip_addr;
    uint32_t mask_addr;
    uint32_t network_addr;

    if (!network || network_size < 16 ||
        parse_ipv4_addr(ip, &ip_addr) != 0 ||
        parse_ipv4_addr(mask, &mask_addr) != 0) {
        return -1;
    }

    network_addr = ip_addr & mask_addr;
    snprintf(network, network_size, "%u.%u.%u.%u",
             (network_addr >> 24) & 0xFF,
             (network_addr >> 16) & 0xFF,
             (network_addr >> 8) & 0xFF,
             network_addr & 0xFF);
    return 0;
}

static int run_command(const char *cmd, int ignored_exit)
{
    int ret;

    NETHUB_LOGD("netif command: %s", cmd);
    ret = system(cmd);
    if (ret == -1) {
        NETHUB_LOGE("failed to run command: %s", cmd);
        return -1;
    }

    if (!WIFEXITED(ret)) {
        NETHUB_LOGE("command exited abnormally: %s", cmd);
        return -1;
    }

    if (WEXITSTATUS(ret) == ignored_exit) {
        return 0;
    }

    if (WEXITSTATUS(ret) != 0) {
        NETHUB_LOGE("command failed: %s (exit=%d)", cmd, WEXITSTATUS(ret));
        return -1;
    }

    return 0;
}

static int netif_exists(const char *ifname)
{
    char cmd[256];

    snprintf(cmd, sizeof(cmd), "ip link show dev %s >/dev/null 2>&1", ifname);
    return run_command(cmd, 1) == 0;
}

static int resolv_conf_has_nameserver(void)
{
    FILE *fp;
    char line[256];

    fp = fopen("/etc/resolv.conf", "r");
    if (!fp) {
        return 0;
    }

    while (fgets(line, sizeof(line), fp)) {
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

static int configure_dns(const char *dns)
{
    FILE *fp;

    if (!dns || dns[0] == '\0') {
        return 0;
    }

    if (parse_ipv4_addr(dns, &(uint32_t){0}) != 0) {
        NETHUB_LOGW("skip invalid DNS address: %s", dns);
        return 0;
    }

    if (resolv_conf_has_nameserver()) {
        NETHUB_LOGI("keep existing DNS config, skip device DNS: %s", dns);
        return 0;
    }

    fp = fopen("/etc/resolv.conf", "w");
    if (!fp) {
        NETHUB_LOGE("failed to open /etc/resolv.conf");
        return -1;
    }

    fprintf(fp, "nameserver %s\n", dns);
    fclose(fp);
    return 0;
}

int nethub_netif_clear_from_env(void)
{
    char cmd[256];
    const char *ifname = netif_name_from_env();

    if (!ifname) {
        return -1;
    }

    snprintf(cmd, sizeof(cmd), "ip rule del priority %d 2>/dev/null", NETHUB_NETIF_RULE_PRIORITY);
    (void)run_command(cmd, 2);

    snprintf(cmd, sizeof(cmd), "ip route flush table %d 2>/dev/null || true", NETHUB_NETIF_ROUTE_TABLE);
    (void)run_command(cmd, -1);

    if (!netif_exists(ifname)) {
        NETHUB_LOGD("netif %s does not exist, skip clear", ifname);
        return 0;
    }

    snprintf(cmd, sizeof(cmd), "ip route del default dev %s 2>/dev/null || true", ifname);
    (void)run_command(cmd, -1);

    snprintf(cmd, sizeof(cmd), "ip addr flush dev %s 2>/dev/null || true", ifname);
    (void)run_command(cmd, -1);

    NETHUB_LOGI("cleared netif %s", ifname);
    return 0;
}

int nethub_netif_config_from_env(const char *ip,
                                 const char *netmask,
                                 const char *gateway,
                                 const char *dns)
{
    char cmd[256];
    char broadcast[16];
    char network[16];
    int cidr;
    const char *ifname = netif_name_from_env();

    if (!ifname) {
        return -1;
    }

    if (parse_ipv4_addr(ip, &(uint32_t){0}) != 0 ||
        parse_ipv4_addr(gateway, &(uint32_t){0}) != 0) {
        NETHUB_LOGE("invalid GOTIP address: ip=%s gateway=%s", ip ? ip : "(null)", gateway ? gateway : "(null)");
        return -1;
    }

    cidr = mask_to_cidr(netmask);
    if (cidr < 0) {
        NETHUB_LOGE("invalid GOTIP netmask: %s", netmask ? netmask : "(null)");
        return -1;
    }

    if (calculate_broadcast(ip, netmask, broadcast, sizeof(broadcast)) != 0) {
        NETHUB_LOGE("failed to calculate broadcast for ip=%s mask=%s", ip, netmask);
        return -1;
    }

    if (calculate_network(ip, netmask, network, sizeof(network)) != 0) {
        NETHUB_LOGE("failed to calculate network for ip=%s mask=%s", ip, netmask);
        return -1;
    }

    snprintf(cmd, sizeof(cmd), "ip rule del priority %d 2>/dev/null", NETHUB_NETIF_RULE_PRIORITY);
    (void)run_command(cmd, 2);

    snprintf(cmd, sizeof(cmd), "ip route flush table %d 2>/dev/null || true", NETHUB_NETIF_ROUTE_TABLE);
    (void)run_command(cmd, -1);

    if (!netif_exists(ifname)) {
        NETHUB_LOGE("netif %s does not exist", ifname);
        return -1;
    }

    snprintf(cmd, sizeof(cmd), "ip addr flush dev %s 2>/dev/null", ifname);
    if (run_command(cmd, -1) != 0) {
        return -1;
    }

    snprintf(cmd, sizeof(cmd), "ip addr add %s/%d brd %s dev %s", ip, cidr, broadcast, ifname);
    if (run_command(cmd, -1) != 0) {
        return -1;
    }

    snprintf(cmd, sizeof(cmd), "ip link set %s up", ifname);
    if (run_command(cmd, -1) != 0) {
        return -1;
    }

    snprintf(cmd, sizeof(cmd), "ip route del default dev %s 2>/dev/null", ifname);
    if (run_command(cmd, 2) != 0) {
        return -1;
    }

    snprintf(cmd, sizeof(cmd), "ip route replace default via %s dev %s metric %d",
             gateway, ifname, NETHUB_NETIF_ROUTE_METRIC);
    if (run_command(cmd, -1) != 0) {
        return -1;
    }

    snprintf(cmd, sizeof(cmd), "ip route add %s/%d dev %s src %s table %d",
             network, cidr, ifname, ip, NETHUB_NETIF_ROUTE_TABLE);
    if (run_command(cmd, -1) != 0) {
        return -1;
    }

    snprintf(cmd, sizeof(cmd), "ip route add default via %s dev %s table %d",
             gateway, ifname, NETHUB_NETIF_ROUTE_TABLE);
    if (run_command(cmd, -1) != 0) {
        return -1;
    }

    snprintf(cmd, sizeof(cmd), "ip rule add from %s/32 table %d priority %d",
             ip, NETHUB_NETIF_ROUTE_TABLE, NETHUB_NETIF_RULE_PRIORITY);
    if (run_command(cmd, -1) != 0) {
        return -1;
    }

    if (configure_dns(dns) != 0) {
        return -1;
    }

    NETHUB_LOGI("configured netif %s: ip=%s mask=%s gw=%s dns=%s",
                ifname, ip, netmask, gateway, dns ? dns : "");
    return 0;
}
