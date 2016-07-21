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

#include "config.h"
#include "fbscreen.h"
#include "math.h"
#include "string.h"

/* https://www.kernel.org/doc/Documentation/fb/fbuffer.txt
 * https://www.kernel.org/doc/Documentation/fb/api.txt */

static int32_t fbscreen_initfb
(
    const char *path,
    struct fbscreen_fbunit *fbunit
)
{
    int32_t result;
    /* open framebuffer */
    fbunit->fd = open(path, O_RDWR);
    assert(!(fbunit->fd < 0));
    if (fbunit->fd < 0) return -1;
    /* get fixed info */
    result = ioctl(fbunit->fd, FBIOGET_FSCREENINFO, &fbunit->fix_info);
    assert(!(result < 0));
    if (result < 0) return -1;
    /* get variable info */
    result = ioctl(fbunit->fd, FBIOGET_VSCREENINFO, &fbunit->var_info);
    assert(!(result < 0));
    if (result < 0) return -1;
    /* expected size of file */
    fbunit->size = fbunit->var_info.yres_virtual * fbunit->fix_info.line_length;
    /* map file into memory */
    fbunit->fbmem = mmap(
        NULL, fbunit->size, PROT_READ | PROT_WRITE, MAP_SHARED, fbunit->fd, 0
    );
    //0 ??
    assert(!(fbunit->fbmem <= 0));
    if (fbunit->fbmem <= 0)
    {
        fbunit->fbmem = NULL;
        return -1;
    }

    return 0;
}

// int32_t fbscreen_init(
//     struct fbscreen *fbscreen,
//     const char *fb_path1
//     const char *fb_path2
// )
int32_t fbscreen_init(
    struct fbscreen *fbscreen,
    const char *fb_path1
)
{
    int32_t result = -1;
    // const char const *paths[] = { fb_path1, fb_path2 };
    const char const *paths[] = { fb_path1 };

    // if ((NULL == fbscreen) || (NULL == fb_path1) || (NULL == fb_path2))
    assert(!((NULL == fbscreen) || (NULL == fb_path1)));
    if ((NULL == fbscreen) || (NULL == fb_path1))
        return -1;

    /* swapping framebuffers doesn't work the way I want.
     * I need to dig deeper in IPU options, this 
     * is just a workaround */
    fbscreen->limit = 0;
    fbscreen->index = 0;

    /* initialize framebuffers */
    for (int32_t i = 0; i <= fbscreen->limit; i++)
    {
        struct fbscreen_fbunit *fbunit = &fbscreen->fbunits[ i ];
        /* Initialize framebuffer */
        result = fbscreen_initfb(paths[ i ], fbunit);
        assert(!(result < 0));
        if (result < 0)
            return -1;
        /* workaround with temp memory */
        fbunit->tmp_fbmem = malloc(fbunit->size);
        assert(!(fbunit->tmp_fbmem == NULL));
        if (fbunit->tmp_fbmem == NULL)
            return -1;
        /* copy fbmem to fbmem_tmp*/
        memcpy(fbunit->tmp_fbmem, fbunit->fbmem, fbunit->size);
        /* use tmp memory instead swapping fbuffers */
        fbunit->use_tmp = 1;
    }

    return 0;
}

