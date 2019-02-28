#ifndef SERIAL_SC_UART_H
#define SERIAL_SC_UART_H

#ifdef __cplusplus
extern "C"
{
#endif


/*********************************************************************
 * SC uart初始化函数
 */
void ScUARTInit(void);

void ScUARTWrite(uint8_t* buf, uint8_t len);


/*****************************************************
 * 串口接收数据回调（包括数据buf及len）
*/
typedef void (*GY_UartRxBufCallback)(uint8_t *buf, uint16_t len);

/*****************************************************
 * 注册串口接收回调任务（将串口接收的数据传给app任务去处理）
*/
void GY_UartTask_SC_RegisterPacketReceivedCallback(GY_UartRxBufCallback callback);




#ifdef __cplusplus
}
#endif

#endif /* SERIAL_SC_UART_H */
