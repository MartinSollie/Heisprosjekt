#include <stdio.h>
#include <stdbool.h>
#include "elev.h"
#include "elev_fsm.h"
#include "timer.h"
#include "elevOrderData.h"
#include "elevPositionData.h"

static state_t state;

state_t fsm_getCurrentState(void){
	return state;
}

void fsm_evSystemStarted(void){
	printf("Entered elevator init function\n");	
	setCurrentDirection(UP);
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
	elev_set_floor_indicator(floorSignal);
	setLastFloorVisited(floorSignal);
	state = STATE_CHECK_ELEVATOR_ACTIONS;
	printf("Entering state STATE_CHECK_ELEVATOR_ACTIONS (ref:init)\n");
}

void fsm_evStopButtonPressed(void){
	elev_set_motor_direction(DIRN_STOP);
	printf("Stopping motor\n");
	elev_set_stop_lamp(1);
	if(elev_get_floor_sensor_signal() != -1){
		elev_set_door_open_lamp(1);
		printf("Opening door\n");
	}
	deactivateAndDeleteOrders();
	for(int i = 0; i < NFLOORS; i++){
		elev_set_button_lamp(BUTTON_COMMAND, i, 0);
		if(i != 0){elev_set_button_lamp(BUTTON_CALL_DOWN, i, 0);}
		if(i != NFLOORS-1){elev_set_button_lamp(BUTTON_CALL_UP, i, 0);}
	}
	printf("Deleted all orders, deactivated ordering\n");
	if(state == STATE_CHECK_ELEVATOR_ACTIONS || state == STATE_CONTINUE_MOVING){
		timer_start();
	}
	state = STATE_STOP_BUTTON_PRESSED;
	printf("Entering state STATE_STOP_BUTTON_PRESSED\n");
}

void fsm_evStopButtonReleasedAtFloor(void){
	activateOrdering();
	elev_set_stop_lamp(0);
	state = STATE_STOP_BUTTON_RELEASED_AT_FLOOR;
	printf("Entering state STATE_STOP_BUTTON_RELEASED_AT_FLOOR\n");
}

void fsm_evStopButtonReleasedBetweenFloors(void){
	activateOrdering();
	elev_set_stop_lamp(0);
	state = STATE_STOP_BUTTON_RELEASED_BETWEEN_FLOORS;
	printf("Entering state STATE_STOP_BUTTON_RELEASED_BETWEEN_FLOORS\n");

}

