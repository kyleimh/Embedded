/*******************************************************************************
 System Interrupts File

  File Name:
    system_int.c

  Summary:
    Raw ISR definitions.

  Description:
    This file contains a definitions of the raw ISRs required to support the
    interrupt sub-system.

  Summary:
    This file contains source code for the interrupt vector functions in the
    system.

  Description:
    This file contains source code for the interrupt vector functions in the
    system.  It implements the system and part specific vector "stub" functions
    from which the individual "Tasks" functions are called for any modules
    executing interrupt-driven in the MPLAB Harmony system.

  Remarks:
    This file requires access to the systemObjects global data structure that
    contains the object handles to all MPLAB Harmony module objects executing
    interrupt-driven in the system.  These handles are passed into the individual
    module "Tasks" functions to identify the instance of the module to maintain.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2011-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <xc.h>
#include <sys/attribs.h>
#include "control.h"
#include "motor.h"
#include "sensor.h"
#include "system_definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector Functions
// *****************************************************************************
// *****************************************************************************
void IntHandlerDrvAdc(void)
{
    outputEvent(ADC_INTERRUPT_OCCURRED);
    
    int i;
    uint16_t irValue = 0;
    /* Clear ADC Interrupt Flag */
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_ADC_1);
    
    /*Read ADC Data*/
    for(i=0;i<16;i++)
        irValue += PLIB_ADC_ResultGetByIndex(ADC_ID_1, i);

    
    irValue = (5461 / (irValue/16)) - 1;
            
            
//    DRV_USART0_WriteByte(irValue >> 8);
//    DRV_USART0_WriteByte(irValue);
    
    BaseType_t xToken; 
    xToken = pdFALSE;
    
    if( xQueueOverwriteFromISR(xQueueADC, (void *)&irValue, &xToken ) != pdPASS )
    {
        outputEvent(ADC_QUEUE_FULL);
    }
    else
    {
        outputEvent(SENT_TO_ADC_QUEUE);
    }
    /* Clear ADC Interrupt Flag */
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_ADC_1);
}




void IntHandlerDrvUsartInstance0(void)
{
BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    //check transmit interrupt
    if (PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT)) {
        //while there are messages in the transmit queue it will 
        //pull a message from the queue and transmit it a byte at a time
        //following the following format
        //1st byte: message id
        //2nd byte: message msg
        //3rd byte: message data1
        //4th byte: message data2
        while (!xQueueIsQueueEmptyFromISR(transmitQueue)) {
            MESSAGE message;
            if (xQueueReceiveFromISR(transmitQueue, &message, &xHigherPriorityTaskWoken) == pdFALSE) {
                outputEvent(TRANSMIT_QUEUE_EMPTY);
            } else {
                DRV_USART0_WriteByte(message.id);
                DRV_USART0_WriteByte(message.msg);
                DRV_USART0_WriteByte(message.data1);
                DRV_USART0_WriteByte(message.data2);
                outputEvent(TRANSMIT_USART_SENT);
            }
        }
        //disable the transmit interrupts once all messages have been sent
        PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
    }
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
    
    
    //check receive interrupt
    if (PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_RECEIVE)) {
        //receives the message a byte at a time following the following format
        //1st byte: message id
        //2nd byte: message msg
        //3rd byte: message data1
        //4th byte: message data2
        MESSAGE received;
        received.id = DRV_USART0_ReadByte();
        received.msg = DRV_USART0_ReadByte();
        received.data1 = DRV_USART0_ReadByte();
        received.data2 = DRV_USART0_ReadByte();
        //send the message to the receive queue to be pulled from
        if (xQueueSendFromISR(controlQueue, &received, &xHigherPriorityTaskWoken) == pdFALSE) {
            outputEvent(CONTROL_QUEUE_FULL);
        } else {
            outputEvent(CONTROL_QUEUE_ITEM_SENT_FROM_USART);
        }
    }
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE);
    
    
    //check error interrupt
    if (PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_ERROR)) {
        //handle errors
    }
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_ERROR);
}

