#define COLOR_RED    "\033[31;1m"
#define COLOR_PURPLE "\033[35;1m"
#define COLOR_RESET  "\033[0m"

/* CHAR */
static void stream_fputc(u8 stream, char value)
{
	if(stream == 0)
	{
		uart_tx(value);
	}
#ifdef ENABLE_FILE
	else
	{
		fs_fputc(stream, value);
	}
#endif
}

/* STRING */
static void stream_fputs(u8 stream, const char *s)
{
	char c;
	while((c = *s++))
	{
		stream_fputc(stream, c);
	}
}

static void stream_fputs_P(u8 stream, const char *s)
{
	char c;
	while((c = pgm_read_byte(s++)))
	{
		stream_fputc(stream, c);
	}
}

static void stream_fputs_X(u8 stream, u8 bank, u16 addr)
{
	char c;
	while((c = memory_r8(bank, addr++)))
	{
		stream_fputc(stream, c);
	}
}

static void stream_fputse_X(u8 stream, u8 bank, u16 addr, u16 len)
{
	while(len--)
	{
		stream_fputc(stream, memory_r8(bank, addr++));
	}
}

/* DEC */
static void _print_radix(u8 stream, i32 value, u8 radix, u8 width)
{
	char *p, buf[16];
	u8 sign, i, len, pad;
	u32 v;

	p = buf;
	sign = (radix == 10 && value < 0);
	v = sign ? -value : value;
	while(v || p == buf)
	{
		i = v % radix;
		v /= radix;
		*p++ = (i < 10) ? i + '0' : i + 'A' - 10;
	}

	if(sign)
	{
		*p++ = '-';
	}

	len = p - buf;
	pad = (radix == 10) ? ' ' : '0';
	while(len < width)
	{
		*p++ = pad;
		++len;
	}

	while(p > buf)
	{
		stream_fputc(stream, *--p);
	}
}

static void stream_fputd(u8 stream, i32 value)
{
	_print_radix(stream, value, 10, 0);
}

static void stream_fputde(u8 stream, i32 value, u8 width)
{
	_print_radix(stream, value, 10, width);
}

/* HEX */
static void stream_fputx(u8 stream, u32 value)
{
	_print_radix(stream, value, 16, 0);
}

static void stream_fputxe(u8 stream, u32 value, u8 width)
{
	_print_radix(stream, value, 16, width);
}

#ifdef ENABLE_FLOAT

/* FLOAT */
static void stream_fputfe(u8 stream, f32 value, u8 width, u8 decimal)
{
	char c;
	i32 int_part;

	int_part = value;
	value = fabs(value - int_part);
	_print_radix(stream, int_part, 10, width);
	if(decimal)
	{
		stream_fputc(stream, '.');
		while(decimal--)
		{
			value *= 10.0;
			c = value;
			stream_fputc(stream, '0' + c);
			value -= c;
		}
	}
}

static void stream_fputf(u8 stream, f32 value)
{
	stream_fputfe(stream, value, 0, 2);
}

#endif
