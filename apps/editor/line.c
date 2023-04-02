#include "line.h"
#include "os.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static i32 _next_pot(i32 v)
{
	i32 power;

	power = 1;
	while(power < v)
	{
		power *= 2;
	}

	return power;
}

Line *line_new(i32 capacity)
{
	i32 size;
	Line *line;

	size = _next_pot(sizeof(Line) + capacity);
	line = os_malloc(size);
	line->Length = 0;
	line->Capacity = size - sizeof(Line);
	return line;
}

void line_replace(Line **line, i32 start, i32 del_len, const char *str, i32 ins_len)
{
	i32 new_length;
	Line *l;

	l = *line;

	new_length = l->Length + ins_len - del_len;
	if(new_length > l->Capacity)
	{
		i32 new_capacity;

		/* increase capacity */
		new_capacity = _next_pot(sizeof(Line) + new_length);
		l->Capacity = new_capacity - sizeof(Line);
		l = os_realloc(l, new_capacity);
	}

	/* shift end */
	memmove(l->Buffer + start + ins_len,
		l->Buffer + start + del_len,
		l->Length - start - del_len);

	/* insert */
	memcpy(l->Buffer + start, str, ins_len);

	l->Length = new_length;

	*line = l;
}

void line_insert(Line **line, i32 start, const char *str, i32 new_len)
{
	line_replace(line, start, 0, str, new_len);
}

void line_delete(Line **line, i32 start, i32 len)
{
	line_replace(line, start, len, NULL, 0);
}

void line_destroy(Line *line)
{
	free(line);
}

void line_debug(Line *line)
{
	printf(
		"Capacity: %d\n"
		"Length: %d\n"
		"Content: \"%.*s\"\n\n",
		line->Capacity,
		line->Length,
		line->Length,
		line->Buffer);
}
