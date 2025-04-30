#include <stdlib.h>
#include "conn.h"
#include "gatt.h"
#include "FreeRTOS.h"
#include "task.h"
#if defined(CONFIG_SHELL)
#include "shell.h"
#else
#include "cli.h"
#endif /* CONFIG_SHELL */

#include "mesh_cli_cmds.h"
#include "src/include/mesh.h"
#include "bt_errno.h"
#include "access.h"
#include "src/mesh.h"
#include "net.h"
#include "transport.h"
#include "foundation.h"
#include "mesh_settings.h"
#include "adv.h"
#include "beacon.h"
#include "hci_core.h"
#include "bt_log.h"
#if defined(CONFIG_BT_MESH_MODEL)
#include "model_opcode.h"
#include "state_transition.h"
#include "state_binding.h"
#if (defined(CONFIG_BT_MESH_MODEL_GEN_SRV) || defined(CONFIG_BT_MESH_MODEL_GEN_CLI))
#include "bfl_ble_mesh_generic_model_api.h"
#endif
#if (defined(CONFIG_BT_MESH_MODEL_LIGHT_SRV) || defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI))
#include "bfl_ble_mesh_lighting_model_api.h"
#endif
#include "bfl_ble_mesh_local_data_operation_api.h"
#include "bfl_ble_mesh_networking_api.h"
#include "generic_server.h"
#else
#if defined(CONFIG_BT_MESH_MODEL_GEN_SRV)
#include "gen_srv.h"
#endif
#endif /* CONFIG_BT_MESH_MODEL */

#if defined(CONFIG_BT_MESH_SYNC)
#include "blsync_ble_app.h" // added blemesh sync 
#endif

#if defined(CONFIG_BT_SETTINGS)
#include "easyflash.h"
#endif
#include <../../blestack/src/include/bluetooth/crypto.h>
#include "local_operation.h"

#if defined(CONFIG_BT_MESH_LOW_POWER)
#include "lpn.h"
#endif
#define CUR_FAULTS_MAX 4

bool blemesh_inited = false;
#if defined(CONFIG_BT_MESH_LOW_POWER)
//below value is for Tmall Genie
u8_t dev_uuid[16] = {0xA8,0x01,0x71,0x5e,0x1c,0x00,0x00,0xe4,0x46,0x46,0x63,0xa7,0xf8,0x02,0x00,0x00};
u8_t auth_value[16] = {0x78,0x8A,0xE3,0xEE,0x0F,0x2A,0x7E,0xFA,0xD3,0x67,0x35,0x81,0x41,0xFE,0x1B,0x06};
#else
u8_t dev_uuid[16] = {0xA8,0x01,0x71,0xe0,0x1a,0x00,0x00,0x0f,0x7e,0x35,0x63,0xa7,0xf8,0x02,0x00,0x00};
u8_t auth_value[16] = {0x7f,0x80,0x1a,0xf4,0xa0,0x8c,0x50,0x39,0xae,0x7d,0x7b,0x44,0xa0,0x92,0xd9,0xc2};
#endif

static bt_mesh_input_action_t input_act;
static u8_t input_size;
static u8_t cur_faults[CUR_FAULTS_MAX];
static u8_t reg_faults[CUR_FAULTS_MAX * 2];

static struct {
	u16_t local;
	u16_t dst;
	u16_t net_idx;
	u16_t app_idx;
} net = {
	.local = BT_MESH_ADDR_UNASSIGNED,
	.dst = BT_MESH_ADDR_UNASSIGNED,
};
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)

#define MMDL_DST_ADDR 0x0001
#define MMDL_NET_IDX  0x0000
#define MMDL_APP_IDX  0x0000

static bfl_ble_mesh_client_common_param_t client_common = {
    .msg_timeout = 0,
    .msg_role = ROLE_NODE,
    .ctx.send_ttl = 3,
    .ctx.addr = MMDL_DST_ADDR,
    .ctx.net_idx = MMDL_NET_IDX,
    .ctx.app_idx = MMDL_APP_IDX,
};
static void common_get(u16_t id, uint32_t opcode)
{
    struct bt_mesh_model *mesh_model = NULL;
    const struct bt_mesh_comp *mesh_comp = bt_mesh_comp_get();

    for(int i = 0; i < mesh_comp->elem_count; ++i){
        mesh_model = bt_mesh_model_find(&mesh_comp->elem[i], id);
        if(mesh_model != NULL){
            break;
        }
    }
    if(mesh_model == NULL){
        return;
    }
    client_common.model = mesh_model;
    client_common.opcode = opcode;
    bfl_ble_mesh_generic_client_get_state(&client_common, NULL);

}
static void common_set(u16_t id, uint32_t opcode, uint8_t ack,
                bfl_ble_mesh_generic_client_set_state_t* gen_client_set)
{
    struct bt_mesh_model *mesh_model = NULL;
    const struct bt_mesh_comp *mesh_comp = bt_mesh_comp_get();

    for(int i = 0; i < mesh_comp->elem_count; ++i){
        mesh_model = bt_mesh_model_find(&mesh_comp->elem[i], id);
        if(mesh_model != NULL){
            break;
        }
    }
    if(mesh_model == NULL){
        return;
    }

    client_common.opcode = opcode - ack;
    client_common.model = mesh_model;

    bfl_ble_mesh_generic_client_set_state(&client_common, gen_client_set);

}
#endif

#if defined(BL602) || defined(BL702) || defined(BL606P) || defined(BL616) || defined(BL808) || defined(BL702L)
#define vOutputString(...)  printf(__VA_ARGS__)
#else
#define vOutputString(...)  bl_print(SYSTEM_UART_ID, PRINT_MODULE_CLI, __VA_ARGS__)
#endif

#if defined(CONFIG_SHELL)
#define BLEMESH_CLI(func) static void blemeshcli_##func(int argc, char **argv)
#else
#define BLEMESH_CLI(func) static void blemeshcli_##func(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
#endif

static void prov_reset(void);
static void gen_dev_uuid(void);
static void link_open(bt_mesh_prov_bearer_t bearer);
static void link_close(bt_mesh_prov_bearer_t bearer);
static int output_number(bt_mesh_output_action_t action, u32_t number);
#ifdef CONFIG_BT_MESH_PROVISIONER
static u8_t capabilities(prov_caps_t* prv_caps, prov_start_t* prv_start);
#endif
static int output_string(const char *str);
static void prov_input_complete(void);
static void prov_complete(u16_t net_idx, u16_t addr);
static void prov_reset(void);
static int input(bt_mesh_input_action_t act, u8_t size);
static void get_faults(u8_t *faults, u8_t faults_size, u8_t *dst, u8_t *count);
static void health_current_status(struct bt_mesh_health_cli *cli, u16_t addr,
				  u8_t test_id, u16_t cid, u8_t *faults, size_t fault_count);
static int fault_get_cur(struct bt_mesh_model *model, u8_t *test_id,
			 u16_t *company_id, u8_t *faults, u8_t *fault_count);
static int fault_get_reg(struct bt_mesh_model *model, u16_t cid,
			 u8_t *test_id, u8_t *faults, u8_t *fault_count);
static int fault_clear(struct bt_mesh_model *model, uint16_t cid);
static int fault_test(struct bt_mesh_model *model, uint8_t test_id, uint16_t cid);
static void attn_on(struct bt_mesh_model *model);
static void attn_off(struct bt_mesh_model *model);

BLEMESH_CLI(init);
#if defined(CONFIG_BT_MESH_GATT_PROXY)
BLEMESH_CLI(proxy_identity);
#endif
#if defined(CONFIG_BT_MESH_PROVISIONER)
BLEMESH_CLI(pvnr_init);
#endif
BLEMESH_CLI(addr_get);
BLEMESH_CLI(set_dev_uuid);
BLEMESH_CLI(input_num);
BLEMESH_CLI(input_str);
#ifndef CFG_IOT_SDK
BLEMESH_CLI(read_str);
#endif
#if defined(CONFIG_BT_MESH_MODEL)
#if defined(CONFIG_BT_MESH_MODEL_GEN_CLI)
BLEMESH_CLI(gen_oo_cli);
#endif
#if defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI)
BLEMESH_CLI(light_lgn_cli);
BLEMESH_CLI(light_ctl_cli);
BLEMESH_CLI(light_hsl_cli);
#endif
#if defined(CONFIG_BT_MESH_MODEL_VENDOR_CLI)
BLEMESH_CLI(vendor_cli);
#endif
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
BLEMESH_CLI(models_cli);
BLEMESH_CLI(pts_models_client_cli);
#endif
#endif /* CONFIG_BT_MESH_MODEL */
BLEMESH_CLI(pb);
#if defined(CONFIG_BT_MESH_PB_ADV)
#if defined(CONFIG_BT_MESH_PROVISIONER)
BLEMESH_CLI(provision_adv);
#endif /* CONFIG_BT_MESH_PROVISIONER */
#endif /* CONFIG_BT_MESH_PB_ADV */
BLEMESH_CLI(reset);
BLEMESH_CLI(net_send);
BLEMESH_CLI(seg_send);
BLEMESH_CLI(rpl_clr);
BLEMESH_CLI(ivu_test);
BLEMESH_CLI(iv_update);
BLEMESH_CLI(fault_set);

#if defined(CONFIG_BT_MESH_LOW_POWER)
BLEMESH_CLI(lpn_set);
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS) 
BLEMESH_CLI(lpn_poll);
BLEMESH_CLI(lpn_clear_friend_send);
BLEMESH_CLI(lpn_friend_req);
BLEMESH_CLI(lpn_group_del);
BLEMESH_CLI(lpn_group_add);
#endif
#endif

#if defined(CONFIG_BT_MESH_CDB)
BLEMESH_CLI(cdb_create);
BLEMESH_CLI(cdb_clear);
BLEMESH_CLI(cdb_show);
BLEMESH_CLI(cdb_node_add);
BLEMESH_CLI(cdb_node_del);
BLEMESH_CLI(cdb_subnet_add);
BLEMESH_CLI(cdb_subnet_del);
BLEMESH_CLI(cdb_app_key_add);
BLEMESH_CLI(cdb_app_key_del);
#endif /* CONFIG_BT_MESH_CDB */
#if defined(CONFIG_BT_MESH_PROVISIONER)
BLEMESH_CLI(beacon_listen);
BLEMESH_CLI(provision);
BLEMESH_CLI(node_cfg);
#endif /* CONFIG_BT_MESH_PROVISIONER */
#if defined(CONFIG_BT_MESH_PROVISIONER)
BLEMESH_CLI(get_comp);
BLEMESH_CLI(set_dst);
BLEMESH_CLI(krp_get);
BLEMESH_CLI(krp_set);
BLEMESH_CLI(cfg_bcn_get);
BLEMESH_CLI(cfg_bcn_set);
BLEMESH_CLI(cfg_dttl_get);
BLEMESH_CLI(cfg_dttl_set);
BLEMESH_CLI(cfg_gpxy_get);
BLEMESH_CLI(cfg_gpxy_set);
BLEMESH_CLI(friend);
BLEMESH_CLI(relay);
BLEMESH_CLI(node_identify);
BLEMESH_CLI(node_reset);
BLEMESH_CLI(network_trans);
BLEMESH_CLI(lpn_timeout_get);
BLEMESH_CLI(net_key_add);
/* Added by bouffalo */
BLEMESH_CLI(net_key_update);
BLEMESH_CLI(net_key_get);
BLEMESH_CLI(net_key_del);
BLEMESH_CLI(app_key_add);
/* Added by bouffalo */
BLEMESH_CLI(app_key_update);
BLEMESH_CLI(app_key_get);
BLEMESH_CLI(app_key_del);
/* Added by bouffalo */
BLEMESH_CLI(kr_update);
BLEMESH_CLI(mod_app_bind);
BLEMESH_CLI(mod_app_unbind);
BLEMESH_CLI(mod_app_get);
BLEMESH_CLI(mod_sub_add);
BLEMESH_CLI(mod_sub_ow);
BLEMESH_CLI(mod_sub_del);
BLEMESH_CLI(mod_sub_del_all);
BLEMESH_CLI(mod_sub_add_va);
BLEMESH_CLI(mod_sub_ow_va);
BLEMESH_CLI(mod_sub_del_va);
BLEMESH_CLI(mod_sub_get);
BLEMESH_CLI(mod_pub);
/* Added by bouffalo */
BLEMESH_CLI(mod_pub_va);
BLEMESH_CLI(hb_sub);
BLEMESH_CLI(hb_pub);

BLEMESH_CLI(clhm_fault);
BLEMESH_CLI(clhm_period);
BLEMESH_CLI(clhm_ats);
#endif
#if defined(CFG_NODE_SEND_CFGCLI_MSG) && defined(CONFIG_BT_MESH_CDB)
BLEMESH_CLI(pvnr_devkey);
#endif
#if defined(CONFIG_BT_MESH_SYNC)
BLEMESH_CLI(sync_start);
BLEMESH_CLI(sync_stop);
#endif

#if defined(CONFIG_BT_MESH_TEST)
static int nodelist_check(uint16_t addr);
static int nodelist_check_clear();
BLEMESH_CLI(nodelist_op);
#endif /* CONFIG_BT_MESH_TEST */

#if defined(CONFIG_BT_MESH_PROVISIONER)
static void print_node_added(u16_t net_idx, u8_t uuid[16], u16_t addr, u8_t num_elem);
#endif

static struct bt_mesh_prov prov = {
	.uuid = dev_uuid,
    .oob_info = 0,
	.link_open = link_open,
	.link_close = link_close,
	.complete = prov_complete,
	.reset = prov_reset,
	.static_val = auth_value,
	.static_val_len = 16,
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
	.output_size = 8,
#else
	.output_size = 6,
#endif
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
	.output_actions = (BT_MESH_BLINK | BT_MESH_BEEP | BT_MESH_VIBRATE | BT_MESH_DISPLAY_NUMBER | BT_MESH_DISPLAY_STRING),
#else
	.output_actions = (BT_MESH_DISPLAY_NUMBER | BT_MESH_DISPLAY_STRING),
#endif
#ifdef CONFIG_BT_MESH_PROVISIONER
	.capabilities = capabilities,
	.node_added = print_node_added,
#endif
	.output_number = output_number,
	.output_string = output_string,
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
	.input_size = 8,
#else
	.input_size = 6,
#endif
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
	.input_actions = (BT_MESH_PUSH | BT_MESH_TWIST | BT_MESH_ENTER_NUMBER | BT_MESH_ENTER_STRING),
#else
	.input_actions = (BT_MESH_ENTER_NUMBER | BT_MESH_ENTER_STRING),
#endif
	.input = input,
	.input_complete = prov_input_complete,
};

static const struct bt_mesh_health_srv_cb health_srv_cb = {
	.fault_get_cur = fault_get_cur,
	.fault_get_reg = fault_get_reg,
	.fault_clear = fault_clear,
	.fault_test = fault_test,
	.attn_on = attn_on,
	.attn_off = attn_off,
};

static struct bt_mesh_health_srv health_srv = {
	.cb = &health_srv_cb,
};

static struct bt_mesh_health_cli health_cli = {
	.current_status = health_current_status,
};

BT_MESH_HEALTH_PUB_DEFINE(health_pub, CUR_FAULTS_MAX);

static struct bt_mesh_cfg_cli cfg_cli = {
};

static struct bt_mesh_cfg_srv cfg_srv = {
	.relay = BT_MESH_RELAY_ENABLED,
	.beacon = BT_MESH_BEACON_ENABLED,//BT_MESH_BEACON_DISABLED,
#if defined(CONFIG_BT_MESH_FRIEND)
	.frnd = BT_MESH_FRIEND_ENABLED,
#else
	.frnd = BT_MESH_FRIEND_NOT_SUPPORTED,
#endif
#if defined(CONFIG_BT_MESH_GATT_PROXY)
	.gatt_proxy = BT_MESH_GATT_PROXY_ENABLED,
#else
	.gatt_proxy = BT_MESH_GATT_PROXY_NOT_SUPPORTED,
#endif

	.default_ttl = 7,

	/* 6 transmissions with 20ms interval */
	.net_transmit = BT_MESH_TRANSMIT(5, 20),
	/* 3 transmissions with 20ms interval */
	.relay_retransmit = BT_MESH_TRANSMIT(2, 20),
};

#if defined(CONFIG_BT_MESH_MODEL)
#if defined(CONFIG_BT_MESH_MODEL_GEN_SRV)
BFL_BLE_MESH_MODEL_PUB_DEFINE(onoff_pub, 2 + 3, ROLE_NODE);
static bfl_ble_mesh_gen_onoff_srv_t onoff_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
};

#endif

#if defined(CONFIG_BT_MESH_MODEL_GEN_CLI)
BFL_BLE_MESH_MODEL_PUB_DEFINE(onoff_cli_pub, 2 + 1, ROLE_NODE);
static bfl_ble_mesh_client_t onoff_client;
#endif

#if defined(CONFIG_BT_MESH_MODEL_LIGHT_SRV)
BFL_BLE_MESH_MODEL_PUB_DEFINE(lightness_pub, 2 + 5, ROLE_NODE);
static bfl_ble_mesh_light_lightness_state_t lightness_state;
static bfl_ble_mesh_light_lightness_srv_t lightness_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .state = &lightness_state,
};
#endif

#if defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI)
BFL_BLE_MESH_MODEL_PUB_DEFINE(lightness_cli_pub, 2 + 1, ROLE_NODE);
static bfl_ble_mesh_client_t lightness_client;
#endif
	
#if defined(CONFIG_BT_MESH_MODEL_LIGHT_SRV)
BFL_BLE_MESH_MODEL_PUB_DEFINE(ctl_pub, 2 + 9, ROLE_NODE);
bfl_ble_mesh_light_ctl_state_t light_ctl_state = {
    .temperature_range_min = 0x320,
    .temperature_range_max = 0x4E20,
    .temperature = 0x320,
};
static bfl_ble_mesh_light_ctl_srv_t ctl_server = {
	.rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
	.rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
	.state = &light_ctl_state,
};
#endif

#if defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI)
BFL_BLE_MESH_MODEL_PUB_DEFINE(ctl_cli_pub, 2 + 1, ROLE_NODE);
static bfl_ble_mesh_client_t ctl_client;
#endif

#if defined(CONFIG_BT_MESH_MODEL_LIGHT_SRV)
BFL_BLE_MESH_MODEL_PUB_DEFINE(hsl_pub, 2 + 9, ROLE_NODE);
static bfl_ble_mesh_light_hsl_state_t hsl_state;
static bfl_ble_mesh_light_hsl_srv_t hsl_server = {
	.rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
	.rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
	.state = &hsl_state,
};
#endif

#if defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI)
BFL_BLE_MESH_MODEL_PUB_DEFINE(hsl_cli_pub, 2 + 1, ROLE_NODE);
static bfl_ble_mesh_client_t hsl_client;
#endif

#else
#if defined(CONFIG_BT_MESH_MODEL_GEN_SRV)
struct bt_mesh_gen_onoff_srv onoff_srv = {
};
#endif
#endif /* CONFIG_BT_MESH_MODEL */

#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)

#define MESH_MSG_LEN 20
BFL_BLE_MESH_MODEL_PUB_DEFINE(level_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_gen_level_srv_t level_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(onoff_pub_2, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_gen_onoff_srv_t onoff_server_2 = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(level_pub_2, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_gen_level_srv_t level_server_2 = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(def_trans_time_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_gen_def_trans_time_srv_t def_trans_time_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(power_onoff_pub, MESH_MSG_LEN, ROLE_NODE);
bfl_ble_mesh_gen_onpowerup_state_t onpowerup_state;
static bfl_ble_mesh_gen_power_onoff_srv_t power_onoff_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .state = &onpowerup_state,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(power_onoff_setup_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_gen_power_onoff_setup_srv_t power_onoff_setup_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .state = &onpowerup_state,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(power_level_pub, MESH_MSG_LEN, ROLE_NODE);
bfl_ble_mesh_gen_power_level_state_t power_level_state;
static bfl_ble_mesh_gen_power_level_srv_t power_level_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .state = &power_level_state,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(power_level_setup_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_gen_power_level_setup_srv_t power_level_setup_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .state = &power_level_state,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(battery_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_gen_battery_srv_t battery_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(location_pub, MESH_MSG_LEN, ROLE_NODE);
bfl_ble_mesh_gen_location_state_t location_state;
static bfl_ble_mesh_gen_location_srv_t location_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .state = &location_state,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(location_setup_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_gen_location_setup_srv_t location_setup_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .state = &location_state,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(user_prop_pub, MESH_MSG_LEN, ROLE_NODE);
u8_t net_buf_data_property_buf[16];
struct net_buf_simple property_buf = {
    .data   = net_buf_data_property_buf,
    .len    = 2,
    .size   = 16,
    .__buf  = net_buf_data_property_buf,
};
bfl_ble_mesh_generic_property_t property = {
    .id = 1,
    .user_access = USER_ACCESS_READ_WRITE,
    .admin_access = ADMIN_ACCESS_READ_WRITE,
    .manu_access = MANU_ACCESS_READ,
    .val = &property_buf,
    };
static bfl_ble_mesh_gen_user_prop_srv_t user_prop_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .property_count = 1,
    .properties = &property,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(admin_prop_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_gen_admin_prop_srv_t admin_prop_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .property_count = 1,
    .properties = &property,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(manu_prop_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_gen_manu_prop_srv_t manu_prop_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .property_count = 1,
    .properties = &property,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(client_prop_pub, MESH_MSG_LEN, ROLE_NODE);
uint16_t property_ids;
static bfl_ble_mesh_gen_client_prop_srv_t client_prop_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .id_count = 1,
    .property_ids =  &property_ids,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(lightness_setup_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_light_lightness_setup_srv_t lightness_setup_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .state = &lightness_state,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(light_ctl_setup_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_light_ctl_setup_srv_t light_ctl_setup_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .state = &light_ctl_state,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(light_ctl_temp_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_light_ctl_temp_srv_t light_ctl_temp_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .state = &light_ctl_state,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(light_hsl_setup_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_light_hsl_setup_srv_t light_hsl_setup_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .state = &hsl_state,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(light_hsl_hue_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_light_hsl_hue_srv_t light_hsl_hue_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .state = &hsl_state,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(light_hsl_sat_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_light_hsl_sat_srv_t light_hsl_sat_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .state = &hsl_state,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(light_xyl_setup_pub, MESH_MSG_LEN, ROLE_NODE);
bfl_ble_mesh_light_xyl_state_t light_xyl_state;
static bfl_ble_mesh_light_xyl_setup_srv_t light_xyl_setup_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .state = &light_xyl_state,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(light_xyl_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_light_xyl_srv_t light_xyl_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .state = &light_xyl_state,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(light_lc_pub, MESH_MSG_LEN, ROLE_NODE);
bfl_ble_mesh_light_control_t light_control;
static bfl_ble_mesh_light_lc_srv_t light_lc_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .lc = &light_control,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(light_lc_setup_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_light_lc_setup_srv_t light_lc_setup_server = {
    .rsp_ctrl.get_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = BFL_BLE_MESH_SERVER_AUTO_RSP,
    .lc = &light_control,
};
BFL_BLE_MESH_MODEL_PUB_DEFINE(level_cli_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_client_t level_client;
BFL_BLE_MESH_MODEL_PUB_DEFINE(def_trans_time_cli_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_client_t def_trans_time_client;
BFL_BLE_MESH_MODEL_PUB_DEFINE(power_onoff_cli_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_client_t power_onoff_client;
BFL_BLE_MESH_MODEL_PUB_DEFINE(power_level_cli_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_client_t power_level_client;
BFL_BLE_MESH_MODEL_PUB_DEFINE(battery_cli_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_client_t battery_client;
BFL_BLE_MESH_MODEL_PUB_DEFINE(location_cli_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_client_t location_client;
BFL_BLE_MESH_MODEL_PUB_DEFINE(property_cli_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_client_t property_client;
BFL_BLE_MESH_MODEL_PUB_DEFINE(light_xyl_cli_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_client_t light_xyl_client;
BFL_BLE_MESH_MODEL_PUB_DEFINE(light_lc_cli_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_client_t light_lc_client;
BFL_BLE_MESH_MODEL_PUB_DEFINE(light_ctl_cli_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_client_t light_ctl_client;
BFL_BLE_MESH_MODEL_PUB_DEFINE(light_hsl_cli_pub, MESH_MSG_LEN, ROLE_NODE);
static bfl_ble_mesh_client_t light_hsl_client;
#endif
static struct bt_mesh_model sig_models[] = {
	BT_MESH_MODEL_CFG_SRV(&cfg_srv),
	BT_MESH_MODEL_CFG_CLI(&cfg_cli),
	BT_MESH_MODEL_HEALTH_SRV(&health_srv, &health_pub),
	#ifdef CONFIG_MESH_IOPT_BV_02_C
	BT_MESH_MODEL_HEALTH_CLI(&health_cli, &health_pub),
	#else
	BT_MESH_MODEL_HEALTH_CLI(&health_cli, NULL),
	#endif

#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
#ifndef CONFIG_MESH_IOPT_BV_02_C
	BFL_BLE_MESH_MODEL_GEN_ONOFF_SRV(&onoff_pub, &onoff_server),
	BFL_BLE_MESH_MODEL_GEN_LEVEL_SRV(&level_pub, &level_server),
	BFL_BLE_MESH_MODEL_LIGHT_LIGHTNESS_SRV(&lightness_pub, &lightness_server),
	BFL_BLE_MESH_MODEL_GEN_DEF_TRANS_TIME_SRV(&def_trans_time_pub, &def_trans_time_server),
	BFL_BLE_MESH_MODEL_GEN_POWER_ONOFF_SRV(&power_onoff_pub, &power_onoff_server),
        BFL_BLE_MESH_MODEL_GEN_POWER_ONOFF_SETUP_SRV(&power_onoff_setup_pub, &power_onoff_setup_server),
        BFL_BLE_MESH_MODEL_GEN_POWER_LEVEL_SRV(&power_level_pub, &power_level_server),
        BFL_BLE_MESH_MODEL_GEN_POWER_LEVEL_SETUP_SRV(&power_level_setup_pub, &power_level_setup_server),
        BFL_BLE_MESH_MODEL_GEN_BATTERY_SRV(&battery_pub, &battery_server),
        BFL_BLE_MESH_MODEL_GEN_LOCATION_SRV(&location_pub, &location_server),
        BFL_BLE_MESH_MODEL_GEN_LOCATION_SETUP_SRV(&location_setup_pub, &location_setup_server),
        BFL_BLE_MESH_MODEL_GEN_USER_PROP_SRV(&user_prop_pub, &user_prop_server),
        BFL_BLE_MESH_MODEL_GEN_ADMIN_PROP_SRV(&admin_prop_pub, &admin_prop_server),
        BFL_BLE_MESH_MODEL_GEN_MANUFACTURER_PROP_SRV(&manu_prop_pub, &manu_prop_server),
        BFL_BLE_MESH_MODEL_GEN_CLIENT_PROP_SRV(&client_prop_pub, &client_prop_server),

	BFL_BLE_MESH_MODEL_GEN_ONOFF_CLI(&onoff_cli_pub, &onoff_client),
	BFL_BLE_MESH_MODEL_GEN_LEVEL_CLI(&level_cli_pub, &level_client),
	BFL_BLE_MESH_MODEL_GEN_DEF_TRANS_TIME_CLI(&def_trans_time_cli_pub, &def_trans_time_client),
        BFL_BLE_MESH_MODEL_GEN_POWER_ONOFF_CLI(&power_onoff_cli_pub, &power_onoff_client),
        BFL_BLE_MESH_MODEL_GEN_POWER_LEVEL_CLI(&power_level_cli_pub, &power_level_client),
        BFL_BLE_MESH_MODEL_GEN_BATTERY_CLI(&battery_cli_pub, &battery_client),
        BFL_BLE_MESH_MODEL_GEN_LOCATION_CLI(&location_cli_pub, &location_client),
        BFL_BLE_MESH_MODEL_GEN_PROPERTY_CLI(&property_cli_pub, &property_client),

        BFL_BLE_MESH_MODEL_LIGHT_LIGHTNESS_SRV(&lightness_pub, &lightness_server),
        BFL_BLE_MESH_MODEL_LIGHT_LIGHTNESS_SETUP_SRV(&lightness_setup_pub, &lightness_setup_server),
        BFL_BLE_MESH_MODEL_LIGHT_CTL_SRV(&ctl_pub, &ctl_server),
        BFL_BLE_MESH_MODEL_LIGHT_CTL_SETUP_SRV(&light_ctl_setup_pub, &light_ctl_setup_server),
        BFL_BLE_MESH_MODEL_LIGHT_CTL_TEMP_SRV(&light_ctl_temp_pub, &light_ctl_temp_server),
        BFL_BLE_MESH_MODEL_LIGHT_HSL_SRV(&hsl_pub, &hsl_server),
        BFL_BLE_MESH_MODEL_LIGHT_HSL_SETUP_SRV(&light_hsl_setup_pub, &light_hsl_setup_server),
        BFL_BLE_MESH_MODEL_LIGHT_HSL_HUE_SRV(&light_hsl_hue_pub, &light_hsl_hue_server),
        BFL_BLE_MESH_MODEL_LIGHT_HSL_SAT_SRV(&light_hsl_sat_pub, &light_hsl_sat_server),
        BFL_BLE_MESH_MODEL_LIGHT_XYL_SRV(&light_xyl_pub, &light_xyl_server),
        BFL_BLE_MESH_MODEL_LIGHT_XYL_SETUP_SRV(&light_xyl_setup_pub, &light_xyl_setup_server),
    
   
	BFL_BLE_MESH_MODEL_LIGHT_LIGHTNESS_CLI(&lightness_cli_pub, &lightness_client),
	BFL_BLE_MESH_MODEL_LIGHT_CTL_CLI(&light_ctl_cli_pub, &light_ctl_client),
	BFL_BLE_MESH_MODEL_LIGHT_HSL_CLI(&light_hsl_cli_pub, &light_hsl_client),
	BFL_BLE_MESH_MODEL_LIGHT_XYL_CLI(&light_xyl_cli_pub, &light_xyl_client),
#endif
#else

#if !defined(CONFIG_BT_MESH_MODEL)
	#if defined(CONFIG_BT_MESH_MODEL_GEN_SRV)
	BT_MESH_MODEL_GEN_ONOFF(&onoff_srv),
	#endif
#else
	#if defined(CONFIG_BT_MESH_MODEL_GEN_SRV)
	BFL_BLE_MESH_MODEL_GEN_ONOFF_SRV(&onoff_pub, &onoff_server),
	#endif
	#if defined(CONFIG_BT_MESH_MODEL_GEN_CLI)
	BFL_BLE_MESH_MODEL_GEN_ONOFF_CLI(&onoff_cli_pub, &onoff_client),
	#endif
    #if defined(CONFIG_BT_MESH_MODEL_LIGHT_SRV)
	BFL_BLE_MESH_MODEL_LIGHT_LIGHTNESS_SRV(&lightness_pub, &lightness_server),
    #endif
    #if defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI)
	BFL_BLE_MESH_MODEL_LIGHT_LIGHTNESS_CLI(&lightness_cli_pub, &lightness_client),
    #endif
    #if defined(CONFIG_BT_MESH_MODEL_LIGHT_SRV)
	BFL_BLE_MESH_MODEL_LIGHT_CTL_SRV(&ctl_pub, &ctl_server),
    #endif
    #if defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI)
	BFL_BLE_MESH_MODEL_LIGHT_CTL_CLI(&ctl_cli_pub, &ctl_client),
    #endif
    #if defined(CONFIG_BT_MESH_MODEL_LIGHT_SRV)
	BFL_BLE_MESH_MODEL_LIGHT_HSL_SRV(&hsl_pub, &hsl_server),
    #endif
    #if defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI)
	BFL_BLE_MESH_MODEL_LIGHT_HSL_CLI(&hsl_cli_pub, &hsl_client),
    #endif
#endif /* CONFIG_BT_MESH_MODEL */

#endif
};

struct vendor_data_t{
	uint8_t data[BT_MESH_TX_VND_SDU_MAX_SHORT];
};
static struct vendor_data_t vendor_data;

#if defined(CONFIG_BT_MESH_MODEL_VENDOR_CLI)
static struct vendor_data_t vendor_data_cli;
#endif /*CONFIG_BT_MESH_MODEL_VENDOR_CLI*/

static void vendor_data_set(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    NET_BUF_SIMPLE_DEFINE(msg, BT_MESH_TX_SDU_MAX);

    vOutputString("data[%s]\n", bt_hex(buf->data, buf->len));

    if (buf == NULL) {
        BT_ERR("%s, Invalid model user_data", __func__);
        return;
    }
    bt_mesh_model_msg_init(&msg, BLE_MESH_MODEL_VND_OP_DATA_STATUS);
	net_buf_simple_add_mem(&msg, buf->data, buf->len);

    bt_mesh_model_send(model, ctx, &msg, NULL, NULL);
}

#if defined(CONFIG_BT_MESH_MODEL_VENDOR_CLI)
static void vendor_data_status(struct bt_mesh_model *model,
                              struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    vOutputString("Vendor status[%s]\n", bt_hex(buf->data, buf->len));
}
#endif /*CONFIG_BT_MESH_MODEL_VENDOR_CLI*/

/* Mapping of message handlers for Generic Power OnOff Server (0x1006) */
const struct bt_mesh_model_op vendor_data_op[] = {
    { BLE_MESH_MODEL_VND_OP_DATA_SET, 1, vendor_data_set },
    { BLE_MESH_MODEL_VND_OP_DATA_SET_UNACK, 1, vendor_data_set },
    BT_MESH_MODEL_OP_END,
};
#if defined(CONFIG_BT_MESH_MODEL_VENDOR_CLI)
const struct bt_mesh_model_op vendor_data_op_cli[] = {
    { BLE_MESH_MODEL_VND_OP_DATA_STATUS, 1, vendor_data_status },
    BT_MESH_MODEL_OP_END,
};
#endif /*CONFIG_BT_MESH_MODEL_VENDOR_CLI*/


static struct bt_mesh_model vendor_models[] = {
	BT_MESH_MODEL_VND(BL_COMP_ID, BT_MESH_VND_MODEL_ID_DATA_SRV, vendor_data_op, NULL, &vendor_data),
#if defined(CONFIG_BT_MESH_MODEL_VENDOR_CLI)
	BT_MESH_MODEL_VND(BL_COMP_ID, BT_MESH_VND_MODEL_ID_DATA_CLI, vendor_data_op_cli, NULL, &vendor_data_cli),
#endif /*CONFIG_BT_MESH_MODEL_VENDOR_CLI*/
#if defined(CONFIG_BT_MESH_SYNC)
	BFL_BLE_MESH_MODEL_VND_SYNC_SRV(),
#endif
};
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
static struct bt_mesh_model second_models[] = {
	BFL_BLE_MESH_MODEL_GEN_ONOFF_SRV(&onoff_pub_2, &onoff_server_2),
	BFL_BLE_MESH_MODEL_GEN_LEVEL_SRV(&level_pub_2, &level_server_2),
	BFL_BLE_MESH_MODEL_LIGHT_CTL_TEMP_SRV(&light_ctl_temp_pub, &light_ctl_temp_server),
	/* Need this mode to pass MMDL/SR/MLTEL/BV-01-C*/
	//BFL_BLE_MESH_MODEL_LIGHT_LC_SRV(&light_lc_pub, &light_lc_server),
	//BFL_BLE_MESH_MODEL_LIGHT_LC_SETUP_SRV(&light_lc_setup_pub, &light_lc_setup_server),
};
static struct bt_mesh_model second_vnd_models[0] = {
};
#endif

static struct bt_mesh_elem elements[] = {
	BT_MESH_ELEM(0, sig_models, vendor_models),
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
	BT_MESH_ELEM(1, second_models, second_vnd_models),
#endif
};

static const struct bt_mesh_comp comp = {
	.cid = BL_COMP_ID,
	.elem = elements,
	.elem_count = ARRAY_SIZE(elements),
};
#if defined(CONFIG_SHELL)
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_init, blemesh_init, blemesh Initialize Parameter:[Null]);
#if defined(CONFIG_BT_MESH_GATT_PROXY)
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_proxy_identity, blemesh_proxy_identity, blemesh proxy identity);
#endif
#if defined(CONFIG_BT_MESH_PROVISIONER)
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_pvnr_init, blemesh_pvnr_init, blemesh_pvnr Initialize Parameter:[Null]);
#endif
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_addr_get, blemesh_addr_get, blemesh address get Parameter:[Null]);
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_set_dev_uuid, blemesh_set_dev_uuid, blemesh input number in provisionging procedure Parameter:[Size:16 Octets]);
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_pb, blemesh_pb, blemesh enable or disable provisioning Parameter:[bear: 1:adv bear; 2:gatt bear][enable: 0:disable provisioning; 1:enable provisioning]);
#if defined(CONFIG_BT_MESH_PB_ADV)
#if defined(CONFIG_BT_MESH_PROVISIONER)
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_provision_adv, blemesh_provision_adv, blemesh enable or disable provisioning Parameter:[bear: 1:adv bear; 2:gatt bear] [enable: 0:disable provisioning; 1:enable provisioning]);
#endif
#endif
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_reset, blemesh_reset, blemesh Reset the state of the local mesh node Parameter:[Null]);
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_net_send, blemesh_net_send, blemesh Send a network packet Parameter:[TTL CTL SRC DST]); 
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_seg_send, blemesh_seg_send, blemesh Send a segmented message Parameter:[SRC DST]);
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_rpl_clr, blemesh_rpl_clr, blemesh Clear replay protection list Parameter:[Null]);
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_ivu_test, blemesh_ivu_test, blemesh Enable or disable iv update procedure Parameter:[enable: 0:disable; 1:enable by sending secure network beacons]);
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_iv_update, blemesh_iv_update, blemesh Enable or disable iv update procedure Parameter:[enable: 0:disable; 1:enable by sending secure network beacons]) ;
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_fault_set, blemesh_fault_set, blemesh Set current fault or registered fault values Parameter:[type: 0:current fault; 1:registered fault][fault: fault array in hex format]);
#if defined(CONFIG_BT_MESH_LOW_POWER)
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_lpn_set, blemesh_lpn_set, blemesh Enable or disable low power node Parameter:[enable: 0:disable lpn; 1:enable lpn]);
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_lpn_poll, blemesh_lpn_poll, blemesh lpn send poll);
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_lpn_clear_friend_send, blemesh_lpn_clear_friend_send, blemesh lpn clear friend send);
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_lpn_friend_req, blemesh_lpn_friend_req, blemesh lpn friend req);
	SHELL_CMD_EXPORT_ALIAS(blemeshcli_lpn_group_add, blemesh_lpn_group_add, blemesh lpn group add);
	SHELL_CMD_EXPORT_ALIAS(blemeshcli_lpn_group_del, blemesh_lpn_group_del, blemesh lpn group del);
#endif
#endif
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_input_num, blemesh_input_num, blemesh input number in provisionging procedure Parameter:[Max Size:16 Octets: e.g.112233445566778899AA]);
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_input_str, blemesh_input_str, blemesh input Alphanumeric in provisionging procedure Parameter:[Max Size:16 Characters: e.g.123ABC]);
#if defined(CONFIG_BT_MESH_MODEL)
#if defined(CONFIG_BT_MESH_MODEL_GEN_CLI)
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_gen_oo_cli, blemesh_gen_oo_cli, blemesh_gen_oo_cli Parameter:[cmd op app_idx opcode msg_role addr net_idx op_en_t onoff tid trans_time delay]);
#endif
#if defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI)
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_light_lgn_cli, blemesh_light_lgn_cli, blemesh_light_lgn_cli Parameter:[cmd op app_idx opcode msg_role addr net_idx op_en_t lightness tid trans_time delay] );
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_light_ctl_cli, blemesh_light_ctl_cli, blemesh_light_ctl_cli Parameter:[cmd op app_idx opcode msg_role addr net_idx op_en_t ctl_lightness ctl_temperatrue ctl_delta_uv tid trans_time delay]);
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_light_hsl_cli, blemesh_light_hsl_cli, blemesh_light_hsl_cli Parameter:[cmd op app_idx opcode msg_role addr net_idx op_en lightness hue saturation tid trans_time delay]);
#endif
#if defined(CONFIG_BT_MESH_MODEL_VENDOR_CLI)
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_vendor_cli, blemesh_vendor_cli, blemesh_vendor_cli Parameter:[cmd op app_idx opcode msg_role addr net_idx]);
#endif
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_pts_models_client_cli, blemesh_pts_models_client_cli, blemeshcli_pts_models_client_cli Parameter:[app_idx opcode addr net_idx models_id payload]);
	SHELL_CMD_EXPORT_ALIAS(blemeshcli_models_cli, blemesh_models_cli, blemesh_models_cli Parameter:[opcode payload]);
#endif
#endif
#if defined(CONFIG_BT_MESH_CDB)
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_cdb_create, blemesh_cdb_create, );
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_cdb_clear, blemesh_cdb_clear, );
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_cdb_show, blemesh_cdb_show, );
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_cdb_node_add, blemesh_cdb_node_add, );
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_cdb_node_del, blemesh_cdb_node_del, );
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_cdb_subnet_add, blemesh_cdb_subnet_add, );
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_cdb_subnet_del, blemesh_cdb_subnet_del, );
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_cdb_app_key_add, blemesh_cdb_app_key_add, );
    SHELL_CMD_EXPORT_ALIAS(blemeshcli_cdb_app_key_del, blemesh_cdb_app_key_del, );
