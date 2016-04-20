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


static const int DEFUALT_SPEED = 2000;
static const int OFFSET = 5;
static bool inst_setup;     //keeps track if instruction is set up
static int Displace[2]; 

typedef enum 
{
    DISTANCE = 0, 
            DEGREE = 1
} DISPLACE;
typedef struct
{
    MOTOR_STATES state;
    int curState;
    int R_speed;
    int L_speed;
} MOTOR_DATA;

MOTOR_DATA motor_data;

//Set up pins and ports to talk to motors and rovers
void INIT_Rover_Motors();

//Basic forward and backward movement
void move_Forward();
void move_Backward();
void setSpeed(int left_speed, int right_speed);
void move_Stop();       //stops all rovers
void checkInstDone(DISPLACE disOrdeg);
//Handles turning the rovers, keep turning until a stop is called
void move_Turn_Right();
void move_Turn_Left();
//void setMotorState(MESSAGE message);
void executeState();
void sendDone(int disOrdeg);
void setDisplacement(DISPLACE disOrDeg, int value);

void test(int speed);

//Harmony functions for the Motor app
void MOTOR_Initialize ( void );
void MOTOR_Tasks( void );

#endif /* _MOTOR_H */