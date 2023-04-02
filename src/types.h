#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>

/* Memory banks */
#define BANK_INPUT             0
#define BANK_PARSER            0
#define BANK_INTERPRETER       1

/* Input */
#define OFFSET_INPUT           0x0000

/* Parser */
#define OFFSET_BREAK_STACK     0x8000
#define OFFSET_CONTINUE_STACK  0x8400
#define OFFSET_VARIABLES       0x8800
#define OFFSET_FUNCTIONS       0x8C00
#define OFFSET_CONSTANTS       0x9000
#define OFFSET_FUNCTION_PARAMS 0x9400
#define OFFSET_FUNCTION_ADDRS  0x9800
#define OFFSET_FUNCTION_USAGES 0x9C00

/* Interpreter */
#define OFFSET_CODE            0x0000
#define OFFSET_RODATA          0x4000
#define OFFSET_DATA            0x8000

/* Types */
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef float f32;
typedef double f64;

typedef u8 bool;

#define true  1
#define false 0

/* Platform */
#define PLATFORM_LINUX         0
#define PLATFORM_AVR           1

#if PLATFORM == PLATFORM_AVR

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#elif PLATFORM == PLATFORM_LINUX

#define PROGMEM

#define pgm_read_word(address) *(address)
#define pgm_read_byte(address) *(address)
#define pgm_read_ptr(address) *(address)

#endif

#endif

