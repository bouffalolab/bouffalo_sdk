
#include <string.h>

#if defined(CONFIG_BL_SDK)
#include <compiler/compiler_ld.h>
#else
#include <compiler/common.h>
#endif

#include <lmac154.h>
#include <openthread_port.h>
#include <ot_radio_trx.h>
#include <ot_utils_ext.h>

typedef  struct _linkMetricList {
    utils_dlist_t       dlist;
    otLinkMetrics       linkMetric;
    otShortAddress      mShortAddress;
    otExtAddress        mExtAddress;
} linkMetricList_t;

void otLinkMetrics_init(int8_t noisefloor)
{
    uint8_t *p = otRadioVar_ptr->linkMetrics.ieLinkMetric;

    utils_dlist_init(&otRadioVar_ptr->linkMetrics.dlist);
    otRadioVar_ptr->linkMetrics.noisefloor = noisefloor;

    ((otRadio_ieHdr_t *)p)->length = IE_VENDOR_ID;
    p += sizeof(otRadio_ieHdr_t);
    p[0] = IE_THREAD_VENDOR_ID & 0xff; 
    p[1] = (IE_THREAD_VENDOR_ID >> 8) & 0xff; 
    p[2] = (IE_THREAD_VENDOR_ID >> 16) & 0xff;
    p[4] = IE_THREAD_ENH_ACK_PROBING_IE_ID;
}

otError otLinkMetrics_configEnhAckProbing(otShortAddress      aShortAddress,
                                            const otExtAddress *aExtAddress,
                                            otLinkMetrics       aLinkMetrics)
{
    utils_dlist_t *p = NULL;
    linkMetricList_t *plm;

    if ((*(uint8_t *) &aLinkMetrics) & 0xf) {

        utils_dlist_for_each(p, &otRadioVar_ptr->linkMetrics.dlist) {
            plm = (linkMetricList_t*)p;
            if (plm->mShortAddress == aShortAddress) {
                plm->linkMetric = aLinkMetrics;
                return OT_ERROR_NONE;
            }
        }

        plm = (linkMetricList_t *)malloc(sizeof(linkMetricList_t));
        if (NULL == plm) {
            return OT_ERROR_NO_BUFS;
        }
        plm->mShortAddress = aShortAddress;
        for (uint32_t i = 0; i < sizeof(otExtAddress); i ++) {
            plm->mExtAddress.m8[i] = aExtAddress->m8[7 -i];
        }
        plm->linkMetric = aLinkMetrics;

        utils_dlist_add(&plm->dlist, &otRadioVar_ptr->linkMetrics.dlist);
    }
    else {
        utils_dlist_for_each(p, &otRadioVar_ptr->linkMetrics.dlist) {
            plm = (linkMetricList_t*)p;
            if (plm->mShortAddress == aShortAddress) {
                utils_dlist_del(&plm->dlist);
                free(plm);
                return OT_ERROR_NONE;
            }
        }
        return OT_ERROR_NOT_FOUND;
    }

    return OT_ERROR_NONE;
}

static inline uint8_t ConvertRssToLinkMargin(int8_t aNoiseFloor, int8_t aRss)
{
    int8_t linkMargin = aRss - aNoiseFloor;

    if (linkMargin < 0 || aRss == OT_RADIO_RSSI_INVALID)
    {
        linkMargin = 0;
    }

    return linkMargin;
}

ATTR_TCM_SECTION
uint32_t otLinkMetrics_genEnhAckData(otInternel_addrType_t addrType, uint8_t *pAddr, int8_t rssi, int8_t lqi, uint8_t * pData) 
{
    utils_dlist_t *p = NULL;
    linkMetricList_t *plm;
    uint32_t len = 0;

    utils_dlist_for_each(p, &otRadioVar_ptr->linkMetrics.dlist) {
        plm = (linkMetricList_t*)p;

        if (addrType == OTINTERNEL_ADDR_TYPE_SHORT) {
            if (memcmp(&plm->mShortAddress, pAddr, 2) ) {
                continue;
            }
        }
        else if (addrType == OTINTERNEL_ADDR_TYPE_EXT) {
            if (memcmp(&plm->mExtAddress, pAddr, 8) ) {
                continue;
            }
        }

        enum
        {
            kEnhAckProbingDataMaxLen = 2,
        };

        if (plm->linkMetric.mLqi) {
            pData[sizeof(otRadio_ieHdr_t) + IE_THREAD_HDR_SIZE + len++] = lqi;
        }
        if (plm->linkMetric.mLinkMargin) {
            pData[sizeof(otRadio_ieHdr_t) + IE_THREAD_HDR_SIZE + len++] = 
                ConvertRssToLinkMargin(otRadioVar_ptr->linkMetrics.noisefloor, rssi) * 255 / 130; // Linear scale Link Margin from [0, 130] to [0, 255]
        }
        if (len < kEnhAckProbingDataMaxLen && plm->linkMetric.mRssi){
            pData[sizeof(otRadio_ieHdr_t) + IE_THREAD_HDR_SIZE + len++] =
                (rssi + 130) * 255 / 130; // Linear scale RSSI from [-130, 0] to [0, 255]
        }

        if (len) {
            memcpy(pData, otRadioVar_ptr->linkMetrics.ieLinkMetric, sizeof(otRadio_ieHdr_t) + IE_THREAD_HDR_SIZE);
            ((otRadio_ieHdr_t*)pData)->length = IE_THREAD_HDR_SIZE + len;

            return sizeof(otRadio_ieHdr_t) + IE_THREAD_HDR_SIZE + len;
        }
    }

    return 0;
}
