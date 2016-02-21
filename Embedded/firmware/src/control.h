#ifndef _CONTROL_H
#define _CONTROL_H

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
	CONTROL_STATE_INIT=0,
    CONTROL_STATE_RUN
} CONTROL_STATES;


typedef struct
{
    CONTROL_STATES state;
} CONTROL_DATA;

void CONTROL_Initialize ( void );
void CONTROL_Tasks( void );

#endif /* _CONTROL_H */