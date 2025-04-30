#include <zephyr/types.h>
#include <string.h>
#include <bt_errno.h>

#include <bluetooth.h>
#include <conn.h>
#include <gatt.h>
#include "bt_uuid.h"
#include <__assert.h>
#include <net/buf.h>
#include <bt_log.h>
#include "dynamic_gatts.h"
#include <byteorder.h>

#define CONTROLLER_INDEX 0
#define MAX_BUFFER_SIZE 512
#define MAX_UUID_LEN 16

/* This masks Permission bits from GATT API */
#define GATT_PERM_MASK			(BT_GATT_PERM_READ | \
					 BT_GATT_PERM_READ_AUTHEN | \
					 BT_GATT_PERM_READ_ENCRYPT | \
					 BT_GATT_PERM_WRITE | \
					 BT_GATT_PERM_WRITE_AUTHEN | \
					 BT_GATT_PERM_WRITE_ENCRYPT | \
					 BT_GATT_PERM_PREPARE_WRITE)
#define GATT_PERM_ENC_READ_MASK		(BT_GATT_PERM_READ_ENCRYPT | \
					 BT_GATT_PERM_READ_AUTHEN)
#define GATT_PERM_ENC_WRITE_MASK	(BT_GATT_PERM_WRITE_ENCRYPT | \
					 BT_GATT_PERM_WRITE_AUTHEN)
#define GATT_PERM_READ_AUTHORIZATION	0x40
#define GATT_PERM_WRITE_AUTHORIZATION	0x80

/* GATT server context */
#define SERVER_MAX_SERVICES		2
#define SERVER_MAX_ATTRIBUTES   50
#define SERVER_BUF_SIZE			1024

/* bt_gatt_attr_next cannot be used on non-registered services */
#define NEXT_DB_ATTR(attr) (attr + 1)
#define LAST_DB_ATTR (server_db + (attr_count - 1))

#define server_buf_push(_len)	net_buf_push(server_buf, ROUND_UP(_len, 4))
#define server_buf_pull(_len)	net_buf_pull(server_buf, ROUND_UP(_len, 4))

static struct bt_gatt_service server_svcs[SERVER_MAX_SERVICES];
static struct bt_gatt_attr server_db[SERVER_MAX_ATTRIBUTES];
static struct net_buf *server_buf;
static struct bt_gatt_attr *db_attr = NULL;
static uint8_t ccc_value;

static dynamic_gatt_wr_callbck_func_t read_callback=NULL;

static dynamic_gatt_wr_callbck_func_t write_callback=NULL;

static dynamic_gatt_noti_callbck_func_t noti_callback=NULL;

#if !defined(BFLB_STATIC_ALLOC_MEM)
NET_BUF_POOL_DEFINE(dynamic_gatt_pool, 1, SERVER_BUF_SIZE, 0, NULL);
#else
struct net_buf_pool dynamic_gatt_pool;
#endif

static uint8_t attr_count;
static uint8_t svc_attr_count;
static uint8_t svc_count;

static struct {
	uint16_t len;
	uint8_t buf[MAX_BUFFER_SIZE];
} gatt_buf;

union uuid {
	struct bt_uuid uuid;
	struct bt_uuid_16 u16;
	struct bt_uuid_128 u128;
};

struct gatt_value {
	uint16_t len;
	uint8_t *data;
	uint8_t enc_key_size;
	uint8_t flags[1];
};

struct set_value {
	const uint8_t *value;
	uint16_t len;
};

static void ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	ccc_value = value;
	if(noti_callback!=NULL)
	{
		noti_callback(attr,ccc_value);
	}
}

static void *gatt_buf_add(const void *data, size_t len)
{
	void *ptr = gatt_buf.buf + gatt_buf.len;

	if ((len + gatt_buf.len) > MAX_BUFFER_SIZE) {
		return NULL;
	}

	if (data) {
		memcpy(ptr, data, len);
	} else {
		(void)memset(ptr, 0, len);
	}

	gatt_buf.len += len;

	BT_DBG("%d/%d used", gatt_buf.len, MAX_BUFFER_SIZE);

	return ptr;
}

static void *gatt_buf_reserve(size_t len)
{
	return gatt_buf_add(NULL, len);
}

static void gatt_buf_clear(void)
{
	(void)memset(&gatt_buf, 0, sizeof(gatt_buf));
}


