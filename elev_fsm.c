#include <stdio.h>
#include "elev.h"
#include "elev_fsm.h"
#include "timer.h"
#include "elevOrderData.h"
#include "elevPositionData.h"


static state_t state;

state_t fsm_getCurrentState(){
	return state;
}

void fsm_elevInit(){	
	setCurrentDirection(DIRN_UP);
	int floorSignal = elev_get_floor_sensor_signal();
	if (floorSignal == -1){ // If between floors
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
	state = STATE_AT_FLOOR;
}

void evStopButtonPressed(){
	elev_set_motor_direction(DIRN_STOP);
	if(elev_get_floor_sensor_signal() != -1){
		elev_set_door_open_lamp(1);
	}
	deactivateOrderingAndDeleteOrders();
	state = STATE_STOP_BUTTON_PRESSED;
}
void evStopButtonReleasedAtFloor(){
	activateOrdering();
	//+sjekk timer og lukk dør hvis den har vært åpen i over 3 sekunder
	elev_set_door_open_lamp(0);
	state = STATE_AT_FLOOR;
}
void evStopButtonReleasedBetweenFloors(){
	activateOrdering();
	state = STATE_STOP_BUTTON_RELEASED;
}
void evAtFloor(){
	direction_t dir = getCurrentDirection();
	position_t floor = getCurrentPosition(); //Will not be between floors at this point

	//Any request to get off on this floor?
	if(getElevPanelFlag(floor)){
		//stop
		elev_set_motor_direction(DIRN_STOP);
		//open door
		elev_set_door_open_lamp(1);
		//start timer
		timer_startTimer();
		//delete orders for this floor
		deleteFloorOrders(floor);
		state = STATE_ELEVATOR_OPEN;
		return;
	}

	//Any request to get on in current direction?
	if(getFloorPanelFlag(floor,dir)){
		//stop
		elev_set_motor_direction(DIRN_STOP);
		//open door
		elev_set_door_open_lamp(1);
		//start timer
		timer_startTimer();
		//delete orders for this floor
		deleteFloorOrders(floor);
		state = STATE_ELEVATOR_OPEN;
		return;
	}

	//Any requests ahead?
	if(dir == DIRN_DOWN){
		for (int i = floor-1; i >= 0; i--){
			if((getElevPanelFlag(i) || getFloorPanelFlag(i,DIRN_UP) || getFloorPanelFlag(i,DIRN_DOWN)) && state != STATE_CONTINUE_MOVING){
				elev_set_motor_direction(dir);
				state = STATE_CONTINUE_MOVING;
				return;
			}
		}
	}
	else{
		for (int i = floor+1; i < N_FLOORS; i++){
			if((getElevPanelFlag(i) || getFloorPanelFlag(i,DIRN_UP) || getFloorPanelFlag(i,DIRN_DOWN)) && state != STATE_CONTINUE_MOVING){
				elev_set_motor_direction(dir);
				state = STATE_CONTINUE_MOVING;
				return;
			}
		}
	}

	//Any request to get on in opposite direction on current floor?
	if(dir == DIRN_DOWN){
		if(getFloorPanelFlag(floor, DIRN_UP)){
			invertCurrentDirection();
			dir = getCurrentDirection();
			//Request to get on in current direction on this floor
			if(getFloorPanelFlag(floor,dir)){
				//stop
				elev_set_motor_direction(DIRN_STOP);
				//open door
				elev_set_door_open_lamp(1);
				//start timer
				timer_startTimer();
				//delete orders for this floor
				deleteFloorOrders(floor);
				state = STATE_ELEVATOR_OPEN;
				return;
			}
		}
	}
	if(dir == DIRN_UP){
		if(getFloorPanelFlag(floor, DIRN_DOWN)){
			invertCurrentDirection();
			dir = getCurrentDirection();
			//Request to get on in current direction on this floor
			if(getFloorPanelFlag(floor,dir)){
				//stop
				elev_set_motor_direction(DIRN_STOP);
				//open door
				elev_set_door_open_lamp(1);
				//start timer
				timer_startTimer();
				//delete orders for this floor
				deleteFloorOrders(floor);
				state = STATE_ELEVATOR_OPEN;
				return;
			}
		}
	}	

	//Any requests behind?
	if(dir == DIRN_DOWN){
		for(int i = floor+1; i < N_FLOORS; i++){
			if(getElevPanelFlag(i) || getFloorPanelFlag(i,DIRN_UP) || getFloorPanelFlag(i,DIRN_DOWN)){
				invertCurrentDirection();
				dir = getCurrentDirection();
				elev_set_motor_direction(dir);
				state = STATE_CONTINUE_MOVING;
				return;
			}
		}
	}
	if(dir == DIRN_UP){
		for(int i = floor-1; i >= 0; i--){
			if(getElevPanelFlag(i) || getFloorPanelFlag(i,DIRN_UP) || getFloorPanelFlag(i,DIRN_DOWN)){
				invertCurrentDirection();
				dir = getCurrentDirection();
				elev_set_motor_direction(dir);
				state = STATE_CONTINUE_MOVING;
				return;
			}
		}
	}
}

void evStoppedBetweenFloorFlagSet(){
	position_t pos = getCurrentPosition(); //will be between floors
	direction_t dir = getCurrentDirection();
	unsigned int upperLimitFloorsBelow;
	unsigned int lowerLimitFloorsAbove;
	switch(pos){
		case(FIRST_SECOND):
			upperLimitFloorsBelow = 0;
			lowerLimitFloorsAbove = 1;
		case(SECOND_THIRD):
			upperLimitFloorsBelow = 1;
			lowerLimitFloorsAbove = 2;
		case(THIRD_FOURTH):
			upperLimitFloorsBelow = 3;
			lowerLimitFloorsAbove = 4;
	}
	if(dir == DIRN_UP){
		for (int i = lowerLimitFloorsAbove; i < N_FLOORS; i++){
			if(getElevPanelFlag(i) || getFloorPanelFlag(i,DIRN_UP) || getFloorPanelFlag(i,DIRN_DOWN)){
				elev_set_motor_direction(dir);
				state = STATE_CONTINUE_MOVING;
				return;
			}
		}
		for (int i = 0; i <= upperLimitFloorsBelow; i++){
			if(getElevPanelFlag(i) || getFloorPanelFlag(i,DIRN_UP) || getFloorPanelFlag(i,DIRN_DOWN)){
				invertCurrentDirection();
				elev_set_motor_direction(dir);
				state = STATE_CONTINUE_MOVING;
				return;
			}
		}
	}
	else{
		for (int i = 0; i <= upperLimitFloorsBelow; i++){
			if(getElevPanelFlag(i) || getFloorPanelFlag(i,DIRN_UP) || getFloorPanelFlag(i,DIRN_DOWN)){
				elev_set_motor_direction(dir);
				state = STATE_CONTINUE_MOVING;
				return;
			}
		}
		for (int i = lowerLimitFloorsAbove; i < N_FLOORS; i++){
			if(getElevPanelFlag(i) || getFloorPanelFlag(i,DIRN_UP) || getFloorPanelFlag(i,DIRN_DOWN)){
				invertCurrentDirection();
				elev_set_motor_direction(dir);
				state = STATE_CONTINUE_MOVING;
				return;
			}
		}		
	}

}

void fsm_evTimeOut(){
	if(timer_isTimeOut()){
		state = STATE_AT_FLOOR;
		elev_set_door_open_lamp(0);
		timer_stopAndResetTimer();
		return;
	}
}






