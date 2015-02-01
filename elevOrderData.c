#include <stdbool.h>

static bool floorFlags[4][2]; //[up/down] in sub-array
for (int i = 0; i < 4; i++){
	for (int j = 0; j < 2, j++){
		floorFlags[i][j] = 0;
	}
} // MÅ HA ENTEN ETT FLAGG PER KNAPP (10 elementer) ELLER TRIKSE LITT

bool getFloorFlag(unsigned int floor, direction_t direction){
	if (direction == DIRN_UP){
		return floorFlags[floor][0];
	}
	else{
		return floorFlags[floor][1];
	}
}
void addElevPanelOrder(unsigned int floor){
	floorFlags[floor][0] = 1;
	floorFlags[floor][1] = 1;
}
void addFloorPanelOrder(unsigned int floor, direction_t direction){
	if(direction == DIRN_UP){
		floorFlags[floor][0] = 1;
	}
	else{
		floorFlags[floor][1] = 1;
	}
}

void deleteOrder(unsigned int floor);
void deactivateAndDeleteOrders(void);
void activateOrders(void);
position_t getCurrentPosition(void);
void setCurrentPosition(position_t position);
direction_t getCurrentDirection(void);
void setCurrentDirection(direction_t direction);