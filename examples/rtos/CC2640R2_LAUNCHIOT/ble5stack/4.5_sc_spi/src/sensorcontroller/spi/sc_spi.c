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
  //scifClearAlertIntSource();
  
  uint8_t FlashIdBuf[2] = {0,0};
  FlashIdBuf[0] = scifTaskData.spiForLaunchpad.output.ManufacturerID;
  FlashIdBuf[1] = scifTaskData.spiForLaunchpad.output.DeviceID;
  
  // 串口打印ID
  TaskUARTdoWrite(0, 0, "ManufacturerID:%02X,DeviceID:%02X\r\n", FlashIdBuf[0], FlashIdBuf[1]); 
}

/*********************************************************************
 * @fn      ScSPIInit
 *
 * @brief   sensor controller串口进程任务初始化
 *          这个初始化函数是由sensor controller studio生成main函数改写
 *
 * @param   None
 *
 * @return  None.
 */
void ScSPIInit(void) 
{
    // Initialize the Sensor Controller
    scifOsalInit();
    scifOsalRegisterCtrlReadyCallback(scCtrlReadyCallback);
    scifOsalRegisterTaskAlertCallback(scTaskAlertCallback);
    scifInit(&scifDriverSetup);
}

/*********************************************************************
 * @fn      ScSPITrans
 *
 * @brief   开始协处理器SPI任务
 *
 * @param   None
 *
 * @return  None.
 */
void ScSPITrans(void)
{
  scifExecuteTasksOnceNbl(BV(SCIF_SPI_FOR_LAUNCHPAD_TASK_ID));
}

/*********************************************************************
 * @fn      ScSPINext
 *
 * @brief   控制协处理器SPI任务下一次运行
 *
 * @param   None
 *
 * @return  None.
 */
void ScSPINext(void)
{
  // Acknowledge the alert event
  scifAckAlertEvents();
}