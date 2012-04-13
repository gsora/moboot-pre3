#ifndef __LIB_FONT_H
#define __LIB_FONT_H

#include <lib/gfx.h>

#define FONT_X	8
#define FONT_Y	16
#define FONT_COLS 1

void font_draw_char(gfx_surface *surface, unsigned char c, int x, int y, uint32_t fg_color, uint32_t bg_color);

#endif

