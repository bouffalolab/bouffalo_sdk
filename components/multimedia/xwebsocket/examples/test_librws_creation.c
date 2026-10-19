/*
 *   Copyright (c) 2014 - 2016 Kulykov Oleh <info@resident.name>
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

#include <app_config.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <librws.h>


#define WEBSOCKET_CONNECTED (0x01)
#define WEBSOCKET_DISCONNECTED (0x02)
#define WEBSOCKET_DATA_NOT_RECVED (0x04)

#ifdef WEBSOCKET_SSL_ENABLE

const char *ECHO_WEBSOCKET_CER = {
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIEZTCCA02gAwIBAgIQQAF1BIMUpMghjISpDBbN3zANBgkqhkiG9w0BAQsFADA/\r\n"
    "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\r\n"
    "DkRTVCBSb290IENBIFgzMB4XDTIwMTAwNzE5MjE0MFoXDTIxMDkyOTE5MjE0MFow\r\n"
    "MjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxCzAJBgNVBAMT\r\n"
    "AlIzMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuwIVKMz2oJTTDxLs\r\n"
    "jVWSw/iC8ZmmekKIp10mqrUrucVMsa+Oa/l1yKPXD0eUFFU1V4yeqKI5GfWCPEKp\r\n"
    "Tm71O8Mu243AsFzzWTjn7c9p8FoLG77AlCQlh/o3cbMT5xys4Zvv2+Q7RVJFlqnB\r\n"
    "U840yFLuta7tj95gcOKlVKu2bQ6XpUA0ayvTvGbrZjR8+muLj1cpmfgwF126cm/7\r\n"
    "gcWt0oZYPRfH5wm78Sv3htzB2nFd1EbjzK0lwYi8YGd1ZrPxGPeiXOZT/zqItkel\r\n"
    "/xMY6pgJdz+dU/nPAeX1pnAXFK9jpP+Zs5Od3FOnBv5IhR2haa4ldbsTzFID9e1R\r\n"
    "oYvbFQIDAQABo4IBaDCCAWQwEgYDVR0TAQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8E\r\n"
    "BAMCAYYwSwYIKwYBBQUHAQEEPzA9MDsGCCsGAQUFBzAChi9odHRwOi8vYXBwcy5p\r\n"
    "ZGVudHJ1c3QuY29tL3Jvb3RzL2RzdHJvb3RjYXgzLnA3YzAfBgNVHSMEGDAWgBTE\r\n"
    "p7Gkeyxx+tvhS5B1/8QVYIWJEDBUBgNVHSAETTBLMAgGBmeBDAECATA/BgsrBgEE\r\n"
    "AYLfEwEBATAwMC4GCCsGAQUFBwIBFiJodHRwOi8vY3BzLnJvb3QteDEubGV0c2Vu\r\n"
    "Y3J5cHQub3JnMDwGA1UdHwQ1MDMwMaAvoC2GK2h0dHA6Ly9jcmwuaWRlbnRydXN0\r\n"
    "LmNvbS9EU1RST09UQ0FYM0NSTC5jcmwwHQYDVR0OBBYEFBQusxe3WFbLrlAJQOYf\r\n"
    "r52LFMLGMB0GA1UdJQQWMBQGCCsGAQUFBwMBBggrBgEFBQcDAjANBgkqhkiG9w0B\r\n"
    "AQsFAAOCAQEA2UzgyfWEiDcx27sT4rP8i2tiEmxYt0l+PAK3qB8oYevO4C5z70kH\r\n"
    "ejWEHx2taPDY/laBL21/WKZuNTYQHHPD5b1tXgHXbnL7KqC401dk5VvCadTQsvd8\r\n"
    "S8MXjohyc9z9/G2948kLjmE6Flh9dDYrVYA9x2O+hEPGOaEOa1eePynBgPayvUfL\r\n"
    "qjBstzLhWVQLGAkXXmNs+5ZnPBxzDJOLxhF2JIbeQAcH5H0tZrUlo5ZYyOqA7s9p\r\n"
    "O5b85o3AM/OJ+CktFBQtfvBhcJVd9wvlwPsk+uyOy2HI7mNxKKgsBTt375teA2Tw\r\n"
    "UdHkhVNcsAKX1H7GNNLOEADksd86wuoXvg==\r\n"
    "-----END CERTIFICATE-----\r\n"
};

#endif

static rws_socket _socket = NULL;
static int state_flags = 0;

static void on_socket_received_text(rws_socket socket, const char *text, const unsigned int length, bool is_finish)
{
    char *buff = NULL;

    if (!socket || !text || !length) {
        DBG("%s: Invalid parameter(s).", __FUNCTION__);
        return;
    }

    buff = (char *)aos_malloc(length + 1);
    if (!buff) {
        DBG("%s: Not enough memory. len:%d", __FUNCTION__, length + 1);
        return;
    }

    state_flags &= (~WEBSOCKET_DATA_NOT_RECVED);

    memcpy(buff, text, length);
    buff[length] = 0;

    DBG("%s: Socket text: %s", __FUNCTION__, buff);
    aos_free(buff);
    buff = NULL;
}

static void on_socket_received_bin(rws_socket socket, const void * data, const unsigned int length, bool is_finish)
{
    char *buff = NULL;

    if (!socket || !data || !length) {
        DBG("%s: Invalid parameter(s).", __FUNCTION__);
        return;
    }

    buff = (char *)aos_malloc(length + 1);
    if (!buff) {
        DBG("%s: Not enough memory. len:%d", __FUNCTION__, length + 1);
        return;
    }

    state_flags &= ~WEBSOCKET_DATA_NOT_RECVED;

    memcpy(buff, data, length);
    buff[length] = 0;

    DBG("%s: Socket bin: \n%s", __FUNCTION__, buff);
    aos_free(buff);
    buff = NULL;
}

static void on_socket_received_pong(rws_socket socket)
{
    if (!socket) {
        DBG("%s: Invalid parameter(s).", __FUNCTION__);
        return;
    }
    DBG("received pong!!!!!!!!!!!");
}

static void on_socket_connected(rws_socket socket)
{
    const char * test_send_text =
        "{\"version\":\"1.0\",\"supportedConnectionTypes\":[\"websocket\"],\"minimumVersion\":\"1.0\",\"channel\":\"/meta/handshake\"}";

    DBG("%s: Socket connected", __FUNCTION__);

    state_flags |= WEBSOCKET_CONNECTED;
    state_flags &= ~WEBSOCKET_DISCONNECTED;
    rws_socket_send_text(socket, test_send_text);
}

static void on_socket_disconnected(rws_socket socket)
{
    rws_error error = rws_socket_get_error(socket);
    if (error) {
        DBG("%s: Socket disconnect with code, error: %i, %s",
            __FUNCTION__,
            rws_error_get_code(error),
            rws_error_get_description(error));
    }

    state_flags &= ~WEBSOCKET_CONNECTED;
    state_flags |= WEBSOCKET_DISCONNECTED;
    _socket = NULL;
}


int websoc_cli_test_int(const char *scheme, const char *host,
                        const char *path, const int port,
                        const char *cert)
{
    int sleep_count = 0;

    if (!scheme || !host || !path) {
        DBG("%s: Invalid parameter(s).", __FUNCTION__);
        return -1;
    }

    if (_socket) {
        DBG("%s: Socket is not closed.", __FUNCTION__);
        return -2;
    }

    _socket = rws_socket_create(); // create and store socket handle
    CHECK_RET_WITH_RET(_socket, -1);

    state_flags = 0;
    state_flags |= WEBSOCKET_DATA_NOT_RECVED;

    rws_socket_set_scheme(_socket, scheme);
    rws_socket_set_host(_socket, host);
    rws_socket_set_path(_socket, path);
    rws_socket_set_port(_socket, port);

#ifdef WEBSOCKET_SSL_ENABLE
    if (cert) {
        rws_socket_set_server_cert(_socket, cert, strlen(cert) + 1);
    }
#endif

    rws_socket_set_on_disconnected(_socket, &on_socket_disconnected);
    rws_socket_set_on_connected(_socket, &on_socket_connected);
    rws_socket_set_on_received_text(_socket, &on_socket_received_text);
    rws_socket_set_on_received_bin(_socket, &on_socket_received_bin);
    rws_socket_set_on_received_pong(_socket, &on_socket_received_pong);

    rws_socket_connect(_socket);

    /* Connecting */
    while ((!(state_flags & WEBSOCKET_CONNECTED)) &&
           (!(state_flags & WEBSOCKET_DISCONNECTED))) {
        rws_thread_sleep(1000);
        sleep_count++;
        if (30 == sleep_count) {
            break;
        }
    }

    /* Receiving data */
    sleep_count = 0;
    if (state_flags & WEBSOCKET_CONNECTED) {
        while ((state_flags & WEBSOCKET_DATA_NOT_RECVED) && _socket &&
               (rws_true == rws_socket_is_connected(_socket))) {
            rws_thread_sleep(1000);
            sleep_count++;
            if (20 == sleep_count) {
                break;
            }
        }
    }
