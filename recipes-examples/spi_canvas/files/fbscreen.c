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

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <sys/types.h>
#include <linux/kd.h>

#include "config.h"
#include "fbscreen.h"
#include "math.h"

/* https://www.kernel.org/doc/Documentation/fb/fbuffer.txt
 * https://www.kernel.org/doc/Documentation/fb/api.txt */

int32_t fbscreen_init(
    struct fbscreen *fbscreen,
    const char *dev_path,
    const char *disable_tty_path
)
{
    if (NULL == fbscreen || NULL == dev_path)
        return -1;
    if ((fbscreen->fd = open(dev_path, O_RDWR)) < 0)
        return -1;
    if (ioctl(fbscreen->fd, FBIOGET_FSCREENINFO, &fbscreen->fix_info) < 0)
        return -1;
    if (ioctl(fbscreen->fd, FBIOGET_VSCREENINFO, &fbscreen->var_info) < 0)
        return -1;
    fbscreen->fsize = fbscreen->var_info.yres_virtual * fbscreen->fix_info.line_length;
    if ((fbscreen->fbuffer = mmap(
        NULL, fbscreen->fsize, PROT_READ | PROT_WRITE, MAP_SHARED, fbscreen->fd, 0)) < 0
    )
    {
        fbscreen->fbuffer = NULL;
        return -1;
    }
    /* switch tty to graphic mode
     * - get rid of blinking cursor
     * - disable sleep mode */
    if (NULL != disable_tty_path)
    {
        int32_t fd;
        if ((fd = open(disable_tty_path, O_RDWR)) < 0)
            return fd;
        if (ioctl(fd, KDSETMODE, KD_GRAPHICS) < 0)
            return -1;
    }
    return 0;
}

int32_t fbscreen_deinit(
    struct fbscreen *fbscreen
)
{
    if (NULL == fbscreen)
        return -1;
    if (fbscreen->fbuffer)
        munmap(fbscreen->fbuffer, fbscreen->fsize);
    if (fbscreen->fd >= 0)
        close(fbscreen->fd);
    return 0;
}

int32_t fbscreen_set_pixel(
    const struct fbscreen *fbscreen,
    const int32_t xpos,
    const int32_t ypos,
    const uint32_t color
)
{
    uint8_t *color_addr;
    assert(!(NULL == fbscreen));
    if (NULL == fbscreen)
        return -1;

    if (xpos < 0 || ypos < 0 || xpos >= fbscreen->var_info.xres_virtual || ypos >= fbscreen->var_info.yres_virtual)
        return -2;

    /* TODO: fix integer range */
    uint32_t pixel_position = ((xpos + fbscreen->var_info.xoffset) * fbscreen->var_info.bits_per_pixel/8);
    pixel_position += (ypos + fbscreen->var_info.yoffset) * fbscreen->fix_info.line_length;

    /* calculate pixel base address */
    color_addr = fbscreen->fbuffer + pixel_position;

    /* setup color of pixels */
    *(color_addr + (fbscreen->var_info.red.offset >> 3)) = CANVAS_RGBCOLOR_RED(color);
    *(color_addr + (fbscreen->var_info.green.offset >> 3)) = CANVAS_RGBCOLOR_GREEN(color);
    *(color_addr + (fbscreen->var_info.blue.offset >> 3)) = CANVAS_RGBCOLOR_BLUE(color);

    return 0;
}

int32_t fbscreen_get_pixel(
    const struct fbscreen *fbscreen,
    const int32_t xpos,
    const int32_t ypos,
    uint32_t *color
)
{
    uint8_t *color_addr;
    assert(!(NULL == fbscreen));
    if (NULL == fbscreen)
        return -1;

    if (xpos < 0 || ypos < 0 || xpos >= fbscreen->var_info.xres_virtual || ypos >= fbscreen->var_info.yres_virtual)
        return -2;

    /* TODO: fix integer range */
    uint32_t pixel_position = ((xpos + fbscreen->var_info.xoffset) * fbscreen->var_info.bits_per_pixel/8);
    pixel_position += (ypos + fbscreen->var_info.yoffset) * fbscreen->fix_info.line_length;

    /* calculate pixel base address */
    color_addr = fbscreen->fbuffer + pixel_position;

    /* get color at address */
    *color = CANVAS_RGBCOLOR(
        *(color_addr + (fbscreen->var_info.red.offset >> 3)),
        *(color_addr + (fbscreen->var_info.green.offset >> 3)),
        *(color_addr + (fbscreen->var_info.blue.offset >> 3))
    );

    return 0;
}

int32_t fbscreen_clear_screen(
    const struct fbscreen *fbscreen,
    const uint32_t color
)
{
    for (int32_t i = 0; i < fbscreen->var_info.xres_virtual; i++)
    {
        for (int32_t j = 0; j < fbscreen->var_info.yres_virtual; j++)
        {
            fbscreen_set_pixel(fbscreen, i, j, color);
        }
    }
    return 0;
}

int32_t fbscreen_draw_rectangle(
    const struct fbscreen *fbscreen,
    const struct fbscreen_rectangle *rectangle
)
{
    assert(!(NULL == fbscreen || NULL == rectangle));
    if (NULL == fbscreen || NULL == rectangle)
    {
        return -1;
    }

    int32_t xpos = rectangle->xpos, ypos = rectangle->ypos;
    if (rectangle->in_centre)
    {
        xpos -= rectangle->width/2;
        ypos -= rectangle->height/2;
    }

    for (int32_t i = 0; i < rectangle->width; i++)
    {
        for (int32_t j = 0; j < rectangle->height; j++)
        {
            fbscreen_set_pixel(
                fbscreen, xpos + i, ypos + j, rectangle->color
            );
        }
    }

    return 0;
}

int32_t fbscreen_draw_circle(
    const struct fbscreen *fbscreen,
    const struct fbscreen_circle *circle
)
{
    assert(!(NULL == fbscreen || NULL == circle));
    if (NULL == fbscreen || NULL == circle)
    {
        return -1;
    }

    int32_t xpos = circle->xpos, ypos = circle->ypos;
    if (!circle->in_centre)
    {
        xpos += circle->radius;
        ypos += circle->radius;
    }

    int32_t xlimit;
    int32_t radius_sqr = circle->radius * circle->radius; 

    for (int32_t i = 0; i < circle->radius; i++)
    {
        xlimit = sqrt(radius_sqr - (i * i));
        for (int32_t j = 0; j < xlimit; j++)
        {
            fbscreen_set_pixel(
                fbscreen, xpos + i, ypos + j, circle->color
            );
            fbscreen_set_pixel(
                fbscreen, xpos - i, ypos + j, circle->color
            );
            fbscreen_set_pixel(
                fbscreen, xpos + i, ypos - j, circle->color
            );
            fbscreen_set_pixel(
                fbscreen, xpos - i, ypos - j, circle->color
            );
        }
    }

    return 0;
}
