#include <stdio.h>
#include <stdbool.h>
#include "timer.h"
#include "elevPositionData.h"
#include "elevOrderData.h"
#include "elev_fsm.h"
#include "elev.h"

int main(void){
	// Initialize elevator hardware
    if (!elev_init()) {
        printf("Unable to initialize elevator hardware!\n");
        return 1;
    }

    fsm_evSystemStarted();

    // Variables preventing button holding from continuosly adding new orders
    bool btn_cmd[N_FLOORS] = {0};
    bool btn_up[N_FLOORS-1] = {0};
    bool btn_down[N_FLOORS-1] = {0};

    while(1){

    	//Poll buttons and add orders
    	for (int i = 0; i < N_FLOORS; i++){
    		if(elev_get_button_signal(BUTTON_COMMAND, i) && btn_cmd[i] == false){
    			addElevPanelOrder(i);
                btn_cmd[i] = true;
                if(getElevPanelFlag(i)){elev_set_button_lamp(BUTTON_COMMAND, i, 1);}
                //printf("Detected command button press %d\n",i);
    		}
            else{
                btn_cmd[i] = false;
            }


    		if(i != 0){
    			if(elev_get_button_signal(BUTTON_CALL_DOWN, i) && btn_down[i-1] == false){
    				addFloorPanelOrder(i, -1);
                    btn_down[i-1] = true;
                    if(getFloorPanelFlag(i,-1)){elev_set_button_lamp(BUTTON_CALL_DOWN, i, 1);}
                    //printf("Detected DOWN button press %d\n",i);
    			}
                else{
                    btn_down[i-1] = false;
                }
    		}
            
    		if(i != N_FLOORS-1){
    			if(elev_get_button_signal(BUTTON_CALL_UP, i) && btn_up[i] == false){
    				addFloorPanelOrder(i,1);
                    btn_up[i] = true;
                    if(getFloorPanelFlag(i,1)){elev_set_button_lamp(BUTTON_CALL_UP, i, 1);}
                    //printf("Detected UP button press %d\n",i);
    			}
                else{
                    btn_up[i] = false;
                }
    		}
    	}


        // Handle events:

        // If stop button is still being held, do nothing
    	if (fsm_getCurrentState() != STATE_STOP_BUTTON_PRESSED){
    		if(elev_get_stop_signal()){
    			fsm_evStopButtonPressed();
    		}
    	}

    	switch(fsm_getCurrentState()){
    		case(STATE_CHECK_ELEVATOR_ACTIONS):
    			fsm_evReadyToCheckActions();
    			break;
			case(STATE_STOP_BUTTON_PRESSED):
				if(elev_get_stop_signal() == 0){
					if(elev_get_floor_sensor_signal() != -1){
						fsm_evStopButtonReleasedAtFloor();
					}
					else{
						fsm_evStopButtonReleasedBetweenFloors();
					}
				}
				break;
			case(STATE_STOP_BUTTON_RELEASED_AT_FLOOR):
				fsm_evReadyToCheckActions();
				break;
			case(STATE_STOP_BUTTON_RELEASED_BETWEEN_FLOORS):
				fsm_evReadyToCheckActions();
				break;
			case(STATE_ELEVATOR_OPEN):
				if(timer_isTimeOut()){
					fsm_evTimeOut();
				}
				break;
			case(STATE_CONTINUE_MOVING):;
				int floor = elev_get_floor_sensor_signal();
				if(((floor != -1) && (floor != getLastFloorVisited())) || ((floor == getLastFloorVisited()) && (getCurrentDirection() != getDirectionWhenLeavingLastFloor()))){
					fsm_evNextFloorReached();
				}
				break;
    	}
    }

}
