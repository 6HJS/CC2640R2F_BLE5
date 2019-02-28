#include "board.h"
#include <ti/drivers/ADC.h>

#include "hal_adc.h"

/*********************************************************************
 * LOCAL PARAMETER
 */   
ADC_Handle ADCHandle;
ADC_Params ADCparams;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      GY_AdcTask_Init
 *
 * @brief   ADC初始化
 *
 * @param   .
 *
 * @return  None.
 */
void HwADCInit(void)
{
  ADC_init();
  ADC_Params_init(&ADCparams);
  ADCHandle = ADC_open(CC2650_LAUNCHXL_ADC0, &ADCparams);
  if (ADCHandle != NULL) {
      ADC_close(ADCHandle);
  }
}

/*********************************************************************
 * @fn      GY_AdcValue_Read
 *
 * @brief   读取ADC采集值
 *
 * @param   .
 *
 * @return  None.
 */
int16_t HwADCRead(void)
{
  int16_t res;
  uint16_t adcValue;
  res = ADC_convert(ADCHandle, &adcValue);  //获取ADC值
  if (res == ADC_STATUS_SUCCESS)
  {
    return  adcValue;
  }
  else
  {
    return -1;
  }
}


