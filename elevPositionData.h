#ifndef _ELEV_POSITION_DATA_H_
#define _ELEV_POSITION_DATA_H_

typedef enum dir{
	DIR_UP,
	DIR_DOWN
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
void invertCurrentDirection(void); ////

#endif
