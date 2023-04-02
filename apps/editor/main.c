#include <stdio.h>
#include <string.h>
#include "line.h"

int main(void)
{
	Line *line = line_new(10);
	line_debug(line);

	line_insert(&line, 0, "Hello World!", 12);
	line_debug(line);

	line_insert(&line, 0, "Test 123", 8);
	line_debug(line);

	line_insert(&line, 0, "Abrakadabra Simsalabim Motherfuckers ", 37);
	line_debug(line);

	line_insert(&line, 12, "Seraphim ", 9);
	line_debug(line);

	line_delete(&line, 46, 4);
	line_debug(line);

	line_replace(&line, 32, 13, "69 Nice", 7);
	line_debug(line);

	return 0;
}

