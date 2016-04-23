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
#define INITIALIZE                              200

#define SENSOR_QUEUE_CREATED                     0
#define SENSOR_QUEUE_FULL                        1
#define SENT_TO_SENSOR_QUEUE                     2
#define SENSOR_QUEUE_ITEM_READY                  3
#define SENSOR_QUEUE_EMPTY                       4

#define TRANSMIT_QUEUE_CREATED                  10
#define TRANSMIT_QUEUE_FULL                     11
#define TRANSMIT_USART_SENT                     12
#define TRANSMIT_QUEUE_ITEM_READY               13
#define TRANSMIT_QUEUE_EMPTY                    14

#define CONTROL_QUEUE_CREATED                   20
#define CONTROL_QUEUE_FULL                      21
#define CONTROL_QUEUE_ITEM_SENT_FROM_USART_INT  22
#define CONTORL_QUEUE_RECEIVED                  23
#define CONTROL_QUEUE_EMPTY                     24

#define MOTOR_QUEUE_CREATED                     30
#define MOTOR_QUEUE_FULL                        31
#define MOTOR_QUEUE_ITEM_SENT                   32
#define MOTOR_QUEUE_RECEIVED                    33
#define MOTOR_QUEUE_EMPTY                       34

#define USART_DATA_NEXT                         40
#define USART_RX_FLAG_THROWN_IN_ISR             41
#define USART_BYTE_RECVD_NEED_MORE_IN_ISR       42
#define USART_ALL_BYTES_RECVD_IN_ISR            43
#define USART_START_BYTE_RECVD_IN_ISR           44

#define ADC_INTERRUPT_OCCURRED                  51

#define LINE_READING_START                      61
#define LINE_CAP_CHARGED                        62
#define LINE_VALID_DATA                         63

/* Error debug codes */
#define LINETIMER_NOT_CREATED                  120
#define LINETIMER_NOT_STARTED                  121
#define READTIMER_NOT_CREATED                  122
#define READTIMER_NOT_STARTED                  123
#define READTIMER_NOT_RESET                    124

#define SENSOR_QUEUE_NOT_CREATED               130

#define MOTOR_QUEUE_NOT_CREATED                140

#define CONTROL_QUEUE_NOT_CREATED              150

#define TRANSMIT_QUEUE_NOT_CREATED             160

#define USART_RECV_BUFFER_EMPTY                170
#define USART_END_BYTE_NOT_RECVD               171
#define USART_BAD_MESSAGE                      172
#define USART_DEFAULT_EXCEPTION                173

void outputEvent( uint8_t value  );
void writeDebug( uint8_t value  );
void debugToUART(uint8_t value);

#endif	/* DEBUG_H */

