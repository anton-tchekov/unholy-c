#ifndef __OS_H__
#define __OS_H__

#include "types.h"

void *os_malloc(i32 size);
void *os_realloc(void *buffer, i32 new_size);

#endif /* __OS_H__ */
