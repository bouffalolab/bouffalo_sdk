/*
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 */

#include <lwip/opt.h>
#if LWIP_RAW /* don't build if not configured for use in lwipopts.h */

#include <string.h>
#include <stdbool.h>
#include <lwip/mem.h>
#include <lwip/raw.h>
#include <lwip/icmp.h>
#include <lwip/netif.h>
#include <lwip/sys.h>
#include <lwip/timeouts.h>
#include <lwip/inet_chksum.h>
#include <lwip/prot/ip4.h>
#include <lwip/dns.h>
#include <lwip/tcpip.h>
#include <lwip/netdb.h>
#include <lwip/inet.h>
#include <utils_list.h>
#include <stdlib.h>
#include <time.h>
#include "at_net_ping.h"
#include <utils_getopt.h>
#include <at_utils.h>
#if CFG_IPV6
#include <lwip/prot/icmp6.h>
#endif

#define LOG_DISP       0

#define PING_TOTAL_NODE_LIMIT 10
#define DEFAULT_PING_INTERVAL 1000
#define DEFAULT_PING_SIZE 32
#define DEFAULT_PING_TIMEOUT 5000

static struct t_hdr* find_and_extract(struct utils_list *list, u32_t seq);
static void ping_free(void *arg);
static void ping_usage();

