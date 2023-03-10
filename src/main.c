#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "types.h"

#include "util.c"
#include "instr.c"
#include "error.c"
#include "stream.c"
#include "memory.c"
#include "builtin.c"
#include "token.c"
#include "tokenizer.c"
#include "parser.c"
#include "interpreter.c"

static const char *_usage_str PROGMEM = "Usage: ./nanoc [file]\n";

#if PLATFORM == PLATFORM_LINUX
int main(int argc, char **argv)
#elif PLATFORM == PLATFORM_AVR
int main(void)
#endif
{
	u8 file;
	i8 ret;
	Interpreter i;

#if PLATFORM == PLATFORM_LINUX
	if(argc != 2)
	{
		stream_str(0, _usage_str);
		return 1;
	}

	if(!(file = stream_open(argv[1], "r")))
	{
		fprintf(stderr, "Failed to open file \"%s\"\n", argv[1]);
		return 1;
	}
#endif

	stream_read(file, BANK_INPUT, 0, 0x10000);
	stream_close(file);

	tokenizer_init();
	ret = parser_compile();
	if(ret)
	{
		char c;
		u16 i, s, len;
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
