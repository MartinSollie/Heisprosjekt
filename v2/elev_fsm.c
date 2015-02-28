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
	pos_setCurrentDirection(UP);
	int floorSignal = elev_get_floor_sensor_signal();

	if (floorSignal == -1){ // If between floors
		elev_set_motor_direction(pos_getCurrentDirection());

		while(floorSignal == -1){ //Stop when a floor is reached
			floorSignal = elev_get_floor_sensor_signal();
			if(floorSignal != -1){
				elev_set_motor_direction(DIRN_STOP);
			}
		}
	}
	elev_set_floor_indicator(floorSignal);
	pos_setLastFloorVisited(floorSignal);
	state = STATE_CHECK_ELEVATOR_ACTIONS;
}

void fsm_evStopButtonPressed(void){
	elev_set_motor_direction(DIRN_STOP);
	elev_set_stop_lamp(1);
	if(elev_get_floor_sensor_signal() != -1){ // If at floor
		elev_set_door_open_lamp(1);
	}
	order_disableAndDeleteOrders();

	for(int i = 0; i < N_FLOORS; i++){ //Turn off all button lamps
		elev_set_button_lamp(BUTTON_COMMAND, i, 0);
		if(i != 0){elev_set_button_lamp(BUTTON_CALL_DOWN, i, 0);}
		if(i != N_FLOORS-1){elev_set_button_lamp(BUTTON_CALL_UP, i, 0);}
	}

	if(state == STATE_CHECK_ELEVATOR_ACTIONS || state == STATE_MOVING){ // If door was not already open
		timer_start();
	}
	state = STATE_STOP_BUTTON_PRESSED;
}

void fsm_evStopButtonReleasedAtFloor(void){
	order_enableOrdering();
	elev_set_stop_lamp(0);
	state = STATE_STOP_BUTTON_RELEASED_AT_FLOOR;
}

void fsm_evStopButtonReleasedBetweenFloors(void){
	order_enableOrdering();
	elev_set_stop_lamp(0);
	state = STATE_STOP_BUTTON_RELEASED_BETWEEN_FLOORS;
}

