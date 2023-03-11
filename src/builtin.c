/* INTEGER */
static u32 _add(u32 *p)
{
	return p[0] + p[1];
}

static u32 _sub(u32 *p)
{
	return p[0] - p[1];
}

static u32 _mul(u32 *p)
{
	return p[0] * p[1];
}

static u32 _div(u32 *p)
{
	return p[0] / p[1];
}

static u32 _mod(u32 *p)
{
	return p[0] % p[1];
}

static u32 _abs(u32 *p)
{
	return (i32)p[0] < 0 ? -p[0] : p[0];
}

static u32 _and(u32 *p)
{
	return p[0] & p[1];
}

static u32 _or(u32 *p)
{
	return p[0] | p[1];
}

static u32 _xor(u32 *p)
{
	return p[0] ^ p[1];
}

static u32 _inv(u32 *p)
{
	return ~p[0];
}

static u32 _shl(u32 *p)
{
	return p[0] << p[1];
}

static u32 _shr(u32 *p)
{
	return p[0] >> p[1];
}

static u32 _not(u32 *p)
{
	return !p[0];
}

static u32 _eq(u32 *p)
{
	return p[0] == p[1];
}

static u32 _ne(u32 *p)
{
	return p[0] != p[1];
}

static u32 _lt(u32 *p)
{
	return (i32)p[0] < (i32)p[1];
}

static u32 _gt(u32 *p)
{
	return (i32)p[0] > (i32)p[1];
}

static u32 _le(u32 *p)
{
	return (i32)p[0] <= (i32)p[1];
}

static u32 _ge(u32 *p)
{
	return (i32)p[0] >= (i32)p[1];
}

static u32 _hi(u32 *p)
{
	return p[0] > p[1];
}

static u32 _lo(u32 *p)
{
	return p[0] < p[1];
}

static u32 _hs(u32 *p)
{
	return p[0] >= p[1];
}

static u32 _ls(u32 *p)
{
	return p[0] <= p[1];
}

/* FLOAT */
#ifdef ENABLE_FLOAT

static u32 _itf(u32 *p)
{
	return fbti((f32)p[0]);
}

static u32 _fti(u32 *p)
{
	return (u32)ibtf(p[0]);
}

static u32 _addf(u32 *p)
{
	return fbti(ibtf(p[0]) + ibtf(p[1]));
}

static u32 _subf(u32 *p)
{
	return fbti(ibtf(p[0]) - ibtf(p[1]));
}

static u32 _mulf(u32 *p)
{
	return fbti(ibtf(p[0]) * ibtf(p[1]));
}

static u32 _divf(u32 *p)
{
	return fbti(ibtf(p[0]) / ibtf(p[1]));
}

static u32 _modf(u32 *p)
{
	return fbti(fmod(ibtf(p[0]), ibtf(p[1])));
}

static u32 _ltf(u32 *p)
{
	return ibtf(p[0]) < ibtf(p[1]);
}

static u32 _gtf(u32 *p)
{
	return ibtf(p[0]) > ibtf(p[1]);
}

static u32 _lef(u32 *p)
{
	return ibtf(p[0]) <= ibtf(p[1]);
}

static u32 _gef(u32 *p)
{
	return ibtf(p[0]) >= ibtf(p[1]);
}

static u32 _absf(u32 *p)
{
	return fbti(fabs(ibtf(p[0])));
}

static u32 _sin(u32 *p)
{
	return fbti(sin(ibtf(p[0])));
}

static u32 _cos(u32 *p)
{
	return fbti(cos(ibtf(p[0])));
}

static u32 _tan(u32 *p)
{
	return fbti(tan(ibtf(p[0])));
}

static u32 _asin(u32 *p)
{
	return fbti(asin(ibtf(p[0])));
}

static u32 _acos(u32 *p)
{
	return fbti(acos(ibtf(p[0])));
}

static u32 _atan(u32 *p)
{
	return fbti(atan(ibtf(p[0])));
}

static u32 _atan2(u32 *p)
{
	return fbti(atan2(ibtf(p[0]), ibtf(p[1])));
}

static u32 _sinh(u32 *p)
{
	return fbti(sinh(ibtf(p[0])));
}

static u32 _cosh(u32 *p)
{
	return fbti(cosh(ibtf(p[0])));
}

static u32 _tanh(u32 *p)
{
	return fbti(tanh(ibtf(p[0])));
}

static u32 _exp(u32 *p)
{
	return fbti(exp(ibtf(p[0])));
}

static u32 _log(u32 *p)
{
	return fbti(log(ibtf(p[0])));
}

static u32 _log10(u32 *p)
{
	return fbti(log10(ibtf(p[0])));
}

static u32 _pow(u32 *p)
{
	return fbti(pow(ibtf(p[0]), ibtf(p[1])));
}

