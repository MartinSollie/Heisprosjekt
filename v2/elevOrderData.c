#include <stdio.h>
#include "elevOrderData.h"

static bool orderingAllowed = true;
static bool elevPanelFlags[NFLOORS] = {0};
static bool floorPanelFlags[NFLOORS][NDIRECTIONS] = {{0}}; 

bool getFloorPanelFlag(unsigned int floor, int direction){
	if (direction == 1){
		return floorPanelFlags[floor][0];
	}
	else{
		return floorPanelFlags[floor][1];
	}
}

bool getElevPanelFlag(unsigned int floor){
	return elevPanelFlags[floor];
}

void addElevPanelOrder(unsigned int floor){
	if(!elevPanelFlags[floor]){
		elevPanelFlags[floor] = true;
		elev_set_button_lamp(BUTTON_COMMAND, floor, 1);
		printf("ElevPanelOrder added for floor %d \n", floor);
	}
	
}
void addFloorPanelOrder(unsigned int floor, int direction){
	if (direction == 1 && !floorPanelFlags[floor][0]){
		floorPanelFlags[floor][0] = true;
		elev_set_button_lamp(BUTTON_CALL_UP, floor, 1);
	}
	else if(!floorPanelFlags[floor][1]){
		floorPanelFlags[floor][1] = true;
		elev_set_button_lamp(BUTTON_CALL_DOWN, floor, 1);
	}
	printf("FloorPanelOrder added for floor %d, ", floor);
	printf("in direction %d \n", direction);
}

void deleteFloorOrders(unsigned int floor){
	elevPanelFlags[floor] = 0;
	floorPanelFlags[floor][0] = 0;
	floorPanelFlags[floor][1] = 0;
	elev_set_button_lamp(BUTTON_COMMAND, floor, 0);
	elev_set_button_lamp(BUTTON_CALL_UP, floor, 0);
	elev_set_button_lamp(BUTTON_CALL_DOWN, floor, 0);
	printf("Orders deleted for floor %d \n", floor);
}

void deactivateAndDeleteOrders(void){
	orderingAllowed = false;
	for(int i = 0; i < NFLOORS; i++){
		deleteFloorOrders(i);
	}
	printf("Orders are not allowed \n");
}

void activateOrdering(void){
	orderingAllowed = true;
	printf("Orders are now allowed \n");
}