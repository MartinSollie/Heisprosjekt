#ifndef _ELEV_POSITION_DATA_H_
#define _ELEV_POSITION_DATA_H_

int getDirectionWhenLeavingLastFloor(void);
void saveDirectionWhenLeavingFloor(int direction);

int getCurrentDirection(void);
void setCurrentDirection(int direction);
void invertCurrentDirection(void);

unsigned int getLastFloorVisited(void);
void setLastFloorVisited(int floor);


#endif