#include <stdio.h>
#include "elevPositionData.h"

static int currentDirection; // Current defined direction used for controlling elevator movement.
static int directionWhenLeavingLastFloor;
static unsigned int lastFloorVisited;

int pos_getDirectionWhenLeavingLastFloor(void){
	return directionWhenLeavingLastFloor;
}

void pos_saveDirectionWhenLeavingFloor(int direction){
	directionWhenLeavingLastFloor = direction;
}


int pos_getCurrentDirection(void){
	return currentDirection;
}

void pos_setCurrentDirection(int direction){
	currentDirection = direction;
}

void pos_invertCurrentDirection(void){
	if(currentDirection == 1){
		currentDirection = -1;
	}
	else{
		currentDirection = 1;
	}
}


unsigned int pos_getLastFloorVisited(void){
	return lastFloorVisited;
}

void pos_setLastFloorVisited(unsigned int floor){
	lastFloorVisited = floor;
}
