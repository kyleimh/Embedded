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
bool intersect;
bool L2, L1, C, R1, R2;
bool stopped;

typedef struct
{
    SENSOR_STATES state;
} SENSOR_DATA;

void SENSOR_Initialize ( void );
void SENSOR_Tasks( void );

#endif /* _SENSOR_H */