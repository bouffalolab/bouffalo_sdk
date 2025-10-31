/*
 * WPA Supplicant / Example program entrypoint
 * Copyright (c) 2003-2005, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"

#include "common.h"
#include "wpa_supplicant_i.h"
#include "config.h"
#include "fhost_wpa.h"


//In order to do coexistence test, Made a temporary patch
#ifdef CFG_FOR_COEXISTENCE_TEST_STOPAP_PATCH
struct wpa_global *g_global = NULL;
#endif
void wpa_supplicant_main(void *env)
{
	int exitcode = 0;
	struct wpa_params params;
	struct wpa_global *global;

	memset(&params, 0, sizeof(params));
#ifdef CONFIG_WPA_LOG_LEVEL
	params.wpa_debug_level = CONFIG_WPA_LOG_LEVEL;
#else
	params.wpa_debug_level = MSG_INFO;
#endif

	global = wpa_supplicant_init(&params);
	if (global == NULL)
		goto end;
//In order to do coexistence test, Made a temporary patch
#ifdef CFG_FOR_COEXISTENCE_TEST_STOPAP_PATCH
	g_global = global;
	printf("=========================g_global = 0x%08lx\r\n", (uint32_t)g_global);
#endif
	fhost_wpa_send_event(FHOST_WPA_STARTED, global->params.ctrl_interface, 0, FHOST_WPA_GLOBAL_VIF);
	global->params.ctrl_interface = NULL;

	exitcode = wpa_supplicant_run(global);

	wpa_supplicant_deinit(global);

  end:
	fhost_wpa_send_event(FHOST_WPA_EXIT, (void *)exitcode, 0, FHOST_WPA_GLOBAL_VIF);
	rtos_task_delete(NULL);
}




