/*
Elevator order storage module
elevOrderData.c
Martin Sollie & Øystein Lasson
*/

#include <assert.h>
#include "elevOrderData.h"
#include "elev.h"

static bool orderingAllowed = true;
static bool elevPanelFlags[NFLOORS] = {0};
static bool floorPanelFlags[NFLOORS][NDIRECTIONS] = {{0}}; // {up flag, down flag} for each floor

bool order_getFloorPanelFlag(unsigned int floor, int direction){
	if (direction == 1){
		return floorPanelFlags[floor][0];
	}
	else{
		return floorPanelFlags[floor][1];
	}
}

bool order_getElevPanelFlag(unsigned int floor){
	return elevPanelFlags[floor];
}

void order_addElevPanelOrder(unsigned int floor){
	if(!elevPanelFlags[floor] && orderingAllowed){
		elevPanelFlags[floor] = true;
		elev_set_button_lamp(BUTTON_COMMAND, floor, 1);
	}
}

void order_addFloorPanelOrder(unsigned int floor, int direction){
	assert(!(floor == 0 && direction == -1));
	assert(!(floor == NFLOORS-1 && direction == 1));

	if (direction == 1 && orderingAllowed){
		floorPanelFlags[floor][0] = true;
		elev_set_button_lamp(BUTTON_CALL_UP, floor, 1);
	}
	else if(direction == -1 && orderingAllowed){
		floorPanelFlags[floor][1] = true;
		elev_set_button_lamp(BUTTON_CALL_DOWN, floor, 1);
	}
}

void order_deleteFloorOrders(unsigned int floor){
	elevPanelFlags[floor] = false;
	floorPanelFlags[floor][0] = false; // up
	floorPanelFlags[floor][1] = false; // down

	//Turn off button lights
	elev_set_button_lamp(BUTTON_COMMAND, floor, 0);
	if(floor != 0){elev_set_button_lamp(BUTTON_CALL_DOWN, floor, 0);}
	if(floor != N_FLOORS-1){elev_set_button_lamp(BUTTON_CALL_UP, floor, 0);}
}

void order_disableAndDeleteOrders(void){
	orderingAllowed = false;
	for(int i = 0; i < NFLOORS; i++){
		order_deleteFloorOrders(i);
	}
}

void order_enableOrdering(void){
	orderingAllowed = true;
}
