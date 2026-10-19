#include "hci_h4.h"
#include <string.h>
#include "bt_log.h"

void h4_init(h4_context_t *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->state = H4_STATE_TYPE;
}

static void h4_reset(h4_context_t *ctx)
{
    ctx->state = H4_STATE_TYPE;
    ctx->pkt_type = 0;
    ctx->header_len = 0;
    ctx->header_recv = 0;
    ctx->data_len = 0;
    ctx->rx_buf = NULL;
    ctx->data_ptr = NULL;
    ctx->data_remaining = 0;
}

static void h4_begin_header(h4_context_t *ctx)
{
    if (ctx->pkt_type == H4_IND_EVENT) {
        ctx->header_len = 2;
    } else if (ctx->pkt_type == H4_IND_ACL_DATA) {
        ctx->header_len = 4;
    } else {
        BT_ERR("Invalid H4 pkt type: 0x%02x", ctx->pkt_type);
        h4_reset(ctx);
        return;
    }

    ctx->header_recv = 0;
    ctx->state = H4_STATE_HEADER;
}

static void h4_header_complete(h4_context_t *ctx, const h4_callbacks_t *cb)
{
    uint16_t total_len;

    switch (ctx->pkt_type) {
    case H4_IND_ACL_DATA:
        ctx->data_len = H4_LE16(&ctx->header[2]);
        total_len = ctx->data_len + ctx->header_len;
        ctx->rx_buf = cb->alloc_buf(H4_IND_ACL_DATA, 0, total_len);
        break;

    case H4_IND_EVENT:
        ctx->data_len = ctx->header[1];
        total_len = ctx->data_len + ctx->header_len;
        ctx->rx_buf = cb->alloc_buf(H4_IND_EVENT, ctx->header[0], total_len);
        break;

    default:
        BT_ERR("Bad pkt_type in header: 0x%02x", ctx->pkt_type);
        h4_reset(ctx);
        return;
    }

    if (!ctx->rx_buf) {
        BT_ERR("Failed to allocate RX buffer for type 0x%02x", ctx->pkt_type);
        h4_reset(ctx);
        return;
    }

    memcpy(ctx->rx_buf->data, ctx->header, ctx->header_len);
    ctx->rx_buf->len = total_len;

    if (ctx->data_len == 0) {
        cb->rx_handler(ctx->rx_buf);
        ctx->rx_buf = NULL;
        h4_reset(ctx);
    } else {
        ctx->data_ptr = ctx->rx_buf->data + ctx->header_len;
        ctx->data_remaining = ctx->data_len;
        ctx->state = H4_STATE_DATA;
    }
}

static void h4_data_complete(h4_context_t *ctx, const h4_callbacks_t *cb)
{
    if (ctx->rx_buf) {
        cb->rx_handler(ctx->rx_buf);
        ctx->rx_buf = NULL;
    }
    h4_reset(ctx);
}

uint16_t h4_recv(h4_context_t *ctx, const uint8_t *data, uint16_t len,
                 const h4_callbacks_t *cb)
{
    uint16_t consumed = 0;

    while (consumed < len) {
        uint16_t avail = len - consumed;
        const uint8_t *src = data + consumed;
        uint16_t to_copy;

        switch (ctx->state) {

        case H4_STATE_TYPE:
            ctx->pkt_type = src[0];
            consumed += 1;
            h4_begin_header(ctx);
            break;

        case H4_STATE_HEADER:
            to_copy = (avail < ctx->header_len - ctx->header_recv) ?
                       avail : (ctx->header_len - ctx->header_recv);
            memcpy(&ctx->header[ctx->header_recv], src, to_copy);
            ctx->header_recv += to_copy;
            consumed += to_copy;

            if (ctx->header_recv == ctx->header_len) {
                h4_header_complete(ctx, cb);
            }
            break;

        case H4_STATE_DATA:
            to_copy = (avail < ctx->data_remaining) ?
                       avail : ctx->data_remaining;
            memcpy(ctx->data_ptr, src, to_copy);
            ctx->data_ptr += to_copy;
            ctx->data_remaining -= to_copy;
            consumed += to_copy;

            if (ctx->data_remaining == 0) {
                h4_data_complete(ctx, cb);
            }
            break;
        }
    }

    return consumed;
}
