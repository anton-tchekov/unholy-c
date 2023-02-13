#define OFFSET_CALL_MAIN 2

typedef struct ADDRESS_STACK
{
	u8 Top, Size;
	u32 Offset;
} AddressStack;

static void address_stack_update(AddressStack *as, u16 prev, u16 addr)
{
	while(as->Top > prev)
	{
		--as->Top;
		memory_w16(OFFSET_CODE + memory_r16(as->Offset + 2 * as->Top), addr);
	}
}

static void address_stack_push(AddressStack *as, u16 addr)
{
	memory_w16(as->Offset + 2 * as->Top, addr);
	++as->Top;
}

typedef struct IDENTIFIER_MAP
{
	u32 Offset;
	u16 Count;
} IdentifierMap;

static i16 identifier_map_find(IdentifierMap *map, char *key)
{
	u16 i, offset;
	char c, r, *p;
	for(i = 0; i < map->Count; ++i)
	{
		p = key;
		offset = memory_r16(map->Offset + 2 * i);
		while((c = *p) && (r = memory_r8(OFFSET_INPUT + offset)) == c)
		{
			++p;
			++offset;
		}

		r = memory_r8(OFFSET_INPUT + offset);
		if(c == '\0' && !is_identifier_char(r))
		{
			return i;
		}
	}

	return -1;
}

static i16 identifier_map_insert(IdentifierMap *map, u16 offset, char *key)
{
	if(identifier_map_find(map, key) >= 0)
	{
		TRACE(ERROR_DUP_MAP_ELEM);
	}

	memory_w16(map->Offset + 2 * map->Count, offset);
	return map->Count++;
}

static void _map_init(IdentifierMap *map, u32 offset)
{
	map->Count = 0;
	map->Offset = offset;
}


#define MAX_FUNCTIONS 256

typedef struct PARSER
{
	u16 UsagesCount;
	u16 Offset;
	u8 LoopNesting;
	AddressStack BreakStack, ContinueStack;
	IdentifierMap Variables, Functions;

	u8 FunctionParams[MAX_FUNCTIONS];
	u16 FunctionAddrs[MAX_FUNCTIONS];
	u16 FunctionUsages[MAX_FUNCTIONS];
} Parser;

static Parser _parser;

static i8 _parser_statement(void);
static i8 _parser_block(u8 local_vars);
static i8 _parser_if(void);
static i8 _parser_while(void);
static i8 _parser_do_while(void);
static i8 _parser_break(void);
static i8 _parser_continue(void);
static i8 _parser_return(void);
static i8 _parser_assign(void);
static i8 _parser_action(void);
static i8 _parser_fn_call(void);
static i8 _parser_expression(void);
static i8 _parser_fn(void);
static void _parser_call_main(void);
static i8 _parser_check_impl(void);
static i8 _parser_main(void);

static void _emit8(u8 v)
{
	memory_w8(OFFSET_CODE + _parser.Offset, v);
	_parser.Offset += 1;
}

static void _emit16(u16 v)
{
	memory_w16(OFFSET_CODE + _parser.Offset, v);
	_parser.Offset += 2;
}

static void _emit32(u32 v)
{
	memory_w32(OFFSET_CODE + _parser.Offset, v);
	_parser.Offset += 4;
}

static void _skip(u8 bytes)
{
	_parser.Offset += bytes;
}

static i8 parser_compile(void)
{
	printf("\t\t\t\t\t\tPARSER COMPILE\n");

	_map_init(&_parser.Variables, OFFSET_VARIABLES);
	_map_init(&_parser.Functions, OFFSET_FUNCTIONS);

	_parser_call_main();

	return_if(tokenizer_next());
	while(_token.Type != TT_NULL)
	{
		if(_token.Type == TT_FN)
		{
			return_if(_parser_fn());
			return_if(tokenizer_next());
		}
		else
		{
			TRACE(ERROR_EXPECTED_FN);
		}
	}

	return_if(_parser_check_impl());
	return_if(_parser_main());
	return 0;
}

static void _parser_call_main(void)
{
	_emit8(INSTR_CALL);
	_emit8(0);
	_skip(2);
	_emit8(INSTR_HALT);
}

static i8 _parser_main(void)
{
	i16 i;
	if((i = identifier_map_find(&_parser.Functions, "main")) < 0)
	{
		printf("Undefined reference to `main`\n");
		TRACE(ERROR_FN_UNDEFINED);
	}

	memory_w16(OFFSET_CODE + OFFSET_CALL_MAIN, _parser.FunctionAddrs[i]);
	return 0;
}

