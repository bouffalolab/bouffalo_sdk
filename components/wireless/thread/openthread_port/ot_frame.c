
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#if defined(CONFIG_BL_SDK)
#include <compiler/compiler_ld.h>
#else
#include <compiler/common.h>
#endif

#include <lmac154.h>
#include <ot_frame.h>

uint8_t * ot_parseMacHeader(uint8_t *rxbuf, otInternel_addrType_t *pDstAddrType, uint8_t **pDstAddr, 
    otInternel_addrType_t *pSrcAddrType, uint8_t **pSrcAddr, otRadio_auxSecHdr_t **pAuxHdr) 
{
    uint8_t  *pr = rxbuf;
    uint16_t fcf;

    *pDstAddrType = OTINTERNEL_ADDR_TYPE_NONE;
    *pSrcAddrType = OTINTERNEL_ADDR_TYPE_NONE;
    *pSrcAddr = NULL;
    *pDstAddr = NULL;
    *pAuxHdr = NULL;

    fcf = *(uint16_t*) pr;

    /** 2 bytes for FCF and 1 byte for seq number */
    pr += 2 + 1;

    if (LMAC154_FRAME_IS_FRAME_2015(fcf)) {

        if ((fcf & LMAC154_FRAME_ADDR_SRC_MASK) != LMAC154_FRAME_ADDR_SRC_NONE && (fcf & LMAC154_FRAME_ADDR_DEST_MASK) != LMAC154_FRAME_ADDR_DEST_NONE) {
            if (fcf & LMAC154_FRAME_PANID_COMPRESSION) {
                if ((fcf & LMAC154_FRAME_ADDR_SRC_MASK) == LMAC154_FRAME_ADDR_SRC_EXT && (fcf & LMAC154_FRAME_ADDR_DEST_MASK) == LMAC154_FRAME_ADDR_DEST_EXT) {
                    return NULL;
                }
                else {
                    pr += 2;
                }
            }
            else {
                pr += 2;
            }

            if ((fcf & LMAC154_FRAME_ADDR_DEST_MASK) == LMAC154_FRAME_ADDR_DEST_SHORT) {
                *pDstAddrType = OTINTERNEL_ADDR_TYPE_SHORT;
                *pDstAddr = pr;
                pr += 2;
            }
            else if ((fcf & LMAC154_FRAME_ADDR_DEST_MASK) == LMAC154_FRAME_ADDR_DEST_EXT) {
                *pDstAddrType = OTINTERNEL_ADDR_TYPE_EXT;
                *pDstAddr = pr;
                pr += 8;
            }

            if (!(fcf & LMAC154_FRAME_PANID_COMPRESSION) && 
                (fcf & LMAC154_FRAME_ADDR_SRC_MASK) != LMAC154_FRAME_ADDR_SRC_EXT && (fcf & LMAC154_FRAME_ADDR_DEST_MASK) != LMAC154_FRAME_ADDR_DEST_EXT) {
                pr += 2;
            }
        }
        else {
            if ((fcf & LMAC154_FRAME_ADDR_SRC_MASK) == LMAC154_FRAME_ADDR_SRC_NONE && (fcf & LMAC154_FRAME_ADDR_DEST_MASK) == LMAC154_FRAME_ADDR_DEST_NONE) {
                if (fcf & LMAC154_FRAME_PANID_COMPRESSION) {
                    pr += 2;
                }
            }
            else {
                if (!(fcf & LMAC154_FRAME_PANID_COMPRESSION)) {
                    pr += 2;
                }
                if ((fcf & LMAC154_FRAME_ADDR_DEST_MASK) == LMAC154_FRAME_ADDR_DEST_SHORT) {
                    *pDstAddrType = OTINTERNEL_ADDR_TYPE_SHORT;
                    *pDstAddr = pr;
                    pr += 2;
                }
                else if ((fcf & LMAC154_FRAME_ADDR_DEST_MASK) == LMAC154_FRAME_ADDR_DEST_EXT) {
                    *pDstAddrType = OTINTERNEL_ADDR_TYPE_EXT;
                    *pDstAddr = pr;
                    pr += 8;
                }
            }
        }

        if ((fcf & LMAC154_FRAME_ADDR_SRC_MASK) == LMAC154_FRAME_ADDR_SRC_SHORT) {
            *pSrcAddrType = OTINTERNEL_ADDR_TYPE_SHORT;
            *pSrcAddr = pr;
            pr += 2;
        }
        else if ((fcf & LMAC154_FRAME_ADDR_SRC_MASK) == LMAC154_FRAME_ADDR_SRC_EXT) {
            *pSrcAddrType = OTINTERNEL_ADDR_TYPE_EXT;
            *pSrcAddr = pr;
            pr += 8;
        }
    }
    else {

        pr += 2;
        if ((fcf & LMAC154_FRAME_ADDR_DEST_MASK) == LMAC154_FRAME_ADDR_DEST_SHORT) {
            *pDstAddrType = OTINTERNEL_ADDR_TYPE_SHORT;
            *pDstAddr = pr;
            pr += 2;
        }
        else if ((fcf & LMAC154_FRAME_ADDR_DEST_MASK) == LMAC154_FRAME_ADDR_DEST_EXT) {
            *pDstAddrType = OTINTERNEL_ADDR_TYPE_EXT;
            *pDstAddr = pr;
            pr += 8;
        }

        if (!(fcf & LMAC154_FRAME_PANID_COMPRESSION)) {
            pr += 2;
        }

        if ((fcf & LMAC154_FRAME_ADDR_SRC_MASK) == LMAC154_FRAME_ADDR_SRC_SHORT) {
            *pSrcAddrType = OTINTERNEL_ADDR_TYPE_SHORT;
            *pSrcAddr = pr;
            pr += 2;
        }
        else if ((fcf & LMAC154_FRAME_ADDR_SRC_MASK) == LMAC154_FRAME_ADDR_SRC_EXT) {
            *pSrcAddrType = OTINTERNEL_ADDR_TYPE_EXT;
            *pSrcAddr = pr;
            pr += 8;
        }
    }

    // Set security header
    if (fcf & LMAC154_FRAME_SECURITY_MASK) {
        if ((*pr & LMAC154_FRAME_AUX_KEY_ID_MODE_MASK) != LMAC154_FRAME_AUX_KEY_ID_MODE_1) {
            return NULL;
        }

        *pAuxHdr = (otRadio_auxSecHdr_t *)pr;

        if (((*pAuxHdr)->keyIdMode) == (LMAC154_FRAME_AUX_KEY_ID_MODE_0 >> 3)) {
            pr += offsetof(otRadio_auxSecHdr_t, keyId);
        }
        else if (((*pAuxHdr)->keyIdMode) == (LMAC154_FRAME_AUX_KEY_ID_MODE_1 >> 3)) {
            pr += sizeof(otRadio_auxSecHdr_t);
        }
        else if (((*pAuxHdr)->keyIdMode) == (LMAC154_FRAME_AUX_KEY_ID_MODE_2 >> 3)) {
            pr += (sizeof(otRadio_auxSecHdr_t) + 4);
        }
    }

    return pr;
}

