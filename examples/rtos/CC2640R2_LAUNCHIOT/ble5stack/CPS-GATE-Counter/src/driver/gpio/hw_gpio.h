#ifndef SERIAL_GPIO_H
#define SERIAL_GPIO_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * GPIO��ʼ������
 */
void HwGPIOInit(void);

/*********************************************************************
 * ����GPIO��ƽ
 */
void HwGPIOSet(uint32_t pin, uint8_t flag);  


#ifdef __cplusplus
}
#endif

#endif /* SERIAL_GPIO_H */
