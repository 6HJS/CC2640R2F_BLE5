#include "board.h"
#include "ti/drivers/PWM.h"

#include "hw_pwm.h"

/*********************************************************************
 * LOCAL PARAMETER
 */
PWM_Handle PWMHandleR;
PWM_Handle PWMHandleG;
PWM_Handle PWMHandleB;
PWM_Handle PWMHandleBuzzer;

PWM_Params PWMparams;
/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      GY_PwmTask_Init
 *
 * @brief   PWM任务初始化及启动
 *
 * @param   pin -> Board_PWM0 - Board_PWM7
 *
 * @return  None.
 */
void HwPWMInit(void)
{
  PWM_init();
  PWM_Params_init(&PWMparams);
  //RED
  PWMparams.idleLevel = PWM_IDLE_LOW;
  PWMparams.periodUnits = PWM_PERIOD_HZ;
  PWMparams.periodValue = 100;
  PWMparams.dutyUnits = PWM_DUTY_US;
  PWMparams.dutyValue = 5000;
  PWMHandleR = PWM_open(CC2640R2_LAUNCHXL_PWM3, &PWMparams);
  //GREEN
  PWMparams.idleLevel = PWM_IDLE_LOW;
  PWMparams.periodUnits = PWM_PERIOD_HZ;
  PWMparams.periodValue = 100;
  PWMparams.dutyUnits = PWM_DUTY_US;
  PWMparams.dutyValue = 5000;
  PWMHandleG = PWM_open(CC2640R2_LAUNCHXL_PWM4, &PWMparams);
  //BLUE
  PWMparams.idleLevel = PWM_IDLE_LOW;
  PWMparams.periodUnits = PWM_PERIOD_HZ;
  PWMparams.periodValue = 100;
  PWMparams.dutyUnits = PWM_DUTY_US;
  PWMparams.dutyValue = 5000;
  PWMHandleB = PWM_open(CC2640R2_LAUNCHXL_PWM5, &PWMparams);
  //Buzzer
  PWMparams.idleLevel = PWM_IDLE_LOW;
  PWMparams.periodUnits = PWM_PERIOD_HZ;
  PWMparams.periodValue = 100;
  PWMparams.dutyUnits = PWM_DUTY_US;
  PWMparams.dutyValue = 2000;
  PWMHandleBuzzer = PWM_open(CC2640R2_LAUNCHXL_PWM2, &PWMparams);
}

/*********************************************************************
 * @fn      GY_PWM_Start
 *
 * @brief   开启当前PWM引脚
 *
 * @param   RGBHandle -> PWM通道
 *
 * @return  None.
 */
void HwPWMStart(uint8_t RGBHandle)
{
  switch(RGBHandle)
  {
    case 0:
      PWM_start(PWMHandleR);
      break;
    case 1:
      PWM_start(PWMHandleG);
      break;
    case 2:
      PWM_start(PWMHandleB);
      break;
    case 3:
      PWM_start(PWMHandleBuzzer);
      break;
    default:
      break;
  }
}

/*********************************************************************
 * @fn      GY_PWM_Stop
 *
 * @brief   暂停当前PWM引脚
 *
 * @param   RGBHandle -> PWM通道
 *
 * @return  None.
 */
void HwPWMStop(uint8_t RGBHandle)
{
  switch(RGBHandle)
  {
    case 0:
      PWM_stop(PWMHandleR);
      break;
    case 1:
      PWM_stop(PWMHandleG);
      break;
    case 2:
      PWM_stop(PWMHandleB);
      break;
    case 3:
      PWM_stop(PWMHandleBuzzer);
      break;
    default:
      break;
  }
}

/*********************************************************************
 * @fn      GY_PWM_Close
 *
 * @brief   关闭当前PWM引脚
 *
 * @param   RGBHandle -> PWM通道
 *
 * @return  None.
 */
void HwPWMClose(uint8_t RGBHandle)
{
  switch(RGBHandle)
  {
    case 0:
      PWM_stop(PWMHandleR);
      PWM_close(PWMHandleR);
      break;
    case 1:
      PWM_stop(PWMHandleG);
      PWM_close(PWMHandleG);
      break;
    case 2:
      PWM_stop(PWMHandleB);
      PWM_close(PWMHandleB);
      break;
    case 3:
      PWM_stop(PWMHandleBuzzer);
      PWM_close(PWMHandleBuzzer);
      break;
    default:
      break;
  }
}

/*********************************************************************
 * @fn      GY_PWM_CloseAll
 *
 * @brief   关闭所有PWM引脚
 *
 * @param   RGBHandle -> PWM通道
 *
 * @return  None.
 */
void HwPWMCloseAll(void)
{
  HwPWMStop(PWM_R);
  HwPWMStop(PWM_G);
  HwPWMStop(PWM_B);
}

/*********************************************************************
 * @fn      HwPWMSetDuty
 *
 * @brief   设置PWM占空比
 *          由于PWM初始化周期为100Hz（即0.01s == 10ms），所以duty取值0~10000us
 *
 * @param   RGBHandle -> PWM通道
 *          duty -> 占空比时间
 *
 * @return  None.
 */
void HwPWMSetDuty(uint8_t RGBHandle,uint32_t duty)
{
  switch(RGBHandle)
  {
    case 0:
      PWM_setDuty(PWMHandleR,duty);
      break;
    case 1:
      PWM_setDuty(PWMHandleG,duty);
      break;
    case 2:
      PWM_setDuty(PWMHandleB,duty);
      break;
    case 3:
      PWM_setDuty(PWMHandleBuzzer,duty);
      break;
    default:
      break;
  }            
}