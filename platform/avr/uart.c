#define UART_BAUD        9600
#define _BAUD                (((F_CPU / (UART_BAUD * 16UL))) - 1)

static void uart_init(void)
{
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	UBRR0 = _BAUD;
}

static void uart_tx(char c)
{
	while(!(UCSR0A & (1 << UDRE0))) {}
	UDR0 = c;
}