static struct bt_gatt_attr *gatt_db_add(const struct bt_gatt_attr *pattern,
					size_t user_data_len)
{
	const union uuid *u = CONTAINER_OF(pattern->uuid, union uuid, uuid);
	size_t uuid_size = u->uuid.type == BT_UUID_TYPE_16 ? sizeof(u->u16) :
							     sizeof(u->u128);

	/* Return NULL if database is full */
	if (db_attr == &server_db[SERVER_MAX_ATTRIBUTES - 1]) {
		return NULL;
	}

	/* First attribute in db must be service */
	if (!svc_count) {
		return NULL;
	}

	memcpy(db_attr, pattern, sizeof(*db_attr));

	/* Store the UUID. */
	db_attr->uuid = server_buf_push(uuid_size);
	memcpy((void *) db_attr->uuid, &u->uuid, uuid_size);
	BT_DBG("attr %p handle 0x%04x uuid %s",
		       db_attr, db_attr->handle, bt_uuid_str(db_attr->uuid));
	/* Copy user_data to the buffer. */
	if (user_data_len) {
		db_attr->user_data = server_buf_push(user_data_len);
		memcpy(db_attr->user_data, pattern->user_data, user_data_len);
	}

	BT_DBG("handle 0x%04x", db_attr->handle);

	attr_count++;
	svc_attr_count++;
	return db_attr++;
}

static int register_service(void)
{
	int err;

	server_svcs[svc_count-1].attrs = server_db +(attr_count - svc_attr_count);
	server_svcs[svc_count-1].attr_count = svc_attr_count;

	err = bt_gatt_service_register(&server_svcs[svc_count-1]);
	if (!err) {
		/* Service registered, reset the counter */
		svc_attr_count  =  0U;
	}

	return err;
}

int ble_dynamic_unregister_service(void)
{
	int err	= 0;
	for(int i =0;i<svc_count;i++)
	{
		err = bt_gatt_service_unregister(&server_svcs[i]);
	}

	if (!err) {
		attr_count = 0U;
		svc_count = 0U;
		memset(&server_svcs[0],0,sizeof(struct bt_gatt_service)*SERVER_MAX_SERVICES);
		memset(&server_db[0],0,sizeof(struct bt_gatt_attr)*SERVER_MAX_ATTRIBUTES);
		db_attr = server_db;
		server_buf_pull(SERVER_BUF_SIZE);

	}

	return err;
}
static struct bt_gatt_attr ccc = BT_GATT_CCC(ccc_cfg_changed,
					     BT_GATT_PERM_READ |
					     BT_GATT_PERM_WRITE);

static struct bt_gatt_attr *add_ccc(const struct bt_gatt_attr *attr)
{
	struct bt_gatt_attr *attr_desc;
	struct bt_gatt_chrc *chrc = attr->user_data;
	struct gatt_value *value = NEXT_DB_ATTR(attr)->user_data;

	/* Check characteristic properties */
	if (!(chrc->properties &(BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_INDICATE))) {
		return NULL;
	}
	/* Add CCC descriptor to GATT database */
	attr_desc = gatt_db_add(&ccc, 0);
	if (!attr_desc) {
		return NULL;
	}

	return attr_desc;
}

static struct bt_gatt_attr *add_cep(const struct bt_gatt_attr *attr_chrc)
{
	struct bt_gatt_chrc *chrc = attr_chrc->user_data;
	struct bt_gatt_cep cep_value;

	/* Extended Properties bit shall be set */
	if (!(chrc->properties & BT_GATT_CHRC_EXT_PROP)) {
		return NULL;
	}

	cep_value.properties = 0x0000;

	/* Add CEP descriptor to GATT database */
	return gatt_db_add(&(struct bt_gatt_attr) BT_GATT_CEP(&cep_value),
			   sizeof(cep_value));
}

void ble_dynamic_gatt_start_server(void)
{
	if (svc_attr_count) {
		if (register_service()) {
			return;
		}
	}
}

int ble_dynamic_gatt_add_service(struct bt_uuid * uuid,unsigned char uuid_type, unsigned char service_type)
{

	struct bt_gatt_attr *attr_svc = NULL;
	size_t uuid_size = 0 ;

	if(uuid_type==BT_UUID_TYPE_16)
	{
		uuid_size=sizeof(struct bt_uuid_16);
	}
	else if(uuid_type ==BT_UUID_TYPE_128)
	{
		uuid_size=sizeof(struct bt_uuid_128);
	}
#if 0
	/* Register last defined service */
	if (svc_attr_count)
	{
		if (register_service()) 
		{
			return -EINVAL;
		}
	}
#endif
	svc_count++;
	switch (service_type) {
	case GATT_SERVICE_PRIMARY:
	attr_svc = gatt_db_add(&(struct bt_gatt_attr)
				BT_GATT_PRIMARY_SERVICE(uuid),
				uuid_size);

	break;
	case GATT_SERVICE_SECONDARY:
	attr_svc = gatt_db_add(&(struct bt_gatt_attr)
				BT_GATT_SECONDARY_SERVICE(uuid),
				uuid_size);
	break;
	}

	if (!attr_svc) 
	{
		svc_count--;
		return -EINVAL;
	}

	return 0;
}