void fsm_evReadyToCheckActions(void){
	if (state == STATE_STOP_BUTTON_RELEASED_AT_FLOOR){
		if(timer_isTimeOut()){
			timer_stop();
		}
		else{
			return;
		}
	}
	if(state == STATE_STOP_BUTTON_RELEASED_BETWEEN_FLOORS){
		bool orderFound = false;
		for(int i = 0; i < NFLOORS; i++){
			if (getElevPanelFlag(i) || getFloorPanelFlag(i,UP) || getFloorPanelFlag(i,DOWN)){
				orderFound = true;
			}
		}
		if (orderFound == false){
			return;
		}
	}

	int dir = getCurrentDirection();
	//Any request to get off, or on in current direction here?
	if (elev_get_floor_sensor_signal() != -1){
		unsigned int currentFloor = getLastFloorVisited();
		if((getElevPanelFlag(currentFloor) == true) || (getFloorPanelFlag(currentFloor,dir) == true)){
			printf("dir = %d\n",dir);
			printf("getElevPanelFlag(currentFloor) = %d\n", getElevPanelFlag(currentFloor));
			printf("getFloorPanelFlag(currentFloor,dir) = %d\n",getFloorPanelFlag(currentFloor,dir));
			elev_set_motor_direction(DIRN_STOP);
			elev_set_door_open_lamp(1);
			deleteFloorOrders(currentFloor);
			elev_set_button_lamp(BUTTON_COMMAND, currentFloor, 0);
			if(currentFloor != NFLOORS-1){elev_set_button_lamp(BUTTON_CALL_UP, currentFloor, 0);}
			if(currentFloor != 0){elev_set_button_lamp(BUTTON_CALL_DOWN, currentFloor, 0);}
			timer_start();
			state = STATE_ELEVATOR_OPEN;
			return;
		}
	}


	//Any request ahead?
	if (elev_get_floor_sensor_signal() != -1){ //Elevator is at a floor
		if (dir == 1){
			for (int i = getLastFloorVisited()+1; i < NFLOORS; i++){
				if(getElevPanelFlag(i) || getFloorPanelFlag(i,UP) || getFloorPanelFlag(i,DOWN)){
					saveDirectionWhenLeavingFloor(dir);
					elev_set_motor_direction(dir);
					state = STATE_CONTINUE_MOVING;
					printf("Entering state STATE_CONTINUE_MOVING\n");
					return;
				}
			}
		}
		else{
			for(int i = getLastFloorVisited()-1; i >= 0; i--){
				if(getElevPanelFlag(i) || getFloorPanelFlag(i,UP) || getFloorPanelFlag(i,DOWN)){
					saveDirectionWhenLeavingFloor(dir);
					elev_set_motor_direction(dir);
					state = STATE_CONTINUE_MOVING;
					printf("Entering state STATE_CONTINUE_MOVING\n");
					return;
				}
			}
		}
	}
	else{ //between floors after pressing and releasing stop button while moving
		//Must find the elevator's position
		int lastDir = getDirectionWhenLeavingLastFloor();
		unsigned int lastFloor = getLastFloorVisited();
		
		if(lastDir == 1){ // Above last floor
			if (dir == 1){ //Current direction is up
				for (int i = lastFloor+1; i < NFLOORS; i++){
					if(getElevPanelFlag(i) || getFloorPanelFlag(i,UP) || getFloorPanelFlag(i,DOWN)){
						elev_set_motor_direction(dir);
						state = STATE_CONTINUE_MOVING;
						printf("Entering state STATE_CONTINUE_MOVING\n");
						return;
					}
				}
			}
			else{ //Current direction is down
				for(int i = lastFloor; i >= 0; i--){
					if(getElevPanelFlag(i) || getFloorPanelFlag(i,UP) || getFloorPanelFlag(i,DOWN)){
						elev_set_motor_direction(dir);
						state = STATE_CONTINUE_MOVING;
						printf("Entering state STATE_CONTINUE_MOVING\n");
						return;
					}
				}
			}
		}
		else{ //below last floor
			//Current direction is up
			if (dir == 1){
				for (int i = lastFloor; i < NFLOORS; i++){
					if(getElevPanelFlag(i) || getFloorPanelFlag(i,UP) || getFloorPanelFlag(i,DOWN)){
						elev_set_motor_direction(dir);
						state = STATE_CONTINUE_MOVING;
						printf("Entering state STATE_CONTINUE_MOVING\n");
						return;
					}
				}
			}
			//Current direction is down
			else{
				for(int i = lastFloor-1; i >= 0; i--){
					if(getElevPanelFlag(i) || getFloorPanelFlag(i,UP) || getFloorPanelFlag(i,DOWN)){					

						elev_set_motor_direction(dir);
						state = STATE_CONTINUE_MOVING;
						printf("Entering state STATE_CONTINUE_MOVING\n");
						return;
					}
				}
			}
		}
	}

	//Any request behind, or in opposite direction here?
	if(elev_get_floor_sensor_signal() != -1){ //Elevator is at floor
		unsigned int lastFloor = getLastFloorVisited();
		if (dir == 1){ //Current direction is up
			//Checking if there are orders heading down on current floor
			if(getFloorPanelFlag(lastFloor,-1)){
				invertCurrentDirection();
				state = STATE_CHECK_ELEVATOR_ACTIONS;
				printf("Entering state STATE_CHECK_ELEVATOR_ACTIONS\n");
				return;
			}
			else{
				//Must check floors below current floor
				bool orderFound = false;
				for(int i = lastFloor-1; i >= 0; i--){
					if (getElevPanelFlag(i) || getFloorPanelFlag(i,UP) || getFloorPanelFlag(i,DOWN)){
						orderFound = true;
					}
				}
				if (orderFound == true){
					invertCurrentDirection();
					state = STATE_CHECK_ELEVATOR_ACTIONS;
					printf("Entering state STATE_CHECK_ELEVATOR_ACTIONS\n");
					return;
				}
			}
		}

		else{ //Current direction is down
			//Checking if there are orders heading up on current floor
			if(getFloorPanelFlag(lastFloor,1)){
				invertCurrentDirection();
				state = STATE_CHECK_ELEVATOR_ACTIONS;
				printf("Entering state STATE_CHECK_ELEVATOR_ACTIONS\n");
				return;
			}
			else{
				//Must check floors above current floor
				bool orderFound = false;
				for (int i = lastFloor+1; i < NFLOORS; i++){
					if (getElevPanelFlag(i) || getFloorPanelFlag(i,UP) || getFloorPanelFlag(i,DOWN)){
						orderFound = true;
					}
				}
				if (orderFound == true){
					invertCurrentDirection();
					state = STATE_CHECK_ELEVATOR_ACTIONS;
					printf("Entering state STATE_CHECK_ELEVATOR_ACTIONS\n");
					return;
				}
			}
		}
	}
	else{ //Elevator is between floors after releasing stop button
		// check for orders in opposite direction
		bool orderFound = false;
		int lastDir = getDirectionWhenLeavingLastFloor();
		unsigned int lastFloor = getLastFloorVisited();
		if(lastDir == 1){
			if (dir == 1){ //check below
				for (int i = lastFloor; i >= 0; i--){
					if (getElevPanelFlag(i) || getFloorPanelFlag(i,UP) || getFloorPanelFlag(i,DOWN)){
						orderFound = true;
					}
				}
			}
			else{ //check above
				for (int i = lastFloor+1; i < NFLOORS; i++){
					if (getElevPanelFlag(i) || getFloorPanelFlag(i,UP) || getFloorPanelFlag(i,DOWN)){
						orderFound = true;
					}
				}
			}
		}
		else{
			if (dir == 1){ //check below
				for (int i = lastFloor-1; i >= 0; i--){
					if (getElevPanelFlag(i) || getFloorPanelFlag(i,UP) || getFloorPanelFlag(i,DOWN)){
						orderFound = true;
					}
				}
			}
			else{ //check above
				for (int i = lastFloor; i < NFLOORS; i++){
					if (getElevPanelFlag(i) || getFloorPanelFlag(i,UP) || getFloorPanelFlag(i,DOWN)){
						orderFound = true;
					}
				}
			}			
		}
		if (orderFound == true){
			invertCurrentDirection();
			state = STATE_CHECK_ELEVATOR_ACTIONS;
			printf("Entering state STATE_CHECK_ELEVATOR_ACTIONS\n");
			return;
		}		
	}
	state = STATE_CHECK_ELEVATOR_ACTIONS;
}


void fsm_evTimeOut(void){
	elev_set_door_open_lamp(0);
	printf("Closing door.\n");
	timer_stop();
	state = STATE_CHECK_ELEVATOR_ACTIONS;
	printf("Entering state STATE_CHECK_ELEVATOR_ACTIONS (ref:timeout)\n");
}


void fsm_evNextFloorReached(void){
	int floorSignal = elev_get_floor_sensor_signal();
	elev_set_floor_indicator(floorSignal);
	setLastFloorVisited(floorSignal);
	printf("Setting last floor to %d\n", floorSignal);
	state = STATE_CHECK_ELEVATOR_ACTIONS;
	printf("Entering state STATE_CHECK_ELEVATOR_ACTIONS (ref:nextfloor)\n");
}
