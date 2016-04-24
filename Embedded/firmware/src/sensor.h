#ifndef _SENSOR_H
#define _SENSOR_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#include "app_public.h"

typedef enum
{
	SENSOR_STATE_INIT=0,
    SENSOR_STATE_RUN
} SENSOR_STATES;

bool raw_cm;
bool push;
bool wait;
bool safe;
bool line_correction;

typedef enum
{
    NONE = 0,
	INTERSECT = 10,
    STOPPED = 20,
    OFF_LINE = 30,
    L2 = 40,
    L1 = 50,
    C = 60,
    R1 = 70,
    R2 = 80            
} LINE_STATE;

LINE_STATE lineState;
uint8_t prevLine;

typedef struct
{
    SENSOR_STATES state;
} SENSOR_DATA;

void SENSOR_Initialize ( void );
void SENSOR_Tasks( void );

#endif /* _SENSOR_H */