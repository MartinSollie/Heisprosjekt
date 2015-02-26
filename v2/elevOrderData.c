#include <stdio.h>
#include "elevOrderData.h"

static bool orderingAllowed = true;
static bool elevPanelFlags[NFLOORS] = {0};
static bool floorPanelFlags[NFLOORS][NDIRECTIONS] = {{0}}; 

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
	if(!elevPanelFlags[floor] && (orderingAllowed == true)){
		elevPanelFlags[floor] = true;
		printf("ElevPanelOrder added for floor %d \n", floor);
	}
	
}
void order_addFloorPanelOrder(unsigned int floor, int direction){
	if (direction == 1 && (floorPanelFlags[floor][0] == false) && (orderingAllowed == true)){
		floorPanelFlags[floor][0] = true;
		printf("FloorPanelOrder added for floor %d, ", floor);
		printf("in direction %d \n", direction);
	}
	else if(direction == -1 && (floorPanelFlags[floor][1] == false) && (orderingAllowed == true)){
		floorPanelFlags[floor][1] = true;
		printf("FloorPanelOrder added for floor %d, ", floor);
		printf("in direction %d \n", direction);
	}
	
}

void order_deleteFloorOrders(unsigned int floor){
	elevPanelFlags[floor] = false;
	floorPanelFlags[floor][0] = false;
	floorPanelFlags[floor][1] = false;
	printf("Orders deleted for floor %d \n", floor);
}

void order_deactivateAndDeleteOrders(void){
	orderingAllowed = false;
	for(int i = 0; i < NFLOORS; i++){
		deleteFloorOrders(i);
	}
	printf("Orders are not allowed \n");
}

void order_activateOrdering(void){
	orderingAllowed = true;
	printf("Orders are allowed \n");
}
