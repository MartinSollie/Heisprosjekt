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

    // Initialize system
    fsm_evSystemStarted();

    // Bool variable for each button used for edge triggering
    // and preventing button holding from continuosly adding new orders
    bool buttonCommandBlocked[N_FLOORS] = {0};
    bool buttonUpBlocked[N_FLOORS-1] = {0};
    bool buttonDownBlocked[N_FLOORS-1] = {0};


    while(1){

    	//Poll buttons and add orders
    	for (int i = 0; i < N_FLOORS; i++){

            // Check elevator panel buttons for all floors
    		if(elev_get_button_signal(BUTTON_COMMAND, i) && (buttonCommandBlocked[i] == false)){
    			order_addElevPanelOrder(i);
                buttonCommandBlocked[i] = true; // Block new presses until button has been released
                if(order_getElevPanelFlag(i)){elev_set_button_lamp(BUTTON_COMMAND, i, 1);}
    		}
            else if (!elev_get_button_signal(BUTTON_COMMAND, i)){
                buttonCommandBlocked[i] = false; //Button is released, allow new press
            }

            // Check floor panel buttons in direction down for all floors except ground floor
    		if(i != 0){
    			if(elev_get_button_signal(BUTTON_CALL_DOWN, i) && (buttonDownBlocked[i-1] == false)){
    				order_addFloorPanelOrder(i, -1);
                    buttonDownBlocked[i-1] = true;
                    if(order_getFloorPanelFlag(i,-1)){elev_set_button_lamp(BUTTON_CALL_DOWN, i, 1);}
                    //printf("Detected DOWN button press %d\n",i);
    			}
                else if (!elev_get_button_signal(BUTTON_CALL_DOWN, i)){
                    buttonDownBlocked[i-1] = false;
                }
    		}
            
            // Check floor panel buttons in direction up for all floors except top floor
    		if(i != N_FLOORS-1){
    			if(elev_get_button_signal(BUTTON_CALL_UP, i) && (buttonUpBlocked[i] == false)){
    				order_addFloorPanelOrder(i,1);
                    buttonUpBlocked[i] = true;
                    if(order_getFloorPanelFlag(i,1)){elev_set_button_lamp(BUTTON_CALL_UP, i, 1);}
                    //printf("Detected UP button press %d\n",i);
    			}
                else if (!elev_get_button_signal(BUTTON_CALL_UP, i)){
                    buttonUpBlocked[i] = false;
                }
    		}
    	}


        // Handle events:

        // If stop button has been pressed, do nothing
    	if (fsm_getCurrentState() != STATE_STOP_BUTTON_PRESSED){
    		if(elev_get_stop_signal()){
    			fsm_evStopButtonPressed();
    		}
    	}

        // Check for events based on current state
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
			case(STATE_CONTINUE_MOVING):{
                // Check if a floor is reached:
                // Must be a different floor than the last floor reached, unless direction is inverted
				int floor = elev_get_floor_sensor_signal();
				if(((floor != -1) && (floor != pos_getLastFloorVisited())) || ((floor == pos_getLastFloorVisited()) && (pos_getCurrentDirection() != pos_getDirectionWhenLeavingLastFloor()))){
					fsm_evNextFloorReached();
				}
				break;
                }
    	}
    }

}
