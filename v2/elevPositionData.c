#include "elevPositionData.h"

static int currentDirection;
static int directionWhenLeavingLastFloor;
static unsigned int lastFloorVisited;



int getDirectionWhenLeavingLastFloor(void){
	return directionWhenLeavingLastFloor;
}

void saveDirectionWhenLeavingFloor(int direction){
	directionWhenLeavingLastFloor = direction;
}


int getCurrentDirection(void){
	return currentDirection;
}

void setCurrentDirection(int direction){
	currentDirection = direction;
}

void invertCurrentDirection(void){
	if(currentDirection == 1){
		currentDirection = -1;
	}
	else{
		currentDirection = 1;
	}
	printf("Inverting direction\n");

}


unsigned int getLastFloorVisited(void){
	return lastFloorVisited;
}

void setLastFloorVisited(int floor){
	lastFloorVisited = floor;
}