#if PLATFORM == PLATFORM_AVR

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

/* PRIVATE */
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

/* PUBLIC */
static void xmem_init(void)
{
	SPI_DIR = (1 << SPI_MOSI) | (1 << SPI_SCK) |
			(1 << SPI_CS_0) | (1 << SPI_CS_1) | (1 << SPI_CS_2);
	SPCR = (1 << SPE) | (1 << MSTR);
	SPSR = (1 << SPI2X);
}

static void xmem_read(u8 bank, u16 addr, void *data, u16 size)
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

static void xmem_write(u8 bank, u16 addr, void *data, u16 size)
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

static void xmem_set(u8 bank, u16 addr, u8 value, u16 size)
{
	u16 i;
	_xmem_select(bank);
	_spi_xchg(SRAM_COMMAND_WRITE);
	_xmem_addr(bank, addr);
	for(i = 0; i < size; ++i)
	{
		_spi_xchg(value);
	}

	_xmem_deselect(bank);
}

#elif PLATFORM == PLATFORM_LINUX

#include <assert.h>

/* 384 KiB, split in 6 banks of 64 KiB */
static u8 _output[3 * 128 * 1024];

/* PRIVATE */
static u8 *_bank_ptr(u8 bank)
{
	return _output + 0x10000 * bank;
}

static u8 *_calculate_ptr(u8 bank, u16 addr)
{
	return _bank_ptr(bank) + addr;
}

/* PUBLIC */
static void xmem_init(void)
{
	/* Do Nothing */
}

static void xmem_read(u8 bank, u16 addr, void *data, u16 size)
{
	memcpy(data, _calculate_ptr(bank, addr), size);
}

static void xmem_write(u8 bank, u16 addr, void *data, u16 size)
{
	memcpy(_calculate_ptr(bank, addr), data, size);
}

static void xmem_set(u8 bank, u16 addr, u8 value, u16 size)
{
	memset(_calculate_ptr(bank, addr), value, size);
}

#endif
