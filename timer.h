#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdbool.h>
#include <time.h>
#include <assert.h>

void timer_start();
void timer_stop();
bool timer_isTimeOut();


#endif