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

#include "kv_test.h"
#include "shell.h"
#include "arch_os.h"

#include "log.h"

#define KV_OK    0
/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * kv_test_main
 ****************************************************************************/

void cmd_kv_test(int argc, char **argv)
{
	LOG_I("%s begin\n", __FUNCTION__);
	int ret = 0;
	int num = 0;
	ret = kv_test();
	if (ret == KV_OK) {
		LOG_I("KV test initiated successfully\n");
		LOG_I("Test will run for approximately %d seconds\n",
		      (CONFIG_KV_TEST_FILE_OPER_SUM * CONFIG_KV_TEST_FILE_OPER_PERIOD) / 1000);
	} else {
		LOG_E("KV test initialization failed\n");
	}
}
