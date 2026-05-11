#include "nethub_ctrl_proto.h"

static int proto_private_init(void)
{
    return NETHUB_WIFI_ERR_NOT_SUPPORTED;
}

static void proto_private_deinit(void)
{
}

static const nethub_ctrl_proto_ops_t g_proto_private = {
    .name = "private",
    .init = proto_private_init,
    .deinit = proto_private_deinit,
};

const nethub_ctrl_proto_ops_t *nethub_ctrl_proto_private(void)
{
    return &g_proto_private;
}
