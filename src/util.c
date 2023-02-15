#include "types.h"

#define ARRLEN(a) (sizeof(a) / sizeof(*a))
#define ABS(n) (((n) < 0) ? -(n) : (n))

static u16 _read_16(u8 *p);
static void _write_16(u8 *b, u16 v);

static u32 _read_32(u8 *p);
static void _write_32(u8 *b, u32 v);

static u16 _read_16(u8 *p)
{
	u16 v;
	v = *p | (*(p + 1) << 8);
	return v;
}

static void _write_16(u8 *b, u16 v)
{
	*b = (u8)(v & 0xFF);
	*(b + 1) = (u8)((v >> 8) & 0xFF);
}

static u32 _read_32(u8 *p)
{
	u32 v;
	v = *p | (*(p + 1) << 8) | (*(p + 2) << 16) | (*(p + 3) << 24);
	return v;
}

static void _write_32(u8 *b, u32 v)
{
	*b = (u8)(v & 0xFF);
	*(b + 1) = (u8)((v >> 8) & 0xFF);
	*(b + 2) = (u8)((v >> 16) & 0xFF);
	*(b + 3) = (u8)((v >> 24) & 0xFF);
}

static i8 _string_find(const char *str, const char *name)
{
	u8 found;
	i8 id;
	char c, d;
	const char *s, *p;

	id = 0;
	found = 1;
	s = str;
	p = name;
	while((c = pgm_read_byte(s++)) != '|')
	{
		d = *p++;
		if(c == '\0')
		{
			if(found == 1 && !d)
			{
				return id;
			}

			found = 1;
			++id;
			p = name;
		}
		else if(!d || d != c)
		{
			found = 0;
		}
	}

	return -1;
}
