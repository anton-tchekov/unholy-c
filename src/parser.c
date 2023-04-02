/* --- ADDRESS STACK ---*/
/* for `break;` and `continue;` statements */
typedef struct ADDRESS_STACK
{
	u8 Top;
	u16 Offset;
} AddressStack;

static void address_stack_update(AddressStack *as, u8 prev, u16 addr)
{
	while(as->Top > prev)
	{
		--as->Top;
		memory_w16(BANK_INTERPRETER,
			OFFSET_CODE + memory_r16(BANK_PARSER, as->Offset + 2 * as->Top), addr);
	}
}

static void address_stack_push(AddressStack *as, u16 addr)
{
	memory_w16(BANK_PARSER, as->Offset + 2 * as->Top, addr);
	++as->Top;
}

/* --- IDENTIFIER MAP --- */
/* for storing variable and function names */
typedef struct IDENTIFIER_MAP
{
	u16 Count, Offset;
} IdentifierMap;

static i16 identifier_map_find(IdentifierMap *map, char *key)
{
	u16 i, offset;
	char c, r, *p;
	for(i = 0; i < map->Count; ++i)
	{
		p = key;
		offset = memory_r16(BANK_PARSER, map->Offset + 2 * i);
		while((c = *p) && (r = memory_r8(BANK_INPUT, OFFSET_INPUT + offset)) == c)
		{
			++p;
			++offset;
		}

		r = memory_r8(BANK_INPUT, OFFSET_INPUT + offset);
		if(c == '\0' && !is_identifier_char(r))
		{
			return i;
		}
	}

	return -1;
}

static i16 _identifier_map_insert(IdentifierMap *map, u16 offset)
{
	memory_w16(BANK_PARSER, map->Offset + 2 * map->Count, offset);
	return map->Count++;
}

static i16 identifier_map_insert(IdentifierMap *map, u16 offset, char *key)
{
	if(identifier_map_find(map, key) >= 0)
	{
		return -1;
	}

	return _identifier_map_insert(map, offset);
}

/* --- EXPECT --- */
/* consume expected token or return error */
#define EXPECT(TYPE, ERROR) \
	do \
	{ \
		if(_token.Type != TYPE) \
		{ \
			TRACE(ERROR); \
		} \
	} while(0)

/* --- PARSER --- */
typedef struct PARSER
{
	u8 NumGlobals, BreakNesting, ContinueNesting;
	u16 UsagesCount, Offset;
	AddressStack BreakStack, ContinueStack;
	IdentifierMap Variables, Functions, Constants;
} Parser;

static Parser _parser;

static StatusCode _parser_statement(void);
static StatusCode _parser_fn_block(u8 local_vars);
static StatusCode _parser_block_inner(void);
static StatusCode _parser_block(void);
static StatusCode _parser_if(void);
static StatusCode _parser_while(void);
static StatusCode _parser_do_while(void);
static StatusCode _parser_for(void);
static StatusCode _parser_switch(void);
static StatusCode _parser_loop(void);
static StatusCode _parser_break(void);
static StatusCode _parser_continue(void);
static StatusCode _parser_return(u8 instr);
static StatusCode _parser_assign(void);
static StatusCode _parser_action(void);
static StatusCode _parser_fn_call(void);
static StatusCode _parser_expression(void);
static StatusCode _parser_fn(void);
static StatusCode _parser_let(u8 *count);
static void _parser_call_main(void);
static StatusCode _parser_check_impl(void);

/* --- FUNCTIONS --- */
static u8 _fn_params_get(u16 idx)
{
	return memory_r8(BANK_PARSER, OFFSET_FUNCTION_PARAMS + idx);
}

static void _fn_params_set(u16 idx, u8 parameters)
{
	memory_w8(BANK_PARSER, OFFSET_FUNCTION_PARAMS + idx, parameters);
}

static u16 _fn_addr_get(u16 idx)
{
	return memory_r16(BANK_PARSER, OFFSET_FUNCTION_ADDRS + 2 * idx);
}

static void _fn_addr_set(u16 idx, u16 addr)
{
	memory_w16(BANK_PARSER, OFFSET_FUNCTION_ADDRS + 2 * idx, addr);
}

