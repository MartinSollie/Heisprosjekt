//Enable POSIX definitions when compiling with C99 instead of GCC.
#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif /* __STDC_VERSION__ */

#include "timer.h"
#include <time.h>
#include <assert.h>
#include <stdio.h>

static unsigned long int start_seconds = -1;
static unsigned long int start_nanoseconds = -1;
static struct timespec ts;


void timer_start(){
	clock_gettime(CLOCK_REALTIME, &ts); //this works only in Linux!
	start_seconds = ts.tv_sec;
	start_nanoseconds = ts.tv_nsec; // number of nanoseconds into last whole second
	assert(start_seconds != -1);
	assert(start_nanoseconds != -1);
	printf("Timer started.\n");
}

void timer_stop(){
	start_seconds = -1;
	start_nanoseconds = -1;
	printf("Timer stopped.\n");
}

bool timer_isTimeOut(){
	if(start_seconds < 0 || start_nanoseconds < 0){
		// There is no timeout, because the timer is not started
		return false;
	}
	clock_gettime(CLOCK_REALTIME, &ts);
	unsigned long int seconds = ts.tv_sec;
	unsigned long int nanoseconds = ts.tv_nsec;
	long int diff_seconds = seconds - start_seconds;
	long int diff_nanoseconds = nanoseconds - start_nanoseconds;
	if(((diff_seconds >= 3) && (diff_nanoseconds > 0))){
		printf("Time is out. Time elapsed: %lu.",diff_seconds);
		printf("%09lu seconds.\n",diff_nanoseconds);
		return true;
	};
	return false;


}



