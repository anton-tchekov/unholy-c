#define COLOR_RED    "\033[31;1m"
#define COLOR_PURPLE "\033[35;1m"
#define COLOR_RESET  "\033[0m"

#if PLATFORM == PLATFORM_AVR

#define UART_BUFFER_SIZE  32
#define _BAUD               (((F_CPU / (UART_BAUD * 16UL))) - 1)

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
		while(_tb_available >= UART_TRANSMIT_BUFFER_SIZE - 1) ;

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
	if(_rb_available < UART_RECEIVE_BUFFER_SIZE)
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

static u8 file_open(const char *filename, const char *mode)
{
	/* TODO */
	return 0;
}

static void file_close(u8 stream)
{
	/* TODO */
}

static void file_write(u8 stream, const void *buffer, u16 size)
{
	/* TODO */
}

static void file_read(u8 stream, const void *buffer, u16 size)
{
	/* TODO */
}

static void file_seek(u8 file, u32 pos)
{
	/* TODO */
}

static void file_copy(const char *dst, const char *src)
{
	/* TODO */
}

static void file_move(const char *dst, const char *src)
{
	/* TODO */
}

static void file_delete(const char *filename)
{
	/* TODO */
}

static u32 file_size(const char *filename)
{
	/* TODO */
	return 0;
}

static void stream_chr(u8 stream, char value)
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

#elif PLATFORM == PLATFORM_LINUX

static FILE *_files[256];

static void stream_init(void)
{
	/* Do Nothing */
}

static u8 _find_slot(void)
{
	u8 i;
	for(i = 0; i < ARRLEN(_files); ++i)
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
	u8 rv, id = _find_slot();
	printf("%s", filename);
	_files[id] = rv = fopen(filename, mode);
	return rv ? id : 0;
}

static void file_close(u8 stream)
{
	fclose(_files[stream]);
	_files[stream] = 0;
}

static void file_write(u8 stream, u8 bank, u16 addr, u16 size)
{
	/* TODO */
}

static void file_read(u8 stream, u8 bank, u16 addr, u16 size)
{
	if(stream == 0)
	{
		/* TODO */
	}
	else
	{
		fread(_calculate_ptr(bank, addr), 1, size, _files[stream]);
	}
}

static void file_seek(u8 file, u32 pos)
{
	/* TODO */
}

static void file_copy(const char *dst, const char *src)
{
	/* TODO */
}

static void file_move(const char *dst, const char *src)
{
	/* TODO */
}

static void file_delete(const char *filename)
{
	/* TODO */
}

static u32 file_size(const char *filename)
{
	/* TODO */
	return 0;
}

static void stream_chr(u8 stream, char value)
{
	if(stream == 0)
	{
		fputc(value, stdout);
	}
	else
	{
		/* TODO */
	}
}

#endif

/* PRINT */
static void stream_str(u8 stream, const char *s)
{
	char c;
	while((c = *s++))
	{
		stream_chr(stream, c);
	}
}

static void stream_str_P(u8 stream, const char *s)
{
	char c;
	while((c = pgm_read_byte(s++)))
	{
		stream_chr(stream, c);
	}
}

static void stream_str_X(u8 stream, u8 bank, u16 addr)
{
	char c;
	while((c = memory_r8(bank, addr++)))
	{
		stream_chr(stream, c);
	}
}

static void stream_str_ext_X(u8 stream, u8 bank, u16 addr, u16 len)
{
	/* TODO */
}

static void stream_dec(u8 stream, u32 value)
{
	/* TODO */
}

static void stream_dec_ext(u8 stream, u32 value, u8 width)
{
	/* TODO */
}

static void stream_hex(u8 stream, u32 value)
{
	/* TODO */
}

static void stream_hex_ext(u8 stream, u32 value, u8 width)
{
	/* TODO */
}

static void stream_float(u8 stream, f32 value)
{
	/* TODO */
}

static void stream_float_ext(u8 stream, f32 value, u8 width, u8 decimal)
{
	/* TODO */
}

