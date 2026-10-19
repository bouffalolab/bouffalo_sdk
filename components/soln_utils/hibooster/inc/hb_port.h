#ifndef HB_PORT_H
#define HB_PORT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct pbuf;

void *hb_malloc(size_t size);
void hb_free(void *ptr);
uint32_t hb_get_ms(void);

/* Allocate a TX pbuf using the final application's lwIP configuration. */
struct pbuf *hb_pbuf_alloc_tx(uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* HB_PORT_H */
