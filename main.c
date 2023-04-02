#include "lib.h"

#define ARRLEN(n)     (sizeof(n) / sizeof(*n))

void os_event_key(int key, int action)
{
}

void os_update(void)
{
	render_rect(10, 10, 50, 50, render_color(0, 255, 0));
}

int main(void)
{
	os_init();
	return 0;
}
