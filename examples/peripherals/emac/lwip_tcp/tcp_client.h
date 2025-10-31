/**
 * @file tcp_client.h
 * @brief
 *
 * Copyright (c) 2022 Bouffalolab team
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
 */
#ifndef __TCP_CLIENT__H__
#define __TCP_CLIENT__H__

#define TCP_CLIENT_TEST_PORT (3365)

#define SERVER_IP_ADDR0 (192)
#define SERVER_IP_ADDR1 (168)
#define SERVER_IP_ADDR2 (1)
#define SERVER_IP_ADDR3 (220)

void tcp_client_init(void);
void tcp_client_raw_init(void);

#endif
