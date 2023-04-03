static u8 spi_xchg(u8 data)
{
	SPDR = data;
	while(!(SPSR & (1 << SPIF))) {}
	return SPDR;
}
