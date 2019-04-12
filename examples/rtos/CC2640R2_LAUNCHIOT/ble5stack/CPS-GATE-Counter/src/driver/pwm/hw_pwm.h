#ifndef SERIAL_PWM_H
#define SERIAL_PWM_H

#ifdef __cplusplus
extern "C"
{
#endif

  
typedef enum CC2650_PWM_RGB {
    PWM_R = 0,
    PWM_G = 1,
    PWM_B = 2,
    PWM_Buzzer = 3,
    PWM_COUNT
} CC2650_PWM_RGBHandle;

/*****************************************************
 * PWM�����ʼ��
*/  
void HwPWMInit(void);

/*****************************************************
 * ������ǰPWM����
*/ 
void HwPWMStart(uint8_t RGBHandle);

/*****************************************************
 * ��ͣ��ǰPWM����
*/ 
void HwPWMStop(uint8_t RGBHandle);

/*****************************************************
 * �رյ�ǰPWM����
*/ 
void HwPWMClose(uint8_t RGBHandle);

void HwPWMCloseAll(void);

void HwPWMMusic(uint8_t i);

#ifdef __cplusplus
{
#endif // extern "C"

#endif // SERIAL_PWM_H