static u16 _fn_usage_get(u16 idx)
{
	return memory_r16(BANK_PARSER, OFFSET_FUNCTION_USAGES + 2 * idx);
}

static void _fn_usage_set(u16 idx, u16 addr)
{
	memory_w16(BANK_PARSER, OFFSET_FUNCTION_USAGES + 2 * idx, addr);
}

static void _fn_usage_add(void)
{
	_fn_usage_set(_parser.UsagesCount++, _parser.Offset);
}

/* --- EMIT --- */
static void _emit8(u8 v)
{
	memory_w8(BANK_INTERPRETER, OFFSET_CODE + _parser.Offset, v);
	_parser.Offset += 1;
}

static void _emit16(u16 v)
{
	memory_w16(BANK_INTERPRETER, OFFSET_CODE + _parser.Offset, v);
	_parser.Offset += 2;
}

static void _emit32(u32 v)
{
	memory_w32(BANK_INTERPRETER, OFFSET_CODE + _parser.Offset, v);
	_parser.Offset += 4;
}

static void _skip(u8 bytes)
{
	_parser.Offset += bytes;
}

static void _pushi(u32 value)
{
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
}

/* --- PARSER --- */
static i8 parser_compile(void)
{
	_parser.Variables.Offset = OFFSET_VARIABLES;
	_parser.Functions.Offset = OFFSET_FUNCTIONS;

	_parser.BreakStack.Offset = OFFSET_BREAK_STACK;
	_parser.ContinueStack.Offset = OFFSET_CONTINUE_STACK;

	RETURN_IF(tokenizer_next());
	while(_token.Type == TT_LET)
	{
		u8 ignore;
		RETURN_IF(_parser_let(&ignore));
		RETURN_IF(tokenizer_next());
	}

	_parser.NumGlobals = _parser.Variables.Count;
	if(_parser.NumGlobals)
	{
		_emit8(INSTR_DSP);
		_emit8(_parser.NumGlobals);
	}

	_parser_call_main();

	while(_token.Type != TT_NULL)
	{
		EXPECT(TT_FN, ERROR_EXPECTED_FN);
		RETURN_IF(_parser_fn());
		RETURN_IF(tokenizer_next());
	}

	RETURN_IF(_parser_check_impl());
	return SUCCESS;
}

#define STR_MAIN_ADDR 0xFFF8

static void _parser_call_main(void)
{
	static char _main_str[] = "main";
	i16 i;

	xmem_write(BANK_INPUT, STR_MAIN_ADDR, _main_str, sizeof(_main_str));
	i = _identifier_map_insert(&_parser.Functions, STR_MAIN_ADDR);
	_fn_addr_set(i, 0);
	_fn_params_set(i, 0);

	_emit8(INSTR_CALL);
	_emit8(0);
	_fn_usage_add();
	_emit16(i);
	_emit8(INSTR_HALT);
}

static const char _undefined_reference_str[] PROGMEM =
	"Undefined reference to `";

static const char _undefined_reference_end_str[] = "`\n";

static StatusCode _parser_check_impl(void)
{
	u16 i;
	u8 fail;

	fail = 0;
	for(i = 0; i < _parser.Functions.Count; ++i)
	{
		if(!_fn_addr_get(i))
		{
			char c;
			u16 addr;

			addr = memory_r16(BANK_PARSER, _parser.Functions.Offset + 2 * i);
			stream_fputs_P(0, _undefined_reference_str);
			while(is_identifier_char(c = memory_r8(BANK_INPUT, addr++)))
			{
				stream_fputc(0, c);
			}

			stream_fputs(0, _undefined_reference_end_str);
			fail = 1;
		}
	}

	if(fail)
	{
		TRACE(ERROR_FN_UNDEFINED);
	}

	return SUCCESS;
}

