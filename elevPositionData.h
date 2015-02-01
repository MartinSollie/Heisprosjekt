#ifndef _ELEV_DATA_ORDER_H_
#define _ELEV_DATA_ORDER_H_

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

position_t getCurrentPosition(void);
void setCurrentPosition(position_t pos);
direction_t getCurrentDirection(void);
void setCurrentDirection(direction_t dir);

#endif