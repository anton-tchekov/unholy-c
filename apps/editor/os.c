#include "os.h"
#include <stdlib.h>

void *os_malloc(i32 size)
{
	void *buffer;

	if(!(buffer = malloc(size)))
	{
		exit(1);
	}

	return buffer;
}

void *os_realloc(void *buffer, i32 new_size)
{
	if(!(buffer = realloc(buffer, new_size)))
	{
		exit(1);
	}

	return buffer;
}
