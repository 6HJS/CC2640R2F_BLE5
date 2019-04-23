
/*
* Copyright (c) 2017, STMicroelectronics - All Rights Reserved
*
* This file is part of VL53L1 Core and is dual licensed,
* either 'STMicroelectronics
* Proprietary license'
* or 'BSD 3-clause "New" or "Revised" License' , at your option.
*
********************************************************************************
*
* 'STMicroelectronics Proprietary license'
*
********************************************************************************
*
* License terms: STMicroelectronics Proprietary in accordance with licensing
* terms at www.st.com/sla0081
*
* STMicroelectronics confidential
* Reproduction and Communication of this document is strictly prohibited unless
* specifically authorized in writing by STMicroelectronics.
*
*
********************************************************************************
*
* Alternatively, VL53L1 Core may be distributed under the terms of
* 'BSD 3-clause "New" or "Revised" License', in which case the following
* provisions apply instead of the ones mentioned above :
*
********************************************************************************
*
* License terms: BSD 3-clause "New" or "Revised" License.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors
* may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*
********************************************************************************
*
*/

#include "hw_i2c.h"
#include "vl53l1_platform.h"
// #include "vl53l1_platform_log.h"
#include "vl53l1_api.h"

// #include "stm32xxx_hal.h"
#include <string.h>
// #include <time.h>
// #include <math.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/I2C.h>
#include <ti/sysbios/knl/Task.h>
#include "hw_i2c.h"
#include <_hal_types.h>

extern I2C_Handle I2CHandle;


// #define I2C_TIME_OUT_BASE   10
// #define I2C_TIME_OUT_BYTE   1

// #ifdef VL53L1_LOG_ENABLE
// #define trace_print(level, ...) VL53L1_trace_print_module_function(VL53L1_TRACE_MODULE_PLATFORM, level, VL53L1_TRACE_FUNCTION_NONE, ##__VA_ARGS__)
// #define trace_i2c(...) VL53L1_trace_print_module_function(VL53L1_TRACE_MODULE_NONE, VL53L1_TRACE_LEVEL_NONE, VL53L1_TRACE_FUNCTION_I2C, ##__VA_ARGS__)
// #endif

// #ifndef HAL_I2C_MODULE_ENABLED
// #warning "HAL I2C module must be enable "
// #endif

//extern I2C_HandleTypeDef hi2c1;
//#define VL53L0X_pI2cHandle    (&hi2c1)

/* when not customized by application define dummy one */
// #ifndef VL53L1_GetI2cBus
/** This macro can be overloaded by user to enforce i2c sharing in RTOS context
 */
// #   define VL53L1_GetI2cBus(...) (void)0
// #endif

// #ifndef VL53L1_PutI2cBus
/** This macro can be overloaded by user to enforce i2c sharing in RTOS context
 */
// #   define VL53L1_PutI2cBus(...) (void)0
// #endif

// uint8_t _I2CBuffer[256];

// int _I2CWrite(VL53L1_DEV Dev, uint8_t *pdata, uint32_t count) {
//     int status = 0;
//     return status;
// }

// int _I2CRead(VL53L1_DEV Dev, uint8_t *pdata, uint32_t count) {
//    int status = 0;
//    return Status;
// }
#define BUF_SZ 2+32 // buffer size
VL53L1_Error VL53L1_WriteMulti(VL53L1_DEV Dev, uint16_t index, uint8_t *pdata, uint32_t count) {
  while (count > 0){
    uint8_t buf[BUF_SZ] = {0};
    uint8_t byte_trans = 0;
    //Wire.beginTransmission(Dev->I2cDevAddr >> 1);
    I2C_Transaction i2cTransaction;
    i2cTransaction.slaveAddress = Slave_Addr;
    
	
	
    
    //Wire.write((index >> 8) & 0xff);
    //Wire.write(index & 0xff);
	buf[byte_trans] = (index >> 8) & 0xff;
	++byte_trans;
	buf[byte_trans] = index & 0xff;

    uint8_t writing = 0;

    //while (count > 0 && Wire.write(*pdata) != 0){
	while(count >0 && byte_trans < BUF_SZ){
	  buf[byte_trans] = *pdata;
	  ++byte_trans;
      pdata++;
      writing++;
      count--;
    }

    //if (writing == 0 || Wire.endTransmission() != 0) { return VL53L1_ERROR_CONTROL_INTERFACE; }
    //index += writing;
    i2cTransaction.readBuf = NULL;
    i2cTransaction.readCount = 0;
    i2cTransaction.writeBuf = buf;
    i2cTransaction.writeCount = byte_trans;
    i2cTransaction.arg = NULL;
    bool ret = I2C_transfer(I2CHandle, &i2cTransaction);
	if(writing == 0 || ret == false){return VL53L1_ERROR_CONTROL_INTERFACE;}
	index += writing;
  }

  return VL53L1_ERROR_NONE;
}

