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

#define TASK_OLD_PRI  1
#define TASK_NEW_PRI  3

Task_Struct mainTaskStruct;
Char task0Stack[TASKSTACKSIZE];

/* LCD Handle*/
Display_Handle  dispHandle = NULL;

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
  //初始化串口
  UChar count = 0;
  while(dispHandle == NULL)
  {
    Task_sleep(1000);
  }
  
  while(1)
  {
    Task_sleep((UInt)arg0);
    PIN_setOutputValue(ledPinHandle, Board_LED1,
                   !PIN_getOutputValue(Board_LED1));
    UART_write(pUartHandle,"MyTask1 print \r\n",strlen("MyTask1 print \r\n"));
    LCD_Print("MyTask1 print");
    if(count++ == 50)
    {
      Task_setPri(Task_self(),TASK_NEW_PRI);
    }
    else if(count == 200)
    {
      Task_setPri(Task_self(),TASK_OLD_PRI);
    }
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
  UARTDriverInit();
  //打开LCD
  dispHandle = Display_open(Display_Type_LCD,NULL);
  
  //显示测试信息
  Display_print0(dispHandle,0,4,"TI-RTOS ");
  Display_print0(dispHandle,1,0,"Task Pri");
 
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
  CreateUserTask();
  /* Start BIOS */
  BIOS_start();
  
  return (0);
}