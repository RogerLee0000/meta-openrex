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
#include "canvas_common.h"
#include "spidevice.h"
#include "fbscreen.h"
#include "canvascmd.h"


void main_loop(
    struct fbscreen *fbscreen,
    struct spidevice *spidevice,
    struct canvascmd *commands
)
{
    uint8_t cmd_code;
    int32_t result;

    canvas_dbg("main loop started \n");
    for (volatile int32_t loop = 1; loop;)
    {
        result = spidevice_read(spidevice, &cmd_code, sizeof(cmd_code));
        assert(result >= 0);
        canvas_dbg("command code: 0x%x \n", cmd_code);
        for (int i = 0; commands[i].cmd_code; i++ )
        {
            canvas_dbg("current code: 0x%x \n", commands[i].cmd_code);
            if (commands[i].cmd_code == cmd_code)
            {
                canvas_dbg("executing\n");
                result = commands[i].cmd_exec(fbscreen, spidevice);
                assert(result >= 0);
                break;
            }
        }
    }
}

int32_t print_screenfb_info(
    struct fbscreen *fbscreen
)
{
    assert(!(NULL == fbscreen));
    if (NULL == fbscreen)
        return -1;

    canvas_dbg("var.xres %d\n", fbscreen->var_info.xres);
    canvas_dbg("var.yres %d\n", fbscreen->var_info.yres);
    canvas_dbg("var.xres_virtual %d\n", fbscreen->var_info.xres_virtual);
    canvas_dbg("var.yres_virtual %d\n", fbscreen->var_info.yres_virtual);
    canvas_dbg("var.xoffset %d\n", fbscreen->var_info.xoffset);
    canvas_dbg("var.yoffset %d\n", fbscreen->var_info.yoffset);
    canvas_dbg("var.bits_per_pixel %d\n", fbscreen->var_info.bits_per_pixel);
    canvas_dbg("var.grayscale %d\n", fbscreen->var_info.grayscale);
    canvas_dbg("var.red %d %d %d\n", fbscreen->var_info.red.offset, fbscreen->var_info.red.length, fbscreen->var_info.red.msb_right);
    canvas_dbg("var.green %d %d %d\n", fbscreen->var_info.green.offset, fbscreen->var_info.green.length, fbscreen->var_info.green.msb_right);
    canvas_dbg("var.blue %d %d %d\n", fbscreen->var_info.blue.offset, fbscreen->var_info.blue.length, fbscreen->var_info.blue.msb_right);
    canvas_dbg("var.transp %d %d %d\n", fbscreen->var_info.transp.offset, fbscreen->var_info.transp.length, fbscreen->var_info.transp.msb_right);
    canvas_dbg("var.nonstd %d\n", fbscreen->var_info.nonstd);
    canvas_dbg("var.activate %d\n", fbscreen->var_info.activate);
    canvas_dbg("var.height %d\n", fbscreen->var_info.height);
    canvas_dbg("var.width %d\n", fbscreen->var_info.width);
    canvas_dbg("var.accel_flags %d\n", fbscreen->var_info.accel_flags);
    canvas_dbg("var.pixclock %d\n", fbscreen->var_info.pixclock);
    canvas_dbg("var.left_margin %d\n", fbscreen->var_info.left_margin);
    canvas_dbg("var.right_margin %d\n", fbscreen->var_info.right_margin);
    canvas_dbg("var.upper_margin %d\n", fbscreen->var_info.upper_margin);
    canvas_dbg("var.lower_margin %d\n", fbscreen->var_info.lower_margin);
    canvas_dbg("var.hsync_len %d\n", fbscreen->var_info.hsync_len);
    canvas_dbg("var.vsync_len %d\n", fbscreen->var_info.vsync_len);
    canvas_dbg("var.sync %d\n", fbscreen->var_info.sync);
    canvas_dbg("var.vmode %d\n", fbscreen->var_info.vmode);
    canvas_dbg("var.rotate %d\n", fbscreen->var_info.rotate);
    canvas_dbg("var.colorspace %d\n", fbscreen->var_info.colorspace);

    canvas_dbg("fix.smem_start %llu\n", fbscreen->fix_info.smem_start);
    canvas_dbg("fix.smem_len %d\n", fbscreen->fix_info.smem_len);
    canvas_dbg("fix.type %d\n", fbscreen->fix_info.type);
    canvas_dbg("fix.type_aux %d\n", fbscreen->fix_info.type_aux);
    canvas_dbg("fix.visual %d\n", fbscreen->fix_info.visual);
    canvas_dbg("fix.xpanstep %d\n", fbscreen->fix_info.xpanstep);
    canvas_dbg("fix.ypanstep %d\n", fbscreen->fix_info.ypanstep);
    canvas_dbg("fix.ywrapstep %d\n", fbscreen->fix_info.ywrapstep);
    canvas_dbg("fix.line_length %d\n", fbscreen->fix_info.line_length);
    canvas_dbg("fix.mmio_start %llu\n", fbscreen->fix_info.mmio_start);
    canvas_dbg("fix.mmio_len %d\n", fbscreen->fix_info.mmio_len);
    canvas_dbg("fix.accel %d\n", fbscreen->fix_info.accel);
    canvas_dbg("fix.capabilities %d\n", fbscreen->fix_info.capabilities);

    return 0;
}

