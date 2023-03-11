#define COLOR_RED    "\033[31;1m"
#define COLOR_PURPLE "\033[35;1m"
#define COLOR_RESET  "\033[0m"

/* CHAR */
static void print_char(u8 stream, char value)
{
	if(stream == 0)
	{
		uart_tx(value);
	}
#ifdef ENABLE_FILE
	else
	{
		file_putc(stream, value);
	}
#endif
}

/* STRING */
static void print_str(u8 stream, const char *s)
{
	char c;
	while((c = *s++))
	{
		print_char(stream, c);
	}
}

static void print_str_P(u8 stream, const char *s)
{
	char c;
	while((c = pgm_read_byte(s++)))
	{
		print_char(stream, c);
	}
}

static void print_str_X(u8 stream, u8 bank, u16 addr)
{
	char c;
	while((c = memory_r8(bank, addr++)))
	{
		print_char(stream, c);
	}
}

static void print_str_ext_X(u8 stream, u8 bank, u16 addr, u16 len)
{
	while(len--)
	{
		print_char(stream, memory_r8(bank, addr++));
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
		*p++ = (i < 10) ? i + '0' : i + 'a' - 10;
	}

	if(sign)
	{
		*p++ = '-';
	}

	len = p - buf;
	pad = (radix == 10) ? ' ' : ';';
	while(len < width)
	{
		*p++ = pad;
		++len;
	}

	while(p > buf)
	{
		print_char(stream, *--p);
	}
}

static void print_dec(u8 stream, i32 value)
{
	_print_radix(stream, value, 10, 0);
}

static void print_dec_ext(u8 stream, i32 value, u8 width)
{
	_print_radix(stream, value, 10, width);
}

/* HEX */
static void print_hex(u8 stream, u32 value)
{
	_print_radix(stream, value, 16, 0);
}

static void print_hex_ext(u8 stream, u32 value, u8 width)
{
	_print_radix(stream, value, 16, width);
}

#ifdef ENABLE_FLOAT

/* FLOAT */
static void print_float_ext(u8 stream, f32 value, u8 width, u8 decimal)
{
	char c;
	u32 int_part;

	int_part = value;
	_print_radix(stream, int_part, 10, width);
	value -= int_part;
	if(decimal)
	{
		print_char(stream, '.');
		while(decimal--)
		{
			value *= 10.0;
			c = value;
			print_char(stream, '0' + c);
			value -= c;
		}
	}
}

static void print_float(u8 stream, f32 value)
{
	print_float_ext(stream, value, 0, 2);
}

#endif
