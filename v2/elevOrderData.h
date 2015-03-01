/*
Elevator order storage module
elevOrderData.h
Martin Sollie & Øystein Lasson
*/

#ifndef _ELEV_ORDER_DATA_H_
#define _ELEV_ORDER_DATA_H_

#define NFLOORS 4 // Number of floors
#define NDIRECTIONS 2 

#include <stdbool.h>

// Floors are indexed from 0 to 3

// Order flags for buttons outside elevator
bool order_getFloorPanelFlag(unsigned int floor, int direction); 
void order_addFloorPanelOrder(unsigned int floor, int direction);

// Order flags for buttons inside elevator
bool order_getElevPanelFlag(unsigned int floor);
void order_addElevPanelOrder(unsigned int floor);


void order_deleteFloorOrders(unsigned int floor); // Clear all flags for the parameter floor

void order_disableAndDeleteOrders(void); // Deletes all orders, and blocks any new orders
void order_enableOrdering(void); // Allow new orders

#endif