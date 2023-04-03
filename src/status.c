#ifdef DEBUG

#define TRACE(E) \
	do \
	{ \
		StatusCode __ret = (E); \
		fprintf(stderr, "[ Trace ] %s - %s:%d (%s)\n", status_message(__ret), __FILE__, __LINE__, __func__); \
		return __ret; \
	} while(false)

#define RETURN_IF(E) \
	do \
	{ \
		StatusCode __ret; \
		if((__ret = (E))) \
		{ \
			fprintf(stderr, "[ Trace ] %s - %s:%d (%s)\n", status_message(__ret), __FILE__, __LINE__, __func__); \
			return __ret; \
		} \
	} while(false)

#else

#define TRACE(E) \
	do \
	{ \
		return (E); \
	} while(false)

#define RETURN_IF(E) \
	do \
	{ \
		StatusCode __ret; \
		if((__ret = (E))) \
		{ \
			return __ret; \
		} \
	} while(false)

#endif

#define SUCCESS 0

#define FOREACH_ERROR(ERROR) \
	ERROR(SUCCESS) \
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
	ERROR(INV_BREAK) \
	ERROR(INV_CONTINUE) \
	ERROR(FN_NUM_ARGS) \
	ERROR(FN_UNDEFINED) \
	ERROR(FN_REDEFINITION) \
	ERROR(VAR_REDEFINITION) \
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

typedef enum STATUS_CODE
{
	FOREACH_ERROR(GENERATE_ENUM)
} StatusCode;

static const char *status_message(StatusCode code)
{
	static const char *const _err_msgs[] PROGMEM =
	{
		FOREACH_ERROR(GENERATE_STRING_ARRAY)
	};

	return pgm_read_ptr(_err_msgs + code);
}
