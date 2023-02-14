#define OFFSET_CALL_MAIN 2

typedef struct ADDRESS_STACK
{
	u8 Top;
	u32 Offset;
} AddressStack;

static void address_stack_update(AddressStack *as, u8 prev, u16 addr)
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

static i16 _identifier_map_insert(IdentifierMap *map, u16 offset)
{
	memory_w16(map->Offset + 2 * map->Count, offset);
	return map->Count++;
}

static i16 identifier_map_insert(IdentifierMap *map, u16 offset, char *key)
{
	if(identifier_map_find(map, key) >= 0)
	{
		TRACE(ERROR_DUP_MAP_ELEM);
	}

	return _identifier_map_insert(map, offset);
}

#define MAX_FUNCTIONS 256

typedef struct PARSER
{
	u16 UsagesCount;
	u16 Offset;
	u8 LoopNesting;
	AddressStack BreakStack, ContinueStack;
	IdentifierMap Variables, Functions, Constants;

	u8 FunctionParams[MAX_FUNCTIONS];
	u16 FunctionAddrs[MAX_FUNCTIONS];
	u16 FunctionUsages[MAX_FUNCTIONS];
} Parser;

static Parser _parser;

static i8 _parser_statement(void);
static i8 _parser_fn_block(u8 local_vars);
static i8 _parser_block_inner(void);
static i8 _parser_block(void);
static i8 _parser_if(void);
static i8 _parser_while(void);
static i8 _parser_do_while(void);
static i8 _parser_loop(void);
static i8 _parser_break(void);
static i8 _parser_continue(void);
static i8 _parser_return(void);
static i8 _parser_assign(void);
static i8 _parser_action(void);
static i8 _parser_fn_call(void);
static i8 _parser_expression(void);
static i8 _parser_fn(void);
static i8 _parser_const(void);
static void _parser_call_main(void);
static i8 _parser_check_impl(void);
static i8 _parser_main(void);

#define EXPECT(TYPE, ERROR) \
	do \
	{ \
		if(_token.Type != TYPE) \
		{ \
			TRACE(ERROR); \
		} \
	} while(0)

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
	_parser.Variables.Offset = OFFSET_VARIABLES;
	_parser.Functions.Offset = OFFSET_FUNCTIONS;
	_parser.Constants.Offset = OFFSET_CONSTANTS;

	_parser.BreakStack.Offset = OFFSET_BREAK_STACK;
	_parser.ContinueStack.Offset = OFFSET_CONTINUE_STACK;

	_parser_call_main();

	RETURN_IF(tokenizer_next());

	while(_token.Type != TT_FN)
	{
		EXPECT(TT_CONST, ERROR_EXPECTED_CONST);
		RETURN_IF(_parser_const());
		RETURN_IF(tokenizer_next());
	}

	while(_token.Type != TT_NULL)
	{
		EXPECT(TT_FN, ERROR_EXPECTED_FN);
		RETURN_IF(_parser_fn());
		RETURN_IF(tokenizer_next());
	}

	RETURN_IF(_parser_check_impl());
	RETURN_IF(_parser_main());
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

static i8 _parser_const(void)
{
	RETURN_IF(tokenizer_next());
	RETURN_IF(identifier_map_insert(&_parser.Constants, _token.Number, _token.Identifier));
	RETURN_IF(tokenizer_next());
	//EXPECT('=', ERROR_EXPECTED_)

	return 0;
}