static u32 _sqrt(u32 *p)
{
	return fbti(sqrt(ibtf(p[0])));
}

static u32 _ceil(u32 *p)
{
	return fbti(ceil(ibtf(p[0])));
}

static u32 _floor(u32 *p)
{
	return fbti(floor(ibtf(p[0])));
}

static u32 _round(u32 *p)
{
	return fbti(round(ibtf(p[0])));
}

#endif

#ifdef ENABLE_CHAR

/* CHAR */
static u32 _isupper(u32 *p)
{
	return isupper(p[0]);
}

static u32 _islower(u32 *p)
{
	return islower(p[0]);
}

static u32 _isdigit(u32 *p)
{
	return isdigit(p[0]);
}

static u32 _isalpha(u32 *p)
{
	return isalpha(p[0]);
}

static u32 _isalnum(u32 *p)
{
	return isalnum(p[0]);
}

static u32 _iscntrl(u32 *p)
{
	return iscntrl(p[0]);
}

static u32 _isgraph(u32 *p)
{
	return isgraph(p[0]);
}

static u32 _isprint(u32 *p)
{
	return isprint(p[0]);
}

static u32 _ispunct(u32 *p)
{
	return ispunct(p[0]);
}

static u32 _isspace(u32 *p)
{
	return isspace(p[0]);
}

static u32 _isxdigit(u32 *p)
{
	return isxdigit(p[0]);
}

static u32 _tolower(u32 *p)
{
	return tolower(p[0]);
}

static u32 _toupper(u32 *p)
{
	return toupper(p[0]);
}

#endif

/* MEMORY */
static u32 _w32(u32 *p)
{
	memory_w32(BANK_INTERPRETER, p[0], p[1]);
	return 0;
}

static u32 _w16(u32 *p)
{
	memory_w16(BANK_INTERPRETER, p[0], p[1]);
	return 0;
}

static u32 _w8(u32 *p)
{
	memory_w8(BANK_INTERPRETER, p[0], p[1]);
	return 0;
}

static u32 _r32(u32 *p)
{
	return memory_r32(BANK_INTERPRETER, p[1]);
}

static u32 _ru16(u32 *p)
{
	return memory_r16(BANK_INTERPRETER, p[0]);
}

static u32 _ru8(u32 *p)
{
	return memory_r8(BANK_INTERPRETER, p[0]);
}

/* sign extension! */
static u32 _rs16(u32 *p)
{
	return (i32)((i16)memory_r16(BANK_INTERPRETER, p[0]));
}

static u32 _rs8(u32 *p)
{
	return (i32)((i8)memory_r8(BANK_INTERPRETER, p[0]));
}

/* bulk memory */
static u32 _mcpy(u32 *p)
{
	memory_cpy(BANK_INTERPRETER, p[0], p[1], p[2]);
	return 0;
}

static u32 _mcmp(u32 *p)
{
	return memory_cmp(BANK_INTERPRETER, p[0], p[1], p[2]);
}

static u32 _mchr(u32 *p)
{
	return memory_chr(BANK_INTERPRETER, p[0], p[1], p[2]);
}

static u32 _mset(u32 *p)
{
	xmem_set(BANK_INTERPRETER, p[0], p[1], p[2]);
	return 0;
}

#ifdef ENABLE_RANDOM

/* RANDOM */
static u32 _srand(u32 *p)
{
	srand(p[0]);
	return 0;
}

static u32 _rand(u32 *p)
{
	return rand();
	(void)p;
}

#endif

/* IO */
static u32 _fputs(u32 *p)
{
	stream_fputs_X(p[0], BANK_INTERPRETER, p[1]);
	return 0;
}

static u32 _fputse(u32 *p)
{
	stream_fputse_X(p[0], BANK_INTERPRETER, p[1], p[2]);
	return 0;
}

static u32 _fputd(u32 *p)
{
	stream_fputd(p[0], p[1]);
	return 0;
}

static u32 _fputde(u32 *p)
{
	stream_fputde(p[0], p[1], p[2]);
	return 0;
}

static u32 _fputx(u32 *p)
{
	stream_fputx(p[0], p[1]);
	return 0;
}

static u32 _fputxe(u32 *p)
{
	stream_fputxe(p[0], p[1], p[2]);
	return 0;
}

#ifdef ENABLE_FLOAT

static u32 _fputf(u32 *p)
{
	stream_fputf(p[0], ibtf(p[0]));
	return 0;
}

static u32 _fputfe(u32 *p)
{
	stream_fputfe(p[0], ibtf(p[1]), p[2], p[3]);
	return 0;
}

#endif

static u32 _fputc(u32 *p)
{
	stream_fputc(p[0], p[1]);
	return 0;
}