otInternel_addrType_t ot_getSourceAddress(uint8_t * rx_buf, uint32_t rx_len, uint8_t * p_srcAddr) 
{
    uint8_t                  *pld = NULL;
    otInternel_addrType_t    dstAddrType, srcAddrType;
    uint8_t                  *pDstAddr = NULL, *pSrcAddr = NULL;
    otRadio_auxSecHdr_t      *pAuxHdr = NULL;

    pld = ot_parseMacHeader(rx_buf, &dstAddrType, &pDstAddr, &srcAddrType, &pSrcAddr, &pAuxHdr);
    if (pld >= rx_buf + rx_len) {
        return OTINTERNEL_ADDR_TYPE_NONE;
    }

    if (srcAddrType == OTINTERNEL_ADDR_TYPE_SHORT) {
        memcpy(p_srcAddr, pSrcAddr, 2);
        return OTINTERNEL_ADDR_TYPE_SHORT;
    }
    else if (srcAddrType == OTINTERNEL_ADDR_TYPE_EXT) {
        memcpy(p_srcAddr, pSrcAddr, 8);
        return OTINTERNEL_ADDR_TYPE_EXT;
    }

    return OTINTERNEL_ADDR_TYPE_NONE;
}

ATTR_TCM_SECTION
uint8_t * ot_genAckMacHeader(uint8_t **rxbuf, uint32_t len, uint32_t *ackbuf, otInternel_addrType_t *pSrcAddrType, 
    uint8_t **pSrcAddr, otRadio_auxSecHdr_t **pAuxHdr, uint32_t macFrameCounter) 
{
    uint8_t  *pr = *rxbuf, *pa = (uint8_t *)ackbuf;
    uint16_t fcf, *pafcf;
    uint32_t ret;

    *pSrcAddrType = OTINTERNEL_ADDR_TYPE_NONE;
    *pSrcAddr = NULL;
    *pAuxHdr = NULL;

    fcf = *(uint16_t*) pr;

    pafcf = (uint16_t*) ackbuf;
    pr += 2; pa += 2;

    // Set sequence number
    *pa ++ = *pr ++;

    // if (LMAC154_FRAME_IS_FRAME_2015(fcf)) {
        /** enh-ack for frame version 2015 */
        pafcf[0] = LMAC154_FRAME_CONTROL_FRAME_TYPE_ACK | LMAC154_FRAME_VERSION_2015 | LMAC154_FRAME_ADDR_SRC_NONE;
        if (fcf & LMAC154_FRAME_SECURITY_MASK){
            pafcf[0] |= LMAC154_FRAME_SECURITY_MASK;
        }

        if ((fcf & LMAC154_FRAME_ADDR_SRC_MASK) != LMAC154_FRAME_ADDR_SRC_NONE && (fcf & LMAC154_FRAME_ADDR_DEST_MASK) != LMAC154_FRAME_ADDR_DEST_NONE) {
            if (fcf & LMAC154_FRAME_PANID_COMPRESSION) {
                if ((fcf & LMAC154_FRAME_ADDR_SRC_MASK) == LMAC154_FRAME_ADDR_SRC_EXT && (fcf & LMAC154_FRAME_ADDR_DEST_MASK) == LMAC154_FRAME_ADDR_DEST_EXT) {
                    return NULL;
                }
                else {
                    *pa ++ = *pr ++;
                    *pa ++ = *pr ++;
                }
            }
            else {
                *pa ++ = *pr ++;
                *pa ++ = *pr ++;
            }

            if ((fcf & LMAC154_FRAME_ADDR_DEST_MASK) == LMAC154_FRAME_ADDR_DEST_SHORT) {
                pr += 2;
            }
            else if ((fcf & LMAC154_FRAME_ADDR_DEST_MASK) == LMAC154_FRAME_ADDR_DEST_EXT) {
                pr += 8;
            }

            if (!(fcf & LMAC154_FRAME_PANID_COMPRESSION) && 
                (fcf & LMAC154_FRAME_ADDR_SRC_MASK) != LMAC154_FRAME_ADDR_SRC_EXT && (fcf & LMAC154_FRAME_ADDR_DEST_MASK) != LMAC154_FRAME_ADDR_DEST_EXT) {
                pr += 2;
            }
        }
        else {
            if ((fcf & LMAC154_FRAME_ADDR_SRC_MASK) == LMAC154_FRAME_ADDR_SRC_NONE && (fcf & LMAC154_FRAME_ADDR_DEST_MASK) == LMAC154_FRAME_ADDR_DEST_NONE) {
                if (fcf & LMAC154_FRAME_PANID_COMPRESSION) {
                    *pa ++ = *pr ++;
                    *pa ++ = *pr ++;
                }
            }
            else {
                if (!(fcf & LMAC154_FRAME_PANID_COMPRESSION)) {
                    *pa ++ = *pr ++;
                    *pa ++ = *pr ++;
                }
                if ((fcf & LMAC154_FRAME_ADDR_DEST_MASK) == LMAC154_FRAME_ADDR_DEST_SHORT) {
                    pr += 2;
                }
                else if ((fcf & LMAC154_FRAME_ADDR_DEST_MASK) == LMAC154_FRAME_ADDR_DEST_EXT) {
                    pr += 8;
                }
            }
        }

        if ((fcf & LMAC154_FRAME_ADDR_SRC_MASK) == LMAC154_FRAME_ADDR_SRC_SHORT) {
            pafcf[0] |= LMAC154_FRAME_ADDR_DEST_SHORT;
            *pSrcAddrType = OTINTERNEL_ADDR_TYPE_SHORT;
            memcpy(pa, pr, 2); 
            *pSrcAddr = pr;
            pr += 2; pa += 2;
        }
        else if ((fcf & LMAC154_FRAME_ADDR_SRC_MASK) == LMAC154_FRAME_ADDR_SRC_EXT) {
            pafcf[0] |= LMAC154_FRAME_ADDR_DEST_EXT;
            *pSrcAddrType = OTINTERNEL_ADDR_TYPE_EXT;
            memcpy(pa, pr, 8); 
            *pSrcAddr = pr;
            pr += 8; pa += 8;
        }
    // }
    // else {

    //     /** imm-ack */
    //     pafcf[0] = LMAC154_FRAME_CONTROL_FRAME_TYPE_ACK;

    //     /** skip dest panid */
    //     pr += 2;

    //     if ((fcf & LMAC154_FRAME_ADDR_DEST_MASK) == LMAC154_FRAME_ADDR_DEST_SHORT) {
    //         pr += 2;
    //     }
    //     else if ((fcf & LMAC154_FRAME_ADDR_DEST_MASK) == LMAC154_FRAME_ADDR_DEST_EXT) {
    //         pr += 8;
    //     }

    //     if (!(fcf & LMAC154_FRAME_PANID_COMPRESSION)) {
    //         /** skip source panid */
    //         pr += 2;
    //     }

    //     if ((fcf & LMAC154_FRAME_ADDR_SRC_MASK) == LMAC154_FRAME_ADDR_SRC_SHORT) {
    //         *pSrcAddrType = OTINTERNEL_ADDR_TYPE_SHORT;
    //         *pSrcAddr = pr;
    //         pr += 2;
    //     }
    //     else if ((fcf & LMAC154_FRAME_ADDR_SRC_MASK) == LMAC154_FRAME_ADDR_SRC_EXT) {
    //         *pSrcAddrType = OTINTERNEL_ADDR_TYPE_EXT;
    //         *pSrcAddr = pr;
    //         pr += 8;
    //     }
    // }

    // Set security header
    if (fcf & LMAC154_FRAME_SECURITY_MASK) {

        *pAuxHdr = (otRadio_auxSecHdr_t *)pr;
        pa[0] = *pr ++;
        memcpy(pa + 1, &macFrameCounter, 4);
        pr += 4;

        if (((*pAuxHdr)->keyIdMode) == (LMAC154_FRAME_AUX_KEY_ID_MODE_1 >> 3)) {
            pa[5] = *pr ++;
        }
        else if (((*pAuxHdr)->keyIdMode) == (LMAC154_FRAME_AUX_KEY_ID_MODE_2 >> 3)) {
            pa[5] = *pr ++;
            pr += 4;
        }
    }

    if ((fcf & (LMAC154_FRAME_VERSION_2015 | LMAC154_FRAME_IE_MASK)) == (LMAC154_FRAME_VERSION_2015 | LMAC154_FRAME_IE_MASK)) {
        len -= 2;
        if ((ret = ot_getIElength(pr, *rxbuf + len)) != 0) {
            pr += ret;
        }
    }

    *rxbuf = pr;

    return pa;
}