// the ranging_sensor_comms.dll will take care of the page selection
VL53L1_Error VL53L1_ReadMulti(VL53L1_DEV Dev, uint16_t index, uint8_t *pdata, uint32_t count) {
	uint8_t Wrbuf[2] = {0};
	//Wire.beginTransmission(Dev->I2cDevAddr >> 1);
	I2C_Transaction i2cTransaction;
    i2cTransaction.slaveAddress = Slave_Addr;
		
	//Wire.write((index >> 8) & 0xff);
	//Wire.write(index & 0xff);
	Wrbuf[0] = (index >> 8) & 0xff;
	Wrbuf[1] = index & 0xff;
	
	//if (Wire.endTransmission() != 0) { return VL53L1_ERROR_CONTROL_INTERFACE; }
	//put this at the end
	//while (count > 0){
		//uint8_t reading = Wire.requestFrom(Dev->I2cDevAddr >> 1, count);

		//if (reading == 0) { return VL53L1_ERROR_CONTROL_INTERFACE; }
		//count -= reading;

		//while (reading-- > 0){
		  //*pdata++ = Wire.read();
		//}
	//}
	  bool ret = false;
	  i2cTransaction.writeBuf = Wrbuf;
	  i2cTransaction.writeCount = 2;
	  i2cTransaction.readBuf = pdata;
	  i2cTransaction.readCount = count;
	  i2cTransaction.slaveAddress = Slave_Addr;
	  i2cTransaction.arg = NULL;
	  ret = I2C_transfer(I2CHandle, &i2cTransaction);
	  if(ret == false){return VL53L1_ERROR_CONTROL_INTERFACE;}
	

  return VL53L1_ERROR_NONE;
}

VL53L1_Error VL53L1_WrByte(VL53L1_DEV Dev, uint16_t index, uint8_t data) {
	bool ret = false;
	uint8_t buf[3] = {0};
	buf[0] = (index >> 8) & 0xff;
	buf[1] = index & 0xff;
	buf[2] = data;
  
	I2C_Transaction i2cTransaction;
	i2cTransaction.writeBuf = buf;
	i2cTransaction.writeCount = 3;
	i2cTransaction.readBuf = NULL;
	i2cTransaction.readCount = 0;
	i2cTransaction.slaveAddress = Slave_Addr;
	i2cTransaction.arg = NULL;
	ret = I2C_transfer(I2CHandle, &i2cTransaction);
	if(ret == false){return VL53L1_ERROR_CONTROL_INTERFACE;}
    return VL53L1_ERROR_NONE;
}

VL53L1_Error VL53L1_WrWord(VL53L1_DEV Dev, uint16_t index, uint16_t data) {
	bool ret = false;
	uint8_t buf[6] = {0};
	buf[0] = (index >> 8) & 0xff;
	buf[1] = index & 0xff;
	buf[2] = (data >> 8) & 0xff;
	buf[3] = data & 0xff;;
  
	I2C_Transaction i2cTransaction;
	i2cTransaction.writeBuf = buf;
	i2cTransaction.writeCount = 4;
	i2cTransaction.readBuf = NULL;
	i2cTransaction.readCount = 0;
	i2cTransaction.slaveAddress = Slave_Addr;
	i2cTransaction.arg = NULL;
	ret = I2C_transfer(I2CHandle, &i2cTransaction);
	if(ret == false){return VL53L1_ERROR_CONTROL_INTERFACE;}
    return VL53L1_ERROR_NONE;
	
}

VL53L1_Error VL53L1_WrDWord(VL53L1_DEV Dev, uint16_t index, uint32_t data) {
	bool ret = false;
	uint8_t buf[6] = {0};
	buf[0] = (index >> 8) & 0xff;
	buf[1] = index & 0xff;
	buf[2] = (data >> 24) & 0xff;
	buf[3] = (data >> 16) & 0xff;
	buf[4] = (data >> 8) & 0xff;
	buf[5] = data & 0xff;
  
	I2C_Transaction i2cTransaction;
	i2cTransaction.writeBuf = buf;
	i2cTransaction.writeCount = 6;
	i2cTransaction.readBuf = NULL;
	i2cTransaction.readCount = 0;
	i2cTransaction.slaveAddress = Slave_Addr;
	i2cTransaction.arg = NULL;
	ret = I2C_transfer(I2CHandle, &i2cTransaction);
	if(ret == false){return VL53L1_ERROR_CONTROL_INTERFACE;}
    return VL53L1_ERROR_NONE;
}

VL53L1_Error VL53L1_UpdateByte(VL53L1_DEV Dev, uint16_t index, uint8_t AndData, uint8_t OrData) {
	uint8_t data;
	VL53L1_Error status = VL53L1_RdByte(Dev, index, &data);
	if (status != VL53L1_ERROR_NONE) { return status; }

	data &= AndData;
	data |= OrData;
	return VL53L1_WrByte(Dev, index, data);
}

