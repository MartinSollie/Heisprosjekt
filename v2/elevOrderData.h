#ifndef _ELEV_ORDER_DATA_H_
#define _ELEV_ORDER_DATA_H_

#define NFLOORS 4 // Number of floors
#define NDIRECTIONS 2 

#include <stdbool.h>


bool order_getFloorPanelFlag(unsigned int floor, int direction); // Outside elevator
bool order_getElevPanelFlag(unsigned int floor); // Inside elevator

void order_addElevPanelOrder(unsigned int floor);
void order_addFloorPanelOrder(unsigned int floor, int direction);

void order_deleteFloorOrders(unsigned int floor); // Clears all orders for the given floor

void order_deactivateAndDeleteOrders(void); // Deletes all orders, and blocks any new orders
void order_activateOrdering(void); // Allow new orders

#endif