uint32_t ot_getIElength(uint8_t *buf, uint8_t *buf_end)
{
    otRadio_ieHdr_t *pIeHdr = NULL;
    uint8_t *p = buf;

    do {
        pIeHdr = (otRadio_ieHdr_t *) p;

        p += pIeHdr->length + sizeof(otRadio_ieHdr_t);
        if (p > buf_end) {
            return 0;
        }

        if (pIeHdr->elementId == IE_TERINATION2_ID) {
            break;
        }

    } while (p < buf_end);

    return p - buf;
}

ATTR_TCM_SECTION
uint32_t ot_genCslIE(uint16_t period, uint16_t phase, uint8_t * pData) 
{
    otThreadCslIE_t ie;

    ie.bf.hdr.length = sizeof(otThreadCslIE_t) - sizeof(otRadio_ieHdr_t);
    ie.bf.hdr.elementId = IE_CSL_ID;
    ie.bf.hdr.type = 0;
    ie.bf.period = period;
    ie.bf.phase = phase;
    memcpy(pData, &ie, sizeof(otThreadCslIE_t));

    return sizeof(otThreadCslIE_t);
}

uint8_t * ot_findIe(uint8_t *pkt, uint32_t len) 
{
    uint8_t                 *pld = NULL;
    otInternel_addrType_t    dstAddrType, srcAddrType;
    uint8_t                 *pDstAddr = NULL, *pSrcAddr = NULL;
    otRadio_auxSecHdr_t     *pAuxHdr = NULL;

    pld = ot_parseMacHeader(pkt, &dstAddrType, &pDstAddr, &srcAddrType, &pSrcAddr, &pAuxHdr);
    if (pld && (pld - pkt) < len ) {
        return pld;
    }

    return NULL;
}

