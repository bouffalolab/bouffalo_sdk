#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <utils_getopt.h>

#include "netifd_nano.h"

#include <shell.h>

static void nifd_print_usage()
{
    printf("Usage:\r\n");
    printf("  netifd config -i <interface> [-a <ipaddr>] [-m <netmask>] [-g <gateway>]\r\n");
    printf("  netifd up -i <interface>\r\n");
    printf("  netifd down -i <interface>\r\n");
    printf("  netifd dhcp start -i <interface>\r\n");
    printf("  netifd dhcp stop -i <interface>\r\n");
    printf("  netifd bridge create -b <bridge>\r\n");
    printf("  netifd bridge delete -b <bridge>\r\n");
    printf("  netifd bridge add -b <bridge> -i <interface>\r\n");
    printf("  netifd bridge remove -b <bridge> -i <interface>\r\n");
    printf("  netifd status\r\n");
}

static void print_interface_status()
{
    struct netif *netif = netif_list;

    printf("Default Interface %c%c%d, @:%p:\r\n",
            netif_default->name[0], netif_default->name[1],
            netif_default->num, netif_default);
    while (netif != NULL) {
        char ipaddr_str[IP4ADDR_STRLEN_MAX];
        char netmask_str[IP4ADDR_STRLEN_MAX];
        char gw_str[IP4ADDR_STRLEN_MAX];
        char mac_str[18];

        ip4addr_ntoa_r(netif_ip4_addr(netif), ipaddr_str, sizeof(ipaddr_str));
        ip4addr_ntoa_r(netif_ip4_netmask(netif), netmask_str, sizeof(netmask_str));
        ip4addr_ntoa_r(netif_ip4_gw(netif), gw_str, sizeof(gw_str));
        snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
                 netif->hwaddr[0], netif->hwaddr[1], netif->hwaddr[2],
                 netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);

        printf("Interface %c%c%d, @:%p:\r\n",
                netif->name[0], netif->name[1], netif->num, netif);
        printf("  IP Address: %s\r\n", ipaddr_str);
        printf("  Netmask: %s\r\n", netmask_str);
        printf("  Gateway: %s\r\n", gw_str);
        printf("  MAC Address: %s\r\n", mac_str);
        printf("  MTU: %d\r\n", netif->mtu);
        printf("  Flags: UP %s, Broadcast %s, Link UP %s, ETHARP %s, Ethernet %s, "
                "IGMP %s, MLD6 %s\r\n",
                netif->flags & NETIF_FLAG_UP ? "Y" : "N",
                netif->flags & NETIF_FLAG_BROADCAST ? "Y" : "N",
                netif->flags & NETIF_FLAG_LINK_UP ? "Y" : "N",
                netif->flags & NETIF_FLAG_ETHARP ? "Y" : "N",
                netif->flags & NETIF_FLAG_ETHERNET ? "Y" : "N",
                netif->flags & NETIF_FLAG_IGMP ? "Y" : "N",
                netif->flags & NETIF_FLAG_MLD6 ? "Y" : "N");
        netif = netif->next;
    }
}

