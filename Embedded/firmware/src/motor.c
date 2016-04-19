#include "motor.h"

void INIT_Rover_Motors() 
{
    //Initialize Motors
    //Set up proper pins as outputs (left)
        //pin that handles left motor enable - see data sheet
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_0); //direction
        //pin that handles left motor direction - see data sheet
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14); 
    
    //Keep motors off during initialize
    PLIB_OC_PulseWidth16BitSet(OC_ID_1, 0);
            
    //Set up proper pins as outputs (right)
        //pin that handles right motor enable - see data sheet
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_1); //direction
        //pin that handles right motor direction - see data sheet
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_1); 
    
    //set motors off to init
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, 0);
    
    //Start OC drivers
    DRV_OC0_Enable(); 
    DRV_OC1_Enable(); 
    
    DRV_OC0_Start();    //Start the OC drivers that talk to the motor shield
    DRV_OC1_Start();
    
    PLIB_PORTS_PinDirectionOutputSet (PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_3);    
    PLIB_PORTS_PinClear (PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_3);
}

void move_Forward(int speed_R, int speed_L)
{
    //Make right motor go forward 
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14);
    PLIB_OC_PulseWidth16BitSet(OC_ID_1, speed_R);
    
    //Make left motor go forward
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_1);
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, speed_L);
    
    R_encoder = 0; 
    L_encoder = 0;
    if (Mv_distance == 0)
    {
        return; // no designated distance
    }
    
    while(R_encoder < Mv_distance && L_encoder < Mv_distance) {;} //wait
    
    move_Stop();
    setDistance(0);
}
void move_Backward(int speed_R, int speed_L)
{
    //Make right motor go backward 
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14);
    PLIB_OC_PulseWidth16BitSet(OC_ID_1, speed_R);
    
    //Make left motor go backward
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_1);
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, speed_L);
    
    R_encoder = 0; 
    L_encoder = 0;
    
    if (Mv_distance == 0)
    {
        return; // no designated distance
    }
    
    while(R_encoder < Mv_distance && L_encoder < Mv_distance) {;} //wait
    
    move_Stop();    
    setDistance(0);
}
void move_Stop()
{
    //Make left motor stop 
    PLIB_OC_PulseWidth16BitSet(OC_ID_1, 0);
    //Make right motor stop
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, 0);
}
//Handles turning the rovers, keep turning until a stop is called
void move_Turn_Right(int speed)
{
    //Make right motor go forward 
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14);
    PLIB_OC_PulseWidth16BitSet(OC_ID_1, speed);
    
    //Make left motor go forward
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_1);
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, speed);
    
    R_encoder = 0; 
    L_encoder = 0;
    
    if (Mv_degree == 0)
    {
        return; // no designated distance
    }
    
    while(R_encoder < Mv_degree ) {;} //wait
    
    move_Stop();
    setDegree(0);
}
void move_Turn_Left(int speed)
{
    //Make right motor go forward 
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14);
    PLIB_OC_PulseWidth16BitSet(OC_ID_1, speed);
    
    //Make left motor go forward
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_1);
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, speed);
    
    R_encoder = 0; 
    L_encoder = 0;
    
    if (Mv_degree == 0)
    {
        return; // no designated distance
    }
    
    while(L_encoder < Mv_degree) {;} //wait
    
    move_Stop();
    setDegree(0);
}

void test(int speed)
{
    //reset encoder count
    R_encoder = 0; 
    L_encoder = 0;
 
    move_Turn_Right(speed);
    
    while (R_encoder <  55) {;} //delay until count is right
    
   move_Stop();     
}

void setDistance(int dis)
{
    dis = dis - 5;
    Mv_distance = (dis * 17) - (dis/5) + (dis/10);
}

void setDegree(int degree)
{
    //what degrees am I going to be able to move
    degree = degree - 5;
    Mv_degree = (0.6914)*degree - 9.1429;
}

void setSpeed(int speed)
{
    Mv_speed = 2000;
}
void MOTOR_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    motor_data.state = MOTOR_STATE_INIT; 
    
    INIT_Rover_Motors();
    
    //Used for debugging
    SYS_PORTS_PinDirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_F, PORTS_BIT_POS_3 );
    SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_3);
    
    R_encoder = 0; //initialized and incremented by ISR
    L_encoder = 0;
    Mv_distance = 0; 
    Mv_degree = 0;
    
    DRV_TMR0_Start();
    DRV_TMR1_Start();   //timer for encoders
    DRV_TMR2_Start();   //timer for encoders
    
    if (!(motorQueue = xQueueCreate(10, sizeof(MESSAGE)))) 
    {
        outputEvent(MOTOR_QUEUE_NOT_CREATED);
    } 
    else 
    {
       outputEvent(MOTOR_QUEUE_CREATED);
    }   
}

void MOTOR_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( motor_data.state )
    {
        /* Application's initial state. */
        case MOTOR_STATE_INIT:
        {
            motor_data.state = MOTOR_STATE_RUN;
            break;
        }      
       
        case MOTOR_STATE_RUN:
        {  
            
            MESSAGE message;
            if (xQueueReceive(motorQueue, &message, portMAX_DELAY) == pdFALSE) 
            {
                outputEvent(MOTOR_QUEUE_EMPTY);
            } 
            else 
            {
                outputEvent(MOTOR_QUEUE_RECEIVED);
                int dis;
                //read message in properly
                switch (message.msg)
                {
                    case STOP:
                        move_Stop();
                        break;
                        
                    case FORWARD:
                        dis = message.data1;
                        setDistance(dis);
                        setSpeed(message.data2);
                        move_Forward(Mv_speed, Mv_speed);
                        sendDone(dis);
                        break;
                        
                    case BACKWARD:
                        dis = message.data1;                        
                        setDistance(dis);
                        setSpeed(message.data2);
                        move_Backward(Mv_speed, Mv_speed);
                        sendDone(dis);
                        break;
                        
                    case TURN_LEFT:
                        dis = message.data1;                        
                        setDegree(dis);
                        setSpeed(message.data2);
                        move_Turn_Left(Mv_speed);
                        sendDone(dis);
                        break;
                        
                    case TURN_RIGHT:
                        dis = message.data1;
                        setDegree(dis);
                        setSpeed(message.data2);
                        move_Turn_Right(Mv_speed);
                        sendDone(dis);
                        break;
                        
                    default:
                        break;
                }
                
                outputEvent(MOTOR_QUEUE_RECEIVED);
            }
            break;
        }

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

void sendDone(int disOrdeg){
    MESSAGE mssge;
        mssge.id    = 0x32;
        mssge.msg   = DONE;
        mssge.data1 = disOrdeg;
        mssge.data2 = 0x0;
        if (xQueueSendToBack(controlQueue, (void *)&mssge, (TickType_t)10) == pdFALSE) 
        {
//            outputEvent;
        } 
        else
        {
//            outputEvent(CONTROL_QUEUE_ITEM_SENT);
        }
}