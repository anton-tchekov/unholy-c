#define ARRLEN(a) (sizeof(a) / sizeof(*a))
#define ABS(n) (((n) < 0) ? -(n) : (n))

static u16 _read_16(u8 *p);
static void _write_16(u8 *b, u16 v);

static u32 _read_32(u8 *p);
static void _write_32(u8 *b, u32 v);

static u16 _read_16(u8 *p)
{
	return ((u16)p[0]) | (((u16)p[1]) << 8);
}

static void _write_16(u8 *b, u16 v)
{
	b[0] = v & 0xFF;
	b[1] = v >> 8;
}

static u32 _read_32(u8 *p)
{
	return ((u32)p[0]) | (((u32)p[1]) << 8) |
		(((u32)p[2]) << 16) | (((u32)p[3]) << 24);
}

static void _write_32(u8 *b, u32 v)
{
	b[0] = v & 0xFF;
	b[1] = ((v >> 8) & 0xFF);
	b[2] = ((v >> 16) & 0xFF);
	b[3] = v >> 24;
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

/* memcpy needed because of strict aliasing rules */
static u32 fbti(f32 p)
{
	u32 i;
	memcpy(&i, &p, 4);
	return i;
}

static f32 ibtf(u32 p)
{
	f32 f;
	memcpy(&f, &p, 4);
	return f;
}
