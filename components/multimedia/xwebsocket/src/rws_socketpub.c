/*
 *   Copyright (c) 2014 - 2019 Oleh Kulykov <info@resident.name>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */


#include "librws.h"
#include "rws_socket.h"
#include "rws_memory.h"
#include "rws_string.h"
#include <assert.h>
//#include <signal.h>

// public
rws_bool rws_socket_connect(rws_socket socket)
{
    const char * params_error_msg = NULL;
    if (!socket) {
        return rws_false;
    }

    rws_error_delete_clean(&socket->error);

    if (socket->port <= 0) {
        params_error_msg = "No URL port provided";
    }
    if (!socket->scheme) {
        params_error_msg = "No URL scheme provided";
    }
    if (!socket->host) {
        params_error_msg = "No URL host provided";
    }
    if (!socket->path) {
        params_error_msg = "No URL path provided";
    }
    if (!socket->on_disconnected) {
        params_error_msg = "No on_disconnected callback provided";
    }
    socket->received_len = 0;
    if (params_error_msg) {
        socket->error = rws_error_new_code_descr(rws_error_code_missed_parameter, params_error_msg);
        return rws_false;
    }
    return rws_socket_create_start_work_thread(socket);
}

void rws_socket_disconnect_and_release(rws_socket socket)
{
    if (!socket) {
        return;
    }

    rws_mutex_lock(socket->work_mutex);
    if (socket->is_connected) { // connected in loop
        socket->command = COMMAND_DISCONNECT;
    } else if (socket->work_thread) { // disconnected in loop
        socket->command = COMMAND_END;
    }
    rws_mutex_unlock(socket->work_mutex);

    // rws_socket_close(socket);
    if (socket->work_thread) {
        socket->task_quit = 1;
        rws_thread_join(socket->work_thread, NULL);
        rws_free_clean((void**)&socket->work_thread);
    }

    rws_socket_delete(socket);
}

rws_bool rws_socket_send_text(rws_socket socket, const char * text)
{
    rws_bool r = rws_false;
    if (socket) {
        rws_mutex_lock(socket->send_mutex);
        r = rws_socket_send_text_priv(socket, text);
        rws_mutex_unlock(socket->send_mutex);
    }
    return r;
}

rws_bool rws_socket_send_text2(rws_socket socket, const char * text, size_t len)
{
    rws_bool r = rws_false;
    if (socket) {
        rws_mutex_lock(socket->send_mutex);
        r = rws_socket_send_text_priv2(socket, text, len);
        rws_mutex_unlock(socket->send_mutex);
    }
    return r;
}

rws_bool rws_socket_send_bin_start(rws_socket socket, const char *bin, size_t len)
{
    rws_bool r = rws_false;
    if (socket) {
        rws_mutex_lock(socket->send_mutex);
        r = rws_socket_send_bin_priv(socket, bin, len, rws_binary_start);
        rws_mutex_unlock(socket->send_mutex);
    }
    return r;
}

rws_bool rws_socket_send_bin_continue(rws_socket socket, const char *bin, size_t len)
{
    rws_bool r = rws_false;
    if (socket) {
        rws_mutex_lock(socket->send_mutex);
        r = rws_socket_send_bin_priv(socket, bin, len, rws_binary_continue);
        rws_mutex_unlock(socket->send_mutex);
    }
    return r;
}

rws_bool rws_socket_send_bin_finish(rws_socket socket, const char *bin, size_t len)
{
    rws_bool r = rws_false;
    if (socket) {
        rws_mutex_lock(socket->send_mutex);
        r = rws_socket_send_bin_priv(socket, bin, len, rws_binary_finish);
        rws_mutex_unlock(socket->send_mutex);
    }
    return r;
}

// void rws_socket_handle_sigpipe(int signal_number) {
// 	printf("\nlibrws handle sigpipe %i", signal_number);
// 	(void)signal_number;
// 	return;
// }

#define STRING_I(s) #s
#define TO_STRING(s) STRING_I(s)

void rws_socket_check_info(const char * info)
{
    assert(info);
    (void)info;
}

rws_socket rws_socket_create(void)
{
    rws_socket s = (rws_socket)rws_malloc_zero(sizeof(struct rws_socket_struct));
    if (!s) {
        return NULL;
    }

    // signal(SIGPIPE, rws_socket_handle_sigpipe);

    s->port = -1;
    s->socket = RWS_INVALID_SOCKET;
    s->command = COMMAND_NONE;

    s->work_mutex = rws_mutex_create_recursive();
    s->send_mutex = rws_mutex_create_recursive();

    s->max_send_append_size = RWS_MAX_SEND_APPEND_SIZE;
    s->send_append_size = 0;

    rws_net_init(&s->n);

    static const char * info = "librws ver: " TO_STRING(RWS_VERSION_MAJOR) "." TO_STRING(RWS_VERSION_MINOR) "." TO_STRING(RWS_VERSION_PATCH) "\n";
    rws_socket_check_info(info);

    return s;
}

