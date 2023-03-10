#if PLATFORM == PLATFORM_LINUX

/* 384 KiB, split in 6 banks of 64 KiB */
static u8 _output[3 * 128 * 1024];

static u8 *_bank_ptr(u8 bank)
{
	return _output + 0x10000 * bank;
}

static u8 *_calculate_ptr(u8 bank, u16 addr)
{
	return _bank_ptr(bank) + addr;
}

/* READ ADDRESS */
static u8 memory_r8(u8 bank, u16 addr)
{
	return *_calculate_ptr(bank, addr);
}

static u16 memory_r16(u8 bank, u16 addr)
{
	return _read_16(_calculate_ptr(bank, addr));
}

static u32 memory_r32(u8 bank, u16 addr)
{
	return _read_32(_calculate_ptr(bank, addr));
}

/* WRITE ADDRESS */
static void memory_w8(u8 bank, u16 addr, u8 val)
{
	*_calculate_ptr(bank, addr) = val;
}

static void memory_w16(u8 bank, u16 addr, u16 val)
{
	_write_16(_calculate_ptr(bank, addr), val);
}

static void memory_w32(u8 bank, u16 addr, u32 val)
{
	_write_32(_calculate_ptr(bank, addr), val);
}

/* BULK */
static void memory_mcpy(u8 bank, u16 dest, u16 src, u16 count)
{
	memmove(_calculate_ptr(bank, dest),
		_calculate_ptr(bank, src), count);
}

static u32 memory_mcmp(u8 bank, u16 a, u16 b, u16 count)
{
	return memcmp(_calculate_ptr(bank, a),
		_calculate_ptr(bank, b), count);
}

static u32 memory_mchr(u8 bank, u16 addr, u8 value, u16 count)
{
	return (u8 *)memchr(_calculate_ptr(bank, addr), value, count) - _bank_ptr(bank);
}

static void memory_mset(u8 bank, u16 addr, u8 value, u16 count)
{
	memset(_calculate_ptr(bank, addr), value, count);
}

#elif PLATFORM == PLATFORM_AVR

#define SRAM_COMMAND_WRITE   2
#define SRAM_COMMAND_READ    3

#define SPI_DIR               DDRB
#define SPI_OUT               PORTB
#define SPI_CS_0             0
#define SPI_CS_1             1
#define SPI_CS_2             2
#define SPI_MOSI             3
#define SPI_MISO             4
#define SPI_SCK              5

static void memory_init(void)
{
	SPI_DIR = (1 << SPI_MOSI) | (1 << SPI_SCK) |
			(1 << SPI_CS_0) | (1 << SPI_CS_1) | (1 << SPI_CS_2);
	SPCR = (1 << SPE) | (1 << MSTR);
	SPSR = (1 << SPI2X);
}

static u8 _spi_xchg(u8 data)
{
	SPDR = data;
	while(!(SPSR & (1 << SPIF))) ;
	return SPDR;
}

static u8 _bank_to_pin(u8 bank)
{
	return bank >> 1;
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
	_spi_xchg(bank & 1);
	_spi_xchg(addr >> 8);
	_spi_xchg(addr & 0xFF);
}

static void _xmem_read(u8 bank, u16 addr, void *data, u16 size)
{
	u16 i;
	u8 *data8;
	_xmem_select(bank);
	data8 = (u8 *)data;
	_spi_xchg(SRAM_COMMAND_READ);
	_xmem_addr(bank, addr);
	for(i = 0; i < size; ++i)
	{
		data8[i] = _spi_xchg(0xFF);
	}

	_xmem_deselect(bank);
}

static void _xmem_write(u8 bank, u16 addr, void *data, u16 size)
{
	u16 i;
	u8 *data8;
	_xmem_select(bank);
	data8 = (u8 *)data;
	_spi_xchg(SRAM_COMMAND_WRITE);
	_xmem_addr(bank, addr);
	for(i = 0; i < size; ++i)
	{
		_spi_xchg(data8[i]);
	}

	_xmem_deselect(bank);
}

/* READ ADDRESS */
static u8 memory_r8(u8 bank, u16 addr)
{
	u8 ret;
	_xmem_read(bank, addr, &ret, 1);
	return ret;
}

static u16 memory_r16(u8 bank, u16 addr)
{
	u16 ret;
	_xmem_read(bank, addr, &ret, 2);
	return ret;
}

static u32 memory_r32(u8 bank, u16 addr)
{
	u32 ret;
	_xmem_read(bank, addr, &ret, 4);
	return ret;
}

/* WRITE ADDRESS */
static void memory_w8(u8 bank, u16 addr, u8 val)
{
	_xmem_write(bank, addr, &val, 1);
}

static void memory_w16(u8 bank, u16 addr, u16 val)
{
	_xmem_write(bank, addr, &val, 2);
}

static void memory_w32(u8 bank, u16 addr, u32 val)
{
	_xmem_write(bank, addr, &val, 4);
}

/* BULK */
#define CHUNK_SIZE 256

static void memory_mcpy(u8 bank, u16 dest, u16 src, u16 count)
{
	/* TODO: Implement memmove, Determine overlap direction! */

	/*u8 chunk[CHUNK_SIZE];
	while()
	{
		_xmem_read()
		_xmem_write()
	}*/
}

static u32 memory_mcmp(u8 bank, u16 a, u16 b, u16 count)
{
	/* TODO: Implement memcmp */
	return 0;
}

static u32 memory_mchr(u8 bank, u16 addr, u8 value, u16 count)
{
	/* TODO: Implement memchr */

	/*u16 bytes;
	while(bytes < count)
	{
		_xmem_read(bank, addr, )
		for(i = 0; i < ; ++i)
		{
			if(chunk[i] == value)
			{
				return addr + i;
			}
		}

		addr += bytes;
	}*/
	return 0;
}

static void memory_mset(u8 bank, u16 addr, u8 value, u16 count)
{
	u16 i;
	_xmem_select(bank);
	_spi_xchg(SRAM_COMMAND_WRITE);
	_xmem_addr(bank, addr);
	for(i = 0; i < count; ++i)
	{
		_spi_xchg(value);
	}

	_xmem_deselect(bank);
}

#endif

