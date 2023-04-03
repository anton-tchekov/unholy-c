#define SRAM_COMMAND_WRITE   2
#define SRAM_COMMAND_READ    3

#define SPI_DIR               DDRB
#define SPI_OUT               PORTB

#define SPI_CS_0             2
#define SPI_MOSI             3
#define SPI_MISO             4
#define SPI_SCK              5

/* PRIVATE */
static u8 _bank_to_pin(u8 bank)
{
	static const u8 _bank_pins[] = { SPI_CS_0 };
	return _bank_pins[bank >> 1];
}

static void _xmem_select(u8 bank)
{
	SPI_OUT &= ~(1 << _bank_to_pin(bank));
}

static void _xmem_deselect(u8 bank)
{
	SPI_OUT |= (1 << _bank_to_pin(bank));
}

static void _xmem_addr(u8 bank, u16 addr)
{
	spi_xchg(bank & 1);
	spi_xchg(addr >> 8);
	spi_xchg(addr & 0xFF);
}

/* PUBLIC */
static void xmem_init(void)
{
	SPI_DIR = (1 << SPI_MOSI) | (1 << SPI_SCK) |
			(1 << SPI_CS_0);
	SPCR = (1 << SPE) | (1 << MSTR);
	SPSR = (1 << SPI2X);
}

static void xmem_read(u8 bank, u16 addr, void *data, u16 size)
{
	u16 i;
	u8 *data8;
	_xmem_select(bank);
	data8 = (u8 *)data;
	spi_xchg(SRAM_COMMAND_READ);
	_xmem_addr(bank, addr);
	for(i = 0; i < size; ++i)
	{
		data8[i] = spi_xchg(0xFF);
	}

	_xmem_deselect(bank);
}

static void xmem_write(u8 bank, u16 addr, void *data, u16 size)
{
	u16 i;
	u8 *data8;
	_xmem_select(bank);
	data8 = (u8 *)data;
	spi_xchg(SRAM_COMMAND_WRITE);
	_xmem_addr(bank, addr);
	for(i = 0; i < size; ++i)
	{
		spi_xchg(data8[i]);
	}

	_xmem_deselect(bank);
}

static void xmem_set(u8 bank, u16 addr, u8 value, u16 size)
{
	u16 i;
	_xmem_select(bank);
	spi_xchg(SRAM_COMMAND_WRITE);
	_xmem_addr(bank, addr);
	for(i = 0; i < size; ++i)
	{
		spi_xchg(value);
	}

	_xmem_deselect(bank);
}

/* Checkerboard Test */
static uint8_t xram_memtest(void)
{
	uint8_t w, v;
	uint32_t i;

	stream_fputs(0, "Writing...\n");

	/* Write */
	SPI_OUT &= ~(1 << SPI_CS_0);
	spi_xchg(SRAM_COMMAND_WRITE);
	spi_xchg(0);
	spi_xchg(0);
	spi_xchg(0);

	v = 0xAA;
	for(i = 0; i < 0x1FFFFUL; ++i)
	{
		spi_xchg(v);
		v = ~v;
	}

	SPI_OUT |= (1 << SPI_CS_0);

	stream_fputs(0, "Reading...\n");

	/* Read */
	SPI_OUT &= ~(1 << SPI_CS_0);
	spi_xchg(SRAM_COMMAND_READ);
	spi_xchg(0);
	spi_xchg(0);
	spi_xchg(0);
	v = 0xAA;
	for(i = 0; i < 0x1FFFFUL; ++i)
	{
		w = spi_xchg(0xFF);
		if(v != w)
		{
			SPI_OUT |= (1 << SPI_CS_0);
			return 1;
		}

		v = ~v;
	}

	SPI_OUT |= (1 << SPI_CS_0);
	return 0;
}
