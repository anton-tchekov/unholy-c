static u32 fbti(f32 p)
{
	return *(u32 *)&p;
}

static f32 ibtf(u32 p)
{
	return *(f32 *)&p;
}

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
	return tolower((int)p[0]);
}

static u32 _toupper(u32 *p)
{
	return toupper((int)p[0]);
}

/* MEMORY */
static u32 _w32(u32 *p)
{
	return 0;
	(void)p;
}

static u32 _wu16(u32 *p)
{
	return 0;
	(void)p;
}

static u32 _wu8(u32 *p)
{
	return 0;
	(void)p;
}

static u32 _ws16(u32 *p)
{
	return 0;
	(void)p;
}

static u32 _ws8(u32 *p)
{
	return 0;
	(void)p;
}

static u32 _r32(u32 *p)
{
	return 0;
	(void)p;
}

static u32 _ru16(u32 *p)
{
	return 0;
	(void)p;
}

static u32 _ru8(u32 *p)
{
	return 0;
	(void)p;
}

static u32 _rs16(u32 *p)
{
	return 0;
	(void)p;
}

static u32 _rs8(u32 *p)
{
	return 0;
	(void)p;
}

static u32 _mcpy(u32 *p)
{
	return 0;
	(void)p;
}

static u32 _mcmp(u32 *p)
{
	return 0;
	(void)p;
}

static u32 _mchr(u32 *p)
{
	return 0;
	(void)p;
}

static u32 _mset(u32 *p)
{
	return 0;
	(void)p;
}

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

/* IO */
static u32 _println(u32 *p)
{
	return printf("%s\n", _output + p[0]);
}

static u32 _print_number(u32 *p)
{
	return printf("PRINT NUMBER: %d\n", p[0]);
}

static const i8 _num_parameters[] PROGMEM =
{
	/* INT */
	2, /* add  */
	2, /* sub  */
	2, /* mul  */
	2, /* div  */
	2, /* and  */
	2, /* mod  */
	2, /* or   */
	2, /* xor  */
	2, /* inv  */
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

	/* MEMORY */
	2, /* w32  */
	2, /* wu16 */
	2, /* wu8  */
	2, /* ws16 */
	2, /* ws8  */
	1, /* r32  */
	1, /* ru16 */
	1, /* ru8  */
	1, /* rs16 */
	1, /* rs8  */
	3, /* mcpy */
	3, /* mcmp */
	3, /* mchr */
	3, /* mset */

	/* RANDOM */
	1, /* srand */
	0, /* rand  */

	/* IO */
	1, /* println */
	1, /* print_number */
};

static u32 (*_builtins[])(u32 *) PROGMEM =
{
	/* INT */
	_add,
	_sub,
	_mul,
	_div,
	_and,
	_mod,
	_or,
	_xor,
	_inv,
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

	/* MEMORY */
	_w32,
	_wu16,
	_wu8,
	_ws16,
	_ws8,
	_r32,
	_ru16,
	_ru8,
	_rs16,
	_rs8,
	_mcpy,
	_mcmp,
	_mchr,
	_mset,

	/* RANDOM */
	_srand,
	_rand,

	/* IO */
	_println,
	_print_number
};

static const char _identifiers[] PROGMEM =

	/* INT */
	"add\0"
	"sub\0"
	"mul\0"
	"div\0"
	"and\0"
	"mod\0"
	"or\0"
	"xor\0"
	"inv\0"
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

	/* MEMORY  */
	"w32\0"
	"wu16\0"
	"wu8\0"
	"ws16\0"
	"ws8\0"
	"r32\0"
	"ru16\0"
	"ru8\0"
	"rs16\0"
	"rs8\0"
	"mcpy\0"
	"mcmp\0"
	"mchr\0"
	"mset\0"

	/* RANDOM */
	"srand\0"
	"rand\0"

	/* IO */
	"println\0"
	"print_number\0|";

#ifdef DEBUG

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
	return (pgm_read_ptr(_builtins + id))(args);
}

