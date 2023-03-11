/* READ ADDRESS */
static u8 memory_r8(u8 bank, u16 addr)
{
	u8 ret;
	xmem_read(bank, addr, &ret, 1);
	return ret;
}

static u16 memory_r16(u8 bank, u16 addr)
{
	u16 ret;
	xmem_read(bank, addr, &ret, 2);
	return ret;
}

static u32 memory_r32(u8 bank, u16 addr)
{
	u32 ret;
	xmem_read(bank, addr, &ret, 4);
	return ret;
}

/* WRITE ADDRESS */
static void memory_w8(u8 bank, u16 addr, u8 val)
{
	xmem_write(bank, addr, &val, 1);
}

static void memory_w16(u8 bank, u16 addr, u16 val)
{
	xmem_write(bank, addr, &val, 2);
}

static void memory_w32(u8 bank, u16 addr, u32 val)
{
	xmem_write(bank, addr, &val, 4);
}

/* BULK */
#define CHUNK_SIZE 256

static void memory_cpy(u8 bank, u16 dest, u16 src, u16 count)
{
	/* TODO: Implement memmove, Determine overlap direction! */

	/*u8 chunk[CHUNK_SIZE];
	while()
	{
		_xmem_read()
		_xmem_write()
	}*/
}

static u32 memory_cmp(u8 bank, u16 a, u16 b, u16 count)
{
	/* TODO: Implement memcmp */
	u8 chunk_a[CHUNK_SIZE],
		chunk_b[CHUNK_SIZE];



	return 0;
}

static u32 memory_chr(u8 bank, u16 addr, u8 value, u16 count)
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