static i8 _parser_check_impl(void)
{
	u16 i;
	u8 fail;

	fail = 0;
	for(i = 0; i < _parser.Functions.Count; ++i)
	{
		if(_parser.FunctionAddrs[i] == 0)
		{
			/*printf("Undefined reference to `%.*s`\n",
				_parser.Functions.Elements[i].Length,
				_parser.Functions.Elements[i].Key);*/

			fail = 1;
		}
	}

	if(fail)
	{
		TRACE(ERROR_FN_UNDEFINED);
	}

	return 0;
}

static i8 _parser_fn(void)
{
	printf("---------------------------------------------------------------------\n");
	printf("\t\t\t\t\t\tPARSER FN\n");

	u16 j;
	i16 i;
	u8 used, args;

	used = 0;

	return_if(tokenizer_next());
	if(_token.Type != TT_FN_IDENTIFIER)
	{
		TRACE(ERROR_EXPECTED_IDENTIFIER);
	}

	printf("%s\n", _token.Identifier);

	if((i = identifier_map_insert(&_parser.Functions, _token.Number, _token.Identifier)) < 0)
	{
		if(i == -ERROR_DUP_MAP_ELEM)
		{
			i = identifier_map_find(&_parser.Functions, _token.Identifier);
			if(_parser.FunctionAddrs[i])
			{
				TRACE(ERROR_FN_REDEFINITION);
			}

			used = 1;
		}
	}

	return_if(tokenizer_next());
	if(_token.Type != '(')
	{
		TRACE(ERROR_EXPECTED_L_PAREN);
	}

	_parser.FunctionAddrs[i] = _parser.Offset;

	return_if(tokenizer_next());

	args = 0;
	while(_token.Type != ')')
	{
		if(_token.Type != TT_VAR_IDENTIFIER)
		{
			TRACE(ERROR_UNEXPECTED_TOKEN);
		}

		return_if(identifier_map_insert(&_parser.Variables, _token.Number, _token.Identifier));
		++args;

		return_if(tokenizer_next());
		if(_token.Type == ',')
		{
			return_if(tokenizer_next());
		}
	}

	return_if(tokenizer_next());
	if(_token.Type != '{')
	{
		TRACE(ERROR_EXPECTED_L_BRACE);
	}

	if(used)
	{
		if(args != _parser.FunctionParams[i])
		{
			TRACE(ERROR_FN_NUM_ARGS);
		}

		for(j = 0; j < _parser.UsagesCount; ++j)
		{
			if(memory_r16(_parser.FunctionUsages[j] + 2) == i)
			{
				memory_w16(_parser.FunctionUsages[j] + 2, _parser.FunctionAddrs[i]);
			}
		}
	}
	else
	{
		_parser.FunctionParams[i] = args;
	}

	return_if(_parser_block(args));
	return 0;
}

static i8 _parser_statement(void)
{
	printf("\t\t\t\t\t\tPARSER STATEMENT\n");

	switch(_token.Type)
	{
	case TT_NULL:
		TRACE(ERROR_UNEXPECTED_EOF);

	case TT_FN_IDENTIFIER:
		return_if(_parser_action());
		break;

	case TT_VAR_IDENTIFIER:
		return_if(_parser_assign());
		break;

	case TT_IF:
		return_if(_parser_if());
		break;

	case TT_WHILE:
		return_if(_parser_while());
		break;

	case TT_DO:
		return_if(_parser_do_while());
		break;

	case TT_BREAK:
		return_if(_parser_break());
		break;

	case TT_CONTINUE:
		return_if(_parser_continue());
		break;

	case TT_RETURN:
		return_if(_parser_return());
		break;

	case '{':
		return_if(_parser_block(0));
		break;

	default:
		TRACE(ERROR_UNEXPECTED_TOKEN);
	}

	return 0;
}

static i8 _parser_assign(void)
{
	printf("\t\t\t\t\t\tPARSER ASSIGN\n");

	i16 i;
	if((i = identifier_map_find(&_parser.Variables, _token.Identifier)) < 0)
	{
		TRACE(ERROR_UNDEFINED_IDENTIFIER);
	}

	return_if(tokenizer_next());
	return_if(tokenizer_next());
	return_if(_parser_expression());
	if(_token.Type != ';')
	{
		TRACE(ERROR_EXPECTED_SEMICOLON);
	}

	_emit8(INSTR_POPL);
	_emit8(i);
	return 0;
}

static i8 _parser_action(void)
{
	printf("\t\t\t\t\t\tPARSER ACTION\n");

	return_if(_parser_fn_call());

	/* Ignored return value */
	_emit8(INSTR_POP);
	return_if(tokenizer_next());
	return 0;
}

