// file name: BMA250_Spec.h
// Author: Jacky
// Create Time: 2015/01/28
// Last Modify Time: 2015/01/28
// Description:
//     BMA250 (3-Axis Digital Accelerometer IC)
//     use IIC to read/write data from/to BMA250

#ifndef __DRITEK_BMA250_SPEC_H__
#define __DRITEK_BMA250_SPEC_H__


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Address
#define BMA250_ADDRESS             ((uint8_t)(0x18)) 


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Register
#define BMA250_REG_DEVID               ((uint8_t)(0x00))
// R,   00000011(11111001), Device ID
#define BMA250_REG_RESERVED0           ((uint8_t)(0x01))
// R,   00000000, Reserved
#define BMA250_REG_X_LSB               ((uint8_t)(0x02))
// R  , 00000000, LSB of X-Axis (LSB[7:6] = X-Axis[1:0])
#define BMA250_REG_X_MSB               ((uint8_t)(0x03))
// R  , 00000000, LSB of X-Axis (MSB[7:0] = X-Axis[9:2])
#define BMA250_REG_Y_LSB               ((uint8_t)(0x04))
// R  , 00000000, LSB of Y-Axis (LSB[7:6] = Y-Axis[1:0])
#define BMA250_REG_Y_MSB               ((uint8_t)(0x05))
// R  , 00000000, LSB of Y-Axis (MSB[7:0] = Y-Axis[9:2])
#define BMA250_REG_Z_LSB               ((uint8_t)(0x06))
// R  , 00000000, LSB of Z-Axis (LSB[7:6] = Z-Axis[1:0])
#define BMA250_REG_Z_MSB               ((uint8_t)(0x07))
// R  , 00000000, LSB of Z-Axis (MSB[7:0] = Z-Axis[9:2])
#define BMA250_REG_TEMP                ((uint8_t)(0x08))
// R  , 00000000, Temperature data
#define BMA250_REG_INT_STATUS0         ((uint8_t)(0x09))
// R  , 00000000, The register contains interrupt status flags.
#define BMA250_REG_INT_STATUS1         ((uint8_t)(0x0A))
// R  , 00000000, The register contains interrupt status flags. 
#define BMA250_REG_INT_STATUS2         ((uint8_t)(0x0B))
// R  , 00000000, The register contains interrupt status flags. 
#define BMA250_REG_INT_STATUS3         ((uint8_t)(0x0C))
// R  , 00000000, The register contains interrupt status flags. 
#define BMA250_REG_RESERVED1           ((uint8_t)(0x0D))
// R/W, 11111111, Reserved
#define BMA250_REG_FIFO_STATUS         ((uint8_t)(0x0E))
// R,   00000000, FIFO status
#define BMA250_REG_PMU_RANGE           ((uint8_t)(0x0F))
// R/W, 00000011, Selection of accelerometer g-range.
#define BMA250_REG_PMU_BW              ((uint8_t)(0x10))
// R/W, 00001111, The register allows the selection of the acceleration data filter bandwidth.  
#define BMA250_REG_PMU_LPW             ((uint8_t)(0x11))
// R/W, 00000000, Selection of the main power modes and the low power sleep period. 
#define BMA250_REG_PMU_LOW_POWER       ((uint8_t)(0x12))
// R/W, 00000000, Configuration settings for low power mode. 
#define BMA250_REG_ACCD_HBW            ((uint8_t)(0x13))
// R/W, 00000000, Acceleration data acquisition and data output format. 
#define BMA250_REG_BGW_SOFTRESET       ((uint8_t)(0x14))
// W, 00000000, Software reset request.
#define BMA250_REG_RESERVED2           ((uint8_t)(0x15))
// R/W, 11111111, Reserved
#define BMA250_REG_INT_EN_0            ((uint8_t)(0x16))
// R/W, 00000000, Controls which interrupt engines in group 0 are enabled. 
#define BMA250_REG_INT_EN_1            ((uint8_t)(0x17))
// R/W, 00000000, Controls which interrupt engines in group 1 are enabled. 
#define BMA250_REG_INT_EN_2            ((uint8_t)(0x18))
// R/W, 00000000, Controls which interrupt engines in group 2 are enabled. 
#define BMA250_REG_INT_MAP_0           ((uint8_t)(0x19))
// R/W, 00000000, Controls which interrupt signals are mapped to the INT1 pin. 
#define BMA250_REG_INT_MAP_1           ((uint8_t)(0x1A))
// R/W, 00000000, Controls which interrupt signals are mapped to the INT1 and INT2 pins. 
#define BMA250_REG_INT_MAP_2           ((uint8_t)(0x1B))
// R/W, 00000000, Controls which interrupt signals are mapped to the INT2 pin. 
#define BMA250_REG_RESERVED3           ((uint8_t)(0x1C))
// R/W, 11111111, Reserved
#define BMA250_REG_RESERVED4           ((uint8_t)(0x1D))
// R/W, 11111111, Reserved
#define BMA250_REG_INT_SRC             ((uint8_t)(0x1E))
// R/W, 00000000, Contains the data source definition for interrupts with selectable data source. 
#define BMA250_REG_RESERVED5           ((uint8_t)(0x1F))
// R/W, 11111111, Reserved
#define BMA250_REG_INT_OUT_CTRL        ((uint8_t)(0x20))
// R/W, 00000101, Contains the behavioural configuration (electrical behaviour) of the interrupt pins. 
#define BMA250_REG_INT_RST_LATCH       ((uint8_t)(0x21))
// R/W, 00000000, Contains the interrupt reset bit and the interrupt mode selection. 
#define BMA250_REG_INT_0               ((uint8_t)(0x22))
// R/W, 00001001, Contains the delay time definition for the low-g interrupt. 
#define BMA250_REG_INT_1               ((uint8_t)(0x23))
// R/W, 00110000, Contains the threshold definition for the low-g interrupt. 
#define BMA250_REG_INT_2               ((uint8_t)(0x24))
// R/W, 00110000, Contains the low-g interrupt mode selection, the low-g interrupt hysteresis setting, and the high-g interrupt hysteresis setting.
#define BMA250_REG_INT_3               ((uint8_t)(0x25))
// R/W, 00001111, Contains the delay time definition for the high-g interrupt. 
#define BMA250_REG_INT_4               ((uint8_t)(0x26))
// R/W, 11000000, Contains the threshold definition for the high-g interrupt. 
#define BMA250_REG_INT_5               ((uint8_t)(0x27))
// R/W, 00000000, Contains the definition of the number of samples to be evaluated for the slope interrupt (any-motion detection) and the slow/no-motion interrupt trigger delay. 
#define BMA250_REG_INT_6               ((uint8_t)(0x28))
// R/W, 00010100, Contains the threshold definition for the any-motion interrupt. 
#define BMA250_REG_INT_7               ((uint8_t)(0x29))
// R/W, 00010100, Contains the threshold definition for the slow/no-motion interrupt. 
#define BMA250_REG_INT_8               ((uint8_t)(0x2A))
// R/W, 00000100, Contains the timing definitions for the single tap and double tap interrupts.
#define BMA250_REG_INT_9               ((uint8_t)(0x2B))
// R/W, 00000100, Contains the definition of the number of samples processed by the single / double-tap interrupt engine after wake-up in low-power mode.
#define BMA250_REG_INT_A               ((uint8_t)(0x2C))
// R/W, 00011000, Contains the definition of hysteresis, blocking, and mode for the orientation interrupt 
#define BMA250_REG_INT_B               ((uint8_t)(0x2D))
// R/W, 01001000, Contains the definition of the axis orientation, up/down masking, and the theta blocking angle for the orientation interrupt.
#define BMA250_REG_INT_C               ((uint8_t)(0x2E))
// R/W, 00001000, Contains the definition of the flat threshold angle for the flat interrupt.
#define BMA250_REG_INT_D               ((uint8_t)(0x2F))
// R/W, 00010001, Contains the definition of the flat interrupt hold time and flat interrupt hysteresis. 
#define BMA250_REG_FIFO_CONFIG_0       ((uint8_t)(0x30))
// R/W, 00000000, Contains the FIFO watermark level. 
#define BMA250_REG_RESERVED6           ((uint8_t)(0x31))
// R/W, 11111111, reserved
#define BMA250_REG_PMU_SELF_TEST       ((uint8_t)(0x32))
// R/W, 00000000, Contains the settings for the sensor self-test configuration and trigger. 
#define BMA250_REG_TRIM_NVM_CTRL       ((uint8_t)(0x33))
// R/W, 11110000, Contains the control settings for the few-time programmable non-volatile memory (NVM). 
#define BMA250_REG_BGW_SPI3_WDT        ((uint8_t)(0x34))
// R/W, 00000000, Contains settings for the digital interfaces. 
#define BMA250_REG_RESERVED7           ((uint8_t)(0x35))
// R/W, 00000000, reserved
#define BMA250_REG_OFC_CTRL            ((uint8_t)(0x36))
// R/W, 00010000, Contains control signals and configuration settings for the fast and the slow offset compensation. 
#define BMA250_REG_OFC_SETTING         ((uint8_t)(0x37))
// R/W, 00000000, Contains configuration settings for the fast and the slow offset compensation. 
#define BMA250_REG_OFC_OFFSET_X        ((uint8_t)(0x38))
// R/W, 00000000, Contains the offset compensation value for x-axis acceleration readout data. 
#define BMA250_REG_OFC_OFFSET_Y        ((uint8_t)(0x39))
// R/W, 00000000, Contains the offset compensation value for y-axis acceleration readout data. 
#define BMA250_REG_OFC_OFFSET_Z        ((uint8_t)(0x3A))
// R/W, 00000000, Contains the offset compensation value for z-axis acceleration readout data. 
#define BMA250_REG_TRIM_GP0            ((uint8_t)(0x3B))
// R/W, 00000000, Contains general purpose data register with NVM back-up. 
#define BMA250_REG_TRIM_GP1            ((uint8_t)(0x3C))
// R/W, 00000000, Contains general purpose data register with NVM back-up. 
#define BMA250_REG_RESERVED8           ((uint8_t)(0x3D))
// R/W, 11111111, reserved
#define BMA250_REG_FIFO_CONFIG_1       ((uint8_t)(0x3E))
// R/W, 00000000, Contains FIFO configuration settings.
#define BMA250_REG_FIFO_DATA           ((uint8_t)(0x3F))
// R, 00000000, FIFO data readout register. 


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x00 (BMA250_REG_DEVID)
#define BMA250_DEVICE_ID               ((uint8_t)(0x03))
#define BMA250E_DEVICE_ID              ((uint8_t)(0xF9))

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x09 (BMA250_REG_INT_STATUS0)
//                    0x19 (BMA250_REG_INT_MAP_0)
//                    0x21 (BMA250_REG_INT_MAP_2)
#define BMA250_FLAT_FLAG           ((uint8_t)(0x80))  //flat interrupt status
#define BMA250_ORIENT_FLAG         ((uint8_t)(0x40))  //orientation interrupt status
#define BMA250_S_TAP_FLAG          ((uint8_t)(0x20))  //single tap interrupt status
#define BMA250_D_TAP_FLAG          ((uint8_t)(0x10))  //double tap interrupt status
#define BMA250_SLO_NOT_MOT_FLAG    ((uint8_t)(0x08))  //slow/no-motion interrupt status
#define BMA250_SLOPE_FLAG          ((uint8_t)(0x04))  //slope interrupt status
#define BMA250_HIGH_FLAG           ((uint8_t)(0x02))  //high-g interrupt status
#define BMA250_LOW_FLAG            ((uint8_t)(0x01))  //low-g interrupt status


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x0A (BMA250_REG_INT_STATUS1)
#define BMA250_IS1_DATA_FLAG           ((uint8_t)(0x80))  //data ready interrupt status
#define BMA250_IS1_FIFO_WM_FLAG        ((uint8_t)(0x40))  //FIFO watermark interrupt status
#define BMA250_IS1_FIFO_FULL_FLAG      ((uint8_t)(0x20))  //FIFO full interrupt status
#define BMA250_IS1_RESERVED4_FLAG      ((uint8_t)(0x10))  //reserved
#define BMA250_IS1_RESERVED3_FLAG      ((uint8_t)(0x08))  //reserved
#define BMA250_IS1_RESERVED2_FLAG      ((uint8_t)(0x04))  //reserved
#define BMA250_IS1_RESERVED1_FLAG      ((uint8_t)(0x02))  //reserved
#define BMA250_IS1_RESERVED0_FLAG      ((uint8_t)(0x01))  //reserved


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x0B (BMA250_REG_INT_STATUS2)
#define BMA250_IS2_TAP_SIGN_FLAG       ((uint8_t)(0x80))  //sign of single/double tap triggering signal was '0'->positive, '1'->negative.
#define BMA250_IS2_TAP_FIRST_Z_FLAG    ((uint8_t)(0x40))  //single/double tap interrupt: '1'->triggered, or '0'->not triggered by z-axis.
#define BMA250_IS2_TAP_FIRST_Y_FLAG    ((uint8_t)(0x20))  //single/double tap interrupt: '1'->triggered, or '0'->not triggered by y-axis.
#define BMA250_IS2_TAP_FIRST_X_FLAG    ((uint8_t)(0x10))  //single/double tap interrupt: '1'->triggered, or '0'->not triggered by x-axis.
#define BMA250_IS2_SLOPE_SIGN_FLAG     ((uint8_t)(0x08))  //slope sign of slope tap triggering signal was '0'->positive, '1'->negative.
#define BMA250_IS2_SLOPE_FIRST_Z_FLAG  ((uint8_t)(0x04))  //slope interrupt: '1'->triggered, or '0'->not triggered by z-axis.
#define BMA250_IS2_SLOPE_FIRST_Y_FLAG  ((uint8_t)(0x02))  //slope interrupt: '1'->triggered, or '0'->not triggered by y-axis.
#define BMA250_IS2_SLOPE_FIRST_X_FLAG  ((uint8_t)(0x01))  //slope interrupt: '1'->triggered, or '0'->not triggered by x-axis.


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x0C (BMA250_REG_INT_STATUS3)
#define BMA250_IS3_FLAT_FLAG           ((uint8_t)(0x80))  //device is in '1'-> flat, '0'-> non flat position; only valid if (0x16)flat_en='1'.
#define BMA250_IS3_ORIENT2_FLAG        ((uint8_t)(0x40))  //Orientation value of z-axis.
#define BMA250_IS3_ORIENT10_MASK       ((uint8_t)(0x30))  //Orientation value of x-y-plane.
#define BMA250_IS3_HIGH_SIGN_FLAG      ((uint8_t)(0x08))  //sign of acceleration signal that triggered high-g interrupt was '0'->positive, '1'->negative.
#define BMA250_IS3_HIGH_FIRST_Z_FLAG   ((uint8_t)(0x04))  //high-g interrupt: '1'->triggered, or '0'->not triggered by z-axis 
#define BMA250_IS3_HIGH_FIRST_Y_FLAG   ((uint8_t)(0x02))  //high-g interrupt: '1'->triggered, or '0'->not triggered by y-axis 
#define BMA250_IS3_HIGH_FIRST_X_FLAG   ((uint8_t)(0x01))  //high-g interrupt: '1'->triggered, or '0'->not triggered by x-axis 


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x0E (BMA250_REG_FIFO_STATUS)
#define BMA250_FIFO_OVERRUN_FLAG        ((uint8_t)(0x80))  //FIFO overrun condition has '1'->occurred, or '0'->not occurred; flag can be cleared by writing to the FIFO configuration register FIFO_CONFIG_1 only.
#define BMA250_FIFO_FRAME_COUNTER_MASK  ((uint8_t)(0x7F))  //Current fill level of FIFO buffer. An empty FIFO corresponds to 0x00. The frame counter can be cleared by reading out all frames from the FIFO buffer or writing to the FIFO configuration register FIFO_CONFIG_1.


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x0F (BMA250_REG_PMU_RANGE)
#define BMA250_RANGE_MASK               ((uint8_t)(0x0F))  // Selection of accelerometer g-range: 
  #define BMA250_RANGE_2G               ((uint8_t)(0x03))  // +/- 2G
  #define BMA250_RANGE_4G               ((uint8_t)(0x05))  // +/- 4G
  #define BMA250_RANGE_8G               ((uint8_t)(0x08))  // +/- 8G
  #define BMA250_RANGE_16G              ((uint8_t)(0x0C))  // +/- 16G


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x10 (BMA250_REG_PMU_BW)
#define BMA250_BANDWIDTH_MASK           ((uint8_t)(0x1F))  // selection of the acceleration data filter bandwidth
  #define BMA250_BANDWIDTH_7P81         ((uint8_t)(0x08))  //  7.81 Hz
  #define BMA250_BANDWIDTH_15P63        ((uint8_t)(0x09))  //  15.63 Hz
  #define BMA250_BANDWIDTH_31P25        ((uint8_t)(0x0A))  //  31.25 Hz
  #define BMA250_BANDWIDTH_62P5         ((uint8_t)(0x0B))  //  62.5  Hz
  #define BMA250_BANDWIDTH_125          ((uint8_t)(0x0C))  // 125    Hz
  #define BMA250_BANDWIDTH_250          ((uint8_t)(0x0D))  // 250    Hz
  #define BMA250_BANDWIDTH_500          ((uint8_t)(0x0E))  // 500    Hz
  #define BMA250_BANDWIDTH_1000         ((uint8_t)(0x0F))  //1000    Hz


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x11 (BMA250_REG_PMU_LPW)
#define BMA250_SLEEP_MODE_MASK               ((uint8_t)(0xE0))   // selection of the acceleration data filter bandwidth
  #define BMA250_SLEEP_MODE_NORMAL           ((uint8_t)(0x00))   // normal
  #define BMA250_SLEEP_MODE_DEEP_SUSPEND     ((uint8_t)(0x20))   // deep suspend
  #define BMA250_SLEEP_MODE_LOW_POWER        ((uint8_t)(0x40))   // low power
  #define BMA250_SLEEP_MODE_SUSPEND          ((uint8_t)(0x80))   // suspend