#endif
#if defined(CONFIG_BT_MESH_PROVISIONER)
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_beacon_listen, blemesh_beacon_listen, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_provision, blemesh_provision, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_node_cfg, blemesh_node_cfg, );
#endif
#if defined(CONFIG_BT_MESH_PROVISIONER)
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_get_comp, blemesh_get_comp, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_set_dst, blemesh_set_dst, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_net_key_add, blemesh_net_key_add, );
     /* Added by bouffalo */
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_net_key_update, blemesh_net_key_update, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_net_key_get, blemesh_net_key_get, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_net_key_del, blemesh_net_key_del, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_app_key_add, blemesh_app_key_add, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_app_key_update, blemesh_app_key_update, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_app_key_get, blemesh_app_key_get, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_app_key_del, blemesh_app_key_del, ); 
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_kr_update, blemesh_kr_update, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_mod_app_bind, blemesh_mod_app_bind, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_mod_app_unbind, blemesh_mod_app_unbind, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_mod_app_get, blemesh_mod_app_get, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_mod_sub_add, blemesh_mod_sub_add, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_mod_sub_ow, blemesh_mod_sub_ow, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_mod_sub_del, blemesh_mod_sub_del, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_mod_sub_del_all, blemesh_mod_sub_del_all, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_mod_sub_add_va, blemesh_mod_sub_add_va, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_mod_sub_ow_va, blemesh_mod_sub_ow_va, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_mod_sub_del_va, blemesh_mod_sub_del_va, );     
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_mod_sub_get, blemesh_mod_sub_get, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_mod_pub, blemesh_mod_pub, );  
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_mod_pub_va, blemesh_mod_pub_va, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_hb_sub, blemesh_hb_sub, );     
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_hb_pub, blemesh_hb_pub, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_krp_get, blemesh_krp_get, );  
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_krp_set, blemesh_krp_set, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_cfg_bcn_get, blemesh_cfg_bcn_get, );     
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_cfg_bcn_set, blemesh_cfg_bcn_set, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_cfg_dttl_get, blemesh_cfg_dttl_get, );  
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_cfg_dttl_set, blemesh_cfg_dttl_set, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_cfg_gpxy_get, blemesh_cfg_gpxy_get, );     
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_cfg_gpxy_set, blemesh_cfg_gpxy_set, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_friend, blemesh_friend, );  
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_relay, blemesh_relay, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_node_identify, blemesh_node_identify, );     
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_node_reset, blemesh_node_reset, Parameter:[net_idx][dst addr]);
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_network_trans, blemesh_network_trans, ); 
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_lpn_timeout_get, blemesh_lpn_timeout_get, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_clhm_fault, blemesh_clhm_fault, ); 
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_clhm_period, blemesh_clhm_period, );
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_clhm_ats, blemesh_clhm_ats, ); 
#endif
#if defined(CFG_NODE_SEND_CFGCLI_MSG) && defined(CONFIG_BT_MESH_CDB)
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_pvnr_devkey, blemesh_pvnr_devkey);
#endif
#if defined(CONFIG_BT_MESH_SYNC)
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_sync_start, blemesh_sync_start, Parameter:[NULL]);
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_sync_stop, blemesh_sync_stop, Parameter:[NULL]);
#endif
#if defined(CONFIG_BT_MESH_TEST)
     SHELL_CMD_EXPORT_ALIAS(blemeshcli_nodelist_op, blemesh_nodelist_op, );
#endif
	 SHELL_CMD_EXPORT_ALIAS(blemeshcli_read_str, blemesh_read_str, );
#else
#if defined(BL602) || defined(BL702) || defined(BL606P) || defined(BL702L)
const struct cli_command btMeshCmdSet[] STATIC_CLI_CMD_ATTRIBUTE = {
#else
const struct cli_command btMeshCmdSet[] = {
#endif
    {"blemesh_init", "blemesh Initialize\r\n Parameter[Null]", blemeshcli_init},
#if defined(CONFIG_BT_MESH_PROVISIONER)
    {"blemesh_pvnr_init", "blemesh_pvnr_init:[Initialize]\r\n Parameter[Null]", blemeshcli_pvnr_init},
#endif
	{"blemesh_addr_get", "blemesh_addr_get: Parameter[Null]", blemeshcli_addr_get},
	{"blemesh_set_dev_uuid", "blemesh_input_num:[input number in provisionging procedure]\r\n\
     [Size:16 Octets, e.g.112233445566778899AA]", blemeshcli_set_dev_uuid},
    {"blemesh_pb", "blemesh_pb:[Enable or disable provisioning]\r\n\
     [bear, 1:adv bear, 2:gatt bear]\r\n\
     [enable, 0:disable provisioning, 1:enable provisioning]", blemeshcli_pb},
#if defined(CONFIG_BT_MESH_PB_ADV)
#if defined(CONFIG_BT_MESH_PROVISIONER)
	{"blemesh_provision_adv", "blemesh_pb:[Enable or disable provisioning]\r\n\
		 [bear, 1:adv bear, 2:gatt bear]\r\n\
		 [enable, 0:disable provisioning, 1:enable provisioning]", blemeshcli_provision_adv},
#endif /* CONFIG_BT_MESH_PROVISIONER */
#endif /* CONFIG_BT_MESH_PB_ADV */
    
    {"blemesh_reset", "blemesh_reset:[Reset the state of the local mesh node]\r\n Parameter[Null]", blemeshcli_reset},
    {"blemesh_net_send", "blemesh_net_send:[Send a network packet]\r\n Parameter[TTL CTL SRC DST]", blemeshcli_net_send},
    {"blemesh_seg_send", "blemesh_seg_send:[Send a segmented message]\r\n Parameter[SRC DST]", blemeshcli_seg_send},
    {"blemesh_rpl_clr", "blemesh_rpl_clr:[Clear replay protection list]\r\n Parameter[Null]", blemeshcli_rpl_clr},
    {"blemesh_ivu_test", "blemesh_ivu_test:[Enable or disable iv update test mode]\r\n\
     [enable, 0:disable, 1:enable]", blemeshcli_ivu_test},
    {"blemesh_iv_update", "blemesh_iv_update:[Enable or disable iv update procedure]\r\n\
     [enable, 0:disable, 1:enable by sending secure network beacons]", blemeshcli_iv_update},
    {"blemesh_fault_set", "blemesh_fault_set:[Set current fault or registered fault values]\r\n\
     [type, 0:current fault, 1:registered fault]\r\n\
     [fault, fault array in hex format]", blemeshcli_fault_set},
    #if defined(CONFIG_BT_MESH_LOW_POWER)
    {"blemesh_lpn_set", "blemesh_lpn_set:[Enable or disable low power node]\r\n\
     [enable, 0:disable lpn, 1:enable lpn]", blemeshcli_lpn_set},
    #endif
    {"blemesh_input_num", "blemesh_input_num:[input number in provisionging procedure]\r\n\
     [Max Size:16 Octets, e.g.112233445566778899AA]", blemeshcli_input_num},
     
    {"blemesh_input_str", "blemesh_input_str:[input Alphanumeric in provisionging procedure]\r\n\
     [Max Size:16 Characters, e.g.123ABC]", blemeshcli_input_str},
#if defined(CONFIG_BT_MESH_MODEL)
#if defined(CONFIG_BT_MESH_MODEL_GEN_CLI)
	{"blemesh_gen_oo_cli", "blemesh_gen_oo_cli:[cmd op app_idx opcode msg_role addr net_idx op_en_t onoff tid trans_time delay]\r\n\
     []", blemeshcli_gen_oo_cli},
#endif
#if defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI)
     {"blemesh_light_lgn_cli", "blemesh_light_lgn_cli:[cmd op app_idx opcode msg_role addr net_idx op_en_t lightness tid trans_time delay]", blemeshcli_light_lgn_cli},
     {"blemesh_light_ctl_cli", "blemesh_light_ctl_cli:[cmd op app_idx opcode msg_role addr net_idx op_en_t ctl_lightness ctl_temperatrue ctl_delta_uv tid trans_time delay]", blemeshcli_light_ctl_cli},
     {"blemesh_light_hsl_cli", "blemesh_light_hsl_cli:[cmd op app_idx opcode msg_role addr net_idx op_en lightness hue saturation tid trans_time delay]", blemeshcli_light_hsl_cli},
#endif
#if defined(CONFIG_BT_MESH_MODEL_VENDOR_CLI)
	{"blemesh_vendor_cli", "blemesh_vendor_cli:[cmd op app_idx opcode msg_role addr net_idx]", blemeshcli_vendor_cli},
#endif
#endif /* CONFIG_BT_MESH_MODEL */
#if defined(CONFIG_BT_MESH_CDB)
	{"blemesh_cdb_create", "blemesh_cdb_create:[Create Configuration Database. Input Parameter :[None will generate random netkey, else 16 Octets network key, e.g.112233445566778899AA] ", blemeshcli_cdb_create},
	{"blemesh_cdb_clear", "blemesh_cdb_clear:[Clean Configuration Database.]", blemeshcli_cdb_clear},
	{"blemesh_cdb_show", "blemesh_cdb_show:[Show Configuration Database.]", blemeshcli_cdb_show},
	{"blemesh_cdb_node_add", "Add a node to the Configuration Database (CDB).Parameter: [ Node's UUID (hexadecimal string)\r\n\
    [Node's address, decimal string]\r\n\
	[Number of elements, decimal string]\r\n\
	[Node's network index: decimal string]\r\n\
	[(optional)Node's device key: hexadecimal string]\r\n", blemeshcli_cdb_node_add},
	{"blemesh_cdb_node_del", "blemesh_cdb_node_del:[Delete a node from the Configuration Database (CDB). Parameter :[ Node's address, decimal string]", blemeshcli_cdb_node_del},
	{"blemesh_cdb_subnet_add","blemesh_cdb_subnet_add: Adds subnet to the Configuration Database (CDB). Parameter :[ Network index of the subnet(decimal string)\r\n\
	[Network key for the subnet (hexadecimal string)", blemeshcli_cdb_subnet_add},
	{"blemesh_cdb_subnet_del", "blemesh_cdb_subnet_del: Delete subnet form the Configuration Database (CDB).Parameter :[ Network index of the subnet(decimal string)\r\n\
	[Network key for the subnet (hexadecimal string)", blemeshcli_cdb_subnet_del},
	{"blemesh_cdb_app_key_add", " Add aApplication Key to the Configuration Database (CDB).Parameter :[Network index of the subnet(decimal string)\r\n\
	[Application index of the key (decimal string)\r\n\
    [(optional): Application key (hexadecimal string)", blemeshcli_cdb_app_key_add},
	{"blemesh_cdb_app_key_del", "blemesh_cdb_app_key_del:[Delete an Application Key from the Configuration Database (CDB).Parameter :[Application index of the key (decimal string)]", blemeshcli_cdb_app_key_del},
#endif /* CONFIG_BT_MESH_CDB */
#if defined(CONFIG_BT_MESH_PROVISIONER)
	{"blemesh_beacon_listen", "blemesh_beacon_listen:[Enables or disables listening for unprovisioned beacon.Parameter :[enable (1) or disable (0)]", blemeshcli_beacon_listen},
	{"blemesh_provision", "blemesh_beacon_listen: provisioning a device is added to the network. Parameter:[network index in decimal format]\r\n\
	[device address (addr) in decimal format]\r\n\
    [(optional) IV Index in decimal format. If not provided, the default value is set to 0]", blemeshcli_provision},
	{"blemesh_node_cfg", " blemesh_node_cfg: checks the configuration status of all nodes in the Configuration Database (CDB).", blemeshcli_node_cfg},
#endif /* CONFIG_BT_MESH_PROVISIONER */
#if defined(CONFIG_BT_MESH_PROVISIONER)
	{"blemesh_get_comp", "blemesh_get_comp: Get composition Data of a specific node. Parameter:[net_idx: Network index(hexadecimal format)]\r\n\
    [Destination address(hexadecimal)]\r\n\
	[Page number (hexadecimal)]", blemeshcli_get_comp},
	{"blemesh_set_dst", "blemesh_set_dst: Sets the destination address. Parameter: [Net destination address (hexadecimal)]", blemeshcli_set_dst},
	{"blemesh_net_key_add", "blemesh_net_key_add: Adding a NetKey to the Bluetooth Mesh network. Parameter: [net_idx:(hexadecimal format)]\r\n\
	[Destination address (hexadecimal format)\r\n\
	[NetKey Index (hexadecimal format)\r\n\
	[NetKey value in hexadecimal format. Either provided as an argument or generated based on default values.]", blemeshcli_net_key_add},
	/* Added by bouffalo */
	{"blemesh_net_key_update", "blemesh_net_key_update: Update of a network key (NetKey) with a specified NetKey Index. Parameter:[net_idx:(hexadecimal formats)]\r\n\
    [Destination address(hexadecimal formats)]\r\n\
	[key_net_idx: (hexadecimal formats)]\r\n\
	[key_val in (hexadecimal formats) New NetKey value in hexadecimal format. Either provided as an argument or generated based on default values.", blemeshcli_net_key_update},
	{"blemesh_net_key_get", "blemesh_net_key_get: Get NetKeys known by a specified node. Parameter: [net_idx:(hexadecimal formats)]\r\n\
    [Destination address(hexadecimal formats)]", blemeshcli_net_key_get},
	{"blemesh_net_key_del", "blemesh_net_key_del:  Delete NetKey a specified destination address. Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[key_net_idx: (hexadecimal formats)]", blemeshcli_net_key_del},
	{"blemesh_app_key_add", "blemesh_app_key_add: Add AppKey to a specified destination address. Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[key_net_idx:(hexadecimal formats)]\r\n\
	[key_app_idx:(hexadecimal formats)]\r\n\
	[key_val (optional): Hexadecimal string representing the AppKey value]", blemeshcli_app_key_add},
	/* Added by bouffalo */
	{"blemesh_app_key_update", "blemesh_app_key_update: Update AppKey to a specified destination address. Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[key_net_idx:(hexadecimal formats)]\r\n\
	[key_app_idx:(hexadecimal formats)]\r\n\
	[key_val (optional): Hexadecimal string representing the AppKey value]", blemeshcli_app_key_update},
	{"blemesh_app_key_get", "blemesh_app_key_get: Get AppKey known by a specified node. Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[key_net_idx:(hexadecimal formats)]", blemeshcli_app_key_get},
	{"blemesh_app_key_del", "blemesh_app_key_del: Delete AppKey known by a specified node. Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[key_net_idx:(hexadecimal formats)]", blemeshcli_app_key_del},
	/* Added by bouffalo */
	{"blemesh_kr_update", "blemesh_kr_update: updating the Key Refresh phase of a specific subnet. Parameter: [subnetidx(hexadecimal formats)Index of the subnet]\r\n\
    [kr: New Key Refresh value(hexadecimal formats)]", blemeshcli_kr_update},
	{"blemesh_mod_app_bind", " blemesh_mod_app_bind: binding an Application Key (AppKey) to a Model within a specific element of a node. Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[elem_addr:(hexadecimal formats)]\r\n\
	[mod_app_idx: (hexadecimal formats)]\r\n\
	[mod_id: (hexadecimal formats)]\r\n\
	[cid (optional): Company ID(hexadecimal formats)] ", blemeshcli_mod_app_bind},
	{"blemesh_mod_app_unbind", "blemesh_mod_app_unbind: unbinding an Application Key (AppKey) from a Model within a specific element of a node. Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[elem_addr:(hexadecimal formats)]\r\n\
	[mod_app_idx: (hexadecimal formats)]\r\n\
	[mod_id: (hexadecimal formats)]\r\n\
	[cid (optional): Company ID(hexadecimal formats)] ", blemeshcli_mod_app_unbind},
	{"blemesh_mod_app_get", "blemesh_mod_app_get: Retrieving the Application Keys (AppKeys) bound to a specific Model within a particular element of a node.Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[elem_addr:(hexadecimal formats)]\r\n\
	[mod_id: (hexadecimal formats)]\r\n\
	[cid (optional): Company ID(hexadecimal formats)]", blemeshcli_mod_app_get},
	{"blemesh_mod_sub_add", "blemesh_mod_sub_add : Add a group address to a SIG model's subscription list. Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[elem_addr:(hexadecimal formats)]\r\n\
	[sub_addr: (hexadecimal formats)]\r\n\
	[mod_id: (hexadecimal formats)]\r\n\
	[cid (optional): Company ID(hexadecimal formats)]", blemeshcli_mod_sub_add},
	{"blemesh_mod_sub_ow", "blemesh_mod_sub_ow: Deletes all subscriptions in the model's subscription list, and adds a single group address instead. Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[elem_addr:(hexadecimal formats)]\r\n\
	[sub_addr: (hexadecimal formats)]\r\n\
	[mod_id: (hexadecimal formats)]\r\n\
	[cid (optional): Company ID(hexadecimal formats)]", blemeshcli_mod_sub_ow},
	{"blemesh_mod_sub_del", "blemeshcli_mod_sub_del: Delete all group addresses in a SIG model's subscription list.Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[elem_addr:(hexadecimal formats)]\r\n\
	[sub_addr: (hexadecimal formats)]\r\n\
	[mod_id: (hexadecimal formats)]\r\n\
	[cid (optional): Company ID(hexadecimal formats)]", blemeshcli_mod_sub_del},
	{"blemesh_mod_sub_del_all", "blemesh_mod_sub_del_all: Delete all group addresses in a SIG model's subscription list. Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[elem_addr:(hexadecimal formats)]\r\n\
	[mod_id: (hexadecimal formats)]\r\n\
	[cid (optional): Company ID(hexadecimal formats)]", blemeshcli_mod_sub_del_all},
	{"blemesh_mod_sub_add_va", "blemesh_mod_sub_add_va: Add a virtual address to a SIG model's subscription list. Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[elem_addr:(hexadecimal formats)]\r\n\
	[mod_id: (hexadecimal formats)]\r\n\
	[label: (hexadecimal formats)]\r\n\
	[cid (optional): Company ID(hexadecimal formats)]", blemeshcli_mod_sub_add_va},
	{"blemesh_mod_sub_ow_va", "blemesh_mod_sub_ow_va: Deletes all subscriptions in the model's subscription list, and adds a single group address instead.Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[elem_addr:(hexadecimal formats)]\r\n\
	[mod_id: (hexadecimal formats)]\r\n\
	[label: (hexadecimal formats)]\r\n\
	[cid (optional): Company ID(hexadecimal formats)]", blemeshcli_mod_sub_ow_va},
	{"blemesh_mod_sub_del_va", "blemesh_mod_sub_del_va: Delete a virtual address in a SIG model's subscription list. Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[elem_addr:(hexadecimal formats)]\r\n\
	[mod_id: (hexadecimal formats)]\r\n\
	[label: (hexadecimal formats)]\r\n\
	[cid (optional): Company ID(hexadecimal formats)]", blemeshcli_mod_sub_del_va},
	{"blemesh_mod_sub_get", "blemesh_mod_sub_get: Get the subscription list of a SIG model on the target node. Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[elem_addr:(hexadecimal formats)]\r\n\
	[mod_id: (hexadecimal formats)]\r\n\
	[cid (optional): Company ID(hexadecimal formats)]", blemeshcli_mod_sub_get},
	{"blemesh_mod_pub", " blemesh_mod_pub: Model Publication Set or Get. Set Parameter:[Destination address(hexadecimal formats)]\r\n\
	[mod_id: (hexadecimal formats)]\r\n\
	[cid (optional): Company ID(hexadecimal formats)]\r\n\
	[net_idx:(hexadecimal formats)]\r\n\
	[pub.addr:(hexadecimal formats)]\r\n\
	[pub.app_idx: (hexadecimal formats)]\r\n\
	[pub.cred_flag: (hexadecimal formats)]\r\n\
	[pub.ttl: (hexadecimal formats)]\r\n\
	[pub.period: (hexadecimal formats)]\r\n\
	[pub.count: (hexadecimal formats)]\r\n\
	Get Parameter: [Destination address(hexadecimal formats)]\r\n\
	[mod_id: (hexadecimal formats)]\r\n\
	[cid (optional): Company ID(hexadecimal formats)]", blemeshcli_mod_pub},
	{"blemesh_mod_pub_va", "blemeshcli_mod_pub_va: Model Publication Set or Get to virtual address.Set Parameter:[Destination address(hexadecimal formats)]\r\n\
	[mod_id: (hexadecimal formats)]\r\n\
	[cid (optional): Company ID(hexadecimal formats)]\r\n\
	[net_idx:(hexadecimal formats)]\r\n\
	[pub.addr:(hexadecimal formats)]\r\n\
	[pub.app_idx: (hexadecimal formats)]\r\n\
	[pub.cred_flag: (hexadecimal formats)]\r\n\
	[pub.ttl: (hexadecimal formats)]\r\n\
	[pub.period: (hexadecimal formats)]\r\n\
	[pub.count: (hexadecimal formats)]\r\n\
	Get Parameter: [Destination address(hexadecimal formats)]\r\n\
	[mod_id: (hexadecimal formats)]\r\n\
	[cid (optional): Company ID(hexadecimal formats)]", blemeshcli_mod_pub_va},
	{"blemesh_hb_sub", " blemesh_hb_sub: Heartbeat Subscription Get or Set. Set Parameter:[net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[sub.src:(hexadecimal formats)]\r\n\
	[sub.dst:(hexadecimal formats)]\r\n\
	[sub.period:(hexadecimal formats)]\r\n\
	Get Parameter:[net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]", blemeshcli_hb_sub},
	{"blemesh_hb_pub", "blemesh_hb_pub: Heartbeat Publication Set or Get. Set Parameter:[net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[pub.dst:(hexadecimal formats)]\r\n\
	[pub.count: (hexadecimal formats)]\r\n\
	[pub.period: (hexadecimal formats)]\r\n\
	[pub.ttl: (hexadecimal formats)]\r\n\
	[pub.feat: (hexadecimal formats)]\r\n\
	[pub.net_idx: (hexadecimal formats)]\r\n\
	Get Parameter:[net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]", blemeshcli_hb_pub},
	{"blemesh_krp_get", "blemesh_krp_get: Getting Key Refresh Phase. Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	krp_buf.NetKeyIndex(hexadecimal formats)]", blemeshcli_krp_get},
	{"blemesh_krp_set", "blemesh_krp_set: Setting Key Refresh Phase. Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	krp_buf.NetKeyIndex(hexadecimal formats)]\r\n\
	krp_buf.Phase(hexadecimal formats)]", blemeshcli_krp_set},
	{"blemesh_cfg_bcn_get", "blemesh_cfg_bcn_get :Getting Config Beacon Status. Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]", blemeshcli_cfg_bcn_get},
	{"blemesh_cfg_bcn_set", "blemesh_cfg_bcn_set :Setting Config Beacon Status. Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[Value:(hexadecimal formats)]", blemeshcli_cfg_bcn_set},
	{"blemesh_cfg_dttl_get", "blemesh_cfg_dttl_get :Getting Default TTL.Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]", blemeshcli_cfg_dttl_get},
	{"blemesh_cfg_dttl_set", "blemesh_cfg_dttl_set :Setting Default TTL. Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[Value:(hexadecimal formats)]", blemeshcli_cfg_dttl_set},
	{"blemesh_cfg_gpxy_get", "blemesh_cfg_gpxy_get: Getting the GATT Proxy state.Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]", blemeshcli_cfg_gpxy_get},
	{"blemesh_cfg_gpxy_set", "blemesh_cfg_gpxy_get: Setting the GATT Proxy state. Parameter: [net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[Value:(hexadecimal formats)]", blemeshcli_cfg_gpxy_set},
	{"blemesh_friend", "blemesh_friend: getting or setting the Friend state. Set Parameter:[net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[Value:(hexadecimal formats)]\r\n\
	Get Parameter:[net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]", blemeshcli_friend},
	{"blemesh_relay", "blemesh_relay: Getting or setting the Relay state. Set Parameter:[net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[Value:(hexadecimal formats)]\r\n\
	[Count:(hexadecimal formats)]\r\n\
	[Interval:(hexadecimal formats)]\r\n\
	Get Parameter:[net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]", blemeshcli_relay},
	{"blemesh_node_identify", "blemesh_node_identify:Getting or setting the Node Identity state.Set Parameter:[net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[netkeyindex:(hexadecimal formats)]\r\n\
	[Couidentitynt:(hexadecimal formats)]\r\n\
	Get Parameter:[net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]", blemeshcli_node_identify},
	{"blemesh_node_reset", "blemesh_node_reset: Mesh Node Reset. Parameter:[net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]", blemeshcli_node_reset},
	{"blemesh_network_trans", "blemesh_network_trans:Network Transmit Get/Set. Set Parameter:[net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[Count:(hexadecimal formats)]\r\n\
	[Interval:(hexadecimal formats)]\r\n\
	Get Parameter:[net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]", blemeshcli_network_trans},
	{"blemesh_lpn_timeout_get", "blemesh_lpn_timeout_get: Get poll timeout of LPN. Parameter:[net_idx:(hexadecimal formats)]\r\n\
	[Destination address(hexadecimal formats)]\r\n\
	[lpn_addr:(hexadecimal formats)]", blemeshcli_lpn_timeout_get},
	{"blemesh_clhm_fault", "blemesh_clhm_fault: Getting or clear health_fault. Get Parameter: Get [Destination address(hexadecimal formats)]\r\n\
	[net_idx:(hexadecimal formats)]\r\n\
	[cid : Company ID(hexadecimal formats)]\r\n\
	Clear Parameter: clear [Destination address(hexadecimal formats)]\r\n\
	[net_idx:(hexadecimal formats)]\r\n\
	[cid : Company ID(hexadecimal formats)]", blemeshcli_clhm_fault},
	{"blemesh_clhm_period", "blemesh_clhm_period: Get or Set Health Period. Get Parameter: Get [Destination address(hexadecimal formats)]\r\n\
	[app_idx:(hexadecimal formats)]\r\n\
	Set Parameter: Set [Destination address(hexadecimal formats)]\r\n\
	[app_idx:(hexadecimal formats)]\r\n\
	[ updated_divisor: (hexadecimal formats)] ", blemeshcli_clhm_period},
	{"blemesh_clhm_ats", "", blemeshcli_clhm_ats},
#endif
#if defined(CFG_NODE_SEND_CFGCLI_MSG) && defined(CONFIG_BT_MESH_CDB)
	{"blemesh_pvnr_devkey", "", blemeshcli_pvnr_devkey},
#endif
#if defined(CONFIG_BT_MESH_SYNC)
	{"blemesh_sync_start", "No parameter", blemeshcli_sync_start},
	{"blemesh_sync_stop", "No parameter", blemeshcli_sync_stop},
#endif
#if defined(CONFIG_BT_MESH_TEST)
    {"blemesh_nodelist_op", "", blemeshcli_nodelist_op},
#endif /* CONFIG_BT_MESH_TEST */
    #if defined(BL70X)
    {NULL, NULL, "No handler / Invalid command", NULL}
    #endif
};
#endif

/* Read string from uart */
#if defined(CFG_IOT_SDK)
/* Read string from uart */
static void read_str(char* str, u8_t size)
{
	extern int cli_getchar(char *inbuf);
	char* str_s = str;
	while(str - str_s <= size){
		while(!cli_getchar(str));
		if(*str == '\r' || *str == '\n'){
			*str = 0;
			break;
		}
		vOutputString("%c", *str);
		str++;
	}
   
}

#else

static char* str_s = NULL;
static u8_t str_size;
static struct k_sem read_sem;
static void read_str(char* str, u8_t size)
{
	str_s=k_malloc(size);
	str_size=size;
	if(str_s!=NULL)
	{
		k_sem_init(&read_sem,0,1);
		k_sem_take(&read_sem,K_FOREVER);
		memcpy(str,str_s,size);
		k_free(str_s);
		str_s =NULL;
		k_sem_delete(&read_sem);
	}
	else
	{
		BT_WARN("Not support!");
	}
}

BLEMESH_CLI(read_str)
{
	if(argc != 2){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
	else
	{
		if(str_s!=NULL)
		{
			memset(str_s,0,str_size);
			memcpy(str_s,argv[1],str_size);
			k_sem_give(&read_sem);
		}
		else
		{
			BT_WARN("Not support!");
		}
	}

}

#endif

#if defined(CONFIG_BT_MESH_LOW_POWER)
BLEMESH_CLI(lpn_set)
{
    static bool lpn_enabled;
    u8_t enable;
    int err;


    get_uint8_from_string(&argv[1], &enable);
    
    if(enable){
        if(lpn_enabled){
            vOutputString("LPN already enabled\r\n");
            return;
        }
        
        err = bt_mesh_lpn_set(true);
        if(err){
            vOutputString("Failed to enable LPN\r\n");
        }else{
            lpn_enabled = true;
            vOutputString("Enable LPN successfully\r\n");
        }     
    }else{
        if(!lpn_enabled){
            vOutputString("LPN already disabled\r\n");
            return;
        }

        err = bt_mesh_lpn_set(false);
        if(err){
            vOutputString("Failed to disable LPN\r\n");
        }else{
            lpn_enabled = false;
            vOutputString("Disable LPN successfully\r\n");
        }
    }
}

static void lpn_cb(u16_t friend_addr, bool established)
{
	if (established) {
		vOutputString("Friendship (as LPN) established to Friend 0x%04x\r\n", friend_addr);
	} else {
		vOutputString("Friendship (as LPN) lost with Friend 0x%04x\r\n", friend_addr);
	}
}
#if defined(CONFIG_BT_MESH_LOW_POWER)
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
BLEMESH_CLI(lpn_poll)
{
	if (IS_ENABLED(CONFIG_BT_MESH_LOW_POWER))
	{
		bt_mesh_lpn_poll();
	}
}

BLEMESH_CLI(lpn_clear_friend_send)
{
	extern int send_friend_clear(void);
	send_friend_clear();
}

BLEMESH_CLI(lpn_friend_req)
{
	extern int send_friend_req(struct bt_mesh_lpn *lpn);
	struct bt_mesh_lpn *lpn = &bt_mesh.lpn;
	if (lpn->state == BT_MESH_LPN_ENABLED)
	{
		if (IS_ENABLED(CONFIG_BT_MESH_LPN_ESTABLISHMENT)) {
			bt_mesh_scan_disable();
		} else {
			bt_mesh_scan_enable();
		}
		send_friend_req(lpn);
	}
}
BLEMESH_CLI(lpn_group_del)
{
	u16_t groupaddr;
	int err;
	get_uint16_from_string(&argv[1], &groupaddr);
	bt_mesh_lpn_group_del(&groupaddr,1);
}

BLEMESH_CLI(lpn_group_add)
{
	u16_t groupaddr;
	int err;
	get_uint16_from_string(&argv[1], &groupaddr);
	bt_mesh_lpn_group_add(groupaddr);
}
#endif
#endif
#endif
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)

static void light_get(u16_t id, uint32_t opcode,
                bfl_ble_mesh_light_client_get_state_t *get_state)
{
    struct bt_mesh_model *mesh_model = NULL;
    const struct bt_mesh_comp *mesh_comp = bt_mesh_comp_get();

    for(int i = 0; i < mesh_comp->elem_count; ++i){
        mesh_model = bt_mesh_model_find(&mesh_comp->elem[i], id);
        if(mesh_model != NULL){
            break;
        }
    }
    if(mesh_model == NULL){
        return;
    }
    client_common.model = mesh_model;
    client_common.opcode = opcode;
    bfl_ble_mesh_light_client_get_state(&client_common, get_state);

}
static void gen_onoff_get(uint8_t *data, uint16_t len)
{
    struct bt_mesh_model *mesh_model = NULL;

    const struct bt_mesh_comp *mesh_comp = bt_mesh_comp_get();
    for(int i = 0; i < mesh_comp->elem_count; ++i){
        mesh_model = bt_mesh_model_find(&mesh_comp->elem[i], BFL_BLE_MESH_MODEL_ID_GEN_ONOFF_CLI);
        if(mesh_model != NULL){
            break;
        }
    }
    if(mesh_model == NULL){
        return;
    }
    client_common.model = mesh_model;
    client_common.opcode = BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_GET;
    bfl_ble_mesh_generic_client_get_state(&client_common, NULL);
}
static void gen_onoff_set(uint8_t *data, uint16_t len)
{
    struct bt_mesh_model *mesh_model = NULL;
    bfl_ble_mesh_generic_client_set_state_t gen_client_set = {0};
    const struct bt_mesh_comp *mesh_comp = bt_mesh_comp_get();

    memset(&gen_client_set, 0, sizeof(gen_client_set));
    for(int i = 0; i < mesh_comp->elem_count; ++i){
        mesh_model = bt_mesh_model_find(&mesh_comp->elem[i], BFL_BLE_MESH_MODEL_ID_GEN_ONOFF_CLI);
        if(mesh_model != NULL){
            break;
        }
    }
    if(mesh_model == NULL){
        return;
    }

    client_common.opcode = BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK - data[0];
    client_common.model = mesh_model;
    gen_client_set.onoff_set.onoff = data[1];
    if(len > 2){
        gen_client_set.onoff_set.trans_time = data[2];
        gen_client_set.onoff_set.delay = data[3];
        gen_client_set.onoff_set.op_en = 1;
    }
    bfl_ble_mesh_generic_client_set_state(&client_common, &gen_client_set);
}
static void light_set(u16_t id, uint32_t opcode, uint8_t ack,
                bfl_ble_mesh_light_client_set_state_t *set_state)
{
    struct bt_mesh_model *mesh_model = NULL;
    const struct bt_mesh_comp *mesh_comp = bt_mesh_comp_get();

    for(int i = 0; i < mesh_comp->elem_count; ++i){
        mesh_model = bt_mesh_model_find(&mesh_comp->elem[i], id);
        if(mesh_model != NULL){
            break;
        }
    }
    if(mesh_model == NULL){
        return;
    }

    client_common.opcode = opcode - ack;
    client_common.model = mesh_model;

    bfl_ble_mesh_light_client_set_state(&client_common, set_state);
}
static void gen_lvl_get(uint8_t *data, uint16_t len)
{
    struct bt_mesh_model *mesh_model = NULL;

    const struct bt_mesh_comp *mesh_comp = bt_mesh_comp_get();
    for(int i = 0; i < mesh_comp->elem_count; ++i){
        mesh_model = bt_mesh_model_find(&mesh_comp->elem[i], BFL_BLE_MESH_MODEL_ID_GEN_LEVEL_CLI);
        if(mesh_model != NULL){
            break;
        }
    }
    if(mesh_model == NULL){
        return;
    }
    client_common.model = mesh_model;
    client_common.opcode = BFL_BLE_MESH_MODEL_OP_GEN_LEVEL_GET;
    bfl_ble_mesh_generic_client_get_state(&client_common, NULL);
}
static void gen_lvl_set(uint8_t *data, uint16_t len)
{
    struct bt_mesh_model *mesh_model = NULL;
    bfl_ble_mesh_generic_client_set_state_t gen_client_set = {0};
    const struct bt_mesh_comp *mesh_comp = bt_mesh_comp_get();

    memset(&gen_client_set, 0, sizeof(gen_client_set));
    for(int i = 0; i < mesh_comp->elem_count; ++i){
        mesh_model = bt_mesh_model_find(&mesh_comp->elem[i], BFL_BLE_MESH_MODEL_ID_GEN_LEVEL_CLI);
        if(mesh_model != NULL){
            break;
        }
    }
    if(mesh_model == NULL){
        return;
    }

    client_common.opcode = BFL_BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK - data[0];
    client_common.model = mesh_model;
    gen_client_set.level_set.level = data[1] + (data[2] << 8);
    if(len > 3){
        gen_client_set.level_set.trans_time = data[3];
        gen_client_set.level_set.delay = data[4];
        gen_client_set.level_set.op_en = 1;
    }
    bfl_ble_mesh_generic_client_set_state(&client_common, &gen_client_set);
}
static void gen_lvl_delta_set(uint8_t *data, uint16_t len)
{
    struct bt_mesh_model *mesh_model = NULL;
    bfl_ble_mesh_generic_client_set_state_t gen_client_set = {0};
    const struct bt_mesh_comp *mesh_comp = bt_mesh_comp_get();

    memset(&gen_client_set, 0, sizeof(gen_client_set));
    for(int i = 0; i < mesh_comp->elem_count; ++i){
        mesh_model = bt_mesh_model_find(&mesh_comp->elem[i], BFL_BLE_MESH_MODEL_ID_GEN_LEVEL_CLI);
        if(mesh_model != NULL){
            break;
        }
    }
    if(mesh_model == NULL){
        return;
    }

    client_common.opcode = BFL_BLE_MESH_MODEL_OP_GEN_DELTA_SET_UNACK - data[0];
    client_common.model = mesh_model;
    gen_client_set.delta_set.level = data[1] + (data[2] << 8) + (data[3] << 16) + (data[4] << 24);
    if(len > 3){
        gen_client_set.delta_set.trans_time = data[5];
        gen_client_set.delta_set.delay = data[6];
        gen_client_set.delta_set.op_en = 1;
    }
    bfl_ble_mesh_generic_client_set_state(&client_common, &gen_client_set);

}
static void gen_lvl_move_set(uint8_t *data, uint16_t len)
{
    struct bt_mesh_model *mesh_model = NULL;
    bfl_ble_mesh_generic_client_set_state_t gen_client_set = {0};
    const struct bt_mesh_comp *mesh_comp = bt_mesh_comp_get();

    memset(&gen_client_set, 0, sizeof(gen_client_set));
    for(int i = 0; i < mesh_comp->elem_count; ++i){
        mesh_model = bt_mesh_model_find(&mesh_comp->elem[i], BFL_BLE_MESH_MODEL_ID_GEN_LEVEL_CLI);
        if(mesh_model != NULL){
            break;
        }
    }
    if(mesh_model == NULL){
        return;
    }

    client_common.opcode = BFL_BLE_MESH_MODEL_OP_GEN_MOVE_SET_UNACK - data[0];
    client_common.model = mesh_model;
    gen_client_set.move_set.delta_level = data[1] + (data[2] << 8);
    if(len > 3){
        gen_client_set.move_set.trans_time = data[3];
        gen_client_set.move_set.delay = data[4];
        gen_client_set.move_set.op_en = 1;
    }
    bfl_ble_mesh_generic_client_set_state(&client_common, &gen_client_set);
}

static void gen_dtt_get(uint8_t *data, uint16_t len)
{
    struct bt_mesh_model *mesh_model = NULL;

    const struct bt_mesh_comp *mesh_comp = bt_mesh_comp_get();
    for(int i = 0; i < mesh_comp->elem_count; ++i){
        mesh_model = bt_mesh_model_find(&mesh_comp->elem[i], BFL_BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI);
        if(mesh_model != NULL){
            break;
        }
    }
    if(mesh_model == NULL){
        return;
    }
    client_common.model = mesh_model;
    client_common.opcode = BFL_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_GET;
    bfl_ble_mesh_generic_client_get_state(&client_common, NULL);
}

static void gen_dtt_set(uint8_t *data, uint16_t len)
{
    struct bt_mesh_model *mesh_model = NULL;
    bfl_ble_mesh_generic_client_set_state_t gen_client_set = {0};
    const struct bt_mesh_comp *mesh_comp = bt_mesh_comp_get();

    memset(&gen_client_set, 0, sizeof(gen_client_set));
    for(int i = 0; i < mesh_comp->elem_count; ++i){
        mesh_model = bt_mesh_model_find(&mesh_comp->elem[i], BFL_BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI);
        if(mesh_model != NULL){
            break;
        }
    }
    if(mesh_model == NULL){
        return;
    }

    client_common.opcode = BFL_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET_UNACK - data[0];
    client_common.model = mesh_model;
    gen_client_set.def_trans_time_set.trans_time = data[1];

    bfl_ble_mesh_generic_client_set_state(&client_common, &gen_client_set);
}

static void gen_ponoff_get(uint8_t *data, uint16_t len)
{
    struct bt_mesh_model *mesh_model = NULL;

    const struct bt_mesh_comp *mesh_comp = bt_mesh_comp_get();
    for(int i = 0; i < mesh_comp->elem_count; ++i){
        mesh_model = bt_mesh_model_find(&mesh_comp->elem[i], BFL_BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI);
        if(mesh_model != NULL){
            break;
        }
    }
    if(mesh_model == NULL){
        return;
    }
    client_common.model = mesh_model;
    client_common.opcode = BFL_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_GET;
    bfl_ble_mesh_generic_client_get_state(&client_common, NULL);
}
static void gen_ponoff_set(uint8_t *data, uint16_t len)
{
    struct bt_mesh_model *mesh_model = NULL;
    bfl_ble_mesh_generic_client_set_state_t gen_client_set = {0};
    const struct bt_mesh_comp *mesh_comp = bt_mesh_comp_get();

    memset(&gen_client_set, 0, sizeof(gen_client_set));
    for(int i = 0; i < mesh_comp->elem_count; ++i){
        mesh_model = bt_mesh_model_find(&mesh_comp->elem[i], BFL_BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI);
        if(mesh_model != NULL){
            break;
        }
    }
    if(mesh_model == NULL){
        return;
    }

    client_common.opcode = BFL_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET_UNACK - data[0];
    client_common.model = mesh_model;
    gen_client_set.power_set.onpowerup = data[1];

    bfl_ble_mesh_generic_client_set_state(&client_common, &gen_client_set);

}

static void gen_plvl_get(uint8_t *data, uint16_t len)
{
    struct bt_mesh_model *mesh_model = NULL;

    const struct bt_mesh_comp *mesh_comp = bt_mesh_comp_get();
    for(int i = 0; i < mesh_comp->elem_count; ++i){
        mesh_model = bt_mesh_model_find(&mesh_comp->elem[i], BFL_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI);
        if(mesh_model != NULL){
            break;
        }
    }
    if(mesh_model == NULL){
        return;
    }
    client_common.model = mesh_model;
    client_common.opcode = BFL_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET;
    bfl_ble_mesh_generic_client_get_state(&client_common, NULL);

}
static void gen_plvl_set(uint8_t *data, uint16_t len)
{
    struct bt_mesh_model *mesh_model = NULL;
    bfl_ble_mesh_generic_client_set_state_t gen_client_set = {0};
    const struct bt_mesh_comp *mesh_comp = bt_mesh_comp_get();

    memset(&gen_client_set, 0, sizeof(gen_client_set));
    for(int i = 0; i < mesh_comp->elem_count; ++i){
        mesh_model = bt_mesh_model_find(&mesh_comp->elem[i], BFL_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI);
        if(mesh_model != NULL){
            break;
        }
    }
    if(mesh_model == NULL){
        return;
    }

    client_common.opcode = BFL_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK - data[0];
    client_common.model = mesh_model;
    gen_client_set.power_level_set.power = data[1] + (data[2] << 8);
    if(len > 3){
        gen_client_set.power_level_set.trans_time = data[3];
        gen_client_set.power_level_set.delay = data[4];
        gen_client_set.power_level_set.op_en = 1;
    }

    bfl_ble_mesh_generic_client_set_state(&client_common, &gen_client_set);

}

static void gen_plvl_last_get(uint8_t *data, uint16_t len)
{
    common_get(BFL_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI, BFL_BLE_MESH_MODEL_OP_GEN_POWER_LAST_GET);
}
static void gen_plvl_dflt_get(uint8_t *data, uint16_t len)
{
    common_get(BFL_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI, BFL_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_GET);
}
static void gen_plvl_range_get(uint8_t *data, uint16_t len)
{
    common_get(BFL_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI, BFL_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_GET);
}
static void gen_plvl_dflt_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_generic_client_set_state_t gen_client_set = {0};
    gen_client_set.power_default_set.power = data[1] + (data[2] << 8);
    common_set(BFL_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI, BFL_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET_UNACK,data[0], &gen_client_set);
}
static void gen_plvl_range_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_generic_client_set_state_t gen_client_set = {0};
    gen_client_set.power_range_set.range_min = data[1] + (data[2] << 8);
    gen_client_set.power_range_set.range_max = data[3] + (data[4] << 8);
    common_set(BFL_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI, BFL_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET_UNACK, data[0], &gen_client_set);
}
static void gen_battery_get(uint8_t *data, uint16_t len)
{
    common_get(BFL_BLE_MESH_MODEL_ID_GEN_BATTERY_CLI, BFL_BLE_MESH_MODEL_OP_GEN_BATTERY_GET);
}
static void gen_loc_global_get(uint8_t *data, uint16_t len)
{
    common_get(BFL_BLE_MESH_MODEL_ID_GEN_LOCATION_CLI, BFL_BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_GET);
}
static void gen_loc_local_get(uint8_t *data, uint16_t len)
{
    common_get(BFL_BLE_MESH_MODEL_ID_GEN_LOCATION_CLI, BFL_BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_GET);
}
static void gen_loc_global_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_generic_client_set_state_t gen_client_set = {0};
    gen_client_set.loc_global_set.global_latitude = data[1] + (data[2] << 8) + (data[3] << 16) + (data[4] << 24);
    gen_client_set.loc_global_set.global_longitude = data[5] + (data[6] << 8) + (data[7] << 16) + (data[8] << 24);
    gen_client_set.loc_global_set.global_altitude = data[9] + (data[10] << 8);
    common_set(BFL_BLE_MESH_MODEL_ID_GEN_LOCATION_CLI, BFL_BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET_UNACK,data[0], &gen_client_set);
}
static void gen_loc_local_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_generic_client_set_state_t gen_client_set = {0};
    gen_client_set.loc_local_set.local_north = data[1] + (data[2] << 8);
    gen_client_set.loc_local_set.local_east = data[3] + (data[4] << 8);
    gen_client_set.loc_local_set.local_altitude = data[5] + (data[6] << 8);
    gen_client_set.loc_local_set.floor_number = data[7];
    gen_client_set.loc_local_set.uncertainty = data[8] + (data[9] << 8);
    common_set(BFL_BLE_MESH_MODEL_ID_GEN_LOCATION_CLI, BFL_BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET_UNACK, data[0], &gen_client_set);

}
static void gen_props_get(uint8_t *data, uint16_t len)
{
    struct bt_mesh_model *mesh_model = NULL;
    bfl_ble_mesh_generic_client_get_state_t get_state;
    uint16_t property_id = data[1] + (data[2] << 8);
    uint32_t opcode;

    switch(data[0]){
    case 0:/*mfr_props*/
        get_state.manufacturer_property_get.property_id = property_id;
        opcode = BFL_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTIES_GET;
        break;
    case 1:/*admin_props*/
        get_state.admin_property_get.property_id = property_id;
        opcode = BFL_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_GET;
        break;
    case 2:/*usr_props*/
        get_state.user_property_get.property_id = property_id;
        opcode = BFL_BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_GET;
        break;
    case 3:/*cli_props*/
        get_state.client_properties_get.property_id = property_id;
        opcode = BFL_BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_GET;
        break;
    default:
        return;
        break;
    }

    const struct bt_mesh_comp *mesh_comp = bt_mesh_comp_get();
    for(int i = 0; i < mesh_comp->elem_count; ++i){
        mesh_model = bt_mesh_model_find(&mesh_comp->elem[i], BFL_BLE_MESH_MODEL_ID_GEN_PROP_CLI);
        if(mesh_model != NULL){
            break;
        }
    }
    if(mesh_model == NULL){
        return;
    }
    client_common.model = mesh_model;
    client_common.opcode = opcode,
    bfl_ble_mesh_generic_client_get_state(&client_common, &get_state);
}
static void gen_prop_get(uint8_t *data, uint16_t len)
{
    struct bt_mesh_model *mesh_model = NULL;
    bfl_ble_mesh_generic_client_get_state_t get_state;
    uint16_t property_id = data[1] + (data[2] << 8);
    uint32_t opcode;

    switch(data[0]){
    case 0:/*mfr_props*/
        get_state.manufacturer_property_get.property_id = property_id;
        opcode = BFL_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_GET;
        break;
    case 1:/*admin_props*/
        get_state.admin_property_get.property_id = property_id;
        opcode = BFL_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_GET;
        break;
    case 2:/*usr_props*/
        get_state.user_property_get.property_id = property_id;
        opcode = BFL_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET;
        break;
    default:
        return;
        break;
    }

    const struct bt_mesh_comp *mesh_comp = bt_mesh_comp_get();
    for(int i = 0; i < mesh_comp->elem_count; ++i){
        mesh_model = bt_mesh_model_find(&mesh_comp->elem[i], BFL_BLE_MESH_MODEL_ID_GEN_PROP_CLI);
        if(mesh_model != NULL){
            break;
        }
    }
    if(mesh_model == NULL){
        return;
    }
    client_common.model = mesh_model;
    client_common.opcode = opcode,
    bfl_ble_mesh_generic_client_get_state(&client_common, &get_state);

}
static void gen_prop_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_generic_client_set_state_t gen_client_set = {0};
    uint32_t opcode;
    struct net_buf_simple *buf = NET_BUF_SIMPLE(40);

    net_buf_simple_init(buf, 0);
    switch(data[1]){
    case 0:/*mfr_props*/
        opcode = BFL_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_SET_UNACK;
        gen_client_set.manufacturer_property_set.property_id = data[2] + (data[3] << 8);
        gen_client_set.manufacturer_property_set.user_access = data[4];
        break;
    case 1:/*admin_props*/
        opcode = BFL_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET_UNACK;
        gen_client_set.admin_property_set.property_id = data[2] + (data[3] << 8);
        gen_client_set.admin_property_set.user_access = data[4];
        net_buf_simple_add_mem(buf, &data[6], data[5]);
        gen_client_set.admin_property_set.property_value = buf;
        break;
    case 2:/*usr_props*/
        opcode = BFL_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET_UNACK;
        gen_client_set.user_property_set.property_id = data[2] + (data[3] << 8);
        net_buf_simple_add_mem(buf, &data[6], data[5]);
        gen_client_set.user_property_set.property_value = buf;
        break;
    default:
        return;
        break;
    }

    common_set(BFL_BLE_MESH_MODEL_ID_GEN_PROP_CLI, opcode, data[0], &gen_client_set);
}
static void light_lightness_get(uint8_t *data, uint16_t len)
{
    light_get(BFL_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET, NULL);
}