static ssize_t read_value(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			  void *buf, uint16_t len, uint16_t offset)
{
	if(read_callback)
	{
		int  rd_len =0;
		uint8_t read_data[251]={0};
		read_callback(attr,read_data,&rd_len);
		return bt_gatt_attr_read(conn, attr, buf, len, offset, read_data,
				 rd_len);

	}
	return 0;
}

static ssize_t write_value(struct bt_conn *conn,
			   const struct bt_gatt_attr *attr, const void *buf,
			   uint16_t len, uint16_t offset, uint8_t flags)
{

	/* Don't write anything if prepare flag is set */
	if (flags & BT_GATT_WRITE_FLAG_PREPARE) {
		return 0;
	}

	/* Maximum attribute value size is 512 bytes */
	__ASSERT_NO_MSG(value->len < 512);

	if(write_callback!=NULL)
	{
		write_callback(attr,buf,len);
	}

	return len;
}

int ble_dynamic_gatt_add_characteristic(struct gatt_add_characteristic *ch)
{
	struct bt_gatt_attr *attr_chrc, *attr_value;
	struct bt_gatt_chrc *chrc_data;
	struct gatt_value value;

	/* Add Characteristic Declaration */
	attr_chrc = gatt_db_add(&(struct bt_gatt_attr)
				BT_GATT_ATTRIBUTE(BT_UUID_GATT_CHRC,
						  BT_GATT_PERM_READ,
						  bt_gatt_attr_read_chrc, NULL,
						  (&(struct bt_gatt_chrc){})),
				sizeof(*chrc_data));
	if (!attr_chrc) {
		return -EINVAL;
	}

	(void)memset(&value, 0, sizeof(value));

	if (ch->permissions & GATT_PERM_READ_AUTHORIZATION) {
		/* To maintain backward compatibility, set Read Permission */
		if (!(ch->permissions & GATT_PERM_ENC_READ_MASK)) {
			ch->permissions |= BT_GATT_PERM_READ;

		}
	}

	if (ch->permissions & GATT_PERM_WRITE_AUTHORIZATION) {
		/* To maintain backward compatibility, set Write Permission */
		if (!(ch->permissions & GATT_PERM_ENC_WRITE_MASK)) {
			ch->permissions |= BT_GATT_PERM_WRITE;

		}
	}

	/* Allow prepare writes */
	ch->permissions |= BT_GATT_PERM_PREPARE_WRITE;

	/* Add Characteristic Value */
	attr_value = gatt_db_add(&(struct bt_gatt_attr)
				 BT_GATT_ATTRIBUTE(ch->uuid,
					ch->permissions & GATT_PERM_MASK,
					read_value, write_value, &value),
					sizeof(value));
	if (!attr_value) {
		server_buf_pull(sizeof(*chrc_data));
		/* Characteristic attribute uuid has constant length */
		server_buf_pull(sizeof(uint16_t));
		return -EINVAL;
	}

	chrc_data = attr_chrc->user_data;
	chrc_data->properties = ch->properties;
	chrc_data->uuid = attr_value->uuid;

	ch->char_id = attr_chrc->handle;
	return 0;
}

static int alloc_descriptor(const struct bt_gatt_attr *attr,
			    struct gatt_add_descriptor *d)
{
	struct bt_gatt_attr *attr_desc;
	struct gatt_value value;

	if (!bt_uuid_cmp(d->uuid, BT_UUID_GATT_CEP)) {

		attr_desc = add_cep(attr);
	} else if (!bt_uuid_cmp(d->uuid, BT_UUID_GATT_CCC)) {

		attr_desc = add_ccc(attr);
	} else {

		(void)memset(&value, 0, sizeof(value));

		if (d->permissions & GATT_PERM_READ_AUTHORIZATION) {
		/*
		* To maintain backward compatibility,
		* set Read Permission
		*/
		if (!(d->permissions & GATT_PERM_ENC_READ_MASK)) {
			d->permissions |= BT_GATT_PERM_READ;
		}
	}

	if (d->permissions & GATT_PERM_WRITE_AUTHORIZATION) {

		/*
		* To maintain backward compatibility,
		* set Write Permission
		*/
		if (!(d->permissions & GATT_PERM_ENC_WRITE_MASK)) {
			d->permissions |= BT_GATT_PERM_WRITE;
		}
	}

	/* Allow prepare writes */
	d->permissions |= BT_GATT_PERM_PREPARE_WRITE;

	attr_desc = gatt_db_add(&(struct bt_gatt_attr)
							BT_GATT_DESCRIPTOR(d->uuid,
							d->permissions & GATT_PERM_MASK,
							read_value, write_value,
							&value), sizeof(value));
	}

