#ifndef SOCKET_RPC_SERVICE_H
#define SOCKET_RPC_SERVICE_H

#include <stdbool.h>
#include <FreeRTOS.h>
#include <lwip/sockets.h>
#include <openamp/rpmsg_rpc_client_server.h>

#ifdef __cplusplus
extern "C" {
#endif

int rpc_service_init(void);

/* Call only after RPC callbacks are quiescent; the shared device stays alive. */
void rpc_service_shutdown(void);
bool rpc_service_is_initialized(void);

#ifdef __cplusplus
}
#endif

#endif /* SOCKET_RPC_SERVICE_H */