static void ping_prepare_echo(struct icmp_echo_hdr *iecho, u16_t len, void *arg)
{
    size_t i;
    size_t data_len = len - sizeof(struct icmp_echo_hdr);
    struct ping_var *env = (struct ping_var *)arg;
    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id     = env->id;
    iecho->seqno  = lwip_htons(++env->seq_num);

    for(i = 0; i < data_len; i++) {
        ((char*)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
    }
    iecho->chksum = inet_chksum(iecho, len);
}

#if CFG_IPV6
static void ping6_prepare_echo(struct pbuf *p, u16_t len, void *arg)
{
    size_t i;
    size_t data_len = len - sizeof(struct icmp_echo_hdr);
    struct icmp_echo_hdr *iecho = (struct icmp_echo_hdr *)p->payload;
    struct ping_var *env = (struct ping_var *)arg;

    ICMPH_TYPE_SET(iecho, ICMP6_TYPE_EREQ);

    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id     = env->id;
    iecho->seqno  = lwip_htons(++env->seq_num);

    for(i = 0; i < data_len; i++) {
        ((char*)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
    }    

    // do checksum in stack
    env->pcb->chksum_reqd = 1;
    env->pcb->chksum_offset = 2;
}
#endif

/* Ping using the raw ip */
static u8_t ping_recv(void *arg, struct raw_pcb *pcb, struct pbuf *p, const ip_addr_t *addr)
{
    struct icmp_echo_hdr *iecho;
    struct t_hdr *find_hdr;
    struct ping_var *env = (struct ping_var*)arg;
    struct icmp_echo_hdr echo_hdr;
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(pcb);
    LWIP_UNUSED_ARG(addr);
    LWIP_ASSERT("p != NULL", p != NULL);
#if LOG_DISP
    log_info("recv arg %p\r\n", arg);
    log_info("recv now\r\n");
    log_info("recv addr %s\r\n", ipaddr_ntoa(addr));
#endif

    if (ip_addr_cmp(&env->dest, addr)) {
        if (p->tot_len == (IP_HLEN + sizeof(struct icmp_echo_hdr)) + env->data_size){
            memcpy(&echo_hdr, p->payload + IP_HLEN, sizeof(struct icmp_echo_hdr));
        #if CFG_IPV6
        }else if (p->tot_len == (IP6_HLEN + sizeof(struct icmp_echo_hdr)) + env->data_size){
            memcpy(&echo_hdr, p->payload + IP6_HLEN, sizeof(struct icmp_echo_hdr));
        #endif
        }else{
            return 0;
        }
        iecho = &echo_hdr;
        if ((iecho->type == 0
            #if CFG_IPV6
             || iecho->type == ICMP6_TYPE_EREP
            #endif
            ) && (iecho->code == 0) && (iecho->id == env->id) && env->node_num > 0) {
            find_hdr = find_and_extract(&env->req_list, iecho->seqno);

            if (find_hdr) {
                env->ping_time = sys_now() - find_hdr->send_time;
                if (env->cb) {
                    env->cb(env->ping_time);
                }
                printf("%" PRId16 " bytes from %s: icmp_seq=%d ttl=%d time=%" PRId32 " ms\r\n ", p->tot_len, ipaddr_ntoa(&env->dest), ntohs(iecho->seqno), *((uint8_t *)p->payload + 8), (sys_now() - find_hdr->send_time));

                utils_memp_free(env->pool, find_hdr);
                env->node_num--;
                pbuf_free(p);
                return 1; /* eat the packet */
            }
        }
    }

    return 0; /* don't eat the packet */
}

static void ping_send(struct ping_var *env)
{
    struct pbuf *p = NULL;
    struct icmp_echo_hdr *iecho;
    struct t_hdr *time_hdr = NULL;
    size_t ping_size = sizeof(struct icmp_echo_hdr) + env->data_size;
    LWIP_ASSERT("ping_size <= 0xffff", ping_size <= 0xffff);
    p = pbuf_alloc(PBUF_IP, (u16_t)ping_size, PBUF_RAM);
    if (!p) {
        goto clean;
    }

    if ((p->len == p->tot_len) && (p->next == NULL)) {
        iecho = (struct icmp_echo_hdr *)p->payload;
        time_hdr = (struct t_hdr*)utils_memp_malloc(env->pool);
        if (!time_hdr) {
            goto clean;
        }
#if LOG_DISP
        log_info("env-adress %p\r\n", env);
        log_info("env--dest %s\r\n", ipaddr_ntoa(&env->dest));
#endif
        #if CFG_IPV6
        if(IP_IS_V6(&env->dest)){
            ping6_prepare_echo(p, (u16_t)ping_size, env);
        }
        else
        #endif
            ping_prepare_echo(iecho, (u16_t)ping_size, env);
        time_hdr->ping_seq = iecho->seqno;

        raw_sendto(env->pcb, p, &env->dest);
        time_hdr->send_time = sys_now();
        utils_list_push_back(&env->req_list, (struct utils_list_hdr*)time_hdr);
        env->node_num++;
        env->requests_count++;
    }

clean:
    if (p) {
        pbuf_free(p);
    }
}

static void ping_timeout(void *arg)
{
    struct ping_var *env = (struct ping_var*)arg;

#if LOG_DISP
    log_info("%d\r\n", env->interval);
    log_info("env-adress%p\r\n", env);
    log_info("env--dest%s\r\n", ipaddr_ntoa(&env->dest));
#endif
    while (env->req_list.first && sys_now() - ((struct t_hdr*)(env->req_list.first))->send_time >= env->timeout){
        printf("The sequence number %d timed out\r\n", ntohs(((struct t_hdr*)(env->req_list.first))->ping_seq));
        utils_memp_free(env->pool, (struct t_hdr*)(utils_list_pop_front(&env->req_list)));
        env->node_num--;
        env->ping_time = -1;
        if (env->cb) {
            env->cb(env->ping_time);
        }
    }
    if (env->node_num == PING_TOTAL_NODE_LIMIT) {
        utils_memp_free(env->pool, (struct t_hdr*)(utils_list_pop_front(&env->req_list)));
        env->node_num--;
#if LOG_DISP
        log_info("send node_num %d\r\n", env->node_num);
#endif
    }
    ping_send(env);

#if LOG_DISP
    log_info("%s\r\n", ipaddr_ntoa(&env->dest));
    log_info("request_count%d\r\n", env->requests_count);
#endif
    sys_timeout(env->interval, ping_timeout, env);

#if LOG_DISP
    log_info("%d\r\n", env->interval);
    log_info("totol_count%d\r\n", env->total_count);
#endif

    if (env->requests_count == env->total_count) {
        sys_untimeout(ping_timeout, env);
        sys_timeout(env->timeout, ping_free, env);
    }

}
#define IP_PROTO_ICMPV6 58
static void ping_free(void *arg)
{
    struct ping_var *env = (struct ping_var*)arg;
    while (env->req_list.first != NULL) {
        printf("The sequence number %d timed out\r\n", ntohs(((struct t_hdr*)(env->req_list.first))->ping_seq));
        utils_memp_free(env->pool, utils_list_pop_front(&env->req_list));
        env->node_num--;

        if (env->cb) {
            env->cb(env->ping_time);
        }
    }
    raw_remove(env->pcb);
    utils_memp_deinit(env->pool);
    vPortFree(env);
}

static int ping_init(void *arg)
{
    struct ping_var *env = (struct ping_var*)arg;
    struct raw_pcb *pcb;
    if (utils_memp_init(&env->pool, sizeof(struct t_hdr), PING_TOTAL_NODE_LIMIT, sizeof(void *))) {
        printf("pool_alloc failed\n");
        return -1;
    }
    utils_list_init(&env->req_list);
#if LOG_DISP
    log_info("env addr is %p\r\n", env);
    log_info("env-dest%s \r\n", ipaddr_ntoa(&env->dest));
#endif

    LOCK_TCPIP_CORE();
#if CFG_IPV6
    if(IP_IS_V6(&env->dest))
        pcb = raw_new(IP_PROTO_ICMPV6);
    else
#endif
        pcb = raw_new(IP_PROTO_ICMP);

    if (!pcb) {
        UNLOCK_TCPIP_CORE();
        printf("pcb null, maybe the pcb pool or sys_timeout pool is empty\r\n");
        return -1;
    }
    env->pcb = pcb;
    raw_recv(pcb, ping_recv, env);
    #if CFG_IPV6
    if(IP_IS_V6(&env->dest))
    {
        raw_bind(pcb, IP6_ADDR_ANY);
    }
    else
    #endif
        raw_bind(pcb, IP_ADDR_ANY);
    sys_timeout(0, ping_timeout, env);
    UNLOCK_TCPIP_CORE();

    return 0;
}

struct ping_var *ping_api_init(u16_t interval, u16_t size, u32_t count, u16_t timeout, ip_addr_t *dest, at_ping_cb_t cb)
{
    struct ping_var *env = NULL;
    env = (struct ping_var*)pvPortMalloc(sizeof(struct ping_var));
    if (!env){
        printf("mem malloc failed!\r\n");
        return NULL;
    }
    memset(env, 0, sizeof(struct ping_var));
    env->cb = cb;
    env->total_count = count;
    env->requests_count = 0;
    env->interval = interval;
    env->data_size = size;
    env->seq_num = 0;
    env->id = random();
    env->timeout = timeout;
    env->node_num = 0;
    env->dest = *dest;
    if (ping_init(env) != 0) {
        printf("ping_init failed\r\n");
        vPortFree(env);
        env = NULL;
    }
    return env;
}

static void ping_cmd(int argc, char **argv)
{
    //
    // -i interval
    // -s size
    // -c count
    // -W timeout
    int opt;
    getopt_env_t getopt_env;
    utils_getopt_init(&getopt_env, 0);
    u16_t interval = DEFAULT_PING_INTERVAL;
    u16_t data_size = DEFAULT_PING_SIZE;
    u32_t total_count = 0;
    u16_t timeout = DEFAULT_PING_TIMEOUT;
    ip_addr_t *dest = NULL;

    while ((opt = utils_getopt(&getopt_env, argc, argv, ":i:s:c:W:h")) != -1) {
        switch (opt) {
            case 'i':
                interval = atoi(getopt_env.optarg);
                break;
            case 's':
                data_size = atoi(getopt_env.optarg);
                break;
            case 'c':
                total_count = atoi(getopt_env.optarg);
#if LOG_DISP
                log_info("totol_count%d\r\n", total_count);
#endif
                break;
            case 'W':
                timeout = atoi(getopt_env.optarg);
                break;
            case 'h':
                ping_usage();
                return;
            case ':':
                printf("%s: %c requires an argument\r\n", *argv, getopt_env.optopt);
                ping_usage();
                return;
            case '?':
                printf("%s: unknown option %c\r\n", *argv, getopt_env.optopt);
                ping_usage();
                return;
        }
    }

    if (getopt_env.optind + 1 == argc) {
        struct hostent *hostinfo = gethostbyname(argv[getopt_env.optind]);
        if (!hostinfo) {
            printf("Failed to resolve domain name\r\n");
        }
        else
        {
            dest = (ip_addr_t *)hostinfo->h_addr;
            ping_api_init(interval, data_size, total_count, timeout, dest, NULL);
        }
    } else {
        printf("Need target address\r\n");
        ping_usage();
    }
    return;
}

static struct t_hdr* find_and_extract(struct utils_list *list, u32_t seq)
{
    struct t_hdr *scan_list;
    struct t_hdr *find= NULL;
    scan_list = (struct t_hdr*)list->first;
    if (scan_list == NULL || scan_list->ping_seq > seq)
        return NULL;
    if (scan_list->ping_seq == seq){
        find = scan_list;
        list->first = (struct utils_list_hdr*)scan_list->next;
    }
    else
    {
        while ((scan_list->next != NULL) && (scan_list->next->ping_seq < seq))
        {
            scan_list = scan_list->next;
        }
        if (scan_list->next != NULL && scan_list->next->ping_seq == seq)
        {
            if (list->last == (struct utils_list_hdr*)scan_list->next)
            {
                list->last = (struct utils_list_hdr*)scan_list;
            }
            find = scan_list->next;
            scan_list->next = find->next;
        }
    }
    return find;
}

#endif

