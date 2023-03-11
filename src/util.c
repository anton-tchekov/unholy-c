#define ARRLEN(a) (sizeof(a) / sizeof(*a))
#define ABS(n) (((n) < 0) ? -(n) : (n))

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

#ifdef ENABLE_FLOAT

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

#endif
