#include <assert.h>
#include <string.h>

/* 128 KiB, split in 2 banks of 64 KiB */
static u8 _output[2 * 64 * 1024];

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
void xmem_read(u8 bank, u16 addr, void *data, u16 size)
{
	memcpy(data, _calculate_ptr(bank, addr), size);
}

void xmem_write(u8 bank, u16 addr, void *data, u16 size)
{
	memcpy(_calculate_ptr(bank, addr), data, size);
}

/* READ ADDRESS */
u8 memory_r8(u8 bank, u16 addr)
{
	u8 ret;
	xmem_read(bank, addr, &ret, 1);
	return ret;
}

u16 memory_r16(u8 bank, u16 addr)
{
	u16 ret;
	xmem_read(bank, addr, &ret, 2);
	return ret;
}

u32 memory_r32(u8 bank, u16 addr)
{
	u32 ret;
	xmem_read(bank, addr, &ret, 4);
	return ret;
}

/* WRITE ADDRESS */
void memory_w8(u8 bank, u16 addr, u8 val)
{
	xmem_write(bank, addr, &val, 1);
}

void memory_w16(u8 bank, u16 addr, u16 val)
{
	xmem_write(bank, addr, &val, 2);
}

void memory_w32(u8 bank, u16 addr, u32 val)
{
	xmem_write(bank, addr, &val, 4);
}

/* BULK */
#define CHUNK_SIZE 256

void memory_cpy(u8 bank, u16 dest, u16 src, u16 count)
{
	/* Handle overlap (like memmove), if src == dest, do nothing */
	if(dest < src)
	{
		/* Copy direction: Low to high */
		u16 cur;
		u8 chunk[CHUNK_SIZE];

		while(count)
		{
			cur = count > CHUNK_SIZE ? CHUNK_SIZE : count;
			xmem_read(bank, src, chunk, cur);
			xmem_write(bank, dest, chunk, cur);
			src += cur;
			dest += cur;
			count -= cur;
		}
	}
	else if(dest > src)
	{
		/* Copy direction: High to low addresses */
		u16 cur;
		u8 chunk[CHUNK_SIZE];

		src += count;
		dest += count;
		while(count)
		{
			cur = count > CHUNK_SIZE ? CHUNK_SIZE : count;
			src -= cur;
			dest -= cur;
			xmem_read(bank, src, chunk, cur);
			xmem_write(bank, dest, chunk, cur);
			count -= cur;
		}
	}
}

i8 memory_cmp(u8 bank, u16 a, u16 b, u16 count)
{
	i8 ret;
	u16 cur;
	u8 chunk_a[CHUNK_SIZE],
		chunk_b[CHUNK_SIZE];

	while(count)
	{
		cur = count > CHUNK_SIZE ? CHUNK_SIZE : count;
		xmem_read(bank, a, chunk_a, cur);
		xmem_read(bank, b, chunk_b, cur);
		if((ret = memcmp(chunk_a, chunk_b, cur)))
		{
			return ret;
		}

		count -= cur;
		a += cur;
		b += cur;
	}

	return 0;
}

u16 memory_chr(u8 bank, u16 addr, u8 value, u16 count)
{
	u16 cur, ret;
	u8 chunk[CHUNK_SIZE];

	while(count)
	{
		cur = count > CHUNK_SIZE ? CHUNK_SIZE : count;
		xmem_read(bank, addr, chunk, cur);
		if((ret = ((u8 *)memchr(chunk, value, cur) - chunk)))
		{
			return ret;
		}

		count -= cur;
		addr += cur;
	}

	return 0;
}

void memory_set(u8 bank, u16 addr, u8 value, u16 size)
{
	memset(_calculate_ptr(bank, addr), value, size);
}

