/**
 ****************************************************************************************
 *
 * @file net_def.h
 *
 * @brief Definitions related to the networking stack
 *
 *
 ****************************************************************************************
 */

#ifndef __NET_DEF_H__
#define __NET_DEF_H__

// implement layer
typedef struct netif        inet_if_t;
typedef struct pbuf_custom  inet_buf_rx_t;
typedef struct pbuf         inet_buf_tx_t;

/// Maximum size of a interface name (including null character)
#define NET_AL_MAX_IFNAME 4

/* Flags we can use with send and recv. */
#define MSG_PEEK       0x01    /* Peeks at an incoming message */
#define MSG_WAITALL    0x02    /* Unimplemented: Requests that the function block until the full amount of data requested can be returned */
#define MSG_OOB        0x04    /* Unimplemented: Requests out-of-band data. The significance and semantics of out-of-band data are protocol-specific */
#define MSG_DONTWAIT   0x08    /* Nonblocking i/o for this operation only */
#define MSG_MORE       0x10    /* Sender will send more */
#define MSG_NOSIGNAL   0x20    /* Uninmplemented: Requests not to send the SIGPIPE signal if an attempt to send is made on a stream-oriented socket that is no longer connected. */

#endif // NET_DEF_H_
