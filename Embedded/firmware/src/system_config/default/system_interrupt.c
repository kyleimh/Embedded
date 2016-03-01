#include <xc.h>
#include <sys/attribs.h>
#include "control.h"
#include "motor.h"
#include "sensor.h"
#include "system_definitions.h"

short bytesRecvd = 0;
uint8_t recvByte;
MESSAGE sending;
MESSAGE received;

void IntHandlerDrvAdc(void)
{
    //outputEvent(ADC_INTERRUPT_OCCURRED);
    
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
    
    /* Check the transmit flag */
    if (PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT)) 
    {
        while (!xQueueIsQueueEmptyFromISR(transmitQueue)) 
        {
            if (xQueueReceiveFromISR(transmitQueue, &sending, &xHigherPriorityTaskWoken) == pdFALSE) 
            {
                outputEvent(TRANSMIT_QUEUE_EMPTY);
            }
            else
            {
                DRV_USART0_WriteByte(sending.id);
                DRV_USART0_WriteByte(sending.msg);
                DRV_USART0_WriteByte(sending.data1);
                DRV_USART0_WriteByte(sending.data2);
                outputEvent(TRANSMIT_USART_SENT);
            }
        }
        /* Disable the transmit interrupts once all messages have been sent */
        PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
        
        /* Clear pending interrupt */
        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
    }
    
    /* Check the receive flag */
    if (PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_RECEIVE)) 
    {
        recvByte = (uint8_t)DRV_USART0_ReadByte();
        if( (recvByte == (uint8_t)MSG_START) && (DRV_USART0_ReceiverBufferIsEmpty()) && bytesRecvd == 0)
        {
            bytesRecvd++;
        }
        else if( (bytesRecvd>0) && (bytesRecvd<5) && (DRV_USART0_ReceiverBufferIsEmpty()) )
        {
            if( recvByte == (uint8_t)MSG_START )
            {
                bytesRecvd = 1;
                goto clearInt;
            }
            if( recvByte == (uint8_t)MSG_END )
            {
                outputEvent(USART_BAD_MESSAGE);
                bytesRecvd = 0;
                goto clearInt;
            }
            outputEvent(USART_DATA_NEXT);
            outputEvent(recvByte);
            outputEvent(USART_DATA_NEXT);
            outputEvent(bytesRecvd);
            switch(bytesRecvd)
            {
                case 1:
                    received.id    = recvByte;
                    break;
                case 2:
                    received.msg   = recvByte;
                    break;
                case 3:
                    received.data1 = recvByte;
                    break;
                case 4:
                    received.data2 = recvByte;
                    break;
                default:
                    outputEvent(USART_BAD_MESSAGE);
                    break;
            }
            bytesRecvd++;
        }
        else if( bytesRecvd == 5 && (DRV_USART0_ReceiverBufferIsEmpty()) )
        {
            if (xQueueSendFromISR(controlQueue, &received, &xHigherPriorityTaskWoken) == pdFALSE) 
            {
                outputEvent(CONTROL_QUEUE_FULL);
            } 
            else 
            {
                outputEvent(CONTROL_QUEUE_ITEM_SENT_FROM_USART_INT);
            }
            bytesRecvd = 0;
        }
        else
        {
            outputEvent(USART_DEFAULT_EXCEPTION);
            bytesRecvd = 0;
        }
        clearInt:
        /* Clear pending interrupt */
        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE);
    }
    
    /* Check error interrupt */
    if (PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_ERROR)) 
    {
        //TODO: Handle errors
    }
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_ERROR);
}

