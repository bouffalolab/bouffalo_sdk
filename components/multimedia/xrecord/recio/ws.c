/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <librws.h>
#include "recio.h"
#include "msp/kernel.h"
#define TAG "rec-ws"

typedef struct _ws_config_t {
    rws_socket sock;
    char *save_name;
    msp_sem_t conn_notify;
} ws_config_t;


#define RECORD_SOCK_MUX

#ifdef RECORD_SOCK_MUX
#define MAX_SOCKET_FD 20
static int s_sock_count = 0;
typedef struct _socks_mgr {
	char *path_pre;
	rws_socket sock;
	msp_mutex_t sendmtx;
} socks_mgr_t;
static socks_mgr_t s_socks_mgr[MAX_SOCKET_FD];
#endif

static rws_bool ws_send_bin(rws_socket _socket, const uint8_t *bin, size_t size)
{
	if (bin && size > 3) {
		int piece_size = size / 3;
		int last_piece_size = piece_size + size % 3;

		return  rws_socket_send_bin_start(_socket, (const char *)&bin[0], piece_size) && \
				rws_socket_send_bin_continue(_socket, (const char *)&bin[piece_size], piece_size) && \
				rws_socket_send_bin_finish(_socket, (const char *)&bin[piece_size * 2], last_piece_size);
	}
	return rws_false;
}

static void on_socket_received_text(rws_socket socket, const char *text, const unsigned int length, bool is_finish)
{
	char *buff = NULL;

	if (!socket || !text || !length)
	{
		LOGE(TAG, "%s: Invalid parameter(s).", __FUNCTION__);
		return;
	}

	buff = (char *)msp_malloc(length + 1);
	if (!buff)
	{
		LOGE(TAG, "%s: Not enough memory. len:%d", __FUNCTION__, length + 1);
		return;
	}

	memcpy(buff, text, length);
	buff[length] = 0;

	LOGD(TAG, "%s: Socket text: %s", __FUNCTION__, buff);
	msp_free(buff);
	buff = NULL;
}

static void on_socket_received_bin(rws_socket socket, const void * data, const unsigned int length, bool is_finish)
{
	if (!socket || !data || !length)
	{
		LOGE(TAG, "%s: Invalid parameter(s).", __FUNCTION__);
		return;
	}
}

static void on_socket_received_pong(rws_socket socket)
{
	if (!socket)
	{
		LOGE(TAG, "%s: Invalid parameter(s).", __FUNCTION__);
		return;
	}
	LOGD(TAG, "received pong!!!!!!!!!!!");
}

static void on_socket_connected(rws_socket socket)
{
    ws_config_t *priv = rws_socket_get_user_object(socket);

	LOGD(TAG, "%s: Socket connected", __FUNCTION__);
    msp_sem_signal(&priv->conn_notify);
}

static void on_socket_disconnected(rws_socket socket)
{
	rws_error error = rws_socket_get_error(socket);
	if (error)
	{
		LOGE(TAG, "%s: Socket disconnect with code, error: %i, %s",
				__FUNCTION__,
				rws_error_get_code(error),
				rws_error_get_description(error));
	}
	LOGD(TAG, "%s: Socket disconnected", __FUNCTION__);

	socket = NULL;
}

static rws_socket ws_rec_init(const char *url)
{
	char *uri;
	int port;
	rws_socket sock = NULL;

	msp_check_return_val(url, NULL);
    char *tbuffer = strdup(url);
	uri = strstr(tbuffer, "ws://"); 
	if (!uri) {
		LOGE(TAG, "ws url is wrong...");
		goto err;  
	}
	uri += strlen("ws://");
	char *str_port = strchr(uri, ':');
	if (str_port) {
		*str_port = '\0';
		str_port ++;
		port = atoi(str_port);
	} else {
		port = 80;
	}
	
    sock = rws_socket_create();
	if (sock == NULL) {
		goto err;
	}

    LOGD(TAG, "ws init, path==%s, port:%d", uri, port);
    rws_socket_set_url(sock, "ws", uri, port, "/");
	rws_socket_set_on_disconnected(sock, &on_socket_disconnected);
	rws_socket_set_on_connected(sock, &on_socket_connected);
	rws_socket_set_on_received_text(sock, &on_socket_received_text);
	rws_socket_set_on_received_bin(sock, &on_socket_received_bin);
	rws_socket_set_on_received_pong(sock, &on_socket_received_pong);

	rws_socket_set_write_timeout(sock, 5000);

	rws_socket_connect(sock);

    // LOGD(TAG, "%s,%d", __func__, __LINE__);
    msp_free(tbuffer);
	return sock;
err:
    msp_free(tbuffer);
    return NULL;
}

static void ws_rec_deinit(ws_config_t *priv)
{
	int cnt;

	if (priv) {
		if (priv->sock) {
#ifdef RECORD_SOCK_MUX
			int cc = 0;
			socks_mgr_t tmp[MAX_SOCKET_FD];

			cnt = 0;
			// backup
			for (int i = 0; i < s_sock_count; i++) {
				tmp[i].path_pre = s_socks_mgr[i].path_pre;
				tmp[i].sock = s_socks_mgr[i].sock;
				tmp[i].sendmtx = s_socks_mgr[i].sendmtx;
			}
			// check is released or not
			for (int i = 0; i < s_sock_count; i++) {
				if (priv->sock == tmp[i].sock) {
					msp_free(tmp[i].path_pre);
					msp_mutex_free(&tmp[i].sendmtx);
					cnt ++;
					continue;
				}
				s_socks_mgr[cc].path_pre = tmp[i].path_pre;
				s_socks_mgr[cc].sock = tmp[i].sock;
				s_socks_mgr[cc].sendmtx = tmp[i].sendmtx;
				cc ++;
			}
			s_sock_count = cc;
			LOGD(TAG, "s_sock_count:%d", s_sock_count);
#else
			cnt = 1;
#endif
			if (cnt > 0) {
				rws_socket_disconnect_and_release(priv->sock);
			}
			LOGD(TAG, "ws deinit");
		}
		if (priv->save_name) {
			msp_free(priv->save_name);
		}
		msp_free(priv);
		priv = NULL;
	}
}

