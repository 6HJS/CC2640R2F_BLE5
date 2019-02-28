#ifndef __DRITEK_BMA250_H__
#define __DRITEK_BMA250_H__

#ifdef __cplusplus
extern "C"
{
#endif


/*********************************************************************
 * ���Զ�ȡBMA250E ID
 */
void BMA250E_IDTest(void);
void BMA250E_Init(void);
bool BMA250E_GetData(void);
bool BMA250E_GetShortData(void);
bool BMA250E_DisableFlag(uint8_t reg, uint8_t flag);
bool BMA250E_GetSleep(uint8_t SleepMode);
bool BMA250E_SetSleep(bool sleep);
uint8_t BMA250E_GetRange(void);
void BMA250E_SetRange(uint8_t range);


#ifdef __cplusplus
}
#endif

#endif //__DRITEK_BMA250_H__


