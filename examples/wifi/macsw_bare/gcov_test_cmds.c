#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bl_fw_api.h"
#include "macsw.h"
#include "csidma.h"
#include "shell.h"
#include "wifi_mgmr.h"
#include "modules/common/src/co_dlist.h"
#include "modules/common/src/co_pool.h"
#include "modules/common/src/co_math.h"
#include "modules/mac/src/mac.h"
#include "modules/mac/src/mac_ie.h"
#include "ip/umac/src/mfp/mfp.h"
#include "ip/umac/src/twt/twt.h"

struct gcov_list_node {
    struct co_list_hdr hdr;
    int value;
};

struct gcov_dlist_node {
    struct co_dlist_hdr hdr;
    int value;
};

static int parse_int_arg(const char *arg, int *value)
{
    char *end = NULL;
    long parsed;

    if (arg == NULL || value == NULL) {
        return -1;
    }

    parsed = strtol(arg, &end, 0);
    if (end == arg || *end != '\0') {
        return -1;
    }

    *value = (int)parsed;
    return 0;
}

static const char *me_param_name(int id)
{
    switch (id) {
        case ME_PARAM_ID_TX_SINGLE_RETRY_CNT_LIMIT:
            return "tx_single_retry_cnt_limit";
        case ME_PARAM_ID_TX_AMPDU_RETRY_CNT_LIMIT:
            return "tx_ampdu_retry_cnt_limit";
        case ME_PARAM_ID_TX_AMPDU_PROTECT_ENABLE:
            return "tx_ampdu_protect_enable";
        case ME_PARAM_ID_TX_AMPDU_DROP_TO_SINGLETON_RETRYCNT_THRESHOLD:
            return "tx_ampdu_drop_to_singleton_retrycnt_threshold";
        case ME_PARAM_ID_TX_POWER:
            return "tx_power";
        case ME_PARAM_ID_LDPC_CONFIG:
            return "ldpc_config";
        default:
            return "unknown";
    }
}

static void wifi_rate_fixed_cmd(int argc, char **argv)
{
    struct wifi_mgmr_rate_config cfg = {
        .fixed_rate_idx = WIFI_RC_IDX_NO_UPDATE,
        .retry_min_rate_idx = WIFI_RC_IDX_NO_UPDATE,
    };
    int idx;
    int ret;

    if (argc < 2 || parse_int_arg(argv[1], &idx) < 0) {
        printf("usage: wifi_rate_fixed <idx>\n");
        printf("idx examples: 8=6M, 15=54M, 16=HT20_MCS0, 44=HT20_MCS7, -1=auto\n");
        return;
    }

    cfg.fixed_rate_idx = idx;
    ret = wifi_mgmr_rate_config(&cfg);
    printf("wifi_rate_fixed idx=%d ret=%d\n", idx, ret);
}

static void wifi_rate_auto_cmd(int argc, char **argv)
{
    struct wifi_mgmr_rate_config cfg = {
        .fixed_rate_idx = WIFI_RC_IDX_AUTO,
        .retry_min_rate_idx = WIFI_RC_IDX_AUTO,
    };
    int ret = wifi_mgmr_rate_config(&cfg);

    printf("wifi_rate_auto ret=%d\n", ret);
}

static void wifi_rate_retry_cmd(int argc, char **argv)
{
    struct wifi_mgmr_rate_config cfg = {
        .fixed_rate_idx = WIFI_RC_IDX_NO_UPDATE,
        .retry_min_rate_idx = WIFI_RC_IDX_NO_UPDATE,
    };
    int idx;
    int ret;

    if (argc < 2 || parse_int_arg(argv[1], &idx) < 0) {
        printf("usage: wifi_rate_retry <min_idx>\n");
        printf("idx examples: 8=6M, 16=HT20_MCS0, -1=auto\n");
        return;
    }

    cfg.retry_min_rate_idx = idx;
    ret = wifi_mgmr_rate_config(&cfg);
    printf("wifi_rate_retry min_idx=%d ret=%d\n", idx, ret);
}

