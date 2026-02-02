#include <string.h>
#include <FreeRTOS.h>
#include <timers.h>
#include <semphr.h>
#include <task.h>

#include <lwip/tcpip.h>
#include <lwip/mld6.h>
#include <lwip/ethip6.h>
#include <lwip/lwip_napt.h>

#include <openthread/link.h>
#include <openthread_port.h>
#include <openthread_br.h>
#include <otbr_rtos_lwip.h>

static struct netif                 otbr_netif;
typedef err_t (* otbr_netif_lwip_output_ip4_t)(struct netif * , struct pbuf *, const ip4_addr_t *);
typedef err_t (* otbr_netif_lwip_output_ip6_t)(struct netif * , struct pbuf *, const ip6_addr_t *);

struct netif * otbr_getThreadNetif(void) 
{
    return &otbr_netif;
}

static inline err_t otbr_lwip_netif_output(struct netif * aNetif, struct pbuf *aBuffer, const ip4_addr_t *aPeerAddr)
{
    (void)aPeerAddr;

    otbrError_t err = otbr_netif_output((otbr_lwip_netif_type_t)aNetif, aBuffer);
    if (err == OTBR_ERROR_NONE) {
        return ERR_OK;
    }
    else if (err == OTBR_ERROR_INVALID_ARGS) {
        return ERR_OK;
    }
    else if (err == OTBR_ERROR_NO_MEM) {
        return ERR_OK;
    }

    return ERR_OK;
}

err_t otbr_netif_init_callback(struct netif * aNetif)
{
    aNetif->name[0]    = 'o';
    aNetif->name[1]    = 't';
    aNetif->hwaddr_len = sizeof(otExtAddress);
    memset(aNetif->hwaddr, 0, sizeof(aNetif->hwaddr));
    aNetif->mtu   = OTBR_OPENTHREAD_CONFIG_IP6_MAX_DATAGRAM_LENGTH;
    aNetif->flags = NETIF_FLAG_BROADCAST;
    aNetif->output = (otbr_netif_lwip_output_ip4_t)otbr_lwip_netif_output;
    aNetif->output_ip6 = (otbr_netif_lwip_output_ip6_t)otbr_lwip_netif_output;
    aNetif->num = 0;

    return ERR_OK;
}

void otbr_infra_netif_output(struct pbuf *q, const ip6_addr_t *ip6addr)
{
    ethip6_output(LWIP_NETIF(otbr_getInfraNetif()), q, ip6addr);
}

bool otbr_lwip_netif_is_up(otbr_lwip_netif_type_t nif) 
{
    return nif != NULL && netif_is_up(LWIP_NETIF(nif));
}

int otbr_lwip_get_ipv6_address_num(void) 
{
    return LWIP_IPV6_NUM_ADDRESSES;
}

int otbr_lwip_get_netif_num(otbr_lwip_netif_type_t nif) 
{
    return LWIP_NETIF(nif)->num;
}

u8_t otbr_lwip_get_netif_index(otbr_lwip_netif_type_t nif) 
{
    return netif_get_index(LWIP_NETIF(nif));
}

ip4_addr_t * otbr_lwip_get_ipv4_address(otbr_lwip_netif_type_t nif) 
{
    if (nif != NULL && netif_is_up(LWIP_NETIF(nif))) {
        return ip_2_ip4(&(LWIP_NETIF(nif)->ip_addr));
    }

    return NULL;
}

ip4_addr_t * otbr_lwip_get_ipv4_mask(otbr_lwip_netif_type_t nif) 
{
    if (nif != NULL && netif_is_up(LWIP_NETIF(nif))) {
        return ip_2_ip4(&(LWIP_NETIF(nif)->netmask));
    }

    return NULL;
}

ip4_addr_t * otbr_lwip_get_ipv4_gateway(otbr_lwip_netif_type_t nif) 
{
    if (nif != NULL && netif_is_up(LWIP_NETIF(nif))) {
        return ip_2_ip4(&(LWIP_NETIF(nif)->gw));
    }

    return NULL;
}

ip_addr_t * otbr_lwip_get_ipv6_ip_addr(otbr_lwip_netif_type_t nif) 
{
    return LWIP_NETIF(nif)->ip6_addr;
}
u8_t * otbr_lwip_get_ipv6_addr_state(otbr_lwip_netif_type_t nif) 
{
    return LWIP_NETIF(nif)->ip6_addr_state;
}
ip_addr_t * otbr_lwip_get_ipv6_ip_addr_at(otbr_lwip_netif_type_t nif, int idx) 
{
    return &(LWIP_NETIF(nif)->ip6_addr[idx]);
}
ip6_addr_t * otbr_lwip_get_ipv6_ip6_addr_at(otbr_lwip_netif_type_t nif, int idx) 
{
    return &(LWIP_NETIF(nif)->ip6_addr[idx].u_addr.ip6);
}
u8_t otbr_lwip_get_ipv6_addr_state_at(otbr_lwip_netif_type_t nif, int idx) 
{
    return (LWIP_NETIF(nif)->ip6_addr_state[idx]);
}
void otbr_lwip_get_hwaddr(otbr_lwip_netif_type_t nif, uint8_t * hwaddr, uint32_t length) 
{
    memcpy(hwaddr, LWIP_NETIF(nif)->hwaddr, length);
}

