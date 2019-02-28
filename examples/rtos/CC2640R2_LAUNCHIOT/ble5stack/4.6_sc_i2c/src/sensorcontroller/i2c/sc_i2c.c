//*****************************************************************************
//  SENSOR CONTROLLER STUDIO EXAMPLE: I2C LIGHT SENSOR
//  Operating system: TI-RTOS
//
//  Demonstrates use of the bit-banged I2C master interface by sampling the
//  OPT3001 light sensor on the CC2650 SensorTag.
//
//  The application is woken if the light sensor output value changes by more
//  than a configurable amount:
//  - If decreasing, the application blinks the red LED1 on the SensorTag.
//  - If increasing, the application blinks the green LED2 on the SensorTag.
//
//
//  Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//****************************************************************************/
#include "scif.h"
#include "task_uart.h"

#define BV(n)               (1 << (n))


// Display error message if the SCIF driver has been generated with incorrect operating system setting
#ifndef SCIF_OSAL_TIRTOS_H
    #error "SCIF driver has incorrect operating system configuration for this example. Please change to 'TI-RTOS' in the Sensor Controller Studio project panel and re-generate the driver."
#endif

// Display error message if the SCIF driver has been generated with incorrect target chip package
#ifndef SCIF_TARGET_CHIP_PACKAGE_QFN48_7X7_RGZ
    #error "SCIF driver has incorrect target chip package configuration for this example. Please change to 'QFN48 7x7 RGZ' in the Sensor Controller Studio project panel and re-generate the driver."
#endif



/*********************************************************************
 * LOCAL FUNCTIONS
 */
static int16_t BMA250E_MakeInt(uint8_t Low, uint8_t High);




void scCtrlReadyCallback(void) 
{

} // scCtrlReadyCallback



/*********************************************************************
 * @fn      scTaskAlertCallback
 *
 * @brief   sensor controller任务进程警报回调
 *
 * @param   none
 *
 * @return  None.
 */
void scTaskAlertCallback(void) 
{
    // Clear the ALERT interrupt source
    scifClearAlertIntSource();

    // The light sensor value is outside of the configured window ...
    uint8_t buffer[7] = {0};
    for(int i=0; i<7 ;i++)
    {
      buffer[i] = scifTaskData.i2cBma250e.output.positionBuf[i];
    }
    
    uint16_t bufXYZ[4] = {0,0,0,0};
    bufXYZ[0] = BMA250E_MakeInt(buffer[0], buffer[1]);
    bufXYZ[1] = BMA250E_MakeInt(buffer[2], buffer[3]);
    bufXYZ[2] = BMA250E_MakeInt(buffer[4], buffer[5]);
    bufXYZ[3] = (uint8_t)(buffer[6] / 2) + 24;
    
      
    TaskUARTdoWrite(0, 0, "X:%d  Y:%d  Z:%d\r\n", buffer[1], buffer[3], buffer[5]);    

    //Display_print3(dispHandle, 4, 0, "X:%d  Y:%d  Z:%d", buffer[1], buffer[3], buffer[5]);
    
    // Acknowledge the alert event
    scifAckAlertEvents();
} // scTaskAlertCallback


/*********************************************************************
 * @fn      ScI2CInit
 *
 * @brief   sensor controller串口进程任务初始化
 *          这个初始化函数是由sensor controller studio生成main函数改写
 *
 * @param   None
 *
 * @return  None.
 */
void ScI2CInit(void) 
{
    // Initialize the Sensor Controller
    scifOsalInit();
    scifOsalRegisterCtrlReadyCallback(scCtrlReadyCallback);
    scifOsalRegisterTaskAlertCallback(scTaskAlertCallback);
    scifInit(&scifDriverSetup);

    // Set the Sensor Controller task tick interval to 1 second
    scifStartRtcTicksNow(0x00010000);
} // GY_I2CTask_SC_Init


/*********************************************************************
 * @fn      ScI2CTrans
 *
 * @brief   开始协处理器I2C任务
 *
 * @param   None
 *
 * @return  None.
 */
void ScI2CTrans(void)
{
  scifStartTasksNbl(BV(SCIF_I2C_BMA250E_TASK_ID));
}

/*********************************************************************
 * @fn      BMA250E_MakeInt
 *
 * @brief   处理拿到的XYZ数据，转化成int型
 *
 * @param   Low -> 数据地位
 *          High -> 数据高位
 *
 * @return  XYZ的int型数据值
 */
static int16_t BMA250E_MakeInt(uint8_t Low, uint8_t High)
{
  uint16_t u;

  u = (High << 2) | (Low >> 6);

  return (int16_t)u;
}

