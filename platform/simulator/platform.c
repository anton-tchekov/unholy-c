#include "types.h"
#include "util.h"
#include "platform.h"

#include "timer.c"
#include "memory.c"
#include "graphics.c"
#include "uart.c"
#include "fs.c"
#include "stream.c"

void platform_init(void)
{
	timer_init();
	graphics_init();
}
