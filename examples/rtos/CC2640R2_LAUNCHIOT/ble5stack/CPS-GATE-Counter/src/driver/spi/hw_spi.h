#ifndef SERIAL_SPI_H
#define SERIAL_SPI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <ti/drivers/SPI.h>
  
/*****************************************************
 * SPI�����ʼ��
*/  
void HwSPIInit(void);  

/*****************************************************
 * SPIͨ�ź���
*/ 
void HwSPITrans(uint8_t csnPin, uint8_t *txbuf, uint8_t *rxbuf ,uint16_t len);



#ifdef __cplusplus
{
#endif // extern "C"

#endif // SERIAL_SPI_H