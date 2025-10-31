#if defined(BL702) || defined(BL602)
#include "ble_lib_api.h"
#else
#include "btble_lib_api.h"
#endif
#include "bluetooth.h"
#include "conn.h"
#include "hci_core.h"
#include "hci_driver.h"
#include "byteorder.h"
#include "bt_log.h"
#include <bt_errno.h>
#if defined(CONFIG_BT_HOST_HCI_TL)
#if defined(CONFIG_BT_HOST_HCI)
#include "bflb_hci_tl.h"
#else
#include "bl_hci_tl.h"
#include "bl_gpio.h"
#endif
#endif

struct hast_le_adv_data{
    u8_t ad[31];
	size_t ad_len;
};

static struct bt_le_scan_param hast_le_scan_param;
static struct bt_le_adv_param hast_le_adv_param;
static struct hast_le_adv_data hast_le_ad;
static struct hast_le_adv_data hast_le_sd;
static bt_le_scan_cb_t *hast_le_scan_cb;

static void hast_ble_scan_assist_cb(const struct bt_le_scan_param *param, bt_le_scan_cb_t cb);
static void hast_ble_adv_assist_cb(const struct bt_le_adv_param *param, const struct bt_data *ad, 
		size_t ad_len, const struct bt_data *sd, size_t sd_len);

static struct hast_cb assist_cb = {
    .le_scan_cb = hast_ble_scan_assist_cb,
	.le_adv_cb = hast_ble_adv_assist_cb,
};

extern struct bt_dev bt_dev;

static void hast_ble_scan_assist_cb(const struct bt_le_scan_param *param, bt_le_scan_cb_t cb)
{
    memcpy(&hast_le_scan_param, param, sizeof(struct bt_le_scan_param));
	hast_le_scan_cb = cb;
}

static void hast_ble_get_ad(const struct bt_data *ad, size_t ad_len,  uint8_t *output)
{
    int i;
	uint8_t data_len = 0;

	for (i = 0; i < ad_len; i++) {
		*(output + data_len) = ad[i].type;
		data_len++;

        *(output + data_len) = ad[i].data_len;
		data_len++;

		memcpy(output + data_len, ad[i].data, ad[i].data_len);
		
		data_len += ad[i].data_len;
	}
}

static void hast_ble_construct_ad(struct hast_le_adv_data *adv_data, struct bt_data *output)
{
     int i;
     size_t ad_len = adv_data->ad_len;
	 u8_t *p_ad = adv_data->ad;
	 

	 for(i = 0; i < ad_len; i++){
         memcpy(&output[i], p_ad, 2);//type, data_len
         p_ad += 2;
         output[i].data = (const u8_t *)p_ad;
	     p_ad += output[i].data_len;
	 }
}

static void hast_ble_adv_assist_cb(const struct bt_le_adv_param *param, const struct bt_data *ad, 
		size_t ad_len, const struct bt_data *sd, size_t sd_len)
{
    memcpy(&hast_le_adv_param, param, sizeof(struct bt_le_adv_param));

	if(ad){	
    	hast_le_ad.ad_len = ad_len;
    	memset(hast_le_ad.ad, 0, sizeof(hast_le_ad.ad));
    	hast_ble_get_ad(ad, ad_len, hast_le_ad.ad);
	}

	if(sd){
    	hast_le_sd.ad_len = sd_len;
    	memset(hast_le_sd.ad, 0, sizeof(hast_le_sd.ad));
    	hast_ble_get_ad(sd, sd_len, hast_le_sd.ad);
	}
}

static int hast_common_reset(void)
{
    
	struct net_buf *rsp;
	int err;
	
    if (!(bt_dev.drv->quirks & BT_QUIRK_NO_RESET)) {
		/* Send HCI_RESET */
		err = bt_hci_cmd_send_sync(BT_HCI_OP_RESET, NULL, &rsp);
		if (err) {
			return err;
		}
		bt_hci_reset_complete(rsp);
		net_buf_unref(rsp);
	}

	#if defined(CONFIG_BT_HCI_ACL_FLOW_CONTROL)
	err = bt_set_flow_control();
	if (err) {
		return err;
	}
    #endif /* CONFIG_BT_HCI_ACL_FLOW_CONTROL */

	return 0;
}

