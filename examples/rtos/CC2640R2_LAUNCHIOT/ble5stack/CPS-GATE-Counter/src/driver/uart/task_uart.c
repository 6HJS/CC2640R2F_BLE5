#include <stdio.h>
#include <string.h>

#include <xdc/std.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>

#include "board.h"
#include <ti/drivers/uart/UARTCC26XX.h>
#include "task_uart.h"
/*********************************************************************
 * LOCAL PARAMETER
 */
// Task configuration
#define UART_TASK_PRIORITY                     2
#define UART_TASK_STACK_SIZE                   644
Task_Struct uartTask;
Char uartTaskStack[UART_TASK_STACK_SIZE];

// Uart configuration
#define UART_BUF_LEN  200
UART_Handle UARTHandle;
UART_Params UARTparams;
uint8_t Uart_RxTempBuf[UART_BUF_LEN];
uint8_t Uart_TxTempLen;
uint8_t Uart_TxTempBuf[UART_BUF_LEN];
// Uart -> App  Callback
GY_UartRxBufCallback GY_UartReviceDataCallback;

// Event used to control the UART thread
static Event_Struct uartEvent;
static Event_Handle hUartEvent;

#define UARTTASK_RX_EVENT      Event_Id_00 

#define UARTTASK_TX_EVENT      Event_Id_01 

#define UARTTASK_EVENT_ALL ( UARTTASK_RX_EVENT | UARTTASK_TX_EVENT )

/*********************************************************************
 * LOCAL FUNCTIONS
 */
void TaskUART_taskInit(void);   
void TaskUART_taskFxn(UArg a0, UArg a1);
void Uart_WriteCallback(UART_Handle handle, void *txBuf, size_t size);
void Uart_ReadCallback(UART_Handle handle, void *rxBuf, size_t size);
void TaskUARTWrite(uint8_t *buf, uint16_t len);  //��ӡ�ַ���
void TaskUARTPrintf(const char* format, ...);   //printf��ӡ

/*********************************************************************
 * @fn      TaskUART_createTask
 *
 * @brief   Task creation function for the uart.
 *
 * @param   None.
 *
 * @return  None.
 */
