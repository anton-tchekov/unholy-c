#define COLOR_RED    "\033[31;1m"
#define COLOR_PURPLE "\033[35;1m"
#define COLOR_RESET  "\033[0m"

#if PLATFORM == PLATFORM_AVR

#define UART_BAUD        9600
#define UART_BUFFER_SIZE   32
#define _BAUD                (((F_CPU / (UART_BAUD * 16UL))) - 1)

static volatile u8 _tb_available, _rb_available;

static volatile char
	_tb_start[UART_BUFFER_SIZE],
	_rb_start[UART_BUFFER_SIZE],
	*_tb_end, *_tb_read, *_tb_write,
	*_rb_end, *_rb_read, *_rb_write;

static void uart_init(void)
{
	UCSR0B = (1 << RXCIE0) | (1 << UDRIE0) | (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	UBRR0 = _BAUD;
	_tb_read = _tb_write = _tb_start;
	_rb_read = _rb_write = _rb_start;
}

static u8 uart_available(void)
{
	return _rb_available;
}

static void uart_tx(char c)
{
	if(UCSR0A & (1 << UDRE0))
	{
		UDR0 = c;
	}
	else
	{
		while(_tb_available >= UART_BUFFER_SIZE - 1) ;

		++_tb_available;
		*_tb_write++ = c;
		if(_tb_write == _tb_end)
		{
			_tb_write = _tb_start;
		}
	}
}

static i16 uart_rx(void)
{
	i16 c = -1;
	if(_rb_available)
	{
		--_rb_available;
		c = *_rb_read++;
		if(_rb_read == _rb_end)
		{
			_rb_read = _rb_start;
		}
	}

	return c;
}

ISR(USART_RX_vect)
{
	if(_rb_available < UART_BUFFER_SIZE)
	{
		++_rb_available;
		*_rb_write++ = UDR0;
		if(_rb_write == _rb_end)
		{
			_rb_write = _rb_start;
		}
	}
}

ISR(USART_UDRE_vect)
{
	if(_tb_available)
	{
		--_tb_available;
		UDR0 = *_tb_read++;
		if(_tb_read == _tb_end)
		{
			_tb_read = _tb_start;
		}
	}
}

static void stream_init(void)
{
	uart_init();
}

static void print_char(u8 stream, char value)
{
	if(stream == 0)
	{
		uart_tx(value);
	}
	else
	{
		/* TODO */
	}
}

static u8 file_open(const char *filename, const char *mode)
{
	/* TODO */
	return 1;
}

static void file_read(u8 file, u8 bank, u16 addr, u16 size)
{
	/* TODO */
}

static void file_close(u8 stream)
{
	/* TODO */
}

#ifdef ENABLE_FILE

static void file_write(u8 file, u8 bank, u16 addr, u16 size)
{
	/* TODO */
}

static void file_seek(u8 file, u32 pos)
{
	/* TODO */
}

static u32 file_tell(u8 file)
{
	/* TODO */
	return 0;
}

#endif

#elif PLATFORM == PLATFORM_LINUX

static FILE *_files[256];

static void print_char(u8 stream, char value)
{
	fputc(value, stream ? _files[stream] : stdout);
}

static u8 _find_slot(void)
{
	u32 i;
	for(i = 1; i < ARRLEN(_files); ++i)
	{
		if(!_files[i])
		{
			return i;
		}
	}

	return 0;
}

static u8 file_open(const char *filename, const char *mode)
{
	u8 id = _find_slot();
	return (_files[id] = fopen(filename, mode)) ? id : 0;
}

static void file_read(u8 file, u8 bank, u16 addr, u16 size)
{
	if(file)
	{
		fread(_calculate_ptr(bank, addr), 1, size, _files[file]);
	}
}

static void file_close(u8 file)
{
	fclose(_files[file]);
	_files[file] = 0;
}

#ifdef ENABLE_FILE

static void file_write(u8 file, u8 bank, u16 addr, u16 size)
{
	if(file)
	{
		fwrite(_calculate_ptr(bank, addr), 1, size, _files[file]);
	}
}

static void file_seek(u8 file, u32 pos)
{
	if(file)
	{
		fseek(_files[file], pos, SEEK_SET);
	}
}

static u32 file_tell(u8 file)
{
	return file ? ftell(_files[file]) : 0;
}

#endif

#endif

/* PRINT */
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

static void _print_radix(u8 stream, i32 value, u8 radix, u8 width)
{
	/* TODO */
    char tmp[16];
    char *tp = tmp;
    int i;
    unsigned v;

    int sign = (radix == 10 && value < 0);
    if (sign)
        v = -value;
    else
        v = (unsigned)value;

    while (v || tp == tmp)
    {
        i = v % radix;
        v /= radix;
        if (i < 10)
          *tp++ = i+'0';
        else
          *tp++ = i + 'a' - 10;
    }

    int len = tp - tmp;

    if (sign)
    {
        print_char(stream, '-');
        len++;
    }

    while (tp > tmp)
        print_char(stream, *--tp);
}


static void print_dec(u8 stream, i32 value)
{
	_print_radix(stream, value, 10, 0);
}

static void print_dec_ext(u8 stream, i32 value, u8 width)
{
	_print_radix(stream, value, 10, width);
}

static void print_hex(u8 stream, u32 value)
{
	_print_radix(stream, value, 16, 0);
}

static void print_hex_ext(u8 stream, u32 value, u8 width)
{
	_print_radix(stream, value, 16, width);
}

#ifdef ENABLE_FLOAT

static void print_float_ext(u8 stream, f32 value, u8 width, u8 decimal)
{
	/* TODO */
}

static void print_float(u8 stream, f32 value)
{
	print_float_ext(stream, value, 0, 2);
}

#endif
