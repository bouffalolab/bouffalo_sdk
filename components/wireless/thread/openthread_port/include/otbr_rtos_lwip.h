
#ifndef __OTBR_LWIP_H
#define __OTBR_LWIP_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include <lwip/netif.h>
#include <lwip/ip_addr.h>
#include <lwip/ip4_addr.h>
#include <lwip/ip6_addr.h>
#include <lwip/udp.h>
#include <lwip/err.h>

#define LWIP_NETIF(x)               ((struct netif *)x)

typedef void * otbr_rtos_void_type_t;

otbr_lwip_netif_type_t otbr_getInfraNetif(void);
struct netif * otbr_getThreadNetif(void);
err_t otbr_netif_init_callback(struct netif * nif);
void otbr_infra_netif_output(struct pbuf *q, const ip6_addr_t *ip6addr);

bool otbr_lwip_netif_is_up(otbr_lwip_netif_type_t nif);
u8_t otbr_lwip_get_netif_index(otbr_lwip_netif_type_t nif);
int otbr_lwip_get_ipv6_address_num(void);
int otbr_lwip_get_netif_num(otbr_lwip_netif_type_t nif);

ip4_addr_t * otbr_lwip_get_ipv4_address(otbr_lwip_netif_type_t nif);
ip4_addr_t * otbr_lwip_get_ipv4_mask(otbr_lwip_netif_type_t nif);
ip4_addr_t * otbr_lwip_get_ipv4_gateway(otbr_lwip_netif_type_t nif);
// ip_addr_t * otbr_lwip_get_ipv6_ip_addr(otbr_lwip_netif_type_t nif);
// u8_t * otbr_lwip_get_ipv6_addr_state(otbr_lwip_netif_type_t nif);
ip_addr_t * otbr_lwip_get_ipv6_ip_addr_at(otbr_lwip_netif_type_t nif, int idx);
ip6_addr_t * otbr_lwip_get_ipv6_ip6_addr_at(otbr_lwip_netif_type_t nif, int idx); 
u8_t otbr_lwip_get_ipv6_addr_state_at(otbr_lwip_netif_type_t nif, int idx);

void otbr_lwip_get_hwaddr(otbr_lwip_netif_type_t nif, uint8_t * hwaddr, uint32_t length);

struct mld_group * otbr_lwip_netif_mld6_data(otbr_lwip_netif_type_t nif);
void otbr_lwip_netif_mld6_data_set(otbr_lwip_netif_type_t nif, struct mld_group * group);
netif_mld_mac_filter_fn otbr_lwip_mld_mac_filter(otbr_lwip_netif_type_t nif);
bool otbr_lwip_netif_checksum_enabled(otbr_lwip_netif_type_t nif, uint16_t flag);

pbuf_layer otbr_lwip_get_pbuf_hlen(pbuf_layer layer);
memp_t otbr_lwip_get_memp_type_mld6_group(void);
uint8_t otbr_lwip_get_dns_max_servers(void);
uint32_t otbr_lwip_get_max_rio_routes(void);
uint32_t otbr_lwip_get_rio_timeout(void);

char * otbr_vendor_name(void);
char * otbr_product_name(void);

void otbr_udp_set_multicast_ttl(struct udp_pcb * pcb, uint32_t ttl);
void otbr_udp_set_flags(struct udp_pcb * pcb, uint32_t flag);
void otbr_udp_clear_flags(struct udp_pcb * pcb, uint32_t flag);

void otbr_lwip_napt_enable_no(u8_t number, int enable);

void otbr_lwip_assert_core_locked(char * file, int line);
void otbr_lwip_lock_tcpip_core(void);
void otbr_lwip_unlock_tcpip_core(void); 

uint32_t otbr_rtos_time_get_ms(void);
void otbr_rtos_delay_ms(uint32_t time_ms);

otbr_rtos_void_type_t otbr_rtos_timer_create(char * name, void *callback);
bool otbr_rtos_timer_start(otbr_rtos_void_type_t timerHandle);
bool otbr_rtos_timer_stop(otbr_rtos_void_type_t timerHandle);
bool otbr_rtos_timer_delete(otbr_rtos_void_type_t timerHandle);

otbr_rtos_void_type_t otbr_rtos_mutex_create(void);
void otbr_rtos_mutex_delete(otbr_rtos_void_type_t mutexHandle);
bool otbr_rtos_mutex_take(otbr_rtos_void_type_t mutexHandle, uint32_t timeout);
bool otbr_rtos_mutex_give(otbr_rtos_void_type_t mutexHandle);

otbr_rtos_void_type_t otbr_rtos_semaphore_create(void);
void otbr_rtos_semaphore_delete(otbr_rtos_void_type_t semapHandle);
bool otbr_rtos_semaphore_take(otbr_rtos_void_type_t semapHandle, uint32_t timeout);
bool otbr_rtos_semaphore_give(otbr_rtos_void_type_t semapHandle);

otbr_rtos_void_type_t otbr_rtos_queue_create(uint32_t queue_num, uint32_t entry_size);
void otbr_rtos_queue_delete(otbr_rtos_void_type_t queueHandle);
bool otbr_rtos_queue_receive(otbr_rtos_void_type_t queueHandle, void *p_entry, uint32_t timeout);
bool otbr_rtos_queue_send(otbr_rtos_void_type_t queueHandle, void *p_entry, uint32_t timeout);

otbr_rtos_void_type_t otbr_rtos_task_handle_get(void);
bool otbr_rtos_task_event_wait(uint32_t clearEventOnEntry, uint32_t clearEventOnExit, uint32_t *pWaitEvent, uint32_t timeout);
bool otbr_rtos_task_event_give(otbr_rtos_void_type_t taskHandle);

otbr_rtos_void_type_t otbr_rtos_task_create(char * taskName, uint32_t taskDepth, uint32_t taskPriority, void * taskFunc);
void otbr_rtos_task_delete(otbr_rtos_void_type_t taskHandle);

#if !defined(LWIP_MULTICAST_TX_OPTIONS) || !LWIP_MULTICAST_TX_OPTIONS
#error "LWIP_MULTICAST_TX_OPTIONS is required."
#endif

#if !defined(LWIP_IPV4) || !LWIP_IPV4
#error "LWIP_IPV4 is required."
#endif

#if !defined(LWIP_IPV6) || !LWIP_IPV6
#error "LWIP_IPV6 is required."
#endif

#if defined(LWIP_SINGLE_NETIF) && LWIP_SINGLE_NETIF
#error "LWIP_SINGLE_NETIF is NOT required."
#endif

#if defined(LWIP_IPV6_SCOPES) && LWIP_IPV6_SCOPES
#error "LWIP_IPV6_SCOPES is NOT required."
#endif

#if !defined(LWIP_TCPIP_CORE_LOCKING) || !LWIP_TCPIP_CORE_LOCKING
#error "LWIP_TCPIP_CORE_LOCKING is required."
#endif

#if !defined(LWIP_MPU_COMPATIBLE) || LWIP_MPU_COMPATIBLE
#error "LWIP_MPU_COMPATIBLE is required and defined to 0."
#endif

#if !defined(NO_SYS) || NO_SYS
#error "NO_SYS is required and defined to 0."
#endif

#ifdef __cplusplus
}
#endif

#endif