uint8_t * ot_skipIe(uint8_t *pkt, uint32_t len2end) 
{
    otRadio_ieHdr_t *pIeHdr = NULL;
    uint8_t *p = pkt;

    do {
        pIeHdr = (otRadio_ieHdr_t *) p;
        p += pIeHdr->length + sizeof(otRadio_ieHdr_t);

        if (pIeHdr->elementId == IE_TERINATION2_ID) {
            return p;
        }

        if (p == len2end + pkt) {
            return p;
        }

    } while (p < len2end + pkt);

    return NULL;
}

uint8_t * ot_findIeEntry(uint8_t *pkt, uint32_t len2end, uint32_t id)
{
    otRadio_ieHdr_t *pIeHdr = NULL;
    uint8_t *p = pkt;

    do {
        pIeHdr = (otRadio_ieHdr_t *) p;

        if (pIeHdr->elementId == id) {
            break;
        }
        if (pIeHdr->elementId == IE_TERINATION2_ID) {
            return NULL;
        }
        p += pIeHdr->length + sizeof(otRadio_ieHdr_t);

    } while (p < len2end + pkt);

    return p;
}


uint8_t * ot_getPayload(uint8_t *pkt, uint32_t len) 
{
    uint8_t                 *pld = NULL, *pie;
    otInternel_addrType_t    dstAddrType, srcAddrType;
    uint8_t                 *pDstAddr = NULL, *pSrcAddr = NULL;
    otRadio_auxSecHdr_t     *pAuxHdr = NULL;
    uint32_t    footlen;

    pld = ot_parseMacHeader(pkt, &dstAddrType, &pDstAddr, &srcAddrType, &pSrcAddr, &pAuxHdr);
    if (pld) {
        footlen = 2 + (pld - pkt);
        if (pAuxHdr) {
            footlen = footlen + (4 << ((pAuxHdr->secLvl & 3) - 1));
        }
        if (footlen == len) {
            return pld;
        }
        else if (footlen < len) {
            len -= footlen;
            if (LMAC154_FRAME_IS_IE_EXIST(*(uint32_t *)pkt)) {
                pie = ot_skipIe(pld, len);
                if (pie && (pie - pld) <= len) {
                    return pie;
                }
            }
            else {
                return NULL;
            }
        }
    }

    return NULL;
}

uint32_t ot_getPayloadLen(uint8_t *pkt, uint32_t len) 
{
    uint8_t                 *pld = NULL, *pie;
    otInternel_addrType_t    dstAddrType, srcAddrType;
    uint8_t                 *pDstAddr = NULL, *pSrcAddr = NULL;
    otRadio_auxSecHdr_t     *pAuxHdr = NULL;
    uint32_t    footlen;

    pld = ot_parseMacHeader(pkt, &dstAddrType, &pDstAddr, &srcAddrType, &pSrcAddr, &pAuxHdr);
    if (pld) {
        footlen = 2 + (pld - pkt);
        if (pAuxHdr) {
            footlen = footlen + (4 << ((pAuxHdr->secLvl & 3) - 1));
        }
        if (footlen == len) {
            return 0;
        }
        else if (footlen < len) {
            len -= footlen;
            if (LMAC154_FRAME_IS_IE_EXIST(*(uint32_t *)pkt)) {
                pie = ot_skipIe(pld, len);
                if (pie && (pie - pld) <= len) {
                    return len - (pie - pld);
                }
            }
            else {
                return len;
            }
        }
    }

    return -1;
}
