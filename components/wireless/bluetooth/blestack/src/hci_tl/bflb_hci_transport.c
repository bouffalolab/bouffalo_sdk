#include "bflb_hci_transport.h"

static const struct bflb_hci_transport *g_transport;
static const bflb_hci_trans_callbacks_t *g_callbacks;

int bflb_hci_transport_register(bflb_hci_transport_type_t type,
                                const struct bflb_hci_transport *transport)
{
    (void)type;
    if (g_transport) {
        return -1;
    }
    g_transport = transport;
    return 0;
}

int bflb_hci_transport_register_cb(const bflb_hci_trans_callbacks_t *cb)
{
    if (g_callbacks) {
        return -1;
    }
    g_callbacks = cb;
    return 0;
}

const struct bflb_hci_transport *bflb_hci_transport_get(void)
{
    return g_transport;
}

const bflb_hci_trans_callbacks_t *bflb_hci_transport_get_callbacks(void)
{
    return g_callbacks;
}

void bflb_hci_transport_unregister(void)
{
    g_transport = NULL;
    g_callbacks = NULL;
}
