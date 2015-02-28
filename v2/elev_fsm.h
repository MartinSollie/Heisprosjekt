#ifndef _ELEV_FSM_H_
#define _ELEV_FSM_H_

// Elevator directions
#define UP 1
#define DOWN -1

typedef enum fsm_states {
	STATE_CHECK_ELEVATOR_ACTIONS, /////// FIND NEW NAME!!!!!!!
	STATE_STOP_BUTTON_PRESSED,
	STATE_STOP_BUTTON_RELEASED_AT_FLOOR,
	STATE_STOP_BUTTON_RELEASED_BETWEEN_FLOORS,
	STATE_ELEVATOR_OPEN,
	STATE_MOVING
} state_t;

state_t fsm_getCurrentState(void);

// Event functions
void fsm_evSystemStarted(void); // Initialization event
void fsm_evStopButtonPressed(void);
void fsm_evStopButtonReleasedAtFloor(void);
void fsm_evStopButtonReleasedBetweenFloors(void);
void fsm_evReadyToCheckActions(void); /////////CHANGE TO: fsm_evCheckElevActions()
void fsm_evTimeOut(void); // Door has been open 3 seconds
void fsm_evNextFloorReached(void);

#endif