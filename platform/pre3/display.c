/*
 * Copyright (c) 2010 Travis Geiselbrecht
 *
 * Copyright (c) 2011 James Sullins
 *
 * Copyright (c) 2012 xndcn
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <err.h>
#include <debug.h>
#include <platform.h>
#include <dev/display.h>
#include <dev/fbcon.h>
#include <lib/gfx.h>
#include <reg.h>
#include <kernel/thread.h>

static int display_w, display_h;
static void *display_fb;

int fb_flush(void *arg)
{
	while (1) {
		thread_sleep(1);	
		fbcon_flush();
	}
}
void platform_init_display()
{
	display_fb = fbcon_display()->base;
	display_w = 480;
	display_h = 800;
	
	thread_resume(thread_create("fb_flush", &fb_flush, NULL, HIGH_PRIORITY, DEFAULT_STACK_SIZE));
}

uint lasty = 0;
void flush(uint starty, uint endy)
{
	if (starty != lasty) {
		lasty = starty;
		fbcon_flush();
	}
}

void display_get_info(struct display_info *info)
{
	info->framebuffer = display_fb;
	info->format = GFX_FORMAT_RGB_565;
	info->width = display_w;
	info->height = display_h;
	info->stride = display_w;
	info->flush = NULL;//flush;
	info->rotation = 0;	
}
