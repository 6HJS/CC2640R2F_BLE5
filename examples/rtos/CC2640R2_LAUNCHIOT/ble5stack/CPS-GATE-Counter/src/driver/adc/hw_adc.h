#ifndef SERIAL_ADC_H
#define SERIAL_ADC_H

#ifdef __cplusplus
extern "C"
{
#endif


/*********************************************************************
 * ADC��ʼ������
 */
void HwADCInit(void);

/*********************************************************************
 * ��ȡADC�ɼ�ֵ
 */
int16_t HwADCRead(void);


#ifdef __cplusplus
}
#endif

#endif /* SERIAL_ADC_H */
