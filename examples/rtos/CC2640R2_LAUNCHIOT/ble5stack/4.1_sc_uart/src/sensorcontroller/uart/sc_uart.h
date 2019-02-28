#ifndef SERIAL_SC_UART_H
#define SERIAL_SC_UART_H

#ifdef __cplusplus
extern "C"
{
#endif


/*********************************************************************
 * SC uart��ʼ������
 */
void ScUARTInit(void);

void ScUARTWrite(uint8_t* buf, uint8_t len);


/*****************************************************
 * ���ڽ������ݻص�����������buf��len��
*/
typedef void (*GY_UartRxBufCallback)(uint8_t *buf, uint16_t len);

/*****************************************************
 * ע�ᴮ�ڽ��ջص����񣨽����ڽ��յ����ݴ���app����ȥ����
*/
void GY_UartTask_SC_RegisterPacketReceivedCallback(GY_UartRxBufCallback callback);




#ifdef __cplusplus
}
#endif

#endif /* SERIAL_SC_UART_H */