static void wifi_me_param_cmd(int argc, char **argv)
{
    struct me_param_req req;
    struct me_param_req cfm;
    int id;
    int value;

    if (argc < 3 || parse_int_arg(argv[2], &id) < 0) {
        printf("usage: wifi_me_param get <id>\n");
        printf("       wifi_me_param set <id> <value> [he_mcs]\n");
        printf("ids: 0=single_retry, 1=ampdu_retry, 2=ampdu_protect, 3=ampdu_drop_to_singleton, 4=tx_power_get_only, 5=ldpc\n");
        return;
    }

    memset(&req, 0, sizeof(req));
    memset(&cfm, 0, sizeof(cfm));

    if (id < 0 || id >= ME_PARAM_ID_MAX) {
        printf("wifi_me_param invalid id=%d\n", id);
        return;
    }

    req.id = (uint8_t)id;

    if (strcmp(argv[1], "get") == 0) {
        req.cmd = ME_PARAM_CMD_GET;
    } else if (strcmp(argv[1], "set") == 0) {
        req.cmd = ME_PARAM_CMD_SET;
        if (argc < 4 || parse_int_arg(argv[3], &value) < 0) {
            printf("usage: wifi_me_param set <id> <value> [he_mcs]\n");
            return;
        }
        if (id == ME_PARAM_ID_TX_POWER) {
            printf("wifi_me_param set tx_power is skipped; use get only for this gcov command\n");
            return;
        }
        if (id == ME_PARAM_ID_LDPC_CONFIG) {
            struct me_ldpc_config_param ldpc = {
                .ldpc_rx_enable = !!value,
                .he_mcs = 1,
            };
            int he_mcs;

            if (argc >= 5 && parse_int_arg(argv[4], &he_mcs) == 0) {
                if (he_mcs < 0) {
                    he_mcs = 0;
                } else if (he_mcs > 2) {
                    he_mcs = 2;
                }
                ldpc.he_mcs = (uint8_t)he_mcs;
            }
            memcpy(req.value, &ldpc, sizeof(ldpc));
        } else {
            uint32_t v = (uint32_t)value;
            memcpy(req.value, &v, sizeof(v));
        }
    } else {
        printf("usage: wifi_me_param get|set <id> [value]\n");
        return;
    }

    macif_kmsg_call(ME_PARAM_CFM, &cfm, sizeof(cfm), ME_PARAM_REQ, TASK_ME, &req, sizeof(req));

    if (req.id == ME_PARAM_ID_LDPC_CONFIG) {
        struct me_ldpc_config_param ldpc = { 0 };
        memcpy(&ldpc, cfm.value, sizeof(ldpc));
        printf("wifi_me_param %s id=%d(%s) ldpc_rx=%d he_mcs=%u\n",
               argv[1], id, me_param_name(id), ldpc.ldpc_rx_enable, ldpc.he_mcs);
    } else {
        uint32_t out = 0;
        memcpy(&out, cfm.value, sizeof(out));
        printf("wifi_me_param %s id=%d(%s) value=%u\n",
               argv[1], id, me_param_name(id), (unsigned int)out);
    }
}

static int parse_mac_addr(const char *mac_str, uint8_t mac_addr[6])
{
    uint32_t mac_addr_tmp[6];
    int ret;

    if (mac_str == NULL || strlen(mac_str) != 17) {
        return -1;
    }

    ret = sscanf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x",
                 &mac_addr_tmp[0], &mac_addr_tmp[1], &mac_addr_tmp[2],
                 &mac_addr_tmp[3], &mac_addr_tmp[4], &mac_addr_tmp[5]);
    if (ret != 6) {
        return -1;
    }

    for (int i = 0; i < 6; i++) {
        mac_addr[i] = (uint8_t)mac_addr_tmp[i];
    }
    return 0;
}

static csi_irqreturn_t gcov_csidma_handler(const struct csidma_buf *buf, void *userdata)
{
    printf("csidma: irq buf=%u cnt=%u ts=%lu\n",
           buf->cfg.buf_idx, buf->irq_cnt, (unsigned long)buf->timestamp);
    return CSIDMA_BUF_READY;
}

