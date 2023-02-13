#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>

#define PLATFORM_LINUX 0
#define PLATFORM_AVR   1

typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef float f32;

#if PLATFORM == PLATFORM_LINUX

#define PROGMEM

#define pgm_read_word(address) *(address)
#define pgm_read_byte(address) *(address)
#define pgm_read_ptr(address) *(address)

#endif

#endif

