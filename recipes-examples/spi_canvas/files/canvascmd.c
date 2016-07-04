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

#include "config.h"
#include "spidevice.h"
#include "fbscreen.h"
#include "canvascmd.h"

int32_t canvascmd_draw_circle(
    struct fbscreen *fbscreen,
    struct spidevice *spidevice
)
{
    struct fbscreen_circle circle = {0};
    int32_t result;

    if (0 > (result = spidevice_read(
        spidevice, (uint8_t*)&circle, sizeof(circle)
    )))
    {
        return result;
    }

    canvas_dbg("drawing circle of size: 0x%x\n", sizeof(circle));
    canvas_dbg("xpos: 0x%x\n", circle.xpos);
    canvas_dbg("ypos: 0x%x\n", circle.ypos);
    canvas_dbg("color: 0x%x\n", circle.color);
    canvas_dbg("in centre: 0x%x\n", circle.in_centre);
    canvas_dbg("radius: 0x%x\n", circle.radius);

    if (0 > (result = fbscreen_draw_circle(
        fbscreen, &circle
    )))
    {
        return result;
    }

    return 0;
}

int32_t canvascmd_draw_rectangle(
    struct fbscreen *fbscreen,
    struct spidevice *spidevice
)
{
    struct fbscreen_rectangle rectangle;
    int32_t result;

    if (0 > (result = spidevice_read(
        spidevice, (uint8_t*)&rectangle, sizeof(rectangle)
    )))
    {
        return result;
    }

    canvas_dbg("drawing rectangle of size: 0x%x\n", sizeof(rectangle));
    canvas_dbg("xpos: 0x%x\n", rectangle.xpos);
    canvas_dbg("ypos: 0x%x\n", rectangle.ypos);
    canvas_dbg("color: 0x%x\n", rectangle.color);
    canvas_dbg("in centre: 0x%x\n", rectangle.in_centre);
    canvas_dbg("width: 0x%x\n", rectangle.width);
    canvas_dbg("height: 0x%x\n", rectangle.height);

    if (0 > (result = fbscreen_draw_rectangle(
        fbscreen, &rectangle
    )))
    {
        return result;
    }
    return 0;
}

int32_t canvascmd_get_dimension(
    struct fbscreen *fbscreen,
    struct spidevice *spidevice
)
{
    int32_t result;
    uint8_t ack = CANVAS_DIMENSION_ACK;
    struct fbscreen_dimension dimension = {
        .width = fbscreen->var_info.xres_virtual,
        .height = fbscreen->var_info.yres_virtual,
    };

    canvas_dbg("exporting dimension: 0x%x\n", sizeof(dimension));
    canvas_dbg("width: 0x%x\n", dimension.width);
    canvas_dbg("height: 0x%x\n", dimension.height);

    if (0 > (result = spidevice_write(
        spidevice, (uint8_t*)&ack, sizeof(ack)
    )))
    {
        return result;
    }

    if (0 > (result = spidevice_write(
        spidevice, (uint8_t*)&dimension, sizeof(dimension)
    )))
    {
        return result;
    }
    return 0;
}

int32_t canvascmd_clear_screen(
    struct fbscreen *fbscreen,
    struct spidevice *spidevice
)
{
    int32_t result;
    uint32_t color;

    if (0 > (result = spidevice_read(
        spidevice, (uint8_t*)&color, sizeof(color)
    )))
    {
        return result;
    }

    canvas_dbg("clearing screen: \n");
    canvas_dbg("color: 0x%x\n", color);

    if (0 > (result = fbscreen_clear_screen(
        fbscreen, color
    )))
    {
        return result;
    }

    return 0;
}

int32_t canvascmd_nocommand(
    struct fbscreen *fbscreen,
    struct spidevice *spidevice
)
{
    canvas_dbg("do nothing \n");
    return 0;
}
