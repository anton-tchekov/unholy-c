enum TOKEN_TYPE
{
	TT_NULL,
	TT_NUMBER,
	TT_FN_IDENTIFIER,
	TT_VAR_IDENTIFIER,
	TT_KEYWORD,
	TT_IF = TT_KEYWORD,
	TT_ELIF,
	TT_ELSE,
	TT_DO,
	TT_WHILE,
	TT_FOR,
	TT_FROM,
	TT_TO,
	TT_STEP,
	TT_VAR,
	TT_FN,
	TT_BREAK,
	TT_CONTINUE,
	TT_RETURN,
	TT_LOOP,
	TT_TRUE,
	TT_FALSE,
	TT_CONST,
	TT_SWITCH
};

static const char _keywords[] PROGMEM =
	"if\0"
	"elif\0"
	"else\0"
	"do\0"
	"while\0"
	"for\0"
	"from\0"
	"to\0"
	"step\0"
	"var\0"
	"fn\0"
	"break\0"
	"continue\0"
	"return\0"
	"loop\0"
	"true\0"
	"false\0"
	"const\0"
	"switch\0|";

static i8 _keyword_find(const char *name)
{
	i8 ret = _string_find(_keywords, name);
	if(ret < 0)
	{
		return ret;
	}

	return TT_KEYWORD + ret;
}

#ifdef DEBUG

static const char *_token_type_string(i8 code)
{
	switch(code)
	{
		case TT_NULL:           return "TT_NULL";
		case TT_NUMBER:         return "TT_NUMBER";
		case TT_FN_IDENTIFIER:  return "TT_FN_IDENTIFIER";
		case TT_VAR_IDENTIFIER: return "TT_VAR_IDENTIFIER";
		case TT_IF:             return "TT_IF";
		case TT_ELIF:           return "TT_ELIF";
		case TT_ELSE:           return "TT_ELSE";
		case TT_DO:             return "TT_DO";
		case TT_WHILE:          return "TT_WHILE";
		case TT_FOR:            return "TT_FOR";
		case TT_FROM:           return "TT_FROM";
		case TT_TO:             return "TT_TO";
		case TT_STEP:           return "TT_STEP";
		case TT_VAR:            return "TT_VAR";
		case TT_FN:             return "TT_FN";
		case TT_BREAK:          return "TT_BREAK";
		case TT_CONTINUE:       return "TT_CONTINUE";
		case TT_RETURN:         return "TT_RETURN";
		case TT_LOOP:           return "TT_LOOP";
		case TT_TRUE:           return "TT_TRUE";
		case TT_FALSE:          return "TT_FALSE";
		case TT_CONST:          return "TT_CONST";
	}

	return "TT_UNDEFINED";
}

#endif
