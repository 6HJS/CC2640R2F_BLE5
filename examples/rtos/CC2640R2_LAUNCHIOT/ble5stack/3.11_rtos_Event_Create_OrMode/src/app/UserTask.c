/*******************************************************************************
               Copyright (c) 2017-2018, Ghostyu Co.,Ltd.
                           All rights reserved.
  TI_RTOS Semaphores experiment
  Data : $2017-07-05 15:05 $
  Version : $ 00001$
*******************************************************************************/
/* XDCtools Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Event.h>

#include <ti/drivers/PIN.h>
#include <ti/drivers/UART.h>
#include <ti/display/Display.h>
/* Board Header files */
#include "Board.h"

#include "UserTask.h"
#include "iotboard_key.h"

/*
 * Const define area
 */

#define TASK_STACK_SIZE   640

#define LCD_FIRST_LINE  2

#ifdef IOTXX_DISPLAY_TFT
#define LCD_LAST_LINE   7
#else
#define LCD_LAST_LINE   9
#endif

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
 * Global virable define
 */
static Task_Struct startTask;
static Task_Struct workTask1;

/* Pin driver handle */
static PIN_Handle ledPinHandle;
static PIN_State  ledPinState;

/* Semaphore handle*/
static Event_Handle  myEventHandle = NULL;

/* LCD Handle*/
Display_Handle  dispHandle;

/*
 * function declear
 */
static Void EVENT_Create(void );
static Void StartTaskFunc(UArg a0,UArg a1);
static Void workTask1Func(UArg a0,UArg a1);
static Void ButtonHandleFunc(UChar keys);

//******************************************************************************
// fn : LCD_Print
//
// brief : LCD屏上打印字符串
//
// param : none
//
// return : none
void LCD_Print(char*str,char *a)
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
// fn : USER_createStartTask
//
// brief : 创建最高优先级任务
//
// param : none
//
// return : none
void USER_createStartTask(void)
{
  Task_Params  taskParams;
  Task_Params_init(&taskParams);
  
  taskParams.priority = Task_numPriorities -1;
  taskParams.stackSize = TASK_STACK_SIZE + 128;
  Task_construct(&startTask,(Task_FuncPtr)StartTaskFunc,&taskParams,NULL);
}

void USER_createWorkTask(UInt priority,Task_FuncPtr func,Task_Struct* pStruct)
{
  LCD_Print("Task Pri=%d",(char*)&priority);
  
  Task_Params  taskParams;
  Task_Params_init(&taskParams);
  
  taskParams.priority = priority;
  taskParams.stackSize = TASK_STACK_SIZE;
  Task_construct(pStruct,func,&taskParams,NULL);
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
  char* infoStr;
  LCD_Print("MyEvt Create",NULL);

  Event_Params  eventParams;
  Event_Params_init(&eventParams);

  myEventHandle = Event_create(&eventParams,NULL);
  if(myEventHandle == NULL)
  {
    infoStr = "fail";
  }
  else
  {
    infoStr = "success";
  }
  LCD_Print(infoStr,NULL);
}
//******************************************************************************
// fn : StartTaskFunc
//
// brief : 启动任务
//
// param : a0 -> 系统参数0
//         a1 -> 系统参数1
//
// return : none
static Void StartTaskFunc(UArg a0,UArg a1)
{
  /* Open LED pins */
  ledPinHandle = PIN_open(&ledPinState, ledPinTable);

  PIN_setOutputValue(ledPinHandle, Board_LED0, 0);
  
  Board_initKeys(ButtonHandleFunc);
  //初始化串口驱动
  //UART_DriverInit();
  //打开LCD
  dispHandle = Display_open(Display_Type_LCD,NULL);
  
  //显示测试信息
  Display_print0(dispHandle,0,4,"TI-RTOS");
  Display_print0(dispHandle,1,0,"Event OR Test");
  
  EVENT_Create();
  //创建一个工作任务
  USER_createWorkTask(1,workTask1Func,&workTask1);
  
  Task_destruct(&startTask);
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
  if(keys & KEY_BTN1)
  {
    Event_post(myEventHandle,Event_Id_00);
  }
  if(keys & KEY_BTN2)
  {
    Event_post(myEventHandle,Event_Id_01);
  }
}
//******************************************************************************
// fn : StartTaskFunc
//
// brief : 启动任务
//
// param : a0 -> 系统参数0
//         a1 -> 系统参数1
//
// return : none
static Void workTask1Func(UArg a0,UArg a1)
{
  UInt eventId = 0;
  char *keyInfo = NULL;
  char pri = Task_getPri(Task_self());
  LCD_Print("work1 run,pri=%d",&pri);

  for(;;)
  {
    LCD_Print("Task pend Events",NULL);
    
    eventId = Event_pend(myEventHandle, Event_Id_NONE,Event_Id_00|Event_Id_01,BIOS_WAIT_FOREVER);
    if(eventId & Event_Id_00)
    {
      keyInfo = "key1 pressed";
    }
    if(eventId & Event_Id_01)
    {
      keyInfo = "key2 pressed";
    }
    
    PIN_setOutputValue(ledPinHandle, Board_LED1,
                       !PIN_getOutputValue(Board_LED1));
    if(keyInfo)
    {
      LCD_Print(keyInfo,NULL);
    }
    keyInfo = NULL;
    PIN_setOutputValue(ledPinHandle, Board_LED1,
                       !PIN_getOutputValue(Board_LED1));
  }
}