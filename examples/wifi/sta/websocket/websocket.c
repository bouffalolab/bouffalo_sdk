#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <lwip/api.h>
#include <lwip/arch.h>
#include <lwip/opt.h>
#include <lwip/inet.h>
#include <lwip/errno.h>
#include <netdb.h>

#include "shell.h"
#include "utils_getopt.h"
#include "bflb_mtimer.h"
#include "librws.h"
#include "rws_error.h"

#include <mbedtls/error.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/debug.h>
#include <mbedtls/x509.h>


static const char *test_ca_crt = {
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIDgjCCAmqgAwIBAgIUDXuOiFDUpfyctDliikuaRtsOA/UwDQYJKoZIhvcNAQEL\r\n"
    "BQAwWTELMAkGA1UEBhMCQ04xEDAOBgNVBAgMB0JlaWppbmcxEDAOBgNVBAcMB0Jl\r\n"
    "aWppbmcxEzARBgNVBAoMCk15IENvbXBhbnkxETAPBgNVBAMMCE15IFdTIENBMB4X\r\n"
    "DTI1MTEwNzA2NTkzNloXDTM1MTEwNTA2NTkzNlowWTELMAkGA1UEBhMCQ04xEDAO\r\n"
    "BgNVBAgMB0JlaWppbmcxEDAOBgNVBAcMB0JlaWppbmcxEzARBgNVBAoMCk15IENv\r\n"
    "bXBhbnkxETAPBgNVBAMMCE15IFdTIENBMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8A\r\n"
    "MIIBCgKCAQEArKRxwazPB1SZAmbibmqqKpj7uFVq/eV7swY2V1Fvy0pEgvk/EYsC\r\n"
    "2Vw0TM29oKTvX3TinRg6Kodf9R+kTw9EB8fYeb782ImU6ZSiaVdPg7Dz2QCpiGY7\r\n"
    "b4g92H6MUongLR7DWvFg3YIS9AXMwnDsKCD9WqoI6QmEPWGCnpknKGOCkSPmgInR\r\n"
    "ruD7c+bxIN8/lefWkjwQOzb4XeqU+DOs74D+75iztaPvtHAQSI6amthfFQtODFDB\r\n"
    "qP26pIQ2r6PSZqv7NwUIjXT2e4jqWmjY/OXJLj/TnrBlyjx2M6Vuh1T5hC3FiFUX\r\n"
    "dpJMMdRt7mnDP09Dw1th+Kzm4tjVqrch1wIDAQABo0IwQDAPBgNVHRMBAf8EBTAD\r\n"
    "AQH/MA4GA1UdDwEB/wQEAwIBBjAdBgNVHQ4EFgQU37YNYVlsbUYi/mQu+OZQ5FJ/\r\n"
    "0BowDQYJKoZIhvcNAQELBQADggEBAKYBU7IxJCauvlJLMoXW7lQIbItQhDgOp+3a\r\n"
    "I+xdoTm2kwDtgwgQc2h1Fl7yVqy/TAF4RrZ/m5yuiz/8oAZqieEydWlaX9Akyim8\r\n"
    "701G+GP88uAWa4wHMs7laaTWlGC2OHIdL5bg1hx69juA2NigxD3rMd1cMUnl2orc\r\n"
    "2qUk/rWKtN0qO5yw9hRd0XYli1PLdQ53w5YYSlme/ABsJKQ/v2FuwrYUfM0haM1i\r\n"
    "4eZxNxR69ohgloz4T6C/srCt1QXzdRrCNxk/wSBJM+EPrXCdiYbVgbuMP4oHKwLZ\r\n"
    "OaWRtEoSKcObt4JRRPAo/LyS+a+/Q/Ixh13R7SnpoMwpLJaYNt0=\r\n"
    "-----END CERTIFICATE-----\r\n"
};

struct arg_param {
    int argc;
    char **argv;
};


void rws_on_socket_callback(struct rws_socket_struct * wsocket){
    rws_error err = rws_socket_get_error(wsocket);
    printf("websocket disconnected\r\n");
    if(err){
        printf("error code: %d, description: %s\r\n", err->code, err->description);
    }

}


void websocket_init(char * host, int port, char *path){
    rws_socket ws_client = rws_socket_create();

    if(!ws_client){
        printf("create websocket socket failed\r\n");
        return;
    }

    rws_socket_set_host(ws_client, host);
    rws_socket_set_port(ws_client, port);
    rws_socket_set_path(ws_client, path);
 #ifdef WEBSOCKET_SSL_ENABLE
    const char * scheme = "wss";
    rws_socket_set_server_cert(ws_client, test_ca_crt, strlen(test_ca_crt)+1);
 #else
    const char * scheme = "ws";
 #endif
    rws_socket_set_on_disconnected(ws_client, rws_on_socket_callback);
    rws_socket_set_scheme(ws_client, scheme);

    if(!rws_socket_connect(ws_client)){
        printf("websocet connect thread create failed\r\n");
        rws_error err = rws_socket_get_error(ws_client);
        if(err)
            printf("error code: %d, description: %s\r\n", err->code, err->description);
        rws_socket_disconnect_and_release(ws_client);
        return;
    }
    printf("websocket connecting to ws://%s:%d%s ...\r\n", host, port, path);
    while (!rws_socket_is_connected(ws_client));
    printf("websocket connect successfully\r\n");
}


void cmd_websocket(int argc, char **argv){

    if (argc < 3) {
        printf("Usage: websocket_test <host> <port> <path>\r\n");
        return;
    }
    char *host = argv[1];
    char *port = argv[2];
    char *path = "/";
    if(argc >=4){
        path = argv[3];
    }
    // struct arg_param arg = {argc, argv};
    if (strlen(host) > 255) {
        printf("Error: hostname too long (max 255 chars)\r\n");
        return;
    }
    if (atoi(port) <= 0 || atoi(port) > 65535) {
        printf("Error: invalid port number (1-65535)\r\n");
        return;
    }


    printf("Set host: %s port: %d path: %s\r\n", host, atoi(port), path);
    websocket_init(host, atoi(port), path);


}

SHELL_CMD_EXPORT_ALIAS(cmd_websocket, websocket_test, websocket test);