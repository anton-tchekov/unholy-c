#ifndef __LIB_H__
#define __LIB_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "types.h"

#define WINDOW_WIDTH          320
#define WINDOW_HEIGHT         480

typedef u32 Color;

void os_init(void);

Color render_color(u8 r, u8 g, u8 b);
void render_rect(i32 x, i32 y, i32 w, i32 h, Color color);

/* User Implemented */
void os_update(void);
void os_event_key(int key, int action);

#endif

