/****************************************************************************
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "mem.h"

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include "wifi_mgmr_ext.h"

#include "bflb_irq.h"
#include "bflb_uart.h"

#include "rfparam_adapter.h"

#include "board.h"
#include "shell.h"

#define DBG_TAG "MAIN"
#include "log.h"

struct bflb_device_s *gpio;

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define WIFI_STACK_SIZE  (1536)
#define TASK_PRIORITY_FW (16)

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct bflb_device_s *uart0;

static wifi_conf_t conf = {
    .country_code = "CN",
};

extern void shell_init_with_task(struct bflb_device_s *shell);

/* Main wifi stack entry point */
extern void wifi_main(void *param);

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Functions
 ****************************************************************************/

void wifi_start_firmware_task(void *param)
{
    LOG_I("Starting wifi ...\r\n");

    wifi_task_create();

    LOG_I("Starting fhost ...\r\n");
    fhost_init();

    vTaskDelete(NULL);
}

void wifi_event_handler(uint32_t code)
{
    switch (code) {
        case CODE_WIFI_ON_INIT_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_INIT_DONE\r\n", __func__);
            wifi_mgmr_init(&conf);
        } break;
        case CODE_WIFI_ON_MGMR_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_MGMR_DONE\r\n", __func__);
        } break;
        case CODE_WIFI_ON_SCAN_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_SCAN_DONE\r\n", __func__);
            wifi_mgmr_sta_scanlist();
        } break;
        case CODE_WIFI_ON_CONNECTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_CONNECTED\r\n", __func__);
            void mm_sec_keydump();
            mm_sec_keydump();
        } break;
        case CODE_WIFI_ON_GOT_IP: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_GOT_IP\r\n", __func__);
            LOG_I("[SYS] Memory left is %d Bytes\r\n", kfree_size());
        } break;
        case CODE_WIFI_ON_DISCONNECT: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_DISCONNECT\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STARTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STARTED\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STOPPED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STOPPED\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STA_ADD: {
            LOG_I("[APP] [EVT] [AP] [ADD] %lld\r\n", xTaskGetTickCount());
        } break;
        case CODE_WIFI_ON_AP_STA_DEL: {
            LOG_I("[APP] [EVT] [AP] [DEL] %lld\r\n", xTaskGetTickCount());
        } break;
        default: {
            LOG_I("[APP] [EVT] Unknown code %u \r\n", code);
        }
    }
}

int main(void)
{
    board_init();

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    if (0 != rfparam_init(0, NULL, 0)) {
        LOG_I("PHY RF init failed!\r\n");
        return 0;
    }

    LOG_I("PHY RF init success!\r\n");

    tcpip_init(NULL, NULL);
    xTaskCreate(wifi_start_firmware_task, "wifi init", 1024, NULL, 10, NULL);

    vTaskStartScheduler();

    while (1) {
    }
}

int cmd_gethostbyname(int argc, char **argv)
{
  struct addrinfo hints, *res, *p;
  int status;
  char ipstr[INET6_ADDRSTRLEN];
  char *name = argv[1];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  if(!strcmp(argv[1], "-4")) {
    hints.ai_family = AF_INET;
    name = argv[2];
  }

  if(!strcmp(argv[1], "-6")) {
    hints.ai_family = AF_INET6;
    name = argv[2];
  }

  hints.ai_socktype = SOCK_STREAM;

  if ((status = getaddrinfo(name, NULL, &hints, &res)) != 0) {
      fprintf(stderr, "getaddrinfo error: %d\r\n", status);
      return 0;
  }

  printf("IP addresses for %s:\r\n", name);

  for (p = res; p != NULL; p = p->ai_next) {
      void *addr;
      char *ipver;

      if (p->ai_family == AF_INET) {
          struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
          addr = &(ipv4->sin_addr);
          ipver = "IPv4";
      } else {
          struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
          addr = &(ipv6->sin6_addr);
          ipver = "IPv6";
      }

      inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
      printf("  %s: %s\r\n", ipver, ipstr);
  }

  freeaddrinfo(res);
  return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_gethostbyname, gethostbyname, gethostbyname command);
