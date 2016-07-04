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

#ifndef __FBSCREEN_H__
#define __FBSCREEN_H__

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

#define RGBCOLOR(r, g, b)           (uint32_t)( (((uint8_t)r) << 16) | (((uint8_t)g) << 8) | (((uint8_t)b)))
#define RGBCOLOR_TO_RED(color)      ((color >> 16) & 0xFF)
#define RGBCOLOR_TO_GREEN(color)    ((color >> 8) & 0xFF)
#define RGBCOLOR_TO_BLUE(color)     (color & 0xFF)

#define WHITE_COLOR     RGBCOLOR(0xFF, 0xFF, 0xFF)
#define BLACK_COLOR     RGBCOLOR(0x00, 0x00, 0x00)
#define RED_COLOR       RGBCOLOR(0xFF, 0x00, 0x00)
#define GREEN_COLOR     RGBCOLOR(0x00, 0xFF, 0x00)
#define BLUE_COLOR      RGBCOLOR(0x00, 0x00, 0xFF)

struct fbscreen {
    int32_t fd;
    struct fb_var_screeninfo var_info;
    struct fb_fix_screeninfo fix_info;
    uint8_t *fbuffer;
    uint32_t fsize;
};

//TODO: packed !!
struct fbscreen_circle {
    int32_t xpos;
    int32_t ypos;
    uint32_t color;
    uint32_t in_centre;
    uint32_t radius;
};

struct fbscreen_rectangle {
    int32_t xpos;
    int32_t ypos;
    uint32_t color;
    uint32_t in_centre;
    uint32_t width;
    uint32_t height;
};

struct fbscreen_triangle {
    int32_t xpos;
    int32_t ypos;
    uint32_t color;
    uint32_t in_centre;
    int32_t pointa_x;
    int32_t pointa_y;
    int32_t pointb_x;
    int32_t pointb_y;
    int32_t pointc_x;
    int32_t pointc_y;
};

struct fbscreen_dimension {
    int32_t width;
    int32_t height;
};

int32_t fbscreen_init(
    struct fbscreen *fbscreen,
    const char *dev_path
);

int32_t fbscreen_deinit(
    struct fbscreen *fbscreen
);

int32_t fbscreen_set_pixel(
    const struct fbscreen *fbscreen,
    const int32_t xpos,
    const int32_t ypos,
    const uint32_t color
);

int32_t fbscreen_clear_screen(
    const struct fbscreen *fbscreen,
    const uint32_t color
);

int32_t fbscreen_draw_rectangle(
    const struct fbscreen *fbscreen,
    const struct fbscreen_rectangle *rectangle
);

int32_t fbscreen_draw_circle(
    const struct fbscreen *fbscreen,
    const struct fbscreen_circle *circle
);


#endif
