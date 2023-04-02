
static uint8_t render_char(int x, int y, char c, Color color)
{
	int width = 0;
	if(c >= 32 && c <= 126)
	{
		int col, row, data;
		c -= 32;
		width = font_default_widths[c];
		for(row = 0; row < FONT_HEIGHT; ++row)
		{
			data = font_default_chars[c * FONT_HEIGHT + row];
			for(col = 0; col < width; ++col)
			{
				if((data >> col) & 1)
				{
					render_pixel(x + col, y + row, color);
				}
			}
		}
	}

	return width;
}

int render_string(int x, int y, char *s, int len, Color color)
{
	char c;
	for(; (c = *s) && len--; ++s)
	{
		x += render_char(x, y, c, color);
	}

	return x;
}

int render_string_width(char *s, int len)
{
	int x = 0;
	char c;
	for(; (c = *s) && --len; ++s)
	{
		if(c >= 32 && c <= 126)
		{
			x += font_default_widths[c - 32];
		}
	}

	return x;
}