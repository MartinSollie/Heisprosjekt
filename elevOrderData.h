#ifndef _ELEV_DATA_ORDER_H_
#define _ELEV_DATA_ORDER_H_

#include <stdio.h>
#include <stdbool.h>

bool getFloorPanelFlag(unsigned int floor, direction_t direction);
void addElevPanelOrder(unsigned int floor);
void addFloorPanelOrder(unsigned int floor, direction_t direction);
void deleteFloorOrders(unsigned int floor);
void deactivateAndDeleteOrders(void);
void activateOrders(void);
position_t getCurrentPosition(void);
void setCurrentPosition(position_t position);
direction_t getCurrentDirection(void);
void setCurrentDirection(direction_t direction);


#endif