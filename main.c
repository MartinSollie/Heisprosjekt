//Enable POSIX definitions when compiling with C99 instead of GCC
#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif /* __STDC_VERSION__ */


#include <stdio.h>
#include "elev.h"
#include "timer.h"
#include "elevPositionData.h"
#include "elevOrderData.h"
#include "elev_fsm.h"

int main() {
    // Initialize hardware
    if (!elev_init()) {
        printf("Unable to initialize elevator hardware!\n");
        return 1;
    }

    fsm_elevInit();
    while(1){
    	//Poll buttons and add orders
    	for (int i = 0; i < N_FLOORS; i++){
    		if(elev_get_button_signal(BUTTON_COMMAND, i)){
    			addElevPanelOrder(i);
                printf("Detected command button press %d\n",i);
    		}
    		if(i != 0){
    			if(elev_get_button_signal(BUTTON_CALL_DOWN, i)){
    				addFloorPanelOrder(i, DIR_DOWN);
                    printf("Detected DOWN button press %d\n",i);
    			}
    		}
    		if(i != N_FLOORS-1){
    			if(elev_get_button_signal(BUTTON_CALL_UP, i)){
    				addFloorPanelOrder(i, DIR_UP);
                    printf("Detected UP button press %d\n",i);
    			}
    		}

    	}
    	
    	switch(fsm_getCurrentState()){
    			case(STATE_AT_FLOOR):
    				// stoppknapp
    				if(elev_get_stop_signal()){
    					evStopButtonPressed();
    				}
    				//hvis ikke stoppknapp
    				else{
    					evAtFloor();
    				}
    				break;
				case(STATE_STOP_BUTTON_PRESSED):
					if(!elev_get_stop_signal()){
						position_t p = getCurrentPosition();
						if(p == FIRST || p == SECOND || p == THIRD || p == FOURTH){
							evStopButtonReleasedAtFloor();
						}
						else{
							evStopButtonReleasedBetweenFloors();
						}
					}
					break;
				case(STATE_STOP_BUTTON_RELEASED_BETWEEN_FLOORS):
					evStoppedBetweenFloorFlagSet();
					break;
				case(STATE_ELEVATOR_OPEN):
					//3 seconds elapsed
					if(timer_isTimeOut()){
						fsm_evTimeOut();
					}
					break;
				case(STATE_CONTINUE_MOVING):;//assignment not allowed without ;
					// if next floor reached, if not: do nothing
					int pos = getCurrentPosition();
					if((pos != getLastFloorVisited()) && (pos == 0 || pos == 1 || pos == 2 || pos == 3)){
						evAtFloor();
					}
					break;
    	}

    }
    

    return 0;
}


