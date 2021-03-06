#ifndef SERIAL_IIC_H
#define SERIAL_IIC_H

#ifdef __cplusplus
extern "C"
{
#endif
  
#include <stdint.h>
#include <stdbool.h>
  #include <ti/drivers/I2C.h>

#define Slave_Addr        0x29 //0b0101001;

/*********************************************************************
 * I2C��ʼ������
 */
void HwI2CInit(void);

void HwI2CClose(void);

/*********************************************************************
 * I2C��ȡ�Ĵ�������
 */
bool HwI2CGet(uint8_t RegAddr, uint8_t *ReadBuf, uint8_t ReadLen);

/*********************************************************************
 * I2C���üĴ���
 */
bool HwI2CSet(uint8_t RegAddr, uint8_t WriteBuf);


#ifdef __cplusplus
}
#endif

#endif /* SERIAL_IIC_H */
