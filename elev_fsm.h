#ifndef _ELEV_FSM_H_
#define _ELEV_FSM_H_


typedef enum fsm_states {
	STATE_AT_FLOOR,
	STATE_STOP_BUTTON_PRESSED,
	STATE_STOP_BUTTON_RELEASED_BETWEEN_FLOORS,
	STATE_ELEVATOR_OPEN,
	STATE_CONTINUE_MOVING
} state_t;

state_t fsm_getCurrentState(void);
void fsm_elevInit(void);
void evStopButtonPressed(void);
void evStopButtonReleasedAtFloor(void);
void evStopButtonReleasedBetweenFloors(void);
void evAtFloor(void);
void evStoppedBetweenFloorFlagSet(void);
void fsm_evTimeOut(void);


#endif
