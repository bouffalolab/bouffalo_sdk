#include "shell.h"
#include <FreeRTOS.h>
#include "task.h"
#include "board.h"

#include "bluetooth.h"
#include "conn.h"
#if defined(BL702) || defined(BL602)
#include "ble_lib_api.h"
#elif defined(BL616)
#include "btble_lib_api.h"
#include "bl616_glb.h"
#include "rfparam_adapter.h"
#elif defined(BL808)
#include "btble_lib_api.h"
#include "bl808_glb.h"
#endif

#include "hci_driver.h"
#include "hci_core.h"

#include "src/include/mesh.h"
#include "mesh_model_opcode.h"
#include "net.h"
#include "mesh_settings.h"
#include "gen_srv.h"
#include "gen_cli.h"

#include "bflb_mtd.h"

void model_gen_cb(uint8_t value)
{
    printf("value=%d\r\n",value);
}

static u8_t dev_uuid[16];
static u8_t auth_value[16];

static void link_open(bt_mesh_prov_bearer_t bearer)
{
	printf("Provisioning link opened on %x\r\n", bearer);
}

static void link_close(bt_mesh_prov_bearer_t bearer)
{
	printf("Provisioning link closed on %x\r\n", bearer);
}

static void prov_complete(u16_t net_idx, u16_t addr)
{
	printf("Local node provisioned, net_idx 0x%04x address 0x%04x\r\n", net_idx, addr);
}

static void prov_reset(void)
{
	printf("The local node has been reset and needs reprovisioning\r\n");
}

static int output_number(bt_mesh_output_action_t action, u32_t number)
{
	printf("OOB Number: %u\r\n", number);
	return 0;
}

static int output_string(const char *str)
{
	printf("OOB String: %s\r\n", str);
	return 0;
}

static void prov_input_complete(void)
{
	printf("Input complete\r\n");
}

static int input(bt_mesh_input_action_t act, u8_t size)
{
	switch (act) {
	case BT_MESH_ENTER_NUMBER:{
		printf("Enter a number (max %u digits) with: input-num <num>:", size);
	}break;
	case BT_MESH_ENTER_STRING:{
		printf("Enter a string (max %u chars) with: input-str <str>\r\n", size);
	}break;
	default:
		printf("Unknown input action %u (size %u) requested!\r\n", act, size);
		return -EINVAL;
	}
	return 0;
}

static struct bt_mesh_prov prov = {
	.uuid = dev_uuid,
    .oob_info = 0,
	.link_open = link_open,
	.link_close = link_close,
	.complete = prov_complete,
	.reset = prov_reset,
	.static_val = auth_value,
	.static_val_len = 16,
	.output_size = 6,
	.output_actions = (BT_MESH_DISPLAY_NUMBER | BT_MESH_DISPLAY_STRING),
	.output_number = output_number,
	.output_string = output_string,
	.input_size = 6,
	.input_actions = (BT_MESH_ENTER_NUMBER | BT_MESH_ENTER_STRING),
	.input = input,
	.input_complete = prov_input_complete,
};

static void gen_dev_uuid(void)
{
	bt_addr_le_t adv_addr;
	u8_t uuid[10] = {0x07,0xaf,0x00,0x00,0x11,0x11,0x22,0x22,0x33,0x33};
    
    bt_get_local_public_address(&adv_addr);
	memcpy(dev_uuid, uuid, 16 - sizeof(adv_addr.a.val));
    memcpy(&dev_uuid[16 - sizeof(adv_addr.a.val) - 1],  adv_addr.a.val, sizeof( adv_addr.a.val));
}

static struct bt_mesh_cfg_srv cfg_srv = {
	.relay = BT_MESH_RELAY_ENABLED,
	.beacon = BT_MESH_BEACON_ENABLED,
	.frnd = BT_MESH_FRIEND_NOT_SUPPORTED,
	.gatt_proxy = BT_MESH_GATT_PROXY_ENABLED,
	.default_ttl = 3,
	/* 6 transmissions with 20ms interval */
	.net_transmit = BT_MESH_TRANSMIT(5, 20),
	/* 3 transmissions with 20ms interval */
	.relay_retransmit = BT_MESH_TRANSMIT(2, 20),
};

static struct bt_mesh_gen_onoff_srv onoff_srv = {};
//BFL_BLE_MESH_MODEL_PUB_DEFINE(onoff_cli_pub, 2 + 1, ROLE_NODE);
//static bfl_ble_mesh_client_t onoff_client;

static struct bt_mesh_model sig_models[] = {
	BT_MESH_MODEL_CFG_SRV(&cfg_srv),
    BT_MESH_MODEL_GEN_ONOFF(&onoff_srv),
    BT_MESH_MODEL_GEN_ONOFF_CLI(NULL, NULL, NULL)
};

static struct bt_mesh_model vendor_models[] = {
};

static struct bt_mesh_elem elements[] = {
	BT_MESH_ELEM(0, sig_models, vendor_models),
};

static const struct bt_mesh_comp comp = {
	.cid = BL_COMP_ID,
	.elem = elements,
	.elem_count = ARRAY_SIZE(elements),
};

int mesh_start(void)
{
    int err;

    gen_dev_uuid();

    err = bt_mesh_init(&prov, &comp);
    if(err){
        printf("Failed to init \r\n");
        return -1;
    }

    printf("Init successfully \r\n");

    if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
        mesh_set();
        mesh_commit();
    }

    if (bt_mesh_is_provisioned()) {
		printf("Mesh network restored from flash\r\n");
	} else {
        err = bt_mesh_prov_enable(BT_MESH_PROV_GATT);
		if (err) {
			printf("Failed to enable PB-GATT (err %d)\r\n", err);
		} else {
			printf("PB-GATT enabled\r\n");
		}
	}

    mesh_gen_srv_callback_register(model_gen_cb);

    return 0;
}
