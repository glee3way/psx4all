#ifndef _PSX4ALL_PROFILER_H_
#define _PSX4ALL_PROFILER_H_

enum{
	PROFILER_TOTAL = 0,
	PROFILER_BLIT,
	PROFILER_DMA0,
	PROFILER_DMA1,
	PROFILER_DMA2,
	PROFILER_DMA3,
	PROFILER_DMA4,
	PROFILER_DMA5,
	PROFILER_DMA6,
	PROFILER_OPCODES,
	PROFILER_READWRITE,
	PROFILER_GPU,
};



#ifndef PROFILER_PSX4ALL

#define psx4all_prof_start(A)
#define psx4all_prof_end(A)
#define psx4all_prof_show()
#define psx4all_prof_init()

#else

#ifdef PROFILER_SDL
#include <SDL.h>
#define PROFILER__REDUCTION__ 0
#define PROFILER__ADJUST__ 0
#define get_time_in_micros() (unsigned long long)SDL_GetTicks()
#else
#ifndef DREAMCAST
#include <sys/time.h>

#ifdef WIN32
int gettimeofday(struct timeval *tp, struct timezone *tzp);
#endif

static INLINE unsigned long long get_time_in_micros(void)
{
	unsigned long long ret;
	struct timeval tm;
	gettimeofday(&tm,NULL);
	ret=tm.tv_sec;
	ret*=1000000LL;
	ret+=tm.tv_usec;
	return ret;
}
#define PROFILER__REDUCTION__ 0
#define PROFILER__ADJUST__ 0
#else
#include <kos.h>
#define PROFILER__REDUCTION__ 2
#define PROFILER__ADJUST__ 1
#define get_time_in_micros() timer_us_gettime64()
#endif
#endif

#define PSX4ALL_PROFILER_MAX 64

extern unsigned long long psx4all_prof_initial[PSX4ALL_PROFILER_MAX];
extern unsigned long long psx4all_prof_sum[PSX4ALL_PROFILER_MAX];
extern unsigned long long psx4all_prof_executed[PSX4ALL_PROFILER_MAX];
extern int psx4all_prof_started[PSX4ALL_PROFILER_MAX];

static INLINE void psx4all_prof_start(unsigned a)
{
	if (psx4all_prof_started[a])
		return;
	psx4all_prof_executed[a]++;
	psx4all_prof_initial[a]=get_time_in_micros();
	psx4all_prof_started[a]=1;
}


static INLINE void psx4all_prof_end(unsigned a)
{
	if (!psx4all_prof_started[a])
		return;

	extern unsigned psx4all_prof_total;
	int i;
	for(i=0;i<psx4all_prof_total;i++)
		psx4all_prof_initial[i]+=PROFILER__REDUCTION__;
	psx4all_prof_sum[a]+=get_time_in_micros()-psx4all_prof_initial[a]+PROFILER__ADJUST__;
	psx4all_prof_started[a]=0;
}

#undef PROFILER__REDUCTION__
#undef PROFILER__ADJUST__

void psx4all_prof_init(void);
void psx4all_prof_add(char *msg);
void psx4all_prof_show(void);
void psx4all_prof_setmsg(int n, char *msg);


#endif





#endif
