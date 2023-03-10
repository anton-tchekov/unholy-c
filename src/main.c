#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "types.h"

#include "util.c"
#include "instr.c"
#include "error.c"
#include "memory.c"
#include "stream.c"
#include "builtin.c"
#include "token.c"
#include "tokenizer.c"
#include "parser.c"
#include "interpreter.c"

static const char *_usage_str PROGMEM = "Usage: ./nanoc [file]\n";
static const char *_fail_open_str PROGMEM = "Failed to open file ";

#if PLATFORM == PLATFORM_LINUX
int main(int argc, char **argv)
#elif PLATFORM == PLATFORM_AVR
int main(void)
#endif
{
	u8 file;
	i8 ret;
	Interpreter i;
	const char *filename;

#if PLATFORM == PLATFORM_LINUX
	if(argc != 2)
	{
		stream_str_P(0, _usage_str);
		return 1;
	}

	filename = argv[1];
#elif PLATFORM == PLATFORM_AVR
	/* Load specific file from SD-Card for now */
	/* TODO: Shell, Editor */
	filename = "life.uhc";
#endif

	printf("here!\n");

	if(!(file = file_open(filename, "r")))
	{
		stream_str_P(0, _fail_open_str);
		stream_str(0, filename);
		stream_chr(0, '\n');
		return 1;
	}

	printf("here!\n");

	file_read(file, BANK_INPUT, 0, 0x8000);
	file_close(file);

	printf("here!\n");

	tokenizer_init();
	ret = parser_compile();
	if(ret)
	{
		char c;
		u16 i, s;
		stream_dec(0, _token.Pos.Row);
		stream_chr(0, ':');
		stream_dec(0, _token.Pos.Col);
		stream_str(0, ": ");
		stream_str(0, error_message(ret));
		stream_chr(0, '\n');
		stream_dec_ext(0, 5, _token.Pos.Row);
		stream_str(0, " | ");

		i = 0;
		s = _tokenizer.LineBegin;
		for(; (c = memory_r8(BANK_INPUT, s)) != '\n'; ++s, ++i)
		{
			if(i == _tokenizer.Pos.Col)
			{
				stream_str(0, COLOR_RED);
			}

			if(i == _tokenizer.Pos.Col)
			{
				stream_str(0, COLOR_RESET);
			}

			stream_chr(0, c);
		}

		stream_str(0, COLOR_RESET);
		stream_chr(0, '\n');
		return 1;
	}

	interpreter_init(&i);

#ifdef DEBUG_INTERPRETER
	disasm(&i, _parser.Offset);
#endif

	while(!interpreter_step(&i))
	{
	}

#if PLATFORM == PLATFORM_LINUX
	for(;;)
	{
	}
#endif

	return 0;
}