static void gcov_csidma_start_cmd(int argc, char **argv)
{
    struct csi_buf_conf cfg;
    int ret;

    memset(&cfg, 0, sizeof(cfg));
    cfg.buf_idx = 0;
    cfg.csi_mode = CSI_MODE_LEGACY_LTF_ONLY;
    cfg.ts_mode = TS_MODE_PERIODIC;
    cfg.trig_itvl_ms = 100;
    cfg.isr_cb = gcov_csidma_handler;
    memset(cfg.mac_addr, 0xff, sizeof(cfg.mac_addr));

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-b") == 0 && i + 1 < argc) {
            cfg.buf_idx = (uint8_t)atoi(argv[++i]);
        } else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
            if (parse_mac_addr(argv[++i], cfg.mac_addr) < 0) {
                printf("csidma_start invalid mac\n");
                return;
            }
        } else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
            cfg.csi_mode = (uint8_t)atoi(argv[++i]);
        } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            cfg.ts_mode = (uint8_t)atoi(argv[++i]);
        } else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            cfg.trig_itvl_ms = (uint32_t)atoi(argv[++i]);
        } else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
            i++;
        } else {
            printf("usage: csidma_start [-b idx] [-a mac] [-m mode] [-t ts_mode] [-i interval_ms] [-r ready]\n");
            return;
        }
    }

    printf("csidma_start configurations:\n");
    printf("  buffer index %u\n", cfg.buf_idx);
    printf("  CSI mode %u\n", cfg.csi_mode);
    printf("  time control mode %u\n", cfg.ts_mode);
    printf("  trigger interval(ms) %lu\n", (unsigned long)cfg.trig_itvl_ms);

    ret = csidma_start(&cfg);
    if (ret) {
        printf("CSI DMA failed to start\n");
    } else {
        printf("CSI DMA started\n");
    }
}

static void gcov_csidma_stop_cmd(int argc, char **argv)
{
    int buf_idx = 0;
    int ret;

    if (argc > 1) {
        buf_idx = atoi(argv[1]);
    }

    ret = csidma_stop((uint8_t)buf_idx);
    printf("stopped csi buf %d ret=%d\n", buf_idx, ret);
}

static void gcov_csidma_ready_cmd(int argc, char **argv)
{
    int buf_idx = 0;

    if (argc > 1) {
        buf_idx = atoi(argv[1]);
    }

    csidma_buffer_ready((uint8_t)buf_idx);
    printf("csidma_ready %d\n", buf_idx);
}

static void gcov_csidma_debug_cmd(int argc, char **argv)
{
    int enable = 0;

    if (argc > 1) {
        enable = atoi(argv[1]);
    }

    csidma_enable_debug(!!enable);
    printf("csidma_debug %d\n", !!enable);
}

