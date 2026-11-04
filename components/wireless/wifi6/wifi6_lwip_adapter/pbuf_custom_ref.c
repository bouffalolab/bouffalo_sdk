#include "lwip/pbuf.h"
#include "lwip/mem.h"
#include "lwip/prot/ethernet.h"

struct pbuf_custom_ref {
    struct pbuf_custom pc;
    struct pbuf *orig;
};

static void pbuf_custom_ref_free(struct pbuf *p)
{
    struct pbuf_custom_ref *pcr = (struct pbuf_custom_ref *)p;

    /* Put the original pbuf. */
    if (pcr->orig != NULL) {
        pbuf_free(pcr->orig);
        pcr->orig = NULL;
    }

    /* Free the custom pbuf_custom_ref itself. */
    mem_free(pcr);
}

/*
 * Create a new reference to the original pbuf, where the payload of the new
 * pbuf points to orig->payload + offset, with the specified len.
 */
struct pbuf *pbuf_custom_ref_create(struct pbuf *orig, u16_t offset, u16_t len)
{
    struct pbuf *n;
    struct pbuf_custom_ref *pcr;

    if (orig == NULL || offset > orig->len ||
        len == 0 || len > (u16_t)(orig->len - offset))
        return NULL;

    pcr = mem_malloc(sizeof(struct pbuf_custom_ref));
    if (pcr == NULL)
        return NULL;

    pcr->pc.custom_free_function = pbuf_custom_ref_free;
    /* One ref on this node. Do not alias orig->next. */
    pbuf_ref(orig);
    pcr->orig = orig;
    n = pbuf_alloced_custom(PBUF_RAW, len, PBUF_REF, &pcr->pc,
                            (void *)((char *)orig->payload + offset), len);
    if (n == NULL) {
        pbuf_free(orig);
        pcr->orig = NULL;
        mem_free(pcr);
        return NULL;
    }
    return n;
}

struct pbuf *pbuf_tx_ref_chain(struct pbuf *orig)
{
    struct pbuf *head;
    struct pbuf *q;
    struct pbuf *ref;

    /*
     * Copy only the Ethernet header. A custom ref covers the rest of the
     * first pbuf, and each later pbuf gets its own ref. fhost_tx_req_do()
     * strips those 14 bytes and drops the empty segment.
     */
    if (orig == NULL || orig->len < SIZEOF_ETH_HDR ||
        orig->tot_len <= SIZEOF_ETH_HDR)
        return NULL;

    head = pbuf_alloc(PBUF_RAW_TX, SIZEOF_ETH_HDR, PBUF_RAM);
    if (head == NULL)
        return NULL;
    if (pbuf_copy_partial(orig, head->payload, SIZEOF_ETH_HDR, 0) !=
        SIZEOF_ETH_HDR) {
        pbuf_free(head);
        return NULL;
    }

    if (orig->len > SIZEOF_ETH_HDR) {
        ref = pbuf_custom_ref_create(orig, SIZEOF_ETH_HDR,
                                     (u16_t)(orig->len - SIZEOF_ETH_HDR));
        if (ref == NULL) {
            pbuf_free(head);
            return NULL;
        }
        pbuf_cat(head, ref);
    }

    for (q = orig->next; q != NULL; q = q->next) {
        if (q->len == 0)
            continue;
        ref = pbuf_custom_ref_create(q, 0, q->len);
        if (ref == NULL) {
            pbuf_free(head);
            return NULL;
        }
        pbuf_cat(head, ref);
    }

    if (head->tot_len <= SIZEOF_ETH_HDR) {
        pbuf_free(head);
        return NULL;
    }
    return head;
}
