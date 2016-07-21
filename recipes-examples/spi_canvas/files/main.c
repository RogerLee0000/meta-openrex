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

#include <linux/kd.h>

#include "config.h"
#include "canvas_common.h"
#include "spidevice.h"
#include "fbscreen.h"
#include "canvascmd.h"

int32_t disable_tty(
    const char *tty_path
)
{
    /* switch tty to graphic mode
     * - get rid of blinking cursor
     * - disable sleep mode */
    int32_t tty_fd;
    int32_t result;

    tty_fd = open(tty_path, O_RDWR);
    assert(!(tty_fd < 0));
    if (tty_fd < 0)
        return tty_fd;
    result = ioctl(tty_fd, KDSETMODE, KD_GRAPHICS);
    assert(!(result < 0));
    close(tty_fd);
    return result < 0 ? -1 : 0;
}

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

int32_t print_fbunit_info(
    struct fbscreen_fbunit *fbunit
)
{
    assert(!(NULL == fbunit));
    if (NULL == fbunit)
        return -1;

    canvas_dbg("var.xres %d\n", fbunit->var_info.xres);
    canvas_dbg("var.yres %d\n", fbunit->var_info.yres);
    canvas_dbg("var.xres_virtual %d\n", fbunit->var_info.xres_virtual);
    canvas_dbg("var.yres_virtual %d\n", fbunit->var_info.yres_virtual);
    canvas_dbg("var.xoffset %d\n", fbunit->var_info.xoffset);
    canvas_dbg("var.yoffset %d\n", fbunit->var_info.yoffset);
    canvas_dbg("var.bits_per_pixel %d\n", fbunit->var_info.bits_per_pixel);
    canvas_dbg("var.grayscale %d\n", fbunit->var_info.grayscale);
    canvas_dbg("var.red %d %d %d\n", fbunit->var_info.red.offset, fbunit->var_info.red.length, fbunit->var_info.red.msb_right);
    canvas_dbg("var.green %d %d %d\n", fbunit->var_info.green.offset, fbunit->var_info.green.length, fbunit->var_info.green.msb_right);
    canvas_dbg("var.blue %d %d %d\n", fbunit->var_info.blue.offset, fbunit->var_info.blue.length, fbunit->var_info.blue.msb_right);
    canvas_dbg("var.transp %d %d %d\n", fbunit->var_info.transp.offset, fbunit->var_info.transp.length, fbunit->var_info.transp.msb_right);
    canvas_dbg("var.nonstd %d\n", fbunit->var_info.nonstd);
    canvas_dbg("var.activate %d\n", fbunit->var_info.activate);
    canvas_dbg("var.height %d\n", fbunit->var_info.height);
    canvas_dbg("var.width %d\n", fbunit->var_info.width);
    canvas_dbg("var.accel_flags %d\n", fbunit->var_info.accel_flags);
    canvas_dbg("var.pixclock %d\n", fbunit->var_info.pixclock);
    canvas_dbg("var.left_margin %d\n", fbunit->var_info.left_margin);
    canvas_dbg("var.right_margin %d\n", fbunit->var_info.right_margin);
    canvas_dbg("var.upper_margin %d\n", fbunit->var_info.upper_margin);
    canvas_dbg("var.lower_margin %d\n", fbunit->var_info.lower_margin);
    canvas_dbg("var.hsync_len %d\n", fbunit->var_info.hsync_len);
    canvas_dbg("var.vsync_len %d\n", fbunit->var_info.vsync_len);
    canvas_dbg("var.sync %d\n", fbunit->var_info.sync);
    canvas_dbg("var.vmode %d\n", fbunit->var_info.vmode);
    canvas_dbg("var.rotate %d\n", fbunit->var_info.rotate);
    canvas_dbg("var.colorspace %d\n", fbunit->var_info.colorspace);

    canvas_dbg("fix.smem_start %llu\n", fbunit->fix_info.smem_start);
    canvas_dbg("fix.smem_len %d\n", fbunit->fix_info.smem_len);
    canvas_dbg("fix.type %d\n", fbunit->fix_info.type);
    canvas_dbg("fix.type_aux %d\n", fbunit->fix_info.type_aux);
    canvas_dbg("fix.visual %d\n", fbunit->fix_info.visual);
    canvas_dbg("fix.xpanstep %d\n", fbunit->fix_info.xpanstep);
    canvas_dbg("fix.ypanstep %d\n", fbunit->fix_info.ypanstep);
    canvas_dbg("fix.ywrapstep %d\n", fbunit->fix_info.ywrapstep);
    canvas_dbg("fix.line_length %d\n", fbunit->fix_info.line_length);
    canvas_dbg("fix.mmio_start %llu\n", fbunit->fix_info.mmio_start);
    canvas_dbg("fix.mmio_len %d\n", fbunit->fix_info.mmio_len);
    canvas_dbg("fix.accel %d\n", fbunit->fix_info.accel);
    canvas_dbg("fix.capabilities %d\n", fbunit->fix_info.capabilities);

    return 0;
}

int32_t print_screenfb_info(
    struct fbscreen *fbscreen
)
{
    for (int32_t i = 0; i <= fbscreen->limit; i++)
    {
        canvas_dbg("framebufer: %d \n", i);
        print_fbunit_info(&fbscreen->fbunits[i]);
    }
    return 0;
}

struct canvascmd commands[] = {
    { CANVAS_CMD_CLEAR, canvascmd_clear_screen },
    { CANVAS_CMD_GETDIMENSION, canvascmd_get_dimension },
    { CANVAS_CMD_RECTANGLE, canvascmd_draw_rectangle },
    { CANVAS_CMD_CIRCLE, canvascmd_draw_circle },
    { CANVAS_CMD_FLUSH_DRAWING, canvascmd_flush_drawing},
    { CANVAS_CMD_DUMMY, canvascmd_do_nothing },
// other commands ...
// and NULL terminated list of commands
    {0},
};

void test_screen(
    struct fbscreen *fbscreen
)
{
    struct fbscreen_fbunit *fbunit = &fbscreen->fbunits[ fbscreen->index ];
    fbscreen_clear_screen(fbscreen, CANVAS_COLOR_GREEN);
    // rectangle
    struct fbscreen_rectangle rectangle = {
        .xpos = fbunit->var_info.xres_virtual/2,
        .ypos = fbunit->var_info.yres_virtual/2,
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
        .xpos = fbunit->var_info.xres_virtual/2,
        .ypos = fbunit->var_info.yres_virtual/2,
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

#ifdef DISABLE_TTY_PATH
    result = disable_tty(DISABLE_TTY_PATH);
    if (0 > result)
    {
        printf("cannot disable tty %d\n", result);
        goto error0;
    }    
#endif

    result = fbscreen_init(&fbscreen, FB_DEVICE);
    if (0 > result)
    {
        printf("cannot initialize framebuffers, error %d\n", result);
        goto error1;
    }
    result = spidevice_init(&spidevice, SPIDEV_PATH, SPIDEV_SPEED);
    if (0 > result)
    {
        printf("cannot initialize spi device, error %d\n", result);
        goto error2;
    }

    // print_screenfb_info(&fbscreen);
    // test_screen(&fbscreen);
    main_loop(&fbscreen, &spidevice, commands);

error2:
    spidevice_deinit(&spidevice);
error1:
    fbscreen_deinit(&fbscreen);
error0:
    return result; 
}