#ifdef ENABLE_FILE

/* FILE */
static u32 _file_open(u32 *p)
{
	char name_buf[16], mode_buf[4];
	xmem_read(BANK_INTERPRETER, p[0], name_buf, sizeof(name_buf));
	xmem_read(BANK_INTERPRETER, p[1], mode_buf, sizeof(mode_buf));
	return fs_fopen(name_buf, mode_buf);
	return 0;
}

static u32 _file_read(u32 *p)
{
	fs_fread(p[0], BANK_INTERPRETER, p[1], p[2]);
	return 0;
}

static u32 _file_close(u32 *p)
{
	fs_fclose(p[0]);
	return 0;
}

static u32 _file_write(u32 *p)
{
	fs_fwrite(p[0], BANK_INTERPRETER, p[1], p[2]);
	return 0;
}

static u32 _file_seek(u32 *p)
{
	fs_fseek(p[0], p[1]);
	return 0;
}

static u32 _file_tell(u32 *p)
{
	return fs_ftell(p[0]);
}

#endif

static u32 _millis(u32 *p)
{
	return millis();
	(void)p;
}

static const i8 _num_parameters[] PROGMEM =
{
	/* INT */
	2, /* add  */
	2, /* sub  */
	2, /* mul  */
	2, /* div  */
	2, /* mod  */
	2, /* abs  */
	2, /* and  */
	2, /* or   */
	2, /* xor  */
	2, /* inv  */
	2, /* shl  */
	2, /* shr  */
	2, /* not  */
	2, /* eq   */
	2, /* ne   */
	2, /* lt   */
	2, /* gt   */
	2, /* le   */
	2, /* ge   */
	2, /* hi   */
	2, /* lo   */
	2, /* hs   */
	2, /* ls   */

#ifdef ENABLE_FLOAT
	/* FLOAT */
	1, /* itf  */
	1, /* fti  */
	2, /* addf */
	2, /* subf */
	2, /* mulf */
	2, /* divf */
	2, /* modf */
	1, /* absf */
	2, /* ltf  */
	2, /* gtf  */
	2, /* lef  */
	2, /* gef  */

	/* MATH */
	1, /* sin   */
	1, /* cos   */
	1, /* tan   */
	1, /* asin  */
	1, /* acos  */
	1, /* atan  */
	2, /* atan2 */
	1, /* sinh  */
	1, /* cosh  */
	1, /* tanh  */
	1, /* exp   */
	1, /* log   */
	1, /* log10 */
	2, /* pow   */
	1, /* sqrt  */
	1, /* ceil  */
	1, /* floor */
	1, /* round */
#endif

#ifdef ENABLE_CHAR
	/* CHAR */
	1, /* isupper  */
	1, /* islower  */
	1, /* isdigit  */
	1, /* isalpha  */
	1, /* isalnum  */
	1, /* iscntrl  */
	1, /* isgraph  */
	1, /* isprint  */
	1, /* ispunct  */
	1, /* isspace  */
	1, /* isxdigit */
	1, /* tolower  */
	1, /* toupper  */
#endif

	/* MEMORY */
	2, /* w32  */
	2, /* w16 */
	2, /* w8  */
	1, /* r32  */
	1, /* ru16 */
	1, /* ru8  */
	1, /* rs16 */
	1, /* rs8  */
	3, /* mcpy */
	3, /* mcmp */
	3, /* mchr */
	3, /* mset */

#ifdef ENABLE_RANDOM
	/* RANDOM */
	1, /* srand */
	0, /* rand  */
#endif

	/* IO */
	2, /* fputs */
	3, /* fputse */
	2, /* fputd */
	3, /* fputde */
	2, /* fputx */
	3, /* fputxe */

#ifdef ENABLE_FLOAT
	2, /* fputf */
	4, /* fputfe */
#endif

	2, /* fputc */

#ifdef ENABLE_FILE
	/* FILE */
	2, /* fopen  */
	3, /* fread  */
	1, /* fclose */
	3, /* fwrite */
	2, /* fseek  */
	1, /* ftell  */
#endif

	0, /* millis */
};

typedef u32 (*const builtin_ptr)(u32 *);

