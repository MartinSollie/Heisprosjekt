#ifndef _ELEV_POSITION_DATA_H_
#define _ELEV_POSITION_DATA_H_

/**
  Direction 1 is up, -1 is down
*/
int pos_getDirectionWhenLeavingLastFloor(void);
void pos_saveDirectionWhenLeavingFloor(int direction);

int pos_getCurrentDirection(void);
void pos_setCurrentDirection(int direction);
void pos_invertCurrentDirection(void); // If the direction is up, set it to down and vice versa


/**
	Floors indexed from 0 to 3
*/
unsigned int pos_getLastFloorVisited(void);
void pos_setLastFloorVisited(unsigned int floor);


#endif