	if (!attr_desc) {
		return -EINVAL;
	}

	d->desc_id = attr_desc->handle;

	return 0;
}

static struct bt_gatt_attr *get_base_chrc(struct bt_gatt_attr *attr)
{
	struct bt_gatt_attr *tmp;

	for (tmp = attr; tmp > server_db; tmp--) {
		/* Service Declaration cannot precede Descriptor declaration */
		if (!bt_uuid_cmp(tmp->uuid, BT_UUID_GATT_PRIMARY) ||
		    !bt_uuid_cmp(tmp->uuid, BT_UUID_GATT_SECONDARY)) {
			break;
		}

		if (!bt_uuid_cmp(tmp->uuid, BT_UUID_GATT_CHRC)) {
			return tmp;
		}
	}

	return NULL;
}

int ble_dynamic_gatt_add_descriptor(struct gatt_add_descriptor *cmd)
{
	struct bt_gatt_attr *chrc;

	/* Must be declared first svc or at least 3 attrs (svc+char+char val) */
	if (!svc_count || attr_count < 3) {
		return -EINVAL;
	}

	/* Lookup preceding Characteristic Declaration here */
	chrc = get_base_chrc(LAST_DB_ATTR);
	if (!chrc) {
		return -EINVAL;
	}
	if (alloc_descriptor(chrc, cmd)) {
		return -EINVAL;
	}

	return 0;
}

static int alloc_included(struct bt_gatt_attr *attr,
			  uint16_t *included_service_id, uint16_t svc_handle)
{
	struct bt_gatt_attr *attr_incl;

	/*
	 * user_data_len is set to 0 to NOT allocate memory in server_buf for
	 * user_data, just to assign to it attr pointer.
	 */
	attr_incl = gatt_db_add(&(struct bt_gatt_attr)
				BT_GATT_INCLUDE_SERVICE(attr), 0);

	if (!attr_incl) {
		return -EINVAL;
	}

	attr_incl->user_data = attr;

	*included_service_id = attr_incl->handle;
	
	return 0;
}

int ble_dynamic_gatt_add_included(uint8_t svc_id)
{
	struct bt_gatt_attr *svc;
	uint16_t included_service_id = 0U;

	if (!svc_count || !svc_id) {
		return -EINVAL;
	}

	svc = &server_db[svc_id - 1];

	/* Fail if attribute stored under requested handle is not a service */
	if (bt_uuid_cmp(svc->uuid, BT_UUID_GATT_PRIMARY) &&
	    bt_uuid_cmp(svc->uuid, BT_UUID_GATT_SECONDARY)) {
		return -EINVAL;
	}

	if (alloc_included(svc, &included_service_id, svc_id)) {
		return -EINVAL;
	}

    return 0;
}


struct bt_gatt_attr* ble_dynamic_gatt_get_attr(struct bt_uuid *uuid)
{
	for(int i=0;i<SERVER_MAX_ATTRIBUTES;i++)
    {
		if(!bt_uuid_cmp(server_db[i].uuid, uuid)) 
			return &server_db[i];
	}
	return NULL;

}

void ble_dynamic_gatt_server_init(void)
{
	#if (BFLB_STATIC_ALLOC_MEM)
	net_buf_init(GATTSERVER, &dynamic_gatt_pool, 1, SERVER_BUF_SIZE, NULL);
	#else
	net_buf_init(&dynamic_gatt_pool, 1, SERVER_BUF_SIZE, NULL);
	#endif /* BFLB_STATIC_ALLOC_MEM */

	server_buf = net_buf_alloc(&dynamic_gatt_pool, K_NO_WAIT);

	net_buf_reserve(server_buf, SERVER_BUF_SIZE);
	db_attr = server_db;

}

void ble_dynamic_gatt_server_deinit(void)
{
	net_buf_deinit(&dynamic_gatt_pool);

	read_callback  = NULL;
	write_callback = NULL;
	noti_callback  = NULL;
}

void ble_dynamic_gatt_cb_register(dynamic_gatt_rd_callbck_func_t rb,dynamic_gatt_wr_callbck_func_t wb,dynamic_gatt_noti_callbck_func_t nb)
{
	if(rb)
	{
		read_callback=rb;
	}
	if(wb)
	{
		write_callback=wb;
	}
	if(nb)
	{
		noti_callback=nb;
	}
}