VL53L1_Error VL53L1_RdByte(VL53L1_DEV Dev, uint16_t index, uint8_t *data) {
    bool ret = false;
	uint8_t buf[3] = {0};
	buf[0] = (index >> 8) & 0xff;
	buf[1] = index & 0xff;
  
	I2C_Transaction i2cTransaction;
	i2cTransaction.writeBuf = buf;
	i2cTransaction.writeCount = 2;
	i2cTransaction.readBuf = data;
	i2cTransaction.readCount = 1;
	i2cTransaction.slaveAddress = Slave_Addr;
	i2cTransaction.arg = NULL;
	ret = I2C_transfer(I2CHandle, &i2cTransaction);
	if(ret == false){return VL53L1_ERROR_CONTROL_INTERFACE;}
    return VL53L1_ERROR_NONE;
}

VL53L1_Error VL53L1_RdWord(VL53L1_DEV Dev, uint16_t index, uint16_t *data) {
    bool ret = false;
	uint8_t buf[3] = {0};
	buf[0] = (index >> 8) & 0xff;
	buf[1] = index & 0xff;
  
	I2C_Transaction i2cTransaction;
	i2cTransaction.writeBuf = buf;
	i2cTransaction.writeCount = 2;
	i2cTransaction.readBuf = data;
	i2cTransaction.readCount = 2;
	i2cTransaction.slaveAddress = Slave_Addr;
	i2cTransaction.arg = NULL;
	ret = I2C_transfer(I2CHandle, &i2cTransaction);
	if(ret == false){return VL53L1_ERROR_CONTROL_INTERFACE;}
    return VL53L1_ERROR_NONE;
}

VL53L1_Error VL53L1_RdDWord(VL53L1_DEV Dev, uint16_t index, uint32_t *data) {
    bool ret = false;
	uint8_t buf[3] = {0};
	buf[0] = (index >> 8) & 0xff;
	buf[1] = index & 0xff;
  
	I2C_Transaction i2cTransaction;
	i2cTransaction.writeBuf = buf;
	i2cTransaction.writeCount = 2;
	i2cTransaction.readBuf = data;
	i2cTransaction.readCount = 4;
	i2cTransaction.slaveAddress = Slave_Addr;
	i2cTransaction.arg = NULL;
	ret = I2C_transfer(I2CHandle, &i2cTransaction);
	if(ret == false){return VL53L1_ERROR_CONTROL_INTERFACE;}
    return VL53L1_ERROR_NONE;
}

VL53L1_Error VL53L1_GetTickCount(uint32_t *ptick_count_ms){
	*ptick_count_ms = Clock_getTicks();
	VL53L1_Error status  = VL53L1_ERROR_NONE;
	return status;
}

//#define trace_print(level, ...) \
//	_LOG_TRACE_PRINT(VL53L1_TRACE_MODULE_PLATFORM, \
//	level, VL53L1_TRACE_FUNCTION_NONE, ##__VA_ARGS__)

//#define trace_i2c(...) \
//	_LOG_TRACE_PRINT(VL53L1_TRACE_MODULE_NONE, \
//	VL53L1_TRACE_LEVEL_NONE, VL53L1_TRACE_FUNCTION_I2C, ##__VA_ARGS__)

VL53L1_Error VL53L1_GetTimerFrequency(int32_t *ptimer_freq_hz)
{
	return VL53L1_ERROR_NOT_IMPLEMENTED;
}

VL53L1_Error VL53L1_WaitMs(VL53L1_Dev_t *pdev, int32_t wait_ms){
    Task_sleep(wait_ms*(1000 / Clock_tickPeriod));
	VL53L1_Error status  = VL53L1_ERROR_NONE;
	return status;
}

#pragma optimize=none
VL53L1_Error VL53L1_WaitUs(VL53L1_Dev_t *pdev, int32_t wait_us){
	while(wait_us > 0){
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		ASM_NOP;
		wait_us--;
	}
	VL53L1_Error status  = VL53L1_ERROR_NONE;
	return status;
}

#define minD(a,b) (((a)<(b))?(a):(b))

VL53L1_Error VL53L1_WaitValueMaskEx(
	VL53L1_Dev_t *pdev,
	uint32_t      timeout_ms,
	uint16_t      index,
	uint8_t       value,
	uint8_t       mask,
	uint32_t      poll_delay_ms)
{
	uint8_t data;
	VL53L1_Error status;

	while (timeout_ms > 0){
		status = VL53L1_RdByte(pdev, index, &data);
		if (status != VL53L1_ERROR_NONE) { return status; }
		if ((data & mask) == value) { return VL53L1_ERROR_NONE; }
		Task_sleep(poll_delay_ms*(1000 / Clock_tickPeriod));
		timeout_ms -= minD(poll_delay_ms, timeout_ms);
	}
  return VL53L1_ERROR_TIME_OUT;
}