#if 0
    const char *text_arr[] = {
        "-----BEGIN CERTIFICATE-----"
        "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"
        "-----END CERTIFICATE-----",
        "AABBsdfasdfasdfasdfasdfasdfasdfadfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfadfasdfasdfasdfasdfasdf",
        "09840523490582034850-2385023845023845023840523orjoewjrt0932u4ojt[iq3w04tu32094503u4t32u4059",
        ";';,,'s'd,fdg;sm;lsdf;g,s;d  ;s,g ;,df;gl s;dg, ;sd,;gf,sd; g,",
        "46s4f64s6df4a6sd4f64sdgf654segf654df66dsfg4e4rt65w4t6w4et64ewr6g4sd64fg65ds4fg",
        "sdfasdg4sag64a6g45sd4 64365 46 4d6f4asd64 f6as4 f6as74f987s6543654165JJKK",
        NULL
    };

    // rws_socket_send_text(_socket, "hello test send text!!??##OOOKKKK.");
    rws_socket_send_bin_start(_socket, "start", strlen("start"));
    int i = 0;
    while(text_arr[i] != NULL) {
        rws_socket_send_bin_continue(_socket, text_arr[i], strlen(text_arr[i]));
        // aos_msleep(100);
        i ++;
    }
    rws_socket_send_bin_finish(_socket, "finish", strlen("finish"));
    return 0;
