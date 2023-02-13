#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "types.h"
#include "readfile.h"
#include "util.c"
#include "error.c"
#include "map.c"
#include "unholy.h"
#include "lexer.c"
#include "parser.c"
#include "interpreter.c"

#define STRINGS_OFFSET (8 * 1024)

#define TEXT_RED "\033[31;1m"
#define TEXT_WHITE "\033[0m"

/* MAIN */
int main(int argc, char **argv)
{
	NanoC nanoc;
	u8 *program, *data;
	int32_t ret;
	int length;
	char *src;
	FILE *fp;

	/* check args */
	if(argc != 2 && argc != 3)
	{
		fprintf(stderr, "usage: ./nanoc filename\n");
		return 1;
	}

	/* load source file */
	if(!(fp = fopen(argv[1], "r")))
	{
		fprintf(stderr, "unable to open file: \"%s\"\n", argv[1]);
		return 1;
	}

	if(!(src = readfile(fp, &length)))
	{
		fprintf(stderr, "error reading file\n");
		return 1;
	}

	fclose(fp);

	/* Init NanoC */
	nanoc_init(&nanoc, _builtins, ARRLEN(_builtins));

	printf("VVV = %d\n", identifier_map_find(&nanoc.Parser.Functions, "andl", 4));
	return;

	/* compile to bytecode */
	if(!(program = calloc(16 * 1024, 1)))
	{
		fprintf(stderr, "error allocating memory for bytecode\n");
		return 1;
	}

	if(!(data = calloc(16 * 1024, 1)))
	{
		fprintf(stderr, "error allocating memory for data\n");
		return 1;
	}

	if((ret = nanoc_compile(&nanoc, src, program, data + STRINGS_OFFSET, STRINGS_OFFSET)))
	{
		int i;
		const char *s;
		printf("parse error: %s\n"
				"row: %d, col: %d\n",
				nanoc_error_message(ret),
				nanoc.Token.Pos.Row, nanoc.Token.Pos.Col);

		for(i = 0, s = nanoc.Lexer.LineBegin; *s != '\n' && s - src < length; ++s, ++i)
		{
			if(i == nanoc.Token.Pos.Col)
			{
				fputs(TEXT_RED, stdout);
			}

			if(i == nanoc.Lexer.Pos.Col)
			{
				fputs(TEXT_WHITE, stdout);
			}

			fputc(*s, stdout);
		}

		fputs(TEXT_WHITE, stdout);
		fputc('\n', stdout);
		return 1;
	}

	/* write output file */
	if(argc == 3)
	{
		u8 header[4];

		u16 program_size, data_size;

		if(!(fp = fopen(argv[2], "w")))
		{
			fprintf(stderr, "unable to open output file for writing: \"%s\"\n", argv[2]);
			return 1;
		}

		program_size = nanoc.Parser.Index;
		data_size = nanoc.Lexer.StringsPtr - nanoc.Lexer.Strings;

		_write_16(header, program_size);
		_write_16(header + 2, data_size);

		fwrite(header, 1, 4, fp);

		fwrite(program, 1, program_size, fp);
		fwrite(data + STRINGS_OFFSET, 1, data_size, fp);

		fclose(fp);
	}

	/* run interpreter */
	nanoc_disasm(program, nanoc.Parser.Index);

	if((ret = nanoc_run(&nanoc, program, data)))
	{
		printf("runtime error: %s\n", nanoc_error_message(ret));
	}

	free(program);
	free(src);
	return 0;
}

