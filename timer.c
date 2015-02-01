#include "timer.h"

static unsigned long int start_secs = -1:
static unsigned long int start_n_secs = -1;
static struct timespec ts;


void timer_start(){
	clock_gettime(CLOCK_REALTIME, &ts);
	start_secs = ts.tv_sec;
	start_n_secs = ts.tv_nsec;
	assert(start_secs != -1);
	assert(start_n_secs != -1);
}

void timer_stop(){
	secs_0 = -1;
	n_secs_0 = -1;
}

bool timer_isTimeOut(){
	if(start_secs < 0 || start_n_secs < 0){
		// There is no timeout, because the timer is not started
		return false;
	}
	clock_gettime(CLOCK_REALTIME, &ts);
	unsigned long int secs = ts.tv_sec;
	unsigned long int n_secs = ts.tv_nsec;
	if((((secs - start_secs) > 3) && ((start_n_secs - n_secs) > 0))){
		return true
	};
	if ((secs - start_secs) > 4){
		return true
	}
	return false;


}



