#include <stdio.h>
#include "timer.h"

int main(void){
	timer_start();
	while(1){
		if(timer_isTimeOut()){
			timer_stop();
			break;
		}
	}

	return 0;
}