static void print_mac_addr(const char *name, const uint8_t mac[6])
{
    printf("%s=%02X:%02X:%02X:%02X:%02X:%02X\n",
           name, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

static void gcov_scan_item_cb(void *env, void *arg, wifi_mgmr_scan_item_t *item)
{
    int *count = (int *)arg;

    if (*count < 5) {
        printf("scan_cb[%d] ssid=%s channel=%u rssi=%d auth=%s cipher=%s mode=%s\n",
               *count, item->ssid, item->channel, item->rssi,
               wifi_mgmr_auth_to_str(item->auth),
               wifi_mgmr_cipher_to_str(item->cipher),
               wifi_mgmr_mode_to_str(item->mode));
    }
    (*count)++;
}

static void print_status_code(uint16_t code)
{
    printf("status_code %u: %s\n", code, wifi_mgmr_get_sm_status_code_str(code));
}

static void wifi_gcov_mgmr_probe_cmd(int argc, char **argv)
{
    char country[4] = { 0 };
    char set_country[3] = "CN";
    uint8_t mac[6] = { 0 };
    uint8_t bssid[6] = { 0 };
    uint8_t ap_mac[6] = { 0 };
    uint8_t channel = 0;
    uint8_t c24_cnt = 0;
    uint8_t c5_cnt = 0;
    uint8_t *c24_list = NULL;
    uint8_t *c5_list = NULL;
    uint32_t addr = 0;
    uint32_t mask = 0;
    uint32_t gw = 0;
    uint32_t dns = 0;
    int rssi = 0;
    int scan_cb_count = 0;
    int ret;

    if (argc > 1 && strlen(argv[1]) >= 2) {
        set_country[0] = argv[1][0];
        set_country[1] = argv[1][1];
    }

    printf("wifi_gcov_mgmr_probe begin country=%c%c\n", set_country[0], set_country[1]);

    ret = wifi_mgmr_sta_mac_get(mac);
    printf("sta_mac ret=%d ", ret);
    print_mac_addr("mac", mac);

    ret = wifi_mgmr_sta_get_bssid(bssid);
    printf("sta_bssid ret=%d ", ret);
    print_mac_addr("bssid", bssid);

    ret = wifi_mgmr_sta_rssi_get(&rssi);
    printf("sta_rssi ret=%d rssi=%d\n", ret, rssi);

    ret = wifi_mgmr_sta_channel_get(&channel);
    printf("sta_channel ret=%d channel=%u\n", ret, channel);

    printf("sta_aid=%d\n", wifi_mgmr_sta_aid_get());
    printf("sta_state=%d\n", wifi_mgmr_sta_state_get());
    printf("sta_status=%u sta_reason=%u\n",
           wifi_mgmr_sta_info_status_code_get(),
           wifi_mgmr_sta_info_reason_code_get());
    print_status_code(wifi_mgmr_sta_info_status_code_get());
    print_status_code(wifi_mgmr_sta_info_reason_code_get());
    print_status_code(0xffff);

    wifi_mgmr_connect_ind_stat_info_t ind_stat;
    memset(&ind_stat, 0, sizeof(ind_stat));
    ret = wifi_mgmr_sta_connect_ind_stat_get(&ind_stat);
    printf("connect_ind ret=%d ssid=%s aid=%u channel=%u security=%u\n",
           ret, ind_stat.ssid, ind_stat.aid, ind_stat.channel, ind_stat.security);

    printf("listen_itv old=%u\n", wifi_mgmr_sta_get_listen_itv());
    ret = wifi_mgmr_sta_set_listen_itv(10);
    printf("listen_itv set ret=%d new=%u\n", ret, wifi_mgmr_sta_get_listen_itv());

    ret = wifi_mgmr_sta_ip_get(&addr, &mask, &gw, &dns);
    printf("ip_get ret=%d addr=0x%08lx mask=0x%08lx gw=0x%08lx dns=0x%08lx\n",
           ret, (unsigned long)addr, (unsigned long)mask, (unsigned long)gw, (unsigned long)dns);
    if (ret == 0 && addr != 0) {
        ret = wifi_mgmr_sta_ip_set(addr, mask, gw, dns);
        printf("ip_set same ret=%d\n", ret);
    }

    ret = wifi_mgmr_set_country_code(set_country);
    printf("set_country ret=%d\n", ret);
    ret = wifi_mgmr_get_country_code(country);
    printf("get_country ret=%d country=%s\n", ret, country);
    ret = wifi_mgmr_get_channel_nums(country, &c24_cnt, &c5_cnt);
    printf("channel_nums ret=%d 2g=%u 5g=%u\n", ret, c24_cnt, c5_cnt);
    ret = wifi_mgmr_get_channel_list(country, &c24_list, &c5_list);
    printf("channel_list ret=%d first_2g=%u first_5g=%u\n",
           ret, c24_list ? c24_list[0] : 0, c5_list ? c5_list[0] : 0);

    uint32_t scan_nums = wifi_mgmr_sta_scanlist_nums_get();
    printf("scanlist_nums=%lu\n", (unsigned long)scan_nums);
    if (scan_nums > 0) {
        uint32_t dump_nums = scan_nums > 8 ? 8 : scan_nums;
        wifi_mgmr_scan_item_t *items = calloc(dump_nums, sizeof(*items));
        if (items) {
            uint32_t dumped = wifi_mgmr_sta_scanlist_dump(items, dump_nums);
            printf("scanlist_dump requested=%lu dumped=%lu\n",
                   (unsigned long)dump_nums, (unsigned long)dumped);
            for (uint32_t i = 0; i < dumped && i < 3; i++) {
                printf("scan_dump[%lu] ssid=%s channel=%u rssi=%d auth=%s cipher=%s mode=%s\n",
                       (unsigned long)i, items[i].ssid, items[i].channel, items[i].rssi,
                       wifi_mgmr_auth_to_str(items[i].auth),
                       wifi_mgmr_cipher_to_str(items[i].cipher),
                       wifi_mgmr_mode_to_str(items[i].mode));
            }
            free(items);
        }
    }
    ret = wifi_mgmr_scan_ap_all(NULL, &scan_cb_count, gcov_scan_item_cb);
    printf("scan_ap_all ret=%d count=%d\n", ret, scan_cb_count);
    printf("sta_scanlist ret=%d\n", wifi_mgmr_sta_scanlist());
    printf("scanlist_free ret=%d\n", wifi_mgmr_sta_scanlist_free());

    printf("ap_state=%d\n", wifi_mgmr_ap_state_get());
    ret = wifi_mgmr_ap_mac_get(ap_mac);
    printf("ap_mac ret=%d ", ret);
    print_mac_addr("ap_mac", ap_mac);

    uint8_t parsed_mac[6] = { 0 };
    printf("mac_parse good ret=%d\n", wifi_mgmr_mac_str_to_addr("12:34:56:78:9A:BC", parsed_mac));
    print_mac_addr("parsed_mac", parsed_mac);
    printf("mac_parse bad ret=%d\n", wifi_mgmr_mac_str_to_addr("12:34:56", parsed_mac));
    printf("mac_parse null ret=%d\n", wifi_mgmr_mac_str_to_addr(NULL, parsed_mac));

    printf("mode strings: %s %s %s\n",
           wifi_mgmr_mode_to_str(WIFI_MODE_B),
           wifi_mgmr_mode_to_str(WIFI_MODE_BGNAX),
           wifi_mgmr_mode_to_str(0xffffffff));
    printf("auth strings: %s %s %s %s\n",
           wifi_mgmr_auth_to_str(WIFI_EVENT_BEACON_IND_AUTH_OPEN),
           wifi_mgmr_auth_to_str(WIFI_EVENT_BEACON_IND_AUTH_WPA_PSK),
           wifi_mgmr_auth_to_str(WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK_WPA3_SAE),
           wifi_mgmr_auth_to_str(0xfe));
    printf("cipher strings: %s %s %s %s\n",
           wifi_mgmr_cipher_to_str(WIFI_EVENT_BEACON_IND_CIPHER_NONE),
           wifi_mgmr_cipher_to_str(WIFI_EVENT_BEACON_IND_CIPHER_AES),
           wifi_mgmr_cipher_to_str(WIFI_EVENT_BEACON_IND_CIPHER_TKIP_AES),
           wifi_mgmr_cipher_to_str(0xfe));

    printf("wifi_gcov_mgmr_probe done\n");
}

static bool gcov_list_cmp(struct co_list_hdr const *element_a,
                          struct co_list_hdr const *element_b)
{
    const struct gcov_list_node *a = (const struct gcov_list_node *)element_a;
    const struct gcov_list_node *b = (const struct gcov_list_node *)element_b;

    return a->value < b->value;
}

static void gcov_probe_lists(void)
{
    struct gcov_list_node nodes[5];
    struct gcov_list_node defaults[2];
    struct co_list list_a;
    struct co_list list_b;
    struct co_list_hdr *hdr;
    struct co_dlist dlist;
    struct gcov_dlist_node dnodes[3];
    struct co_pool pool;
    struct co_pool_hdr pool_hdr[3];
    uint32_t pool_elems[3];
    struct co_pool_hdr *alloc_hdr;

    memset(nodes, 0, sizeof(nodes));
    for (int i = 0; i < 5; i++) {
        nodes[i].value = 10 - i;
    }

    co_list_init(&list_a);
    co_list_init(&list_b);
    co_list_push_back(&list_a, &nodes[0].hdr);
    co_list_push_front(&list_a, &nodes[1].hdr);
    co_list_insert(&list_a, &nodes[2].hdr, gcov_list_cmp);
    co_list_insert_after(&list_a, &nodes[0].hdr, &nodes[3].hdr);
    co_list_insert_before(&list_a, &nodes[1].hdr, &nodes[4].hdr);
    printf("co_list count=%lu find0=%d empty=%d first=%p last=%p\n",
           (unsigned long)co_list_cnt(&list_a),
           co_list_find(&list_a, &nodes[0].hdr),
           co_list_is_empty(&list_a),
           co_list_pick(&list_a),
           co_list_pick_last(&list_a));
    co_list_extract(&list_a, &nodes[3].hdr);
    hdr = co_list_pop_front(&list_a);
    printf("co_list pop=%p count=%lu\n", hdr, (unsigned long)co_list_cnt(&list_a));

    memset(defaults, 0x5a, sizeof(defaults));
    co_list_pool_init(&list_b, defaults, sizeof(defaults[0]), 2, NULL);
    co_list_concat(&list_a, &list_b);
    if (list_a.first != NULL) {
        co_list_remove(&list_a, NULL, list_a.first);
    }
    if (list_a.first != NULL && list_a.first->next != NULL) {
        co_list_remove(&list_a, list_a.first, list_a.first->next);
    }
    printf("co_list final count=%lu\n", (unsigned long)co_list_cnt(&list_a));

    memset(dnodes, 0, sizeof(dnodes));
    co_dlist_init(&dlist);
    co_dlist_push_back(&dlist, &dnodes[0].hdr);
    co_dlist_push_front(&dlist, &dnodes[1].hdr);
    co_dlist_push_back(&dlist, &dnodes[2].hdr);
    printf("co_dlist count=%lu empty=%d first=%p next=%p\n",
           (unsigned long)co_dlist_cnt(&dlist),
           co_dlist_is_empty(&dlist),
           co_dlist_pick(&dlist),
           co_dlist_next(&dnodes[1].hdr));
    co_dlist_extract(&dlist, &dnodes[0].hdr);
    printf("co_dlist pop=%p count=%lu\n",
           co_dlist_pop_front(&dlist),
           (unsigned long)co_dlist_cnt(&dlist));

    co_pool_init(&pool, pool_hdr, pool_elems, sizeof(pool_elems[0]), 3);
    alloc_hdr = co_pool_alloc(&pool, 2);
    printf("co_pool after alloc freecnt=%lu alloc=%p elem=%p\n",
           (unsigned long)pool.freecnt, alloc_hdr, alloc_hdr ? alloc_hdr->element : NULL);
    printf("co_pool overalloc=%p\n", co_pool_alloc(&pool, 3));
    if (alloc_hdr) {
        co_pool_free(&pool, alloc_hdr, 2);
    }
    printf("co_pool final freecnt=%lu\n", (unsigned long)pool.freecnt);
}

static void gcov_probe_mac_ie(void)
{
    uint8_t ie[] = {
        MAC_ELTID_RATES, 4, 0x82, 0x84, 0x8b, 0x96,
        MAC_ELTID_EXT_RATES, 2, 0x0c, 0x12,
        MAC_ELTID_COUNTRY, 6, 'C', 'N', 0x20, 1, 13, 20,
        MAC_ELTID_DS, 1, 6,
        MAC_ELTID_TIM, 4, 0, 1, 0, 0,
        MAC_ELTID_ERP, 1, 0,
        MAC_ELTID_HT_CAPA, 26,
            0x6e, 0x01, 0x17, 0xff, 0xff, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0xdd, 0xdd, 0xdd, 0xdd,
            0xdd, 0xdd,
        MAC_ELTID_RSN_IEEE, 20,
            1, 0, 0x00, 0x0f, 0xac, 4, 1, 0, 0x00, 0x0f,
            0xac, 4, 1, 0, 0x00, 0x0f, 0xac, 2, 0, 0,
        MAC_ELTID_EXT_CAPA, 3, 1, 2, 3,
        MAC_ELTID_VHT_CAPA, 12,
            0, 0, 0, 0, 0xff, 0xff, 0, 0, 0, 0, 0, 0,
        MAC_ELTID_EXT, 4, MAC_ELTID_EXT_HE_CAPA, 1, 2, 3,
        MAC_ELTID_EXT, 4, MAC_ELTID_EXT_HE_OPERATION, 4, 5, 6,
        MAC_ELTID_OUI, 6, 0x00, 0x50, 0xf2, 0x01, 1, 0,
    };
    uint8_t bad_ie[] = { MAC_ELTID_RATES, 8, 0x82 };
    uint8_t ie_len = 0;
    uint16_t full_len = 0;
    uint16_t ext_len = 0;
    bool valid_len = false;
    uint8_t wpa_oui[] = { 0x00, 0x50, 0xf2, 0x01 };
    uint32_t base = CPU2HW(ie);

    printf("mac_ie_check good=%d bad=%d\n",
           mac_ie_check(base, sizeof(ie)),
           mac_ie_check(CPU2HW(bad_ie), sizeof(bad_ie)));
    printf("mac_ie rates=%lx len=%u ext_rates=%lx len=%u\n",
           (unsigned long)mac_ie_rates_find(base, sizeof(ie), &ie_len), ie_len,
           (unsigned long)mac_ie_ext_rates_find(base, sizeof(ie), &ie_len), ie_len);
    printf("mac_ie country=%lx rsn=%lx wpa=%lx wapi=%lx\n",
           (unsigned long)mac_ie_country_find(base, sizeof(ie), &ie_len),
           (unsigned long)mac_ie_rsn_find(base, sizeof(ie), &ie_len),
           (unsigned long)mac_ie_wpa_find(base, sizeof(ie), &ie_len),
           (unsigned long)mac_ie_wapi_find(base, sizeof(ie), &ie_len));
    printf("mac_ie tim=%lx ds=%lx erp=%lx ht=%lx vht=%lx\n",
           (unsigned long)mac_ie_tim_find(base, sizeof(ie)),
           (unsigned long)mac_ie_ds_find(base, sizeof(ie)),
           (unsigned long)mac_ie_erp_find(base, sizeof(ie)),
           (unsigned long)mac_ie_ht_capa_find(base, sizeof(ie)),
           (unsigned long)mac_ie_vht_capa_find(base, sizeof(ie)));
    printf("mac_ie he_capa=%lx he_oper=%lx ext_cap=%lx\n",
           (unsigned long)mac_ie_he_capa_find(base, sizeof(ie), &ie_len),
           (unsigned long)mac_ie_he_oper_find(base, sizeof(ie), &ie_len),
           (unsigned long)mac_ie_ext_cap_find(base, sizeof(ie), &ext_len));
    printf("mac_ie find missing=%lx vsie=%lx generic=%lx ext=%lx\n",
           (unsigned long)mac_ie_find(base, sizeof(ie), 0xee, &full_len),
           (unsigned long)mac_vsie_find(base, sizeof(ie), wpa_oui, sizeof(wpa_oui), &full_len),
           (unsigned long)mac_ie_find(base, sizeof(ie), MAC_ELTID_RATES, &full_len),
           (unsigned long)mac_ext_ie_find(base, sizeof(ie), MAC_ELTID_EXT_HE_OPERATION, &full_len));
    printf("mac_ie invalid secondary=%lx wide=%lx valid=%d wps=%d\n",
           (unsigned long)mac_ie_sec_chnl_offset_find(base, sizeof(ie), &valid_len),
           (unsigned long)mac_ie_wide_bw_chnl_find(base, sizeof(ie), &valid_len),
           valid_len,
           mac_ie_wps_pbc_check(base, sizeof(ie)));
}

static void gcov_probe_mac_mfp_twt(void)
{
    struct mac_addr mac = { .array = { 0x2211, 0x4433, 0x6655 } };
    struct mac_addr bssid = { 0 };
    struct twt_flow_tag *flows[MACSW_TWT_FLOW_NB ? MACSW_TWT_FLOW_NB : 1];
    struct twt_pending_req_tag *pending;
    uint8_t reserved[10];

    printf("mac paid sta=0x%lx ap=0x%lx rcpi=%u/%u/%u\n",
           (unsigned long)mac_paid_gid_sta_compute(&mac),
           (unsigned long)mac_paid_gid_ap_compute(&mac, 1),
           mac_rcpi_format(1), mac_rcpi_format(-120), mac_rcpi_format(-50));
    printf("mac suite akm=%d/%d/%d cipher=%d/%d/%d\n",
           mac_akm_suite_value(MAC_RSNIE_AKM_PSK),
           mac_akm_suite_value(MAC_WPA_AKM_PSK),
           mac_akm_suite_value(0xffffffff),
           mac_cipher_suite_value(MAC_RSNIE_CIPHER_CCMP_128),
           mac_cipher_suite_value(MAC_WPA_CIPHER_TKIP),
           mac_cipher_suite_value(0xffffffff));
    printf("mac bssid non_txed=%d invalid=%d\n",
           mac_nontxed_bssid_get(1, 2, &mac, &bssid),
           mac_nontxed_bssid_get(0, 9, &mac, &bssid));
    mac_ref_bssid_get(1, 2, &bssid, &mac);

    printf("mfp robust data=%d deauth=%d disassoc=%d action_public=%d action_sa=%d\n",
           mfp_is_robust_frame(MAC_FCTRL_DATA_T, 0),
           mfp_is_robust_frame(MAC_FCTRL_DEAUTHENT, 0),
           mfp_is_robust_frame(MAC_FCTRL_DISASSOC, 0),
           mfp_is_robust_frame(MAC_FCTRL_ACTION, MAC_PUBLIC_ACTION_CATEGORY),
           mfp_is_robust_frame(MAC_FCTRL_ACTION, MAC_SA_QUERY_ACTION_CATEGORY));

    twt_init();
    printf("twt init flows=%d next=%lu\n",
           twt_get_flow_num(),
           (unsigned long)twt_get_next_wakeup());
    for (int i = 0; i < 10; i++) {
        reserved[i] = twt_reserve_flow_id();
        printf("twt reserve[%d]=%u\n", i, reserved[i]);
    }
    twt_free_flow_id(reserved[0]);
    printf("twt reserve after free=%u\n", twt_reserve_flow_id());
    printf("twt dialog=%u/%u\n", twt_alloc_dialog_token(), twt_alloc_dialog_token());
    printf("twt add pending=%d/%d/%d/%d/%d\n",
           twt_add_pending_request(1, 1, 0, 0),
           twt_add_pending_request(2, 2, 0, 0),
           twt_add_pending_request(3, 3, 0, 0),
           twt_add_pending_request(4, 4, 0, 0),
           twt_add_pending_request(5, 5, 0, 0));
    pending = twt_find_pending_request(2, 0, 0);
    printf("twt find pending=%p missing=%p\n",
           pending,
           twt_find_pending_request(9, 0, 0));
    if (pending) {
        twt_remove_pending_request(pending);
    }
    twt_check_pending_timeouts();
    printf("twt flows matched=%u get=%p get_missing=%p\n",
           twt_get_flows(flows, 0, 0),
           twt_get_flow(0, 0, 0),
           twt_get_flow(0xff, 0xff, 0xff));
}

static void macsw_gcov_probe_cmd(int argc, char **argv)
{
    uint8_t crc_data[] = { 1, 2, 3, 4, 5 };

    printf("macsw_gcov_probe begin\n");
    printf("co_crc32=0x%08lx co_clz=%lu/%lu\n",
           (unsigned long)co_crc32(CPU2HW(crc_data), sizeof(crc_data), 0xffffffff),
           (unsigned long)co_clz(0),
           (unsigned long)co_clz(0x10));
    gcov_probe_lists();
    gcov_probe_mac_ie();
    gcov_probe_mac_mfp_twt();
    printf("macsw_gcov_probe done\n");
}

SHELL_CMD_EXPORT_ALIAS(wifi_rate_fixed_cmd, wifi_rate_fixed, set fixed WiFi rate for gcov test.);
SHELL_CMD_EXPORT_ALIAS(wifi_rate_auto_cmd, wifi_rate_auto, clear fixed WiFi rate for gcov test.);
SHELL_CMD_EXPORT_ALIAS(wifi_rate_retry_cmd, wifi_rate_retry, set retry min WiFi rate for gcov test.);
SHELL_CMD_EXPORT_ALIAS(wifi_me_param_cmd, wifi_me_param, get or set MACSW ME param for gcov test.);
SHELL_CMD_EXPORT_ALIAS(gcov_csidma_start_cmd, csidma_start, start csidma);
SHELL_CMD_EXPORT_ALIAS(gcov_csidma_stop_cmd, csidma_stop, stop csidma);
SHELL_CMD_EXPORT_ALIAS(gcov_csidma_ready_cmd, csidma_ready, ready csidma);
SHELL_CMD_EXPORT_ALIAS(gcov_csidma_debug_cmd, csidma_debug, debug csidma);
SHELL_CMD_EXPORT_ALIAS(wifi_gcov_mgmr_probe_cmd, wifi_gcov_mgmr_probe, probe wifi manager paths for gcov test.);
SHELL_CMD_EXPORT_ALIAS(macsw_gcov_probe_cmd, macsw_gcov_probe, probe macsw helper paths for gcov test.);
