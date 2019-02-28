//*****************************************************************************
//  SENSOR CONTROLLER STUDIO EXAMPLE: UART EMULATOR FOR LAUNCHPAD
//  Operating system: TI-RTOS
//
//  The Sensor Controller runs a UART Emulator, which implements an full-duplex
//  UART interface using bit-banging. The application example uses a small
//  subset of the supplied driver API to implement loopback, echoing each
//  received character (57600 baud, 8-N-2).
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
#include "sc_uart.h"

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
 * LOCAL PARAMETER
 */
GY_UartRxBufCallback GY_UartReviceDataCallback;


/*********************************************************************
 * @fn      GY_UartTask_RegisterPacketReceivedCallback
 *
 * @brief   ע�ᴮ�ڽ��ջص����񣨽����ڽ��յ����ݴ���app����ȥ����
 *
 * @param   callback -> ���ڽ������ݻص�����������buf��len��
 *
 * @return  None.
 */
void GY_UartTask_SC_RegisterPacketReceivedCallback(GY_UartRxBufCallback callback)
{
  GY_UartReviceDataCallback = callback;
}



void scCtrlReadyCallback(void) {

}

/*********************************************************************
 * @fn      scTaskAlertCallback
 *
 * @brief   sensor controller������̾����ص����������ͬ�ڴ���RX�Ľ��ջص�
 *          ���ǿ��Դ������õ�UART RX���յ�������
 *
 * @param   none
 *
 * @return  None.
 */
void scTaskAlertCallback(void) {

    // Clear the ALERT interrupt source
    scifClearAlertIntSource();

    // Echo all characters currently in the RX FIFO
    int rxFifoCount = scifUartGetRxFifoCount();
    
    uint8_t rxbuf[200] = {0};
    for(int i=0; i<rxFifoCount; i++)
    {
      rxbuf[i] = scifUartRxGetChar();
    }
    //GY_UartReviceDataCallback(rxbuf, rxFifoCount);      //��app����һ�����ڶ��ص�
    ScUARTWrite(rxbuf, rxFifoCount);   //���Դ�ӡ
    
    // Clear the events that triggered this
    scifUartClearEvents();

    // Acknowledge the alert event
    scifAckAlertEvents();
    
}

/*********************************************************************
 * @fn      GY_UartTask_SC_Write
 *
 * @brief   ����д����
 *
 * @param   buf -> ��Ҫд������ָ��
 *          len -> ��Ҫд�����ݳ���
 *
 * @return  None.
 */
void ScUARTWrite(uint8_t* buf, uint8_t len)
{
  for(int i=0; i<len; i++)
  {
    scifUartTxPutChar((char) buf[i]);
  }
}

/*********************************************************************
 * @fn      GY_UartTask_SC_Init
 *
 * @brief   sensor controller���ڽ��������ʼ��
 *          �����ʼ����������sensor controller studio����main������д
 *
 * @param   None
 *
 * @return  None.
 */
void ScUARTInit (void)
{
    // Initialize the Sensor Controller
    scifOsalInit();
    scifOsalRegisterCtrlReadyCallback(scCtrlReadyCallback);
    scifOsalRegisterTaskAlertCallback(scTaskAlertCallback);
    scifInit(&scifDriverSetup);

    // Start the UART emulator
    scifExecuteTasksOnceNbl(BV(SCIF_UART_EMULATOR_TASK_ID));

    // Enable baud rate generation
    scifUartSetBaudRate(115200);

    // Enable RX (10 idle bit periods required before enabling start bit detection)
    scifUartSetRxFifoThr(SCIF_UART_RX_FIFO_MAX_COUNT / 2);
    scifUartSetRxTimeout(10 * 2);
    scifUartSetRxEnableReqIdleCount(10 * 2);
    scifUartRxEnable(1);

    // Enable events (half full RX FIFO or 10 bit period timeout
    scifUartSetEventMask(BV_SCIF_UART_ALERT_RX_FIFO_ABOVE_THR | BV_SCIF_UART_ALERT_RX_BYTE_TIMEOUT);

}
