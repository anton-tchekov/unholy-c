#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "types.h"

#include "util.c"
#include "instr.c"
#include "error.c"
#include "xmem.c"
#include "memory.c"
#include "uart.c"
#include "fs.c"
#include "timer.c"
#include "stream.c"
#include "builtin.c"
#include "token.c"
#include "tokenizer.c"
#include "parser.c"
#include "interpreter.c"

static const char _usage_str[] PROGMEM = "Usage: ./nanoc [file]\n";
static const char _fail_open_str[] PROGMEM = "Failed to open file ";

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

	timer_init();
	uart_init();
	xmem_init();
	fs_init();

#if PLATFORM == PLATFORM_LINUX
	if(argc != 2)
	{
		print_str_P(0, _usage_str);
		return 1;
	}

	filename = argv[1];
#elif PLATFORM == PLATFORM_AVR
	/* Load specific file from SD-Card for now */
	/* TODO: Shell, Editor */
	filename = "life.uhc";
#endif

	if(!(file = file_open(filename, "r")))
	{
		print_str_P(0, _fail_open_str);
		print_str(0, filename);
		print_char(0, '\n');
		return 1;
	}

	memory_w8(BANK_INPUT, file_read(file, BANK_INPUT, 0, 0x8000), '\0');
	file_close(file);

	tokenizer_init();
	if((ret = parser_compile()) && ret != -ERROR_FN_UNDEFINED-1)
	{
		char c;
		u16 i, s;
		print_dec(0, _token.Pos.Row);
		print_char(0, ':');
		print_dec(0, _token.Pos.Col);
		print_str(0, ": ");
		print_str(0, error_message(ret));
		print_char(0, '\n');
		print_dec_ext(0, _token.Pos.Row, 5);
		print_str(0, " | ");

		i = 0;
		s = _tokenizer.LineBegin;
		for(; (c = memory_r8(BANK_INPUT, s)) && c != '\n'; ++s, ++i)
		{
			if(i == _token.Pos.Col)
			{
				print_str(0, COLOR_RED);
			}

			if(i == _tokenizer.Pos.Col)
			{
				print_str(0, COLOR_RESET);
			}

			print_char(0, c);
		}

		print_str(0, COLOR_RESET);
		print_char(0, '\n');
		return 1;
	}

	interpreter_init(&i);

#ifdef DEBUG_INTERPRETER
	disasm(&i, _parser.Offset);
#endif

	while(!interpreter_step(&i))
	{
	}

#if PLATFORM == PLATFORM_AVR
	for(;;)
	{
	}
#endif

	return 0;
}
