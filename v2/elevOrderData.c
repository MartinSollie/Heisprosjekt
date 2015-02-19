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
	if(!elevPanelFlags[floor] && (orderingAllowed == true)){
		elevPanelFlags[floor] = true;
		printf("ElevPanelOrder added for floor %d \n", floor);
	}
	
}
void addFloorPanelOrder(unsigned int floor, int direction){
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

void deleteFloorOrders(unsigned int floor){
	elevPanelFlags[floor] = false;
	floorPanelFlags[floor][0] = false;
	floorPanelFlags[floor][1] = false;
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
