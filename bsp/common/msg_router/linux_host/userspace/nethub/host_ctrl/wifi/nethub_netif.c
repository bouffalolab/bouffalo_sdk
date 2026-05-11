#include "nethub_netif.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "nethub_log.h"

#define NETHUB_NETIF_DEFAULT_NAME "mr_eth0"
#define NETHUB_NETIF_ROUTE_METRIC 700

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

static int is_valid_ifname(const char *ifname)
{
    size_t len;

    if (!ifname) {
        return 0;
    }

    len = strlen(ifname);
    if (len == 0 || len >= 64) {
        return 0;
    }

    for (size_t i = 0; i < len; i++) {
        unsigned char ch = (unsigned char)ifname[i];

        if (!isalnum(ch) && ch != '_' && ch != '-' && ch != '.' && ch != ':') {
            return 0;
        }
    }

    return 1;
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

static const char *select_netif_name(void)
{
    const char *ifname = getenv("NETHUB_NETIF_NAME");
    const char *transport = getenv("NETHUB_VCHAN_TRANSPORT");

    if (ifname && ifname[0] != '\0') {
        return ifname;
    }

    if (transport && strcmp(transport, "usb") == 0) {
        NETHUB_LOGW("NETHUB_NETIF_NAME is required when NETHUB_VCHAN_TRANSPORT=usb; skip netif auto config");
        return NULL;
    }

    return NETHUB_NETIF_DEFAULT_NAME;
}

int nethub_netif_config_from_env(const char *ip,
                                 const char *netmask,
                                 const char *gateway,
                                 const char *dns)
{
    const char *ifname = select_netif_name();
    char cmd[256];
    char broadcast[16];
    int cidr;

    if (!ifname) {
        return 0;
    }

    if (!is_valid_ifname(ifname)) {
        NETHUB_LOGE("invalid netif name: %s", ifname);
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

    snprintf(cmd, sizeof(cmd), "ip route add default via %s dev %s metric %d",
             gateway, ifname, NETHUB_NETIF_ROUTE_METRIC);
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