static int ws_open(recio_t *io, const char *path)
{
	int got_same = 0;
	rws_socket sock;

	LOGD(TAG, "ws open path: %s", path);
#ifdef RECORD_SOCK_MUX
	// ws://192.168.1.105:9090/1579607019_mic0_data.pcm
	int i;
	char *re_path = strdup(path);
	char *suffix = strrchr(re_path, '/');
	if (suffix) {
		int len = strlen(re_path) - strlen(suffix);
		re_path[len] = 0;
		LOGD(TAG, "re_path:%s", re_path);
		for (i = 0; i < s_sock_count; i++) {
			if (strcmp(s_socks_mgr[i].path_pre, re_path) == 0) {
				got_same = 1;
				break;
			}
		}
	}

	if (got_same) {
		sock = s_socks_mgr[i].sock;
		LOGD(TAG, "#################use same sock:0x%x", sock);
	} else 
#endif
	{
		sock = ws_rec_init(path);
		LOGD(TAG, "@@@@@@@@@@@@@@@@@new sock:0x%x", sock);
	}

    if (sock) {
        ws_config_t *priv = msp_zalloc_check(sizeof(ws_config_t));
		if (got_same == 0)
        	msp_sem_new(&priv->conn_notify, 0);
		rws_socket_set_user_object(sock, (void *)priv);
        priv->sock = sock;
        char *filename = strrchr(path, '/');
        if (filename) {
            priv->save_name = strdup(++filename);
        } else {
            priv->save_name = strdup("default.pcm");
        }
        LOGD(TAG, "filename: %s", priv->save_name);
        io->private = (void *)priv;
		if (got_same == 0) {
			msp_sem_wait(&priv->conn_notify, 5000);
			msp_sem_free(&priv->conn_notify);
		}
        if (rws_socket_is_connected(sock) == rws_false) {
            LOGE(TAG, "timeout ws is not connected");
			ws_rec_deinit(priv);
			msp_free(re_path);
            return -1;
        }
#ifdef RECORD_SOCK_MUX
		if (got_same == 0) {
			if (s_sock_count < MAX_SOCKET_FD - 1) {
				// add new one
				s_socks_mgr[s_sock_count].sock = sock;
				s_socks_mgr[s_sock_count].path_pre = strdup(re_path);
				msp_mutex_new(&s_socks_mgr[s_sock_count].sendmtx);
				s_sock_count ++;				
			}
		}
		LOGD(TAG, "s_sock_count:%d", s_sock_count);
#endif
		msp_free(re_path);
        return 0;
    }

	msp_free(re_path);
    return -1;
}

static int ws_close(recio_t *io)
{
    ws_config_t *priv = (ws_config_t *)io->private;

	if (priv) {
		LOGD(TAG, "out to disconnect ws[%s]", priv->save_name);
	} else {
		LOGD(TAG, "out to disconnect ws");
	}
	ws_rec_deinit(priv);
    
    return 0;
}

static int ws_read(recio_t *io, uint8_t *buffer, int length, int timeoutms)
{

    return 0;
}

static int ws_write(recio_t *io, uint8_t *buffer, int length, int timeoutms)
{
    int ret;
	msp_mutex_t *p_sendmtx = NULL;
    ws_config_t *priv = (ws_config_t *)io->private;
    rws_socket sock = priv->sock;

#ifdef RECORD_SOCK_MUX
	for (int i = 0; i < s_sock_count; i++) {
		if (sock == s_socks_mgr[i].sock) {
			p_sendmtx = &s_socks_mgr[i].sendmtx;
			break;
		}
	}
	if (p_sendmtx == NULL) {
		LOGE(TAG, "sock maybe released yet!");
		return -1;
	}
#endif
    if (!(sock && rws_socket_is_connected(sock))) {
        LOGE(TAG, "not connected");
        return -1;
    }

    int slen = strlen(priv->save_name) + 1 + length;
    uint8_t *tmpbuf = msp_malloc_check(slen + 1);
    if (tmpbuf == NULL) {
        return -1;
    }
	if (p_sendmtx) {
		msp_mutex_lock(p_sendmtx, MSP_WAIT_FOREVER);
	}
    strcpy((char *)tmpbuf, priv->save_name);
    memcpy(tmpbuf + strlen(priv->save_name) + 1, buffer, length);
    ret = ws_send_bin(sock, tmpbuf, slen);
	if (ret == rws_false) {
		LOGE(TAG, "ws send bin e");
	}
    msp_free(tmpbuf);
    // LOGD(TAG, "ws send:%s, %d", priv->save_name, slen);
	if (p_sendmtx) {
		msp_mutex_unlock(p_sendmtx);
	}
    return ret == rws_true ? slen : -1;
}

static int ws_seek(recio_t *io, size_t offset, int whence)
{
    return 0;
}

const recio_cls_t websocket = {
    .name = "ws",
    .open = ws_open,
    .close = ws_close,
    .write = ws_write,
    .read = ws_read,
    .seek = ws_seek,
};

int recio_register_ws(const char *path)
{
    // LOGD(TAG, "%s, %d, path:%s", __func__, __LINE__, path);

    return recio_register(&websocket, path);
}
