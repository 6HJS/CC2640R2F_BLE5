/*******************************************************************************
               Copyright (c) 2017-2018, Ghostyu Co.,Ltd.
                           All rights reserved.
  TI_RTOS Semaphores experiment
  Data : $2017-07-05 15:05 $
  Version : $ 00001$
*******************************************************************************/
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/drivers/PIN.h>
#include <ti/drivers/UART.h>
#include <ti/display/Display.h>
/* Board Header files */
#include "Board.h"

#include "UserTask.h"

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
static Task_Struct workTask2;

/* Pin driver handle */
static PIN_Handle ledPinHandle;
static PIN_State  ledPinState;

/* Semaphore handle*/
static Semaphore_Handle  semBinHandle;

/* LCD Handle*/
Display_Handle  dispHandle;

/*
 * function declear
 */
static Void SEM_Create(void );
static Void StartTaskFunc(UArg a0,UArg a1);
static Void workTask1Func(UArg a0,UArg a1);
static Void workTask2Func(UArg a0,UArg a1);

//******************************************************************************
// fn : LCD_Print
//
// brief : LCD屏上打印字符串
//
// param : none
//
// return : none
void LCD_Print(char*str,char* a)
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
//  System_printf("System Create StartTask \n");
//  System_flush();
  Task_Params  taskParams;
  Task_Params_init(&taskParams);
  
  taskParams.priority = Task_numPriorities -1;
  taskParams.stackSize = TASK_STACK_SIZE + 128;
  Task_construct(&startTask,(Task_FuncPtr)StartTaskFunc,&taskParams,NULL);
  
}

void USER_createWorkTask(UInt priority,Task_FuncPtr func,Task_Struct* pStruct)
{
//  System_printf("System Create WordTask Pri = %d \n",priority);
//  System_flush();
  Task_Params  taskParams;
  Task_Params_init(&taskParams);
  
  taskParams.priority = priority;
  taskParams.stackSize = TASK_STACK_SIZE;
  Task_construct(pStruct,func,&taskParams,NULL);
}
//******************************************************************************
// fn : SEM_Create
//
// brief : 创建二值信号量
//
// param : none
//
// return : none
static Void SEM_Create(void )
{
  Semaphore_Params  semParams;
  Semaphore_Params_init(&semParams);
  semParams.mode = Semaphore_Mode_BINARY_PRIORITY;
  semBinHandle = Semaphore_create(0,&semParams,NULL);
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

  
  PIN_setOutputValue(ledPinHandle, Board_LED0, 1);
  
  //打开LCD
  dispHandle = Display_open(Display_Type_LCD,NULL);
  
  //显示测试信息
  Display_print0(dispHandle,0,4,"TI-RTOS ");
  Display_print0(dispHandle,1,0,"Binary_Pri Test");
  
  SEM_Create();
  //创建两个工作任务
  USER_createWorkTask(1,workTask1Func,&workTask1);
  
  for(;;)
  {
    Task_sleep(100000); //sleep 1s
    Semaphore_post(semBinHandle);
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
  USER_createWorkTask(2,workTask2Func,&workTask2);
  for(;;)
  {
    //低优先级任务将不能打印字符串
    Semaphore_pend(semBinHandle,BIOS_WAIT_FOREVER);
    
    PIN_setOutputValue(ledPinHandle, Board_LED1,
                       !PIN_getOutputValue(Board_LED1));

    LCD_Print("Task1 pend",NULL);
    

  }
}
//******************************************************************************
// fn : workTaskFunc
//
// brief : 启动任务
//
// param : a0 -> 系统参数0
//         a1 -> 系统参数1
//
// return : none
static Void workTask2Func(UArg a0,UArg a1)
{

  for(;;)
  {
    Semaphore_pend(semBinHandle,BIOS_WAIT_FOREVER);
    PIN_setOutputValue(ledPinHandle, Board_LED0,
                       !PIN_getOutputValue(Board_LED0));

    LCD_Print("Task2 pend",NULL);
    

  }
}