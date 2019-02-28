//*****************************************************************************
//  SENSOR CONTROLLER STUDIO EXAMPLE: ADC DATA STREAMER FOR LAUNCHPAD
//  Operating system: TI-RTOS
//
//  The Sensor Controller is used to sample and buffer a single ADC channel at
//  20 kHz. The ADC samples are stored in a circular buffer, and the Sensor
//  Controller maintains a head index indicating where the next sample will be
//  written. Likewise, the System CPU application maintains a tail index
//  indicating the next sample to be read and processed.
//
//  The Sensor Controller notifies the application when the buffer is half
//  full (or more). The application then wakes up, calculates the sum of 64-
//  samples chunks and transfers these sums over UART (115200 baud, 8-N-1).
//
//  Use a terminal window to connect to the LaunchPad's XDS110 Application/User
//  USB serial port.
//
//
//  Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/
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
#include "string.h"
#include "stdio.h"
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
 * @fn      scTaskAlertCallback
 *
 * @brief   sensor controller任务进程警报回调
 *
 * @param   none
 *
 * @return  None.
 */
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
void scTaskAlertCallback(void) {

    // Clear the ALERT interrupt source
    //scifClearAlertIntSource();

    // Indicate on LEDs whether the current ADC value is high and/or low
    TaskUARTdoWrite(0, 0, "adcValue:%d\r\n", scifTaskData.adcWindowMonitor.output.adcValue);

    // Acknowledge the alert event
    scifAckAlertEvents();
} // taskAlertCallback

/*********************************************************************
 * @fn      ScADCInit
 *
 * @brief   sensor controller串口进程任务初始化
 *          这个初始化函数是由sensor controller studio生成main函数改写
 *
 * @param   None
 *
 * @return  None.
 */
void ScADCInit(void) 
{
    // Initialize the Sensor Controller
    scifOsalInit();
    scifOsalRegisterCtrlReadyCallback(scCtrlReadyCallback);
    scifOsalRegisterTaskAlertCallback(scTaskAlertCallback);
    scifInit(&scifDriverSetup);
    scifStartRtcTicksNow(0x00010000);
}

/*********************************************************************
 * @fn      ScADCStart
 *
 * @brief   开始协处理器ADC采集任务
 *
 * @param   None
 *
 * @return  None.
 */
void ScADCStart(void)
{
    scifStartTasksNbl(BV(SCIF_ADC_WINDOW_MONITOR_TASK_ID));
}