static StatusCode _parser_fn(void)
{
#ifdef DEBUG_PARSER
	printf("---------------------------------------------------------------------\n"
		"\t\t\t\t\t\tPARSER FN\n");
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
		if(_fn_addr_get(i))
		{
			TRACE(ERROR_FN_REDEFINITION);
		}
	}

	_fn_addr_set(i, _parser.Offset);

	parameters = 0;
	RETURN_IF(tokenizer_next());
	EXPECT('(', ERROR_EXPECTED_L_PAREN);
	RETURN_IF(tokenizer_next());
	while(_token.Type != ')')
	{
		EXPECT(TT_LET_IDENTIFIER, ERROR_EXPECTED_IDENTIFIER);
		if(identifier_map_insert(&_parser.Variables, _token.Number, _token.Identifier) < 0)
		{
			TRACE(ERROR_VAR_REDEFINITION);
		}

		++parameters;
		RETURN_IF(tokenizer_next());
		if(_token.Type == ',')
		{
			RETURN_IF(tokenizer_next());
			EXPECT(TT_LET_IDENTIFIER, ERROR_EXPECTED_IDENTIFIER);
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
		if(parameters != _fn_params_get(i))
		{
			TRACE(ERROR_FN_NUM_ARGS);
		}

		for(j = 0; j < _parser.UsagesCount; ++j)
		{
			u16 usage = _fn_usage_get(j);
			if(memory_r16(BANK_INTERPRETER, usage) == i)
			{
				memory_w16(BANK_INTERPRETER, usage, _fn_addr_get(i));
			}
		}
	}
	else
	{
		_fn_params_set(i, parameters);
	}

	RETURN_IF(_parser_fn_block(parameters));

	/* No return statement */
	if(memory_r8(BANK_INTERPRETER, OFFSET_CODE + _parser.Offset - 1) != INSTR_RET)
	{
		_emit8(INSTR_PUSHI8);
		_emit8(0);
		_emit8(INSTR_RET);
	}

	return SUCCESS;
}

static StatusCode _parser_statement(void)
{
#ifdef DEBUG_PARSER
	printf("\t\t\t\t\t\tPARSER STATEMENT\n");
#endif

	switch(_token.Type)
	{
	case TT_NULL:
		TRACE(ERROR_UNEXPECTED_EOF);

	case TT_FN_IDENTIFIER:
		RETURN_IF(_parser_action());
		break;

	case TT_LET_IDENTIFIER:
		RETURN_IF(_parser_assign());
		break;

	case TT_IF:
		RETURN_IF(_parser_if());
		break;

	case TT_WHILE:
		RETURN_IF(_parser_while());
		break;

	case TT_FOR:
		RETURN_IF(_parser_for());
		break;

	case TT_SWITCH:
		RETURN_IF(_parser_switch());
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
		RETURN_IF(_parser_return(INSTR_RET));
		break;

	case TT_EXIT:
		RETURN_IF(_parser_return(INSTR_HALT));
		break;

	case '{':
		RETURN_IF(_parser_block());
		break;

	default:
		TRACE(ERROR_UNEXPECTED_TOKEN);
	}

	return SUCCESS;
}

static void _popv(u8 i)
{
	if(i < _parser.NumGlobals)
	{
		_emit8(INSTR_POPG);
		_emit8(i);
	}
	else
	{
		_emit8(INSTR_POPL);
		_emit8(i - _parser.NumGlobals);
	}
}

static void _pushv(u8 i)
{
	if(i < _parser.NumGlobals)
	{
		_emit8(INSTR_PUSHG);
		_emit8(i);
	}
	else
	{
		_emit8(INSTR_PUSHL);
		_emit8(i - _parser.NumGlobals);
	}
}

static StatusCode _parser_assign(void)
{
#ifdef DEBUG_PARSER
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
	_popv(i);
	EXPECT(';', ERROR_EXPECTED_SEMICOLON);
	return SUCCESS;
}

static StatusCode _parser_action(void)
{
#ifdef DEBUG_PARSER
	printf("\t\t\t\t\t\tPARSER ACTION\n");
#endif

	RETURN_IF(_parser_fn_call());

	/* Ignored return value */
	_emit8(INSTR_POP);
	RETURN_IF(tokenizer_next());
	return SUCCESS;
}