static i8 _parser_fn_call(void)
{
	printf("\t\t\t\t\t\tPARSER FN CALL\n");

	i16 i, addr;
	u8 args, parameters;

	args = 0;
	addr = 0;

	if((i = _builtin_find(_token.Identifier)) >= 0)
	{
		/* Builtin Function */
		addr = -i - 1;
		parameters = _builtin_num_parameters(i);
	}
	else if((i = identifier_map_find(&_parser.Functions, _token.Identifier)) >= 0)
	{
		/* Already defined function */
		addr = _parser.FunctionAddrs[i];
		parameters = _parser.FunctionParams[i];
	}
	else
	{
		/* New function */
		return_if(i = identifier_map_insert(&_parser.Functions, _token.Number, _token.Identifier));
		_parser.FunctionAddrs[i] = 0;
		_parser.FunctionParams[i] = args;
		_parser.FunctionUsages[_parser.UsagesCount++] = _parser.Offset;
	}

	return_if(tokenizer_next());
	if(_token.Type != '(')
	{
		TRACE(ERROR_EXPECTED_L_PAREN);
	}

	return_if(tokenizer_next());
	while(_token.Type != ')')
	{
		return_if(_parser_expression());
		++args;
		if(_token.Type == ',')
		{
			return_if(tokenizer_next());
		}
	}

	if(addr)
	{
		/* Check number of arguments */
		if(args != parameters)
		{
			TRACE(ERROR_FN_NUM_ARGS);
		}
	}

	/* Call function */
	_emit8(INSTR_CALL);
	_emit8(args);
	_emit16(addr);
	return 0;
}

static i8 _parser_block(u8 local_vars)
{
	printf("\t\t\t\t\t\tPARSER BLOCK\n");

	return_if(tokenizer_next());
	while(_token.Type == TT_VAR)
	{
		do
		{
			return_if(tokenizer_next());
			if(_token.Type != TT_VAR_IDENTIFIER)
			{
				TRACE(ERROR_EXPECTED_IDENTIFIER);
			}

			return_if(identifier_map_insert(&_parser.Variables, _token.Number, _token.Identifier));
			++local_vars;
			return_if(tokenizer_next());
		}
		while(_token.Type == ',');

		if(_token.Type != ';')
		{
			TRACE(ERROR_EXPECTED_SEMICOLON);
		}

		return_if(tokenizer_next());
	}

	if(local_vars)
	{
		_emit8(INSTR_DSP);
		_emit8(local_vars);
	}

	while(_token.Type != '}')
	{
		return_if(_parser_statement());
		return_if(tokenizer_next());
	}

	_parser.Variables.Count -= local_vars;
	return 0;
}

static i8 _parser_expression(void)
{
	printf("\t\t\t\t\t\tPARSER EXPRESSION\n");

	if(_token.Type == TT_NUMBER)
	{
		u32 value = _token.Number;
		if(value <= 0xFF)
		{
			_emit8(INSTR_PUSHI8);
			_emit8(value);
		}
		else if(value <= 0xFFFF)
		{
			_emit8(INSTR_PUSHI16);
			_emit16(value);
		}
		else
		{
			_emit8(INSTR_PUSHI32);
			_emit32(value);
		}

		return_if(tokenizer_next());
	}
	else if(_token.Type == TT_VAR_IDENTIFIER)
	{
		i16 i;
		if((i = identifier_map_find(&_parser.Variables, _token.Identifier)) < 0)
		{
			TRACE(ERROR_UNDEFINED_IDENTIFIER);
		}

		_emit8(INSTR_PUSHL);
		_emit8(i);
		return_if(tokenizer_next());
	}
	else if(_token.Type == TT_FN_IDENTIFIER)
	{
		return_if(_parser_fn_call());
		return_if(tokenizer_next());
	}
	else
	{
		TRACE(ERROR_UNEXPECTED_TOKEN);
	}

	return 0;
}

