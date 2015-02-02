#ifndef _ELEV_ORDER_DATA_H_
#define _ELEV_ORDER_DATA_H_

#include <stdbool.h>
#include "elevPositionData.h" /////fyfyfyfy, lag egen type for retning

bool getFloorPanelFlag(unsigned int floor, direction_t direction);
bool getElevPanelFlag(unsigned int floor);
void addElevPanelOrder(unsigned int floor);
void addFloorPanelOrder(unsigned int floor, direction_t direction);
void deleteFloorOrders(unsigned int floor);
void deactivateAndDeleteOrders(void);
void activateOrdering(void);


#endif