static StatusCode _parser_fn_call(void)
{
#ifdef DEBUG_PARSER
	printf("\t\t\t\t\t\tPARSER FN CALL\n");
#endif

	i16 i, addr;
	i8 args, parameters;
	u8 impl;

	impl = 1;
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
		parameters = _fn_params_get(i);
		if(!(addr = _fn_addr_get(i)))
		{
			/* Not implemented yet */
			impl = 0;
			addr = i;
		}
	}
	else
	{
		/* New function */
		i = _identifier_map_insert(&_parser.Functions, _token.Number);
		_fn_addr_set(i, 0);
		impl = 0;
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
	if(parameters >= 0)
	{
		if(args != parameters)
		{
			TRACE(ERROR_FN_NUM_ARGS);
		}
	}
	else
	{
		_fn_params_set(i, args);
	}

	/* Call function */
	_emit8(INSTR_CALL);
	_emit8(args);
	if(!impl)
	{
		_fn_usage_add();
	}

	_emit16(addr);
	return SUCCESS;
}

static StatusCode _parser_let(u8 *count)
{
	u8 i;

	i = 0;
	do
	{
		RETURN_IF(tokenizer_next());
		EXPECT(TT_LET_IDENTIFIER, ERROR_EXPECTED_IDENTIFIER);
		if(identifier_map_insert(&_parser.Variables, _token.Number, _token.Identifier) < 0)
		{
			TRACE(ERROR_VAR_REDEFINITION);
		}

		++i;
		RETURN_IF(tokenizer_next());
	}
	while(_token.Type == ',');
	EXPECT(';', ERROR_EXPECTED_SEMICOLON);

	*count = i;

	return SUCCESS;
}

static StatusCode _parser_fn_block(u8 local_vars)
{
#ifdef DEBUG_PARSER
	printf("\t\t\t\t\t\tPARSER FN BLOCK\n");
#endif

	RETURN_IF(tokenizer_next());
	while(_token.Type == TT_LET)
	{
		u8 count;
		RETURN_IF(_parser_let(&count));
		local_vars += count;
		RETURN_IF(tokenizer_next());
	}

	if(local_vars)
	{
		_emit8(INSTR_DSP);
		_emit8(local_vars);
	}

	RETURN_IF(_parser_block_inner());
	_parser.Variables.Count -= local_vars;
	return SUCCESS;
}

static StatusCode _parser_block_inner(void)
{
	while(_token.Type != '}')
	{
		RETURN_IF(_parser_statement());
		RETURN_IF(tokenizer_next());
	}

	return SUCCESS;
}

static StatusCode _parser_block(void)
{
#ifdef DEBUG_PARSER
	printf("\t\t\t\t\t\tPARSER BLOCK\n");
#endif

	EXPECT('{', ERROR_EXPECTED_L_BRACE);
	RETURN_IF(tokenizer_next());
	RETURN_IF(_parser_block_inner());
	return SUCCESS;
}

static StatusCode _parser_expression(void)
{
#ifdef DEBUG_PARSER
	printf("\t\t\t\t\t\tPARSER EXPRESSION\n");
#endif

	if(_token.Type == TT_NUMBER)
	{
		_pushi(_token.Number);
		RETURN_IF(tokenizer_next());
	}
	else if(_token.Type == TT_LET_IDENTIFIER)
	{
		i16 i;
		if((i = identifier_map_find(&_parser.Variables, _token.Identifier)) < 0)
		{
			TRACE(ERROR_UNDEFINED_IDENTIFIER);
		}

		_pushv(i);
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

	return SUCCESS;
}

static StatusCode _parser_if(void)
{
#ifdef DEBUG_PARSER
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
		memory_w16(BANK_INTERPRETER, OFFSET_CODE + idx_cond, _parser.Offset);

		if(_token.Type == TT_ELIF)
		{
			RETURN_IF(_parser_if());
		}
		else
		{
			RETURN_IF(tokenizer_next());
			RETURN_IF(_parser_block());
		}

		memory_w16(BANK_INTERPRETER, OFFSET_CODE + idx_end, _parser.Offset);
	}
	else
	{
		/* Reset state */
		_tokenizer = state;
		memory_w16(BANK_INTERPRETER, OFFSET_CODE + idx_cond, _parser.Offset);
	}

	return SUCCESS;
}

