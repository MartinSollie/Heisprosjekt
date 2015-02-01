#ifndef _ELEV_FSM_H_
#define _ELEV_FSM_H_


typedef enum fsm_states {
	STATE_AT_FLOOR,
	STATE_STOP_BUTTON_PRESSED,
	STATE_STOP_BUTTON_RELEASED,
	STATE_ELEVATOR_OPEN,
	STATE_CONTINUE_MOVING
} state_t;

state_t fsm_getCurrentState();
int fsm_elevInit();
void evStopButtonPressed();
void evStopButtonReleasedAtFloor();
void evStopButtonReleasedBetweenFloors();
void evAtFloor();


#endif