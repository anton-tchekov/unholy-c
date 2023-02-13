#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct POSITION
{
	int32_t Col, Row;
} Position;

typedef struct TOKEN
{
	Position Pos;
	i8 Type;
	u32 Number;
	char Identifier[32];
} Token;

typedef struct TOKENIZER
{
	u32 InputOffset, LineBegin, DataOffset;
	Position Pos;
	char Current;
} Tokenizer;

static Token _token;
static Tokenizer _tokenizer;

static u8 _ascii_hex(i8 c)
{
	return (c & 15) + (c >= 'A' ? 9 : 0);
}

static u8 _is_hex(i8 c)
{
	return isdigit(c) || (c >= 'A' && c <= 'F');
}

static u8 _is_bin(i8 c)
{
	return c == '0' || c == '1';
}

static u8 is_identifier_char(char c)
{
	return isalnum(c) || c == '_';
}

static void _tokenizer_read(void)
{
	_tokenizer.Current = memory_r8(OFFSET_INPUT + _tokenizer.InputOffset);
	if(_tokenizer.Current == '\n')
	{
		++_tokenizer.Pos.Row;
		_tokenizer.Pos.Col = -1;
		_tokenizer.LineBegin = _tokenizer.InputOffset + 1;
	}
}

static char _tokenizer_advance(void)
{
	if(_tokenizer.Current == '\0')
	{
		return '\0';
	}

	++_tokenizer.InputOffset;
	++_tokenizer.Pos.Col;
	_tokenizer_read();
	return _tokenizer.Current;
}

static char _tokenizer_current(void)
{
	return _tokenizer.Current;
}

static void tokenizer_init(void)
{
	_tokenizer.DataOffset = 0;
	_tokenizer.LineBegin = 0;
	_tokenizer.InputOffset = 0;
	_tokenizer.Pos.Col = 0;
	_tokenizer.Pos.Row = 1;
	_tokenizer_read();
}

static i8 _tokenizer_char(u8 *v)
{
	char b, c;
	u8 ret;

	c = _tokenizer_current();
	if(c == '\\')
	{
		c = _tokenizer_advance();
		switch(c)
		{
		case '\\': ret = '\\'; break;
		case '\'': ret = '\''; break;
		case '\"': ret = '\"'; break;
		case '0':  ret = '\0'; break;
		case 'a':  ret = '\a'; break;
		case 'b':  ret = '\b'; break;
		case 'e':  ret = 0x1B; break;
		case 'f':  ret = '\f'; break;
		case 'n':  ret = '\n'; break;
		case 'r':  ret = '\r'; break;
		case 't':  ret = '\t'; break;
		case 'v':  ret = '\v'; break;
		case 'x':
			b = _tokenizer_advance();
			if(!_is_hex(b))
			{
				TRACE(ERROR_UNEXPECTED_CHARACTER);
			}

			c = _tokenizer_advance();
			if(!_is_hex(c))
			{
				TRACE(ERROR_UNEXPECTED_CHARACTER);
			}

			ret = 16 * _ascii_hex(b) + _ascii_hex(c);
			break;

		default:
			TRACE(ERROR_INV_ESCAPE_SEQUENCE);
		}
	}
	else if(c >= 32 && c <= 126)
	{
		ret = c;
	}
	else
	{
		TRACE(ERROR_UNEXPECTED_CHARACTER);
	}

	*v = ret;
	return 0;
}

static void _tokenizer_comment(void)
{
	char c;

	c = _tokenizer_advance();
	if(c == '*')
	{
		/* multi line comment */
		c = _tokenizer_advance();
		while(c)
		{
			c = _tokenizer_advance();
			if(c != '*')
			{
				continue;
			}

			c = _tokenizer_advance();
			if(c == '/')
			{
				break;
			}
		}

		_tokenizer_advance();
		_token.Pos = _tokenizer.Pos;
	}
}

static void _tokenizer_space(void)
{
	while(isspace(_tokenizer_advance())) ;
}

static void _tokenizer_identifier(void)
{
	i8 keyword;
	char c, *p;

	_token.Number = _tokenizer.InputOffset;

	p = _token.Identifier;
	c = _tokenizer_current();

	while(is_identifier_char(c))
	{
		*p++ = c;
		c = _tokenizer_advance();
	}

	*p = '\0';
	if((keyword = _keyword_find(_token.Identifier)) >= 0)
	{
		_token.Type = keyword;
		return;
	}

	if(isspace(c))
	{
		_tokenizer_space();
	}

	_token.Type = (_tokenizer_current() == '(')
			? TT_FN_IDENTIFIER
			: TT_VAR_IDENTIFIER;

	return;
}

static void _tokenizer_hexadecimal(void)
{
	char c;
	u32 n;

	n = 0;
	c = _tokenizer_advance();
	while(_is_hex(c))
	{
		n = 16 * n + _ascii_hex(c);
		c = _tokenizer_advance();
	}

	_token.Number = n;
}