static StatusCode _parser_while(void)
{
#ifdef DEBUG_PARSER
	printf("\t\t\t\t\t\tPARSER WHILE\n");
#endif

	u16 idx_before, idx_branch;
	u8 prev_break, prev_continue;

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

	++_parser.BreakNesting;
	++_parser.ContinueNesting;
	RETURN_IF(_parser_block());
	--_parser.BreakNesting;
	--_parser.ContinueNesting;

	/* Jump back to loop condition */
	_emit8(INSTR_JMP);
	_emit16(idx_before);

	/* Fill in exit address */
	memory_w16(BANK_INTERPRETER, OFFSET_CODE + idx_branch, _parser.Offset);

	/* Handle break and continue statements */
	address_stack_update(&_parser.BreakStack, prev_break, _parser.Offset);
	address_stack_update(&_parser.ContinueStack, prev_continue, idx_before);
	return SUCCESS;
}

static StatusCode _parser_loop(void)
{
#ifdef DEBUG_PARSER
	printf("\t\t\t\t\t\tPARSER LOOP\n");
#endif

	u16 idx_before;
	u8 prev_break, prev_continue;

	idx_before = _parser.Offset;

	/* Loop body */
	prev_break = _parser.BreakStack.Top;
	prev_continue = _parser.ContinueStack.Top;

	++_parser.BreakNesting;
	++_parser.ContinueNesting;
	RETURN_IF(tokenizer_next());
	RETURN_IF(_parser_block());
	--_parser.BreakNesting;
	--_parser.ContinueNesting;

	/* Jump back to loop condition */
	_emit8(INSTR_JMP);
	_emit16(idx_before);

	/* Handle break and continue statements */
	address_stack_update(&_parser.BreakStack, prev_break, _parser.Offset);
	address_stack_update(&_parser.ContinueStack, prev_continue, idx_before);
	return SUCCESS;
}

static StatusCode _parser_for(void)
{
#ifdef DEBUG_PARSER
	printf("\t\t\t\t\t\tPARSER FOR\n");
#endif

	i16 i;
	u16 idx_cmp, idx_before, idx_branch;
	u8 prev_break, prev_continue, type;
	u32 chg;

	/* Parse condition */
	RETURN_IF(tokenizer_next());

	if((i = identifier_map_find(&_parser.Variables, _token.Identifier)) < 0)
	{
		TRACE(ERROR_UNDEFINED_IDENTIFIER);
	}

	RETURN_IF(tokenizer_next());
	EXPECT('=', ERROR_EXPECTED_ASSIGN);
	RETURN_IF(tokenizer_next());
	RETURN_IF(_parser_expression());
	_popv(i);
	EXPECT(TT_TO, ERROR_UNEXPECTED_TOKEN);
	RETURN_IF(tokenizer_next());

	idx_before = _parser.Offset;
	_pushv(i);
	RETURN_IF(_parser_expression());
	_emit8(INSTR_CALL);
	_emit8(2);
	idx_cmp = _parser.Offset;
	_skip(2);

	chg = 1;
	type = TT_INC;
	if(_token.Type == TT_INC || _token.Type == TT_DEC)
	{
		type = _token.Type;
		RETURN_IF(tokenizer_next());
		EXPECT(TT_NUMBER, ERROR_EXPECTED_CONSTANT);
		chg = _token.Number;
		RETURN_IF(tokenizer_next());
	}

	/* Conditional jump to exit loop */
	_emit8(INSTR_JZ);
	idx_branch = _parser.Offset;
	_skip(2);

	/* Loop body */
	prev_break = _parser.BreakStack.Top;
	prev_continue = _parser.ContinueStack.Top;

	++_parser.BreakNesting;
	++_parser.ContinueNesting;
	RETURN_IF(_parser_block());
	--_parser.BreakNesting;
	--_parser.ContinueNesting;

	/* Increment */
	_pushv(i);
	_pushi(chg);
	_emit8(INSTR_CALL);
	_emit8(2);
	_emit16(type == TT_INC ? -1 : -2); /* add / sub */
	_popv(i);

	memory_w16(BANK_INTERPRETER, OFFSET_CODE + idx_cmp, type == TT_INC ? -18 : -19); /* le / ge */

	/* Jump back to loop condition */
	_emit8(INSTR_JMP);
	_emit16(idx_before);

	/* Fill in exit address */
	memory_w16(BANK_INTERPRETER, OFFSET_CODE + idx_branch, _parser.Offset);

	/* Handle break and continue statements */
	address_stack_update(&_parser.BreakStack, prev_break, _parser.Offset);
	address_stack_update(&_parser.ContinueStack, prev_continue, idx_before);
	return SUCCESS;
}

