#include "types.h"

#ifdef DEBUG

#define TRACE(E) \
	do \
	{ \
		i8 __ret = -(E) - 1; \
		fprintf(stderr, "[ Trace ] %s - %s:%d (%s)\n", error_message(__ret), __FILE__, __LINE__, __func__); \
		return __ret; \
	} while(0)

#define RETURN_IF(E) \
	do \
	{ \
		i8 __ret; \
		if((__ret = (E)) < 0) \
		{ \
			fprintf(stderr, "[ Trace ] %s - %s:%d (%s)\n", error_message(__ret), __FILE__, __LINE__, __func__); \
			return __ret; \
		} \
	} while(0)

#else

#define TRACE(E) \
	do \
	{ \
		return -(E) - 1; \
	} while(0)

#define RETURN_IF(E) \
	do \
	{ \
		i8 ret; \
		if((ret = (E)) < 0) \
		{ \
			return ret; \
		} \
	} while(0)

#endif

#define FOREACH_ERROR(ERROR) \
	ERROR(UNEXPECTED_EOF) \
	ERROR(UNEXPECTED_TOKEN) \
	ERROR(EXPECTED_L_BRACE) \
	ERROR(EXPECTED_L_BRACKET) \
	ERROR(EXPECTED_L_PAREN) \
	ERROR(EXPECTED_R_PAREN) \
	ERROR(EXPECTED_SEMICOLON) \
	ERROR(EXPECTED_WHILE) \
	ERROR(EXPECTED_ASSIGN) \
	ERROR(EXPECTED_IDENTIFIER) \
	ERROR(EXPECTED_FN) \
	ERROR(UNDEFINED_IDENTIFIER) \
	ERROR(EXPECTED_DIGIT) \
	ERROR(EXPECTED_CONSTANT) \
	ERROR(DUP_MAP_ELEM) \
	ERROR(INV_BREAK) \
	ERROR(INV_CONTINUE) \
	ERROR(FN_NUM_ARGS) \
	ERROR(FN_UNDEFINED) \
	ERROR(FN_REDEFINITION) \
	ERROR(VAR_UNDEFINED) \
	ERROR(STACK_OVERFLOW) \
	ERROR(STACK_UNDERFLOW) \
	ERROR(INV_INSTR) \
	ERROR(INV_MEM_ACCESS) \
	ERROR(INV_ESCAPE_SEQUENCE) \
	ERROR(UNTERMINATED_CHAR_LITERAL) \
	ERROR(UNEXPECTED_CHARACTER) \
	ERROR(UNTERMINATED_STRING_LITERAL)

#define GENERATE_ENUM(ENUM) ERROR_##ENUM,
#define GENERATE_STRING(STRING) static const char STRING_##STRING[] PROGMEM = #STRING;
#define GENERATE_STRING_ARRAY(STRING) STRING_##STRING,

FOREACH_ERROR(GENERATE_STRING)

enum NANOC_ERROR
{
	FOREACH_ERROR(GENERATE_ENUM)
};

static const char *error_message(i8 code)
{
	static const char *const _err_msgs[] PROGMEM =
	{
		"",
		FOREACH_ERROR(GENERATE_STRING_ARRAY)
	};

	i8 idx = -code;
	if(idx < 0 || idx >= (i8)ARRLEN(_err_msgs))
	{
		idx = 0;
	}

	return pgm_read_ptr(_err_msgs + idx);
}
