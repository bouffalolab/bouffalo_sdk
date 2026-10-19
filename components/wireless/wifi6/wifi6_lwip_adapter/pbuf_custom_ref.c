#include "lwip/pbuf.h"
#include "lwip/mem.h"

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

    pcr = mem_malloc(sizeof(struct pbuf_custom_ref));
    if (pcr == NULL)
        return NULL;

    pcr->pc.custom_free_function = pbuf_custom_ref_free;
    /* Store the reference to the original pbuf. */
    pbuf_ref(orig);
    pcr->orig = orig;
    n = pbuf_alloced_custom(PBUF_RAW, len, PBUF_REF, &pcr->pc,
                            (void *)((char *)orig->payload + offset), len);
    /* Link the following pbuf. */
    if (orig->next) {
        n->next = orig->next;
        /* n->len is set to orig->len in pbuf_alloced_custom, fix it. */
        n->len = orig->len - offset;
    }
    return n;
}
