#include "elevOrderData.h"

static bool floorPanelFlags[N_FLOORS][2] = {0}; //[up,down] in sub-array
static bool elevPanelFlags[N_FLOORS] = {0};
static bool orderingAllowed = true;

bool getFloorPanelFlag(unsigned int floor, direction_t direction){
	return floorPanelFlags[floor][direction];
}

bool getElevPanelFlag(unsigned int floor){
	return elevPanelFlags[floor];
}

void addElevPanelOrder(unsigned int floor){
	if(orderingAllowed && !getElevPanelFlag(floor)){
		elevPanelFlags[floor] = 1;
		printf("Elevator panel order for floor %d added.\n",floor)
	}
}

void addFloorPanelOrder(unsigned int floor, direction_t direction){
	if(orderingAllowed && !getFloorPanelFlag(floor,direction)){
		floorFlags[floor][direction] = 1;
		printf("Floor panel order for floor %d ",floor);
		printf("in direction %d added.\n", direction);
	}
}

void deleteFloorOrders(unsigned int floor){
	elevPanelFlags[floor] = 0;
	floorPanelFlags[floor][DIRN_UP] = 0;
	floorPanelFlags[floor][DIRN_DOWN] = 0;
	printf("Orders for floor %d deleted\n", floor);
}

void deactivateAndDeleteOrders(void){
	orderingAllowed = false;
	for (int floor = 0; floor < 4; floor++){
		deleteOrder(floor);
	}
}
void activateOrders(void){
	orderingAllowed = true;
}
