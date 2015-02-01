#ifndef ELEV_DATA_ORDER_H
#define ELEV_DATA_ORDER_H

#include <stdio.h>

typedef enum dir{
	DIRN_UP,
	DIRN_DOWN
} direction_t;

typedef enum pos{
	FIRST,
	SECOND,
	THIRD,
	FOURTH,
	FIRST_SECOND, //Between floors
	SECOND_THIRD,
	THIRD_FOURTH
} position_t;

bool getFloorFlag(unsigned int floor, direction_t direction);
void addElevPanelOrder(unsigned int floor);
void addFloorPanelOrder(unsigned int floor, direction_t direction);
void deleteOrder(unsigned int floor);
void deactivateAndDeleteOrders(void);
void activateOrders(void);
position_t getCurrentPosition(void);
void setCurrentPosition(position_t position);
direction_t getCurrentDirection(void);
void setCurrentDirection(direction_t direction);


#endif