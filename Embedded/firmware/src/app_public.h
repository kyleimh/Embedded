/* 
 * File:   app_public.h
 * Author: Kyle
 *
 * 
 */

#ifndef APP_PUBLIC_H
#define	APP_PUBLIC_H

#include "FreeRTOS.h" /* Necessary to include before including "timers.h" */
#include "queue.h"
#include "debug.h"
#include "timers.h"

QueueHandle_t transmitQueue;
QueueHandle_t xQueueADC;
QueueHandle_t controlQueue;
QueueHandle_t motorQueue;

TimerHandle_t xTimer1;
void vTimerCallback( TimerHandle_t pxTimer );

typedef struct {
    //TODO:
    uint8_t id;
    //TODO:
    uint8_t msg;
    //TODO:
    uint8_t data1;
    //TODO:
    uint8_t data2;
} MESSAGE;

#endif	/* APP_PUBLIC_H */

