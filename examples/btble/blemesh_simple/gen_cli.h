#ifndef  _GENERIC_CLIENT_H_
#define  _GENERIC_CLIENT_H_

#include "common_srv.h"
#include "include/access.h"


extern const struct bt_mesh_model_op bt_mesh_gen_onoff_cli_op[];

#define BT_MESH_MODEL_GEN_ONOFF_CLI(cli_pub, cli_data, cb)         \
		BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_ONOFF_CLI, NULL, cli_pub,   \
						 cli_data, cb)

typedef void (*bt_mesh_model_gen_cb)(uint8_t value);
void mesh_gen_srv_callback_register(bt_mesh_model_gen_cb cb);
#endif