static int hast_ble_reset(void)
{
    struct bt_hci_cp_write_le_host_supp *cp_le;
	struct net_buf *buf, *rsp;
	int err;

	if (!BT_FEAT_LE(bt_dev.features)) {
		BT_ERR("Non-LE capable controller detected!");
		return -ENODEV;
	}

	if (BT_FEAT_BREDR(bt_dev.features)) {
		buf = bt_hci_cmd_create(BT_HCI_OP_LE_WRITE_LE_HOST_SUPP,
					sizeof(*cp_le));
		if (!buf) {
			return -ENOBUFS;
		}

		cp_le = net_buf_add(buf, sizeof(*cp_le));

		/* Explicitly enable LE for dual-mode controllers */
		cp_le->le = 0x01;
		cp_le->simul = 0x00;
		err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_WRITE_LE_HOST_SUPP, buf,
					   NULL);
		if (err) {
			return err;
		}
	}

	if (IS_ENABLED(CONFIG_BT_CONN) &&
	    IS_ENABLED(CONFIG_BT_DATA_LEN_UPDATE) &&
	    BT_FEAT_LE_DLE(bt_dev.le.features)) {
		struct bt_hci_cp_le_write_default_data_len *cp;
		struct bt_hci_rp_le_read_max_data_len *rp;
		u16_t tx_octets, tx_time;

		err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_READ_MAX_DATA_LEN, NULL,
					   &rsp);
		if (err) {
			return err;
		}

		rp = (void *)rsp->data;
		tx_octets = sys_le16_to_cpu(rp->max_tx_octets);
		tx_time = sys_le16_to_cpu(rp->max_tx_time);
		net_buf_unref(rsp);

		buf = bt_hci_cmd_create(BT_HCI_OP_LE_WRITE_DEFAULT_DATA_LEN,
					sizeof(*cp));
		if (!buf) {
			return -ENOBUFS;
		}

		cp = net_buf_add(buf, sizeof(*cp));
		cp->max_tx_octets = sys_cpu_to_le16(tx_octets);
		cp->max_tx_time = sys_cpu_to_le16(tx_time);

		err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_WRITE_DEFAULT_DATA_LEN,
					   buf, NULL);
		if (err) {
			return err;
		}
	}

	return bt_le_set_event_mask();
}

#if defined(CONFIG_BT_BREDR)
static int hast_br_reset(void)
{  
    struct net_buf *buf;
	struct bt_hci_cp_write_ssp_mode *ssp_cp;
	struct bt_hci_cp_write_inquiry_mode *inq_cp;
	struct bt_hci_write_local_name *name_cp;
	int err;

    
    /* Set SSP mode */
	buf = bt_hci_cmd_create(BT_HCI_OP_WRITE_SSP_MODE, sizeof(*ssp_cp));
	if (!buf) {
		return -ENOBUFS;
	}

	ssp_cp = net_buf_add(buf, sizeof(*ssp_cp));
	ssp_cp->mode = 0x01;
	err = bt_hci_cmd_send_sync(BT_HCI_OP_WRITE_SSP_MODE, buf, NULL);
	if (err) {
		return err;
	}

	/* Enable Inquiry results with RSSI or extended Inquiry */
	buf = bt_hci_cmd_create(BT_HCI_OP_WRITE_INQUIRY_MODE, sizeof(*inq_cp));
	if (!buf) {
		return -ENOBUFS;
	}

	inq_cp = net_buf_add(buf, sizeof(*inq_cp));
	inq_cp->mode = 0x02;
	err = bt_hci_cmd_send_sync(BT_HCI_OP_WRITE_INQUIRY_MODE, buf, NULL);
	if (err) {
		return err;
	}

	/* Set local name */
	buf = bt_hci_cmd_create(BT_HCI_OP_WRITE_LOCAL_NAME, sizeof(*name_cp));
	if (!buf) {
		return -ENOBUFS;
	}

	name_cp = net_buf_add(buf, sizeof(*name_cp));
	strlcpy((char *)name_cp->local_name, CONFIG_BT_DEVICE_NAME,
		sizeof(name_cp->local_name));

	err = bt_hci_cmd_send_sync(BT_HCI_OP_WRITE_LOCAL_NAME, buf, NULL);
	if (err) {
		return err;
	}

	/* Set page timeout*/
	buf = bt_hci_cmd_create(BT_HCI_OP_WRITE_PAGE_TIMEOUT, sizeof(u16_t));
	if (!buf) {
		return -ENOBUFS;
	}

	net_buf_add_le16(buf, CONFIG_BT_PAGE_TIMEOUT);

	err = bt_hci_cmd_send_sync(BT_HCI_OP_WRITE_PAGE_TIMEOUT, buf, NULL);
	if (err) {
		return err;
	}

	
	/* Enable BR/EDR SC if supported */
	if (BT_FEAT_SC(bt_dev.features)) {
		struct bt_hci_cp_write_sc_host_supp *sc_cp;

		buf = bt_hci_cmd_create(BT_HCI_OP_WRITE_SC_HOST_SUPP,
					sizeof(*sc_cp));
		if (!buf) {
			return -ENOBUFS;
		}

		sc_cp = net_buf_add(buf, sizeof(*sc_cp));
		sc_cp->sc_support = 0x01;

		err = bt_hci_cmd_send_sync(BT_HCI_OP_WRITE_SC_HOST_SUPP, buf,
					   NULL);
		if (err) {
			return err;
		}
	}

	return 0;
	
}
#endif

