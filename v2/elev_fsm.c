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

//
static bool checkRequestAhead(void);
static bool checkRequestStop(void);
static bool checkRequestOppositeDirection(void);


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

	// Guard conditions
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
	} // end of guard conditions

	// Determine next elevator action based on current requests
	if (checkRequestStop()) return;
	if (checkRequestAhead()) return;
	if (checkRequestOppositeDirection()) return;
	state = STATE_CHECK_ELEVATOR_ACTIONS; //If no requests are found
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


static bool checkRequestStop(void){
	// Check for request(s) to get off, or on in current direction here
	if (elev_get_floor_sensor_signal() != -1){ //Elevator is at a floor
		unsigned int currentFloor = pos_getLastFloorVisited();
		int dir = pos_getCurrentDirection();
		if((order_getElevPanelFlag(currentFloor) == true) || (order_getFloorPanelFlag(currentFloor,dir) == true)){
			elev_set_motor_direction(DIRN_STOP);
			elev_set_door_open_lamp(1);
			timer_start();

			//Delete orders for this floor
			order_deleteFloorOrders(currentFloor);
			elev_set_button_lamp(BUTTON_COMMAND, currentFloor, 0);
			if(currentFloor != NFLOORS-1){elev_set_button_lamp(BUTTON_CALL_UP, currentFloor, 0);}
			if(currentFloor != 0){elev_set_button_lamp(BUTTON_CALL_DOWN, currentFloor, 0);}
			
			state = STATE_ELEVATOR_OPEN;
			return true;
		}
	}
	return false;
}

static bool checkRequestAhead(void){
	// Check for request(s) ahead
	int dir = pos_getCurrentDirection();
	if (elev_get_floor_sensor_signal() != -1){ 
		// If direction is up: check floors above, if direction is down: check floors below
		for (int i = pos_getLastFloorVisited()+dir; ((i < NFLOORS) && (i >= 0)); i += dir){
			if(order_getElevPanelFlag(i) || order_getFloorPanelFlag(i,UP) || order_getFloorPanelFlag(i,DOWN)){
				pos_saveDirectionWhenLeavingFloor(dir);
				elev_set_motor_direction(dir);
				state = STATE_MOVING;
				return true;
			}
		}
	}

	else{ //Elevator is between floors after pressing and releasing stop button while moving
		int lastDir = pos_getDirectionWhenLeavingLastFloor();
		unsigned int lastFloor = pos_getLastFloorVisited();

		unsigned int searchFromFloor;
		if (dir == 1 && lastDir == 1){ // Above last floor, current direction is up 
			searchFromFloor = lastFloor+1;
		}
		else if(dir == 1 && lastDir == -1){ // Below last floor, current direction is up 
			searchFromFloor = lastFloor;
		}
		else if(dir == -1 && lastDir == 1){ // Above last floor, current direction is down 
			searchFromFloor = lastFloor;
		}
		else{ // Below last floor, current direction is down
			searchFromFloor = lastFloor-1;
		}

		for (int i = searchFromFloor; ((i < NFLOORS) && (i >= 0)); i += dir){
			if(order_getElevPanelFlag(i) || order_getFloorPanelFlag(i,UP) || order_getFloorPanelFlag(i,DOWN)){
				elev_set_motor_direction(dir);
				state = STATE_MOVING;
				return true;
			}
		}
	}
	return false;
}

static bool checkRequestOppositeDirection(void){
	//Any request behind, or in opposite direction here?
	int dir = pos_getCurrentDirection();
	unsigned int lastFloor = pos_getLastFloorVisited();
	if(elev_get_floor_sensor_signal() != -1){
		for(int i = lastFloor; ((i < NFLOORS) && (i >= 0)); i -= dir){ // Iterate in opposite direction of dir
			if ((i != lastFloor && (order_getElevPanelFlag(i) || order_getFloorPanelFlag(i,UP) || order_getFloorPanelFlag(i,DOWN))) || (i == lastFloor && order_getFloorPanelFlag(i,-dir))){
				pos_invertCurrentDirection();
				state = STATE_CHECK_ELEVATOR_ACTIONS;
				return true;
			}
		}
	}


	else{ //Elevator is between floors after releasing stop button
		// check for orders in opposite direction
		int lastDir = pos_getDirectionWhenLeavingLastFloor();

		unsigned int searchFromFloor;

		if (dir == 1 && lastDir == 1){ // Above last floor, current direction is up 
			searchFromFloor = lastFloor;
		}
		else if (dir == -1 && lastDir == -1){ // Below last floor, current direction is down
			searchFromFloor = lastFloor;
		}
		else if(dir == 1 && lastDir == -1){ // Below last floor, current direction is up 
			searchFromFloor = lastFloor-1;
		}
		else{ // Above last floor, current direction is down 
			searchFromFloor = lastFloor+1;
		}


		for (int i = searchFromFloor; ((i < NFLOORS) && (i >= 0)); i -= dir){
			if (order_getElevPanelFlag(i) || order_getFloorPanelFlag(i,UP) || order_getFloorPanelFlag(i,DOWN)){
				pos_invertCurrentDirection();
				state = STATE_CHECK_ELEVATOR_ACTIONS;
				return true;				
			}
		}	
	}
	return false;
}