int cmd_netifd_nano(int argc, char *argv[])
{
    getopt_env_t getopt_env;
    if (argc < 2) {
        nifd_print_usage();
        return -1;
    }

    const char *cmd = argv[1];

    if (strcmp(cmd, "config") == 0) {
        char *interface = NULL;
        char *ipaddr = NULL;
        char *netmask = NULL;
        char *gateway = NULL;
        int opt;

        utils_al_getopt_init(&getopt_env, 0);
        while ((opt = utils_al_getopt(&getopt_env, argc - 1, argv + 1, "i:a:m:g:")) != -1) {
            switch (opt) {
                case 'i':
                    interface = getopt_env.optarg;
                    break;
                case 'a':
                    ipaddr = getopt_env.optarg;
                    break;
                case 'm':
                    netmask = getopt_env.optarg;
                    break;
                case 'g':
                    gateway = getopt_env.optarg;
                    break;
                default:
                    nifd_print_usage();
                    return -1;
            }
        }

        if (interface == NULL) {
            nifd_print_usage();
            return -1;
        }

        netifd_config_t config;
        strncpy(config.name, interface, sizeof(config.name) - 1);
        if (ipaddr) ip4addr_aton(ipaddr, &config.ipaddr);
        if (netmask) ip4addr_aton(netmask, &config.netmask);
        if (gateway) ip4addr_aton(gateway, &config.gw);

        if (netifd_init(&config) == 0) {
            printf("Interface %s configured successfully.\r\n", interface);
        } else {
            printf("Failed to configure interface %s.\r\n", interface);
        }
    } else if (strcmp(cmd, "up") == 0) {
        char *interface = NULL;
        int opt;

        utils_al_getopt_init(&getopt_env, 0);
        while ((opt = utils_al_getopt(&getopt_env, argc - 1, argv + 1, "i:")) != -1) {
            if (opt == 'i') {
                interface = getopt_env.optarg;
            }
        }

        if (interface == NULL) {
            nifd_print_usage();
            return -1;
        }

        if (netifd_up(interface) == 0) {
            printf("Interface %s brought up successfully.\r\n", interface);
        } else {
            printf("Failed to bring up interface %s.\r\n", interface);
        }
    } else if (strcmp(cmd, "down") == 0) {
        char *interface = NULL;
        int opt;

        utils_al_getopt_init(&getopt_env, 0);
        while ((opt = utils_al_getopt(&getopt_env, argc - 1, argv + 1, "i:")) != -1) {
            if (opt == 'i') {
                interface = getopt_env.optarg;
            }
        }

        if (interface == NULL) {
            nifd_print_usage();
            return -1;
        }

        if (netifd_down(interface) == 0) {
            printf("Interface %s brought down successfully.\r\n", interface);
        } else {
            printf("Failed to bring down interface %s.\r\n", interface);
        }
    } else if (strcmp(cmd, "dhcp") == 0) {
        if (argc < 4) {
            nifd_print_usage();
            return -1;
        }

        const char *dhcp_cmd = argv[2];
        char *interface = NULL;
        int opt;

        utils_al_getopt_init(&getopt_env, 0);
        while ((opt = utils_al_getopt(&getopt_env, argc - 2, argv + 2, "i:")) != -1) {
            if (opt == 'i') {
                interface = getopt_env.optarg;
            }
        }

        if (interface == NULL) {
            nifd_print_usage();
            return -1;
        }

        if (strcmp(dhcp_cmd, "start") == 0) {
            if (netifd_dhcp_start(interface) == 0) {
                printf("DHCP client started on interface %s.\r\n", interface);
            } else {
                printf("Failed to start DHCP client on interface %s.\r\n", interface);
            }
        } else if (strcmp(dhcp_cmd, "stop") == 0) {
            if (netifd_dhcp_stop(interface) == 0) {
                printf("DHCP client stopped on interface %s.\r\n", interface);
            } else {
                printf("Failed to stop DHCP client on interface %s.\r\n", interface);
            }
        } else {
            nifd_print_usage();
            return -1;
        }
    } else if (strcmp(cmd, "pppoe") == 0) {
        return -1;
    } else if (strcmp(cmd, "bridge") == 0) {
        if (argc < 4) {
            nifd_print_usage();
            return -1;
        }

        const char *bridge_cmd = argv[2];
        char *bridge = NULL;
        char *interface = NULL;
        int opt;

        utils_al_getopt_init(&getopt_env, 0);
        while ((opt = utils_al_getopt(&getopt_env, argc - 2, argv + 2, "b:i:")) != -1) {
            switch (opt) {
                case 'b':
                    bridge = getopt_env.optarg;
                    break;
                case 'i':
                    interface = getopt_env.optarg;
                    break;
                default:
                    nifd_print_usage();
                    return -1;
            }
        }

        if (bridge == NULL) {
            nifd_print_usage();
            return -1;
        }

        if (strcmp(bridge_cmd, "create") == 0) {
            struct netif *netif = netifd_bridge_create(bridge);
            if (netif >= 0) {
                printf("Bridge interface %s%d (renamed from %s) created successfully.\r\n",
                        netif->name, netif->num, bridge);
            } else {
                printf("Failed to create bridge interface %s.\r\n", bridge);
            }
        } else if (strcmp(bridge_cmd, "delete") == 0) {
            if (netifd_bridge_delete(bridge) == 0) {
                printf("Bridge interface %s deleted successfully.\r\n", bridge);
            } else {
                printf("Failed to delete bridge interface %s.\r\n", bridge);
            }
        } else if (strcmp(bridge_cmd, "add") == 0) {

            if (interface == NULL) {
                nifd_print_usage();
                return -1;
            }
            struct netif* current_netif = netif_find(interface);
            if (!current_netif) {
                printf("Interface %s is not found\r\n", interface);
                return -1;
            }
            current_netif->flags |= NETIF_FLAG_ETHERNET;

            if (netifd_bridge_add_if(bridge, interface) == 0) {
                printf("Interface %s added to bridge %s successfully.\r\n", interface, bridge);
            } else {
                printf("Failed to add interface %s to bridge %s.\r\n", interface, bridge);
            }
        } else if (strcmp(bridge_cmd, "remove") == 0) {
            if (interface == NULL) {
                nifd_print_usage();
                return -1;
            }
            if (netifd_bridge_remove_if(bridge, interface) == 0) {
                printf("Interface %s removed from bridge %s successfully.\r\n", interface, bridge);
            } else {
                printf("Failed to remove interface %s from bridge %s.\r\n", interface, bridge);
            }
        } else {
            nifd_print_usage();
            return -1;
        }
    } else if (strcmp(cmd, "status") == 0) {
        print_interface_status();
    } else {
        nifd_print_usage();
        return -1;
    }

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_netifd_nano, netifd, netifd config.);

int netifd_nano_init(void)
{
    return 0;
}
