enum TOKEN_TYPE
{
	TT_NULL,
	TT_NUMBER,
	TT_FN_IDENTIFIER,
	TT_LET_IDENTIFIER,
	TT_KEYWORD,
	TT_IF = TT_KEYWORD,
	TT_ELIF,
	TT_ELSE,
	TT_DO,
	TT_WHILE,
	TT_FOR,
	TT_TO,
	TT_INC,
	TT_DEC,
	TT_LET,
	TT_FN,
	TT_BREAK,
	TT_CONTINUE,
	TT_RETURN,
	TT_LOOP,
	TT_TRUE,
	TT_FALSE,
	TT_CONST,
	TT_SWITCH,
	TT_EXIT
};

static const char _keywords[] PROGMEM =
	"if\0"
	"elif\0"
	"else\0"
	"do\0"
	"while\0"
	"for\0"
	"to\0"
	"inc\0"
	"dec\0"
	"let\0"
	"fn\0"
	"break\0"
	"continue\0"
	"return\0"
	"loop\0"
	"true\0"
	"false\0"
	"const\0"
	"switch\0"
	"exit\0"
	"|";

static i8 _keyword_find(const char *name)
{
	i8 ret = _string_find(_keywords, name);
	if(ret < 0)
	{
		return ret;
	}

	return TT_KEYWORD + ret;
}
