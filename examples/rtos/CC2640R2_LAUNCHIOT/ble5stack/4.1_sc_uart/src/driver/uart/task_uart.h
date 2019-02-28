#ifndef SERIAL_UART_H
#define SERIAL_UART_H

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************
 * ���������ʼ��
*/  
void TaskUART_createTask(void);

/*****************************************************
 * ����д����
*/
void TaskUARTdoWrite(uint8_t *buf, uint16_t len, const char* format, ...);

/*****************************************************
 * ���ڽ������ݻص�����������buf��len��
*/
typedef void (*GY_UartRxBufCallback)(uint8_t *buf, uint16_t len);

/*****************************************************
 * ע�ᴮ�ڽ��ջص����񣨽����ڽ��յ����ݴ���app����ȥ����
*/
void GY_UartTask_RegisterPacketReceivedCallback(GY_UartRxBufCallback callback);


void GY_UartFristRead(void);


/*****************************************************
 * ����������������������ʾ
*/
#  define Display_print0_Uarttask(handle, line, col, fmt) \
    TaskUARTdoWrite(NULL, NULL, fmt, NULL),TaskUARTdoWrite(NULL, NULL, "\r\n", NULL)

#  define Display_print1_Uarttask(handle, line, col, fmt, a0) \
    TaskUARTdoWrite(NULL, NULL, fmt, a0),TaskUARTdoWrite(NULL, NULL, "\r\n", NULL)
      
#  define Display_print2_Uarttask(handle, line, col, fmt, a0, a1) \
    TaskUARTdoWrite(NULL, NULL, fmt, a0, a1),TaskUARTdoWrite(NULL, NULL, "\r\n", NULL)

#  define Display_print3_Uarttask(handle, line, col, fmt, a0, a1, a2) \
    TaskUARTdoWrite(NULL, NULL, fmt, a0, a1, a2),TaskUARTdoWrite(NULL, NULL, "\r\n", NULL)


#ifdef __cplusplus
{
#endif // extern "C"

#endif // end of SERIAL_UART_H definition