#else
    if (_socket) {
        rws_socket_disconnect_and_release(_socket);
    }

    _socket = NULL;

    return state_flags & WEBSOCKET_DATA_NOT_RECVED ? -3 : 0;
#endif
}

extern int websoc_set_get_test(void);
void websoc_cli_test_entry(void)
{
    int ret = 0, ssl_ret = 0;

#if 1
    char *cert = (char *)ECHO_WEBSOCKET_CER;
#ifdef WEBSOCKET_SSL_ENABLE
    DBG("Test client wss.");
    ssl_ret = websoc_cli_test_int("wss", "echo.websocket.org", "/", 443, cert);
#endif
#else
    DBG("Test client ws.");
    ret = websoc_cli_test_int("ws", "echo.websocket.org", "/", 80, NULL);
#endif
    DBG("Test set get.");
    websoc_set_get_test();

    if (0 == ret && 0 == ssl_ret) {
        DBG("example project test success.");
    } else {
        DBG("example project test failed. ret:%d, ssl_ret:%d", ret, ssl_ret);
    }
}

static void func1(void *argv)
{
    websoc_cli_test_entry();
}

void ws_test_task(void)
{
    aos_task_t task;

    LOGD("ws", "ws_test_task");
    aos_task_new("ws-test", func1, NULL, 10 * 1024);
}
