#include "elevPositionData.h"

static direction_t direction;
static position_t position;
unsigned int lastFloorVisited;

position_t getCurrentPosition(void){
	return position;
}

void setCurrentPosition(position_t pos){
	position = pos;
	return;
}

direction_t getCurrentDirection(void){
	return direction;
}

void setCurrentDirection(direction_t dir){
	direction = dir;
	return;
}

void invertCurrentDirection(void){
	if(direction == DIR_UP){
		direction = DIR_DOWN;
	}
	else{
		direction = DIR_UP;
	}
}

void setLastFloorVisited(int floor){
	lastFloorVisited = floor;
}

unsigned int getLastFloorVisited(void){
	return lastFloorVisited;
}