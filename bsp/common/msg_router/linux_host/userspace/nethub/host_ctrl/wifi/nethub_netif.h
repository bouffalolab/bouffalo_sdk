#ifndef NETHUB_NETIF_H
#define NETHUB_NETIF_H

#ifdef __cplusplus
extern "C" {
#endif

int nethub_netif_config_from_env(const char *ip,
                                 const char *netmask,
                                 const char *gateway,
                                 const char *dns);
int nethub_netif_clear_from_env(void);

#ifdef __cplusplus
}
#endif

#endif /* NETHUB_NETIF_H */
