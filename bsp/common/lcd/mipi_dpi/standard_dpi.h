/**
 * @file standard_dpi.h
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

#ifndef _STANDARD_DBI_H_
#define _STANDARD_DBI_H_

#include "../lcd_conf.h"

#if defined LCD_DPI_STANDARD

/* Do not modify the following */

#if (STANDARD_DPI_PIXEL_FORMAT == 1)
#define STANDARD_DPI_COLOR_DEPTH 16
typedef uint16_t standard_dpi_color_t;
#elif (STANDARD_DPI_PIXEL_FORMAT == 2)
#define STANDARD_DPI_COLOR_DEPTH 32
typedef uint32_t standard_dpi_color_t;
#endif

int standard_dpi_init(standard_dpi_color_t *screen_buffer);
int standard_dpi_screen_switch(standard_dpi_color_t *screen_buffer);
standard_dpi_color_t *standard_dpi_get_screen_using(void);
int standard_dpi_frame_callback_register(uint32_t callback_type, void (*callback)(void));

#endif
#endif
