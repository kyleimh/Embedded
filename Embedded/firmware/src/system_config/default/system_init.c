
#include "system_config.h"
#include "system_definitions.h"


// ****************************************************************************
// ****************************************************************************
// Section: Configuration Bits
// ****************************************************************************
// ****************************************************************************
// <editor-fold defaultstate="collapsed" desc="Configuration Bits">

/*** DEVCFG0 ***/

#pragma config DEBUG =      OFF
#pragma config ICESEL =     ICS_PGx2
#pragma config PWP =        OFF
#pragma config BWP =        OFF
#pragma config CP =         OFF

/*** DEVCFG1 ***/

#pragma config FNOSC =      PRIPLL
#pragma config FSOSCEN =    OFF
#pragma config IESO =       ON
#pragma config POSCMOD =    XT
#pragma config OSCIOFNC =   OFF
#pragma config FPBDIV =     DIV_1
#pragma config FCKSM =      CSDCMD
#pragma config WDTPS =      PS1048576
#pragma config FWDTEN =     ON

/*** DEVCFG2 ***/

#pragma config FPLLIDIV =   DIV_12
#pragma config FPLLMUL =    MUL_20
#pragma config FPLLODIV =   DIV_1
#pragma config UPLLIDIV =   DIV_2
#pragma config UPLLEN =     OFF

/*** DEVCFG3 ***/

#pragma config USERID =     0xffff
#pragma config FSRSSEL =    PRIORITY_7
#pragma config FMIIEN =     ON
#pragma config FETHIO =     ON
#pragma config FCANIO =     ON
#pragma config FUSBIDIO =   ON
#pragma config FVBUSONIO =  ON
// </editor-fold>


// *****************************************************************************
// *****************************************************************************
// Section: Library/Stack Initialization Data
// *****************************************************************************
// *****************************************************************************



// *****************************************************************************
// *****************************************************************************
// Section: Driver Initialization Data
// *****************************************************************************
// *****************************************************************************

//<editor-fold defaultstate="collapsed" desc="DRV_Timer Initialization Data">

// </editor-fold>
//<editor-fold defaultstate="collapsed" desc="DRV_USART Initialization Data">

// </editor-fold>

// *****************************************************************************
// *****************************************************************************
// Section: System Data
// *****************************************************************************
// *****************************************************************************

/* Structure to hold the object handles for the modules in the system. */
SYSTEM_OBJECTS sysObj;


// *****************************************************************************
// *****************************************************************************
// Section: Module Initialization Data
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Device Control System Service Initialization Data
  
  <editor-fold defaultstate="collapsed" 
  desc="Device Control System Service Initialization Data">
*/

const SYS_DEVCON_INIT sysDevconInit =
{
    .moduleInit = {0},
};

// </editor-fold>


// *****************************************************************************
// *****************************************************************************
// Section: Static Initialization Functions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: System Initialization
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void SYS_Initialize ( SYS_INIT_DATA *data )

  Summary:
    Initializes the board, services, drivers, application and other modules.

  Remarks:
    See prototype in system/common/sys_module.h.
 */

void SYS_Initialize ( void* data )
{
    /* Core Processor Initialization */
    SYS_CLK_Initialize( NULL );
    sysObj.sysDevcon = SYS_DEVCON_Initialize(SYS_DEVCON_INDEX_0, (SYS_MODULE_INIT*)&sysDevconInit);
    SYS_DEVCON_PerformanceConfig(SYS_CLK_SystemFrequencyGet());
    SYS_DEVCON_JTAGDisable();
    SYS_PORTS_Initialize();

    /* Initialize Drivers */
    /* Initialize ADC */
    DRV_ADC_Initialize();
    /*Initialize TMR0 */
    DRV_TMR0_Initialize();
    /*Initialize TMR1 */
    DRV_TMR1_Initialize();
    /*Initialize TMR2 */
    DRV_TMR2_Initialize();
 
    DRV_USART0_Initialize();
    

     /*Initialize OC0 */
    DRV_OC0_Initialize();

    /*Initialize OC1 */
    DRV_OC1_Initialize();

    /* Initialize System Services */
    SYS_INT_Initialize();  

    
    SYS_PORTS_PinModeSelect(PORTS_ID_0, PORTS_ANALOG_PIN_11, PORTS_PIN_MODE_DIGITAL);
    SYS_PORTS_PinModeSelect(PORTS_ID_0, PORTS_ANALOG_PIN_12, PORTS_PIN_MODE_DIGITAL);
    SYS_PORTS_PinModeSelect(PORTS_ID_0, PORTS_ANALOG_PIN_13, PORTS_PIN_MODE_DIGITAL);
    
    SYS_PORTS_DirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_A, 0x8);
    SYS_PORTS_Set(PORTS_ID_0, PORT_CHANNEL_A, 0x8, 0x8);
    SYS_PORTS_DirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_E, 0xFF);
    SYS_PORTS_Clear(PORTS_ID_0, PORT_CHANNEL_E, 0xFF);
    
    debugScheme = 0;
    line_correction = true;
    outputEvent(INITIALIZE);
    /* Initialize Middleware */

    /* Initialize the Application */
    CONTROL_Initialize();
    MOTOR_Initialize();
    SENSOR_Initialize();
}


/*******************************************************************************
 End of File
*/