void fsm_evReadyToCheckActions(void){
	if (state == STATE_STOP_BUTTON_RELEASED_AT_FLOOR){
		if(timer_isTimeOut()){ // Door has been open 3 seconds
			elev_set_door_open_lamp(0);
			timer_stop();
		}
		else{ // Door has not been open 3 seconds, must wait longer
			return;
		}
	}
	if(state == STATE_STOP_BUTTON_RELEASED_BETWEEN_FLOORS){
		// An order must be placed to proceed, search all flags
		bool orderFound = false;
		for(int i = 0; i < NFLOORS; i++){
			if (order_getElevPanelFlag(i) || order_getFloorPanelFlag(i,UP) || order_getFloorPanelFlag(i,DOWN)){
				orderFound = true;
			}
		}
		if (orderFound == false){
			return;
		}
	}

	int dir = pos_getCurrentDirection();

	// Check for request(s) to get off, or on in current direction here?
	if (elev_get_floor_sensor_signal() != -1){
		unsigned int currentFloor = pos_getLastFloorVisited();
		if((order_getElevPanelFlag(currentFloor) == true) || (order_getFloorPanelFlag(currentFloor,dir) == true)){
			elev_set_motor_direction(DIRN_STOP);
			elev_set_door_open_lamp(1);
			timer_start();

			order_deleteFloorOrders(currentFloor);
			elev_set_button_lamp(BUTTON_COMMAND, currentFloor, 0);
			if(currentFloor != NFLOORS-1){elev_set_button_lamp(BUTTON_CALL_UP, currentFloor, 0);}
			if(currentFloor != 0){elev_set_button_lamp(BUTTON_CALL_DOWN, currentFloor, 0);}
			
			state = STATE_ELEVATOR_OPEN;
			return;
		}
	}


	// Check for request(s) ahead
	if (elev_get_floor_sensor_signal() != -1){ //Elevator is at a floor

		////// TEST THIS
		// If direction is up: check floors above, if direction is down: check floors below
		for (int i = pos_getLastFloorVisited()+dir; dir == 1 ? (i < NFLOORS) : (i >= 0); i += dir){
			if(order_getElevPanelFlag(i) || order_getFloorPanelFlag(i,UP) || order_getFloorPanelFlag(i,DOWN)){
				pos_saveDirectionWhenLeavingFloor(dir);
				elev_set_motor_direction(dir);
				state = STATE_MOVING;
				return;
			}
		}


		/*
		if (dir == 1){
			for (int i = pos_getLastFloorVisited()+1; i < NFLOORS; i++){
				if(order_getElevPanelFlag(i) || order_getFloorPanelFlag(i,UP) || order_getFloorPanelFlag(i,DOWN)){
					pos_saveDirectionWhenLeavingFloor(dir);
					elev_set_motor_direction(dir);
					state = STATE_MOVING;
					return;
				}
			}
		}
		else{
			for(int i = pos_getLastFloorVisited()-1; i >= 0; i--){
				if(order_getElevPanelFlag(i) || order_getFloorPanelFlag(i,UP) || order_getFloorPanelFlag(i,DOWN)){
					pos_saveDirectionWhenLeavingFloor(dir);
					elev_set_motor_direction(dir);
					state = STATE_MOVING;
					return;
				}
			}
		}
		*/
	}





	else{ //between floors after pressing and releasing stop button while moving
		//Must find the elevator's position
		int lastDir = pos_getDirectionWhenLeavingLastFloor();
		unsigned int lastFloor = pos_getLastFloorVisited();
		unsigned int countFromFloor = lastFloor+(dir+lastDir)/2; // KOMMENTER !!!

		for (int i = countFromFloor; dir == 1 ? (i < NFLOORS) : (i >= 0); i += dir){
			if(order_getElevPanelFlag(i) || order_getFloorPanelFlag(i,UP) || order_getFloorPanelFlag(i,DOWN)){
				elev_set_motor_direction(dir);
				state = STATE_MOVING;
				return;
			}
		}

		

		/*
		if(lastDir == 1){ // Above last floor
			if (dir == 1){ //Current direction is up
				for (int i = lastFloor+1; i < NFLOORS; i++){
					if(order_getElevPanelFlag(i) || order_getFloorPanelFlag(i,UP) || order_getFloorPanelFlag(i,DOWN)){
						elev_set_motor_direction(dir);
						state = STATE_MOVING;
						return;
					}
				}
			}
			else{ //Current direction is down
				for(int i = lastFloor; i >= 0; i--){
					if(order_getElevPanelFlag(i) || order_getFloorPanelFlag(i,UP) || order_getFloorPanelFlag(i,DOWN)){
						elev_set_motor_direction(dir);
						state = STATE_MOVING;
						return;
					}
				}
			}
		}
		else{ //below last floor
			//Current direction is up
			if (dir == 1){
				for (int i = lastFloor; i < NFLOORS; i++){
					if(order_getElevPanelFlag(i) || order_getFloorPanelFlag(i,UP) || order_getFloorPanelFlag(i,DOWN)){
						elev_set_motor_direction(dir);
						state = STATE_MOVING;
						return;
					}
				}
			}
			//Current direction is down
			else{
				for(int i = lastFloor-1; i >= 0; i--){
					if(order_getElevPanelFlag(i) || order_getFloorPanelFlag(i,UP) || order_getFloorPanelFlag(i,DOWN)){					
						elev_set_motor_direction(dir);
						state = STATE_MOVING;
						return;
					}
				}
			}
		}*/
	}








	//Any request behind, or in opposite direction here?
	if(elev_get_floor_sensor_signal() != -1){ //Elevator is at floor
		unsigned int lastFloor = pos_getLastFloorVisited();

		for(int i = lastFloor; dir == 1 ? (i >= 0) : (i < NFLOORS); i -= dir){
			if ((i != lastFloor && (order_getElevPanelFlag(i) || order_getFloorPanelFlag(i,UP) || order_getFloorPanelFlag(i,DOWN))) || (i == lastFloor && order_getFloorPanelFlag(i,-dir))){
				pos_invertCurrentDirection();
				state = STATE_CHECK_ELEVATOR_ACTIONS;
				return;
			}
		}

		/*
		if (dir == 1){ //Current direction is up
			//Checking if there are orders heading down on current floor
			if(order_getFloorPanelFlag(lastFloor,-1)){
				pos_invertCurrentDirection();
				state = STATE_CHECK_ELEVATOR_ACTIONS;
				return;
			}
			else{
				//Must check floors below current floor
				bool orderFound = false;
				for(int i = lastFloor-1; i >= 0; i--){
					if (order_getElevPanelFlag(i) || order_getFloorPanelFlag(i,UP) || order_getFloorPanelFlag(i,DOWN)){
						orderFound = true;
					}
				}
				if (orderFound == true){
					pos_invertCurrentDirection();
					state = STATE_CHECK_ELEVATOR_ACTIONS;
					return;
				}
			}
		}

		else{ //Current direction is down
			//Checking if there are orders heading up on current floor
			if(order_getFloorPanelFlag(lastFloor,1)){
				pos_invertCurrentDirection();
				state = STATE_CHECK_ELEVATOR_ACTIONS;
				return;
			}
			else{
				//Must check floors above current floor
				bool orderFound = false;
				for (int i = lastFloor+1; i < NFLOORS; i++){
					if (order_getElevPanelFlag(i) || order_getFloorPanelFlag(i,UP) || order_getFloorPanelFlag(i,DOWN)){
						orderFound = true;
					}
				}
				if (orderFound == true){
					pos_invertCurrentDirection();
					state = STATE_CHECK_ELEVATOR_ACTIONS;
					return;
				}
			}
		}*/
	}
	else{ //Elevator is between floors after releasing stop button
		// check for orders in opposite direction
		////bool orderFound = false;
		int lastDir = pos_getDirectionWhenLeavingLastFloor();
		unsigned int lastFloor = pos_getLastFloorVisited();
		unsigned int countFromFloor = (lastDir - dir)/2;

		for (int i = countFromFloor; dir == 1 ? (i >= 0) : (i < NFLOORS); i -= dir){
			if (order_getElevPanelFlag(i) || order_getFloorPanelFlag(i,UP) || order_getFloorPanelFlag(i,DOWN)){
				pos_invertCurrentDirection();
				state = STATE_CHECK_ELEVATOR_ACTIONS;
				return;				
			}
		}


		/*
		if(lastDir == 1){
			if (dir == 1){ //check below
				for (int i = lastFloor; i >= 0; i--){
					if (order_getElevPanelFlag(i) || order_getFloorPanelFlag(i,UP) || order_getFloorPanelFlag(i,DOWN)){
						orderFound = true;
					}
				}
			}
			else{ //check above
				for (int i = lastFloor+1; i < NFLOORS; i++){
					if (order_getElevPanelFlag(i) || order_getFloorPanelFlag(i,UP) || order_getFloorPanelFlag(i,DOWN)){
						orderFound = true;
					}
				}
			}
		}
		else{
			if (dir == 1){ //check below
				for (int i = lastFloor-1; i >= 0; i--){
					if (order_getElevPanelFlag(i) || order_getFloorPanelFlag(i,UP) || order_getFloorPanelFlag(i,DOWN)){
						orderFound = true;
					}
				}
			}
			else{ //check above
				for (int i = lastFloor; i < NFLOORS; i++){
					if (order_getElevPanelFlag(i) || order_getFloorPanelFlag(i,UP) || order_getFloorPanelFlag(i,DOWN)){
						orderFound = true;
					}
				}
			}			
		}
		if (orderFound == true){
			pos_invertCurrentDirection();
			state = STATE_CHECK_ELEVATOR_ACTIONS;
			return;
		}	
		*/	
	}
	state = STATE_CHECK_ELEVATOR_ACTIONS;
}


void fsm_evTimeOut(void){
	elev_set_door_open_lamp(0);
	timer_stop();
	state = STATE_CHECK_ELEVATOR_ACTIONS;
}


void fsm_evNextFloorReached(void){
	int floorSignal = elev_get_floor_sensor_signal();
	elev_set_floor_indicator(floorSignal);
	pos_setLastFloorVisited(floorSignal);
	state = STATE_CHECK_ELEVATOR_ACTIONS;
}
