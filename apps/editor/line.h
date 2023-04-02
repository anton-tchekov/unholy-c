#ifndef __LINE_H__
#define __LINE_H__

#include "types.h"

typedef struct LINE
{
	i32 Capacity, Length;
	char Buffer[];
} Line;

Line *line_new(i32 capacity);
void line_replace(Line **line, i32 start, i32 del_len, const char *str, i32 ins_len);
void line_insert(Line **line, i32 start, const char *str, i32 new_len);
void line_delete(Line **line, i32 start, i32 len);
void line_destroy(Line *line);
void line_debug(Line *line);

#endif /* __LINE_H__ */
