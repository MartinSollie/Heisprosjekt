/*
Elevator timekeeping module
timer.c
Martin Sollie & Øystein Lasson
*/

#include "timer.h"
#include <time.h>
#include <assert.h>

static time_t start_seconds = -1;
static long int start_nanoseconds = -1;
static struct timespec ts; // Structure declared in time.h, members are tv_sec and tv_nsec

void timer_start(){
	clock_gettime(CLOCK_REALTIME, &ts); // Retrieves Unix time to struct ts
	start_seconds = ts.tv_sec; // Seconds since Unix epoch (jan. 1, 1970)
	start_nanoseconds = ts.tv_nsec; // Nanoseconds expired in the current second
	assert(start_seconds != -1);
	assert(start_nanoseconds != -1);
}

void timer_stop(){
	start_seconds = -1;
	start_nanoseconds = -1;
}

bool timer_isTimeOut(){
	if((start_seconds < 0) || (start_nanoseconds < 0)){
		return false; // There is no timeout, because the timer is not started
	}
	clock_gettime(CLOCK_REALTIME, &ts);
	time_t seconds = ts.tv_sec;
	long int nanoseconds = ts.tv_nsec;
	long int diff_seconds = seconds - start_seconds;
	long int diff_nanoseconds = nanoseconds - start_nanoseconds;
	if((diff_seconds >= 3) && (diff_nanoseconds > 0)){ // If more than 3 seconds has passed
		return true;
	}
	return false;
}
