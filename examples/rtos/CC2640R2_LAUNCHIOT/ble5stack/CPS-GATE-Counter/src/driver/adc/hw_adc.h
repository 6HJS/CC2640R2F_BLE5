#ifndef SERIAL_ADC_H
#define SERIAL_ADC_H

#ifdef __cplusplus
extern "C"
{
#endif


/*********************************************************************
 * ADC初始化函数
 */
void HwADCInit(void);

/*********************************************************************
 * 读取ADC采集值
 */
int16_t HwADCRead(void);


#ifdef __cplusplus
}
#endif

#endif /* SERIAL_ADC_H */
