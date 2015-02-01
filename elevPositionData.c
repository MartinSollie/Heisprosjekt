#include "elevPositionData.h"

static direction_t direction;
static position_t position;

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