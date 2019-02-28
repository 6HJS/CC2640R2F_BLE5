#ifndef SERIAL_IIC_H
#define SERIAL_IIC_H

#ifdef __cplusplus
extern "C"
{
#endif


/*********************************************************************
 * I2C初始化函数
 */
void HwI2CInit(void);

/*********************************************************************
 * I2C获取寄存器数据
 */
bool HwI2CGet(uint8_t RegAddr, uint8_t *ReadBuf, uint8_t ReadLen);

/*********************************************************************
 * I2C配置寄存器
 */
bool HwI2CSet(uint8_t RegAddr, uint8_t WriteBuf);


#ifdef __cplusplus
}
#endif

#endif /* SERIAL_IIC_H */
