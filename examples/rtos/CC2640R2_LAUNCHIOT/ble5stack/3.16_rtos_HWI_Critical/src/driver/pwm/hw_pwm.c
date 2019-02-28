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
  PWMparams.periodUnits = PWM_PERIOD_US;
  PWMparams.periodValue = 3000;
  PWMparams.dutyUnits = PWM_DUTY_US;
  PWMparams.dutyValue = 1500;
  PWMHandleR = PWM_open(Board_PWMRED, &PWMparams);
  //GREEN
  PWMparams.idleLevel = PWM_IDLE_LOW;
  PWMparams.periodUnits = PWM_PERIOD_US;
  PWMparams.periodValue = 3000;
  PWMparams.dutyUnits = PWM_DUTY_US;
  PWMparams.dutyValue = 1500;
  PWMHandleG = PWM_open(Board_PWMGREEN, &PWMparams);
  //BLUE
  PWMparams.idleLevel = PWM_IDLE_LOW;
  PWMparams.periodUnits = PWM_PERIOD_US;
  PWMparams.periodValue = 3000;
  PWMparams.dutyUnits = PWM_DUTY_US;
  PWMparams.dutyValue = 1500;
  PWMHandleB = PWM_open(Board_PWMBLUE, &PWMparams);
  //Buzzer
  PWMparams.idleLevel = PWM_IDLE_LOW;
  PWMparams.periodUnits = PWM_PERIOD_HZ;
  PWMparams.periodValue = 2700;
  PWMparams.dutyUnits = PWM_DUTY_FRACTION;
  PWMparams.dutyValue = PWM_DUTY_FRACTION_MAX/100;
  PWMHandleBuzzer = PWM_open(Board_PWMBuzzer, &PWMparams);
}

/*********************************************************************
 * @fn      GY_PWM_Start
 *
 * @brief   开启当前PWM引脚
 *
 * @param   None
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
 * @param   None
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
 * @fn      GY_PWM_Start
 *
 * @brief   开启当前PWM引脚
 *
 * @param   None
 *
 * @return  None.
 */
void HwPWMSetDuty(uint8_t RGBHandle, uint32_t value)
{

  switch(RGBHandle)
  {
    case 0:
      PWM_setDuty(PWMHandleR, value);
      break;
    case 1:

      PWM_setDuty(PWMHandleG, value);
      break;
    case 2:     
     PWM_setDuty(PWMHandleB, value);
      break;
    case 3:
      PWM_setDuty(PWMHandleBuzzer, value);
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
 * @param   None
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
 * @param   None
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
 * @fn      HwPWMBuzzer
 *
 * @brief   蜂鸣器控制函数
 *
 * @param   frequency -> PWM频率
 *
 * @return  None.
 */
void HwPWMBuzzer(uint16_t frequency)
{
  PWM_stop(PWMHandleBuzzer);
  PWM_close(PWMHandleBuzzer);
  PWMparams.idleLevel = PWM_IDLE_LOW;
  PWMparams.periodUnits = PWM_PERIOD_HZ;
  PWMparams.periodValue = frequency;
  PWMparams.dutyUnits = PWM_DUTY_FRACTION;
  PWMparams.dutyValue = PWM_DUTY_FRACTION_MAX/10;
  PWMHandleBuzzer = PWM_open(Board_PWMBuzzer, &PWMparams);
  PWM_start(PWMHandleBuzzer);
}