static StatusCode _parser_switch(void)
{
	u8 i, count, prev_break;
	u16 idx_skip, jt[64];

	RETURN_IF(tokenizer_next());
	RETURN_IF(_parser_expression());

	/* jump to jump table */
	_emit8(INSTR_JMP);
	idx_skip = _parser.Offset;
	_skip(2);
	EXPECT('[', ERROR_EXPECTED_L_BRACKET);

	prev_break = _parser.BreakStack.Top;

	count = 0;
	++_parser.BreakNesting;
	do
	{
		jt[count++] = _parser.Offset;
		RETURN_IF(tokenizer_next());
		EXPECT('{', ERROR_EXPECTED_L_BRACE);
		RETURN_IF(_parser_block());
		RETURN_IF(tokenizer_next());
		if(_token.Type == ',')
		{
			/* break */
			_emit8(INSTR_JMP);
			address_stack_push(&_parser.BreakStack, _parser.Offset);
			_skip(2);
		}
		else if(_token.Type == '=')
		{
			/* fallthrough */
			RETURN_IF(tokenizer_next());
			EXPECT('>', ERROR_UNEXPECTED_TOKEN);
		}
	} while(_token.Type != ']');
	--_parser.BreakNesting;

	/* jump over jump table */
	_emit8(INSTR_JMP);
	address_stack_push(&_parser.BreakStack, _parser.Offset);
	_skip(2);

	/* generate jump table */
	memory_w16(BANK_INTERPRETER, OFFSET_CODE + idx_skip, _parser.Offset);
	_emit8(INSTR_JT);
	_emit8(count);
	for(i = 0; i < count; ++i)
	{
		_emit16(jt[i]);
	}

	/* handle break */
	address_stack_update(&_parser.BreakStack, prev_break, _parser.Offset);
	return SUCCESS;
}

static StatusCode _parser_do_while(void)
{
#ifdef DEBUG_PARSER
	printf("\t\t\t\t\t\tPARSER DO WHILE\n");
#endif

	u16 idx_begin, idx_branch;
	u8 prev_break, prev_continue;

	idx_begin = _parser.Offset;

	/* Loop body */
	RETURN_IF(tokenizer_next());
	prev_break = _parser.BreakStack.Top;
	prev_continue = _parser.ContinueStack.Top;

	++_parser.BreakNesting;
	++_parser.ContinueNesting;
	RETURN_IF(_parser_block());
	--_parser.BreakNesting;
	--_parser.ContinueNesting;

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
	return SUCCESS;
}

static StatusCode _parser_break(void)
{
#ifdef DEBUG_PARSER
	printf("\t\t\t\t\t\tPARSER BREAK\n");
#endif

	RETURN_IF(tokenizer_next());
	EXPECT(';', ERROR_EXPECTED_SEMICOLON);
	if(!_parser.BreakNesting)
	{
		TRACE(ERROR_INV_BREAK);
	}

	_emit8(INSTR_JMP);
	address_stack_push(&_parser.BreakStack, _parser.Offset);
	_skip(2);
	return SUCCESS;
}

static StatusCode _parser_continue(void)
{
#ifdef DEBUG_PARSER
	printf("\t\t\t\t\t\tPARSER CONTINUE\n");
#endif

	RETURN_IF(tokenizer_next());
	EXPECT(';', ERROR_EXPECTED_SEMICOLON);
	if(!_parser.ContinueNesting)
	{
		TRACE(ERROR_INV_CONTINUE);
	}

	_emit8(INSTR_JMP);
	address_stack_push(&_parser.ContinueStack, _parser.Offset);
	_skip(2);
	return SUCCESS;
}

static StatusCode _parser_return(u8 instr)
{
#ifdef DEBUG_PARSER
	printf("\t\t\t\t\t\tPARSER RETURN\n");
#endif

	bool cond;

	cond = false;

	RETURN_IF(tokenizer_next());
	if(_token.Type == TT_IF)
	{
		cond = true;
		RETURN_IF(tokenizer_next());
	}

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

	if(cond)
	{
		_emit8(INSTR_DUP);
		_emit8(INSTR_JZ);
		_emit16(_parser.Offset + 3);
	}

	_emit8(instr);
	return SUCCESS;
}