int32_t fbscreen_deinit(
    struct fbscreen *fbscreen
)
{
    if (NULL == fbscreen)
        return -1;

    for (int32_t i = 0; i <= fbscreen->limit; i++)
    {
        munmap(
            fbscreen->fbunits[ i ].fbmem,
            fbscreen->fbunits[ i ].size
        );
        close(fbscreen->fbunits[ i ].fd);
    }

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
    const struct fb_var_screeninfo *var_info = NULL;
    const struct fb_fix_screeninfo *fix_info = NULL;

    /* assert params */
    assert(!(NULL == fbscreen));
    if ((NULL == fbscreen) || (fbscreen->index > fbscreen->limit))
        return -1;

    /* get var/fix info */
    var_info = &fbscreen->fbunits[ fbscreen->index ].var_info;
    fix_info = &fbscreen->fbunits[ fbscreen->index ].fix_info;

    if ((xpos < 0) || (ypos < 0) || (xpos >= var_info->xres_virtual) || (ypos >= var_info->yres_virtual))
        return -2;

    /* TODO: fix integer range */
    uint32_t pixel_position = (xpos + var_info->xoffset) * (var_info->bits_per_pixel / 8);
    pixel_position += (ypos + var_info->yoffset) * fix_info->line_length;

    /* calculate pixel base address */
    if (fbscreen->fbunits[ fbscreen->index ].use_tmp)
    {
        color_addr = fbscreen->fbunits[ fbscreen->index ].tmp_fbmem + pixel_position;
    }
    else
    {
        color_addr = fbscreen->fbunits[ fbscreen->index ].fbmem + pixel_position;
    }

    /* setup color of pixels */
    *(color_addr + (var_info->red.offset >> 3)) = CANVAS_RGBCOLOR_RED(color);
    *(color_addr + (var_info->green.offset >> 3)) = CANVAS_RGBCOLOR_GREEN(color);
    *(color_addr + (var_info->blue.offset >> 3)) = CANVAS_RGBCOLOR_BLUE(color);

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
    const struct fb_var_screeninfo *var_info = NULL;
    const struct fb_fix_screeninfo *fix_info = NULL;

    /* assert params */
    assert(!(NULL == fbscreen));
    if ((NULL == fbscreen) || (fbscreen->index > fbscreen->limit))
        return -1;

    /* get var/fix info */
    var_info = &fbscreen->fbunits[ fbscreen->index ].var_info;
    fix_info = &fbscreen->fbunits[ fbscreen->index ].fix_info;

    if ((xpos < 0) || (ypos < 0) || (xpos >= var_info->xres_virtual) || (ypos >= var_info->yres_virtual))
        return -2;

    /* TODO: fix integer range */
    uint32_t pixel_position = (xpos + var_info->xoffset) * (var_info->bits_per_pixel / 8);
    pixel_position += (ypos + var_info->yoffset) * fix_info->line_length;

    /* calculate pixel base address */
    if (fbscreen->fbunits[ fbscreen->index ].use_tmp)
    {
        color_addr = fbscreen->fbunits[ fbscreen->index ].tmp_fbmem + pixel_position;
    }
    else
    {
        color_addr = fbscreen->fbunits[ fbscreen->index ].fbmem + pixel_position;
    }

    /* get color at address */
    *color = CANVAS_RGBCOLOR(
        *(color_addr + (var_info->red.offset >> 3)),
        *(color_addr + (var_info->green.offset >> 3)),
        *(color_addr + (var_info->blue.offset >> 3))
    );

    return 0;
}

int32_t fbscreen_clear_screen(
    const struct fbscreen *fbscreen,
    const uint32_t color
)
{
    /* get var/fix info */
    const struct fb_var_screeninfo *var_info = &fbscreen->fbunits[ fbscreen->index ].var_info;

    for (int32_t i = 0; i < var_info->xres_virtual; i++)
    {
        for (int32_t j = 0; j < var_info->yres_virtual; j++)
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

int32_t fbscreen_flush_drawing
(
    const struct fbscreen *fbscreen
)
{
    const struct fbscreen_fbunit *fbunit = NULL;

    assert(!(NULL == fbscreen));
    if (NULL == fbscreen)
    {
        return -1;
    }

    /* workaround - copy tmp_fbmem to fbmem 
     * instead swaping active/inactive frambuffers */
    fbunit = &fbscreen->fbunits[ fbscreen->index ];
    memcpy(fbunit->fbmem, fbunit->tmp_fbmem, fbunit->size);
    return 0;
}
