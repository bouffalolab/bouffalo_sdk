/**
 * @file sdh_sdcard.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
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

#ifndef FATFS_DISKIO_REGISTER_H
#define FATFS_DISKIO_REGISTER_H

#if defined(CONFIG_FATFS_SDH_SDCARD)

void fatfs_sdh_driver_register(void);

#elif defined(CONFIG_FATFS_USBH)

void fatfs_usbh_driver_register(struct usbh_msc *msc_class);

#endif

#endif