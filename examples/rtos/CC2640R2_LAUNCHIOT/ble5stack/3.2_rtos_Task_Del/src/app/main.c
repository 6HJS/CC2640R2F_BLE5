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

/* XDCtools Header files */
#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
// #include <ti/drivers/I2C.h>
#include <ti/drivers/PIN.h>
// #include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>
// #include <ti/drivers/Watchdog.h>
#include <ti/display/Display.h>
/* Board Header files */
#include "Board.h"

#define TASKSTACKSIZE   640

#define LCD_FIRST_LINE  2

#ifdef IOTXX_DISPLAY_TFT
#define LCD_LAST_LINE   7
#else
#define LCD_LAST_LINE   9
#endif

Task_Struct mainTaskStruct;
Char task0Stack[TASKSTACKSIZE];

/* LCD Handle*/
Display_Handle  dispHandle;

/* Pin driver handle */
static PIN_Handle ledPinHandle;
static PIN_State ledPinState;

/* Uart handle*/
UART_Handle pUartHandle;

/* Functions declear*/
void CreateUserTask(void);
void UARTDriverInit(void);


/*
 * Application LED pin configuration table:
 *   - All LEDs board LEDs are off.
 */
PIN_Config ledPinTable[] = {
    Board_LED0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

void UARTDriverInit(void)
{
  UART_Params uartParams;
  UART_Params_init(&uartParams);
  uartParams.writeDataMode = UART_DATA_BINARY;
  uartParams.readDataMode = UART_DATA_BINARY;
  uartParams.readReturnMode = UART_RETURN_FULL;
  uartParams.readEcho = UART_ECHO_OFF;
  pUartHandle = UART_open(CC2640R2_LAUNCHXL_UART0, &uartParams);
}
void LCD_Print(char*str)
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
  Display_print0(dispHandle,line++,0,str);
}

Void MyTask1(UArg arg0, UArg arg1)
{
  while(1)
  {
    Task_sleep((UInt)arg0);
    PIN_setOutputValue(ledPinHandle, Board_LED1,
                   !PIN_getOutputValue(Board_LED1));
    UART_write(pUartHandle,"MyTask1 print \r\n",strlen("MyTask1 print \r\n"));
    LCD_Print("MyTask1 print");
  }
}
//******************************************************************************
// fn : MyTask2
//
// brief : Toggle the Board_LED0. The Task_sleep is determined by arg0 which is 
//         configured for the heartBeat Task instance.
//
// param : arg0 -> system parameter 1
//         arg1 -> system parameter 2
//
// return : none
Void MyTask2(UArg arg0, UArg arg1)
{
  while(1) 
  {
    Task_sleep((UInt)arg0);
    PIN_setOutputValue(ledPinHandle, Board_LED0,
                       !PIN_getOutputValue(Board_LED0));
    UART_write(pUartHandle,"MyTask2 print \r\n",strlen("MyTask2 print \r\n"));
    LCD_Print("MyTask2 print");
  }
}
//******************************************************************************
// fn : MainTask
//
// brief : the application main task function.it will create a user task  
//         .it only run one times.       
//         
// param : arg0 -> system parameter 1
//         arg1 -> system parameter 2
//
// return : none
Void MainTask(UArg arg0,UArg arg1)
{
  UARTDriverInit();
    //打开LCD
  dispHandle = Display_open(Display_Type_LCD,NULL);
  
  //显示测试信息
  Display_print0(dispHandle,0,4,"TI-RTOS ");
  Display_print0(dispHandle,1,0,"Create Task");
  

  //创建用户任务
  CreateUserTask();

  //删除
  Task_destruct(&mainTaskStruct);
}
//******************************************************************************
// fn : createUserTask
//
// brief : crate a user task with Task_create API
//         
// param : none
//
// return : none
void CreateUserTask(void)
{
  Task_Params taskParams;
  
  /* Construct heartBeat Task  thread */
  Task_Params_init(&taskParams);
  taskParams.arg0 = 1000000 / Clock_tickPeriod;
  taskParams.stackSize = TASKSTACKSIZE;
  taskParams.stack = &task0Stack;
  taskParams.priority = 1;
  Task_create((Task_FuncPtr)MyTask2, &taskParams, NULL);
}
//******************************************************************************
// fn : createMainTask
//
// brief : create a user task with Task_create API
//
// param : none
//
// return : none
void CreateMainTask(void)
{
  Task_Params taskParams;
  /* Construct heartBeat Task  thread */
  Task_Params_init(&taskParams);
  taskParams.priority = Task_numPriorities -1;
  Task_construct(&mainTaskStruct, (Task_FuncPtr)MainTask, &taskParams, NULL);
}
//*****************************************************************************
// fn : main
//
// brief : application entry
//
// param : none
//
// return : none
int main(void)
{
  /* Call board init functions */
  Board_initGeneral();
  // Board_initI2C();
  // Board_initSPI();
  Board_initUART();
  // Board_initWatchdog();
  
  /* Open LED pins */
  ledPinHandle = PIN_open(&ledPinState, ledPinTable);
  
  PIN_setOutputValue(ledPinHandle, Board_LED1, 1);
  CreateMainTask();
  /* Start BIOS */
  BIOS_start();
  
  return (0);
}