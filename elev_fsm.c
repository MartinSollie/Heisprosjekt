#include <stdio.h>
#include "elev.h"
#include "elev_fsm.h"
#include "timer.h"
#include "elevOrderData.h"


static state_t state;

state_t fsm_getCurrentState(){
	return state;
}
int fsm_elevInit(){
	//Clear all lights
	elev_set_door_open_lamp(0);
	elev_set_stop_lamp(0);
	for(int i = 0; i < 4; i++){
		elev_set_button_lamp(BUTTON_COMMAND, i, 0);
		if (i != 3){
			elev_set_button_lamp(BUTTON_CALL_UP, i, 0);
		}
		if (i != 0){
			elev_set_button_lamp(BUTTON_CALL_DOWN, i, 0);
		}
	}
	
	setCurrentDirection(DIRN_UP);
	int floorSignal = elev_get_floor_sensor_signal();
	if (floorSignal == -1){
		elev_set_motor_direction(getCurrentDirection());
		//Stop when a floor is reached
		while(1){
			floorSignal = elev_get_floor_sensor_signal();
			if(floorSignal != -1){
				elev_set_motor_direction(DIRN_STOP);
				break;
			}
		}
	}
	setCurrentPosition(floorSignal);
	elev_set_floor_indicator(floorSignal);
}

void evStopButtonPressed(){
	elev_set_motor_direction(DIRN_STOP);		//Bør dette kun gjøres hvis den ikke står i ro?
	if(elev_get_floor_sensor_signal() != -1){
		elev_set_door_open_lamp(1);
	}
	deactivateOrderingAndDeleteOrders();
	state = state_stopButtonPressed;
}
void evStopButtonReleasedAtFloor(){
	activateOrdering();
	//+sjekk timer og lukk dør hvis den har vært åpen i over 3 sekunder
	state = state_atFloor;
}
void evStopButtonReleasedBetweenFloors(){
	activateOrdering();
	state = state_stopButtonReleased;
}
void evAtFloor(){
	direction_t dir = getCurrentDirection();
	position_t floor = getCurrentPosition();
	//Any request to get off on this floor?
	//Any request to get on in current direction?
	//Any requests ahead?
	//Any request to get on in opposite direction on current floor?
	//Any requests behind?
}

void evAtFloor(){

}