void rws_socket_delete(rws_socket s)
{
    if (s->work_thread) {
        s->task_quit = 1;
        rws_thread_join(s->work_thread, NULL);
        rws_free_clean((void**)&s->work_thread);
    }

    rws_socket_close(s);

#ifdef WEBSOCKET_SSL_ENABLE
    if(s->ssl) {
        // avoid mbedtls_net_free close socket again 
        if (s->socket == RWS_INVALID_SOCKET) {
            s->ssl->net_ctx.fd = -1;
        }
        rws_ssl_close(s);
    }
#endif /* WEBSOCKET_SSL_ENABLE */
    rws_string_delete_clean(&s->sec_ws_accept);

    rws_free_clean(&s->received);
    s->received_size = 0;
    s->received_len = 0;

    s->send_append_size = 0;

    rws_socket_delete_all_frames_in_list(s->send_frames);
    rws_list_delete_clean(&s->send_frames);
    rws_socket_delete_all_frames_in_list(s->recvd_frames);
    rws_list_delete_clean(&s->recvd_frames);

    rws_string_delete_clean(&s->scheme);
    rws_string_delete_clean(&s->host);
    rws_string_delete_clean(&s->path);
    rws_error_delete_clean(&s->error);

    rws_mutex_delete(s->work_mutex);
    rws_mutex_delete(s->send_mutex);

    rws_free(s);
}

void rws_socket_set_url(rws_socket socket,
                        const char * scheme,
                        const char * host,
                        const int port,
                        const char * path)
{
    if (socket) {
        rws_string_delete(socket->scheme);
        socket->scheme = rws_string_copy(scheme);

        rws_string_delete(socket->host);
        socket->host = rws_string_copy(host);

        rws_string_delete(socket->path);
        socket->path = rws_string_copy(path);

        socket->port = port;
    }
}

void rws_socket_set_user_str(rws_socket socket, const char * user_str)
{
    if (socket) {
        rws_string_delete(socket->user_str);
        socket->user_str = rws_string_copy(user_str);
    }
}

void rws_socket_set_scheme(rws_socket socket, const char * scheme)
{
    if (socket) {
        rws_string_delete(socket->scheme);
        socket->scheme = rws_string_copy(scheme);
    }
}

const char * rws_socket_get_scheme(rws_socket socket)
{
    return socket ? socket->scheme : NULL;
}

void rws_socket_set_host(rws_socket socket, const char * host)
{
    if (socket) {
        rws_string_delete(socket->host);
        socket->host = rws_string_copy(host);
    }
}

const char * rws_socket_get_host(rws_socket socket)
{
    return socket ? socket->host : NULL;
}

void rws_socket_set_path(rws_socket socket, const char * path)
{
    if (socket) {
        rws_string_delete(socket->path);
        socket->path = rws_string_copy(path);
    }
}

const char * rws_socket_get_path(rws_socket socket)
{
    return socket ? socket->path : NULL;
}

void rws_socket_set_port(rws_socket socket, const int port)
{
    if (socket) {
        socket->port = port;
    }
}

int rws_socket_get_port(rws_socket socket)
{
    return socket ? socket->port : -1;
}

/*
unsigned int _rws_socket_get_receive_buffer_size(rws_socket_t socket) {
	unsigned int size = 0;
#if defined(RWS_OS_WINDOWS)
	int len = sizeof(unsigned int);
	if (getsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char *)&size, &len) == -1) {
		size = 0;
	}
#else
	socklen_t len = sizeof(unsigned int);
	if (getsockopt(socket, SOL_SOCKET, SO_RCVBUF, &size, &len) == -1) {
		size = 0;
	}
#endif
	return size;
}

unsigned int rws_socket_get_receive_buffer_size(rws_socket socket) {
	_rws_socket * s = (_rws_socket *)socket;
	if (!s) {
		return 0;
	}
	if (s->socket == RWS_INVALID_SOCKET) {
		return 0;
	}
	return _rws_socket_get_receive_buffer_size(s->socket);
}
*/

rws_error rws_socket_get_error(rws_socket socket)
{
    return socket ? socket->error : NULL;
}

void rws_socket_set_user_object(rws_socket socket, void * user_object)
{
    if (socket) {
        socket->user_object = user_object;
    }
}

void * rws_socket_get_user_object(rws_socket socket)
{
    return socket ? socket->user_object : NULL;
}

int rws_socket_set_read_timeout(rws_socket socket, int timeout_ms)
{
    socket->read_timeout = timeout_ms;
    return 0;
}

int rws_socket_set_write_timeout(rws_socket socket, int timeout_ms)
{
    socket->write_timeout = timeout_ms;
    return 0;
}

void rws_socket_set_on_connected(rws_socket socket, rws_on_socket callback)
{
    if (socket) {
        socket->on_connected = callback;
    }
}

void rws_socket_set_on_disconnected(rws_socket socket, rws_on_socket callback)
{
    if (socket) {
        socket->on_disconnected = callback;
    }
}

void rws_socket_set_on_received_text(rws_socket socket, rws_on_socket_recvd_text callback)
{
    if (socket) {
        socket->on_recvd_text = callback;
    }
}

void rws_socket_set_on_received_bin(rws_socket socket, rws_on_socket_recvd_bin callback)
{
    if (socket) {
        socket->on_recvd_bin = callback;
    }
}

void rws_socket_set_on_received_pong(rws_socket socket, rws_on_socket_recvd_pong callback)
{
    if (socket) {
        socket->on_recvd_pong = callback;
    }
}

rws_bool rws_socket_is_connected(rws_socket socket)
{
    rws_bool r = rws_false;
    if (socket) {
        rws_mutex_lock(socket->send_mutex);
        r = socket->is_connected;
        rws_mutex_unlock(socket->send_mutex);
    }
    return r;
}

#ifdef WEBSOCKET_SSL_ENABLE
void rws_socket_set_server_cert(rws_socket socket, const char *server_cert, int server_cert_len)
{
    // DBG("%s: server_cert[len:%d]: \n%s", __FUNCTION__, server_cert_len, server_cert);

    rws_socket s = socket;
    if (s) {
        s->server_cert = server_cert;
        s->server_cert_len = server_cert_len;
    }
}
#endif
