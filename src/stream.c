#define COLOR_RED    "\033[31;1m"
#define COLOR_PURPLE "\033[35;1m"
#define COLOR_RESET  "\033[0m"

#if PLATFORM == PLATFORM_AVR

#define UART_BUFFER_SIZE  32
#define _BAUD               (((F_CPU / (UART_BAUD * 16UL))) - 1)

static volatile u8
	_tb_available,
	_rb_available;

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

static void uart_tx_str(const char *s)
{
	register char c;
	while((c = *s++))
	{
		uart_tx(c);
	}
}

static void uart_tx_str_P(const char *s)
{
	register char c;
	while((c = pgm_read_byte(s++)))
	{
		uart_tx(c);
	}
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

static u8 stream_open(const char *filename, const char *mode)
{

}

static void stream_close(u8 stream)
{

}

static void stream_write(u8 stream, const void *buffer, u16 size)
{

}

static void stream_read(u8 stream, const void *buffer, u16 size)
{

}

/* PRINT */
static void stream_str(u32 *p)
{
}

static void stream_str_ext()
{
	/* TODO */
}

static void stream_dec(u8 stream, u32 value)
{
}

static void stream_dec_ext(u8 stream, u32 value)
{
	/* TODO */
}

static void stream_hex(u8 stream, u32 value)
{
}

static void stream_hex_ext(u8 stream, u32 value)
{
	/* TODO */
}

static void stream_float(u8 stream, f32 value)
{

}

static void stream_float_ext(u8 stream, f32 value)
{
	/* TODO */
}

static void stream_chr(u8 stream, char value)
{
}

#elif PLATFORM == PLATFORM_LINUX

static void stream_init(void)
{
}

#endif

/*
file_seek()
file_copy()
file_move()
file_delete()
file_size()
file_list()
*/
