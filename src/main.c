#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "types.h"
#include "platform.h"
#include "terminal.h"

#include "util.c"
#include "instr.c"
#include "status.c"
#include "builtin.c"
#include "token.c"
#include "tokenizer.c"
#include "parser.c"
#include "interpreter.c"

static const char _usage_str[] PROGMEM = "Usage: ./nanoc [file]\n";
static const char _fail_open_str[] PROGMEM = "Failed to open file ";
static const char _memory_test_failed[] PROGMEM = "Memory Test FAILED\n";
static const char _memory_test_passed[] PROGMEM = "Memory Test PASSED\n";

#if PLATFORM == PLATFORM_LINUX
int main(int argc, char **argv)
#elif PLATFORM == PLATFORM_AVR
int main(void)
#endif
{
	u8 file;
	StatusCode ret;
	Interpreter i;
	const char *filename;

	platform_init();

#if PLATFORM == PLATFORM_LINUX
	if(argc != 2)
	{
		stream_fputs_P(0, _usage_str);
		return 1;
	}

	filename = argv[1];
#elif PLATFORM == PLATFORM_AVR
	/* Load specific file from SD-Card for now */
	/* TODO: Shell, Editor */
	filename = "life.uhc";
	sei();

	if(xram_memtest())
	{
		stream_fputs_P(0, _memory_test_failed);
		panic();
	}

	stream_fputs_P(0, _memory_test_passed);
#endif


	if(!(file = fs_fopen(filename, "r")))
	{
		stream_fputs_P(0, _fail_open_str);
		stream_fputs(0, filename);
		stream_fputc(0, '\n');
		return 1;
	}

	memory_w8(BANK_INPUT,
		fs_fread(file, BANK_INPUT, OFFSET_INPUT, 0x8000), '\0');

	fs_fclose(file);

	tokenizer_init();
	ret = parser_compile();
	if(ret && ret != ERROR_FN_UNDEFINED)
	{
		char c;
		u16 i, s;
		stream_fputd(0, _token.Pos.Row);
		stream_fputc(0, ':');
		stream_fputd(0, _token.Pos.Col);
		stream_fputs(0, ": ");
		stream_fputs(0, status_message(ret));
		stream_fputc(0, '\n');
		stream_fputde(0, _token.Pos.Row, 5);
		stream_fputs(0, " | ");

		i = 0;
		s = _tokenizer.LineBegin;
		for(; (c = memory_r8(BANK_INPUT, s)) && c != '\n'; ++s, ++i)
		{
			if(i == _token.Pos.Col)
			{
				stream_fputs(0, COLOR_RED);
			}

			if(i == _tokenizer.Pos.Col)
			{
				stream_fputs(0, COLOR_RESET);
			}

			stream_fputc(0, c);
		}

		stream_fputs(0, COLOR_RESET);
		stream_fputc(0, '\n');
		return 1;
	}

	interpreter_init(&i);

#ifdef DEBUG_INTERPRETER
	disasm(&i, _parser.Offset);
#endif

	while(!interpreter_step(&i))
	{
	}

	graphics_destroy();

#if PLATFORM == PLATFORM_AVR
	for(;;)
	{
	}
#endif

	return 0;
}
