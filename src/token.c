#ifndef __TOKEN_TYPE_H__
#define __TOKEN_TYPE_H__

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
	TT_VAR,
	TT_FN,
	TT_BREAK,
	TT_CONTINUE,
	TT_RETURN,
	TT_LOOP,
	TT_TRUE,
	TT_FALSE
};

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
		case TT_VAR:            return "TT_VAR";
		case TT_FN:             return "TT_FN";
		case TT_BREAK:          return "TT_BREAK";
		case TT_CONTINUE:       return "TT_CONTINUE";
		case TT_RETURN:         return "TT_RETURN";
		case TT_LOOP:           return "TT_LOOP";
		case TT_TRUE:           return "TT_TRUE";
		case TT_FALSE:          return "TT_FALSE";
	}

	return "TT_UNDEFINED";
}

#endif

