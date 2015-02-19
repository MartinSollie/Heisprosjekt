#ifndef _ELEV_FSM_H_
#define _ELEV_FSM_H_

#define UP 1
#define DOWN -1

typedef enum fsm_states {
	STATE_CHECK_ELEVATOR_ACTIONS,
	STATE_STOP_BUTTON_PRESSED,
	STATE_STOP_BUTTON_RELEASED_AT_FLOOR,
	STATE_STOP_BUTTON_RELEASED_BETWEEN_FLOORS,
	STATE_ELEVATOR_OPEN,
	STATE_CONTINUE_MOVING
} state_t;

state_t fsm_getCurrentState(void);
void fsm_evSystemStarted(void);

void fsm_evStopButtonPressed(void);
void fsm_evStopButtonReleasedAtFloor(void);
void fsm_evStopButtonReleasedBetweenFloors(void);
void fsm_evReadyToCheckActions(void);
void fsm_evTimeOut(void);
void fsm_evNextFloorReached(void);

#endif