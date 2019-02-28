#include "board.h"
#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPICC26XXDMA.h>

#include "hw_spi.h"
#include "task_uart.h"
/*********************************************************************
 * LOCAL PARAMETER
 */
SPI_Handle      SPIHandle;
SPI_Params      SPIparams;

/*********************************************************************
 * LOCAL FUNCTIONS
 */


/*********************************************************************
 * @fn      GY_SpiTask_Init
 *
 * @brief   SPI任务初始化及启动
 *
 * @param   spiIndex -> 0:Board_SPI0 | 1:Board_SPI1
 *          spiMode ->  0:SPI_MASTER | 1:SPI_SLAVE
 *
 * @return  None.
 */
void HwSPIInit(void)
{
  SPI_init();
  SPI_Params_init(&SPIparams);
  SPIparams.bitRate  = 1000000;                    //1MHz
  SPIparams.dataSize = 8; 
  SPIparams.frameFormat = SPI_POL1_PHA1;           //相位1极性1
  SPIparams.mode = SPI_MASTER;                     //SPI主从模式
  SPIparams.transferCallbackFxn = NULL;
  SPIparams.transferMode = SPI_MODE_BLOCKING;      //阻塞
  SPIparams.transferTimeout = SPI_WAIT_FOREVER;
}

/*********************************************************************
 * @fn      GY_SPI_Trans
 *
 * @brief   SPI通信函数
 *
 * @param   csnPin -> CSN引脚
 *          txbuf -> 发送数据
 *          txbuf -> 接收数据
 *          len -> 通信的数据长度
 *
 * @return  None.
 */
void HwSPITrans(uint8_t csnPin, uint8_t *txbuf, uint8_t *rxbuf ,uint16_t len)
{
  SPI_Transaction spiTransaction;
  spiTransaction.arg = NULL;
  spiTransaction.count = len;
  spiTransaction.txBuf = txbuf;
  spiTransaction.rxBuf = rxbuf;

  SPIHandle = SPI_open(Board_SPI, &SPIparams);
  
  int csnFlag;
  csnFlag = SPI_control(SPIHandle, SPICC26XXDMA_CMD_SET_CSN_PIN, &csnPin);  //选择CSN片选引脚
  if(csnFlag == SPI_STATUS_SUCCESS)
  {
    SPI_transfer(SPIHandle, &spiTransaction);
  }
  TaskUARTdoWrite(rxbuf+1, 3, NULL, NULL);
  
  SPI_transferCancel(SPIHandle);
  SPI_close(SPIHandle);
}



