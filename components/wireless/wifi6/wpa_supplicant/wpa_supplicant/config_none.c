/*
 * WPA Supplicant / Configuration backend for embedded MACSW targets
 * Copyright (c) 2003-2005, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"

#include "common.h"
#include "config.h"

#ifdef CONFIG_WIFI_P2P_PERSISTENT_GROUP
#include "crc32.h"
#include "easyflash.h"
#include "bflb_mtd.h"

#define P2P_CONFIG_NAME "p2p"
#define P2P_GROUP_STORE_KEY "wpa.p2p.groups"
#define P2P_GROUP_STORE_MAGIC 0x47503250U
#define P2P_GROUP_STORE_VERSION 1
#define P2P_GROUP_STORE_HEADER_LEN 20
#define P2P_GROUP_STORE_RECORD_LEN 64
#define P2P_GROUP_STORE_PSK_LEN (ETH_ALEN + 32 + sizeof(u8))
#define P2P_GROUP_STORE_MAX_LEN (24 * 1024)

static int p2p_group_store_initialized;

static int p2p_group_store_init(void)
{
	if (p2p_group_store_initialized)
		return 0;

	bflb_mtd_init();
	if (easyflash_init() != EF_NO_ERR) {
		wpa_printf(MSG_ERROR, "P2P: Persistent storage init failed");
		return -1;
	}

	p2p_group_store_initialized = 1;
	return 0;
}

static int p2p_group_store_name(const char *name)
{
	return name && os_strcmp(name, P2P_CONFIG_NAME) == 0;
}

static int p2p_group_store_profile(const struct wpa_ssid *ssid)
{
	return ssid->disabled == 2 && ssid->p2p_persistent_group &&
		ssid->ssid && ssid->ssid_len > 0 && ssid->ssid_len <= SSID_MAX_LEN;
}

static int p2p_group_store_add_size(size_t *total, size_t len)
{
	if (len > P2P_GROUP_STORE_MAX_LEN ||
	    *total > P2P_GROUP_STORE_MAX_LEN - len)
		return -1;
	*total += len;
	return 0;
}

static int p2p_group_store_measure(const struct wpa_config *config,
				   size_t *total_len, u32 *group_count)
{
	const struct wpa_ssid *ssid;
	size_t total = P2P_GROUP_STORE_HEADER_LEN;
	u32 count = 0;

	for (ssid = config->ssid; ssid; ssid = ssid->next) {
		size_t passphrase_len;
		size_t psk_count;

		if (!p2p_group_store_profile(ssid))
			continue;
		passphrase_len = ssid->passphrase ? os_strlen(ssid->passphrase) : 0;
		psk_count = dl_list_len(&ssid->psk_list);
		if (passphrase_len > 63 ||
		    ssid->num_p2p_clients > P2P_MAX_STORED_CLIENTS ||
		    psk_count > P2P_MAX_STORED_CLIENTS ||
		    count == P2P_GROUP_STORE_MAX_GROUPS ||
		    p2p_group_store_add_size(&total,
			P2P_GROUP_STORE_RECORD_LEN + ssid->ssid_len +
			passphrase_len + ssid->num_p2p_clients * 2 * ETH_ALEN +
			psk_count * P2P_GROUP_STORE_PSK_LEN))
			return -1;
		count++;
	}

	*total_len = total;
	*group_count = count;
	return 0;
}

static void p2p_group_store_put_record(u8 **pos, const struct wpa_ssid *ssid)
{
	struct psk_list_entry *entry;
	size_t passphrase_len = ssid->passphrase ? os_strlen(ssid->passphrase) : 0;
	size_t psk_count = dl_list_len(&ssid->psk_list);
	size_t record_len = P2P_GROUP_STORE_RECORD_LEN + ssid->ssid_len +
		passphrase_len + ssid->num_p2p_clients * 2 * ETH_ALEN +
		psk_count * P2P_GROUP_STORE_PSK_LEN;
	u8 *p = *pos;

	WPA_PUT_LE32(p, record_len);
	p += 4;
	WPA_PUT_LE32(p, (u32) ssid->id);
	p += 4;
	*p++ = (u8) ssid->mode;
	*p++ = (u8) ssid->pbss;
	*p++ = (u8) ssid->ssid_len;
	*p++ = (u8) passphrase_len;
	*p++ = !!ssid->psk_set;
	*p++ = !!ssid->bssid_set;
	WPA_PUT_LE16(p, ssid->num_p2p_clients);
	p += 2;
	WPA_PUT_LE16(p, psk_count);
	p += 2;
	WPA_PUT_LE16(p, 0);
	p += 2;
	os_memcpy(p, ssid->bssid, ETH_ALEN);
	p += ETH_ALEN;
	os_memcpy(p, ssid->go_p2p_dev_addr, ETH_ALEN);
	p += ETH_ALEN;
	os_memcpy(p, ssid->psk, sizeof(ssid->psk));
	p += sizeof(ssid->psk);
	os_memcpy(p, ssid->ssid, ssid->ssid_len);
	p += ssid->ssid_len;
	if (passphrase_len) {
		os_memcpy(p, ssid->passphrase, passphrase_len);
		p += passphrase_len;
	}
	if (ssid->num_p2p_clients) {
		size_t len = ssid->num_p2p_clients * 2 * ETH_ALEN;

		os_memcpy(p, ssid->p2p_client_list, len);
		p += len;
	}
	dl_list_for_each(entry, &ssid->psk_list, struct psk_list_entry, list) {
		os_memcpy(p, entry->addr, ETH_ALEN);
		p += ETH_ALEN;
		os_memcpy(p, entry->psk, sizeof(entry->psk));
		p += sizeof(entry->psk);
		*p++ = entry->p2p;
	}

	*pos = p;
}

static int p2p_group_store_write(struct wpa_config *config)
{
	struct wpa_ssid *ssid;
	u8 *blob, *pos;
	size_t total_len;
	u32 group_count;
	int ret = -1;

	if (p2p_group_store_init())
		return -1;
	if (p2p_group_store_measure(config, &total_len, &group_count)) {
		wpa_printf(MSG_ERROR, "P2P: Persistent group data is too large");
		return -1;
	}
	blob = os_zalloc(total_len);
	if (!blob)
		return -1;
	WPA_PUT_LE32(blob, P2P_GROUP_STORE_MAGIC);
	WPA_PUT_LE16(blob + 4, P2P_GROUP_STORE_VERSION);
	WPA_PUT_LE16(blob + 6, P2P_GROUP_STORE_HEADER_LEN);
	WPA_PUT_LE32(blob + 8, total_len);
	WPA_PUT_LE32(blob + 12, group_count);
	pos = blob + P2P_GROUP_STORE_HEADER_LEN;
	for (ssid = config->ssid; ssid; ssid = ssid->next) {
		if (p2p_group_store_profile(ssid))
			p2p_group_store_put_record(&pos, ssid);
	}
	WPA_PUT_LE32(blob + 16,
		     crc32(blob + P2P_GROUP_STORE_HEADER_LEN,
			   total_len - P2P_GROUP_STORE_HEADER_LEN));

	if (ef_set_env_blob(P2P_GROUP_STORE_KEY, blob, total_len) == EF_NO_ERR)
		ret = 0;
	else
		wpa_printf(MSG_ERROR, "P2P: Failed to write persistent group data");
	bin_clear_free(blob, total_len);
	return ret;
}

static int p2p_group_store_get_record(struct wpa_config *config,
				      const u8 **pos, const u8 *end,
				      int *loaded_id)
{
	const u8 *record = *pos;
	const u8 *p;
	struct wpa_ssid *ssid;
	struct psk_list_entry *entry;
	size_t record_len, expected_len;
	size_t ssid_len, passphrase_len, client_count, psk_count, i;
	int id, mode, pbss, psk_set, bssid_set;

	if ((size_t) (end - record) < P2P_GROUP_STORE_RECORD_LEN)
		return -1;
	record_len = WPA_GET_LE32(record);
	if (record_len < P2P_GROUP_STORE_RECORD_LEN ||
	    record_len > (size_t) (end - record))
		return -1;
	p = record + 4;
	id = (int) WPA_GET_LE32(p);
	p += 4;
	mode = *p++;
	pbss = *p++;
	ssid_len = *p++;
	passphrase_len = *p++;
	psk_set = *p++;
	bssid_set = *p++;
	client_count = WPA_GET_LE16(p);
	p += 2;
	psk_count = WPA_GET_LE16(p);
	p += 4;

	expected_len = P2P_GROUP_STORE_RECORD_LEN + ssid_len + passphrase_len +
		client_count * 2 * ETH_ALEN +
		psk_count * P2P_GROUP_STORE_PSK_LEN;
	if (id < 0 || wpa_config_get_network(config, id) ||
	    (mode != WPAS_MODE_INFRA && mode != WPAS_MODE_P2P_GO) ||
	    pbss > 1 || !ssid_len || ssid_len > SSID_MAX_LEN ||
	    (passphrase_len && (passphrase_len < 8 || passphrase_len > 63)) ||
	    psk_set > 1 || bssid_set > 1 ||
	    client_count > P2P_MAX_STORED_CLIENTS ||
	    psk_count > P2P_MAX_STORED_CLIENTS || expected_len != record_len)
		return -1;

	ssid = wpa_config_add_network(config);
	if (!ssid)
		return -1;
	ssid->id = id;
	wpa_config_set_network_defaults(ssid);
	ssid->p2p_group = 1;
	ssid->p2p_persistent_group = 1;
	ssid->disabled = 2;
	ssid->mode = mode;
	ssid->pbss = pbss;
	ssid->bssid_set = bssid_set;
	ssid->auth_alg = WPA_AUTH_ALG_OPEN;
	ssid->key_mgmt = WPA_KEY_MGMT_PSK;
	ssid->proto = WPA_PROTO_RSN;
	ssid->pairwise_cipher = pbss ? WPA_CIPHER_GCMP : WPA_CIPHER_CCMP;
	ssid->group_cipher = ssid->pairwise_cipher;
	ssid->export_keys = 1;
	os_memcpy(ssid->bssid, p, ETH_ALEN);
	p += ETH_ALEN;
	os_memcpy(ssid->go_p2p_dev_addr, p, ETH_ALEN);
	p += ETH_ALEN;
	os_memcpy(ssid->psk, p, sizeof(ssid->psk));
	ssid->psk_set = psk_set;
	p += sizeof(ssid->psk);
	ssid->ssid = os_memdup(p, ssid_len);
	if (!ssid->ssid)
		goto fail;
	ssid->ssid_len = ssid_len;
	p += ssid_len;
	if (passphrase_len) {
		ssid->passphrase = os_malloc(passphrase_len + 1);
		if (!ssid->passphrase)
			goto fail;
		os_memcpy(ssid->passphrase, p, passphrase_len);
		ssid->passphrase[passphrase_len] = '\0';
		p += passphrase_len;
	}
	if (client_count) {
		size_t len = client_count * 2 * ETH_ALEN;

		ssid->p2p_client_list = os_memdup(p, len);
		if (!ssid->p2p_client_list)
			goto fail;
		ssid->num_p2p_clients = client_count;
		p += len;
	}
	for (i = 0; i < psk_count; i++) {
		entry = os_zalloc(sizeof(*entry));
		if (!entry)
			goto fail;
		os_memcpy(entry->addr, p, ETH_ALEN);
		p += ETH_ALEN;
		os_memcpy(entry->psk, p, sizeof(entry->psk));
		p += sizeof(entry->psk);
		entry->p2p = *p++;
		dl_list_add_tail(&ssid->psk_list, &entry->list);
	}

	*loaded_id = id;
	*pos = record + record_len;
	return 0;

fail:
	wpa_config_remove_network(config, id);
	return -1;
}

static int p2p_group_store_read(struct wpa_config *config)
{
	u8 *blob = NULL;
	const u8 *pos, *end;
	size_t total_len = 0, read_len;
	u32 group_count, stored_crc;
	int *loaded_ids = NULL;
	size_t loaded = 0, i;
	int ret = -1;

	if (p2p_group_store_init())
		return -1;
	ef_get_env_blob(P2P_GROUP_STORE_KEY, NULL, 0, &total_len);
	if (!total_len)
		return 0;
	if (total_len < P2P_GROUP_STORE_HEADER_LEN ||
	    total_len > P2P_GROUP_STORE_MAX_LEN)
		goto out;
	blob = os_malloc(total_len);
	if (!blob)
		goto out;
	read_len = ef_get_env_blob(P2P_GROUP_STORE_KEY, blob, total_len, NULL);
	if (read_len != total_len ||
	    WPA_GET_LE32(blob) != P2P_GROUP_STORE_MAGIC ||
	    WPA_GET_LE16(blob + 4) != P2P_GROUP_STORE_VERSION ||
	    WPA_GET_LE16(blob + 6) != P2P_GROUP_STORE_HEADER_LEN ||
	    WPA_GET_LE32(blob + 8) != total_len)
		goto out;
	group_count = WPA_GET_LE32(blob + 12);
	stored_crc = WPA_GET_LE32(blob + 16);
	if (group_count > P2P_GROUP_STORE_MAX_GROUPS ||
	    stored_crc != crc32(blob + P2P_GROUP_STORE_HEADER_LEN,
				  total_len - P2P_GROUP_STORE_HEADER_LEN))
		goto out;
	if (group_count) {
		loaded_ids = os_calloc(group_count, sizeof(*loaded_ids));
		if (!loaded_ids)
			goto out;
	}

	pos = blob + P2P_GROUP_STORE_HEADER_LEN;
	end = blob + total_len;
	for (i = 0; i < group_count; i++) {
		if (p2p_group_store_get_record(config, &pos, end,
					       &loaded_ids[loaded]))
			goto out;
		loaded++;
	}
	if (pos != end)
		goto out;

	wpa_printf(MSG_INFO, "P2P: Restored %u persistent group(s)",
		   (unsigned int) group_count);
	ret = 0;

out:
	if (ret) {
		while (loaded)
			wpa_config_remove_network(config, loaded_ids[--loaded]);
		wpa_printf(MSG_ERROR, "P2P: Ignoring invalid persistent group data");
	}
	os_free(loaded_ids);
	if (blob)
		bin_clear_free(blob, total_len);
	return ret;
}
#endif /* CONFIG_WIFI_P2P_PERSISTENT_GROUP */

struct wpa_config * wpa_config_read(const char *name, struct wpa_config *cfgp)
{
	struct wpa_config *config;

	if (name == NULL)
		return NULL;
	if (cfgp)
		config = cfgp;
	else
		config = wpa_config_alloc_empty(NULL, NULL);
	if (config == NULL)
		return NULL;

#ifdef CONFIG_WIFI_P2P_PERSISTENT_GROUP
	if (p2p_group_store_name(name)) {
		config->update_config = 1;
		p2p_group_store_read(config);
	}
#endif /* CONFIG_WIFI_P2P_PERSISTENT_GROUP */
	return config;
}

int wpa_config_write(const char *name, struct wpa_config *config)
{
#ifdef CONFIG_WIFI_P2P_PERSISTENT_GROUP
	if (p2p_group_store_name(name))
		return p2p_group_store_write(config);
#else /* CONFIG_WIFI_P2P_PERSISTENT_GROUP */
	(void) name;
	(void) config;
#endif /* CONFIG_WIFI_P2P_PERSISTENT_GROUP */
	return 0;
}
