/*******************************************************************************
               Copyright (c) 2017-2018, Ghostyu Co.,Ltd.
                           All rights reserved.
  TI_RTOS Semaphores experiment
  Data : $2017-07-05 15:05 $
  Version : $ 00001$
*******************************************************************************/
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>

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
typedef struct Rec
{
  Queue_Elem  elem;
  UChar        key;
}Rec;


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

/* Uart driver handle*/
static UART_Handle   uartHandle;

/* Semaphore handle*/
static Event_Handle  myEventHandle = NULL;
static Semaphore_Handle keySemHandle = NULL;
static Queue_Handle myQueueHandle = NULL;
static Rec          queue_rec;

/* LCD Handle*/
Display_Handle  dispHandle;

/*
 * function declear
 */
static void UART_DriverInit(void);
static Void EVENT_Create(void );
static Void SEM_Create(void);
static Void QUEUE_Create(void);
static Void StartTaskFunc(UArg a0,UArg a1);
static Void workTask1Func(UArg a0,UArg a1);
static Void workTask2Func(UArg a0,UArg a1);
static Void ButtonHandleFunc(UChar keys);

//******************************************************************************
// fn : LCD_Print
//
// brief : LCD���ϴ�ӡ�ַ���
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
// brief : ����������ȼ�����
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
  LCD_Print("Task Pri=%d\n",(char*)&priority);

  Task_Params  taskParams;
  Task_Params_init(&taskParams);
  
  taskParams.priority = priority;
  taskParams.stackSize = TASK_STACK_SIZE;
  Task_construct(pStruct,func,&taskParams,NULL);
}
//******************************************************************************
// fn : UART_DriverInit
//
// brief : ��ʼ��LAUNCHIOT������
//
// param : none
//
// return : none
static void UART_DriverInit(void)
{
  UART_Params uartParams;
  UART_Params_init(&uartParams);
  uartParams.writeDataMode = UART_DATA_BINARY;
  uartParams.readDataMode = UART_DATA_BINARY;
  uartParams.readReturnMode = UART_RETURN_FULL;
  uartParams.readEcho = UART_ECHO_OFF;
  uartHandle = UART_open(CC2640R2_LAUNCHXL_UART0, &uartParams);
}
//******************************************************************************
// fn : EVENT_Create
//
// brief : �����¼�
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
// fn : SEM_Create
//
// brief : ������ֵ�ź���
//
// param : none
//
// return : none
static Void SEM_Create(void)
{
  LCD_Print("KeySem Create\n",NULL);
  Semaphore_Params  semParams;
  Semaphore_Params_init(&semParams);
  semParams.mode = Semaphore_Mode_BINARY;
  
  keySemHandle = Semaphore_create(0,&semParams,NULL);
}
static Void QUEUE_Create(void)
{
  LCD_Print("Queue Create\n",NULL);

  myQueueHandle = Queue_create(NULL,NULL);
}
//******************************************************************************
// fn : StartTaskFunc
//
// brief : ��������
//
// param : a0 -> ϵͳ����0
//         a1 -> ϵͳ����1
//
// return : none
static Void StartTaskFunc(UArg a0,UArg a1)
{
  /* Open LED pins */
  ledPinHandle = PIN_open(&ledPinState, ledPinTable);
  PIN_setOutputValue(ledPinHandle, Board_LED0, 0);

  Board_initKeys(ButtonHandleFunc);
  //��ʼ����������
  UART_DriverInit();
  //��LCD
  dispHandle = Display_open(Display_Type_LCD,NULL);
  
  //��ʾ������Ϣ
  Display_print0(dispHandle,0,4,"TI-RTOS");
  Display_print0(dispHandle,1,0,"Queue Test");
  
  LCD_Print("StartTask Run\n",NULL);
  
  EVENT_Create();
  SEM_Create();
  QUEUE_Create();
  //����һ����������
  USER_createWorkTask(1,workTask1Func,&workTask1);
  USER_createWorkTask(2,workTask2Func,&workTask2);
  
  LCD_Print("StartTask Del\n",NULL);
  
  Task_destruct(&startTask);
}
//******************************************************************************
// fn : ButtonHandleFunc
//
// brief : ����ť�����¼�
//
// param : keys -> ��ť����
//
// return : none
static void ButtonHandleFunc(UChar keys)
{
  queue_rec.key = keys;
  Queue_enqueue(myQueueHandle,&queue_rec.elem);
  Semaphore_post(keySemHandle);    //֪ͨ�ź�������ť�Ѿ�����
}
//******************************************************************************
// fn : StartTaskFunc
//
// brief : ��������
//
// param : a0 -> ϵͳ����0
//         a1 -> ϵͳ����1
//
// return : none
static Void workTask1Func(UArg a0,UArg a1)
{

  for(;;)
  {
    LCD_Print("Task pend Events",NULL);
    
    Event_pend(myEventHandle, Event_Id_00|Event_Id_01, Event_Id_NONE,BIOS_WAIT_FOREVER);
    
    PIN_setOutputValue(ledPinHandle, Board_LED1,
                       !PIN_getOutputValue(Board_LED1));
    LCD_Print("Task pend Ok",NULL);
    PIN_setOutputValue(ledPinHandle, Board_LED1,
                       !PIN_getOutputValue(Board_LED1));
  }
}
static Void workTask2Func(UArg a0,UArg a1)
{
  UInt eventId = 0;
  char *keyInfo = NULL;
  Rec *rp;

  for(;;)
  {
    Semaphore_pend(keySemHandle,BIOS_WAIT_FOREVER);
    if(!Queue_empty(myQueueHandle))
    {
      rp = Queue_dequeue(myQueueHandle);
    }
    eventId = Event_Id_NONE;
    if(rp->key & KEY_BTN1)
    {
      eventId |= Event_Id_00;
      keyInfo = "Key1 pressed";
    }
    if(rp->key & KEY_BTN2)
    {
      eventId |= Event_Id_01;
      keyInfo = "Key2 pressed";
    }
    PIN_setOutputValue(ledPinHandle, Board_LED0,
                       !PIN_getOutputValue(Board_LED0));
    LCD_Print(keyInfo,NULL);
    

    PIN_setOutputValue(ledPinHandle, Board_LED0,
                       !PIN_getOutputValue(Board_LED0));
    
    //������ť�¼�
    Event_post(myEventHandle,eventId);
  }
}