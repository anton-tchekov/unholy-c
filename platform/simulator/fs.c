#include <stdio.h>

static FILE *_files[256];

/* --- PRIVATE --- */
static u8 _find_slot(void)
{
	u32 i;
	for(i = 1; i < ARRLEN(_files); ++i)
	{
		if(!_files[i])
		{
			return i;
		}
	}

	return 0;
}

/* --- PUBLIC --- */
u8 fs_fopen(const char *filename, const char *mode)
{
	u8 id = _find_slot();
	return (_files[id] = fopen(filename, mode)) ? id : 0;
}

u16 fs_fread(u8 file, u8 bank, u16 addr, u16 size)
{
	if(file)
	{
		return fread(_calculate_ptr(bank, addr), 1, size, _files[file]);
	}

	return 0;
}

void fs_fclose(u8 file)
{
	fclose(_files[file]);
	_files[file] = 0;
}

void fs_fputc(u8 file, char c)
{
	if(file)
	{
		fputc(c, _files[file]);
	}
}

u16 fs_fwrite(u8 file, u8 bank, u16 addr, u16 size)
{
	if(file)
	{
		return fwrite(_calculate_ptr(bank, addr), 1, size, _files[file]);
	}

	return 0;
}

void fs_fseek(u8 file, u32 pos)
{
	if(file)
	{
		fseek(_files[file], pos, SEEK_SET);
	}
}

u32 fs_ftell(u8 file)
{
	return file ? ftell(_files[file]) : 0;
}
