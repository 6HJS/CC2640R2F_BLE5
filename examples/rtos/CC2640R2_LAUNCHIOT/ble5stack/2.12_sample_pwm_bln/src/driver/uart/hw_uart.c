#include "board.h"
#include <ti/drivers/uart/UARTCC26XX.h>

#include "stdio.h"
#include "string.h"
#include "hw_uart.h"
#include "simple_peripheral.h"

/*********************************************************************
 * LOCAL PARAMETER
 */
UART_Handle UARTHandle;
UART_Params UARTparams;
uint8_t Uart_RxTempBuf[200];

GY_UartRxBufCallback GY_UartReviceDataCallback;
/*********************************************************************
 * LOCAL FUNCTIONS
 */
void Uart_WriteCallback(UART_Handle handle, void *txBuf, size_t size);
void Uart_ReadCallback(UART_Handle handle, void *rxBuf, size_t size);

/*********************************************************************
 * @fn      Uart_ReadCallback
 *
 * @brief   串口读回调
 *
 * @param   handle -> 串口通道
 *          rxBuf -> 串口接收数据的指针
 *          size -> 串口接收数据的长度
 *
 * @return  None.
 */
void Uart_ReadCallback(UART_Handle handle, void *rxBuf, size_t size)
{ 
  //UART_write(UARTHandle, rxBuf, size);       //回显打印
  UART_read(handle, Uart_RxTempBuf, 200);      //再次打开一个串口读
  if ( GY_UartReviceDataCallback )
  {
    GY_UartReviceDataCallback(rxBuf, size);  //给app任务一个串口读回调
  }
}

/*********************************************************************
 * @fn      Uart_WriteCallback
 *
 * @brief   串口写回调
 *
 * @param   handle -> 串口通道
 *          txBuf -> 串口发送数据的指针
 *          size -> 串口发送数据的长度
 *
 * @return  None.
 */
void Uart_WriteCallback(UART_Handle handle, void *txBuf, size_t size)
{
  //
}

/*********************************************************************
 * @fn      GY_UartTask_Init
 *
 * @brief   串口任务初始化
 *
 * @param   None
 *
 * @return  None.
 */
void HwUARTInit(void)
{
  UART_init();                                      //初始化模块的串口功能
  UART_Params_init(&UARTparams);                    //初始化串口参数
  UARTparams.baudRate = 115200;                     //串口波特率115200
  UARTparams.dataLength = UART_LEN_8;               //串口数据位8
  UARTparams.stopBits = UART_STOP_ONE;              //串口停止位1
  UARTparams.readDataMode = UART_DATA_BINARY;       //串口接收数据不做处理
  UARTparams.writeDataMode = UART_DATA_TEXT;        //串口发送数据不做处理
  UARTparams.readMode = UART_MODE_CALLBACK;         //串口异步读
  UARTparams.writeMode = UART_MODE_CALLBACK;        //串口异步写
  UARTparams.readEcho = UART_ECHO_OFF;              //串口不回显
  UARTparams.readReturnMode = UART_RETURN_NEWLINE;  //当接收到换行符时，回调
  UARTparams.readCallback = Uart_ReadCallback;      //串口读回调
  UARTparams.writeCallback = Uart_WriteCallback;    //串口写回调
  
  UARTHandle = UART_open(Board_UART0, &UARTparams); //打开串口通道
  UART_control(UARTHandle, UARTCC26XX_RETURN_PARTIAL_ENABLE,  NULL);   //允许接收部分回调
  
  UART_read(UARTHandle, Uart_RxTempBuf, 200);       //打开一个串口读
}

/*********************************************************************
 * @fn      GY_UartTask_Write
 *
 * @brief   串口写函数
 *
 * @param   buf -> 需要写的数据指针
 *          len -> 需要写的数据长度
 *
 * @return  None.
 */
void HwUARTWrite(uint8_t *buf, uint16_t len)
{
  UART_write(UARTHandle, buf, len);
}

/*********************************************************************
 * @fn      GY_UartTask_Printf
 *
 * @brief   串口写函数（类似系统printf）
 *
 * @param   format -> 不定参数标志位，例如%d,%s等
 *          ... -> 不定参数
 *
 * @return  None.
 */
void HwUARTPrintf(const char* format, ...)
{
  va_list arg;
  va_start(arg,format);
  uint8_t buf[200];
  uint16_t len;
  len = vsprintf((char*)buf, format, arg);
  UART_write(UARTHandle, buf, len);
}
/*********************************************************************
 * @fn      GY_UartTask_RegisterPacketReceivedCallback
 *
 * @brief   注册串口接收回调任务（将串口接收的数据传给app任务去处理）
 *
 * @param   callback -> 串口接收数据回调（包括数据buf及len）
 *
 * @return  None.
 */
void GY_UartTask_RegisterPacketReceivedCallback(GY_UartRxBufCallback callback)
{
  GY_UartReviceDataCallback = callback;
}