
#include <string.h>
#ifdef CONFIG_BOUFFALO_SDK
#include <compiler/compiler_ld.h>
#include <bflb_common.h>
#else
#include <compiler/common.h>
#endif

#include <lmac154.h>
#include <openthread_port.h>
#include <ot_radio_trx.h>

typedef struct {
    lmac154_ie_hdr_t hdr;
    uint8_t oui[3];
    uint8_t sub_type;
} __packed ot_link_metrics_ie_hdr_t;

typedef struct {
    utils_dlist_t   dlist;
    otLinkMetrics   metrics;
    otShortAddress  short_addr;
    otExtAddress    ext_addr;
} link_metric_entry_t;

void ot_link_metrics_init(int8_t noise_floor)
{
    utils_dlist_init(&ot_radio_ctx.link_metrics.dlist);
    ot_radio_ctx.link_metrics.noise_floor = noise_floor;
}

ATTR_TCM_SECTION
uint32_t ot_link_metrics_gen_enh_ack_data(uint8_t *short_addr, uint8_t *ext_addr,
                                          int8_t rssi, uint8_t lqi, uint8_t *out_data)
{
    utils_dlist_t *p;
    link_metric_entry_t *entry;
    ot_link_metrics_ie_hdr_t *ie_hdr = (ot_link_metrics_ie_hdr_t *)out_data;
    uint32_t payload_len = 0;
    uint8_t *payload_ptr = out_data + sizeof(ot_link_metrics_ie_hdr_t);

    utils_dlist_for_each(p, &ot_radio_ctx.link_metrics.dlist) {
        bool match = false;
        entry = (link_metric_entry_t *)p;

        if (ext_addr) {
            if (arch_memcmp(entry->ext_addr.m8, ext_addr, 8) == 0)
                match = true;
        }

        if (!match && short_addr) {
            uint16_t s_addr = short_addr[0] | ((uint16_t)short_addr[1] << 8);
            if (s_addr != 0xfffe && entry->short_addr == s_addr)
                match = true;
        }

        if (!match)
            continue;

        if (entry->metrics.mLqi)
            payload_ptr[payload_len++] = lqi;

        if (entry->metrics.mLinkMargin) {
            int8_t margin = rssi - ot_radio_ctx.link_metrics.noise_floor;
            payload_ptr[payload_len++] = (uint8_t)(margin < 0 ? 0 : margin);
        }

        if (entry->metrics.mRssi)
            payload_ptr[payload_len++] = (uint8_t)(rssi + 130);

        if (payload_len > 0) {

            ie_hdr->hdr.type = 0;
            ie_hdr->hdr.element_id = VENDOR_IE_HEADER_ID;
            ie_hdr->hdr.length = sizeof(ot_link_metrics_ie_hdr_t) + payload_len - sizeof(lmac154_ie_hdr_t);

            ie_hdr->oui[0] = (uint8_t)(IE_THREAD_VENDOR_ID & 0xff);
            ie_hdr->oui[1] = (uint8_t)((IE_THREAD_VENDOR_ID >> 8) & 0xff);
            ie_hdr->oui[2] = (uint8_t)((IE_THREAD_VENDOR_ID >> 16) & 0xff);
            ie_hdr->sub_type = IE_THREAD_ENH_ACK_PROBING_IE_ID;

            return sizeof(ot_link_metrics_ie_hdr_t) + payload_len;
        }
    }

    return 0;
}

otError otPlatRadioConfigureEnhAckProbing(otInstance *         aInstance,
                                          otLinkMetrics        aLinkMetrics,
                                          const otShortAddress aShortAddress,
                                          const otExtAddress * aExtAddress)
{
    utils_dlist_t *p;
    link_metric_entry_t *entry = NULL;
    uintptr_t flags;
    bool is_delete;

    OT_UNUSED_VARIABLE(aInstance);

    is_delete = !((*(uint8_t *)&aLinkMetrics) & 0xf);

    flags = otrEnterCrit();

    utils_dlist_for_each(p, &ot_radio_ctx.link_metrics.dlist) {
        link_metric_entry_t *tmp = (link_metric_entry_t *)p;

        if (tmp->short_addr == aShortAddress ||
            (aExtAddress && memcmp(tmp->ext_addr.m8, aExtAddress->m8, 8) == 0)) {
            entry = tmp;
            break;
        }
    }

    if (is_delete) {
        if (entry) {
            utils_dlist_del(&entry->dlist);
            free(entry);
        }
        otrExitCrit(flags);
        return entry ? OT_ERROR_NONE : OT_ERROR_NOT_FOUND;
    }

    if (entry) {
        entry->metrics = aLinkMetrics;
    } else {
        entry = malloc(sizeof(*entry));
        if (!entry) {
            otrExitCrit(flags);
            return OT_ERROR_NO_BUFS;
        }
        entry->short_addr = aShortAddress;
        if (aExtAddress) {
            for (int i = 0; i < 8; i ++) {
                entry->ext_addr.m8[i] = aExtAddress->m8[7 - i];
            }
        }
        entry->metrics = aLinkMetrics;
        utils_dlist_add(&entry->dlist, &ot_radio_ctx.link_metrics.dlist);
    }

    otrExitCrit(flags);
    return OT_ERROR_NONE;
}
