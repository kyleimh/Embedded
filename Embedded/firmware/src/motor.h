#ifndef _MOTOR_H
#define _MOTOR_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#include "app_public.h"

typedef enum
{
	/* Application's state machine's initial state. */
	MOTOR_STATE_INIT=0,
    MOTOR_STATE_RUN
} MOTOR_STATES;


typedef struct
{
    MOTOR_STATES state;
} MOTOR_DATA;

void MOTOR_Initialize ( void );
void MOTOR_Tasks( void );

#endif /* _MOTOR_H */