static void light_lightness_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_light_client_set_state_t set_state = {0};
    set_state.lightness_set.lightness = data[1] + (data[2] << 8);
    if(len > 3){
        set_state.lightness_set.trans_time = data[3];
        set_state.lightness_set.delay = data[4];
        set_state.lightness_set.op_en = 1;
    }
    light_set(BFL_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK,data[0], &set_state);
}

static void light_lightness_linear_get(uint8_t *data, uint16_t len)
{
    light_get(BFL_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET, NULL);
}

static void light_lightness_linear_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_light_client_set_state_t set_state = {0};
    set_state.lightness_linear_set.lightness = data[1] + (data[2] << 8);
    if(len > 3){
        set_state.lightness_set.trans_time = data[3];
        set_state.lightness_set.delay = data[4];
        set_state.lightness_set.op_en = 1;
    }
    light_set(BFL_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK,data[0], &set_state);
}
static void light_lightness_last_get(uint8_t *data, uint16_t len)
{
    light_get(BFL_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET, NULL);
}
static void light_lightness_default_get(uint8_t *data, uint16_t len)
{
    light_get(BFL_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET, NULL);
}
static void light_lightness_default_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_light_client_set_state_t set_state = {0};
    set_state.lightness_default_set.lightness = data[1] + (data[2] << 8);
    light_set(BFL_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK, data[0], &set_state);
}
static void light_lightness_range_get(uint8_t *data, uint16_t len)
{
    light_get(BFL_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET, NULL);
}
static void light_lightness_range_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_light_client_set_state_t set_state = {0};
    set_state.lightness_range_set.range_min = data[1] + (data[2] << 8);
    set_state.lightness_range_set.range_max = data[3] + (data[4] << 8);
    light_set(BFL_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK, data[0], &set_state);
}
static void light_lc_mode_get(uint8_t *data, uint16_t len)
{
    light_get(BFL_BLE_MESH_MODEL_ID_LIGHT_LC_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_LC_MODE_GET, NULL);
}
static void light_lc_mode_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_light_client_set_state_t set_state = {0};
    set_state.lc_mode_set.mode = data[1];
    light_set(BFL_BLE_MESH_MODEL_ID_LIGHT_LC_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET_UNACK,data[0], &set_state);

}

static void light_ctl_states_get(uint8_t *data, uint16_t len)
{
    light_get(BFL_BLE_MESH_MODEL_ID_LIGHT_CTL_CLI,
                BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_GET, NULL);
}
static void light_ctl_states_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_light_client_set_state_t set_state = {0};
    set_state.ctl_set.ctl_lightness = data[1] + (data[2] << 8);
    set_state.ctl_set.ctl_temperatrue = data[3] + (data[4] << 8);
    set_state.ctl_set.ctl_delta_uv = data[5] + (data[6] << 8);

    if(len > 7){
        set_state.ctl_set.trans_time = data[7];
        set_state.ctl_set.delay = data[8];
        set_state.ctl_set.op_en = 1;
    }

    light_set(BFL_BLE_MESH_MODEL_ID_LIGHT_CTL_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK, data[0], &set_state);
}
static void light_ctl_temperature_get(uint8_t *data, uint16_t len)
{
    light_get(BFL_BLE_MESH_MODEL_ID_LIGHT_CTL_CLI,
                BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_GET, NULL);
}
static void light_ctl_temperature_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_light_client_set_state_t set_state = {0};
    set_state.ctl_temperature_set.ctl_temperatrue = data[1] + (data[2] << 8);
    set_state.ctl_temperature_set.ctl_delta_uv = data[3] + (data[4] << 8);

    if(len > 5){
        set_state.ctl_temperature_set.trans_time = data[5];
        set_state.ctl_temperature_set.delay = data[6];
        set_state.ctl_temperature_set.op_en = 1;
    }

    light_set(BFL_BLE_MESH_MODEL_ID_LIGHT_CTL_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET_UNACK, data[0], &set_state);
}
static void light_ctl_default_get(uint8_t *data, uint16_t len)
{
    light_get(BFL_BLE_MESH_MODEL_ID_LIGHT_CTL_CLI,
                BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_GET, NULL);
}
static void light_ctl_default_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_light_client_set_state_t set_state = {0};
    set_state.ctl_default_set.lightness = data[1] + (data[2] << 8);
    set_state.ctl_default_set.temperature = data[3] + (data[4] << 8);
    set_state.ctl_default_set.delta_uv = data[5] + (data[6] << 8);

    light_set(BFL_BLE_MESH_MODEL_ID_LIGHT_CTL_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET_UNACK, data[0], &set_state);
}
static void light_ctl_temp_range_get(uint8_t *data, uint16_t len)
{
    light_get(BFL_BLE_MESH_MODEL_ID_LIGHT_CTL_CLI,
                BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_GET, NULL);
}
static void light_ctl_temp_range_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_light_client_set_state_t set_state = {0};
    set_state.ctl_temperature_range_set.range_min = data[1] + (data[2] << 8);
    set_state.ctl_temperature_range_set.range_max = data[3] + (data[4] << 8);

    light_set(BFL_BLE_MESH_MODEL_ID_LIGHT_CTL_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK, data[0], &set_state);
}

static void light_xyl_get(uint8_t *data, uint16_t len)
{
    light_get(BFL_BLE_MESH_MODEL_ID_LIGHT_XYL_CLI,
                BFL_BLE_MESH_MODEL_OP_LIGHT_XYL_GET, NULL);
}
static void light_xyl_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_light_client_set_state_t set_state = {0};
    set_state.xyl_set.xyl_lightness = data[1] + (data[2] << 8);
    set_state.xyl_set.xyl_x = data[3] + (data[4] << 8);
    set_state.xyl_set.xyl_y = data[5] + (data[6] << 8);
    if(len > 7){
        set_state.xyl_set.op_en = 1;
        set_state.xyl_set.trans_time = data[7];
        set_state.xyl_set.delay = data[8];
    }

    light_set(BFL_BLE_MESH_MODEL_ID_LIGHT_XYL_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_XYL_SET_UNACK,data[0], &set_state);
}
static void light_xyl_target_get(uint8_t *data, uint16_t len)
{
    light_get(BFL_BLE_MESH_MODEL_ID_LIGHT_XYL_CLI,
                BFL_BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_GET, NULL);
}
static void light_xyl_default_get(uint8_t *data, uint16_t len)
{
    light_get(BFL_BLE_MESH_MODEL_ID_LIGHT_XYL_CLI,
                BFL_BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_GET, NULL);
}
static void light_xyl_default_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_light_client_set_state_t set_state = {0};
    set_state.xyl_default_set.lightness = data[1] + (data[2] << 8);
    set_state.xyl_default_set.xyl_x = data[3] + (data[4] << 8);
    set_state.xyl_default_set.xyl_y = data[5] + (data[6] << 8);

    light_set(BFL_BLE_MESH_MODEL_ID_LIGHT_XYL_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET_UNACK, data[0], &set_state);
}
static void light_xyl_range_get(uint8_t *data, uint16_t len)
{
    light_get(BFL_BLE_MESH_MODEL_ID_LIGHT_XYL_CLI,
                BFL_BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_GET, NULL);
}
static void light_xyl_range_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_light_client_set_state_t set_state = {0};
    set_state.xyl_range_set.xyl_x_range_min = data[1] + (data[2] << 8);
    set_state.xyl_range_set.xyl_y_range_min = data[3] + (data[4] << 8);
    set_state.xyl_range_set.xyl_x_range_max = data[5] + (data[6] << 8);
    set_state.xyl_range_set.xyl_y_range_max = data[7] + (data[8] << 8);

    light_set(BFL_BLE_MESH_MODEL_ID_LIGHT_XYL_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET_UNACK, data[0], &set_state);
}
static void light_hsl_get(uint8_t *data, uint16_t len)
{
    light_get(BFL_BLE_MESH_MODEL_ID_LIGHT_HSL_CLI,
                BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_GET, NULL);
}
static void light_hsl_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_light_client_set_state_t set_state = {0};
    set_state.hsl_set.hsl_lightness = data[1] + (data[2] << 8);
    set_state.hsl_set.hsl_hue = data[3] + (data[4] << 8);
    set_state.hsl_set.hsl_saturation = data[5] + (data[6] << 8);
    if(len > 7){
        set_state.xyl_set.op_en = 1;
        set_state.xyl_set.trans_time = data[7];
        set_state.xyl_set.delay = data[8];
    }

    light_set(BFL_BLE_MESH_MODEL_ID_LIGHT_HSL_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK, data[0], &set_state);
}
static void light_hsl_target_get(uint8_t *data, uint16_t len)
{
    light_get(BFL_BLE_MESH_MODEL_ID_LIGHT_HSL_CLI,
                BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_GET, NULL);
}
static void light_hsl_default_get(uint8_t *data, uint16_t len)
{
    light_get(BFL_BLE_MESH_MODEL_ID_LIGHT_HSL_CLI,
                    BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_GET, NULL);
}
static void light_hsl_default_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_light_client_set_state_t set_state = {0};
    set_state.hsl_default_set.lightness = data[1] + (data[2] << 8);
    set_state.hsl_default_set.hue = data[3] + (data[4] << 8);
    set_state.hsl_default_set.saturation = data[5] + (data[6] << 8);

    light_set(BFL_BLE_MESH_MODEL_ID_LIGHT_HSL_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET_UNACK,data[0], &set_state);
}
static void light_hsl_range_get(uint8_t *data, uint16_t len)
{
    light_get(BFL_BLE_MESH_MODEL_ID_LIGHT_HSL_CLI,
                        BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_GET, NULL);
}
static void light_hsl_range_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_light_client_set_state_t set_state = {0};
    set_state.hsl_range_set.hue_range_min = data[1] + (data[2] << 8);
    set_state.hsl_range_set.saturation_range_min = data[3] + (data[4] << 8);
    set_state.hsl_range_set.hue_range_max = data[5] + (data[6] << 8);
    set_state.hsl_range_set.saturation_range_max = data[7] + (data[8] << 8);

    light_set(BFL_BLE_MESH_MODEL_ID_LIGHT_HSL_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET_UNACK, data[0], &set_state);

}
static void light_hsl_hue_get(uint8_t *data, uint16_t len)
{
    light_get(BFL_BLE_MESH_MODEL_ID_LIGHT_HSL_CLI,
                        BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_GET, NULL);
}
static void light_hsl_hue_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_light_client_set_state_t set_state = {0};
    set_state.hsl_hue_set.hue = data[1] + (data[2] << 8);
    if(len > 3){
        set_state.hsl_hue_set.op_en = 1;
        set_state.hsl_hue_set.trans_time = data[3];
        set_state.hsl_hue_set.delay = data[4];
    }

    light_set(BFL_BLE_MESH_MODEL_ID_LIGHT_HSL_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET_UNACK,data[0], &set_state);
}
static void light_hsl_saturation_get(uint8_t *data, uint16_t len)
{
    light_get(BFL_BLE_MESH_MODEL_ID_LIGHT_HSL_CLI,
                        BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_GET, NULL);
}
static void light_hsl_saturation_set(uint8_t *data, uint16_t len)
{
    bfl_ble_mesh_light_client_set_state_t set_state = {0};
    set_state.hsl_saturation_set.saturation = data[1] + (data[2] << 8);
    if(len > 3){
        set_state.hsl_saturation_set.op_en = 1;
        set_state.hsl_saturation_set.trans_time = data[3];
        set_state.hsl_saturation_set.delay = data[4];
    }

    light_set(BFL_BLE_MESH_MODEL_ID_LIGHT_HSL_CLI,
        BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET_UNACK, data[0], &set_state);
}


