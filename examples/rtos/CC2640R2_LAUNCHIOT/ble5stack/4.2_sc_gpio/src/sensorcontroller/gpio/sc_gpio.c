//*****************************************************************************
//  SENSOR CONTROLLER STUDIO EXAMPLE: LED BLINKER FOR LAUNCHPAD
//  Operating system: TI-RTOS
//
//  The Sensor Controller blinks the red LED on the LaunchPad. For every N'th
//  LED toggling, a counter value (output.counter) is communicated to the
//  application, which uses this value to blink the green LED.
//
//  This examples also demonstrates starting and stopping the Sensor Controller
//  task by using the LaunchPad push buttons:
//  - BTN1: Starts the LED Blinker task
//  - BTN2: Stops the LED Blinker task
//
//
//  Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
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
#include <ti/display/Display.h>

#define BV(n)               (1 << (n))


// Display error message if the SCIF driver has been generated with incorrect operating system setting
#ifndef SCIF_OSAL_TIRTOS_H
    #error "SCIF driver has incorrect operating system configuration for this example. Please change to 'TI-RTOS' in the Sensor Controller Studio project panel and re-generate the driver."
#endif

// Display error message if the SCIF driver has been generated with incorrect target chip package
#ifndef SCIF_TARGET_CHIP_PACKAGE_QFN48_7X7_RGZ
    #error "SCIF driver has incorrect target chip package configuration for this example. Please change to 'QFN48 7x7 RGZ' in the Sensor Controller Studio project panel and re-generate the driver."
#endif

static uint32_t RLED_BlinkPeriod_count = 0;



void scCtrlReadyCallback(void) {

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

  TaskUARTdoWrite(0, 0, "RLED_BlinkPriod_Count: %ld\r\n",RLED_BlinkPeriod_count++);

  // Acknowledge the ALERT event
  scifAckAlertEvents();

} // scTaskAlertCallback

/*********************************************************************
 * @fn      ScGPIOEnable
 *
 * @brief   使能协处理器GPIO控制
 *
 * @param   None
 *
 * @return  None.
 */
void ScGPIOEnable(void) 
{
  while (scifWaitOnNbl(0) != SCIF_SUCCESS);
  scifStartTasksNbl(BV(SCIF_LED_BLINKER_TASK_ID));
}

/*********************************************************************
 * @fn      ScGPIOInit
 *
 * @brief   sensor controller串口进程任务初始化
 *          这个初始化函数是由sensor controller studio生成main函数改写
 *
 * @param   None
 *
 * @return  None.
 */
void ScGPIOInit(void) 
{
  // Initialize and start the Sensor Controller
  scifOsalInit();
  scifOsalRegisterCtrlReadyCallback(scCtrlReadyCallback);
  scifOsalRegisterTaskAlertCallback(scTaskAlertCallback);
  scifInit(&scifDriverSetup);
  scifStartRtcTicksNow(0x00010000 / 10);

}
