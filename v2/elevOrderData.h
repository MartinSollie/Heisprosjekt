#ifndef _ELEV_ORDER_DATA_H_
#define _ELEV_ORDER_DATA_H_

#define NFLOORS 4
#define NDIRECTIONS 2

#include <stdbool.h>
bool getFloorPanelFlag(unsigned int floor, int direction);
bool getElevPanelFlag(unsigned int floor);

void addElevPanelOrder(unsigned int floor);
void addFloorPanelOrder(unsigned int floor, int direction);

void deleteFloorOrders(unsigned int floor);

void deactivateAndDeleteOrders(void);
void activateOrdering(void);

#endif