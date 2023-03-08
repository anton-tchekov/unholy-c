#include "types.h"

#if PLATFORM == PLATFORM_LINUX

/* 384 KiB, split in 6 banks of 64 KiB */
static u8 _output[3 * 128 * 1024];

static u8 *_calculate_ptr(u8 bank, u16 addr)
{
	return _output + 0x10000 * bank + addr;
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

#elif PLATFORM == PLATFORM_AVR

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
	spi_xchg((u8)(((bank & 1) << 16) & 0xFF));
	spi_xchg((u8)((addr >> 8) & 0xFF));
	spi_xchg((u8)(addr & 0xFF));
}

static void _xmem_read(u8 bank, u16 addr, void *data, u16 size)
{
	u16 i;
	u8 *data8;
	_xmem_select();
	data8 = (u8 *)data;
	spi_xchg(SRAM_COMMAND_READ);
	_xmem_addr(bank, addr);
	for(i = 0; i < size; ++i)
	{
		data8[i] = spi_xchg(0xFF);
	}

	_xmem_deselect();
}

static void _xmem_write(u8 bank, u16 addr, void *data, u16 size)
{
	u16 i;
	u8 *data8;
	_xmem_select();
	data8 = (u8 *)data;
	spi_xchg(SRAM_COMMAND_WRITE);
	_xmem_addr(bank, addr);
	for(i = 0; i < size; ++i)
	{
		spi_xchg(data8[i]);
	}

	_xmem_deselect();
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
	_xmem_write(bank, addr, &ret, 4);
}

static void memory_w16(u32 addr, u16 val)
{
	_xmem_write(bank, addr, &ret, 4);
}

static void memory_w32(u32 addr, u32 val)
{
	_xmem_write(bank, addr, &ret, 4);
}

#endif

