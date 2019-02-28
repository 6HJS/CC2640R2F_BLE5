//*
/*******************************************************************************
*           Copyright(c) 2017-2018, Ghostyu Co.,Ltd
*                  All rights reserved.
*
* 
*  Forum : www.iotxx.com
*          
*
*******************************************************************************/
//*

/*
 *  ======== empty.c ========
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Swi.h>

/* TI-RTOS Header files */
// #include <ti/drivers/I2C.h>
#include <ti/drivers/PIN.h>
// #include <ti/drivers/SPI.h>
// #include <ti/drivers/UART.h>
// #include <ti/drivers/Watchdog.h>
#include <ti/display/Display.h>
/* Board Header files */
#include "Board.h"

#define TASKSTACKSIZE   640

typedef enum
{
  POST,
  DEC,
  ANDN
}swi_t;

/* LCD Handle*/
Display_Handle  dispHandle;

static swi_t  swi_trigger = POST;

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];


/* Pin driver handle */
static PIN_Handle ledPinHandle;
static PIN_State ledPinState;

/* Swi struct*/
static Swi_Struct  swiStruct;


/* local function declear*/
static void swi_Fxn(UArg a0, UArg a1);

/*
 * Application LED pin configuration table:
 *   - All LEDs board LEDs are off.
 */
PIN_Config ledPinTable[] = 
{
    Board_LED0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    Board_MOTOR  | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

//******************************************************************************
// fn : SWI_Create;
//
// brief : Create swi instance object
//
// param : none
//
// reutn : none
void SWI_Create(void)
{
  Swi_Params  swiParams;
  Swi_Params_init(&swiParams);
  swiParams.arg0 = 0;
  swiParams.arg1 = 0;
  swiParams.trigger = 3;
  swiParams.priority = 2;
  Swi_construct(&swiStruct,swi_Fxn,&swiParams,NULL);
  
}
//******************************************************************************
// fn : heartBeatFxn
//
// brief : Toggle the Board_LED0. The Task_sleep is determined by arg0 which.
//         is configured for the heartBeat Task instance.
//
// params : arg0 -- params -1
//          arg1 -- params -2
//
// return : none
Void heartBeatFxn(UArg arg0, UArg arg1)
{
    UInt mask = 0;
    //打开LCD
    dispHandle = Display_open(Display_Type_LCD,NULL);
  
    //显示测试信息
    Display_print0(dispHandle,0,4,"TI-RTOS ");
    Display_print0(dispHandle,1,0,"SWI Test");
    
    SWI_Create();  //创建一个swi对象
    
    while (1) 
    {
        Task_sleep((UInt)arg0);
        //trigger a swi
        if(swi_trigger == POST)
        {
          mask = 0;
          PIN_setOutputValue(ledPinHandle, Board_LED0,
                              !PIN_getOutputValue(Board_LED0));
          Swi_post(Swi_handle(&swiStruct));

        }
        else if(swi_trigger == DEC)
        {
          mask = 0;
          PIN_setOutputValue(ledPinHandle, Board_LED1,
                             !PIN_getOutputValue(Board_LED1));
          Swi_dec(Swi_handle(&swiStruct));
        
        }
        else
        {
          mask++;
          PIN_setOutputValue(ledPinHandle, Board_MOTOR,
                             !PIN_getOutputValue(Board_MOTOR));
          Swi_andn(Swi_handle(&swiStruct),mask);
                    
        }
        
    }
}

/*
 *  ======== main ========
 */
int main(void)
{
    Task_Params taskParams;

    /* Call board init functions */
    Board_initGeneral();
    // Board_initI2C();
    // Board_initSPI();
    // Board_initUART();
    // Board_initWatchdog();

    /* Construct heartBeat Task  thread */
    Task_Params_init(&taskParams);
    taskParams.arg0 = 1000000 / Clock_tickPeriod;
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)heartBeatFxn, &taskParams, NULL);

    /* Open LED pins */
    ledPinHandle = PIN_open(&ledPinState, ledPinTable);
    PIN_setOutputValue(ledPinHandle, Board_LED1, 1);

    /* Start BIOS */
    BIOS_start();

    return (0);
}
//******************************************************************************
// fn: swi_Fxn
//
// brief : swi 对象中断函数
//
// param : a0 -> 参数1
//         a1 -> 参数2
//
// return : none
static void swi_Fxn(UArg a0, UArg a1)
{
  if(swi_trigger == POST)
  {
    swi_trigger = DEC;
  }
  else if(swi_trigger == DEC)
  {
    swi_trigger = ANDN;
  }
  else
  {
    swi_trigger = POST;
  }
}