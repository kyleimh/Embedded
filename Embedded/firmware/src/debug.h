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

#define ADC_QUEUE_CREATED                       0
#define ADC_INTERRUPT_OCCURRED                  1
#define SENT_TO_ADC_QUEUE                       2
#define RECEIVED_FROM_ADC_QUEUE                 3
#define ADC_QUEUE_EMPTY                         4

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

/* Error debug codes */
#define TIMER1_NOT_CREATED                      120
#define TIMER1_NOT_STARTED                      121

#define ADC_QUEUE_NOT_CREATED                   130
#define ADC_QUEUE_FULL                          131

#define MOTOR_QUEUE_NOT_CREATED                 140

#define CONTROL_QUEUE_NOT_CREATED               150

#define TRANSMIT_QUEUE_NOT_CREATED              160

#define USART_RECV_BUFFER_EMPTY                 170
#define USART_END_BYTE_NOT_RECVD                171
#define USART_BAD_MESSAGE                       172
#define USART_DEFAULT_EXCEPTION                 173

void outputEvent( uint8_t value  );

#endif	/* DEBUG_H */