void TaskUART_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = uartTaskStack;
  taskParams.stackSize = UART_TASK_STACK_SIZE;
  taskParams.priority = UART_TASK_PRIORITY;

  Task_construct(&uartTask, TaskUART_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      TaskUART_taskInit
 *
 * @brief   ���ڳ�ʼ��
 *
 * @param   None
 *
 * @return  None.
 */
void TaskUART_taskInit(void)
{
  UART_init();                                      //��ʼ��ģ��Ĵ��ڹ���
  UART_Params_init(&UARTparams);                    //��ʼ�����ڲ���
  UARTparams.baudRate = 115200;                     //���ڲ�����115200
  UARTparams.dataLength = UART_LEN_8;               //��������λ8
  UARTparams.stopBits = UART_STOP_ONE;              //����ֹͣλ1
  UARTparams.readDataMode = UART_DATA_BINARY;       //���ڽ������ݲ�������
  UARTparams.writeDataMode = UART_DATA_TEXT;        //���ڷ������ݲ�������
  UARTparams.readMode = UART_MODE_CALLBACK;         //�����첽��
  UARTparams.writeMode = UART_MODE_CALLBACK;        //�����첽д
  UARTparams.readEcho = UART_ECHO_OFF;              //���ڲ�����
  UARTparams.readReturnMode = UART_RETURN_NEWLINE;  //�����յ����з�ʱ���ص�
  UARTparams.readCallback = Uart_ReadCallback;      //���ڶ��ص�
  UARTparams.writeCallback = Uart_WriteCallback;    //����д�ص�
  
  UARTHandle = UART_open(Board_UART0, &UARTparams); //�򿪴���ͨ��
  UART_control(UARTHandle, UARTCC26XX_RETURN_PARTIAL_ENABLE,  NULL);   //������ղ��ֻص�
  
  UART_read(UARTHandle, Uart_RxTempBuf, UART_BUF_LEN);       //��һ�����ڶ�
}

/*********************************************************************
 * @fn      TaskUART_taskFxn
 *
 * @brief   ����������
 *
 * @param   None
 *
 * @return  None.
 */
void TaskUART_taskFxn(UArg a0, UArg a1)
{ 
  Event_Params evParams;
  Event_Params_init(&evParams);
  Event_construct(&uartEvent, &evParams);
  hUartEvent = Event_handle(&uartEvent);
  
  TaskUART_taskInit();

  while(1)
  {
    UInt events;
    events = Event_pend(hUartEvent,Event_Id_NONE, UARTTASK_EVENT_ALL, BIOS_WAIT_FOREVER);
    
    if(events & UARTTASK_RX_EVENT)
    {
      UART_read(UARTHandle, Uart_RxTempBuf, UART_BUF_LEN);     //�ٴδ�һ�����ڶ�
    }
    
    if(events & UARTTASK_TX_EVENT)
    {
      TaskUARTWrite(Uart_TxTempBuf, Uart_TxTempLen);  //���ڴ�ӡ����
    }
  }
}

/*********************************************************************
 * @fn      Uart_ReadCallback
 *
 * @brief   ���ڶ��ص�
 *
 * @param   handle -> ����ͨ��
 *          rxBuf -> ���ڽ������ݵ�ָ��
 *          size -> ���ڽ������ݵĳ���
 *
 * @return  None.
 */
void Uart_ReadCallback(UART_Handle handle, void *rxBuf, size_t size)
{ 
  //UART_write(UARTHandle, rxBuf, size);       //���Դ�ӡ
  Event_post(hUartEvent, UARTTASK_RX_EVENT);
  //UART_read(handle, Uart_RxTempBuf, UART_BUF_LEN);    //�ٴδ�һ�����ڶ�
  GY_UartReviceDataCallback(rxBuf, size);      //��app����һ�����ڶ��ص�
}

/*********************************************************************
 * @fn      Uart_WriteCallback
 *
 * @brief   ����д�ص�
 *
 * @param   handle -> ����ͨ��
 *          txBuf -> ���ڷ������ݵ�ָ��
 *          size -> ���ڷ������ݵĳ���
 *
 * @return  None.
 */
void Uart_WriteCallback(UART_Handle handle, void *txBuf, size_t size)
{
  
}

/*********************************************************************
 * @fn      GY_UartTask_Write
 *
 * @brief   ����д����
 *
 * @param   buf -> ��Ҫд������ָ��
 *          len -> ��Ҫд�����ݳ���
 *
 * @return  None.
 */
void TaskUARTWrite(uint8_t *buf, uint16_t len)
{
  UART_write(UARTHandle, buf, len);
}

/*********************************************************************
 * @fn      GY_UartTask_Printf
 *
 * @brief   ����д����������ϵͳprintf��
 *
 * @param   format -> ����������־λ������%d,%s��
 *          ... -> ��������
 *
 * @return  None.
 */
void TaskUARTPrintf(const char* format, ...)
{
  va_list arg;
  va_start(arg,format);
  uint8_t buf[UART_BUF_LEN];
  uint16_t len;
  len = vsprintf((char*)buf, format, arg);
  UART_write(UARTHandle, buf, len);
}

/*********************************************************************
 * @fn      HwUARTdoWrite
 *
 * @brief   ����д����������APP��ӡʹ�ã�
 *
 * @param   buf -> ����buf
 *          len -> ����len
 *          format -> ����������־λ������%d,%s��
 *          ... -> ��������
 *
 * @return  None.
 *
 *          ע�⣺buf��lenΪ�ַ�����ӡ��format��...Ϊprintf��ӡ����֧��ͬʱʹ��
 */
void TaskUARTdoWrite(uint8_t *buf, uint16_t len, const char* format, ...)
{
  if(buf == NULL)
  {
    va_list arg;
    va_start(arg,format);
    uint8_t pbuf[UART_BUF_LEN];
    uint16_t plen;
    plen = vsprintf((char*)pbuf, format, arg);
    Uart_TxTempLen = plen;
    memcpy(Uart_TxTempBuf, pbuf, plen);
  }
  else
  {
     Uart_TxTempLen = len;
     memcpy(Uart_TxTempBuf, buf, len);
  }
  Event_post(hUartEvent, UARTTASK_TX_EVENT);
}

/*********************************************************************
 * @fn      GY_UartTask_RegisterPacketReceivedCallback
 *
 * @brief   ע�ᴮ�ڽ��ջص����񣨽����ڽ��յ����ݴ���app����ȥ����
 *
 * @param   callback -> ���ڽ������ݻص�����������buf��len��
 *
 * @return  None.
 */
void GY_UartTask_RegisterPacketReceivedCallback(GY_UartRxBufCallback callback)
{
  GY_UartReviceDataCallback = callback;
}