static builtin_ptr _builtins[] PROGMEM =
{
	/* INT */
	_add,
	_sub,
	_mul,
	_div,
	_mod,
	_abs,
	_and,
	_or,
	_xor,
	_inv,
	_shl,
	_shr,
	_not,
	_eq,
	_ne,
	_lt,
	_gt,
	_le,
	_ge,
	_hi,
	_lo,
	_hs,
	_ls,

#ifdef ENABLE_FLOAT
	/* FLOAT */
	_itf,
	_fti,
	_addf,
	_subf,
	_mulf,
	_divf,
	_modf,
	_absf,
	_ltf,
	_gtf,
	_lef,
	_gef,

	/* MATH */
	_sin,
	_cos,
	_tan,
	_asin,
	_acos,
	_atan,
	_atan2,
	_sinh,
	_cosh,
	_tanh,
	_exp,
	_log,
	_log10,
	_pow,
	_sqrt,
	_ceil,
	_floor,
	_round,
#endif

#ifdef ENABLE_CHAR
	/* CHAR */
	_isupper,
	_islower,
	_isdigit,
	_isalpha,
	_isalnum,
	_iscntrl,
	_isgraph,
	_isprint,
	_ispunct,
	_isspace,
	_isxdigit,
	_tolower,
	_toupper,
#endif

	/* MEMORY */
	_w32,
	_w16,
	_w8,
	_r32,
	_ru16,
	_ru8,
	_rs16,
	_rs8,
	_mcpy,
	_mcmp,
	_mchr,
	_mset,

#ifdef ENABLE_RANDOM
	/* RANDOM */
	_srand,
	_rand,
#endif

	/* IO */
	_fputs,
	_fputse,
	_fputd,
	_fputde,
	_fputx,
	_fputxe,

#ifdef ENABLE_FLOAT
	_fputf,
	_fputfe,
#endif

	_fputc,

#ifdef ENABLE_FILE
	/* FILE */
	_file_open,
	_file_read,
	_file_close,
	_file_write,
	_file_seek,
	_file_tell,
#endif

	_millis,
};

static const char _identifiers[] PROGMEM =

	/* INT */
	"add\0"
	"sub\0"
	"mul\0"
	"div\0"
	"mod\0"
	"abs\0"
	"and\0"
	"or\0"
	"xor\0"
	"inv\0"
	"shl\0"
	"shr\0"
	"not\0"
	"eq\0"
	"ne\0"
	"lt\0"
	"gt\0"
	"le\0"
	"ge\0"
	"hi\0"
	"lo\0"
	"hs\0"
	"ls\0"

#ifdef ENABLE_FLOAT
	/* FLOAT */
	"itf\0"
	"fti\0"
	"addf\0"
	"subf\0"
	"mulf\0"
	"divf\0"
	"modf\0"
	"absf\0"
	"ltf\0"
	"gtf\0"
	"lef\0"
	"gef\0"

	/* MATH */
	"sin\0"
	"cos\0"
	"tan\0"
	"asin\0"
	"acos\0"
	"atan\0"
	"atan2\0"
	"sinh\0"
	"cosh\0"
	"tanh\0"
	"exp\0"
	"log\0"
	"log10\0"
	"pow\0"
	"sqrt\0"
	"ceil\0"
	"floor\0"
	"round\0"
#endif

#ifdef ENABLE_CHAR
	/* CHAR */
	"isupper\0"
	"islower\0"
	"isdigit\0"
	"isalpha\0"
	"isalnum\0"
	"iscntrl\0"
	"isgraph\0"
	"isprint\0"
	"ispunct\0"
	"isspace\0"
	"isxdigit\0"
	"tolower\0"
	"toupper\0"
#endif

	/* MEMORY  */
	"w32\0"
	"w16\0"
	"w8\0"
	"r32\0"
	"ru16\0"
	"ru8\0"
	"rs16\0"
	"rs8\0"

	"mcpy\0"
	"mcmp\0"
	"mchr\0"
	"mset\0"

#ifdef ENABLE_RANDOM
	/* RANDOM */
	"srand\0"
	"rand\0"
#endif

	/* IO */
	"fputs\0"
	"fputse\0"
	"fputd\0"
	"fputde\0"
	"fputx\0"
	"fputxe\0"

#ifdef ENABLE_FLOAT
	"fputf\0"
	"fputfe\0"
#endif

	"fputc\0"

#ifdef ENABLE_FILE
	/* FILE */
	"fopen\0"
	"fread\0"
	"fclose\0"
	"fwrite\0"
	"fseek\0"
	"ftell\0"
#endif

	"millis\0"

	"|";

#ifdef DEBUG_INTERPRETER

static const char *_builtin_name(u32 id)
{
	const char *start;
	u32 count;

	if(id > ARRLEN(_num_parameters))
	{
		return "INVALID";
	}

	start = _identifiers;
	for(count = 0; count < id; ++count)
	{
		start += strlen(start) + 1;
	}

	return start;
}

#endif

static i8 _builtin_find(const char *name)
{
	return _string_find(_identifiers, name);
}

static i8 _builtin_num_parameters(u8 id)
{
	return pgm_read_byte(_num_parameters + id);
}

static u32 _builtin_call(u8 id, u32 *args)
{
	return ((builtin_ptr)pgm_read_ptr(_builtins + id))(args);
}
