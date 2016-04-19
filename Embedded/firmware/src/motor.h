#ifndef _MOTOR_H
#define _MOTOR_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#include "app_public.h"

#include "peripheral/ports/plib_ports.h"
#include "peripheral/tmr/plib_tmr.h"
#include "peripheral/oc/plib_oc.h"

typedef enum
{
	MOTOR_STATE_INIT=0,
    MOTOR_STATE_RUN
} MOTOR_STATES;


void sendDone(int disOrdeg);
void setDegree(int degree);
void setDistance(int dis);
void setSpeed(int speed);
int Mv_degree;
int Mv_distance;
int Mv_speed;

typedef struct
{
    MOTOR_STATES state;
} MOTOR_DATA;

MOTOR_DATA motor_data;

//Set up pins and ports to talk to motors and rovers
void INIT_Rover_Motors();

//Basic forward and backward movement
void move_Forward(int speed_R, int speed_L);
void move_Backward(int speed_R, int speed_L);
void move_Stop();       //stops all rovers

//Handles turning the rovers, keep turning until a stop is called
void move_Turn_Right(int speed);
void move_Turn_Left(int speed);

void test(int speed);

int get_Distance(int ticks);

//Harmony functions for the Motor app
void MOTOR_Initialize ( void );
void MOTOR_Tasks( void );

#endif /* _MOTOR_H */