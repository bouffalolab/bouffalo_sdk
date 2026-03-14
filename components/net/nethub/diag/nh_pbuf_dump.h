#ifndef NH_PBUF_DUMP_H
#define NH_PBUF_DUMP_H

struct pbuf;

/*
 * Internal diagnostic helper that prints a bounded protocol summary for a pbuf.
 * It is intended for debug tracing only and must not be treated as a parser API.
 */
void nh_diag_dump_pbuf(const char *tag, const struct pbuf *p);

#endif
