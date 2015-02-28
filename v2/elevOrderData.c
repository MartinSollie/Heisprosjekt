#include <assert.h>
#include "elevOrderData.h"

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
	}
}

void order_addFloorPanelOrder(unsigned int floor, int direction){
	assert(floor != 0 && direction != -1);
	assert(floor != NFLOORS-1 && direction != 1);

	if (direction == 1 && orderingAllowed){
		floorPanelFlags[floor][0] = true;
	}
	else if(direction == -1 && orderingAllowed){
		floorPanelFlags[floor][1] = true;
	}
}

void order_deleteFloorOrders(unsigned int floor){
	elevPanelFlags[floor] = false;
	floorPanelFlags[floor][0] = false; // down
	floorPanelFlags[floor][1] = false; // up
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
