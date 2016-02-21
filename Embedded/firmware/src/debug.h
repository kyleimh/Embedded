/* 
 * File:   debug.h
 * Author: Kyle
 *
 */

#ifndef DEBUG_H
#define	DEBUG_H

#include "FreeRTOS.h"
#include "system_config/default/system_definitions.h"

/* Success debug codes */
#define INITIALIZE 67

#define ADC_QUEUE_CREATED 0
#define ADC_INTERRUPT_OCCURRED 1
#define SENT_TO_ADC_QUEUE 2
#define RECEIVED_FROM_ADC_QUEUE 3
#define ADC_QUEUE_EMPTY 4

#define TRANSMIT_QUEUE_CREATED 10
#define TRANSMIT_QUEUE_FULL 11
#define TRANSMIT_USART_SENT 12
#define TRANSMIT_QUEUE_ITEM_READY 13
#define TRANSMIT_QUEUE_EMPTY 14

#define CONTROL_QUEUE_CREATED 20
#define CONTROL_QUEUE_FULL 21
#define CONTROL_QUEUE_ITEM_SENT_FROM_USART 22
#define CONTORL_QUEUE_RECEIVED 23
#define CONTROL_QUEUE_EMPTY 24

#define MOTOR_QUEUE_CREATED 30
#define MOTOR_QUEUE_FULL 31
#define MOTOR_QUEUE_ITEM_SENT 32
#define MOTOR_QUEUE_RECEIVED 33
#define MOTOR_QUEUE_EMPTY 34

/* Error debug codes */
#define ADC_QUEUE_FULL 120
#define ADC_QUEUE_NOT_CREATED 121
#define MOTOR_QUEUE_NOT_CREATED 122
#define CONTROL_QUEUE_NOT_CREATED 123
#define TRANSMIT_QUEUE_NOT_CREATED 124
#define TIMER1_NOT_CREATED         125
#define TIMER1_NOT_STARTED         126

void outputEvent( uint32_t value  );

#endif	/* DEBUG_H */

