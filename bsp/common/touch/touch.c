/**
 * @file touch.c
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
#include "touch.h"

int touch_init(touch_coord_t *max_value)
{
    return _TOUCH_FUNC_DEFINE(init, max_value);
}

int touch_read(uint8_t *point_num, touch_coord_t *touch_coord, uint8_t max_num)
{
    return _TOUCH_FUNC_DEFINE(read, point_num, touch_coord, max_num);
}
