#include <stdio.h>
#include "elev.h"
#include "elev_fsm.h"
#include "timer.h"
#include "elevOrderData.h"
#include "elevPositionData.h"


static state_t state;

state_t fsm_getCurrentState(void){
	return state;
}

void fsm_elevInit(void){
	printf("Entered elevator init function\n");	
	setCurrentDirection(DIRN_UP);
	int floorSignal = elev_get_floor_sensor_signal();
	printf("Init floorSignal = %d \n",floorSignal);
	if (floorSignal == -1){ // If between floors
		printf("Starting motor\n");
		printf("getCurrentDirection returns %d \n",getCurrentDirection());
		elev_set_motor_direction(getCurrentDirection());
		//Stop when a floor is reached
		while(floorSignal == -1){
			floorSignal = elev_get_floor_sensor_signal();
			if(floorSignal != -1){
				printf("Stopping motor\n");
				elev_set_motor_direction(DIRN_STOP);
			}
		}
	}
	setCurrentPosition(floorSignal);
	elev_set_floor_indicator(floorSignal);
	setLastFloorVisited(floorSignal);
	state = STATE_AT_FLOOR;
	printf("Entering state STATE_AT_FLOOR (ref:1)\n");
	return;
}

void evStopButtonPressed(){
	elev_set_motor_direction(DIRN_STOP);
	if(elev_get_floor_sensor_signal() != -1){
		elev_set_door_open_lamp(1);
	}
	deactivateAndDeleteOrders();
	state = STATE_STOP_BUTTON_PRESSED;
	printf("Entering state STATE_STOP_BUTTON_PRESSED\n");
}
void evStopButtonReleasedAtFloor(){
	activateOrdering();
	//+sjekk timer og lukk dør hvis den har vært åpen i over 3 sekunder
	elev_set_door_open_lamp(0);
	state = STATE_AT_FLOOR;
	printf("Entering state STATE_AT_FLOOR (ref:2)\n");
}
void evStopButtonReleasedBetweenFloors(){
	activateOrdering();
	state = STATE_STOP_BUTTON_RELEASED_BETWEEN_FLOORS;
	printf("Entering state STATE_STOP_BUTTON_RELEASED_BETWEEN_FLOORS\n");
}
void evAtFloor(){
	direction_t dir = getCurrentDirection();
	position_t floor = getCurrentPosition(); //Will not be between floors at this point
	elev_set_floor_indicator(floor);

	//Any request to get off on this floor?
	if(getElevPanelFlag(floor)){
		printf("Request to get off on this floor detected\n");
		//stop
		elev_set_motor_direction(DIRN_STOP);
		printf("   Stopping motor\n");
		//open door
		elev_set_door_open_lamp(1);
		printf("   Opening doors\n");
		//start timer
		timer_start();
		//delete orders for this floor
		deleteFloorOrders(floor);
		state = STATE_ELEVATOR_OPEN;
		printf("   Entering state STATE_ELEVATOR_OPEN (ref:1)\n");
		return;
	}

	//Any request to get on in current direction?
	if(getFloorPanelFlag(floor,dir)){
		printf("Request to get on in current direction detected\n");
		//stop
		elev_set_motor_direction(DIRN_STOP);
		printf("   Stopping motor\n");
		//open door
		elev_set_door_open_lamp(1);
		printf("   Opening doors\n");
		//start timer
		timer_start();
		//delete orders for this floor
		deleteFloorOrders(floor);
		state = STATE_ELEVATOR_OPEN;
		printf("   Entering state STATE_ELEVATOR_OPEN (ref:2)\n");
		return;
	}

	//Any requests ahead?
	if(dir == DIRN_DOWN){
		for (int i = floor-1; i >= 0; i--){
			if((getElevPanelFlag(i) || getFloorPanelFlag(i,DIRN_UP) || getFloorPanelFlag(i,DIRN_DOWN)) && state != STATE_CONTINUE_MOVING){
				printf("Request ahead detected\n");
				elev_set_motor_direction(dir);
				printf("   Starting motor\n");
				setLastFloorVisited(getCurrentPosition());
				state = STATE_CONTINUE_MOVING;
				printf("   Entering state STATE_CONTINUE_MOVING (ref:1)\n");
				return;
			}
		}
	}
	else{
		for (int i = floor+1; i < N_FLOORS; i++){
			if((getElevPanelFlag(i) || getFloorPanelFlag(i,DIRN_UP) || getFloorPanelFlag(i,DIRN_DOWN)) && state != STATE_CONTINUE_MOVING){
				printf("Request ahead detected\n");
				elev_set_motor_direction(dir);
				printf("   Starting motor\n");
				setLastFloorVisited(getCurrentPosition());
				state = STATE_CONTINUE_MOVING;
				printf("   Entering state STATE_CONTINUE_MOVING (ref:2)\n");
				return;
			}
		}
	}

	//Any request to get on in opposite direction on current floor?
	if(dir == DIRN_DOWN){
		if(getFloorPanelFlag(floor, DIRN_UP)){
			printf("Request to get on in opposite direction on current floor detected\n");
			invertCurrentDirection();
			dir = getCurrentDirection();
			//Request to get on in current direction on this floor
			if(getFloorPanelFlag(floor,dir)){
				//stop
				elev_set_motor_direction(DIRN_STOP);
				printf("   Stopping motor\n");
				//open door
				elev_set_door_open_lamp(1);
				printf("   Opening doors\n");
				//start timer
				timer_start();
				//delete orders for this floor
				deleteFloorOrders(floor);
				state = STATE_ELEVATOR_OPEN;
				printf("   Entering state STATE_ELEVATOR_OPEN (ref:3)\n");
				return;
			}
		}
	}
	if(dir == DIRN_UP){
		if(getFloorPanelFlag(floor, DIRN_DOWN)){
			printf("Request to get on in opposite direction on current floor detected\n");
			invertCurrentDirection();
			dir = getCurrentDirection();
			//Request to get on in current direction on this floor
			if(getFloorPanelFlag(floor,dir)){
				//stop
				elev_set_motor_direction(DIRN_STOP);
				printf("   Stopping motor\n");
				//open door
				elev_set_door_open_lamp(1);
				printf("   Opening doors\n");
				//start timer
				timer_start();
				//delete orders for this floor
				deleteFloorOrders(floor);
				state = STATE_ELEVATOR_OPEN;
				printf("Entering state STATE_ELEVATOR_OPEN (ref:4)\n");
				return;
			}
		}
	}	

	//Any requests behind?
	if(dir == DIRN_DOWN){
		for(int i = floor+1; i < N_FLOORS; i++){
			if(getElevPanelFlag(i) || getFloorPanelFlag(i,DIRN_UP) || getFloorPanelFlag(i,DIRN_DOWN)){
				printf("Requests behind detected\n");
				invertCurrentDirection();
				printf("   Direction inverted\n");
				dir = getCurrentDirection();
				elev_set_motor_direction(dir);
				printf("   starting motor");
				setLastFloorVisited(getCurrentPosition());
				state = STATE_CONTINUE_MOVING;
				printf("   Entering state STATE_CONTINUE_MOVING (ref:3)\n");
				return;
			}
		}
	}
	if(dir == DIRN_UP){
		for(int i = floor-1; i >= 0; i--){
			if(getElevPanelFlag(i) || getFloorPanelFlag(i,DIRN_UP) || getFloorPanelFlag(i,DIRN_DOWN)){
				printf("Requests behind detected\n");
				invertCurrentDirection();
				printf("   Direction inverted\n");
				dir = getCurrentDirection();
				elev_set_motor_direction(dir);
				printf("   starting motor");
				setLastFloorVisited(getCurrentPosition());
				state = STATE_CONTINUE_MOVING;
				printf("    Entering state STATE_CONTINUE_MOVING (ref:4)\n");
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
				setLastFloorVisited(getCurrentPosition());
				state = STATE_CONTINUE_MOVING;
				printf("Entering state STATE_CONTINUE_MOVING (ref:5)\n");
				return;
			}
		}
		for (int i = 0; i <= upperLimitFloorsBelow; i++){
			if(getElevPanelFlag(i) || getFloorPanelFlag(i,DIRN_UP) || getFloorPanelFlag(i,DIRN_DOWN)){
				invertCurrentDirection();
				elev_set_motor_direction(dir);
				setLastFloorVisited(getCurrentPosition());
				state = STATE_CONTINUE_MOVING;
				printf("Entering state STATE_CONTINUE_MOVING (ref:6)\n");
				return;
			}
		}
	}
	else{
		for (int i = 0; i <= upperLimitFloorsBelow; i++){
			if(getElevPanelFlag(i) || getFloorPanelFlag(i,DIRN_UP) || getFloorPanelFlag(i,DIRN_DOWN)){
				elev_set_motor_direction(dir);
				setLastFloorVisited(getCurrentPosition());
				state = STATE_CONTINUE_MOVING;
				printf("Entering state STATE_CONTINUE_MOVING (ref:7)\n");
				return;
			}
		}
		for (int i = lowerLimitFloorsAbove; i < N_FLOORS; i++){
			if(getElevPanelFlag(i) || getFloorPanelFlag(i,DIRN_UP) || getFloorPanelFlag(i,DIRN_DOWN)){
				invertCurrentDirection();
				elev_set_motor_direction(dir);
				setLastFloorVisited(getCurrentPosition());
				state = STATE_CONTINUE_MOVING;
				printf("Entering state STATE_CONTINUE_MOVING (ref:8)\n");
				return;
			}
		}		
	}

}

void fsm_evTimeOut(){
	if(timer_isTimeOut()){
		state = STATE_AT_FLOOR;
		printf("Entering state STATE_AT_FLOOR after timeout\n");
		elev_set_door_open_lamp(0);
		timer_stop();
		return;
	}
}






