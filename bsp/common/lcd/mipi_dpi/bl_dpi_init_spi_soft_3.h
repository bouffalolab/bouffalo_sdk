/**
 * @file bl_dpi_init_spi_soft_3.h
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

#ifndef _BL_DPI_INIT_SPI_SOFT_3_H
#define _BL_DPI_INIT_SPI_SOFT_3_H

#include "../lcd_conf.h"

#if (defined(LCD_DPI_INIT_INTERFACE_TYPE) && (LCD_DPI_INIT_INTERFACE_TYPE == 1))

int lcd_dpi_init_spi_soft_3_init(void);

int lcd_dpi_init_spi_soft_3_transmit_cmd_para(uint8_t cmd, uint8_t *para, size_t para_num);

#endif
#endif
