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
    CONTROL_STATE_RUN=1,
    CONTROL_STATE_ECHO=2,
    CONTROL_STATE_STANDBY=3,
    CONTROL_STATE_DELIVER=4,
    CONTROL_STATE_RETURN=5,
    CONTROL_STATE_CLEANUP=6,
    CONTROL_STATE_PASS=7,
    CONTROL_STATE_TEST=8
} CONTROL_STATES;


typedef enum
{
    CONTROL_MODE_CLEAN=0,
    CONTROL_MODE_DELIVER=1
} CONTROL_MODES;

typedef enum
{
    CONTROL_MSG_PRIMARY_STATE=0xaa,
    CONTROL_MSG_SECONDARY_STATE=0xab,
    CONTROL_MSG_RESET=0xba,
    CONTROL_MSG_TEST=0xbb
} CONTROL_MSGS;

typedef struct
{
    CONTROL_STATES state;
    CONTROL_MODES mode;
    uint8_t delivery_turn;
    int delivery_intctn_ct;
    int return_turns;
    int return_turns_passed;
    bool out;
} CONTROL_DATA;


void CONTROL_Initialize ( void );
void CONTROL_Tasks( void );

#endif /* _CONTROL_H */