struct mld_group * otbr_lwip_netif_mld6_data(otbr_lwip_netif_type_t nif) 
{
    return netif_mld6_data(LWIP_NETIF(nif));
}

void otbr_lwip_netif_mld6_data_set(otbr_lwip_netif_type_t nif, struct mld_group * group) 
{
    netif_set_client_data(LWIP_NETIF(nif), LWIP_NETIF_CLIENT_DATA_INDEX_MLD6, group);
}

netif_mld_mac_filter_fn otbr_lwip_mld_mac_filter(otbr_lwip_netif_type_t nif) 
{
    return LWIP_NETIF(nif)->mld_mac_filter;
}

bool otbr_lwip_netif_checksum_enabled(otbr_lwip_netif_type_t nif, uint16_t flag) 
{
    IF__NETIF_CHECKSUM_ENABLED(LWIP_NETIF(nif), flag) {
        return true;
    }

    return false;
}

pbuf_layer otbr_lwip_get_pbuf_hlen(pbuf_layer layer)
{
    return (pbuf_layer)(PBUF_LINK_ENCAPSULATION_HLEN + layer);
}
memp_t otbr_lwip_get_memp_type_mld6_group(void)
{
    return MEMP_MLD6_GROUP;
}

uint8_t otbr_lwip_get_dns_max_servers(void) 
{
    return DNS_MAX_SERVERS;
}

uint32_t otbr_lwip_get_max_rio_routes(void) 
{
    return OTBR_MAX_RIO_ROUTE;
}

uint32_t otbr_lwip_get_rio_timeout(void) 
{
    return OTBR_RIO_TIMEOUT;
}

char * otbr_hostname(void) 
{
#ifndef OTBR_HOSTNAME
    #define OTBR_HOSTNAME "bflb_otbr"
#endif
    uint8_t eui[8];
    static char hostname[sizeof(OTBR_HOSTNAME) + 8];

    otPlatRadioGetIeeeEui64(NULL, eui);
    snprintf(hostname, sizeof(hostname), "%s-%02x%02x", OTBR_HOSTNAME, eui[4], eui[5]);

    return hostname;
}

char * otbr_vendor_name(void) 
{
#ifndef OTBR_VENDOR_NAME
    #define OTBR_VENDOR_NAME "BouffaloLab"
#endif

    return OTBR_VENDOR_NAME;
}

char * otbr_product_name(void) 
{
#ifndef OTBR_PRODUCT_NAME
    #define OTBR_PRODUCT_NAME "OTBR"
#endif

    return OTBR_PRODUCT_NAME;
}

void otbr_udp_set_multicast_ttl(struct udp_pcb * pcb, uint32_t ttl) 
{
    udp_set_multicast_ttl(pcb, ttl);
}

void otbr_udp_set_flags(struct udp_pcb * pcb, uint32_t flag) 
{
    udp_set_flags(pcb, UDP_FLAGS_MULTICAST_LOOP);
}

void otbr_udp_clear_flags(struct udp_pcb * pcb, uint32_t flag) 
{
    udp_clear_flags(pcb, UDP_FLAGS_MULTICAST_LOOP);
}

void otbr_lwip_assert_core_locked(char * file, int line) 
{
    if(lock_tcpip_core && (sys_is_inside_interrupt() || !sys_mutex_is_locked(&lock_tcpip_core))) {
      printf("api must call with lwip core lock %s: %d\r\n", file, line);
      asm("ebreak");
   }
}

void otbr_lwip_napt_enable_no(u8_t number, int enable) 
{
    ip_napt_enable_no(number, enable);
}

void otbr_lwip_lock_tcpip_core(void) 
{
    LOCK_TCPIP_CORE();
}

void otbr_lwip_unlock_tcpip_core(void) 
{
    UNLOCK_TCPIP_CORE();
}