static i8 _parser_if(void)
{
	printf("\t\t\t\t\t\tPARSER IF\n");

	Tokenizer state;
	u16 idx_cond, idx_end;

	/* Parse condition */
	return_if(tokenizer_next());
	return_if(_parser_expression());

	/* Conditional jump to skip if */
	_emit8(INSTR_JZ);
	idx_cond = _parser.Offset;
	_skip(2);

	/* Parse if body */
	if(_token.Type != '{')
	{
		TRACE(ERROR_EXPECTED_L_BRACE);
	}

	return_if(_parser_block(0));

	/* Save tokenizer state */
	state = _tokenizer;
	return_if(tokenizer_next());

	/* Is there an else or elif */
	if(_token.Type == TT_ELIF || _token.Type == TT_ELSE)
	{
		/* Jump to end of elif-else block */
		_emit8(INSTR_JMP);
		idx_end = _parser.Offset;
		_skip(2);

		/* Jump to else/elif block if false */
		memory_w16(OFFSET_CODE + idx_cond, _parser.Offset);

		if(_token.Type == TT_ELIF)
		{
			return_if(_parser_if());
		}
		else
		{
			return_if(tokenizer_next());
			if(_token.Type != '{')
			{
				TRACE(ERROR_EXPECTED_L_BRACE);
			}

			return_if(_parser_block(0));
			memory_w16(OFFSET_CODE + idx_end, _parser.Offset);
		}
	}
	else
	{
		/* Reset state */
		_tokenizer = state;
		memory_w16(OFFSET_CODE + idx_cond, _parser.Offset);
	}

	return 0;
}

static i8 _parser_while(void)
{
	printf("\t\t\t\t\t\tPARSER WHILE\n");

	u16 idx_before, idx_branch, prev_break, prev_continue;

	idx_before = _parser.Offset;

	/* Parse condition */
	return_if(tokenizer_next());
	return_if(_parser_expression());

	/* Conditional jump to exit loop */
	_emit8(INSTR_JZ);
	idx_branch = _parser.Offset;
	_skip(2);

	/* Loop body */
	if(_token.Type != '{')
	{
		TRACE(ERROR_EXPECTED_L_BRACE);
	}

	prev_break = _parser.BreakStack.Top;
	prev_continue = _parser.ContinueStack.Top;

	++_parser.LoopNesting;
	return_if(_parser_block(0));
	--_parser.LoopNesting;

	/* Jump back to loop condition */
	_emit8(INSTR_JMP);
	_emit16(idx_before);

	/* Fill in exit address */
	memory_w16(OFFSET_CODE + idx_branch, _parser.Offset);

	/* Handle break and continue statements */
	address_stack_update(&_parser.BreakStack, prev_break, _parser.Offset);
	address_stack_update(&_parser.ContinueStack, prev_continue, idx_before);
	return 0;
}

static i8 _parser_do_while(void)
{
	printf("\t\t\t\t\t\tPARSER DO WHILE\n");

	u16 idx_begin, idx_branch, prev_break, prev_continue;

	idx_begin = _parser.Offset;

	/* Loop body */
	return_if(tokenizer_next());
	prev_break = _parser.BreakStack.Top;
	prev_continue = _parser.ContinueStack.Top;

	if(_token.Type != '{')
	{
		TRACE(ERROR_EXPECTED_L_BRACE);
	}

	++_parser.LoopNesting;
	return_if(_parser_block(0));
	--_parser.LoopNesting;

	/* Parse condition */
	return_if(tokenizer_next());
	if(_token.Type != TT_WHILE)
	{
		TRACE(ERROR_EXPECTED_WHILE);
	}

	return_if(tokenizer_next());
	return_if(_parser_expression());
	if(_token.Type != ';')
	{
		TRACE(ERROR_EXPECTED_SEMICOLON);
	}

	/* Conditional jump to loop beginning */
	idx_branch = _parser.Offset;
	_emit8(INSTR_JNZ);
	_emit16(idx_begin);

	/* Handle break and continue statements */
	address_stack_update(&_parser.BreakStack, prev_break, _parser.Offset);
	address_stack_update(&_parser.ContinueStack, prev_continue, idx_branch);
	return 0;
}

static i8 _parser_break(void)
{
	return_if(tokenizer_next());
	if(_token.Type != ';')
	{
		TRACE(ERROR_EXPECTED_SEMICOLON);
	}

	if(!_parser.LoopNesting)
	{
		TRACE(ERROR_INV_BREAK);
	}

	_emit8(INSTR_JMP);
	address_stack_push(&_parser.BreakStack, _parser.Offset);
	_skip(2);
	return 0;
}

static i8 _parser_continue(void)
{
	return_if(tokenizer_next());
	if(_token.Type != ';')
	{
		TRACE(ERROR_EXPECTED_SEMICOLON);
	}

	if(!_parser.LoopNesting)
	{
		TRACE(ERROR_INV_CONTINUE);
	}

	_emit8(INSTR_JMP);
	address_stack_push(&_parser.ContinueStack, _parser.Offset);
	_skip(2);
	return 0;
}

static i8 _parser_return(void)
{
	printf("\t\t\t\t\t\tPARSER RETURN\n");

	return_if(tokenizer_next());
	return_if(_parser_expression());
	if(_token.Type != ';')
	{
		TRACE(ERROR_EXPECTED_SEMICOLON);
	}

	_emit8(INSTR_RET);
	return 0;
}