static void pts_tester_handle_mesh_model(u32_t opcode, uint8_t *data, uint16_t len)
{
    switch (opcode) {
    case BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_GET:{gen_onoff_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK:{gen_onoff_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_LEVEL_GET:{gen_lvl_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK:{gen_lvl_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_DELTA_SET_UNACK:{gen_lvl_delta_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_MOVE_SET_UNACK:{gen_lvl_move_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_GET:{gen_dtt_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET_UNACK:{gen_dtt_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_GET:{gen_ponoff_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET_UNACK:{gen_ponoff_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET:{gen_plvl_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK:{gen_plvl_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_POWER_LAST_GET:{gen_plvl_last_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_GET:{gen_plvl_dflt_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_GET:{gen_plvl_range_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET_UNACK:{gen_plvl_dflt_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET_UNACK:{gen_plvl_range_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_BATTERY_GET:{gen_battery_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_GET:{gen_loc_global_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_GET:{gen_loc_local_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET_UNACK:{gen_loc_global_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET_UNACK:{gen_loc_local_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTIES_GET:{gen_props_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_GET:{gen_props_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_GET:{gen_props_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_GET:{gen_prop_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_GET:{gen_prop_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET:{gen_prop_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_SET_UNACK:{gen_prop_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET_UNACK:{gen_prop_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET_UNACK:{gen_prop_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET:{light_lightness_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK:{light_lightness_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET:{light_lightness_linear_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK:{light_lightness_linear_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET:{light_lightness_last_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET:{light_lightness_default_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK:{light_lightness_default_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET:{light_lightness_range_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK:{light_lightness_range_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_GET:{light_ctl_states_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK:{light_ctl_states_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_GET:{light_ctl_temperature_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET_UNACK:{light_ctl_temperature_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_GET:{light_ctl_default_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET_UNACK:{light_ctl_default_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_GET:{light_ctl_temp_range_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK:{light_ctl_temp_range_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_XYL_GET:{light_xyl_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_XYL_SET_UNACK:{light_xyl_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_GET:{light_xyl_target_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_GET:{light_xyl_default_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET_UNACK:{light_xyl_default_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_GET:{light_xyl_range_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET_UNACK:{light_xyl_range_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_GET:{light_hsl_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK:{light_hsl_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_GET:{light_hsl_target_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_GET:{light_hsl_default_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET_UNACK:{light_hsl_default_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_GET:{light_hsl_range_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET_UNACK:{light_hsl_range_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_GET:{light_hsl_hue_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET_UNACK:{light_hsl_hue_set(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_GET:{light_hsl_saturation_get(data, len);}break;
    case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET_UNACK:{light_hsl_saturation_set(data, len);}break;
	default:
		break;
	}
}

BLEMESH_CLI(pts_models_client_cli)
{
	u32_t opcode;
	get_uint32_from_string(&argv[1], &opcode);
	BT_WARN("Opcode =%lx",opcode);
	if(argc == 3)
	{
		uint16_t len = strlen(argv[2])>>1;
		uint8_t val[40];
		get_bytearray_from_string(&argv[2], val, len);
		for(int i=0; i<len;i++)
		{
			BT_WARN("data[%d] =%x",i,val[i]);
		}
		pts_tester_handle_mesh_model(opcode,val,len);
	}
	else
	{
		pts_tester_handle_mesh_model(opcode,NULL,0);
	}
}
#endif
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
/*
A Generic Power OnOff Server shall use the Generic OnPowerUp state to determine the behavior 
after a node is powered up.
If the value of the Generic OnPowerUp state is 0x00, the Generic OnOff state shall be set to Off.
If the value of the Generic OnPowerUp state is 0x01, the Generic OnOff state shall be set to On. The 
bound states shall be set to their default values, if defined.
If the value of the Generic OnPowerUp state is 0x02, the bound states shall be restored to the states 
they were in when powered down. If the bound states were in transition to new target states when a 
node was powered down, they shall be restored to the target states. If the bound states were in 
transition with unknown target states (i.e., as a result of receiving a Generic Move message), they 
shall continue the transition.
If the value of the Generic OnPowerUp state is 0x02 and a transition was in progress when powered 
down, the element restores the target state when powered up. 
If the value of the Generic OnPowerUp state is 0x02 and a transition was not in progress when 
powered down, the element restores the state it was in when powered down.
Each element shall transition to the determined state using its value of the Generic Default Transition 
Time state as the transition time. If the Generic Default Transition Time is not defined for the element, 
it shall transition to the determined state instantaneously.
*/
/* Generic OnOff State Power-Up Behavior – Off
 * Generic OnOff State Power-Up Behavior – Default
 * Generic OnOff State Power-Up Behavior – Restore
 */


/* Generic Power Actual State Binding With Generic Level
 * Generic Power Actual State Binding With Generic OnOff – Scenario 1
 * Generic Power Actual State Binding With Generic OnOff – Scenario 2
 * Generic Power Actual State Binding With Generic OnOff – Using Last Value
 * Generic Power Actual State Binding With Generic OnOff – Using Default Value
 *
 * Generic Level State Implicit Binding With Generic OnOff – Scenario 1
 * Generic Level State Implicit Binding With Generic OnOff – Scenario 2
 * Generic Power Actual State Power-Up Behavior
 */

static void mmdl_generic_server_cb(bfl_ble_mesh_generic_server_cb_event_t event,
                                               bfl_ble_mesh_generic_server_cb_param_t *param)
{
    const struct bt_mesh_comp *mesh_comp = bt_mesh_comp_get();
    struct bt_mesh_model *gpl_m,*goo_m,*gl_m,*gpo_m,*lln_m,*lctl_m,*lhsl_m,*lc_m;
    bfl_ble_mesh_gen_power_level_srv_t* gpl_srv;
    bfl_ble_mesh_gen_onoff_srv_t* goo_srv;
    bfl_ble_mesh_gen_level_srv_t* gl_srv;
    bfl_ble_mesh_gen_power_onoff_srv_t* gpo_srv;
    bfl_ble_mesh_light_lightness_srv_t* lln_srv;
    bfl_ble_mesh_light_ctl_srv_t* lctl_srv;
    bfl_ble_mesh_light_hsl_srv_t* lhsl_srv;

    gpl_m = bt_mesh_model_find(&mesh_comp->elem[0], BFL_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV);
    gpl_srv = (bfl_ble_mesh_gen_power_level_srv_t*)gpl_m->user_data;
    goo_m = bt_mesh_model_find(&mesh_comp->elem[0], BFL_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV);
    goo_srv = (bfl_ble_mesh_gen_onoff_srv_t*)goo_m->user_data;
    gl_m = bt_mesh_model_find(&mesh_comp->elem[0], BFL_BLE_MESH_MODEL_ID_GEN_LEVEL_SRV);
    gl_srv = (bfl_ble_mesh_gen_level_srv_t*)gl_m->user_data;
    gpo_m = bt_mesh_model_find(&mesh_comp->elem[0], BFL_BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV);
    gpo_srv = (bfl_ble_mesh_gen_power_onoff_srv_t*)gpo_m->user_data;

    lln_m = bt_mesh_model_find(&mesh_comp->elem[0], BFL_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV);
    lln_srv = (bfl_ble_mesh_light_lightness_srv_t*)lln_m->user_data;
    lctl_m = bt_mesh_model_find(&mesh_comp->elem[0], BFL_BLE_MESH_MODEL_ID_LIGHT_CTL_SRV);
    lctl_srv = (bfl_ble_mesh_light_ctl_srv_t*)lctl_m->user_data;
    lhsl_m = bt_mesh_model_find(&mesh_comp->elem[0], BFL_BLE_MESH_MODEL_ID_LIGHT_HSL_SRV);
    lhsl_srv = (bfl_ble_mesh_light_hsl_srv_t*)lhsl_m->user_data;

    //bfl_ble_mesh_gen_onoff_srv_t *srv;
    BT_WARN("event 0x%02x, opcode 0x%04lx, src 0x%04x, dst 0x%04x\n",
        event, param->ctx.recv_op, param->ctx.addr, param->ctx.recv_dst);

    switch (event) {
    case BFL_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT:{
        BT_WARN("BFL_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT\n");
        switch(param->ctx.recv_op) {
        case BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET:
        case BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK:{
            BT_WARN("GEN_ONOFF 0x%02x\n", param->value.state_change.onoff_set.onoff);
            /* Generic OnPowerUp */
            /* Generic Power Actual Binding with the Generic OnOff state */
            if(param->value.state_change.onoff_set.onoff){
                gpl_srv->state->power_actual = gpl_srv->state->power_default ?
                    gpl_srv->state->power_default : gpl_srv->state->power_last;
            }
            else{
                gpl_srv->state->power_actual = 0;
            }
            gen_power_level_publish(gpl_m, BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS);

            gl_srv->state.level = gpl_srv->state->power_actual - 32768;
            gen_level_publish(gl_m);

            /* Light Lightness Actual */
            if(param->value.state_change.onoff_set.onoff){
                lln_srv->state->lightness_actual = lln_srv->state->lightness_default ?
                    lln_srv->state->lightness_default : lln_srv->state->lightness_last;
            }
            else{
                lln_srv->state->lightness_actual = 0;
            }
            lln_srv->state->lightness_linear = bt_mesh_convert_lightness_actual_to_linear(lln_srv->state->lightness_actual);
            light_lightness_publish(lln_m, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS);
            gl_srv->state.level = lln_srv->state->lightness_actual - 32768;
            gen_level_publish(gl_m);
       }
        break;
        case BFL_BLE_MESH_MODEL_OP_GEN_LEVEL_SET:
        case BFL_BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK:
            BT_WARN("GEN_LEVEL_SET 0x%02x\n", param->value.state_change.level_set.level);
            /* Binding with the Generic Level state, Generic Power Actual = Generic Level + 32768 */
            gpl_srv->state->power_actual = param->value.state_change.level_set.level + 32768;
            gen_power_level_publish(gpl_m, BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS);
            goo_srv->state.onoff = gpl_srv->state->power_actual ? 1 : 0;
            gen_onoff_publish(goo_m);

            /* Light Lightness Actual = Generic Level + 32768 */
            lln_srv->state->lightness_actual = param->value.state_change.level_set.level + 32768;
            lln_srv->state->lightness_linear = bt_mesh_convert_lightness_actual_to_linear(lln_srv->state->lightness_actual);
            light_lightness_publish(lln_m, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS);
            goo_srv->state.onoff = lln_srv->state->lightness_actual ? 1 : 0;
            gen_onoff_publish(goo_m);

            lctl_srv->state->temperature = bt_mesh_covert_gen_level_to_temperature(
                param->value.state_change.level_set.level, lctl_srv->state->temperature_range_min,
                lctl_srv->state->temperature_range_max);
            light_ctl_publish(lln_m, BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS);
            /* Light HSL Hue, Binding with the Generic Level state */
            lhsl_srv->state->hue = param->value.state_change.level_set.level + 32768;
            light_hsl_publish(lhsl_m, BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS);
            /* Light HSL Saturation, Binding with the Generic Level state */
            lhsl_srv->state->saturation = param->value.state_change.level_set.level + 32768;
            gen_level_publish(gl_m);
        break;
        case BFL_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET:
        case BFL_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET_UNACK:
            BT_WARN("GEN_ONPOWERUP_SET 0x%02x\n", param->value.state_change.onpowerup_set.onpowerup);
            ef_set_env_blob("onpowerup", &param->value.state_change.onpowerup_set.onpowerup,
                sizeof(param->value.state_change.onpowerup_set.onpowerup));
        break;
        case BFL_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET:
        case BFL_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK:{
            BT_WARN("GEN_POWER_LEVEL_SET 0x%02x\n", param->value.state_change.power_level_set.power);
            /* Binding with the Generic Level state, Generic Level = Generic Power Actual – 32768 */
            gl_srv->state.level = param->value.state_change.power_level_set.power - 32768;
            gen_level_publish(gl_m);
            /* Binding with the Generic Level state, Binding with the Generic OnOff state */
            goo_srv->state.onoff = param->value.state_change.power_level_set.power ? 1 : 0;
            gen_onoff_publish(goo_m);
        }
        break;
        case BFL_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET:
        case BFL_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET_UNACK:{
            BT_WARN("GEN_POWER_RANGE_SET [%x][%x]\n", param->value.state_change.power_range_set.range_min,
                    param->value.state_change.power_range_set.range_max);
        }
        break;
        case BFL_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET:
        case BFL_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET_UNACK:{
            BT_WARN("GEN_POWER_DEFAULT_SET [%x]\n", param->value.state_change.power_default_set.power);
        }
        break;
        case BFL_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET:
        case BFL_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET_UNACK:{
            BT_WARN("GEN_DEF_TRANS_TIME_SET [%x]\n", param->value.state_change.def_trans_time_set.trans_time);
        }
        break;
        }

    }break;
    /* for auto pts test */
    case BFL_BLE_MESH_GENERIC_SERVER_RECV_SET_MSG_EVT:{
        switch(param->ctx.recv_op) {
        case BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET:
        case BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK:{
            BT_WARN("RECV_SET GEN_ONOFF 0x%02x\n", goo_srv->state.target_onoff);
        }break;
        case BFL_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET:
        case BFL_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK:{
            BT_WARN("RECV_SET GEN_POWER_LEVEL_SET\n");
        }break;
        }
    }
        break;
    default:
        BT_WARN( "Unknown Generic Server event 0x%02x\n", event);
        break;
    }

    ef_set_env_blob("gpl_state", gpl_srv->state,
            sizeof(*gpl_srv->state));
    //BT_WARN("gpl_srv->state [%x][%x][%x][%x][%x]", gpl_srv->state->power_actual,
    //    gpl_srv->state->power_default, gpl_srv->state->power_last,
    //    gpl_srv->state->power_range_min, gpl_srv->state->power_range_max);
    ef_set_env_blob("target_onoff", &goo_srv->state.target_onoff,
                    sizeof(goo_srv->state.target_onoff));
}

static void mmdl_lighting_server_cb(bfl_ble_mesh_lighting_server_cb_event_t event,
           bfl_ble_mesh_lighting_server_cb_param_t *param)
{
    const struct bt_mesh_comp *mesh_comp = bt_mesh_comp_get();
    struct bt_mesh_model *lln_m,*gl_m,*goo_m,*lctl_m,*lhsl_m,*lxyl_m;
    bfl_ble_mesh_light_lightness_srv_t* lln_srv;
    bfl_ble_mesh_light_ctl_srv_t* lctl_srv;
    bfl_ble_mesh_light_hsl_srv_t* lhsl_srv;
    bfl_ble_mesh_light_xyl_srv_t* lxyl_srv;
    bfl_ble_mesh_gen_level_srv_t* gl_srv;
    bfl_ble_mesh_gen_onoff_srv_t* goo_srv;

    lln_m = bt_mesh_model_find(&mesh_comp->elem[0], BFL_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV);
    lln_srv = (bfl_ble_mesh_light_lightness_srv_t*)lln_m->user_data;
    lctl_m = bt_mesh_model_find(&mesh_comp->elem[0], BFL_BLE_MESH_MODEL_ID_LIGHT_CTL_SRV);
    lctl_srv = (bfl_ble_mesh_light_ctl_srv_t*)lctl_m->user_data;
    lhsl_m = bt_mesh_model_find(&mesh_comp->elem[0], BFL_BLE_MESH_MODEL_ID_LIGHT_HSL_SRV);
    lhsl_srv = (bfl_ble_mesh_light_hsl_srv_t*)lhsl_m->user_data;
    lxyl_m = bt_mesh_model_find(&mesh_comp->elem[0], BFL_BLE_MESH_MODEL_ID_LIGHT_XYL_SRV);
    lxyl_srv = (bfl_ble_mesh_light_xyl_srv_t*)lxyl_m->user_data;
    gl_m = bt_mesh_model_find(&mesh_comp->elem[0], BFL_BLE_MESH_MODEL_ID_GEN_LEVEL_SRV);
    gl_srv = (bfl_ble_mesh_gen_level_srv_t*)gl_m->user_data;
    goo_m = bt_mesh_model_find(&mesh_comp->elem[0], BFL_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV);
    goo_srv = (bfl_ble_mesh_gen_onoff_srv_t*)goo_m->user_data;

    BT_WARN("event 0x%02x, opcode 0x%04lx, src 0x%04x, dst 0x%04x\n",
        event, param->ctx.recv_op, param->ctx.addr, param->ctx.recv_dst);

    switch (event) {
    case BFL_BLE_MESH_LIGHTING_SERVER_STATE_CHANGE_EVT:
        BT_WARN("BFL_BLE_MESH_LIGHTING_SERVER_STATE_CHANGE_EVT\n");
        if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK) {
            BT_WARN("LIGHT_LIGHTNESS_SET [%x]\n", param->value.state_change.lightness_set.lightness);
            lln_srv->state->lightness_linear = bt_mesh_convert_lightness_actual_to_linear(param->value.state_change.lightness_set.lightness);

            /* Light Lightness Actual Generic Level = Light Lightness Actual – 32768 */
            gl_srv->state.level = param->value.state_change.lightness_set.lightness - 32768;
            gen_level_publish(gl_m);
            goo_srv->state.onoff = lln_srv->state->lightness_actual ? 1 : 0;
            gen_onoff_publish(goo_m);

            /* Light CTL Lightness,  Binding with the Light Lightness Actual state */
            lctl_srv->state->lightness = param->value.state_change.lightness_set.lightness;
            light_ctl_publish(lctl_m, BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS);

            /* Light HSL Lightness, Binding with the Light Lightness Actual state */
            lhsl_srv->state->lightness = param->value.state_change.lightness_set.lightness;
            light_hsl_publish(lhsl_m, BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS);
        }
        else if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK) {
            BT_WARN("LIGHT_LIGHTNESS_LINEAR_SET [%x]\n", param->value.state_change.lightness_linear_set.lightness);
            /* Light Lightness Actual */
            lln_srv->state->lightness_actual = bt_mesh_convert_lightness_linear_to_actual(param->value.state_change.lightness_linear_set.lightness);
            gl_srv->state.level = lln_srv->state->lightness_actual - 32768;
            gen_level_publish(gl_m);
            goo_srv->state.onoff = lln_srv->state->lightness_actual ? 1 : 0;
            gen_onoff_publish(goo_m);
        }
        else if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK) {
            BT_WARN("LIGHT_CTL_SET [%x][%x][%x]\n",
                param->value.state_change.ctl_set.lightness,
                param->value.state_change.ctl_set.temperature,
                param->value.state_change.ctl_set.delta_uv);
            /* Light CTL Lightness,  Binding with the Light Lightness Actual state */
            lln_srv->state->lightness_actual = param->value.state_change.ctl_set.lightness;
            light_lightness_publish(lln_m, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS);

            gl_srv->state.level = bt_mesh_convert_temperature_to_gen_level(
                param->value.state_change.ctl_set.temperature, lctl_srv->state->temperature_range_min,
                lctl_srv->state->temperature_range_max);
            gen_level_publish(gl_m);
        }
        else if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET_UNACK) {
            BT_WARN("LIGHT_CTL_TEMPERATURE_SET [%x][%x]\n", 
                param->value.state_change.ctl_temp_set.temperature,
                param->value.state_change.ctl_temp_set.delta_uv);
            gl_srv->state.level = bt_mesh_convert_temperature_to_gen_level(
                param->value.state_change.ctl_temp_set.temperature, lctl_srv->state->temperature_range_min,
                lctl_srv->state->temperature_range_max);
            BT_WARN("LIGHT_CTL_TEMPERATURE_SET calc [%x][%x][%x]", gl_srv->state.level,
                lctl_srv->state->temperature_range_min, lctl_srv->state->temperature_range_max);
            gen_level_publish(gl_m);
        }
        else if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET_UNACK) {
            BT_WARN("LIGHT_CTL_DEFAULT_SET [%x][%x][%x]\n",
                param->value.state_change.ctl_default_set.lightness,
                param->value.state_change.ctl_default_set.temperature,
                param->value.state_change.ctl_default_set.delta_uv);
        }
        else if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK) {
            BT_WARN("LIGHT_CTL_TEMPERATURE_RANGE_SET [%x][%x]\n",
                param->value.state_change.ctl_temp_range_set.range_min,
                param->value.state_change.ctl_temp_range_set.range_max);
        }
        else if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK) {
            BT_WARN("LIGHT_HSL_SET [%x][%x][%x]\n",
                param->value.state_change.hsl_set.lightness,
                param->value.state_change.hsl_set.hue,
                param->value.state_change.hsl_set.saturation);
            /* Light HSL Lightness, Binding with the Light Lightness Actual state */
            lln_srv->state->lightness_actual = param->value.state_change.hsl_set.lightness;
            light_lightness_publish(lln_m, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS);
            /* Light HSL Hue, Binding with the Generic Level state */
            gl_srv->state.level = param->value.state_change.hsl_set.hue - 32768;
            gen_level_publish(gl_m);
            /* Light HSL Saturation, Binding with the Generic Level state */
            gl_srv->state.level = param->value.state_change.hsl_set.saturation - 32768;
            gen_level_publish(gl_m);

            /* Light xyL Lightness, Binding with the Light HSL state */
            lxyl_srv->state->lightness = param->value.state_change.hsl_set.lightness;
            light_xyl_publish(lxyl_m, BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS);
        }
        else if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET_UNACK) {
            BT_WARN("LIGHT_HSL_HUE_SET [%x]\n",
                param->value.state_change.hsl_hue_set.hue);
            /* Light HSL Hue, Binding with the Generic Level state */
            gl_srv->state.level = param->value.state_change.hsl_hue_set.hue - 32768;
            gen_level_publish(gl_m);
        }
        else if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET_UNACK) {
            BT_WARN("LIGHT_HSL_SATURATION_SET [%x]\n",
                param->value.state_change.hsl_saturation_set.saturation);
            /* Light HSL Saturation, Binding with the Generic Level state */
            gl_srv->state.level = param->value.state_change.hsl_saturation_set.saturation - 32768;
            gen_level_publish(gl_m);
        }
        else if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET_UNACK) {
            BT_WARN("LIGHT_HSL_DEFAULT_SET [%x][%x][%x]\n",
                param->value.state_change.hsl_default_set.lightness,
                param->value.state_change.hsl_default_set.hue,
                param->value.state_change.hsl_default_set.saturation);
        }
        else if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET_UNACK) {
            BT_WARN("LIGHT_HSL_RANGE_SET [%x][%x][%x][%x]\n",
                param->value.state_change.hsl_range_set.hue_range_min,
                param->value.state_change.hsl_range_set.hue_range_max,
                param->value.state_change.hsl_range_set.saturation_range_min,
                 param->value.state_change.hsl_range_set.saturation_range_max);
        }
        else if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_XYL_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_XYL_SET_UNACK) {
            BT_WARN("LIGHT_XYL_SET [%x][%x][%x]\n",
                param->value.state_change.xyl_set.lightness,
                param->value.state_change.xyl_set.x,
                param->value.state_change.xyl_set.y);
            /* Light xyL Lightness, Binding with the Light HSL state */
            lhsl_srv->state->lightness = param->value.state_change.xyl_set.lightness;
            light_hsl_publish(lhsl_m, BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS);
        }
        else if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET_UNACK) {
            BT_WARN("LIGHT_XYL_DEFAULT_SET [%x][%x][%x]\n",
                param->value.state_change.xyl_default_set.lightness,
                param->value.state_change.xyl_default_set.x,
                param->value.state_change.xyl_default_set.y);
            /* Binding with the Light HSL state */
            lhsl_srv->state->lightness = param->value.state_change.xyl_set.lightness;
            light_hsl_publish(lhsl_m, BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS);
        }
        else if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET_UNACK) {
            BT_WARN("LIGHT_XYL_RANGE_SET [%x][%x][%x][%x]\n",
                param->value.state_change.xyl_range_set.x_range_min,
                param->value.state_change.xyl_range_set.x_range_max,
                param->value.state_change.xyl_range_set.y_range_min,
                param->value.state_change.xyl_range_set.y_range_max);
            /* Binding with the Light HSL state */
            lhsl_srv->state->lightness = param->value.state_change.xyl_set.lightness;
            light_hsl_publish(lhsl_m, BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS);
        }
        else if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET_UNACK) {
            BT_WARN("LIGHT_LC_OM_SET [%x]\n",
                param->value.state_change.lc_om_set.mode);
            //state_machine_om(&lc_srv->lc->state_machine.state,
            //    param->value.state_change.lc_om_set.mode);
        }
        else if (param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET ||
            param->ctx.recv_op == BFL_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET_UNACK) {
            BT_WARN("LIGHT_LC_PROPERTY_SET [%x] %s\n",
                param->value.state_change.lc_property_set.property_id, 
                bt_hex(param->value.state_change.lc_property_set.property_value->data,
                param->value.state_change.lc_property_set.property_value->len));
        }
        break;
    default:
        BT_WARN( "Unknown Server event opcode[%lx] 0x%02x", param->ctx.recv_op, event);
        break;
    }
    ef_set_env_blob("lln_state", lln_srv->state,
            sizeof(*lln_srv->state));
    //BT_WARN("lln_state [%x][%x][%x][%x][%x]", lln_srv->state->lightness_actual,
    //    lln_srv->state->lightness_default, lln_srv->state->lightness_last,
    //    lln_srv->state->lightness_range_min, lln_srv->state->lightness_range_max);
    ef_set_env_blob("lctl_state", lctl_srv->state,
            sizeof(*lctl_srv->state));
    //BT_WARN("lctl_state [%x][%x][%x][%x][%x][%x]", lctl_srv->state->temperature,
    //        lctl_srv->state->target_temperature, lctl_srv->state->temperature_default,
    //        lctl_srv->state->delta_uv, lctl_srv->state->target_delta_uv,
    //        lctl_srv->state->target_delta_uv);
    ef_set_env_blob("lhsl_state", lhsl_srv->state,
           sizeof(*lhsl_srv->state));
    //BT_WARN("lhsl_state [%x][%x][%x][%x][%x][%x][%x][%x][%x]", lhsl_srv->state->saturation,
    //        lhsl_srv->state->saturation_default, lhsl_srv->state->target_saturation,
    //        lhsl_srv->state->hue, lhsl_srv->state->hue_default,
    //        lhsl_srv->state->target_hue, lhsl_srv->state->lightness,
    //        lhsl_srv->state->lightness_default, lhsl_srv->state->target_lightness);
    ef_set_env_blob("lxyl_state", lxyl_srv->state,
           sizeof(*lxyl_srv->state));
    //BT_WARN("lxyl_state [%x][%x][%x][%x][%x][%x][%x][%x][%x]", lxyl_srv->state->x,
    //        lxyl_srv->state->x_default, lxyl_srv->state->target_x,
    //        lxyl_srv->state->y, lxyl_srv->state->y_default,
    //        lxyl_srv->state->target_y, lxyl_srv->state->lightness,
    //        lxyl_srv->state->lightness_default, lxyl_srv->state->target_lightness);
    //BT_WARN("lc_state [%x][%x][%x][%x][%x][%x][%x]", lc_srv->lc->state.mode,
    //        lc_srv->lc->state.occupancy, lc_srv->lc->state.occupancy_mode,
    //        lc_srv->lc->state.light_onoff, lc_srv->lc->state.target_light_onoff,
    //        lc_srv->lc->state.ambient_luxlevel, lc_srv->lc->state.linear_output);
    #if 1
    //ef_set_env_blob("lc_state_machine", &lc_srv->lc->state_machine.state,
    //               sizeof(lc_srv->lc->state_machine.state));
    //        BT_WARN("lc_state_machine [%x]", lc_srv->lc->state_machine.state);
    #endif
}

static void mmdl_ready(void)
{
    const struct bt_mesh_comp *mesh_comp = bt_mesh_comp_get();
    struct bt_mesh_model *gpl_m,*goo_m,*gl_m,*gpo_m,*lln_m,*lctl_m,*lhsl_m,*lxyl_m,*lc_m;
    bfl_ble_mesh_gen_power_level_srv_t* gpl_srv;
    bfl_ble_mesh_gen_onoff_srv_t* goo_srv;
    bfl_ble_mesh_gen_level_srv_t* gl_srv;
    bfl_ble_mesh_gen_power_onoff_srv_t* gpo_srv;
    bfl_ble_mesh_light_lightness_srv_t* lln_srv;
    bfl_ble_mesh_light_ctl_srv_t* lctl_srv;
    bfl_ble_mesh_light_hsl_srv_t* lhsl_srv;
    bfl_ble_mesh_light_xyl_srv_t* lxyl_srv;
    bfl_ble_mesh_light_lc_srv_t* lc_srv;

    gpl_m = bt_mesh_model_find(&mesh_comp->elem[0], BFL_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV);
    gpl_srv = (bfl_ble_mesh_gen_power_level_srv_t*)gpl_m->user_data;
    goo_m = bt_mesh_model_find(&mesh_comp->elem[0], BFL_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV);
    goo_srv = (bfl_ble_mesh_gen_onoff_srv_t*)goo_m->user_data;
    gl_m = bt_mesh_model_find(&mesh_comp->elem[0], BFL_BLE_MESH_MODEL_ID_GEN_LEVEL_SRV);
    gl_srv = (bfl_ble_mesh_gen_level_srv_t*)gl_m->user_data;
    gpo_m = bt_mesh_model_find(&mesh_comp->elem[0], BFL_BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV);
    gpo_srv = (bfl_ble_mesh_gen_power_onoff_srv_t*)gpo_m->user_data;

    lln_m = bt_mesh_model_find(&mesh_comp->elem[0], BFL_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV);
    lln_srv = (bfl_ble_mesh_light_lightness_srv_t*)lln_m->user_data;
    lctl_m = bt_mesh_model_find(&mesh_comp->elem[0], BFL_BLE_MESH_MODEL_ID_LIGHT_CTL_SRV);
    lctl_srv = (bfl_ble_mesh_light_lightness_srv_t*)lctl_m->user_data;
    lhsl_m = bt_mesh_model_find(&mesh_comp->elem[0], BFL_BLE_MESH_MODEL_ID_LIGHT_HSL_SRV);
    lhsl_srv = (bfl_ble_mesh_light_hsl_srv_t*)lhsl_m->user_data;
    lxyl_m = bt_mesh_model_find(&mesh_comp->elem[0], BFL_BLE_MESH_MODEL_ID_LIGHT_XYL_SRV);
    lxyl_srv = (bfl_ble_mesh_light_xyl_srv_t*)lxyl_m->user_data;

    ef_get_env_blob("onpowerup", &gpo_srv->state->onpowerup,
        sizeof(gpo_srv->state->onpowerup), NULL);
    BT_WARN("onpowerup_state.onpowerup = %d", gpo_srv->state->onpowerup);

    ef_get_env_blob("gpl_state", gpl_srv->state,
            sizeof(*gpl_srv->state), NULL);
    BT_WARN("gpl_state [%x][%x][%x][%x][%x]", gpl_srv->state->power_actual,
            gpl_srv->state->power_default, gpl_srv->state->power_last,
            gpl_srv->state->power_range_min, gpl_srv->state->power_range_max);

    ef_get_env_blob("lln_state", lln_srv->state,
            sizeof(*lln_srv->state), NULL);
    BT_WARN("lln_state [%x][%x][%x][%x][%x]", lln_srv->state->lightness_linear,
        lln_srv->state->lightness_default, lln_srv->state->lightness_last,
        lln_srv->state->lightness_range_min, lln_srv->state->lightness_range_max);

    ef_get_env_blob("lctl_state", lctl_srv->state,
           sizeof(*lctl_srv->state), NULL);
    BT_WARN("lctl_state [%x][%x][%x][%x][%x][%x]", lctl_srv->state->temperature,
            lctl_srv->state->target_temperature, lctl_srv->state->temperature_default,
            lctl_srv->state->delta_uv, lctl_srv->state->target_delta_uv,
            lctl_srv->state->target_delta_uv);
    ef_get_env_blob("lhsl_state", lhsl_srv->state,
           sizeof(*lhsl_srv->state), NULL);
    BT_WARN("lhsl_state [%x][%x][%x][%x][%x][%x][%x][%x][%x]", lhsl_srv->state->saturation,
            lhsl_srv->state->saturation_default, lhsl_srv->state->target_saturation,
            lhsl_srv->state->hue, lhsl_srv->state->hue_default,
            lhsl_srv->state->target_hue, lhsl_srv->state->lightness,
            lhsl_srv->state->lightness_default, lhsl_srv->state->target_lightness);
    ef_get_env_blob("lxyl_state", lxyl_srv->state,
           sizeof(*lxyl_srv->state), NULL);
    BT_WARN("lxyl_state [%x][%x][%x][%x][%x][%x][%x][%x][%x]", lxyl_srv->state->x,
            lxyl_srv->state->x_default, lxyl_srv->state->target_x,
            lxyl_srv->state->y, lxyl_srv->state->y_default,
            lxyl_srv->state->target_y, lxyl_srv->state->lightness,
            lxyl_srv->state->lightness_default, lxyl_srv->state->target_lightness);

    if(gpo_srv->state->onpowerup > 2){
        gpo_srv->state->onpowerup = 0;
    }
    if(gpo_srv->state->onpowerup == 0){
        goo_srv->state.onoff = 0;
        gpl_srv->state->power_actual = 0;
        lln_srv->state->lightness_actual = 0;
        /* Light CTL Temperature */
        lctl_srv->state->temperature = lctl_srv->state->temperature_default;
        /* Light CTL Delta UV */
        lctl_srv->state->delta_uv = lctl_srv->state->delta_uv_default;
        //lctl_srv->state->lightness = lln_srv->state->lightness_actual;

        /* Light HSL Saturation */
        lhsl_srv->state->saturation = lhsl_srv->state->saturation_default;
        /* Light HSL Hue */
        lhsl_srv->state->hue = lhsl_srv->state->hue_default;
        lhsl_srv->state->lightness = 0;

        /* Light xyL x */
        lxyl_srv->state->x = lxyl_srv->state->x_default;
        /* Light xyL y */
        lxyl_srv->state->y = lxyl_srv->state->y_default;
        lxyl_srv->state->lightness = 0;

    }
    else if(gpo_srv->state->onpowerup == 1){
        goo_srv->state.onoff = 1;
        gpl_srv->state->power_actual = gpl_srv->state->power_default ?
            gpl_srv->state->power_default : gpl_srv->state->power_last;
        lln_srv->state->lightness_actual = lln_srv->state->lightness_default ?
            lln_srv->state->lightness_default : lln_srv->state->lightness_last;
        /* Light CTL Temperature */
        lctl_srv->state->temperature = lctl_srv->state->temperature_default;
        /* Light CTL Delta UV */
        lctl_srv->state->delta_uv = lctl_srv->state->delta_uv_default;
        //lctl_srv->state->lightness = lln_srv->state->lightness_actual;

        /* Light HSL Saturation */
        lhsl_srv->state->saturation = lhsl_srv->state->saturation_default;
        /* Light HSL Hue */
        lhsl_srv->state->hue = lhsl_srv->state->hue_default;
        //TODO
        lhsl_srv->state->lightness = lhsl_srv->state->lightness_default ?
            lhsl_srv->state->lightness_default : 0;

        /* Light xyL x */
        lxyl_srv->state->x = lxyl_srv->state->x_default;
        /* Light xyL y */
        lxyl_srv->state->y = lxyl_srv->state->y_default;
        //TODO
        lxyl_srv->state->lightness = lxyl_srv->state->lightness_default ?
            lxyl_srv->state->lightness_default : 0;

    }
    else if(gpo_srv->state->onpowerup == 2){
        ef_get_env_blob("target_onoff", &goo_srv->state.onoff,
            sizeof(goo_srv->state.onoff), NULL);
        BT_WARN("onoff_server.state.onoff = %d", goo_srv->state.onoff);
        /*  last known value of the Generic Power Actual state before the node is powered down */
        gpl_srv->state->power_actual = gpl_srv->state->target_power_actual;
        lln_srv->state->lightness_actual = lln_srv->state->target_lightness_actual;
        /* Light CTL Temperature */
        lctl_srv->state->temperature = lctl_srv->state->target_temperature;
        /* Light CTL Delta UV */
        lctl_srv->state->delta_uv = lctl_srv->state->target_delta_uv;
        //lctl_srv->state->lightness = lln_srv->state->lightness_actual;

        /* Light HSL Saturation */
        lhsl_srv->state->saturation = lhsl_srv->state->target_saturation;
        /* Light HSL Hue */
        lhsl_srv->state->hue = lhsl_srv->state->target_hue;
        //TODO
        lhsl_srv->state->lightness = lhsl_srv->state->target_lightness;

        /* Light xyL x */
        lxyl_srv->state->x = lxyl_srv->state->target_x;
        /* Light xyL y */
        lxyl_srv->state->y = lxyl_srv->state->target_y;
        //TODO
        lxyl_srv->state->lightness = lxyl_srv->state->target_lightness;

    }

    gen_power_level_publish(gpl_m, BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS);
    light_lightness_publish(lln_m, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS);
    light_ctl_publish(lctl_m, BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS);
    light_hsl_publish(lhsl_m, BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS);
    light_xyl_publish(lxyl_m, BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS);
}
void mmdl_generic_client_cb(bfl_ble_mesh_generic_client_cb_event_t event,
        bfl_ble_mesh_generic_client_cb_param_t *param)
{
    uint32_t opcode = param->params->opcode;

    BT_WARN("enter %s: event is %d, error code is %d, opcode is 0x%lx\n",
             __func__, event, param->error_code, opcode);

    switch (event) {
    case BFL_BLE_MESH_GENERIC_CLIENT_GET_STATE_EVT: {
        switch (opcode) {
        case BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_GET:{
            bfl_ble_mesh_gen_onoff_status_cb_t* onoff_status;
            onoff_status = &param->status_cb.onoff_status;
            if (param->error_code == BFL_OK) {
                BT_WARN("GEN_ONOFF_GET:OK[%x]\n", onoff_status->present_onoff);
            } else {
                BT_WARN("GEN_ONOFF_GET:Fail[%x]\n", param->error_code);
            }
        }break;
        case BFL_BLE_MESH_MODEL_OP_GEN_LEVEL_GET:{
            bfl_ble_mesh_gen_level_status_cb_t* level_status;
            level_status = &param->status_cb.level_status;
            if (param->error_code == BFL_OK) {
                BT_WARN("GEN_LEVEL_GET:OK[%x]\n", level_status->present_level);
            } else {
                BT_WARN("GEN_LEVEL_GET:Fail[%x]\n", param->error_code);
            }
        }break;
        case BFL_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_GET:{
            bfl_ble_mesh_gen_def_trans_time_status_cb_t* def_trans_time_status;
            def_trans_time_status = &param->status_cb.def_trans_time_status;
            if (param->error_code == BFL_OK) {
                BT_WARN("DEF_TRANS_TIME_GET:OK[%x]\n", def_trans_time_status->trans_time);
            } else {
                BT_WARN("DEF_TRANS_TIME_GET:Fail[%x]\n", param->error_code);
            }
        }break;
        case BFL_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_GET:{
            bfl_ble_mesh_gen_onpowerup_status_cb_t* onpowerup_status;
            onpowerup_status = &param->status_cb.onpowerup_status;
            if (param->error_code == BFL_OK) {
                BT_WARN("GEN_ONPOWERUP_GET:OK[%x]\n", onpowerup_status->onpowerup);
            } else {
                BT_WARN("GEN_ONPOWERUP_GET:Fail[%x]\n", param->error_code);
            }
        }break;
        case BFL_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET:{
            bfl_ble_mesh_gen_power_level_status_cb_t* power_level_status;
            power_level_status = &param->status_cb.power_level_status;
            if (param->error_code == BFL_OK) {
                BT_WARN("GEN_POWER_LEVEL_GET:OK[%x]\n", power_level_status->present_power);
            } else {
                BT_WARN("GEN_POWER_LEVEL_GET:Fail[%x]\n", param->error_code);
            }
        }break;
        case BFL_BLE_MESH_MODEL_OP_GEN_POWER_LAST_GET:{
            bfl_ble_mesh_gen_power_last_status_cb_t* power_last_status;
            power_last_status = &param->status_cb.power_last_status;
            if (param->error_code == BFL_OK) {
                BT_WARN("GEN_POWER_LAST_GET:OK[%x]\n", power_last_status->power);
            } else {
                BT_WARN("GEN_POWER_LAST_GET:Fail[%x]\n", param->error_code);
            }
        }break;
        case BFL_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_GET:{
            bfl_ble_mesh_gen_power_default_status_cb_t* power_default_status;
            power_default_status = &param->status_cb.power_default_status;
            if (param->error_code == BFL_OK) {
                BT_WARN("GEN_POWER_DEFAULT_GET:OK[%x]\n", power_default_status->power);
            } else {
                BT_WARN("GEN_POWER_DEFAULT_GET:Fail[%x]\n", param->error_code);
            }
        }break;
        case BFL_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_GET:{
            bfl_ble_mesh_gen_power_range_status_cb_t* power_range_status;
            power_range_status = &param->status_cb.power_range_status;
            if (param->error_code == BFL_OK) {
                BT_WARN("GEN_POWER_RANGE_GET:OK[%x][%x]\n", power_range_status->range_min, power_range_status->range_max);
            } else {
                BT_WARN("GEN_POWER_RANGE_GET:Fail[%x]\n", param->error_code);
            }
        }break;
        case BFL_BLE_MESH_MODEL_OP_GEN_BATTERY_GET:{
            bfl_ble_mesh_gen_battery_status_cb_t* battery_status;
            battery_status = &param->status_cb.battery_status;
            if (param->error_code == BFL_OK) {
                BT_WARN("GEN_BATTERY_GET:OK[%x][%x][%x][%x]\n", battery_status->battery_level,
                    battery_status->time_to_discharge, battery_status->time_to_charge,
                    battery_status->flags);
            } else {
                BT_WARN("GEN_BATTERY_GET:Fail[%x]\n", param->error_code);
            }
        }break;
        case BFL_BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_GET:{
            bfl_ble_mesh_gen_loc_global_status_cb_t* location_global_status;
            location_global_status = &param->status_cb.location_global_status;
            if (param->error_code == BFL_OK) {
                 BT_WARN("GEN_BATTERY_GET:OK[%lx][%lx][%x]\n", location_global_status->global_latitude,
                    location_global_status->global_longitude, location_global_status->global_altitude);
            } else {
                BT_WARN("GEN_BATTERY_GET:Fail[%x]\n", param->error_code);
            }
        }break;
        case BFL_BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_GET:{
            bfl_ble_mesh_gen_loc_local_status_cb_t* location_local_status;
            location_local_status = &param->status_cb.location_local_status;
            if (param->error_code == BFL_OK) {
                BT_WARN("GEN_BATTERY_GET:OK[%x][%x][%x][%x][%x]\n", location_local_status->local_north,
                    location_local_status->local_east, location_local_status->local_altitude,
                    location_local_status->floor_number, location_local_status->uncertainty);
            } else {
                BT_WARN("GEN_BATTERY_GET:Fail[%x]\n", param->error_code);
            }
        }break;
        case BFL_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTIES_GET:
        case BFL_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_GET:
        case BFL_BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_GET:
        case BFL_BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_GET:
        {
            bfl_ble_mesh_gen_user_properties_status_cb_t* user_properties_status;
            user_properties_status = &param->status_cb.user_properties_status;
            if (param->error_code == BFL_OK) {
                BT_WARN("PROPERTIES_GET:OK[%p]\n", user_properties_status->property_ids);
            } else {
                BT_WARN("PROPERTIES_GET:Fail[%x]\n", param->error_code);
            }
        }break;
        case BFL_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_GET:
        case BFL_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_GET:
        case BFL_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET:
        {
            bfl_ble_mesh_gen_user_property_status_cb_t* user_property_status;
            user_property_status = &param->status_cb.user_property_status;
            if (param->error_code == BFL_OK) {
                BT_WARN("PROPERTIES_GET:OK[%x][%x]\n", user_property_status->property_id, user_property_status->user_access);
            } else {
                BT_WARN("PROPERTIES_GET:Fail[%x]\n", param->error_code);
            }
        }break;
        default:
            break;
        }
        break;
    }
    case BFL_BLE_MESH_GENERIC_CLIENT_SET_STATE_EVT: {
        switch (opcode) {
        case BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET:
            if (param->error_code == BFL_OK) {
                BT_WARN("GEN_ONOFF_SET:OK[%x]\n", param->status_cb.onoff_status.present_onoff);
            } else {
                BT_WARN("GEN_ONOFF_SET:,Fail[%x]\n", param->error_code);
            }
            break;
        case BFL_BLE_MESH_MODEL_OP_GEN_LEVEL_SET:
            if (param->error_code == BFL_OK) {
                BT_WARN("GEN_LEVEL_SET:OK[%x]\n", param->status_cb.level_status.present_level);
            } else {
                BT_WARN("GEN_LEVEL_SET:Fail[%x]\n", param->error_code);
            }
            break;
        case BFL_BLE_MESH_MODEL_OP_GEN_DELTA_SET:
            if (param->error_code == BFL_OK) {
                BT_WARN("GEN_DELTA_SET:OK[%x]\n", param->status_cb.level_status.present_level);
            } else {
                BT_WARN("GEN_DELTA_SET:Fail[%x]\n", param->error_code);
            }
            break;
        case BFL_BLE_MESH_MODEL_OP_GEN_MOVE_SET:
            if (param->error_code == BFL_OK) {
                BT_WARN("GEN_MOVE_SET:OK[%x]\n", param->status_cb.level_status.present_level);
            } else {
                BT_WARN("GEN_MOVE_SET:Fail[%x]\n", param->error_code);
            }
            break;
        case BFL_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET:
            if (param->error_code == BFL_OK) {
                BT_WARN("DEF_TRANS_TIME_SET:OK[%x]\n", param->status_cb.def_trans_time_status.trans_time);
            } else {
                BT_WARN("DEF_TRANS_TIME_SET:Fail[%x]\n", param->error_code);
            }
            break;
        case BFL_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET:
            if (param->error_code == BFL_OK) {
                BT_WARN("GEN_ONPOWERUP_SET:OK[%x]\n", param->status_cb.onpowerup_status.onpowerup);
            } else {
                BT_WARN("GEN_ONPOWERUP_SET:Fail[%x]\n", param->error_code);
            }
            break;
        case BFL_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET:
            if (param->error_code == BFL_OK) {
                BT_WARN("GEN_POWER_LEVEL_SET:OK[%x]\n", param->status_cb.power_level_status.present_power);
            } else {
                BT_WARN("GEN_POWER_LEVEL_SET:Fail[%x]\n", param->error_code);
            }
            break;
        case BFL_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET:
            if (param->error_code == BFL_OK) {
                BT_WARN("GEN_POWER_DEFAULT_SET:OK[%x]\n", param->status_cb.power_default_status.power);
            } else {
                BT_WARN("GEN_POWER_DEFAULT_SET:Fail[%x]\n", param->error_code);
            }
            break;
        case BFL_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET:
            if (param->error_code == BFL_OK) {
                BT_WARN("GEN_POWER_DEFAULT_SET:OK[%x][%x]\n", param->status_cb.power_range_status.range_min, param->status_cb.power_range_status.range_max);
            } else {
                BT_WARN("GEN_POWER_DEFAULT_SET:Fail[%x]\n", param->error_code);
            }
            break;
        case BFL_BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET:
            if (param->error_code == BFL_OK) {
               BT_WARN("GEN_LOC_GLOBAL_SET:OK[%lx][%lx][%x]\n", param->status_cb.location_global_status.global_latitude,
                    param->status_cb.location_global_status.global_longitude, param->status_cb.location_global_status.global_altitude);
            } else {
                BT_WARN("GEN_LOC_GLOBAL_SET:Fail[%x]\n", param->error_code);
            }
            break;
        case BFL_BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET:
            if (param->error_code == BFL_OK) {
                BT_WARN("GEN_LOC_LOCAL_SET:OK[%x][%x][%x][%x][%x]\n", param->status_cb.location_local_status.local_north,
                    param->status_cb.location_local_status.local_east, param->status_cb.location_local_status.local_altitude,
                    param->status_cb.location_local_status.floor_number, param->status_cb.location_local_status.uncertainty);
            } else {
                BT_WARN("GEN_LOC_LOCAL_SET:Fail[%x]\n", param->error_code);
            }
            break;
        case BFL_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_SET:
        case BFL_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET:
        case BFL_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET:
        {
            bfl_ble_mesh_gen_user_property_status_cb_t* user_property_status;
            user_property_status = &param->status_cb.user_property_status;
            if (param->error_code == BFL_OK) {
                BT_WARN("PROPERTIES_GET:OK[%x][%x]\n", user_property_status->property_id, user_property_status->user_access);
            } else {
                BT_WARN("PROPERTIES_GET:Fail[%x]\n", param->error_code);
            }
        }break;
        /* This case maybe delete */
        case BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK:
            if (param->error_code == BFL_OK) {
                 BT_WARN("GenOnOffClient:SetUNACK,OK, opcode[%lx] raddr[%x]\n", 
                                opcode, param->params->ctx.addr);
            } else {
                BT_WARN("GenOnOffClient:SetUNACK,Fail[%x]\n", param->error_code);
            }
            break;
        default:
            break;
        }
        break;
    }
    case BFL_BLE_MESH_GENERIC_CLIENT_PUBLISH_EVT: {
        if (param->error_code == BFL_OK) {
            switch (opcode) {
                case BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS:
                    BT_WARN("Recv onoff status, raddr[%x]\n", 
                                        param->params->ctx.addr);
                break;
                }
        } else {
            BT_WARN("GenOnOffClient:Publish,Fail[%x]\n", param->error_code);
        }
        break;
    }
    case BFL_BLE_MESH_GENERIC_CLIENT_TIMEOUT_EVT:
        BT_WARN("GenOnOffClient:TimeOut[%x]\n", param->error_code);
        break;
    case BFL_BLE_MESH_GENERIC_CLIENT_EVT_MAX:
        BT_WARN("GenONOFFClient:InvalidEvt[%x]\n", param->error_code);
        break;
    default:
        break;
    }
}

void mmdl_light_client_cb(bfl_ble_mesh_light_client_cb_event_t event,
        bfl_ble_mesh_light_client_cb_param_t *param)
{
    uint32_t opcode = param->params->opcode;

    BT_WARN("enter %s: event is %d, error code is %d, opcode is 0x%lx\n",
             __func__, event, param->error_code, opcode);

}
static uint8_t models_callback_init(void)
{
    int err = BFL_OK;
    size_t ret;
    /* For server */
    err = bfl_ble_mesh_register_generic_server_callback(mmdl_generic_server_cb);
    if(err != BFL_OK){
        return err;
    }
    err = bfl_ble_mesh_register_lighting_server_callback(mmdl_lighting_server_cb);
    if(err != BFL_OK){
        return err;
    }

    /* For client */
    err = bfl_ble_mesh_register_generic_client_callback(mmdl_generic_client_cb);
    if(err != BFL_OK){
        return err;
    }
    err = bfl_ble_mesh_register_light_client_callback(mmdl_light_client_cb);
    if(err != BFL_OK){
        return err;
    }

#if defined(MESH_LCTL_BIND_WITH_GENLVL) 	// MMDL/SR/LCTLT/BV-02-C

    struct bt_mesh_model *gl_m,*lctl_m;
    bfl_ble_mesh_gen_level_srv_t* gl_srv;
    bfl_ble_mesh_light_ctl_srv_t* lctl_srv;

    gl_m = bt_mesh_model_find(&elements[0], BFL_BLE_MESH_MODEL_ID_GEN_LEVEL_SRV);
    gl_srv = (bfl_ble_mesh_gen_level_srv_t*)gl_m->user_data;
    lctl_m = bt_mesh_model_find(&elements[0], BFL_BLE_MESH_MODEL_ID_LIGHT_CTL_SRV);
    lctl_srv = (bfl_ble_mesh_light_ctl_srv_t*)lctl_m->user_data;
    lctl_srv->state->temperature_range_min = 0x0320;
    lctl_srv->state->temperature_range_max = 0x4E20;
    lctl_srv->state->temperature = 0x320;
    lctl_srv->state->delta_uv = 0;
    lctl_srv->state->lightness = 0;
    gl_srv->state.level = bt_mesh_convert_temperature_to_gen_level(lctl_srv->state->temperature,
        lctl_srv->state->temperature_range_min, lctl_srv->state->temperature_range_max);

#endif

#if defined(MESH_LHSL_BIND_WITH_GENLVL) //MMDL/SR/LHSLH/BV-02-C

    struct bt_mesh_model *gl_m,*lhsl_m;
    bfl_ble_mesh_gen_level_srv_t* gl_srv;
    bfl_ble_mesh_light_hsl_srv_t* lhsl_srv;
    gl_m = bt_mesh_model_find(&elements[0], BFL_BLE_MESH_MODEL_ID_GEN_LEVEL_SRV);
    gl_srv = (bfl_ble_mesh_gen_level_srv_t*)gl_m->user_data;
    lhsl_m = bt_mesh_model_find(&elements[0], BFL_BLE_MESH_MODEL_ID_LIGHT_HSL_SRV);
    lhsl_srv = (bfl_ble_mesh_light_hsl_srv_t*)lhsl_m->user_data;
    gl_srv->state.level = lhsl_srv->state->hue - 32768;

#endif

#if defined(MESH_LHSLSA_BIND_WITH_GENLVL) //MMDL/SR/LHSLSA/BV-02-C

    struct bt_mesh_model *gl_m,*lhsl_m;
    bfl_ble_mesh_gen_level_srv_t* gl_srv;
    bfl_ble_mesh_light_hsl_srv_t* lhsl_srv;
    gl_m = bt_mesh_model_find(&elements[0], BFL_BLE_MESH_MODEL_ID_GEN_LEVEL_SRV);
    gl_srv = (bfl_ble_mesh_gen_level_srv_t*)gl_m->user_data;
    lhsl_m = bt_mesh_model_find(&elements[0], BFL_BLE_MESH_MODEL_ID_LIGHT_HSL_SRV);
    lhsl_srv = (bfl_ble_mesh_light_hsl_srv_t*)lhsl_m->user_data;
    gl_srv->state.level = lhsl_srv->state->saturation - 32768;

#endif

    return err;
}
#endif

BLEMESH_CLI(init)
{
    int err;

    if(blemesh_inited){
        vOutputString("Has initialized \r\n");
        return;
    }

    // auto generate device uuid
    gen_dev_uuid();
#ifdef CONFIG_BT_MESH_PROVISIONER
	/* set role type as node */
	prov.role = 0;
#endif
    err = bt_mesh_init(&prov, &comp);
    if(err){
        vOutputString("Failed to init \r\n");
        return;
    }

    blemesh_inited = true;
    vOutputString("Init successfully \r\n");

    if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
        mesh_set();
        mesh_commit();
    }

    if (bt_mesh_is_provisioned()) {
		vOutputString("Mesh network restored from flash\r\n");
	} else {
		vOutputString("Use pb-adv or pb-gatt to enable advertising\r\n");
	}

#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
    int i;

    // used for almost all test cases
    vOutputString("[PTS] TSPX_bd_addr_iut: %s\n", 
    				bt_hex(bt_dev.id_addr[0].a.val, 6));

    // used for almost all test cases
    vOutputString("[PTS] TSPX_device_uuid: %s\n", bt_hex(dev_uuid, 16));

	 // used for almost all test cases
    vOutputString("[PTS] TSPX_device_auth: %s\n", bt_hex(prov.static_val, prov.static_val_len));

    // used for test case MESH/NODE/RLY/BV-02-C
    vOutputString("[PTS] TSPX_maximum_network_message_cache_entries: %d\r\n", 
    				CONFIG_BT_MESH_MSG_CACHE_SIZE);

    // used for test case MESH/NODE/CFG/CFGR/BV-01-C
    vOutputString("[PTS] TSPX_iut_supports_relay: %s\r\n", 
    				cfg_srv.relay == BT_MESH_RELAY_ENABLED ? "TRUE" : "FALSE");

    // used for test case MESH/NODE/CFG/SL/BV-03-C
    vOutputString("[PTS] TSPX_vendor_model_id: %04X%04X\r\n", 
    				vendor_models[0].vnd.company, vendor_models[0].vnd.id);
#endif

#if IS_ENABLED(CONFIG_BT_MESH_LOW_POWER)
	bt_mesh_lpn_set_cb(lpn_cb);
#endif
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
#ifndef CONFIG_MESH_IOPT_BV_02_C
        mmdl_ready();
	models_callback_init();
#endif
#endif
}

#if defined(CONFIG_BT_MESH_PROVISIONER)
static const u16_t net_idx = BT_MESH_NET_PRIMARY;
static const u16_t app_idx = BT_MESH_APP_PRIMARY;

static u16_t self_addr = 1;
static void setup_cdb(void)
{
	struct bt_mesh_cdb_app_key *key;

	key = bt_mesh_cdb_app_key_alloc(net_idx, app_idx);
	if (key == NULL) {
		vOutputString("Failed to allocate app-key\n");
		return;
	}

	bt_rand(key->keys[0].app_key, 16);

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_cdb_app_key_store(key);
	}
}

BLEMESH_CLI(pvnr_init)
{
	u8_t net_key[16], dev_key[16];
	int err;

	if(blemesh_inited){
        vOutputString("Has initialized \r\n");
        return;
    }

#ifdef CONFIG_BT_MESH_PROVISIONER
	/* set role type as provisioner */
	prov.role = 1;
#endif

	err = bt_mesh_init(&prov, &comp);
	if (err) {
		vOutputString("Initializing mesh failed (err %d)\n", err);
		return;
	}
	blemesh_inited = true;
	printk("Mesh initialized\n");

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		vOutputString("Loading stored settings\n");
		mesh_set();
		mesh_commit();
	}

	bt_rand(net_key, 16);

	err = bt_mesh_cdb_create(net_key);
	if (err == -EALREADY) {
		vOutputString("Using stored CDB\n");
	} else if (err) {
		vOutputString("Failed to create CDB (err %d)\n", err);
		return;
	} else {
		vOutputString("Created CDB\n");
		setup_cdb();
	}

	bt_rand(dev_key, 16);

	err = bt_mesh_provision(net_key, BT_MESH_NET_PRIMARY, 0, 0, self_addr,
				dev_key);
	if (err == -EALREADY) {
		vOutputString("Using stored settings\n");
	} else if (err) {
		vOutputString("Provisioning failed (err %d)\n", err);
		return;
	} else {
		vOutputString("Network key:%s\n", bt_hex(net_key, 16));
		vOutputString("Dev key:%s\n", bt_hex(dev_key, 16));
		vOutputString("Provisioning completed\n");

		//Do local APP KEY Add
		if (IS_ENABLED(CONFIG_BT_MESH_CDB)) {
			struct bt_mesh_cdb_app_key *app_key;
			
			app_key = bt_mesh_cdb_app_key_get(app_idx);
			if (!app_key) {
				return;
			}
			u8_t status;
			err = bt_mesh_cfg_app_key_add(net_idx, self_addr, net_idx,
						app_idx, app_key->keys[0].app_key, &status);
			if (err) {
				vOutputString("Unable to send App Key Add (err %d)", err);
				return;
			}
		}
		//Do local model bound.
		bt_mesh_local_model_bind(net_idx, app_idx);
	}

	return;
}
#endif

BLEMESH_CLI(addr_get)
{
	extern u16_t bt_mesh_primary_addr(void);
	if (!bt_mesh_is_provisioned()){
		vOutputString("Node is not provisioned\r\n");
		return;
	}
	vOutputString("Primary address is 0x%04x\r\n", bt_mesh_primary_addr());
}

static const char *bearer2str(bt_mesh_prov_bearer_t bearer)
{
	switch (bearer) {
	case BT_MESH_PROV_ADV:
		return "PB-ADV";
	case BT_MESH_PROV_GATT:
		return "PB-GATT";
	default:
		return "unknown";
	}
}

#if defined(CONFIG_BT_MESH_PROV)
BLEMESH_CLI(pb)
{
    int err;
    uint8_t bearer;
    uint8_t enable;

    if(argc != 3){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
    get_uint8_from_string(&argv[1], &bearer);
    get_uint8_from_string(&argv[2], &enable);
    
	if (enable) {
		err = bt_mesh_prov_enable(bearer);
		if (err) {
			vOutputString("Failed to enable %s (err %d)\r\n", bearer2str(bearer), err);
		} else {
			vOutputString("%s enabled\r\n", bearer2str(bearer));
		}
	} else {
		err = bt_mesh_prov_disable(bearer);
		if (err) {
			vOutputString("Failed to disable %s (err %d)\r\n",
				    bearer2str(bearer), err);
		} else {
			vOutputString("%s disabled\r\n", bearer2str(bearer));
		}
	}
}
#endif

#if defined(CONFIG_BT_MESH_PB_ADV)
#if defined(CONFIG_BT_MESH_PROVISIONER)
BLEMESH_CLI(provision_adv)
{
	u8_t uuid[16];
	u8_t attention_duration;
	u16_t net_idx;
	u16_t addr;
	size_t len;
	int err;
	/* Get UUID Form arg[1] */
	len = hex2bin(argv[1], strlen(argv[1]), uuid, sizeof(uuid));
	(void)memset(uuid + len, 0, sizeof(uuid) - len);

	/* Get Net Idx Form arg[2] */
	net_idx = strtoul(argv[2], NULL, 0);
	/* Get Unicast address Form arg[3] */
	addr = strtoul(argv[3], NULL, 0);
	/* Get Unicast address Form arg[4] */
	attention_duration = strtoul(argv[4], NULL, 0);

	err = bt_mesh_provision_adv(uuid, net_idx, addr, attention_duration);
	if (err) {
		vOutputString("Provisioning failed (err %d)\n", err);
	}

	return;
}
#endif /* CONFIG_BT_MESH_PROVISIONER */
#endif /* CONFIG_BT_MESH_PB_ADV */

static void link_open(bt_mesh_prov_bearer_t bearer)
{
	vOutputString("Provisioning link opened on %s\r\n", bearer2str(bearer));
}

static void link_close(bt_mesh_prov_bearer_t bearer)
{
	vOutputString("Provisioning link closed on %s\r\n", bearer2str(bearer));
}
#ifdef CONFIG_BT_MESH_PROVISIONER
const static char* output_oob_action[5]= {
"Blink","Beep","Vibrate","Output Numeric","Output Alphanumeric",
};

const static char* input_oob_action[4]= {
"Push","Twist","Input Numeric","Input Alphanumeric",
};

static u8_t capabilities(prov_caps_t* prv_caps, prov_start_t* prv_start)
{
	vOutputString(">>>num_ele[%x]\n", prv_caps->num_ele);

	prv_start->algo = 0;
	vOutputString(">>>Provisionee algo[%x], Provisioner algo[%x]\n"
					, prv_caps->algo, prv_start->algo);

	prv_start->pub_key = prv_caps->pub_key_type;
	vOutputString(">>>Provisionee pub_key_type[%x], Provisioner pub_key_type[%x]\n"
					, prv_caps->pub_key_type, prv_start->pub_key);
	if(prv_start->pub_key){
		//vOutputString(">>>Enter Provisionee's public key:\n");
		//char str[64+64+4];
		//read_str(str, sizeof(str)-1);
		//char* pstr = str;
		//get_bytearray_from_string(&pstr, prv_start->pub_key_data, 64);
		
		const u8_t prvee_key[] = {
		0x5B, 0x2A, 0xD8, 0xB0, 0x34, 0xD8, 0x07, 0x43, 0x53, 0x6E, 0x7F, 0x1B, 0x23, 0x54, 0xFE, 0x08,
		0x69, 0xF5, 0x0E, 0x8F, 0x08, 0x5B, 0xEA, 0x04, 0x71, 0x19, 0x59, 0x0F, 0x4E, 0x74, 0xAC, 0x67,
		0xF0, 0x15, 0x1E, 0xFB, 0xC5, 0x31, 0xFB, 0xAB, 0x26, 0x73, 0xB1, 0xD1, 0xAD, 0xF7, 0x89, 0x31,
		0xF4, 0x00, 0xD9, 0x50, 0xE4, 0x66, 0xD0, 0x26, 0xFC, 0x2E, 0x7D, 0x09, 0x0E, 0x2A, 0x1A, 0x0C
		};
		memcpy(&prv_start->pub_key_data, prvee_key, sizeof(prv_start->pub_key_data));
		vOutputString(">>>Recved Provisionee's public key:%s\n"
					, bt_hex(prv_start->pub_key_data, sizeof(prv_start->pub_key_data)));
	}

	vOutputString(">>>Provisionee stc_oob_type: %x\n", prv_caps->stc_oob_type);
	if(prv_caps->stc_oob_type){
		prv_start->auth_method = 0x01;
		prv_start->auth_action = 0;
		prv_start->auth_size = 0;
		return 0;
	}
	if(prv_caps->op_oob_size == 0
		&& prv_caps->ip_oob_size == 0){
		prv_start->auth_method = 0x00;
		prv_start->auth_action = 0;
		prv_start->auth_size = 0;
		return 0;
	}
	
	if(prv_caps->op_oob_size){
		vOutputString(">>>Provisionee op_oob_size: %x\n", prv_caps->op_oob_size);
		vOutputString(">>>Provisionee op_oob_action: %x\n", prv_caps->op_oob_action);
		vOutputString(">>>Enter a number within:");
		int cnt = 0, index = 0;
		for(int i = 0; i < 5; ++i){
			if(prv_caps->op_oob_action&(1<<i)){
				vOutputString("[%d %s] ", i, output_oob_action[i]);
				cnt++;
				index = i;
			}
		}
		vOutputString(":\n");
		if(cnt > 1){
			char str[20];
			read_str(str, sizeof(str)-1);
			prv_start->auth_action = strtoul(str, NULL, 10);
		}
		else{
			prv_start->auth_action = index;
		}
		prv_start->auth_size = prv_caps->op_oob_size;
		prv_start->auth_method = 0x02;
	}

	if(prv_caps->ip_oob_size){
		vOutputString(">>>Provisionee ip_oob_size: %x\n", prv_caps->ip_oob_size);
		vOutputString(">>>Provisionee ip_oob_action: %x\n", prv_caps->ip_oob_action);
		vOutputString(">>>Provisionee Enter a number within:");
		int cnt = 0, index = 0;
		for(int i = 0; i < 4; ++i){
			if(prv_caps->ip_oob_action&(1<<i)){
				vOutputString("[%d %s] ", i, input_oob_action[i]);
				cnt++;
				index = i;
			}
		}
		vOutputString(":\n");
		if(cnt>1){
			char str[20];
			read_str(str, sizeof(str)-1);
			prv_start->auth_action = strtoul(str, NULL, 10);
		}else{
			prv_start->auth_action = index;
		}
		prv_start->auth_size = prv_caps->ip_oob_size;
		prv_start->auth_method = 0x03;
	}
	
	vOutputString(">>>Provisioner auth method[%x] action[%x] size[%x]:\n"
					, prv_start->auth_method, prv_start->auth_action
					, prv_start->auth_size);
	return 0;
}
#endif /*CONFIG_BT_MESH_PROVISIONER*/
static int output_number(bt_mesh_output_action_t action, u32_t number)
{
	vOutputString("OOB Number: %lu\r\n", number);
	return 0;
}

static int output_string(const char *str)
{
	vOutputString("OOB String: %s\r\n", str);
	return 0;
}

static void prov_input_complete(void)
{
	vOutputString("Input complete\r\n");
}

static void prov_complete(u16_t net_idx, u16_t addr)
{
	vOutputString("Local node provisioned, net_idx 0x%04x address 0x%04x\r\n", net_idx, addr);
	net.net_idx = net_idx,
	net.local = addr;
	net.dst = addr;
}

static void prov_reset(void)
{
	vOutputString("The local node has been reset and needs reprovisioning\r\n");
}

static void gen_dev_uuid(void)
{
//device uuid: 0a380000-1111-2222-3333-mac address(6 bytes)

	bt_addr_le_t adv_addr;
	bt_get_local_public_address(&adv_addr);
	u8_t uuid[16] = {0x0a,0x38,0x00,0x00,0x11,0x11,0x22,0x22,0x33,0x33,
	                        adv_addr.a.val[5],
	                        adv_addr.a.val[4],
	                        adv_addr.a.val[3],
	                        adv_addr.a.val[2],
	                        adv_addr.a.val[1],
	                        adv_addr.a.val[0]};
	memcpy(dev_uuid, uuid, 16);

}

BLEMESH_CLI(set_dev_uuid)
{
    if(argc != 2){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    vOutputString("device uuid is %s\r\n", argv[1]);
    get_bytearray_from_string(&argv[1], dev_uuid,16);
}

BLEMESH_CLI(input_num)
{
	int err;
    uint32_t num;

    if(argc != 2){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
     
    num = strtoul(argv[1], NULL, 10);

	if (input_act != BT_MESH_ENTER_NUMBER) {
		vOutputString("A number hasn't been requested!\r\n");
		return;
	}

	if (strlen(argv[1]) < input_size) {
		vOutputString("Too short input (%u digits required)\r\n", input_size);
		return;
	}

	err = bt_mesh_input_number(num);
	if (err) {
		vOutputString("Numeric input failed (err %d)\r\n", err);
		return;
	}

	input_act = BT_MESH_NO_INPUT;
}

BLEMESH_CLI(input_str)
{
	int err;

    if(argc != 2){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

	if (input_act != BT_MESH_ENTER_STRING) {
		vOutputString("A string hasn't been requested!\r\n");
		return;
	}

	if (strlen(argv[1]) < input_size) {
		vOutputString("Too short input (%u characters required)\r\n", input_size);
		return;
	}

	err = bt_mesh_input_string(argv[1]);
	if (err) {
		vOutputString("String input failed (err %d)\r\n", err);
		return;
	}

	input_act = BT_MESH_NO_INPUT;
}

#if defined(CONFIG_BT_MESH_MODEL)
#if defined(CONFIG_BT_MESH_MODEL_GEN_CLI)
void ble_mesh_generic_onoff_client_model_cb(bfl_ble_mesh_generic_client_cb_event_t event,
        bfl_ble_mesh_generic_client_cb_param_t *param)
{
    uint32_t opcode = param->params->opcode;

    vOutputString("enter %s: event is %d, error code is %d, opcode is 0x%lx\n",
             __func__, event, param->error_code, opcode);

    switch (event) {
    case BFL_BLE_MESH_GENERIC_CLIENT_GET_STATE_EVT: {
        switch (opcode) {
        case BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_GET:
            if (param->error_code == BFL_OK) {
                vOutputString("GenOnOffClient:GetStatus,OK[%x]\n", param->status_cb.onoff_status.present_onoff);
            } else {
                vOutputString("GenOnOffClient:GetStatus,Fail[%x]\n", param->error_code);
            }
            break;
        default:
            break;
        }
        break;
    }
    case BFL_BLE_MESH_GENERIC_CLIENT_SET_STATE_EVT: {
        switch (opcode) {
        case BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET:
            if (param->error_code == BFL_OK) {
				/* Add protection for packet loss rate test*/
				unsigned int key = irq_lock();
                vOutputString("GenOnOffClient:SetStatus,OK[%x]\n", param->status_cb.onoff_status.present_onoff);
				irq_unlock(key);
            } else {
                vOutputString("GenOnOffClient:SetStatus,Fail[%x]\n", param->error_code);
            }
            break;
        case BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK:
            if (param->error_code == BFL_OK) {
                vOutputString("GenOnOffClient:SetUNACK,OK, opcode[%lx] raddr[%x]\n",
                                opcode, param->params->ctx.addr);
            } else {
                vOutputString("GenOnOffClient:SetUNACK,Fail[%x]\n", param->error_code);
            }
            break;
        default:
            break;
        }
        break;
    }
    case BFL_BLE_MESH_GENERIC_CLIENT_PUBLISH_EVT: {
        if (param->error_code == BFL_OK) {
            switch (opcode) {
                case BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS:
                    vOutputString("Recv onoff status, raddr[%x]\n",
                                        param->params->ctx.addr);
					#if defined(CONFIG_BT_MESH_TEST)
                    if(nodelist_check(param->params->ctx.addr)){
                        vOutputString("Recv all response\r\n");
                    }
					#endif /* CONFIG_BT_MESH_TEST */
                break;
                }
        } else {
            vOutputString("GenOnOffClient:Publish,Fail[%x]\n", param->error_code);
        }
        break;
    }
    case BFL_BLE_MESH_GENERIC_CLIENT_TIMEOUT_EVT:
        vOutputString("GenOnOffClient:TimeOut[%x]\n", param->error_code);
        break;
    case BFL_BLE_MESH_GENERIC_CLIENT_EVT_MAX:
        vOutputString("GenONOFFClient:InvalidEvt[%x]\n", param->error_code);
        break;
    default:
        break;
    }
    vOutputString("exit %s \n", __func__);
}

BLEMESH_CLI(gen_oo_cli)
{
	int err = BFL_OK;

    if(argc < 1){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    bfl_ble_mesh_generic_client_set_state_t gen_client_set;
    bfl_ble_mesh_generic_client_get_state_t gen_client_get;
    bfl_ble_mesh_client_common_param_t onoff_common = {
        .msg_timeout = 0,
        .ctx.send_ttl = 7,
        .model = NULL,
    };

    vOutputString("enter %s\n", __func__);

	/*Get client model*/
	for(int i = 0; i < sizeof(sig_models)/sizeof(struct bt_mesh_model); ++i){
		if(sig_models[i].id == BFL_BLE_MESH_MODEL_ID_GEN_ONOFF_CLI){
			 onoff_common.model = &sig_models[i];
		}
	}
	if(onoff_common.model == NULL){
		vOutputString("Can't find gen onoff cli %s\n", __func__);
		return;
	}

	if(argc == 7 || argc == 12){
	    /* appkey_index */
	    get_uint16_from_string(&argv[2], &onoff_common.ctx.app_idx);
	    /* opcode */
	    get_uint32_from_string(&argv[3], &onoff_common.opcode);
	    /* role */
	    get_uint8_from_string(&argv[4], &onoff_common.msg_role);
	    /* address */
	    get_uint16_from_string(&argv[5], &onoff_common.ctx.addr);
	    /* network key index */
	    get_uint16_from_string(&argv[6], &onoff_common.ctx.net_idx);

		if(argc == 12){
			/* op_en */
			uint8_t op_en_t;
			get_uint8_from_string(&argv[7], &op_en_t);
			gen_client_set.onoff_set.op_en = op_en_t?true:false;

			/* onoff */
			get_uint8_from_string(&argv[8], &gen_client_set.onoff_set.onoff);
			/* tid */
			get_uint8_from_string(&argv[9], &gen_client_set.onoff_set.tid);
			/* trans_time */
			get_uint8_from_string(&argv[10], &gen_client_set.onoff_set.trans_time);
			/* delay */
			get_uint8_from_string(&argv[11], &gen_client_set.onoff_set.delay);
		}
	}
	else{
		if(argc != 2){
			vOutputString("Number of Parameters is not correct\r\n");
			return;
		}
	}

    if (strcmp(argv[1], "get") == 0) {
        err = bfl_ble_mesh_generic_client_get_state(&onoff_common, &gen_client_get);
    } else if (strcmp(argv[1], "set") == 0) {
		/* Add protection for packet loss rate test*/
		unsigned int key = irq_lock();
		vOutputString("%sSetStart\n", __func__);
		irq_unlock(key);

#if defined(CONFIG_BT_MESH_TEST)
        if(onoff_common.opcode == BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET
            && BT_MESH_ADDR_IS_GROUP(onoff_common.ctx.addr)){
            nodelist_check_clear();
        }
#endif /* CONFIG_BT_MESH_TEST */
        err = bfl_ble_mesh_generic_client_set_state(&onoff_common, &gen_client_set);
    } else if (strcmp(argv[1], "reg") == 0) {
        err = bfl_ble_mesh_register_generic_client_callback(ble_mesh_generic_onoff_client_model_cb);
        if (err == BFL_OK) {
            vOutputString("GenONOFFClient:Reg,OK\n");
        }
    }
    vOutputString("exit %s\n", __func__);
}
#endif

#if defined(CONFIG_BT_MESH_MODEL_LIGHT_CLI)
void ble_mesh_light_client_model_cb(bfl_ble_mesh_light_client_cb_event_t event,
        bfl_ble_mesh_light_client_cb_param_t *param)
{
	uint32_t opcode = param->params->opcode;

    vOutputString("enter %s: event is %d, error code is %d, opcode is 0x%lx\n",
             __func__, event, param->error_code, opcode);

    switch (event) {
    case BFL_BLE_MESH_LIGHT_CLIENT_GET_STATE_EVT: {
        switch (opcode) {
        case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET:
            if (param->error_code == BFL_OK) {
                vOutputString("Light lightness Client:GetStatus,OK, ln[%x]ln_t[%x]\n"
								, param->status_cb.lightness_status.present_lightness
								, param->status_cb.lightness_status.target_lightness);
            } else {
                vOutputString("Light lightness Client:GetStatus,Fail[%x]\n", param->error_code);
            }
            break;
		case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_GET:
            if (param->error_code == BFL_OK) {
                vOutputString("Light ctl Client:GetStatus,OK,ln[%x]ln_t[%x] tp[%x]tp_t[%x]\n",
					param->status_cb.ctl_status.present_ctl_lightness,
					param->status_cb.ctl_status.target_ctl_lightness,
					param->status_cb.ctl_status.present_ctl_temperature,
					param->status_cb.ctl_status.target_ctl_temperature);
            } else {
                vOutputString("Light ctl Client:GetStatus,Fail[%x]\n", param->error_code);
            }
            break;
		case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_GET:
            if (param->error_code == BFL_OK) {
                vOutputString("Light hsl Client:GetStatus,OK,l[%x] h[%x] s[%x]\n",
					param->status_cb.hsl_status.hsl_lightness,
					param->status_cb.hsl_status.hsl_hue,
					param->status_cb.hsl_status.hsl_saturation);
            } else {
                vOutputString("Light hsl Client:GetStatus,Fail[%x]\n", param->error_code);
            }
            break;
        default:
            break;
        }
        break;
    }
    case BFL_BLE_MESH_LIGHT_CLIENT_SET_STATE_EVT: {
        switch (opcode) {
        case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET:
            if (param->error_code == BFL_OK) {
                vOutputString("Light lightness Client:SetStatus,OK, ln[%x]ln_t[%x]\n"
								, param->status_cb.lightness_status.present_lightness
								, param->status_cb.lightness_status.target_lightness);
            } else {
                vOutputString("Light lightness Client:SetStatus,Fail[%x]\n", param->error_code);
            }
            break;
        case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK:
            if (param->error_code == BFL_OK) {
                vOutputString("Light lightness Client:SetUNACK,OK\n");
            } else {
                vOutputString("Light lightness Client:SetUNACK,Fail[%x]\n", param->error_code);
            }
            break;
		case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_SET:
            if (param->error_code == BFL_OK) {
                vOutputString("Light ctl Client:SetStatus,OK, ln[%x]ln_t[%x] tp[%x]tp_t[%x]\n", 
					param->status_cb.ctl_status.present_ctl_lightness,
					param->status_cb.ctl_status.target_ctl_lightness,
					param->status_cb.ctl_status.present_ctl_temperature,
					param->status_cb.ctl_status.target_ctl_temperature);
            } else {
                vOutputString("Light ctl Client:SetStatus,Fail[%x]\n", param->error_code);
            }
            break;
        case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK:
            if (param->error_code == BFL_OK) {
                vOutputString("Light ctl Client:SetUNACK,OK\n");
            } else {
                vOutputString("Light ctl Client:SetUNACK,Fail[%x]\n", param->error_code);
            }
            break;
		case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SET:
            if (param->error_code == BFL_OK) {
                vOutputString("Light hsl Client:SetStatus,OK, l[%x] h[%x] s[%x]\n", 
					param->status_cb.hsl_status.hsl_lightness,
					param->status_cb.hsl_status.hsl_hue,
					param->status_cb.hsl_status.hsl_saturation);
            } else {
                vOutputString("Light ctl Client:SetStatus,Fail[%x]\n", param->error_code);
            }
            break;
        case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK:
            if (param->error_code == BFL_OK) {
                vOutputString("Light hsl Client:SetUNACK,OK\n");
            } else {
                vOutputString("Light hsl Client:SetUNACK,Fail[%x]\n", param->error_code);
            }
            break;
        default:
            break;
        }
        break;
    }
    case BFL_BLE_MESH_LIGHT_CLIENT_PUBLISH_EVT: {
		switch (opcode) {
        case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET:
	        if (param->error_code == BFL_OK) {
	            vOutputString("Light lightness Client:Publish,OK\n");
	        } else {
	            vOutputString("Light lightness Client:Publish,Fail[%x]\n", param->error_code);
	        }
			break;
		case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_GET:
	        if (param->error_code == BFL_OK) {
	            vOutputString("Light ctl Client:Publish,OK\n");
	        } else {
	            vOutputString("Light ctl Client:Publish,Fail[%x]\n", param->error_code);
	        }
			break;
		case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_GET:
	        if (param->error_code == BFL_OK) {
	            vOutputString("Light hsl Client:Publish,OK\n");
	        } else {
	            vOutputString("Light hsl Client:Publish,Fail[%x]\n", param->error_code);
	        }
			break;
		default:
        	break;
		}
		break;
    }
    case BFL_BLE_MESH_LIGHT_CLIENT_TIMEOUT_EVT:{
		switch (opcode) {
        case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET:
		case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET:
		case BFL_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK:
        	vOutputString("Light lightness Client:TimeOut[%x]\n", param->error_code);
			break;
		case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_GET:
		case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_SET:
		case BFL_BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK:
        	vOutputString("Light ctl Client:TimeOut[%x]\n", param->error_code);
			break;
		case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_GET:
		case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SET:
		case BFL_BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK:
        	vOutputString("Light hsl Client:TimeOut[%x]\n", param->error_code);
			break;
		default:
			break;
		}
        break;
    }
    case BFL_BLE_MESH_LIGHT_CLIENT_EVT_MAX:{
        vOutputString("InvalidEvt, Opcode[%lx] [%x]\n", opcode, param->error_code);
        break;
    }
    default:
        break;
    }
    vOutputString("exit %s \n", __func__);
}

BLEMESH_CLI(light_lgn_cli)
{
	int err = BFL_OK;

    if(argc < 1){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    bfl_ble_mesh_light_client_set_state_t light_client_set;
    bfl_ble_mesh_light_client_get_state_t light_client_get;
    bfl_ble_mesh_client_common_param_t lgn_common = {
        .msg_timeout = 0,
        .ctx.send_ttl = 7,
        .model = NULL,
    };

    vOutputString("enter %s\n", __func__);

	/*Get client model*/
	for(int i = 0; i < sizeof(sig_models)/sizeof(struct bt_mesh_model); ++i){
		if(sig_models[i].id == BFL_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI){
			 lgn_common.model = &sig_models[i];
		}
	}
	if(lgn_common.model == NULL){
		vOutputString("Can't find light ligntness cli %s\n", __func__);
		return;
	}

	if(argc > 6){
	    /* appkey_index */
	    get_uint16_from_string(&argv[2], &lgn_common.ctx.app_idx);
	    /* opcode */
	    get_uint32_from_string(&argv[3], &lgn_common.opcode);
	    /* role */
	    get_uint8_from_string(&argv[4], &lgn_common.msg_role);
	    /* address */
	    get_uint16_from_string(&argv[5], &lgn_common.ctx.addr);
	    /* network key index */
	    get_uint16_from_string(&argv[6], &lgn_common.ctx.net_idx);
	}
	if(argc > 11){
	    /* op_en */
	    uint8_t op_en_t;
	    get_uint8_from_string(&argv[7], &op_en_t);
		light_client_set.lightness_set.op_en = op_en_t?true:false;

	    /* lightness */
	    get_uint16_from_string(&argv[8], &light_client_set.lightness_set.lightness);
	    /* tid */
	    get_uint8_from_string(&argv[9], &light_client_set.lightness_set.tid);
	    /* trans_time */
	    get_uint8_from_string(&argv[10], &light_client_set.lightness_set.trans_time);
	    /* delay */
	    get_uint8_from_string(&argv[11], &light_client_set.lightness_set.delay);
	}

    if (strcmp(argv[1], "get") == 0) {
        err = bfl_ble_mesh_light_client_get_state(&lgn_common, &light_client_get);
    } else if (strcmp(argv[1], "set") == 0) {
        err = bfl_ble_mesh_light_client_set_state(&lgn_common, &light_client_set);
    } else if (strcmp(argv[1], "reg") == 0) {
        err = bfl_ble_mesh_register_light_client_callback(ble_mesh_light_client_model_cb);
        if (err == BFL_OK) {
            vOutputString("Light lightness client:Reg,OK\n");
        }
    }
    vOutputString("exit %s\n", __func__);
}

BLEMESH_CLI(light_ctl_cli)
{
	int err = BFL_OK;

	if(argc < 1){
		vOutputString("Number of Parameters is not correct\r\n");
		return;
	}

	bfl_ble_mesh_light_client_set_state_t light_client_set;
	bfl_ble_mesh_light_client_get_state_t light_client_get;
	bfl_ble_mesh_client_common_param_t ctl_common = {
		.msg_timeout = 0,
		.ctx.send_ttl = 7,
		.model = NULL,
	};

	vOutputString("enter %s\n", __func__);

	/*Get client model*/
	for(int i = 0; i < sizeof(sig_models)/sizeof(struct bt_mesh_model); ++i){
		if(sig_models[i].id == BFL_BLE_MESH_MODEL_ID_LIGHT_CTL_CLI){
			 ctl_common.model = &sig_models[i];
		}
	}
	if(ctl_common.model == NULL){
		vOutputString("Can't find light ctl cli %s\n", __func__);
		return;
	}

	if(argc > 6){
		/* appkey_index */
		get_uint16_from_string(&argv[2], &ctl_common.ctx.app_idx);
		/* opcode */
		get_uint32_from_string(&argv[3], &ctl_common.opcode);
		/* role */
		get_uint8_from_string(&argv[4], &ctl_common.msg_role);
		/* address */
		get_uint16_from_string(&argv[5], &ctl_common.ctx.addr);
		/* network key index */
		get_uint16_from_string(&argv[6], &ctl_common.ctx.net_idx);
	}
	if(argc > 13){
		/* op_en */
		uint8_t op_en_t;
		get_uint8_from_string(&argv[7], &op_en_t);
		light_client_set.ctl_set.op_en = op_en_t?true:false;

		/* onoff */
		get_uint16_from_string(&argv[8], &light_client_set.ctl_set.ctl_lightness);

		get_uint16_from_string(&argv[9], &light_client_set.ctl_set.ctl_temperatrue);

		get_uint16_from_string(&argv[10], (uint16_t*)&light_client_set.ctl_set.ctl_delta_uv);
		/* tid */
		get_uint8_from_string(&argv[11], &light_client_set.ctl_set.tid);
		/* trans_time */
		get_uint8_from_string(&argv[12], &light_client_set.ctl_set.trans_time);
		/* delay */
		get_uint8_from_string(&argv[13], &light_client_set.ctl_set.delay);
	}

	if (strcmp(argv[1], "get") == 0) {
		err = bfl_ble_mesh_light_client_get_state(&ctl_common, &light_client_get);
	} else if (strcmp(argv[1], "set") == 0) {
		err = bfl_ble_mesh_light_client_set_state(&ctl_common, &light_client_set);
	} else if (strcmp(argv[1], "reg") == 0) {
		err = bfl_ble_mesh_register_light_client_callback(ble_mesh_light_client_model_cb);
		if (err == BFL_OK) {
			vOutputString("Light lightness client:Reg,OK\n");
		}
	}
	vOutputString("exit %s\n", __func__);
}

BLEMESH_CLI(light_hsl_cli)
{
	int err = BFL_OK;

	if(argc < 1){
		vOutputString("Number of Parameters is not correct\n");
		return;
	}

	bfl_ble_mesh_light_client_set_state_t light_client_set;
	bfl_ble_mesh_light_client_get_state_t light_client_get;
	bfl_ble_mesh_client_common_param_t hsl_common = {
		.msg_timeout = 0,
		.ctx.send_ttl = 7,
		.model = NULL,
	};

	vOutputString("enter %s\n", __func__);

	/*Get client model*/
	for(int i = 0; i < sizeof(sig_models)/sizeof(struct bt_mesh_model); ++i){
		if(sig_models[i].id == BFL_BLE_MESH_MODEL_ID_LIGHT_HSL_CLI){
			 hsl_common.model = &sig_models[i];
		}
	}
	if(hsl_common.model == NULL){
		vOutputString("Can't find light ligntness cli %s\n", __func__);
		return;
	}

	if(argc > 6){
		/* appkey_index */
		get_uint16_from_string(&argv[2], &hsl_common.ctx.app_idx);
		/* opcode */
		get_uint32_from_string(&argv[3], &hsl_common.opcode);
		/* role */
		get_uint8_from_string(&argv[4], &hsl_common.msg_role);
		/* address */
		get_uint16_from_string(&argv[5], &hsl_common.ctx.addr);
		/* network key index */
		get_uint16_from_string(&argv[6], &hsl_common.ctx.net_idx);
	}
	if(argc > 13){
		/* op_en */
		uint8_t op_en_t;
		get_uint8_from_string(&argv[7], &op_en_t);
		light_client_set.hsl_set.op_en = op_en_t?true:false;

		/* onoff */
		get_uint16_from_string(&argv[8], &light_client_set.hsl_set.hsl_lightness);
		get_uint16_from_string(&argv[9], &light_client_set.hsl_set.hsl_hue);
		get_uint16_from_string(&argv[10], &light_client_set.hsl_set.hsl_saturation);
		/* tid */
		get_uint8_from_string(&argv[11], &light_client_set.hsl_set.tid);
		/* trans_time */
		get_uint8_from_string(&argv[12], &light_client_set.hsl_set.trans_time);
		/* delay */
		get_uint8_from_string(&argv[13], &light_client_set.hsl_set.delay);
	}

	if (strcmp(argv[1], "get") == 0) {
		err = bfl_ble_mesh_light_client_get_state(&hsl_common, &light_client_get);
	} else if (strcmp(argv[1], "set") == 0) {
		err = bfl_ble_mesh_light_client_set_state(&hsl_common, &light_client_set);
	} else if (strcmp(argv[1], "reg") == 0) {
		err = bfl_ble_mesh_register_light_client_callback(ble_mesh_light_client_model_cb);
		if (err == BFL_OK) {
			vOutputString("Light lightness client:Reg,OK\n");
		}
	}
	vOutputString("exit %s\n", __func__);
}
#endif
#if defined(CONFIG_BT_MESH_MODEL_VENDOR_CLI)
BLEMESH_CLI(vendor_cli)
{
    u16_t id;
    struct bt_mesh_msg_ctx ctx = {.send_ttl = 3};

    if(argc != 6 && argc != 7){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    vOutputString("enter %s\n", __func__);

    BT_MESH_MODEL_BUF_DEFINE(msg, BT_MESH_MODEL_OP_3(0x0000, BL_COMP_ID), 40);

    get_uint16_from_string(&argv[1], &ctx.app_idx);
    get_uint16_from_string(&argv[2], &id);
    get_uint16_from_string(&argv[4], &ctx.addr);
    get_uint16_from_string(&argv[5], &ctx.net_idx);

    bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_3(id, BL_COMP_ID));
    
    if(argc == 7){
        uint8_t len = strlen(argv[6])>>1;
        uint8_t val[40];
        get_bytearray_from_string(&argv[6], val, len);
        net_buf_simple_add_mem(&msg, val, len);
        vOutputString("send data %s\r\n", bt_hex(msg.data, msg.len));
    }

    

    struct bt_mesh_model* model_t;
    model_t = bt_mesh_model_find_vnd(elements, BL_COMP_ID, BT_MESH_VND_MODEL_ID_DATA_CLI);
    if(model_t == NULL){
        BT_ERR("Unable to found vendor model");
    }

    if (bt_mesh_model_send(model_t, &ctx, &msg, NULL, NULL)){
		BT_ERR("Unable to send vendor cli command");
	}
}
#endif /* CONFIG_BT_MESH_MODEL_VENDOR_CLI */


BLEMESH_CLI(models_cli)
{
	u32_t opcode;
	u32_t model_id;
	struct bt_mesh_msg_ctx ctx = {.send_ttl = 3};

	if(argc != 6 && argc != 7){
		vOutputString("Number of Parameters is not correct\r\n");
		return;
	}
	get_uint16_from_string(&argv[1], &ctx.app_idx);
	get_uint32_from_string(&argv[2], &opcode);
	get_uint16_from_string(&argv[3], &ctx.addr);
	get_uint16_from_string(&argv[4], &ctx.net_idx);
	get_uint32_from_string(&argv[5], &model_id);
	BT_MESH_MODEL_BUF_DEFINE(msg,opcode, 40);
	bt_mesh_model_msg_init(&msg, opcode);

	if(argc == 7){
		uint8_t len = strlen(argv[6])>>1;
		uint8_t val[40];
		get_bytearray_from_string(&argv[6], val, len);
		net_buf_simple_add_mem(&msg, val, len);
		vOutputString("send data %s\r\n", bt_hex(msg.data, msg.len));
	}
	
	struct bt_mesh_model* model_t;
	model_t = bt_mesh_model_find(elements, model_id);
	if(model_t == NULL){
		BT_ERR("Unable to found model");
	}

	if (bt_mesh_model_send(model_t, &ctx, &msg, NULL, NULL)){
		BT_ERR("Unable to send models cli command");
	}
}
#endif /* CONFIG_BT_MESH_MODEL */

static int input(bt_mesh_input_action_t act, u8_t size)
{
	char str[20];
	switch (act) {
	case BT_MESH_ENTER_NUMBER:{
		vOutputString("Enter a number (max %u digits) with: input-num <num>:", size);
		read_str(str, sizeof(str)-1);
		u32_t num = strtoul(str, NULL, 10);
		vOutputString("Recved num[%lu]\n", num);
		int err = bt_mesh_input_number(num);
		if (err) {
			vOutputString("Numeric input failed (err %d)\r\n", err);
		}
	}break;
	case BT_MESH_ENTER_STRING:{
		vOutputString("Enter a string (max %u chars) with: input-str <str>\r\n", size);
		read_str(str, sizeof(str)-1);
		vOutputString("Recved string[%s]\n", str);
		int err = bt_mesh_input_string(str);
		if (err) {
			vOutputString("String input failed (err %d)\r\n", err);
		}
	}break;
	default:
		vOutputString("Unknown input action %u (size %u) requested!\r\n", act, size);
		return -EINVAL;
	}

	input_act = act;
	input_size = size;
	return 0;
}

BLEMESH_CLI(reset)
{
	if(!blemesh_inited){
		vOutputString("blemesh not init\n");
		return;
	}
	bt_mesh_reset();
	vOutputString("Local node reset complete\r\n");
#if defined(CONFIG_BT_MESH_PROVISIONER)
	bt_mesh_cdb_clear();
	vOutputString("Cleared CDB\n");
#endif
}

BLEMESH_CLI(net_send)
{
    uint8_t ttl;
    uint8_t ctl;
    uint16_t src;
    uint16_t dst;
    uint8_t payload[16] = {0x00};
    
    if(argc != 5){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
    if (!bt_mesh_is_provisioned()) {
        vOutputString("Local node is not yet provisioned\r\n");
        return;
    }
    
    get_uint8_from_string(&argv[1], &ttl);
    get_uint8_from_string(&argv[2], &ctl);
    get_uint16_from_string(&argv[3], &src);
    get_uint16_from_string(&argv[4], &dst);
    
    struct bt_mesh_msg_ctx ctx = {
        .net_idx = net.net_idx,
        .app_idx = ctl ? BT_MESH_KEY_UNUSED : BT_MESH_KEY_DEV,
        .addr = dst,
        .send_rel = 0,
        .send_ttl = ttl,
    };
    
    struct bt_mesh_net_tx tx = {
        .sub = bt_mesh_subnet_get(ctx.net_idx),
        .ctx = &ctx,
        .src = src,
        .xmit = bt_mesh_net_transmit_get(),
        .friend_cred = 0,
    };
    
    struct net_buf *buf = bt_mesh_adv_create(BT_MESH_ADV_DATA, tx.xmit, K_NO_WAIT);
    if (!buf) {
        vOutputString("Out of network buffers\r\n");
        return;
    }
    
    vOutputString("Sending network packet\r\n");
    
    net_buf_reserve(buf, BT_MESH_NET_HDR_LEN);
    net_buf_add_mem(buf, payload, sizeof(payload));
    
    bt_mesh_net_send(&tx, buf, NULL, NULL);
}

static uint16_t get_app_idx(void)
{
    int i;
    
    for (i = 0; i < ARRAY_SIZE(bt_mesh.app_keys); i++) {
        struct bt_mesh_app_key *key = &bt_mesh.app_keys[i];
        
        if (key->net_idx != BT_MESH_KEY_UNUSED) {
            return key->app_idx;
        }
    }
    
    return BT_MESH_KEY_UNUSED;
}

BLEMESH_CLI(seg_send)
{
    NET_BUF_SIMPLE_DEFINE(sdu, BT_MESH_TX_SDU_MAX);
    uint16_t src;
    uint16_t dst;
    uint8_t payload[12] = {0xFF};
    
    if(argc != 3){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
    if (!bt_mesh_is_provisioned()) {
        vOutputString("Local node is not yet provisioned\r\n");
        return;
    }
    
    get_uint16_from_string(&argv[1], &src);
    get_uint16_from_string(&argv[2], &dst);
    
    struct bt_mesh_msg_ctx ctx = {
        .net_idx = net.net_idx,
        .app_idx = get_app_idx(),
        .addr = dst,
        .send_rel = 1,
        .send_ttl = BT_MESH_TTL_DEFAULT,
    };
    
    struct bt_mesh_net_tx tx = {
        .sub = bt_mesh_subnet_get(ctx.net_idx),
        .ctx = &ctx,
        .src = src,
        .xmit = bt_mesh_net_transmit_get(),
        .friend_cred = 0,
    };
    
    vOutputString("Sending segmented message\r\n");
    
    net_buf_simple_add_mem(&sdu, payload, sizeof(payload));
    
    bt_mesh_trans_send(&tx, &sdu, NULL, NULL);
}

BLEMESH_CLI(rpl_clr)
{
#if defined(CONFIG_BT_SETTINGS)

	bt_mesh_clear_rpl();

	vOutputString("Replay protection list (size: %d) cleared\r\n", CONFIG_BT_MESH_CRPL);
#endif
    
    memset(bt_mesh.rpl, 0, sizeof(bt_mesh.rpl));
}

BLEMESH_CLI(ivu_test)
{
    uint8_t enable;
    
    if(argc != 2){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
    get_uint8_from_string(&argv[1], &enable);
    
#if defined(CONFIG_BT_MESH_IV_UPDATE_TEST)
    bt_mesh_iv_update_test(enable);
#endif
    
    if (enable) {
        vOutputString("IV Update test mode enabled\r\n");
    } else {
        vOutputString("IV Update test mode disabled\r\n");
    }
}

BLEMESH_CLI(iv_update)
{
    uint8_t enable;
    
    if(argc != 2){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
    if (!bt_mesh_is_provisioned()) {
        vOutputString("Local node is not yet provisioned\r\n");
        return;
    }
    
    get_uint8_from_string(&argv[1], &enable);
    
    if (enable) {
        vOutputString("IV Update procedure started\r\n");
        
#if defined(CONFIG_BT_MESH_IV_UPDATE_TEST)
        bt_mesh_iv_update();
#endif
        
        if (!atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_INITIATOR)) {
            bt_mesh_beacon_ivu_initiator(1);
        }
    } else {
        vOutputString("IV Update procedure stopped\r\n");
        
        if (atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_INITIATOR)) {
            bt_mesh_beacon_ivu_initiator(0);
        }
    }
}

BLEMESH_CLI(fault_set)
{
    uint8_t type;
    int i;
    
    if(argc != 3){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
    get_uint8_from_string(&argv[1], &type);
    
    if (type == 0) {
        if(strlen(argv[2])/2 >= sizeof(cur_faults)) {
            get_bytearray_from_string(&argv[2], cur_faults,sizeof(cur_faults));
        } else {
            memset(cur_faults, 0x00, sizeof(cur_faults));
            get_bytearray_from_string(&argv[2], cur_faults,strlen(argv[2])/2);
        }
        
        vOutputString("Current Fault: ");
        for(i=0; i<sizeof(cur_faults); i++) {
            vOutputString("%02X", cur_faults[i]);
        }
        vOutputString("\r\n");
    } else {
        if(strlen(argv[2])/2 >= sizeof(reg_faults)) {
            get_bytearray_from_string(&argv[2], reg_faults,sizeof(reg_faults));
        } else {
            memset(reg_faults, 0x00, sizeof(reg_faults));
            get_bytearray_from_string(&argv[2], reg_faults,strlen(argv[2])/2);
        }
        
        vOutputString("Registered Fault: ");
        for(i=0; i<sizeof(reg_faults); i++) {
            vOutputString("%02X", reg_faults[i]);
        }
        vOutputString("\r\n");
    }
}

static void get_faults(u8_t *faults, u8_t faults_size, u8_t *dst, u8_t *count)
{
	u8_t i, limit = *count;

	for (i = 0U, *count = 0U; i < faults_size && *count < limit; i++) {
		if (faults[i]) {
			*dst++ = faults[i];
			(*count)++;
		}
	}
}

void show_faults(u8_t test_id, u16_t cid, u8_t *faults, size_t fault_count)
{
	size_t i;

	if (!fault_count) {
		vOutputString("Health Test ID 0x%02x Company ID 0x%04x: no faults\r\n", test_id, cid);
		return;
	}

	vOutputString("Health Test ID 0x%02x Company ID 0x%04x Fault Count %zu:\r\n", test_id, cid, fault_count);

	for (i = 0; i < fault_count; i++) {
		vOutputString("\t0x%02x", faults[i]);
	}
}

static void health_current_status(struct bt_mesh_health_cli *cli, u16_t addr,
				  u8_t test_id, u16_t cid, u8_t *faults,
				  size_t fault_count)
{
	vOutputString("Health Current Status from 0x%04x\r\n", addr);
	show_faults(test_id, cid, faults, fault_count);
}

static int fault_get_cur(struct bt_mesh_model *model, u8_t *test_id,
			 u16_t *company_id, u8_t *faults, u8_t *fault_count)
{
	vOutputString("Sending current faults\r\n");

	*test_id = 0x00;
	*company_id = BL_COMP_ID;

	get_faults(cur_faults, sizeof(cur_faults), faults, fault_count);

	return 0;
}

static int fault_get_reg(struct bt_mesh_model *model, u16_t cid,
			 u8_t *test_id, u8_t *faults, u8_t *fault_count)
{
	if (cid != BL_COMP_ID) {
		vOutputString("Faults requested for unknown Company ID 0x%04x\r\n", cid);
		return -EINVAL;
	}

	vOutputString("Sending registered faults\r\n");

	*test_id = 0x00;

	get_faults(reg_faults, sizeof(reg_faults), faults, fault_count);

	return 0;
}

static int fault_clear(struct bt_mesh_model *model, uint16_t cid)
{
	if (cid != BL_COMP_ID) {
		return -EINVAL;
	}

	(void)memset(reg_faults, 0, sizeof(reg_faults));

	return 0;
}

static int fault_test(struct bt_mesh_model *model, uint8_t test_id, uint16_t cid)
{
	if (cid != BL_COMP_ID) {
		return -EINVAL;
	}

	if (test_id != 0x00) {
		return -EINVAL;
	}

	return 0;
}

static void attn_on(struct bt_mesh_model *model)
{
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
	vOutputString("[PTS] Attention timer on\r\n");
#endif
}

static void attn_off(struct bt_mesh_model *model)
{
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
	vOutputString("[PTS] Attention timer off\r\n");
#endif
}

#if defined(CONFIG_BT_MESH_GATT_PROXY)
BLEMESH_CLI(proxy_identity)
{
	int err;

	err = bt_mesh_proxy_identity_enable();
	if (err) {
		vOutputString("Failed advertise using Node Identity (err ""%d)\r\n", err);
	}
}
#endif /* MESH_GATT_PROXY */

#if defined(CONFIG_BT_MESH_CDB)
BLEMESH_CLI(cdb_create)
{
	u8_t net_key[16];
	size_t len;
	int err;

	if (argc < 2) {
		bt_rand(net_key, 16);
	} else {
		len = hex2bin(argv[1], strlen(argv[1]), net_key,
			      sizeof(net_key));
		memset(net_key + len, 0, sizeof(net_key) - len);
	}

	err = bt_mesh_cdb_create(net_key);
	if (err < 0) {
		vOutputString("Failed to create CDB (err %d)\n", err);
	}
	vOutputString("Net key:%s\n", bt_hex(net_key, 16));
}

BLEMESH_CLI(cdb_clear)
{
	if(!blemesh_inited){
		vOutputString("blemesh not init\n");
		return;
	}
	bt_mesh_cdb_clear();

	vOutputString("Cleared CDB\n");
}

static void cdb_print_nodes(void)
{
	char key_hex_str[32 + 1], uuid_hex_str[32 + 1];
	struct bt_mesh_cdb_node *node;
	int i, total = 0;
	bool configured;

	vOutputString("Address  Elements  Flags  %-32s  DevKey\n", "UUID");

	for (i = 0; i < ARRAY_SIZE(bt_mesh_cdb.nodes); ++i) {
		node = &bt_mesh_cdb.nodes[i];
		if (node->addr == BT_MESH_ADDR_UNASSIGNED) {
			continue;
		}

		configured = atomic_test_bit(node->flags,
					     BT_MESH_CDB_NODE_CONFIGURED);

		total++;
		bin2hex(node->uuid, 16, uuid_hex_str, sizeof(uuid_hex_str));
		bin2hex(node->dev_key, 16, key_hex_str, sizeof(key_hex_str));
		vOutputString("0x%04x   %-8d  %-5s  %s  %s\n", node->addr,
			    node->num_elem, configured ? "C" : "-",
			    uuid_hex_str, key_hex_str);
	}

	vOutputString("> Total nodes: %d\n", total);
}

static void cdb_print_subnets(void)
{
	struct bt_mesh_cdb_subnet *subnet;
	char key_hex_str[32 + 1];
	int i, total = 0;

	vOutputString("NetIdx  NetKey\n");

	for (i = 0; i < ARRAY_SIZE(bt_mesh_cdb.subnets); ++i) {
		subnet = &bt_mesh_cdb.subnets[i];
		if (subnet->net_idx == BT_MESH_KEY_UNUSED) {
			continue;
		}

		total++;
		bin2hex(subnet->keys[0].net_key, 16, key_hex_str,
			sizeof(key_hex_str));
		vOutputString("0x%03x   %s\n", subnet->net_idx,
			    key_hex_str);
	}

	vOutputString("> Total subnets: %d\n", total);
}

static void cdb_print_app_keys(void)
{
	struct bt_mesh_cdb_app_key *app_key;
	char key_hex_str[32 + 1];
	int i, total = 0;

	vOutputString("NetIdx  AppIdx  AppKey\n");

	for (i = 0; i < ARRAY_SIZE(bt_mesh_cdb.app_keys); ++i) {
		app_key = &bt_mesh_cdb.app_keys[i];
		if (app_key->net_idx == BT_MESH_KEY_UNUSED) {
			continue;
		}

		total++;
		bin2hex(app_key->keys[0].app_key, 16, key_hex_str,
			sizeof(key_hex_str));
		vOutputString("0x%03x   0x%03x   %s\n",
			    app_key->net_idx, app_key->app_idx, key_hex_str);
	}

	vOutputString("> Total app-keys: %d\n", total);
}

BLEMESH_CLI(cdb_show)
{
	if (!atomic_test_bit(bt_mesh_cdb.flags, BT_MESH_CDB_VALID)) {
		vOutputString("No valid networks\n");
		return;
	}

	vOutputString("Mesh Network Information\n");
	vOutputString("========================\n");

	cdb_print_nodes();
	vOutputString("---\n");
	cdb_print_subnets();
	vOutputString("---\n");
	cdb_print_app_keys();
}

BLEMESH_CLI(cdb_node_add)
{
	struct bt_mesh_cdb_node *node;
	u8_t uuid[16], dev_key[16];
	u16_t addr, net_idx;
	u8_t num_elem;
	size_t len;

	len = hex2bin(argv[1], strlen(argv[1]), uuid, sizeof(uuid));
	memset(uuid + len, 0, sizeof(uuid) - len);

	addr = strtoul(argv[2], NULL, 0);
	num_elem = strtoul(argv[3], NULL, 0);
	net_idx = strtoul(argv[4], NULL, 0);

	if (argc < 6) {
		bt_rand(dev_key, 16);
	} else {
		len = hex2bin(argv[5], strlen(argv[5]), dev_key,
			      sizeof(dev_key));
		memset(dev_key + len, 0, sizeof(dev_key) - len);
	}

	node = bt_mesh_cdb_node_alloc(uuid, addr, num_elem, net_idx);
	if (node == NULL) {
		vOutputString("Failed to allocate node\n");
		return;
	}

	memcpy(node->dev_key, dev_key, 16);

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_store_cdb_node(node);
	}

	vOutputString("Added node 0x%04x\n", addr);
}

BLEMESH_CLI(cdb_node_del)
{
	struct bt_mesh_cdb_node *node;
	u16_t addr;

	addr = strtoul(argv[1], NULL, 0);

	node = bt_mesh_cdb_node_get(addr);
	if (node == NULL) {
		vOutputString("No node with address 0x%04x\n", addr);
		return;
	}

	bt_mesh_cdb_node_del(node, true);

	vOutputString("Deleted node 0x%04x\n", addr);
}

BLEMESH_CLI(cdb_subnet_add)
{
	struct bt_mesh_cdb_subnet *sub;
	u8_t net_key[16];
	u16_t net_idx;
	size_t len;

	net_idx = strtoul(argv[1], NULL, 0);

	if (argc < 3) {
		bt_rand(net_key, 16);
	} else {
		len = hex2bin(argv[2], strlen(argv[2]), net_key,
			      sizeof(net_key));
		memset(net_key + len, 0, sizeof(net_key) - len);
	}

	sub = bt_mesh_cdb_subnet_alloc(net_idx);
	if (sub == NULL) {
		vOutputString("Could not add subnet\n");
		return;
	}

	memcpy(sub->keys[0].net_key, net_key, 16);

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_store_cdb_subnet(sub);
	}

	vOutputString("Added Subnet 0x%03x\n", net_idx);
}

BLEMESH_CLI(cdb_subnet_del)
{
	struct bt_mesh_cdb_subnet *sub;
	u16_t net_idx;

	net_idx = strtoul(argv[1], NULL, 0);

	sub = bt_mesh_cdb_subnet_get(net_idx);
	if (sub == NULL) {
		vOutputString("No subnet with NetIdx 0x%03x\n", net_idx);
		return;
	}

	bt_mesh_cdb_subnet_del(sub, true);

	vOutputString("Deleted subnet 0x%03x\n", net_idx);
}

BLEMESH_CLI(cdb_app_key_add)
{
	struct bt_mesh_cdb_app_key *key;
	u16_t net_idx, app_idx;
	u8_t app_key[16];
	size_t len;

	net_idx = strtoul(argv[1], NULL, 0);
	app_idx = strtoul(argv[2], NULL, 0);

	if (argc < 4) {
		bt_rand(app_key, 16);
	} else {
		len = hex2bin(argv[3], strlen(argv[3]), app_key,
			      sizeof(app_key));
		memset(app_key + len, 0, sizeof(app_key) - len);
	}

	key = bt_mesh_cdb_app_key_alloc(net_idx, app_idx);
	if (key == NULL) {
		vOutputString("Could not add AppKey\n");
		return;
	}

	memcpy(key->keys[0].app_key, app_key, 16);

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_store_cdb_app_key(key);
	}

	vOutputString("Added AppKey 0x%03x\n", app_idx);
}

BLEMESH_CLI(cdb_app_key_del)
{
	struct bt_mesh_cdb_app_key *key;
	u16_t app_idx;

	app_idx = strtoul(argv[1], NULL, 0);

	key = bt_mesh_cdb_app_key_get(app_idx);
	if (key == NULL) {
		vOutputString("No AppKey 0x%03x\n", app_idx);
		return;
	}

	bt_mesh_cdb_app_key_del(key, true);

	vOutputString("Deleted AppKey 0x%03x\n", app_idx);
}
#endif /* CONFIG_BT_MESH_CDB */

#if defined(CONFIG_BT_MESH_PROVISIONER)
static void print_unprovisioned_beacon(u8_t uuid[16],
				bt_mesh_prov_oob_info_t oob_info,
				u32_t *uri_hash)
{
	vOutputString("Received unprovisioned beacon info:\n");
	vOutputString("uuid:[%s]\n", bt_hex(uuid, 16));
	vOutputString("oob_info:[%x]\n", (u16_t)oob_info);
	if(uri_hash)
		vOutputString("uri_hash:[%lx]\n", *uri_hash);
	else
		vOutputString("uri_hash:[NULL]\n");
}

static void print_node_added(u16_t net_idx, u8_t uuid[16], u16_t addr, u8_t num_elem)
{
	vOutputString("Node added:\n");
	vOutputString("net_idx:[%x]\n", net_idx);
	vOutputString("uuid:[%s]\n", bt_hex(uuid, 16));
	vOutputString("addr:[%x]\n", addr);
	vOutputString("num_elem:[%x]\n", num_elem);
	
	/** Added by bouffalolab, 
	 * clear message cache for node that has proved again.
	 **/
	bt_mesh_rx_reset_node(addr);
	
}

BLEMESH_CLI(beacon_listen)
{
	u8_t val;
	if(argc != 2){
		vOutputString("Number of Parameters is not correct\r\n");
		return;
	}

	get_uint8_from_string(&argv[1], &val);
	vOutputString("Beacon listen:[%x]\n", val);
	if (val) {
		prov.unprovisioned_beacon = print_unprovisioned_beacon;
	} else {
		prov.unprovisioned_beacon = NULL;
	}
	return;
}

/* Default net, app & dev key values, unless otherwise specified */
static const u8_t default_key[16] = {
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
};
BLEMESH_CLI(provision)
{
	const u8_t *net_key = default_key;
	u16_t net_idx, addr;
	u32_t iv_index;
	int err;

	if (argc < 3) {
		return;
	}

	net_idx = strtoul(argv[1], NULL, 0);
	addr = strtoul(argv[2], NULL, 0);

	if (argc > 3) {
		iv_index = strtoul(argv[3], NULL, 0);
	} else {
		iv_index = 0U;
	}

	if (IS_ENABLED(CONFIG_BT_MESH_CDB)) {
		const struct bt_mesh_cdb_subnet *sub;

		sub = bt_mesh_cdb_subnet_get(net_idx);
		if (!sub) {
			vOutputString("No cdb entry for subnet 0x%03x\n",
				    net_idx);
			return;
		}

		net_key = sub->keys[sub->kr_flag].net_key;
	}

	err = bt_mesh_provision(net_key, net_idx, 0, iv_index, addr,
				default_key);
	if (err) {
		vOutputString("Provisioning failed (err %d)\n", err);
	}

	return;
}

static void configure_self(struct bt_mesh_cdb_node *self)
{
	struct bt_mesh_cdb_app_key *key;
	int err;

	vOutputString("Configuring self...\n");

	key = bt_mesh_cdb_app_key_get(app_idx);
	if (key == NULL) {
		vOutputString("No app-key 0x%04x\n", app_idx);
		return;
	}

	/* Add Application Key */
	err = bt_mesh_cfg_app_key_add(self->net_idx, self->addr, self->net_idx,
				      app_idx, key->keys[0].app_key, NULL);
	if (err < 0) {
		vOutputString("Failed to add app-key (err %d)\n", err);
		return;
	}

	#if 0
	/* Bind to Health model */
	err = bt_mesh_cfg_mod_app_bind(self->net_idx, self->addr, self->addr,
				       app_idx, BT_MESH_MODEL_ID_HEALTH_CLI,
				       NULL);
	if (err < 0) {
		vOutputString("Failed to bind app-key (err %d)\n", err);
		return;
	}
	#endif
	/* Bind to Generic onoff model */
	err = bt_mesh_cfg_mod_app_bind(self->net_idx, self->addr, self->addr,
				       app_idx, BT_MESH_MODEL_ID_GEN_ONOFF_CLI,
				       NULL);
	if (err < 0) {
		vOutputString("Failed to bind app-key (err %d)\n", err);
		return;
	}

	atomic_set_bit(self->flags, BT_MESH_CDB_NODE_CONFIGURED);

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_cdb_node_store(self);
	}

	vOutputString("Configuration complete\n");
}

static void configure_node(struct bt_mesh_cdb_node *node)
{
	struct bt_mesh_cdb_app_key *key;
	struct bt_mesh_cfg_mod_pub pub;
	u8_t status;
	int err;

	vOutputString("Configuring node 0x%04x...\n", node->addr);

	key = bt_mesh_cdb_app_key_get(app_idx);
	if (key == NULL) {
		vOutputString("No app-key 0x%04x\n", app_idx);
		return;
	}

	/* Add Application Key */
	err = bt_mesh_cfg_app_key_add(net_idx, node->addr, net_idx, app_idx,
				      key->keys[0].app_key, NULL);
	if (err < 0) {
		vOutputString("Failed to add app-key (err %d)\n", err);
		return;
	}
	#if 0
	/* Bind to Health model */
	err = bt_mesh_cfg_mod_app_bind(net_idx, node->addr, node->addr, app_idx,
				       BT_MESH_MODEL_ID_HEALTH_SRV, NULL);
	if (err < 0) {
		vOutputString("Failed to bind app-key (err %d)\n", err);
		return;
	}
	#endif

	/* Bind to Health model */
	err = bt_mesh_cfg_mod_app_bind(net_idx, node->addr, node->addr, app_idx,
				       BT_MESH_MODEL_ID_GEN_ONOFF_SRV, NULL);
	if (err < 0) {
		vOutputString("Failed to bind app-key (err %d)\n", err);
		return;
	}

	pub.addr = 1;
	pub.app_idx = key->app_idx;
	pub.cred_flag = false;
	pub.ttl = 7;
	pub.period = BT_MESH_PUB_PERIOD_10SEC(1);
	pub.transmit = 0;

	err = bt_mesh_cfg_mod_pub_set(net_idx, node->addr, node->addr,
				      BT_MESH_MODEL_ID_HEALTH_SRV, &pub,
				      &status);
	if (err < 0) {
		vOutputString("mod_pub_set %d, %d\n", err, status);
		return;
	}

	atomic_set_bit(node->flags, BT_MESH_CDB_NODE_CONFIGURED);

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_cdb_node_store(node);
	}

	vOutputString("Configuration complete\n");
}


static u8_t check_unconfigured(struct bt_mesh_cdb_node *node, void *data)
{
	if (!atomic_test_bit(node->flags, BT_MESH_CDB_NODE_CONFIGURED)) {
		if (node->addr == self_addr) {
			configure_self(node);
		} else {
			configure_node(node);
		}
	}

	return BT_MESH_CDB_ITER_CONTINUE;
}

/*CONFIG node*/
BLEMESH_CLI(node_cfg)
{
	if (argc < 1) {
		return;
	}
	
	bt_mesh_cdb_node_foreach(check_unconfigured, NULL);
	vOutputString("Node config ok\n");
}


#endif /* CONFIG_BT_MESH_PROVISIONER */

#if defined(CONFIG_BT_MESH_PROVISIONER)
BLEMESH_CLI(get_comp)
{
	if (argc != 4) {
		vOutputString("Number of Parameters is not correct\r\n");
		return;
	}
	
	NET_BUF_SIMPLE_DEFINE(comp_buf, BT_MESH_RX_SDU_MAX);
	u8_t status, page;
	u16_t net_idx, dst;
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint8_from_string(&argv[3], &page);

	int err = bt_mesh_cfg_comp_data_get(net_idx, dst, page,
					&status, &comp_buf);
	if (err) {
		vOutputString("Getting composition failed (err %d)\n", err);
		return;
	}

	if (status != 0x00) {
		vOutputString("Got non-success status 0x%02x\n", status);
		return;
	}

	vOutputString("Got Composition Data for 0x%04x:\n", dst);
	vOutputString("\tCID      0x%04x\n",
		    net_buf_simple_pull_le16(&comp_buf));
	vOutputString("\tPID      0x%04x\n",
		    net_buf_simple_pull_le16(&comp_buf));
	vOutputString("\tVID      0x%04x\n",
		    net_buf_simple_pull_le16(&comp_buf));
	vOutputString("\tCRPL     0x%04x\n",
		    net_buf_simple_pull_le16(&comp_buf));
	vOutputString("\tFeatures 0x%04x\n",
		    net_buf_simple_pull_le16(&comp_buf));

	while (comp_buf.len > 4) {
		u8_t sig, vnd;
		u16_t loc;
		int i;

		loc = net_buf_simple_pull_le16(&comp_buf);
		sig = net_buf_simple_pull_u8(&comp_buf);
		vnd = net_buf_simple_pull_u8(&comp_buf);

		vOutputString("\tElement @ 0x%04x:\n", loc);

		if (comp_buf.len < ((sig * 2U) + (vnd * 4U))) {
			vOutputString("\t\t...truncated data!\n");
			break;
		}

		if (sig) {
			vOutputString("\t\tSIG Models:\n");
		} else {
			vOutputString("\t\tNo SIG Models\n");
		}

		for (i = 0; i < sig; i++) {
			u16_t mod_id = net_buf_simple_pull_le16(&comp_buf);

			vOutputString("\t\t\t0x%04x\n", mod_id);
		}

		if (vnd) {
			vOutputString("\t\tVendor Models:\n");
		} else {
			vOutputString("\t\tNo Vendor Models\n");
		}

		for (i = 0; i < vnd; i++) {
			u16_t cid = net_buf_simple_pull_le16(&comp_buf);
			u16_t mod_id = net_buf_simple_pull_le16(&comp_buf);

			vOutputString("\t\t\tCompany 0x%04x: 0x%04x\n", cid,
				    mod_id);
		}
	}

	return;
}

/* Add by bouffalo */
BLEMESH_CLI(set_dst)
{
	if (argc != 2) {
		return;
	}
	net.dst = strtoul(argv[1], NULL, 0);
	vOutputString("Net dst[0x%x]\n", net.dst);
}

#define CID_NVAL   0xffff
BLEMESH_CLI(net_key_add)
{
	bool has_key_val = (argc > 2);
	u8_t key_val[16];
	u16_t key_net_idx, net_idx, dst;
	u8_t status;
	int err;

	if (argc < 4) {
		return;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &key_net_idx);

	if (has_key_val) {
		size_t len;

		len = hex2bin(argv[5], strlen(argv[5]),
			      key_val, sizeof(key_val));
		(void)memset(key_val, 0, sizeof(key_val) - len);
	} else {
		memcpy(key_val, default_key, sizeof(key_val));
	}

	if (IS_ENABLED(CONFIG_BT_MESH_CDB)) {
		struct bt_mesh_cdb_subnet *subnet;

		subnet = bt_mesh_cdb_subnet_get(key_net_idx);
		if (subnet) {
			if (has_key_val) {
				vOutputString("Subnet 0x%03x already has a value",
					    key_net_idx);
				return;
			}

			memcpy(key_val, subnet->keys[0].net_key, 16);
		} else {
			subnet = bt_mesh_cdb_subnet_alloc(key_net_idx);
			if (!subnet) {
				vOutputString("No space for subnet in cdb");
				return;
			}

			memcpy(subnet->keys[0].net_key, key_val, 16);
			bt_mesh_cdb_subnet_store(subnet);
		}
	}

	err = bt_mesh_cfg_net_key_add(net_idx, dst, key_net_idx,
				      key_val, &status);
	if (err) {
		vOutputString("Unable to send NetKey Add (err %d)", err);
		return;
	}

	if (status) {
		vOutputString("NetKeyAdd failed with status 0x%02x",
			    status);
	} else {
		vOutputString("NetKey added with NetKey Index 0x%03x",
			    key_net_idx);
	}

	return;
}

/** Added by bouffalo
 * Default net, app & dev key values, unless otherwise specified 
 **/
static const u8_t default_new_key[16] = {
	0x02, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	0x02, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
};

/* Added by bouffalo */
BLEMESH_CLI(net_key_update)
{
	bool has_key_val = (argc > 4);
	u8_t key_val[16];
	u16_t key_net_idx, net_idx, dst;
	u8_t status;
	int err;

	if (argc < 4) {
		return;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &key_net_idx);

	if (has_key_val) {
		size_t len;

		len = hex2bin(argv[4], strlen(argv[4]),
			      key_val, sizeof(key_val));
		(void)memset(key_val, 0, sizeof(key_val) - len);
	} else {
		memcpy(key_val, default_new_key, sizeof(key_val));
	}

	err = bt_mesh_cfg_net_key_update(net_idx, dst, key_net_idx,
				      key_val, &status);
	if (err) {
		vOutputString("Unable to send NetKey Update (err %d)", err);
		return;
	}

	if (status) {
		vOutputString("NetKeyUpdate failed with status 0x%02x",
			    status);
	} else {
		vOutputString("NetKey updated with NetKey Index 0x%03x",
			    key_net_idx);
	}
	return;
}

BLEMESH_CLI(net_key_get)
{
	u16_t keys[16], net_idx, dst;
	size_t cnt;
	int err, i;

	cnt = ARRAY_SIZE(keys);
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	err = bt_mesh_cfg_net_key_get(net_idx, dst, keys, &cnt);
	if (err) {
		vOutputString("Unable to send NetKeyGet (err %d)", err);
		return;
	}

	vOutputString("NetKeys known by 0x%04x:", dst);
	for (i = 0; i < cnt; i++) {
		vOutputString("\t0x%03x", keys[i]);
	}

	return;
}

BLEMESH_CLI(net_key_del)
{
	u16_t key_net_idx, net_idx, dst;
	u8_t status;
	int err;
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &key_net_idx);

	err = bt_mesh_cfg_net_key_del(net_idx, dst, key_net_idx,
				      &status);
	if (err) {
		vOutputString("Unable to send NetKeyDel (err %d)", err);
		return;
	}

	if (status) {
		vOutputString("NetKeyDel failed with status 0x%02x",
			    status);
	} else {
		vOutputString("NetKey 0x%03x deleted", key_net_idx);
	}

	return;
}

BLEMESH_CLI(app_key_add)
{
	u8_t key_val[16];
	u16_t net_idx, dst, key_net_idx, key_app_idx;
	bool has_key_val = (argc > 5);
	u8_t status;
	int err;

	if (argc < 5) {
		return;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);

	get_uint16_from_string(&argv[3], &key_net_idx);
	get_uint16_from_string(&argv[4], &key_app_idx);

	if (has_key_val) {
		size_t len;

		len = hex2bin(argv[5], strlen(argv[5]),
			      key_val, sizeof(key_val));
		(void)memset(key_val, 0, sizeof(key_val) - len);
	} else {
		memcpy(key_val, default_key, sizeof(key_val));
	}

	if (IS_ENABLED(CONFIG_BT_MESH_CDB)) {
		struct bt_mesh_cdb_app_key *app_key;

		app_key = bt_mesh_cdb_app_key_get(key_app_idx);
		if (app_key) {
			if (has_key_val) {
				vOutputString("App key 0x%03x already has a value",
					key_app_idx);
				return;
			}

			memcpy(key_val, app_key->keys[0].app_key, 16);
		} else {
			app_key = bt_mesh_cdb_app_key_alloc(key_net_idx,
							    key_app_idx);
			if (!app_key) {
				vOutputString("No space for app key in cdb");
				return;
			}

			memcpy(app_key->keys[0].app_key, key_val, 16);
			bt_mesh_cdb_app_key_store(app_key);
		}
	}

	err = bt_mesh_cfg_app_key_add(net_idx, dst, key_net_idx,
				      key_app_idx, key_val, &status);
	if (err) {
		vOutputString("Unable to send App Key Add (err %d)", err);
		return;
	}

	if (status) {
		vOutputString("AppKeyAdd failed with status 0x%02x",
			    status);
	} else {
		vOutputString("AppKey added, NetKeyIndex 0x%04x "
			    "AppKeyIndex 0x%04x", key_net_idx, key_app_idx);
	}

	return;
}

/* Added by bouffalo */
BLEMESH_CLI(app_key_update)
{
	u8_t key_val[16];
	u16_t key_net_idx, key_app_idx, net_idx, dst;
	bool has_key_val = (argc > 6);
	u8_t status;
	int err;

	if (argc < 5) {
		return;
	}

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &key_net_idx);
	get_uint16_from_string(&argv[4], &key_app_idx);

	if (has_key_val) {
		size_t len;

		len = hex2bin(argv[5], strlen(argv[5]),
			      key_val, sizeof(key_val));
		(void)memset(key_val, 0, sizeof(key_val) - len);
	} else {
		memcpy(key_val, default_new_key, sizeof(key_val));
	}

	err = bt_mesh_cfg_app_key_update(net_idx, dst, key_net_idx,
				      key_app_idx, key_val, &status);
	if (err) {
		vOutputString("Unable to send App Key Add (err %d)", err);
		return;
	}

	if (status) {
		vOutputString("AppKeyAdd failed with status 0x%02x",
			    status);
	} else {
		vOutputString("AppKey added, NetKeyIndex 0x%04x "
			    "AppKeyIndex 0x%04x", key_net_idx, key_app_idx);
	}

	return;
}

BLEMESH_CLI(app_key_get)
{
	u16_t key_net_idx, net_idx, dst;
	u16_t keys[16];
	size_t cnt;
	u8_t status;
	int err, i;

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &key_net_idx);
	cnt = ARRAY_SIZE(keys);

	err = bt_mesh_cfg_app_key_get(net_idx, dst, key_net_idx, &status,
				      keys, &cnt);
	if (err) {
		vOutputString("Unable to send AppKeyGet (err %d)", err);
		return;
	}

	if (status) {
		vOutputString("AppKeyGet failed with status 0x%02x",
			    status);
		return;
	}

	vOutputString("AppKeys for NetKey 0x%03x known by 0x%04x:", net_idx,
		    dst);
	for (i = 0; i < cnt; i++) {
		vOutputString("\t0x%03x", keys[i]);
	}

	return;
}

BLEMESH_CLI(app_key_del)
{
	u16_t key_net_idx, key_app_idx, net_idx, dst;
	u8_t status;
	int err;

	if (argc < 5) {
		return;
	}

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &key_net_idx);
	get_uint16_from_string(&argv[4], &key_app_idx);

	err = bt_mesh_cfg_app_key_del(net_idx, dst, key_net_idx,
				      key_app_idx, &status);
	if (err) {
		vOutputString("Unable to send App Key del(err %d)", err);
		return;
	}

	if (status) {
		vOutputString("AppKeyDel failed with status 0x%02x",
			    status);
	} else {
		vOutputString("AppKey deleted, NetKeyIndex 0x%04x "
			    "AppKeyIndex 0x%04x", key_net_idx, key_app_idx);
	}

	return;
}

/* Added bouffalo */
BLEMESH_CLI(kr_update)
{
	u16_t idx;
	u8_t kr;
	if (argc != 3) {
		return;
	}

	get_uint16_from_string(&argv[1], &idx);
	get_uint8_from_string(&argv[2], &kr);
	struct bt_mesh_subnet *sub = bt_mesh_subnet_get(idx);
	bool kr_change = bt_mesh_kr_update(sub, kr, true);
	if (kr_change) {
		bt_mesh_net_beacon_update(sub);
	}
}

BLEMESH_CLI(mod_app_bind)
{
	u16_t elem_addr, mod_app_idx, mod_id, cid;
	u8_t status;
	int err;

	if (argc < 6) {
		return;
	}
	u16_t net_idx, dst;
		
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);

	get_uint16_from_string(&argv[3], &elem_addr);
	get_uint16_from_string(&argv[4], &mod_app_idx);
	get_uint16_from_string(&argv[5], &mod_id);

	if (argc > 6) {
		get_uint16_from_string(&argv[6], &cid);
		err = bt_mesh_cfg_mod_app_bind_vnd(net_idx, dst,
						   elem_addr, mod_app_idx,
						   mod_id, cid, &status);
	} else {
		err = bt_mesh_cfg_mod_app_bind(net_idx, dst, elem_addr,
					       mod_app_idx, mod_id, &status);
	}

	if (err) {
		vOutputString("Unable to send Model App Bind (err %d)",
			    err);
		return;
	}

	if (status) {
		vOutputString("Model App Bind failed with status 0x%02x",
			    status);
	} else {
		vOutputString("AppKey successfully bound");
	}

	return;
}

BLEMESH_CLI(mod_app_unbind)
{
	u16_t elem_addr, mod_app_idx, mod_id, cid;
	u8_t status;
	int err;

	if (argc < 6) {
		return;
	}
	u16_t net_idx, dst;
		
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	
	get_uint16_from_string(&argv[3], &elem_addr);
	get_uint16_from_string(&argv[4], &mod_app_idx);
	get_uint16_from_string(&argv[5], &mod_id);

	if (argc > 6) {
		get_uint16_from_string(&argv[6], &cid);
		err = bt_mesh_cfg_mod_app_unbind_vnd(net_idx, dst,
						   elem_addr, mod_app_idx,
						   mod_id, cid, &status);
	} else {
		err = bt_mesh_cfg_mod_app_unbind(net_idx, dst,
				elem_addr, mod_app_idx, mod_id, &status);
	}

	if (err) {
		vOutputString("Unable to send Model App Unbind (err %d)",
			    err);
		return;
	}

	if (status) {
		vOutputString("Model App Unbind failed with status 0x%02x",
			    status);
	} else {
		vOutputString("AppKey successfully unbound");
	}

	return;
}

BLEMESH_CLI(mod_app_get)
{
	u16_t elem_addr, mod_id, cid, net_idx, dst;
	u16_t apps[16];
	u8_t status;
	size_t cnt;
	int err, i;

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &elem_addr);
	get_uint16_from_string(&argv[4], &mod_id);
	cnt = ARRAY_SIZE(apps);

	if (argc > 5) {
		get_uint16_from_string(&argv[5], &cid);
		err = bt_mesh_cfg_mod_app_get_vnd(net_idx, dst,
						  elem_addr, mod_id, cid,
						  &status, apps, &cnt);
	} else {
		err = bt_mesh_cfg_mod_app_get(net_idx, dst, elem_addr,
					      mod_id, &status, apps, &cnt);
	}

	if (err) {
		vOutputString("Unable to send Model App Get (err %d)\n",
			    err);
		return;
	}

	if (status) {
		vOutputString("Model App Get failed with status 0x%02x\n",
			    status);
	} else {
		vOutputString("Apps bound to Element 0x%04x, Model 0x%04x %s:\n",
			elem_addr, mod_id, argc > 5 ? argv[5] : "(SIG)");

		if (!cnt) {
			vOutputString("\tNone.\n");
		}

		for (i = 0; i < cnt; i++) {
			vOutputString("\t0x%04x\n", apps[i]);
		}
	}

	return;
}

BLEMESH_CLI(mod_sub_add)
{
	u16_t elem_addr, sub_addr, mod_id, cid, net_idx, dst;
	u8_t status;
	int err;

	if (argc < 6) {
		return;
	}

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &elem_addr);
	get_uint16_from_string(&argv[4], &sub_addr);
	get_uint16_from_string(&argv[5], &mod_id);

	if (argc > 6) {
		get_uint16_from_string(&argv[6], &cid);
		err = bt_mesh_cfg_mod_sub_add_vnd(net_idx, dst,
						  elem_addr, sub_addr, mod_id,
						  cid, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_add(net_idx, dst, elem_addr,
					      sub_addr, mod_id, &status);
	}

	if (err) {
		vOutputString("Unable to send Model Subscription Add "
			    "(err %d)\n", err);
		return;
	}

	if (status) {
		vOutputString("Model Subscription Add failed with status "
			    "0x%02x\n", status);
	} else {
		vOutputString("Model subscription was successful\n");
	}

	return;
}

/** Added by bouffalo 
 * Config Model Subscription Overwrite.
 **/
BLEMESH_CLI(mod_sub_ow)
{
	u16_t elem_addr, sub_addr, mod_id, cid, net_idx, dst;
	u8_t status;
	int err;

	if (argc < 6) {
		return;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &elem_addr);
	get_uint16_from_string(&argv[4], &sub_addr);
	get_uint16_from_string(&argv[5], &mod_id);

	if (argc > 6) {
		get_uint16_from_string(&argv[6], &cid);
		err = bt_mesh_cfg_mod_sub_overwrite_vnd(net_idx, dst,
						  elem_addr, sub_addr, mod_id,
						  cid, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_overwrite(net_idx, dst, elem_addr,
					      sub_addr, mod_id, &status);
	}

	if (err) {
		vOutputString("Unable to send Model Subscription overwrite "
			    "(err %d)\n", err);
		return;
	}

	if (status) {
		vOutputString("Model Subscription overwrite failed with status "
			    "0x%02x\n", status);
	} else {
		vOutputString("Model subscription was successful\n");
	}

	return;
}

BLEMESH_CLI(mod_sub_del)
{
	u16_t elem_addr, sub_addr, mod_id, cid, net_idx, dst;
	u8_t status;
	int err;

	if (argc < 6) {
		return;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &elem_addr);
	get_uint16_from_string(&argv[4], &sub_addr);
	get_uint16_from_string(&argv[5], &mod_id);

	if (argc > 6) {
		get_uint16_from_string(&argv[6], &cid);
		err = bt_mesh_cfg_mod_sub_del_vnd(net_idx, dst,
						  elem_addr, sub_addr, mod_id,
						  cid, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_del(net_idx, dst, elem_addr,
					      sub_addr, mod_id, &status);
	}

	if (err) {
		vOutputString("Unable to send Model Subscription Delete (err %d)\n", err);
		return;
	}

	if (status) {
		vOutputString("Model Subscription Delete failed with "
			    "status 0x%02x\n", status);
	} else {
		vOutputString("Model subscription deltion was successful\n");
	}

	return;
}

BLEMESH_CLI(mod_sub_del_all)
{
	u16_t elem_addr, mod_id, cid, net_idx, dst;
	u8_t status;
	int err;

	if (argc < 5) {
		return;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &elem_addr);
	get_uint16_from_string(&argv[4], &mod_id);

	if (argc > 6) {
		get_uint16_from_string(&argv[6], &cid);
		err = bt_mesh_cfg_mod_sub_del_all_vnd(net_idx, dst,
						  elem_addr, mod_id,
						  cid, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_del_all(net_idx, dst, elem_addr,
					      mod_id, &status);
	}

	if (err) {
		vOutputString("Unable to send Model Subscription Delete "
			    "(err %d)\n", err);
		return;
	}

	if (status) {
		vOutputString("Model Subscription Delete failed with "
			    "status 0x%02x\n", status);
	} else {
		vOutputString("Model subscription deltion was successful\n");
	}

	return;
}

BLEMESH_CLI(mod_sub_add_va)
{
	u16_t elem_addr, sub_addr, mod_id, cid, net_idx, dst;
	u8_t label[16];
	u8_t status;
	size_t len;
	int err;

	if (argc < 6) {
		return;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &elem_addr);

	len = hex2bin(argv[4], strlen(argv[4]), label, sizeof(label));
	(void)memset(label + len, 0, sizeof(label) - len);

	get_uint16_from_string(&argv[5], &mod_id);

	if (argc > 6) {
		get_uint16_from_string(&argv[6], &cid);
		err = bt_mesh_cfg_mod_sub_va_add_vnd(net_idx, dst,
						     elem_addr, label, mod_id,
						     cid, &sub_addr, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_va_add(net_idx, dst,
						 elem_addr, label, mod_id,
						 &sub_addr, &status);
	}

	if (err) {
		vOutputString("Unable to send Mod Sub VA Add (err %d)\n",
			    err);
		return;
	}

	if (status) {
		vOutputString("Mod Sub VA Add failed with status 0x%02x\n",
			    status);
	} else {
		vOutputString("0x%04x subscribed to Label UUID %s "
			    "(va 0x%04x)\n", elem_addr, argv[2], sub_addr);
	}

	return;
}

				

/** Added by bouffalo 
 * Config Model Subscription Overwrite.
 **/
BLEMESH_CLI(mod_sub_ow_va)
{
	u16_t elem_addr, mod_id, cid, net_idx, dst;
	u8_t status;
	int err;
	u16_t virt_addr;
	size_t len;

	if (argc < 4) {
		return;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &elem_addr);
	u8_t label[16];
	len = hex2bin(argv[4], strlen(argv[4]), label, sizeof(label));
	(void)memset(label + len, 0, sizeof(label) - len);
	get_uint16_from_string(&argv[5], &mod_id);

	if (argc > 6) {
		get_uint16_from_string(&argv[6], &cid);
		err = bt_mesh_cfg_mod_sub_va_overwrite_vnd(net_idx, dst,
					 elem_addr, label,
					 mod_id, cid,
					 &virt_addr, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_va_overwrite(net_idx, dst, elem_addr,
				     label,
				     mod_id, &virt_addr,
				     &status);
	}
	
	if (err) {
		vOutputString("Unable to send Model Subscription overwrite "
				"(err %d)\n", err);
		return;
	}

	if (status) {
		vOutputString("Model Subscription overwrite failed with status "
				"0x%02x\n", status);
	} else {
		vOutputString("Model subscription was successful\n");
	}

	return;
}

BLEMESH_CLI(mod_sub_del_va)
{
	u16_t elem_addr, sub_addr, mod_id, cid, net_idx, dst;
	u8_t label[16];
	u8_t status;
	size_t len;
	int err;

	if (argc < 6) {
		return;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &elem_addr);

	len = hex2bin(argv[4], strlen(argv[4]), label, sizeof(label));
	(void)memset(label + len, 0, sizeof(label) - len);

	get_uint16_from_string(&argv[5], &mod_id);

	if (argc > 6) {
		get_uint16_from_string(&argv[6], &cid);
		err = bt_mesh_cfg_mod_sub_va_del_vnd(net_idx, dst,
						     elem_addr, label, mod_id,
						     cid, &sub_addr, &status);
	} else {
		err = bt_mesh_cfg_mod_sub_va_del(net_idx, dst,
						 elem_addr, label, mod_id,
						 &sub_addr, &status);
	}

	if (err) {
		vOutputString("Unable to send Model Subscription Delete "
			    "(err %d)\n", err);
		return;
	}

	if (status) {
		vOutputString("Model Subscription Delete failed with "
			    "status 0x%02x\n", status);
	} else {
		vOutputString("0x%04x unsubscribed from Label UUID %s "
			    "(va 0x%04x)\n", elem_addr, argv[4], sub_addr);
	}

	return;
}

BLEMESH_CLI(mod_sub_get)
{
	u16_t elem_addr, mod_id, cid, net_idx, dst;
	u16_t subs[16];
	u8_t status;
	size_t cnt;
	int err, i;
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &elem_addr);
	get_uint16_from_string(&argv[4], &mod_id);
	cnt = ARRAY_SIZE(subs);

	if (argc > 5) {
		get_uint16_from_string(&argv[5], &cid);
		err = bt_mesh_cfg_mod_sub_get_vnd(net_idx, dst,
						  elem_addr, mod_id, cid,
						  &status, subs, &cnt);
	} else {
		err = bt_mesh_cfg_mod_sub_get(net_idx, dst, elem_addr,
					      mod_id, &status, subs, &cnt);
	}

	if (err) {
		vOutputString("Unable to send Model Subscription Get "
			    "(err %d)\n", err);
		return;
	}

	if (status) {
		vOutputString("Model Subscription Get failed with "
			    "status 0x%02x\n", status);
	} else {
		vOutputString("Model Subscriptions for Element 0x%04x, "
			"Model 0x%04x %s:\n",
			elem_addr, mod_id, argc > 5 ? argv[5] : "(SIG)");

		if (!cnt) {
			vOutputString("\tNone.\n");
		}

		for (i = 0; i < cnt; i++) {
			vOutputString("\t0x%04x\n", subs[i]);
		}
	}

	return;
}

static void mod_pub_get(u16_t addr, u16_t mod_id,
		       u16_t cid)
{
	struct bt_mesh_cfg_mod_pub pub;
	u8_t status;
	int err;

	if (cid == CID_NVAL) {
		err = bt_mesh_cfg_mod_pub_get(net.net_idx, net.dst, addr,
					      mod_id, &pub, &status);
	} else {
		err = bt_mesh_cfg_mod_pub_get_vnd(net.net_idx, net.dst, addr,
						  mod_id, cid, &pub, &status);
	}

	if (err) {
		vOutputString("Model Publication Get failed (err %d)\n",
			    err);
		return;
	}

	if (status) {
		vOutputString("Model Publication Get failed "
			    "(status 0x%02x)\n", status);
		return;
	}

	vOutputString("Model Publication for Element 0x%04x, Model 0x%04x:\n"
		    "\tPublish Address:                0x%04x\n"
		    "\tAppKeyIndex:                    0x%04x\n"
		    "\tCredential Flag:                %u\n"
		    "\tPublishTTL:                     %u\n"
		    "\tPublishPeriod:                  0x%02x\n"
		    "\tPublishRetransmitCount:         %u\n"
		    "\tPublishRetransmitInterval:      %ums\n",
		    addr, mod_id, pub.addr, pub.app_idx, pub.cred_flag, pub.ttl,
		    pub.period, BT_MESH_PUB_TRANSMIT_COUNT(pub.transmit),
		    BT_MESH_PUB_TRANSMIT_INT(pub.transmit));

	return;
}

static void mod_pub_set(u16_t addr, u16_t mod_id,
		       u16_t cid, int argc, char *argv[])
{
	struct bt_mesh_cfg_mod_pub pub;
	u8_t status, count;
	u16_t interval, net_idx, dst;
	int err;
	get_uint16_from_string(&argv[0], &net_idx);
	get_uint16_from_string(&argv[1], &dst);
	get_uint16_from_string(&argv[2], &pub.addr);
	get_uint16_from_string(&argv[3], &pub.app_idx);
	get_uint8_from_string(&argv[4], (uint8_t*)&pub.cred_flag);
	get_uint8_from_string(&argv[5], &pub.ttl);
	get_uint8_from_string(&argv[6], &pub.period);

	get_uint8_from_string(&argv[7], &count);
	if (count > 9) {
		vOutputString("Invalid retransmit count\n");
		return;
	}

	get_uint16_from_string(&argv[8], &interval);
	if (interval > (31 * 50) || (interval % 50)) {
		vOutputString("Invalid retransmit interval %u\n", interval);
		return;
	}

	pub.transmit = BT_MESH_PUB_TRANSMIT(count, interval);

	vOutputString("count[%x] interval[%x]\n", count, interval);

	if (cid == CID_NVAL) {
		err = bt_mesh_cfg_mod_pub_set(net_idx, dst, addr,
					      mod_id, &pub, &status);
	} else {
		err = bt_mesh_cfg_mod_pub_set_vnd(net_idx, dst, addr,
						  mod_id, cid, &pub, &status);
	}

	if (err) {
		vOutputString("Model Publication Set failed (err %d)\n",
			    err);
		return;
	}

	if (status) {
		vOutputString("Model Publication Set failed \n"
			    "(status 0x%02x)", status);
	} else {
		vOutputString("Model Publication successfully set\n");
	}

	return;
}

static void mod_pub_va_set(u16_t addr, u16_t mod_id,
			  u16_t cid, int argc, char *argv[])
{
	struct bt_mesh_cfg_mod_pub pub;
	u8_t status, count;
	u16_t interval, net_idx, dst;
	int err;
	u8_t label[16];

	get_uint16_from_string(&argv[0], &net_idx);
	get_uint16_from_string(&argv[1], &dst);
	get_bytearray_from_string(&argv[2], label, sizeof(label));
	get_uint16_from_string(&argv[3], &pub.app_idx);
	get_uint8_from_string(&argv[4], (uint8_t*)&pub.cred_flag);
	get_uint8_from_string(&argv[5], &pub.ttl);
	get_uint8_from_string(&argv[6], &pub.period);

	get_uint8_from_string(&argv[7], &count);
	if (count > 9) {
		vOutputString("Invalid retransmit count\n");
		return;
	}

	get_uint16_from_string(&argv[8], &interval);
	if (interval > (31 * 50) || (interval % 50)) {
		vOutputString("Invalid retransmit interval %u\n", interval);
		return;
	}

	pub.transmit = BT_MESH_PUB_TRANSMIT(count, interval);

	vOutputString("count[%x] interval[%x]\n", count, interval);

	if (cid == CID_NVAL) {
		err = bt_mesh_cfg_mod_pub_va_set(net_idx, dst, addr,
				label, mod_id, &pub, &status);
	} else {
		err = bt_mesh_cfg_mod_pub_va_set(net_idx, dst, addr,
			label, mod_id, &pub, &status);
	}

	if (err) {
		vOutputString("Model Publication Set failed (err %d)\n",
				err);
		return;
	}

	if (status) {
		vOutputString("Model Publication Set failed \n"
				"(status 0x%02x)", status);
	} else {
		vOutputString("Model Publication successfully set\n");
	}

	return;
}

BLEMESH_CLI(mod_pub)
{
	u16_t addr, mod_id, cid;

	if (argc < 3) {
		return;
	}

	get_uint16_from_string(&argv[1], &addr);
	get_uint16_from_string(&argv[2], &mod_id);

	argc -= 3;
	argv += 3;

	if (argc == 1 || argc == 8) {
		get_uint16_from_string(&argv[0], &cid);
		argc--;
		argv++;
	} else {
		cid = CID_NVAL;
	}

	if (argc > 0) {
		if (argc < 7) {
			return;
		}

		mod_pub_set(addr, mod_id, cid, argc, argv);
	} else {
		mod_pub_get(addr, mod_id, cid);
	}
}

BLEMESH_CLI(mod_pub_va)
{
	u16_t addr, mod_id, cid;

	if (argc < 3) {
		return;
	}

	get_uint16_from_string(&argv[1], &addr);
	get_uint16_from_string(&argv[2], &mod_id);

	argc -= 3;
	argv += 3;

	if (argc == 1 || argc == 8) {
		get_uint16_from_string(&argv[0], &cid);
		argc--;
		argv++;
	} else {
		cid = CID_NVAL;
	}

	if (argc > 0) {
		if (argc < 7) {
			return;
		}

		mod_pub_va_set(addr, mod_id, cid, argc, argv);
	} else {
		mod_pub_get(addr, mod_id, cid);
	}
}

static void hb_sub_print(struct bt_mesh_cfg_hb_sub *sub)
{
	vOutputString("Heartbeat Subscription:\n"
		    "\tSource:      0x%04x\n"
		    "\tDestination: 0x%04x\n"
		    "\tPeriodLog:   0x%02x\n"
		    "\tCountLog:    0x%02x\n"
		    "\tMinHops:     %u\n"
		    "\tMaxHops:     %u\n",
		    sub->src, sub->dst, sub->period, sub->count,
		    sub->min, sub->max);
}

static void hb_sub_get(size_t argc, char *argv[])
{
	struct bt_mesh_cfg_hb_sub sub;
	u8_t status;
	u16_t net_idx, dst;
	int err;

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	err = bt_mesh_cfg_hb_sub_get(net_idx, dst, &sub, &status);
	if (err) {
		vOutputString("Heartbeat Subscription Get failed (err %d)\n",
			    err);
		return;
	}

	if (status) {
		vOutputString("Heartbeat Subscription Get failed "
			    "(status 0x%02x)\n", status);
	} else {
		hb_sub_print(&sub);
	}

	return;
}

static void hb_sub_set(size_t argc, char *argv[])
{
	struct bt_mesh_cfg_hb_sub sub;
	u8_t status;
	u16_t net_idx, dst;
	int err;

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &sub.src);
	get_uint16_from_string(&argv[4], &sub.dst);
	get_uint8_from_string(&argv[5], &sub.period);

	err = bt_mesh_cfg_hb_sub_set(net_idx, dst, &sub, &status);
	if (err) {
		vOutputString("Heartbeat Subscription Set failed (err %d)\n",
			    err);
		return;
	}

	if (status) {
		vOutputString("Heartbeat Subscription Set failed "
			    "(status 0x%02x)\n", status);
	} else {
		hb_sub_print(&sub);
	}

	return;
}

BLEMESH_CLI(hb_sub)
{
	if (argc > 3) {
		if (argc < 4) {
			return;
		}

		hb_sub_set(argc, argv);
	} else {
		hb_sub_get(argc, argv);
	}
}

static void hb_pub_get(size_t argc, char *argv[])
{
	struct bt_mesh_cfg_hb_pub pub;
	u8_t status;
	u16_t net_idx, dst;
	int err;

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	err = bt_mesh_cfg_hb_pub_get(net_idx, dst, &pub, &status);
	if (err) {
		vOutputString("Heartbeat Publication Get failed (err %d)\n",
			    err);
		return;
	}

	if (status) {
		vOutputString("Heartbeat Publication Get failed "
			    "(status 0x%02x)\n", status);
		return;
	}

	vOutputString("Heartbeat publication:\n");
	vOutputString("\tdst 0x%04x count 0x%02x period 0x%02x\n",
		    pub.dst, pub.count, pub.period);
	vOutputString("\tttl 0x%02x feat 0x%04x net_idx 0x%04x\n",
		    pub.ttl, pub.feat, pub.net_idx);

	return;
}

static void hb_pub_set(size_t argc, char *argv[])
{
	struct bt_mesh_cfg_hb_pub pub;
	u8_t status;
	u16_t net_idx, dst;
	int err;

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &pub.dst);
	get_uint8_from_string(&argv[4], &pub.count);
	get_uint8_from_string(&argv[5], &pub.period);
	get_uint8_from_string(&argv[6], &pub.ttl);
	get_uint16_from_string(&argv[7], &pub.feat);
	get_uint16_from_string(&argv[8], &pub.net_idx);

	err = bt_mesh_cfg_hb_pub_set(net_idx, dst, &pub, &status);
	if (err) {
		vOutputString("Heartbeat Publication Set failed (err %d)\n",
			    err);
		return;
	}

	if (status) {
		vOutputString("Heartbeat Publication Set failed "
			    "(status 0x%02x)\n", status);
	} else {
		vOutputString("Heartbeat publication successfully set\n");
	}

	return;
}

BLEMESH_CLI(hb_pub)
{
	if (argc > 3) {
		if (argc < 8) {
			return;
		}

		hb_pub_set(argc, argv);
	} else {
		hb_pub_get(argc, argv);
	}
}

/* Add by bouffalo */
BLEMESH_CLI(krp_get)
{
	struct bt_mesh_cfg_krp krp_buf;
	u8_t status;
	u16_t net_idx, dst;
	int err;
	if (argc != 4) {
		return;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &krp_buf.NetKeyIndex);

	err = bt_mesh_cfg_krp_get(net_idx, dst,
					&status, &krp_buf);
	if (err) {
		vOutputString("Getting Key Refresh Phase failed (err %d)\n", err);
		return;
	}

	if (status != 0x00) {
		vOutputString("Got non-success status 0x%02x\n", status);
		return;
	}

	vOutputString("Got Key Refresh Phase for 0x%04x:\n", dst);
	vOutputString("\tNetKeyIndex      0x%04x\n",
		    krp_buf.NetKeyIndex);
	vOutputString("\tPhase      0x%04x\n",
		    krp_buf.Phase);

	return;
}

/* Add by bouffalo */
BLEMESH_CLI(krp_set)
{
	struct bt_mesh_cfg_krp krp_buf;
	u8_t status;
	u16_t net_idx, dst;
	int err;
	if (argc != 5) {
		return;
	}
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &krp_buf.NetKeyIndex);
	get_uint8_from_string(&argv[4], &krp_buf.Transition);
	err = bt_mesh_cfg_krp_set(net_idx, dst,
					&status, &krp_buf);
	if (err) {
		vOutputString("Setting Key Refresh Phase failed (err %d)\n", err);
		return;
	}

	if (status != 0x00) {
		vOutputString("Got non-success status 0x%02x\n", status);
		return;
	}

	vOutputString("Got Key Refresh Phase for 0x%04x:\n", dst);
	vOutputString("\tNetKeyIndex      0x%04x\n",
		    krp_buf.NetKeyIndex);
	vOutputString("\tPhase      0x%04x\n",
		    krp_buf.Phase);

	return;
}

/* Add by bouffalo */
BLEMESH_CLI(cfg_bcn_get)
{
	u8_t status = 0;
	int err;
	if (argc != 3) {
		return;
	}
	u16_t net_idx;
	u16_t dst;
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	
	err = bt_mesh_cfg_beacon_get(net_idx, dst, &status);
	if (err) {
		vOutputString("Getting Config Beacon Status failed (err %d)\n", err);
		return;
	}

	vOutputString("Got Config Beacon Status for 0x%04x:\n", dst);
	vOutputString("\tSecure Network Beacon state      0x%02x\n",
		    status);
}

/* Add by bouffalo */
BLEMESH_CLI(cfg_bcn_set)
{
	u8_t status = 0;
	int err;
	if (argc != 4) {
		return;
	}
	u16_t net_idx = 0;
	u16_t dst = 0;
	u8_t val = 0;
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint8_from_string(&argv[3], &val);
	
	err = bt_mesh_cfg_beacon_set(net_idx, dst, val, &status);
	if (err) {
		vOutputString("Getting Config Beacon Status failed (err %d)\n", err);
		return;
	}

	vOutputString("Got Config Beacon Status for 0x%04x:\n", dst);
	vOutputString("\tSecure Network Beacon state      0x%02x\n",
		    status);
}

/* Add by bouffalo */
BLEMESH_CLI(cfg_dttl_get)
{
	int err;
	if (argc != 3) {
		return;
	}
	u16_t net_idx;
	u16_t dst;
	u8_t ttl = 0;
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	
	err = bt_mesh_cfg_ttl_get(net_idx, dst, &ttl);
	if (err) {
		vOutputString("Getting Default TTL failed (err %d)\n", err);
		return;
	}

	vOutputString("Got Default TTL for 0x%04x:\n", dst);
	vOutputString("\tDefault TTL      0x%02x\n",
		    ttl);
}

/* Add by bouffalo */
BLEMESH_CLI(cfg_dttl_set)
{
	int err;
	if (argc != 4) {
		return;
	}
	u16_t net_idx = 0;
	u16_t dst = 0;
	u8_t val = 0;
	u8_t ttl = 0;
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint8_from_string(&argv[3], &val);
	
	err = bt_mesh_cfg_ttl_set(net_idx, dst, val, &ttl);
	if (err) {
		vOutputString("Getting Default TTL failed (err %d)\n", err);
		return;
	}

	vOutputString("Got Default TTL for 0x%04x:\n", dst);
	vOutputString("\tDefault TTL      0x%02x\n",
		    ttl);
}

/* Add by bouffalo */
BLEMESH_CLI(cfg_gpxy_get)
{
	u8_t status = 0;
	int err;
	if (argc != 3) {
		return;
	}
	u16_t net_idx;
	u16_t dst;
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	
	err = bt_mesh_cfg_gatt_proxy_get(net_idx, dst, &status);
	if (err) {
		vOutputString("Getting the GATT Proxy state (err %d)\n", err);
		return;
	}
	vOutputString("\tGetting the GATT Proxy state  0x%02x\n",
		    status);
}

/* Add by bouffalo */
BLEMESH_CLI(cfg_gpxy_set)
{
	u8_t status = 0;
	int err;
	if (argc != 4) {
		return;
	}
	u16_t net_idx = 0;
	u16_t dst = 0;
	u8_t val = 0;
	
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint8_from_string(&argv[3], &val);
	
	err = bt_mesh_cfg_gatt_proxy_set(net_idx, dst, val, &status);
	if (err) {
		vOutputString("Getting Default TTL failed (err %d)\n", err);
		return;
	}

	vOutputString("Got Default TTL for 0x%04x:\n", dst);
	vOutputString("\tDefault TTL      0x%02x\n",
		    status);
}

BLEMESH_CLI(friend)
{
	u8_t frnd;
	int err = -1;

	if (argc == 3) {
		u16_t net_idx = 0;
		u16_t dst = 0;
		
		get_uint16_from_string(&argv[1], &net_idx);
		get_uint16_from_string(&argv[2], &dst);
		err = bt_mesh_cfg_friend_get(net_idx, dst, &frnd);
	} else if(argc == 4){
		u16_t net_idx = 0;
		u16_t dst = 0;
		u8_t val = 0;
		
		get_uint16_from_string(&argv[1], &net_idx);
		get_uint16_from_string(&argv[2], &dst);
		get_uint8_from_string(&argv[3], &val);

		err = bt_mesh_cfg_friend_set(net_idx, dst, val, &frnd);
	}

	if (err) {
		vOutputString("Unable to send Friend Get/Set (err %d)",err);
		return;
	}

	vOutputString("Friend is set to 0x%02x", frnd);

	return;
}

BLEMESH_CLI(relay)
{
	u8_t relay, transmit;
	int err;

	if (argc < 4) {
		u16_t net_idx = 0;
		u16_t dst = 0;
		
		get_uint16_from_string(&argv[1], &net_idx);
		get_uint16_from_string(&argv[2], &dst);
		err = bt_mesh_cfg_relay_get(net_idx, dst, &relay,
					    &transmit);
	} else {
		u8_t count, interval, new_transmit, val;
		u16_t net_idx = 0;
		u16_t dst = 0;
		
		get_uint8_from_string(&argv[3], &val);
		get_uint16_from_string(&argv[1], &net_idx);
		get_uint16_from_string(&argv[2], &dst);

		if (val) {
			if (argc > 4) {
				get_uint8_from_string(&argv[4], &count);
			} else {
				count = 2U;
			}

			if (argc > 5) {
				get_uint8_from_string(&argv[5], &interval);
			} else {
				interval = 20U;
			}

			new_transmit = BT_MESH_TRANSMIT(count, interval);
		} else {
			new_transmit = 0U;
		}

		err = bt_mesh_cfg_relay_set(net_idx, dst, val,
					    new_transmit, &relay, &transmit);
	}

	if (err) {
		vOutputString("Unable to send Relay Get/Set (err %d)",
			    err);
		return;
	}

	vOutputString("Relay is 0x%02x, Transmit 0x%02x (count %u interval"
		    " %ums)", relay, transmit, BT_MESH_TRANSMIT_COUNT(transmit),
		    BT_MESH_TRANSMIT_INT(transmit));

	return;
}

/* Added by bouffalo */
BLEMESH_CLI(node_identify)
{
	u8_t status, identity;
	u16_t netkeyindex, net_idx, dst;
	int err = -1;

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);

	if (argc == 5) {
		get_uint16_from_string(&argv[3], &netkeyindex);
		get_uint8_from_string(&argv[4], &identity);
		err = bt_mesh_cfg_node_identity_set(net_idx, dst,
						&status, &netkeyindex, &identity);
	} else if (argc == 4) {
		get_uint16_from_string(&argv[3], &netkeyindex);
		err = bt_mesh_cfg_node_identity_get(net_idx, dst,
						&status, &netkeyindex, &identity);
	}

	if (err) {
		vOutputString("Unable to node identity Get/Set (err %d)",
				err);
		return;
	}

	vOutputString("netkeyindex is %x, identity %x",
					netkeyindex, identity);

}

/* Added by bouffalo */
BLEMESH_CLI(node_reset)
{
	u16_t net_idx, dst;
	if (argc != 3) {
		vOutputString("Number of Parameters is not correct\r\n");
		return;
	}

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	
	int err = bt_mesh_cfg_node_reset(net_idx, dst);
	if (err) {
		vOutputString("Unable to node reset  (err %d)\n",
				err);
	}

#if defined(CONFIG_BT_MESH_CDB)
	/* Do local cdb clear */
	struct bt_mesh_cdb_node *node;
	
	node = bt_mesh_cdb_node_get(dst);
	if (node == NULL) {
		vOutputString("No node with address 0x%04x\n", dst);
		return;
	}

	bt_mesh_cdb_node_del(node, true);

	vOutputString("Deleted node 0x%04x\n", dst);
#endif
}

/* Added by bouffalo */
BLEMESH_CLI(network_trans)
{
	u8_t count, interval;
	u16_t net_idx, dst;
	int err = -1;

	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	
	if (argc == 5) {
		get_uint8_from_string(&argv[3], &count);
		get_uint8_from_string(&argv[4], &interval);
		err = bt_mesh_cfg_network_transmit_set(net_idx, dst,
						&count, &interval);
	} else if (argc == 3) {
		err = bt_mesh_cfg_network_transmit_get(net_idx, dst,
						&count, &interval);
	}

	if (err) {
		vOutputString("Unable to Network Transmit Get/Set (err %d)",
				err);
		return;
	}

	vOutputString("count is %x, interval %x",
					count, interval);
}

/* Added by bouffalo */
BLEMESH_CLI(lpn_timeout_get)
{
	u16_t lpn_addr, net_idx, dst;
	u32_t poll_timeout;
	int err;

	if (argc != 4) {
		return ;
	}
	get_uint16_from_string(&argv[1], &net_idx);
	get_uint16_from_string(&argv[2], &dst);
	get_uint16_from_string(&argv[3], &lpn_addr);
	err = bt_mesh_cfg_lpn_poll_timeout_get(net_idx, dst,
					&lpn_addr, &poll_timeout);

	if (err) {
		vOutputString("Unable to LPN Poll time out Get/Set (err %d)",
				err);
		return;
	}

	vOutputString("lpn_addr is %x, poll_timeout %lx",
					lpn_addr, poll_timeout);
}

/* Added by bouffalo for health client fault state*/
BLEMESH_CLI(clhm_fault)
{
	int err = 0;
	u16_t app_idx, cid, dst;
	u8_t test_id, faults[16];
	size_t fault_count;

	if(0 == strcmp(argv[1], "test")){
		get_uint16_from_string(&argv[2], &dst);
		get_uint16_from_string(&argv[3], &app_idx);
		get_uint16_from_string(&argv[4], &cid);
		get_uint8_from_string(&argv[5], &test_id);
		if(argc == 6)
			/* Health Fault Test */
			err = bt_mesh_health_fault_test(dst, app_idx, cid,
						 test_id, faults, &fault_count);
		else
			/* Health Fault Test Unacknowledged */
			err = bt_mesh_health_fault_test(dst, app_idx, cid,
						 test_id, NULL, &fault_count);
	}
	else if(0 == strcmp(argv[1], "get")){
		get_uint16_from_string(&argv[2], &dst);
		get_uint16_from_string(&argv[3], &app_idx);
		get_uint16_from_string(&argv[4], &cid);
		err = bt_mesh_health_fault_get(dst, app_idx, cid,
						 &test_id, faults, &fault_count);
	}
	else if(0 == strcmp(argv[1], "clear")){
		get_uint16_from_string(&argv[2], &dst);
		get_uint16_from_string(&argv[3], &app_idx);
		get_uint16_from_string(&argv[4], &cid);
		if(argc == 5)
			/* Health Fault Clear */
			err = bt_mesh_health_fault_clear(dst, app_idx, cid,
							 &test_id, faults, &fault_count);
		else
			/* Health Fault Clear Unacknowledged */
			err = bt_mesh_health_fault_clear(dst, app_idx, cid,
							 NULL, faults, &fault_count);
	}

	if (err) {
		vOutputString("Unable to health fault Get/Set (err %d)",
				err);
		return;
	}

}

/* Added by bouffalo for health client period state*/
BLEMESH_CLI(clhm_period)
{
	u8_t divisor, updated_divisor;
	u16_t app_idx, dst;
	int err = -1;

	if(0 == strcmp(argv[1], "get")){
		get_uint16_from_string(&argv[2], &dst);
		get_uint16_from_string(&argv[3], &app_idx);
		err = bt_mesh_health_period_get(dst, app_idx, &divisor);
	}
	else if(0 == strcmp(argv[1], "set")){
		get_uint16_from_string(&argv[2], &dst);
		get_uint16_from_string(&argv[3], &app_idx);
		get_uint8_from_string(&argv[4], &divisor);
		if(argc == 5)
			/* Acknowledged*/
			err = bt_mesh_health_period_set(dst, app_idx, divisor,
				 &updated_divisor);
		else
			/* Unacknowledged */
			err = bt_mesh_health_period_set(dst, app_idx, divisor,
				 NULL);
	}

	if (err) {
		vOutputString("Unable to health period Get/Set (err %d)",
				err);
		return;
	}
}
/* Added by bouffalo for health client attention timer state*/
BLEMESH_CLI(clhm_ats)
{
	u8_t attention, updated_attention;
	int err = -1;
	u16_t app_idx, dst;

	if(0 == strcmp(argv[1], "get")){
		get_uint16_from_string(&argv[2], &dst);
		get_uint16_from_string(&argv[3], &app_idx);
		err = bt_mesh_health_attention_get(dst, app_idx, &attention);
	}
	else if(0 == strcmp(argv[1], "set")){
		get_uint16_from_string(&argv[2], &dst);
		get_uint16_from_string(&argv[3], &app_idx);
		get_uint8_from_string(&argv[4], &attention);
		if(argc == 5)
			/* Acknowledged*/
			err = bt_mesh_health_attention_set(dst, app_idx, attention,
					 &updated_attention);
		else
			/* Unacknowledged */
			err = bt_mesh_health_attention_set(dst, app_idx, attention,
					 NULL);
	}

	if (err) {
		vOutputString("Unable to health attention Get/Set (err %d)",
				err);
		return;
	}
}
#endif

#if defined(CFG_NODE_SEND_CFGCLI_MSG) && defined(CONFIG_BT_MESH_CDB)
BLEMESH_CLI(pvnr_devkey)
{
	u8_t key[16], num_elem;
	u16_t addr;
	
	get_uint16_from_string(&argv[1], &addr);
	get_uint8_from_string(&argv[2], &num_elem);
	get_bytearray_from_string(&argv[3], key, sizeof(key));

	if(NULL == bt_mesh_cdb_node_set_fornode(addr, num_elem, key)){
		vOutputString("Set node devkey error\n");
	}
	else{
		vOutputString("Set node Addr[%x] element[%x], devkey[%s]\n",
						addr, num_elem, bt_hex(key, sizeof(key)));
	}
}
#endif /* CFG_NODE_SEND_CFGCLI_MSG */

#if defined(CONFIG_BT_MESH_SYNC)
BLEMESH_CLI(sync_start)
{
	return blsync_blemesh_start();
}

BLEMESH_CLI(sync_stop)
{
	return blsync_blemesh_stop();
}
#endif


#if defined(CONFIG_BT_MESH_TEST)
#define CONFIG_NODE_SIZE_BUFF (CONFIG_BT_MESH_NODE_COUNT<<1)
uint16_t node_list[CONFIG_NODE_SIZE_BUFF];

#define NODE_ADDR_FUN_MSK (0x8000)

/* check all nodelist is check*/
static int nodelist_check(uint16_t addr)
{
    int i = 0;

    if((node_list[addr] & (NODE_ADDR_FUN_MSK-1)) == addr){
        node_list[addr] |=  NODE_ADDR_FUN_MSK;
        for(i = 0; i < CONFIG_NODE_SIZE_BUFF; ++i){
            if(node_list[i] == 0)
                continue;
            if((node_list[i] & NODE_ADDR_FUN_MSK) == 0)
                return 0;
        }
    }

    return 1;
}


static int nodelist_check_clear()
{
    int i = 0;
    for(i = 0; i < CONFIG_NODE_SIZE_BUFF; ++i){
        if(node_list[i] == 0)
            continue;
        node_list[i] &= NODE_ADDR_FUN_MSK-1;
    }
    return 0;
}

BLEMESH_CLI(nodelist_op)
{
    /** arg1: 1,add address from start to end; 2,add single address, 3,delete single address
     *  arg2: start address or single address.
     *  arg3: end address.
     **/
    u8_t type = 0;
    u16_t i, saddr = 0, eaddr = 0;

    if (argc >= 3) {
        get_uint8_from_string(&argv[1], &type);
        get_uint16_from_string(&argv[2], &saddr);
        if(saddr >= CONFIG_NODE_SIZE_BUFF){
            vOutputString("node_list cache is small\r\n");
            return;
        }
        if(type == 1 && argc == 4){
            get_uint16_from_string(&argv[3], &eaddr);
            if(eaddr >= CONFIG_NODE_SIZE_BUFF){
                vOutputString("node_list cache is small\r\n");
                return;
            }
            if(eaddr <= saddr){
                vOutputString("Start address isn't bigger than end address\r\n");
                return;
            }
            for(i = saddr; i <= eaddr; ++i){
                node_list[i] = i;
            }
        }
        else if(type == 2){
            node_list[saddr] = saddr;
        }
        else{
            node_list[saddr] = 0;
        }
	}
    else{
        vOutputString("invalid parameter\r\n");
        return;
    }
}
#endif /* CONFIG_BT_MESH_TEST */

int blemesh_cli_register(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //aos_cli_register_commands(btStackCmdSet, sizeof(btMeshCmdSet)/sizeof(btMeshCmdSet[0]));
    return 0;
}
