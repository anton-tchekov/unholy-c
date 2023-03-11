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

#elif PLATFORM == PLATFORM_LINUX

#include <stdio.h>

static void uart_init(void)
{
	/* Do Nothing */
}

static u8 uart_available(void)
{
	return 1;
}

static void uart_tx(char c)
{
	fputc(c, stdout);
}

static i16 uart_rx(void)
{
	return fgetc(stdin);
}

#endif