static int  hast_host_hci_reset(void)
{
    int err;
	ATOMIC_DEFINE(old_flags, BT_DEV_NUM_FLAGS);
	memcpy(old_flags, bt_dev.flags, sizeof(old_flags));

	err = hast_common_reset();
	
	if (err) {
		return err;
	}

	err = hast_ble_reset();
	if (err) {
		return err;
	}

    #if defined(CONFIG_BT_BREDR)
    if (BT_FEAT_BREDR(bt_dev.features)) {
		err = hast_br_reset();
		if (err) {
			return err;
		}
	}
	#endif

	err = bt_set_event_mask();
	if (err) {
		return err;
	}

    memcpy(bt_dev.flags, old_flags, sizeof(old_flags));
	
	return 0;
}

static void hast_host_state_restore(void)
{
    struct bt_data *ad = NULL;
	struct bt_data *sd = NULL;
    k_sem_give(&bt_dev.ncmd_sem);
    net_buf_unref(bt_dev.sent_cmd);
    bt_dev.sent_cmd = NULL;

	hast_host_hci_reset();

    #if defined(CONFIG_BT_CONN)
	bt_conn_cleanup_all();
	#endif

    atomic_set_bit(bt_dev.flags, BT_DEV_ASSIST_RUN);
	
    #if defined(CONFIG_BT_OBSERVER)
	if(atomic_test_bit(bt_dev.flags, BT_DEV_EXPLICIT_SCAN))
	{
	     BT_WARN("Restore BLE scan\r\n");
        atomic_clear_bit(bt_dev.flags, BT_DEV_EXPLICIT_SCAN);
	    atomic_clear_bit(bt_dev.flags, BT_DEV_SCANNING);
		bt_le_scan_start((const struct bt_le_scan_param *)&hast_le_scan_param, hast_le_scan_cb);
	}
    #endif
	
	if(atomic_test_and_clear_bit(bt_dev.flags, BT_DEV_ADVERTISING))
	{
	     BT_WARN("Restore BLE advertising\r\n");
	    if(hast_le_ad.ad_len > 0)
	    {
	        ad = k_malloc(sizeof(struct bt_data) * hast_le_ad.ad_len);
	        hast_ble_construct_ad(&hast_le_ad, ad);
	    }
		if(hast_le_sd.ad_len > 0)
		{
		    sd = k_malloc(sizeof(struct bt_data) * hast_le_sd.ad_len);
	        hast_ble_construct_ad(&hast_le_sd, sd);
		}
			
        bt_le_adv_start((const struct bt_le_adv_param *)&hast_le_adv_param, ad,
			hast_le_ad.ad_len, sd, hast_le_sd.ad_len);

		if(ad)
			k_free(ad);
		if(sd)
			k_free(sd);
	}

	atomic_clear_bit(bt_dev.flags, BT_DEV_ASSIST_RUN);
}

void hast_bt_reset(void)
{
	#if defined(BL602) || defined(BL702)
	ble_controller_reset();
	#else
	#if defined(CONFIG_BT_HOST_HCI_TL)
	#if defined(CONFIG_BT_HOST_HCI)
	bflb_hci_reset();
	#else
	bl_gpio_enable_output(CTRL_RESET_PIN, 0, 0);
	bl_gpio_output_set(CTRL_RESET_PIN, 0);
	k_sleep(10);
	bl_gpio_output_set(CTRL_RESET_PIN, 1);
	k_sleep(500); // wait controller ready
	bl_hci_reset();
	#endif
	#else
	btble_controller_reset();
	#endif  
	#endif
	hast_host_state_restore();
}

void hast_init(void)
{
    memset(&hast_le_ad, 0, sizeof(struct hast_le_adv_data));
	memset(&hast_le_sd, 0, sizeof(struct hast_le_adv_data));
    bt_register_host_assist_cb(&assist_cb);
}