static i8 _parser_fn(void)
{
#ifdef DEBUG
	printf("---------------------------------------------------------------------\n");
	printf("\t\t\t\t\t\tPARSER FN\n");
#endif

	i16 i;
	u8 used, parameters;

	RETURN_IF(tokenizer_next());
	EXPECT(TT_FN_IDENTIFIER, ERROR_EXPECTED_IDENTIFIER);

	if((i = identifier_map_find(&_parser.Functions, _token.Identifier)) < 0)
	{
		used = 0;
		i = _identifier_map_insert(&_parser.Functions, _token.Number);
	}
	else
	{
		used = 1;
		if(_parser.FunctionAddrs[i])
		{
			TRACE(ERROR_FN_REDEFINITION);
		}
	}

	_parser.FunctionAddrs[i] = _parser.Offset;

	parameters = 0;
	RETURN_IF(tokenizer_next());
	EXPECT('(', ERROR_EXPECTED_L_PAREN);
	RETURN_IF(tokenizer_next());
	while(_token.Type != ')')
	{
		EXPECT(TT_VAR_IDENTIFIER, ERROR_EXPECTED_IDENTIFIER);
		RETURN_IF(identifier_map_insert(&_parser.Variables, _token.Number, _token.Identifier));
		++parameters;
		RETURN_IF(tokenizer_next());
		if(_token.Type == ',')
		{
			RETURN_IF(tokenizer_next());
			EXPECT(TT_VAR_IDENTIFIER, ERROR_EXPECTED_IDENTIFIER);
		}
		else if(_token.Type != ')')
		{
			TRACE(ERROR_UNEXPECTED_TOKEN);
		}
	}

	RETURN_IF(tokenizer_next());
	EXPECT('{', ERROR_EXPECTED_L_BRACE);
	if(used)
	{
		u16 j;
		if(parameters != _parser.FunctionParams[i])
		{
			TRACE(ERROR_FN_NUM_ARGS);
		}

		for(j = 0; j < _parser.UsagesCount; ++j)
		{
			if(memory_r16(_parser.FunctionUsages[j]) == i)
			{
				memory_w16(_parser.FunctionUsages[j], _parser.FunctionAddrs[i]);
			}
		}
	}
	else
	{
		_parser.FunctionParams[i] = parameters;
	}

	RETURN_IF(_parser_fn_block(parameters));

	/* No return statement */
	if(memory_r8(OFFSET_CODE + _parser.Offset - 1) != INSTR_RET)
	{
		_emit8(INSTR_PUSHI8);
		_emit8(0);
		_emit8(INSTR_RET);
	}

	return 0;
}

static i8 _parser_statement(void)
{
#ifdef DEBUG
	printf("\t\t\t\t\t\tPARSER STATEMENT\n");
#endif

	switch(_token.Type)
	{
	case TT_NULL:
		TRACE(ERROR_UNEXPECTED_EOF);

	case TT_FN_IDENTIFIER:
		RETURN_IF(_parser_action());
		break;

	case TT_VAR_IDENTIFIER:
		RETURN_IF(_parser_assign());
		break;

	case TT_IF:
		RETURN_IF(_parser_if());
		break;

	case TT_WHILE:
		RETURN_IF(_parser_while());
		break;

	case TT_DO:
		RETURN_IF(_parser_do_while());
		break;

	case TT_LOOP:
		RETURN_IF(_parser_loop());
		break;

	case TT_BREAK:
		RETURN_IF(_parser_break());
		break;

	case TT_CONTINUE:
		RETURN_IF(_parser_continue());
		break;

	case TT_RETURN:
		RETURN_IF(_parser_return());
		break;

	case '{':
		RETURN_IF(_parser_block());
		break;

	default:
		TRACE(ERROR_UNEXPECTED_TOKEN);
	}

	return 0;
}

static i8 _parser_assign(void)
{
#ifdef DEBUG
	printf("\t\t\t\t\t\tPARSER ASSIGN\n");
#endif

	i16 i;
	if((i = identifier_map_find(&_parser.Variables, _token.Identifier)) < 0)
	{
		TRACE(ERROR_UNDEFINED_IDENTIFIER);
	}

	RETURN_IF(tokenizer_next());
	EXPECT('=', ERROR_EXPECTED_ASSIGN);
	RETURN_IF(tokenizer_next());
	RETURN_IF(_parser_expression());
	EXPECT(';', ERROR_EXPECTED_SEMICOLON);
	_emit8(INSTR_POPL);
	_emit8(i);
	return 0;
}

static void _add_fn_usage(void)
{
	_parser.FunctionUsages[_parser.UsagesCount++] = _parser.Offset + 2;
}

