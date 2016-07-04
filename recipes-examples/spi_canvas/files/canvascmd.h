/**
 *  Copyright 2016 
 *  Marian Cingel - cingel.marian@gmail.com
 *
 *  Licensed to the Apache Software Foundation (ASF) under one
 *  or more contributor license agreements.  See the NOTICE file
 *  distributed with this work for additional information
 *  regarding copyright ownership.  The ASF licenses this file
 *  to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 *  specific language governing permissions and limitations
 *  under the License.
 */

#ifndef __CANVASCMD_H__
#define __CANVASCMD_H__

#include <stdint.h>
#include "spidevice.h"
#include "fbscreen.h"

#define CANVAS_CLEAR_CMD        0x01
#define CANVAS_DIMENSION_CMD    0x02
#define CANVAS_RECTANGLE_CMD    0x03
#define CANVAS_CIRCLE_CMD       0x04
#define CANVAS_NOCOMMAND_CMD    0xFF

#define CANVAS_DIMENSION_ACK    0x02

struct canvascmd {
    uint8_t cmd_code;
    int32_t (*cmd_exec)(struct fbscreen *fbscreen, struct spidevice *spidevice);
};

int32_t canvascmd_draw_circle(
    struct fbscreen *fbscreen,
    struct spidevice *spidevice
);

int32_t canvascmd_draw_rectangle(
    struct fbscreen *fbscreen,
    struct spidevice *spidevice
);

int32_t canvascmd_get_dimension(
    struct fbscreen *fbscreen,
    struct spidevice *spidevice
);

int32_t canvascmd_clear_screen(
    struct fbscreen *fbscreen,
    struct spidevice *spidevice
);

int32_t canvascmd_nocommand(
    struct fbscreen *fbscreen,
    struct spidevice *spidevice
);

#endif
