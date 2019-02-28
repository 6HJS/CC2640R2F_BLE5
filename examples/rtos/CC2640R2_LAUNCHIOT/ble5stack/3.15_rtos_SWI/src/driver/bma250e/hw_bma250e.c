#include "string.h"
#include "board.h"
#include <ti/mw/display/Display.h>

#include "task_uart.h"
#include "hw_i2c.h"

#include "hw_bma250e_Spec.h"
#include "hw_bma250e.h"


extern Display_Handle dispHandle;
/*********************************************************************
 * Local Functions
*/
static uint16_t BMA250E_MakeInt(uint8_t Low, uint8_t High);



/*********************************************************************
 * @fn      BMA250E_IDTest
 *
 * @brief   ���Զ�ȡBMA250E ID
 *
 * @param   None
 *
 * @return  None.
 */
void BMA250E_IDTest(void)
{
  bool status = false;
  uint8_t rxbuf[10];
  status = HwI2CGet(0x00, rxbuf, 1);
  if(status == true)
  {
    if(rxbuf[0] == 0xF9)
    TaskUARTdoWrite("ReadID_OK",10, NULL, NULL);
  }
  else
  {
    TaskUARTdoWrite("ReadID_ERR",11, NULL, NULL);
  }
}

/*********************************************************************
 * @fn      BMA250E_Init
 *
 * @brief   BMA250E��ʼ����������RANGE��BW��
 *
 * @param   NONE.
 *
 * @return  None.
 */
void BMA250E_Init(void)
{
  HwI2CSet(0x0F, 0x03);
  HwI2CSet(0x10, 0x08);
}

/*********************************************************************
 * @fn      BMA250E_MakeInt
 *
 * @brief   �����õ���XYZ���ݣ�ת����int��
 *
 * @param   Low -> ���ݵ�λ
 *          High -> ���ݸ�λ
 *
 * @return  XYZ��int������ֵ
 */
static uint16_t BMA250E_MakeInt(uint8_t Low, uint8_t High)
{
  uint16_t u;
  if(Low & 0x01)
  u = (High << 2) | (Low >> 6);

  return u;
}

/*********************************************************************
 * @fn      BMA250E_GetData
 *
 * @brief   ��ȡXYZ�Ĵ���ֵ
 *
 * @param   None
 *
 * @return  None.
 */
bool BMA250E_GetData(void)
{
  bool ret = false;
  uint8_t buffer[7] = {0,0,0,0,0,0,0};
  uint16_t bufXYZ[4] = {0,0,0,0};
  
  ret = HwI2CGet(0x02, buffer, 7);

  bufXYZ[0] = BMA250E_MakeInt(buffer[0], buffer[1]);
  bufXYZ[1] = BMA250E_MakeInt(buffer[2], buffer[3]);
  bufXYZ[2] = BMA250E_MakeInt(buffer[4], buffer[5]);
  bufXYZ[3] = (uint8_t)(buffer[6] / 2) + 24;
  
  TaskUARTdoWrite(NULL, NULL, "XYZ:\r\nX: %04ld, Y: %04ld, Z: %04ld\r\n",buffer[1],buffer[3],buffer[5]);

  //GY_UartTask_Printf("Temp: %d"\r\n,bufXYZ[3]);;  //��ӡ�¶�ֵ
  //Display_print0(dispHandle, 4, 0, "BMA250E");
  //Display_print1(dispHandle, 5, 0, "X: %ld", bufXYZ[0]);
  //Display_print1(dispHandle, 6, 0, "Y: %ld", bufXYZ[1]);
  //Display_print1(dispHandle, 7, 0, "Z: %ld", bufXYZ[2]);
  
  return ret;
}
/*********************************************************************
 * @fn      BMA250E_GetShortData
 *
 * @brief   ��ȡXYZ�Ĵ���ֵ
 *
 * @param   None
 *
 * @return  None.
 */
bool BMA250E_GetShortData(void)
{
  bool ret = false;
  uint8_t buffer[7] = {0,0,0,0,0,0,0};
  uint8_t bufXYZ[4] = {0,0,0,0};
  
  ret = HwI2CGet(0x02, buffer, 6);

  bufXYZ[0] = (int8_t)(buffer[1]);
  bufXYZ[1] = (int8_t)(buffer[3]);
  bufXYZ[2] = (int8_t)(buffer[5]);
  bufXYZ[3] = (uint8_t)(buffer[6] / 2) + 24;
    
  TaskUARTdoWrite(bufXYZ, 4, NULL, NULL);
  
  return ret;
}

/*********************************************************************
 * @fn      BMA250E_DisableFlag
 *
 * @brief   ���üĴ���
 *
 * @param   reg -> �Ĵ�����ַ
 *          flag -> ���ñ�־λ
 *
 * @return  None.
 */
bool BMA250E_DisableFlag(uint8_t reg, uint8_t flag)
{
  bool ret = false;
  ret = HwI2CSet(reg, ~flag); 
  return ret;
}

/*********************************************************************
 * @fn      BMA250E_GetSleep
 *
 * @brief   ��ȡ˯��ģʽ
 *
 * @param   SleepMode -> ˯��״̬
 *
 * @return  None.
 */
bool BMA250E_GetSleep(uint8_t SleepMode)
{
  bool  ret = FALSE;
  ret = HwI2CGet(0x11, &SleepMode, 1);
  return ret;
}

/*********************************************************************
 * @fn      BMA250E_SetSleep
 *
 * @brief   �����Ƿ�˯��
 *
 * @param   sleep -> �Ƿ�����
 *
 * @return  None.
 */
bool BMA250E_SetSleep(bool sleep)
{
  bool ret = false;
  uint8_t SleepMode = 0;
  ret = BMA250E_GetSleep(SleepMode);
  SleepMode = SleepMode & (~BMA250_SLEEP_MODE_MASK);
  if (sleep)
  {
    SleepMode |= BMA250_SLEEP_MODE_SUSPEND;
  }
  ret = HwI2CSet(0x11,SleepMode);           

  return ret;
} 

/*********************************************************************
 * @fn      BMA250E_GetRange
 *
 * @brief   ��ȡrange
 *
 * @param   None
 *
 * @return  None.
 */
uint8_t BMA250E_GetRange(void)
{
  uint8_t ret, range[1];
  
  HwI2CGet(0x0F, range, 1);
  
  range[0] = range[0] & BMA250_RANGE_MASK;
  switch (range[0])
  {
  case BMA250_RANGE_16G:
    ret = 16;
    break;
  case BMA250_RANGE_8G:
    ret = 8;
    break;
  case BMA250_RANGE_4G:
    ret = 4;
    break;
  case BMA250_RANGE_2G:
    ret = 2;
    break;
  default:
    ret = 0;
    break;
  }
  
  return ret;
}

/*********************************************************************
 * @fn      BMA250E_SetRange
 *
 * @brief   ����range
 *
 * @param   range -> range��С
 *
 * @return  None.
 */
void BMA250E_SetRange(uint8_t range)
{
  uint8_t flag;
  switch (range)
  {
  case 16:
    flag = BMA250_RANGE_16G;
    break;
  case 8:
    flag = BMA250_RANGE_8G;
    break;
  case 4:
    flag = BMA250_RANGE_4G;
    break;
  case 2:
  default:
    flag = BMA250_RANGE_2G;
    break;
  }
  
  HwI2CSet(0x0F, flag);
}