static i8 _parser_action(void)
{
#ifdef DEBUG
	printf("\t\t\t\t\t\tPARSER ACTION\n");
#endif

	RETURN_IF(_parser_fn_call());

	/* Ignored return value */
	_emit8(INSTR_POP);
	RETURN_IF(tokenizer_next());
	return 0;
}

static i8 _parser_fn_call(void)
{
#ifdef DEBUG
	printf("\t\t\t\t\t\tPARSER FN CALL\n");
#endif

	i16 i, addr;
	i8 args, parameters;

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
		parameters = _parser.FunctionParams[i];
		if(!(addr = _parser.FunctionAddrs[i]))
		{
			/* Not implemented yet */
			_add_fn_usage();
			addr = i;
		}
	}
	else
	{
		/* New function */
		i = _identifier_map_insert(&_parser.Functions, _token.Number);
		_add_fn_usage();
		_parser.FunctionAddrs[i] = 0;
		_parser.FunctionParams[i] = args;
		addr = i;
		parameters = -1;
	}

	RETURN_IF(tokenizer_next());
	EXPECT('(', ERROR_EXPECTED_L_PAREN);
	RETURN_IF(tokenizer_next());
	while(_token.Type != ')')
	{
		RETURN_IF(_parser_expression());
		++args;
		if(_token.Type == ',')
		{
			RETURN_IF(tokenizer_next());
			if(_token.Type == ')')
			{
				TRACE(ERROR_UNEXPECTED_TOKEN);
			}
		}
		else if(_token.Type != ')')
		{
			TRACE(ERROR_UNEXPECTED_TOKEN);
		}
	}

	/* Check number of arguments */
	if(parameters > 0 && args != parameters)
	{
		TRACE(ERROR_FN_NUM_ARGS);
	}

	/* Call function */
	_emit8(INSTR_CALL);
	_emit8(args);
	_emit16(addr);
	return 0;
}

static i8 _parser_fn_block(u8 local_vars)
{
#ifdef DEBUG
	printf("\t\t\t\t\t\tPARSER FN BLOCK\n");
#endif

	RETURN_IF(tokenizer_next());
	while(_token.Type == TT_VAR)
	{
		do
		{
			RETURN_IF(tokenizer_next());
			EXPECT(TT_VAR_IDENTIFIER, ERROR_EXPECTED_IDENTIFIER);
			RETURN_IF(identifier_map_insert(&_parser.Variables, _token.Number, _token.Identifier));
			++local_vars;
			RETURN_IF(tokenizer_next());
		}
		while(_token.Type == ',');
		EXPECT(';', ERROR_EXPECTED_SEMICOLON);
		RETURN_IF(tokenizer_next());
	}

	if(local_vars)
	{
		_emit8(INSTR_DSP);
		_emit8(local_vars);
	}

	RETURN_IF(_parser_block_inner());
	_parser.Variables.Count -= local_vars;
	return 0;
}

static i8 _parser_block_inner(void)
{
	while(_token.Type != '}')
	{
		RETURN_IF(_parser_statement());
		RETURN_IF(tokenizer_next());
	}

	return 0;
}

static i8 _parser_block(void)
{
#ifdef DEBUG
	printf("\t\t\t\t\t\tPARSER BLOCK\n");
#endif

	EXPECT('{', ERROR_EXPECTED_L_BRACE);
	RETURN_IF(tokenizer_next());
	RETURN_IF(_parser_block_inner());
	return 0;
}

static i8 _parser_expression(void)
{
#ifdef DEBUG
	printf("\t\t\t\t\t\tPARSER EXPRESSION\n");
#endif

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

		RETURN_IF(tokenizer_next());
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
		RETURN_IF(tokenizer_next());
	}
	else if(_token.Type == TT_FN_IDENTIFIER)
	{
		RETURN_IF(_parser_fn_call());
		RETURN_IF(tokenizer_next());
	}
	else
	{
		TRACE(ERROR_UNEXPECTED_TOKEN);
	}

	return 0;
}