uint32_t otbr_rtos_time_get_ms(void) 
{
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

void otbr_rtos_delay_ms(uint32_t time_ms)
{
    vTaskDelay(pdMS_TO_TICKS(time_ms));
}

otbr_rtos_void_type_t otbr_rtos_timer_create(char * name, void *callback) 
{
    return xTimerCreate(name, pdMS_TO_TICKS(100), 1, NULL, (TimerCallbackFunction_t)callback); 
}

bool otbr_rtos_timer_start(otbr_rtos_void_type_t timerHandle) 
{
    return xTimerStart(timerHandle, 0) == pdPASS;
}

bool otbr_rtos_timer_stop(otbr_rtos_void_type_t timerHandle) 
{
    return xTimerStop(timerHandle, 0) == pdPASS;
}
bool otbr_rtos_timer_delete(otbr_rtos_void_type_t timerHandle) 
{
    return xTimerDelete(timerHandle, 0) == pdPASS;
}

otbr_rtos_void_type_t otbr_rtos_mutex_create(void) 
{
    return xSemaphoreCreateMutex();
}
void otbr_rtos_mutex_delete(otbr_rtos_void_type_t mutexHandle)
{
    vSemaphoreDelete(mutexHandle);
}
bool otbr_rtos_mutex_take(otbr_rtos_void_type_t mutexHandle, uint32_t timeout) 
{
    if (timeout == -1) {
        return xSemaphoreTakeRecursive(mutexHandle, portMAX_DELAY) == pdPASS;
    }
    else {
        return xSemaphoreTakeRecursive(mutexHandle, pdMS_TO_TICKS(timeout)) == pdPASS;
    }
}
bool otbr_rtos_mutex_give(otbr_rtos_void_type_t mutexHandle) 
{
    return xSemaphoreGiveRecursive(mutexHandle) == pdPASS;
}

otbr_rtos_void_type_t otbr_rtos_semaphore_create(void)
{
    return xSemaphoreCreateBinary();
}
void otbr_rtos_semaphore_delete(otbr_rtos_void_type_t semapHandle)
{
    vSemaphoreDelete(semapHandle);
}
bool otbr_rtos_semaphore_take(otbr_rtos_void_type_t semapHandle, uint32_t timeout) 
{
    if (timeout == -1) {
        return xSemaphoreTakeRecursive(semapHandle, portMAX_DELAY) == pdPASS;
    }
    else {
        return xSemaphoreTakeRecursive(semapHandle, pdMS_TO_TICKS(timeout)) == pdPASS;
    }
}
bool otbr_rtos_semaphore_give(otbr_rtos_void_type_t semapHandle) 
{
    return xSemaphoreGiveRecursive(semapHandle) == pdPASS;
}

otbr_rtos_void_type_t otbr_rtos_queue_create(uint32_t queue_num, uint32_t entry_size)
{
    return xQueueCreate(queue_num, entry_size);
}
void otbr_rtos_queue_delete(otbr_rtos_void_type_t queueHandle)
{
    vQueueDelete(queueHandle);
}
bool otbr_rtos_queue_receive(otbr_rtos_void_type_t queueHandle, void *p_entry, uint32_t timeout)
{
    if (-1 == timeout) {
        return xQueueReceive(queueHandle, p_entry, portMAX_DELAY) == pdTRUE;
    }
    else {
        return xQueueReceive(queueHandle, p_entry, pdMS_TO_TICKS(timeout)) == pdTRUE;
    }
}
bool otbr_rtos_queue_send(otbr_rtos_void_type_t queueHandle, void *p_entry, uint32_t timeout)
{
    if (-1 == timeout) {
        return xQueueSend(queueHandle, p_entry, portMAX_DELAY) == pdPASS;
    }
    else {
        return xQueueSend(queueHandle, p_entry, pdMS_TO_TICKS(timeout)) == pdPASS;
    }
}

otbr_rtos_void_type_t otbr_rtos_task_handle_get(void) 
{
    return xTaskGetCurrentTaskHandle();
}
bool otbr_rtos_task_event_wait(uint32_t clearEventOnEntry, uint32_t clearEventOnExit, uint32_t *pWaitEvent, uint32_t timeout)
{
    if (timeout == -1) {
        return xTaskNotifyWait(0, 0, NULL, portMAX_DELAY) == pdPASS;
    }
    else {
        return xTaskNotifyWait(0, 0, NULL, pdMS_TO_TICKS(timeout)) == pdPASS;
    }
}
bool otbr_rtos_task_event_give(otbr_rtos_void_type_t taskHandle) 
{
    return xTaskNotifyGive(taskHandle);
}

otbr_rtos_void_type_t otbr_rtos_task_create(char * taskName, uint32_t taskDepth, uint32_t taskPriority, void * taskFunc)
{
    TaskHandle_t taskHandle = NULL;

    if (xTaskCreate(taskFunc, "mdns", taskDepth, NULL, taskPriority, &taskHandle) == pdPASS) {
        return taskHandle;
    }

    return NULL;
}
void otbr_rtos_task_delete(otbr_rtos_void_type_t taskHandle)
{
    vTaskDelete(taskHandle);
}
