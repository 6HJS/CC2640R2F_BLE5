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
#include <ti/sysbios/knl/Event.h>

#include <ti/sysbios/hal/Hwi.h>

/* TI-RTOS Header files */
// #include <ti/drivers/I2C.h>
#include <ti/drivers/PIN.h>
// #include <ti/drivers/SPI.h>
// #include <ti/drivers/UART.h>
// #include <ti/drivers/Watchdog.h>

#include <ti/display/Display.h>

/* Board Header files */
#include "Board.h"
#include "iotboard_key.h"


#define TASKSTACKSIZE   (640)

#define LCD_FIRST_LINE  2

#ifdef IOTXX_DISPLAY_TFT
#define LCD_LAST_LINE   7
#else
#define LCD_LAST_LINE   9
#endif


Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];
Task_Struct task1Struct;

/* LCD Handle*/
Display_Handle  dispHandle;

/* Pin driver handle */

static PIN_Handle ledPinHandle;
static PIN_State ledPinState;

static Event_Handle  myEventHandle = NULL;

static UChar g_Count = 0;
/*
 * Application LED pin configuration table:
 *   - All LEDs board LEDs are off.
 */
PIN_Config ledPinTable[] = 
{
    Board_LED0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

/*
 * function declear
 */
Void TaskKeyFxn(UArg arg0, UArg arg1);

//******************************************************************************
// fn : LCD_Print
//
// brief : LCD屏上打印字符串
//
// param : none
//
// return : none
void LCD_Print(char*str,UChar *a)
{
  static UChar line = LCD_FIRST_LINE;
  
  if(line > LCD_LAST_LINE)
  {
    Display_clearLines(dispHandle,LCD_FIRST_LINE,LCD_LAST_LINE);
    line = LCD_FIRST_LINE;
  }
  if(line < LCD_FIRST_LINE)
  {
    line = LCD_FIRST_LINE;
  }
  if(a == NULL)
  {
    Display_print0(dispHandle,line++,0,str);
  }
  else
  {
    Display_print1(dispHandle,line++,0,str,*a);
  }
}
//******************************************************************************
// fn : EVENT_Create
//
// brief : 创建事件
//
// param : none
//
// return : none
static Void EVENT_Create(void )
{
  LCD_Print("MyEvent Create\n",NULL);
  Event_Params  eventParams;
  Event_Params_init(&eventParams);

  myEventHandle = Event_create(&eventParams,NULL);
}
//******************************************************************************
// fn : ButtonHandleFunc
//
// brief : 处理按钮按下事件
//
// param : keys -> 按钮集合
//
// return : none
static void ButtonHandleFunc(UChar keys)
{
  UInt eventId = Event_Id_NONE;
  if(keys & KEY_BTN1)
  {
    eventId |= Event_Id_00;
  }
  if(keys & KEY_BTN2)
  {
    eventId |= Event_Id_01;
  }
  if(eventId)
  {
    Event_post(myEventHandle,eventId);    
  }
}

//******************************************************************************
// fn : UserTaskCreate;
//
// brief : Create task instance object
//
// param : none
//
// reutn : none
void UserTaskCreate(void)
{
  Task_Params taskParams;
  Task_Params_init(&taskParams);
  
  taskParams.arg0 = 1000000 / Clock_tickPeriod;
  taskParams.stackSize = TASKSTACKSIZE;
  Task_construct(&task1Struct, (Task_FuncPtr)TaskKeyFxn, &taskParams, NULL);
}
//******************************************************************************
// fn : heartBeatFxn
//
// brief : Toggle the Board_LED0. The Task_sleep is determined by arg0 whichis 
//         configured for the heartBeat Task instance.
//
//
// params : arg0
//          arg1
// 
// return : none
Void heartBeatFxn(UArg arg0, UArg arg1)
{
  UInt key;

  Board_initKeys(ButtonHandleFunc);
  //打开LCD
  dispHandle = Display_open(Display_Type_LCD,NULL);
  
  //显示测试信息
  Display_print0(dispHandle,0,4,"TI-RTOS");
  Display_print0(dispHandle,1,0,"Gates Test");
  
  EVENT_Create();
  //创建用户任务
  UserTaskCreate();
  while (1)
  {
    Task_sleep((UInt)arg0);
    key = Hwi_disable();
    g_Count++;
    Hwi_restore(key);
    LCD_Print("Value=%d",&g_Count);
    PIN_setOutputValue(ledPinHandle, Board_LED0,
                       !PIN_getOutputValue(Board_LED0));
  }
}
//******************************************************************************
// fn : TaskKeyFxn
//
// brief : pend the event with handle the key
//
//
// params : arg0
//          arg1
// 
// return : none
Void TaskKeyFxn(UArg arg0, UArg arg1)
{
  UInt eventId = 0;
  UInt key;
  
  while (1)
  {

    eventId = Event_pend(myEventHandle,Event_Id_NONE,Event_Id_00|Event_Id_01,10000);
    if(eventId == Event_Id_00 )
    {
      key = Hwi_disable();
      g_Count = 0;
      Hwi_restore(key);
    }
    if(Event_Id_01 == eventId)
    {
      key = Hwi_disable();
      g_Count = 200;
      Hwi_restore(key);
    }
    
    PIN_setOutputValue(ledPinHandle, Board_LED1,
                       !PIN_getOutputValue(Board_LED1));
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