static void _tokenizer_binary(void)
{
	char c;
	u32 n;

	n = 0;
	c = _tokenizer_advance();
	while(_is_bin(c))
	{
		n = 2 * n + (c - '0');
		c = _tokenizer_advance();
	}

	_token.Number = n;
}

static i8 _tokenizer_float(i32 n)
{
	char c;
	f32 f;

	f = n;
	c = _tokenizer_advance();
	if(!isdigit(c))
	{
		TRACE(ERROR_EXPECTED_DIGIT);
	}

	n = 10;
	while(isdigit(c))
	{
		f += (c - '0') / (f32)n;
		n *= 10;
		c = _tokenizer_advance();
	}

	_token.Number = *(u32 *)&f;
	return 0;
}

static i8 _tokenizer_decimal(i8 b)
{
	i32 n;
	char c;
	u8 is_neg;

	c = _tokenizer_current();
	if(b == '-')
	{
		is_neg = 1;
		n = 0;
		if(!isdigit(c))
		{
			TRACE(ERROR_EXPECTED_DIGIT);
		}
	}
	else
	{
		is_neg = 0;
		n = b - '0';
	}

	while(isdigit(c))
	{
		n = 10 * n + (c - '0');
		c = _tokenizer_advance();
	}

	if(c == '.')
	{
		return _tokenizer_float(n);
	}

	_token.Number = is_neg ? -n : n;
	return 0;
}

static i8 _tokenizer_number(void)
{
	char c, b;

	_token.Type = TT_NUMBER;
	b = _tokenizer_current();
	c = _tokenizer_advance();
	if(c == 'x')
	{
		_tokenizer_hexadecimal();
	}
	else if(c == 'b')
	{
		_tokenizer_binary();
	}
	else
	{
		return _tokenizer_decimal(b);
	}

	return 0;
}

static i8 _tokenizer_char_literal(void)
{
	u8 v;
	char c;

	_tokenizer_advance();
	RETURN_IF(_tokenizer_char(&v));
	if(v == '\'')
	{
		TRACE(ERROR_UNEXPECTED_CHARACTER);
	}

	c = _tokenizer_advance();
	if(c != '\'')
	{
		TRACE(ERROR_UNTERMINATED_CHAR_LITERAL);
	}

	_tokenizer_advance();
	_token.Type = TT_NUMBER;
	_token.Number = v;
	return 0;
}

static i8 _tokenizer_string_literal(void)
{
	u8 v;
	u16 offset;

	offset = (OFFSET_DATA - OFFSET_CODE) + _tokenizer.DataOffset;
	do
	{
		while(_tokenizer_advance() != '\"')
		{
			RETURN_IF(_tokenizer_char(&v));
			memory_w8(OFFSET_DATA + _tokenizer.DataOffset, v);
			_tokenizer.DataOffset += 1;
		}

		_tokenizer_space();
	}
	while(_tokenizer_current() == '\"');

	memory_w8(OFFSET_DATA + _tokenizer.DataOffset, '\0');
	_tokenizer.DataOffset += 1;

	_token.Type = TT_NUMBER;
	_token.Number = offset;
	return 0;
}

#ifdef DEBUG

static void _tokenizer_debug(void)
{
	printf("%3d:%3d    ", _token.Pos.Row, _token.Pos.Col);
	if(_token.Type == TT_VAR_IDENTIFIER ||
		_token.Type == TT_FN_IDENTIFIER)
	{
		printf("%s\n", _token.Identifier);
	}
	else if(_token.Type == TT_NUMBER)
	{
		printf("%12d        %10f\n", _token.Number, *(f32 *)&_token.Number);
	}
	else if(_token.Type >= 32)
	{
		printf("'%c'\n", _token.Type);
	}
	else if(_token.Type >= TT_KEYWORD)
	{
		printf("%s\n", _token_type_string(_token.Type));
	}
	else
	{
		printf("\n");
	}
}

#endif

static i8 tokenizer_next(void)
{
	char c;

	_token.Pos = _tokenizer.Pos;
	while((c = _tokenizer_current()))
	{
		if(c == '/')
		{
			_tokenizer_comment();
		}
		else if(isspace(c))
		{
			_tokenizer_space();
			_token.Pos = _tokenizer.Pos;
		}
		else if(c == '\'')
		{
			RETURN_IF(_tokenizer_char_literal());
			_tokenizer_debug();
			return 0;
		}
		else if(c == '\"')
		{
			RETURN_IF(_tokenizer_string_literal());
			_tokenizer_debug();
			return 0;
		}
		else if(isalpha(c) || c == '_')
		{
			_tokenizer_identifier();
			_tokenizer_debug();
			return 0;
		}
		else if(c == '-' || isdigit(c))
		{
			RETURN_IF(_tokenizer_number());
			_tokenizer_debug();
			return 0;
		}
		else
		{
			_token.Type = c;
			_tokenizer_advance();
			_tokenizer_debug();
			return 0;
		}
	}

	_token.Type = TT_NULL;
	return 0;
}

