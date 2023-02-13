#include "types.h"

static const char _keywords[] PROGMEM =
	"if\0"
	"elif\0"
	"else\0"
	"do\0"
	"while\0"
	"var\0"
	"fn\0"
	"break\0"
	"continue\0"
	"return\0"
	"loop\0"
	"true\0"
	"false\0|";

static i8 _keyword_find(const char *name)
{
	i8 ret = _string_find(_keywords, name);
	if(ret < 0)
	{
		return ret;
	}

	return TT_KEYWORD + ret;
}

