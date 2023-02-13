#include "types.h"

#define OFFSET_CODE           0x0000
#define OFFSET_DATA           0x8000
#define OFFSET_INPUT          0x10000
#define OFFSET_BREAK_STACK    0x20000
#define OFFSET_CONTINUE_STACK 0x20400
#define OFFSET_VARIABLES      0x20800
#define OFFSET_FUNCTIONS      0x20C00

#if PLATFORM == PLATFORM_LINUX

static u8 _output[512 * 1024];

/* WRITE ADDRESS */
static void memory_w8(u32 addr, u8 val)
{
	_output[addr] = val;
}

static void memory_w16(u32 addr, u16 val)
{
	_write_16(_output + addr, val);
}

static void memory_w32(u32 addr, u32 val)
{
	_write_32(_output + addr, val);
}

/* READ ADDRESS */
static u8 memory_r8(u32 addr)
{
	return _output[addr];
}

static u16 memory_r16(u32 addr)
{
	return _read_16(_output + addr);
}

static u32 memory_r32(u32 addr)
{
	return _read_32(_output + addr);
}


#elif PLATFORM == PLATFORM_AVR



#endif