#define BMA250_SLEEP_DURATION_MASK           ((uint8_t)(0x1E))   // selection of the acceleration data filter bandwidth
  #define BMA250_SLEEP_DURATION_0P5          ((uint8_t)(0x00))   // 0.5 ms
  #define BMA250_SLEEP_DURATION_1            ((uint8_t)(0x0C))   // 1.0 ms
  #define BMA250_SLEEP_DURATION_2            ((uint8_t)(0x0E))   // 2.0 ms
  #define BMA250_SLEEP_DURATION_4            ((uint8_t)(0x10))   // 4.0 ms
  #define BMA250_SLEEP_DURATION_6            ((uint8_t)(0x12))   // 6.0 ms
  #define BMA250_SLEEP_DURATION_10           ((uint8_t)(0x14))   // 10 ms
  #define BMA250_SLEEP_DURATION_25           ((uint8_t)(0x16))   // 25 ms
  #define BMA250_SLEEP_DURATION_50           ((uint8_t)(0x18))   // 50 ms
  #define BMA250_SLEEP_DURATION_100          ((uint8_t)(0x1A))   // 100 ms
  #define BMA250_SLEEP_DURATION_500          ((uint8_t)(0x1C))   // 500 ms
  #define BMA250_SLEEP_DURATION_1000         ((uint8_t)(0x1E))   // 1000 ms


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x12 (BMA250_REG_PMU_LOW_POWER)
#define BMA250_LOW_POWER_MODE_FLAG               ((uint8_t)(0x40))   // configuration for SUSPEND and LOW_POWER mode. 
#define BMA250_SLEEP_TIMER_MODE_FLAG             ((uint8_t)(0x20))   // selection of the acceleration data filter bandwidth


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x13 (BMA250_REG_ACCD_HBW)
#define BMA250_ACCD_HBW_DATA_HIGH_FLAG                ((uint8_t)(0x80))   // select '1' filtered data may be read from the acceleration data registers.
#define BMA250_ACCD_HBW_SHADOW_DIS_FLAG               ((uint8_t)(0x40))   // '1'->disable, or '0'->the shadowing mechanism for the acceleration data output registers. 


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x14 (BMA250_REG_BGW_SOFTRESET)
#define BMA250_SOFTRESET_TRIGGER_EVENT                ((uint8_t)(0xB6))   // triggers a reset. 


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x16 (BMA250_REG_INT_EN_0)
#define BMA250_INT_EN0_FLAT_FLAG                ((uint8_t)(0x80))   // flat
#define BMA250_INT_EN0_ORIENT_FLAG              ((uint8_t)(0x40))   // orientation
#define BMA250_INT_EN0_S_TAB_FLAG               ((uint8_t)(0x20))   // single tab
#define BMA250_INT_EN0_D_TAB_FLAG               ((uint8_t)(0x10))   // double tab
#define BMA250_INT_EN0_RESERVED_FLAD            ((uint8_t)(0x08))   // reserved
#define BMA250_INT_EN0_SLOPE_Z_FLAG             ((uint8_t)(0x04))   // slope interrupt, z-axis component.
#define BMA250_INT_EN0_SLOPE_Y_FLAG             ((uint8_t)(0x02))   // slope interrupt, y-axis component.
#define BMA250_INT_EN0_SLOPE_X_FLAG             ((uint8_t)(0x01))   // slope interrupt, x-axis component.


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x17 (BMA250_REG_INT_EN_1)
#define BMA250_INT_EN1_RESERVED_FLAG         ((uint8_t)(0x80))   // reserved
#define BMA250_INT_EN1_FWM_FLAG              ((uint8_t)(0x40))   // FIFO watermak interrupt.
#define BMA250_INT_EN1_FFUL_FLAG             ((uint8_t)(0x20))   // FIFO full interrupt.
#define BMA250_INT_EN1_DATA_FLAG             ((uint8_t)(0x10))   // data ready interrupt.
#define BMA250_INT_EN1_LOW_FLAG              ((uint8_t)(0x08))   // low-g interrupt
#define BMA250_INT_EN1_HIGH_Z_FLAG           ((uint8_t)(0x04))   // high-g interrupt, z-axis component.
#define BMA250_INT_EN1_HIGH_Y_FLAG           ((uint8_t)(0x02))   // high-g interrupt, y-axis component.
#define BMA250_INT_EN1_HIGH_X_FLAG           ((uint8_t)(0x01))   // high-g interrupt, x-axis component.


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x18 (BMA250_REG_INT_EN_2)
#define BMA250_INT_EN2_RESERVED_MASK         ((uint8_t)(0xF0))   // reserved
#define BMA250_INT_EN2_SLO_NO_MOT_SEL_FLAG   ((uint8_t)(0x08))   // select '0->'slow-motion, '1'-> no-motion interrupt function.
#define BMA250_INT_EN2_SLO_NO_MOT_EN_Z_FLAG  ((uint8_t)(0x04))   // slow/n-motion interrupt, z-axis component.
#define BMA250_INT_EN2_SLO_NO_MOT_EN_Y_FLAG  ((uint8_t)(0x10))   // slow/n-motion interrupt, y-axis component.
#define BMA250_INT_EN2_SLO_NO_MOT_EN_X_FLAG  ((uint8_t)(0x08))   // slow/n-motion interrupt, x-axis component.


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x19 (BMA250_REG_INT_MAP_0)
//<TODO>

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x1A (BMA250_REG_INT_MAP_1)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x1B (BMA250_REG_INT_MAP_2)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x1E (BMA250_REG_INT_SRC)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x20 (BMA250_REG_INT_OUT_CTRL)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x21 (BMA250_REG_INT_RST_LATCH)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x22 (BMA250_REG_INT_0)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x23 (BMA250_REG_INT_1)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x24 (BMA250_REG_INT_2)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x25 (BMA250_REG_INT_3)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x26 (BMA250_REG_INT_4)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x27 (BMA250_REG_INT_5)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x28 (BMA250_REG_INT_6)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x29 (BMA250_REG_INT_7)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x2A (BMA250_REG_INT_8)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x2B (BMA250_REG_INT_9)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x2C (BMA250_REG_INT_A)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x2D (BMA250_REG_INT_B)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x2E (BMA250_REG_INT_C)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x2F (BMA250_REG_INT_D)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x30 (BMA250_REG_FIFO_CONFIG_0)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x32 (BMA250_REG_PMU_SELF_TEST)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x33 (BMA250_REG_TRIM_NVM_CTRL)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x34 (BMA250_REG_BGW_SPI3_WDT)
#define BMA250_I2C_WDT_EN_FLAG  ((uint8_t)(0x04))   // if I2C interface mode is selected 1・   disables the watchdog at the SDI pin (= SDA for I2C) 
#define BMA250_I2C_WDT_SEL_FLAG ((uint8_t)(0x02))   // select an I2C watchdog timer period  1  50 ms
#define BMA250_SPI3_MODE_FLAG ((uint8_t)(0x02))     // select ・0・  4-wire SPI, or ・1・  3-wire SPI mode 


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x36 (BMA250_REG_OFC_CTRL)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x37 (BMA250_REG_OFC_SETTING)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x38 (BMA250_REG_OFC_OFFSET_X)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x39 (BMA250_REG_OFC_OFFSET_Y)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x3A (BMA250_REG_OFC_OFFSET_Z)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x3B (BMA250_REG_TRIM_GP0)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x3C (BMA250_REG_TRIM_GP1)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x3E (BMA250_REG_FIFO_CONFIG_1)
//<TODO>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Used with register 0x3F (BMA250_REG_FIFO_DATA)
//<TODO>


#endif //__DRITEK_BMA250_SPEC_H__


