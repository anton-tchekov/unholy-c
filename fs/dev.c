typedef struct BLOCK_DEVICE
{
	i32 BlockSize, NumBlocks;
	StatusCode (*Read)(u32 offset, u32 count, u8 *buffer);
	StatusCode (*Write)(u32 offset, u32 count, u8 *buffer);
} BlockDevice;

static BlockDevice *_devices[26];

static StatusCode dev_read(u8 dev, u32 offset, u32 count, u8 *buffer)
{
	if(!_devices[i])
	{
		return ERROR_FS_NO_DEVICE;
	}

	return _devices[i]->Read(offset, count, buffer);
}

static StatusCode dev_write(u8 dev, u32 offset, u32 count, u8 *buffer)
{
	if(!_devices[i])
	{
		return ERROR_FS_NO_DEVICE;
	}

	return _devices[i]->Write(offset, count, buffer);
}
