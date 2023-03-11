#if PLATFORM == PLATFORM_AVR

static void timer_init(void)
{
	/* TODO */
}

static u32 millis(void)
{
	/* TODO */
	return 0;
}

#elif PLATFORM == PLATFORM_LINUX

#include <sys/time.h>

static u32 _sec_start, _usec_start;

static void timer_init(void)
{
	struct timeval ts;
	gettimeofday(&ts, NULL);
	_sec_start = ts.tv_sec;
	_usec_start = ts.tv_usec;
}

static u32 millis(void)
{
	struct timeval ts;
	gettimeofday(&ts, NULL);
	return (ts.tv_sec - _sec_start) * 1000 + (ts.tv_usec - _usec_start) / 1000;
}

#endif