struct canvascmd commands[] = {
    { CANVAS_CMD_CLEAR, canvascmd_clear_screen },
    { CANVAS_CMD_GETDIMENSION, canvascmd_get_dimension },
    { CANVAS_CMD_RECTANGLE, canvascmd_draw_rectangle },
    { CANVAS_CMD_CIRCLE, canvascmd_draw_circle },
    { CANVAS_CMD_DUMMY, canvascmd_do_nothing },
// other commands ...
// and NULL terminated list of commands
    {0},
};

void test_screen(
    struct fbscreen *fbscreen
)
{
    fbscreen_clear_screen(fbscreen, CANVAS_COLOR_GREEN);
    // rectangle
    struct fbscreen_rectangle rectangle = {
        .xpos = fbscreen->var_info.xres_virtual/2,
        .ypos = fbscreen->var_info.yres_virtual/2,
        .color = CANVAS_COLOR_BLUE,
        .width = 120,
        .height = 120,
        .in_centre = 1,
    };
    fbscreen_draw_rectangle(fbscreen, &rectangle);
    rectangle.in_centre = 0,
    rectangle.xpos = 0;
    rectangle.ypos = 0;
    fbscreen_draw_rectangle(fbscreen, &rectangle);
    // circle
    struct fbscreen_circle circle = {
        .xpos = fbscreen->var_info.xres_virtual/2,
        .ypos = fbscreen->var_info.yres_virtual/2,
        .color = CANVAS_COLOR_RED,
        .radius = 60,
        .in_centre = 1,
    };
    fbscreen_draw_circle(fbscreen, &circle);
    circle.in_centre = 0;
    circle.xpos = 0;
    circle.ypos = 0;
    fbscreen_draw_circle(fbscreen, &circle);
}

struct fbscreen fbscreen = {0};
struct spidevice spidevice = {0};

int main(void)
{
    int32_t result;
    int32_t ds;

    result = fbscreen_init(&fbscreen, FB_DEVICE, DISABLE_TTY_PATH);
    if (0 > result)
    {
        printf("cannot initialize framebuffer, error %d\n", result);
        goto error1;
    }
    result = spidevice_init(&spidevice, SPIDEV_PATH, SPIDEV_SPEED);
    if (0 > result)
    {
        printf("cannot initialize framebuffer, error %d\n", result);
        goto error2;
    }

    print_screenfb_info(&fbscreen);
    //test_screen(&fbscreen);
    main_loop(&fbscreen, &spidevice, commands);

error2:
    spidevice_deinit(&spidevice);
error1:
    fbscreen_deinit(&fbscreen);
    return result; 
}

