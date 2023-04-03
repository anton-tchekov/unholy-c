#include <stdio.h>

static void uart_tx(char c)
{
	fputc(c, stdout);
}

static i16 uart_rx(void)
{
	return fgetc(stdin);
}
