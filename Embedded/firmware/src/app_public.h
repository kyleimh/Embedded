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

#define MSG_START 0x02
#define MSG_END   0x03

#define enableLineSensor 1
#define enableADC        0

#define true  1
#define false 0

//keep encoder count
volatile unsigned int R_encoder;
volatile unsigned int L_encoder;


static bool Motor_Test = false; 
typedef enum {
    STOP = 0,
    FORWARD = 11, //B
    BACKWARD = 12, //C
    TURN_RIGHT = 13, //D
    TURN_LEFT  = 14, //E
            TEST_ON = 21, 
            TEST_OFF = 20,
    DONE       = 100
} MOTOR_MESSAGE;

typedef enum {  
    S_SLOW = 0,
    SLOW = 200, 
    AVERAGE = 400,
    FAST = 600,
    S_FAST = 800            
} MOTOR_SPEED;


typedef enum {
    SENSOR_TO_MOTOR = 23,
} COMMUNICATION;

bool validData;
static bool line_correction;
uint8_t debugScheme;

QueueHandle_t transmitQueue;
QueueHandle_t sensorQueue;
QueueHandle_t controlQueue;
QueueHandle_t motorQueue;

TimerHandle_t lineTimer;
TimerHandle_t readTimer;
TimerHandle_t xTimer1;

void vTimerCallback(TimerHandle_t pxTimer );
void lineTimerCallback( TimerHandle_t pxTimer );
void readTimerCallback( TimerHandle_t pxTimer );

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

