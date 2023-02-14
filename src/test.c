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
#include "find.c"
#include "builtin.c"
#include "token.c"
#include "keyword.c"
#include "tokenizer.c"
#include "parser.c"
#include "interpreter.c"

u8 code[1024] =
{
	INSTR_PUSHI8,
	42,
	INSTR_CALL,
	6,
	0,
	INSTR_HALT,

	/* square */
	INSTR_DSP,
	1,
	INSTR_PUSHL,
	0,
	INSTR_PUSHL,
	0,
	INSTR_CALL,
	0xFD,
	0xFF,
	INSTR_RET
};

int main(int argc, char **argv)
{
	i8 ret;
	FILE *fp;

	/*memcpy(_output, code, sizeof(code));*/

	if(argc != 2)
	{
		return 1;
	}

	if(!(fp = fopen(argv[1], "r")))
	{
		fprintf(stderr, "Failed to open file \"%s\"\n", argv[1]);
		return 1;
	}

	fread(_output + OFFSET_INPUT, 1, 0x10000, fp);
	fclose(fp);

	tokenizer_init();
	if((ret = parser_compile()))
	{
		printf("parse error: %s\n"
				"row: %d, col: %d\n",
				error_message(ret),
				_token.Pos.Row, _token.Pos.Col);
		return 1;
	}

	Interpreter i;
	interpreter_init(&i, OFFSET_CODE);

	_disasm(&i, _parser.Offset);

	while(!interpreter_step(&i))
	{
	}

	return 0;
}
