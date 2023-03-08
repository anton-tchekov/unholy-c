#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>

#define BANK_INPUT             0
#define BANK_PARSER            1
#define BANK_INTERPRETER       2

#define OFFSET_INPUT           0x0000

#define OFFSET_CODE            0x0000
#define OFFSET_RODATA          0x4000
#define OFFSET_DATA            0x8000

#define OFFSET_BREAK_STACK     0x0000
#define OFFSET_CONTINUE_STACK  0x0400
#define OFFSET_VARIABLES       0x0800
#define OFFSET_FUNCTIONS       0x0C00
#define OFFSET_CONSTANTS       0x1000
#define OFFSET_FUNCTION_PARAMS 0x1400
#define OFFSET_FUNCTION_ADDRS  0x1800
#define OFFSET_FUNCTION_USAGES 0x1C00

#define PLATFORM_LINUX         0
#define PLATFORM_AVR           1

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