static i8 _parser_if(void)
{
#ifdef DEBUG
	printf("\t\t\t\t\t\tPARSER IF\n");
#endif

	Tokenizer state;
	u16 idx_cond, idx_end;

	/* Parse condition */
	RETURN_IF(tokenizer_next());
	RETURN_IF(_parser_expression());

	/* Conditional jump to skip if */
	_emit8(INSTR_JZ);
	idx_cond = _parser.Offset;
	_skip(2);

	/* Parse if body */
	RETURN_IF(_parser_block());

	/* Save tokenizer state */
	state = _tokenizer;
	RETURN_IF(tokenizer_next());

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
			RETURN_IF(_parser_if());
		}
		else
		{
			RETURN_IF(tokenizer_next());
			RETURN_IF(_parser_block());
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
#ifdef DEBUG
	printf("\t\t\t\t\t\tPARSER WHILE\n");
#endif

	u16 idx_before, idx_branch, prev_break, prev_continue;

	idx_before = _parser.Offset;

	/* Parse condition */
	RETURN_IF(tokenizer_next());
	RETURN_IF(_parser_expression());

	/* Conditional jump to exit loop */
	_emit8(INSTR_JZ);
	idx_branch = _parser.Offset;
	_skip(2);

	/* Loop body */
	prev_break = _parser.BreakStack.Top;
	prev_continue = _parser.ContinueStack.Top;

	++_parser.LoopNesting;
	RETURN_IF(_parser_block());
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

static i8 _parser_loop(void)
{
#ifdef DEBUG
	printf("\t\t\t\t\t\tPARSER LOOP\n");
#endif

	u16 idx_before, prev_break, prev_continue;

	idx_before = _parser.Offset;

	/* Loop body */
	prev_break = _parser.BreakStack.Top;
	prev_continue = _parser.ContinueStack.Top;

	++_parser.LoopNesting;
	RETURN_IF(tokenizer_next());
	RETURN_IF(_parser_block());
	--_parser.LoopNesting;

	/* Jump back to loop condition */
	_emit8(INSTR_JMP);
	_emit16(idx_before);

	/* Handle break and continue statements */
	address_stack_update(&_parser.BreakStack, prev_break, _parser.Offset);
	address_stack_update(&_parser.ContinueStack, prev_continue, idx_before);
	return 0;
}


static i8 _parser_do_while(void)
{
#ifdef DEBUG
	printf("\t\t\t\t\t\tPARSER DO WHILE\n");
#endif

	u16 idx_begin, idx_branch;
	u8 prev_break, prev_continue;

	idx_begin = _parser.Offset;

	/* Loop body */
	RETURN_IF(tokenizer_next());
	prev_break = _parser.BreakStack.Top;
	prev_continue = _parser.ContinueStack.Top;

	++_parser.LoopNesting;
	RETURN_IF(_parser_block());
	--_parser.LoopNesting;

	/* Parse condition */
	RETURN_IF(tokenizer_next());
	EXPECT(TT_WHILE, ERROR_EXPECTED_WHILE);
	RETURN_IF(tokenizer_next());
	RETURN_IF(_parser_expression());
	EXPECT(';', ERROR_EXPECTED_SEMICOLON);

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
#ifdef DEBUG
	printf("\t\t\t\t\t\tPARSER BREAK\n");
#endif

	RETURN_IF(tokenizer_next());
	EXPECT(';', ERROR_EXPECTED_SEMICOLON);
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
#ifdef DEBUG
	printf("\t\t\t\t\t\tPARSER CONTINUE\n");
#endif

	RETURN_IF(tokenizer_next());
	EXPECT(';', ERROR_EXPECTED_SEMICOLON);
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
#ifdef DEBUG
	printf("\t\t\t\t\t\tPARSER RETURN\n");
#endif

	RETURN_IF(tokenizer_next());
	if(_token.Type == ';')
	{
		_emit8(INSTR_PUSHI8);
		_emit8(0);
	}
	else
	{
		RETURN_IF(_parser_expression());
		EXPECT(';', ERROR_EXPECTED_SEMICOLON);
	}

	_emit8(INSTR_RET);
	return 0;
}

