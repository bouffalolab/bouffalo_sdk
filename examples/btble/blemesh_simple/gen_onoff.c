#include <string.h>
#include <errno.h>
#include <net/buf.h>
#include "zephyr.h"
#include "mesh_model_opcode.h"
#include "gen_cli.h"
#include "common_cli.h"
#include "bt_log.h"
#include "src/mesh.h"
#include "transport.h"
#include "access.h"


static void generic_status(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    printf("generic_status\r\n");
}

const struct bt_mesh_model_op bt_mesh_gen_onoff_cli_op[] = {
    { BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS, 1, generic_status },
    BT_MESH_MODEL_OP_END,
};


static int bt_mesh_client_send_msg(struct bt_mesh_model *model,
                            u32_t opcode,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *msg,
                            k_work_handler_t timer_handler,
                            s32_t timeout, bool need_ack,
                            const struct bt_mesh_send_cb *cb,
                            void *cb_data)
{
    int err = 0;

    if (ctx->addr == BT_MESH_ADDR_UNASSIGNED) {
        BT_ERR("%s, Invalid DST 0x%04x", __func__, ctx->addr);
        return -EINVAL;
    }

    if (!need_ack) {
        return bt_mesh_model_send(model, ctx, msg, cb, cb_data);
    }

    if (!BT_MESH_ADDR_IS_UNICAST(ctx->addr)) {

        return bt_mesh_model_send(model, ctx, msg, cb, cb_data);
    }

    return err;
}


static int gen_set_state(struct bt_mesh_model *model,
		       struct bt_mesh_msg_ctx *ctx, uint32_t op, void *set, bool need_ack)
{
    int err = 0;

    NET_BUF_SIMPLE_DEFINE(msg, BT_MESH_TX_SDU_MAX);

    bt_mesh_model_msg_init(&msg, op);

    switch (op) {
    case BLE_MESH_MODEL_OP_GEN_ONOFF_SET:
    case BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK: {
        bt_mesh_gen_onoff_set_t *p_set = (bt_mesh_gen_onoff_set_t *)set;
        net_buf_simple_add_u8(&msg, p_set->onoff);
        net_buf_simple_add_u8(&msg, p_set->tid);
        if (p_set->op_en) {
            net_buf_simple_add_u8(&msg, p_set->trans_time);
            net_buf_simple_add_u8(&msg, p_set->delay);
        }
        break;
    }

    default:
        BT_ERR("%s, Not a Generic Client Set message opcode", __func__);
        err = -EINVAL;
    }

    err = bt_mesh_client_send_msg(model, op, ctx, &msg,
                                  NULL, 0, need_ack,
                                  NULL, NULL);
    if (err) {
        BT_ERR("%s, Failed to send Generic Set message (err %d)", __func__, err);
    }

    return err;
}


int bt_mesh_gen_cli_set_state(struct bt_mesh_model *model,
		       struct bt_mesh_msg_ctx *ctx, uint32_t op, void *set)
{
    bool need_ack = false;

    if (!ctx || !model || !set) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    switch (op) {
    case BLE_MESH_MODEL_OP_GEN_ONOFF_SET:
        need_ack = true;
		__attribute__ ((fallthrough));
    case BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK: {
        break;
    }
    default:
        BT_ERR("%s, Not a Generic Client Set message opcode", __func__);
        return -EINVAL;
    }

    return gen_set_state(model, ctx, op, set, need_ack);
}


void cmd_gen_cli_send(int argc, char **argv)
{
	int err;

    if(argc != 2){
        printf("Number of Parameters is not correct\r\n");
        return;
    }

    struct bt_mesh_model *p_mdl = NULL;
    struct bt_mesh_msg_ctx ctx = {
        .app_idx = 0,
        .net_idx = 0,
        .addr = 0,
        .send_ttl = 3,
    };

    get_uint16_from_string(&argv[1], &ctx.addr);
    printf("Destination addr 0x%04x\r\n", ctx.addr);
    
    static bt_mesh_generic_client_set_state_t gen_client_set;
    gen_client_set.onoff_set.tid++;
    gen_client_set.onoff_set.onoff = (gen_client_set.onoff_set.onoff+1)&0x01;

    p_mdl = bt_mesh_model_find(bt_mesh_elem_find(0),
					 BT_MESH_MODEL_ID_GEN_ONOFF_CLI);

    err = bt_mesh_gen_cli_set_state(p_mdl, &ctx, BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK, &gen_client_set);
    if(err){
        printf("Set error [%d].", err);
    }
}
