#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#
#  target compatibility key = iar.targets.arm.M3{1,0,8.11,3
#
ifeq (,$(MK_NOGENDEPS))
-include package/cfg/app_ble_prm3.orm3.dep
package/cfg/app_ble_prm3.orm3.dep: ;
endif

package/cfg/app_ble_prm3.orm3: | .interfaces
package/cfg/app_ble_prm3.orm3: package/cfg/app_ble_prm3.c package/cfg/app_ble_prm3.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clrm3 $< ...
	LC_ALL=C $(iar.targets.arm.M3.rootDir)/bin/iccarm    -D BOARD_DISPLAY_USE_LCD=1   -D BOARD_DISPLAY_USE_UART=1   -D BOARD_DISPLAY_USE_UART_ANSI=1   -D _CC2640R2MOD_RGZ   -D CC2640R2MOD_RSM   -D CC26XX   -D CC26XX_R2   -D DeviceFamily_CC26X0R2   -D xDisplay_DISABLE_ALL   -D ICALL_EVENTS   -D ICALL_JT   -D ICALL_LITE   -D ICALL_MAX_NUM_ENTITIES=6   -D ICALL_MAX_NUM_TASKS=3   -D ICALL_STACK0_ADDR   -D POWER_SAVING   -D RF_SINGLEMODE   -D STACK_LIBRARY   -D USE_ICALL   -D xdc_runtime_Assert_DISABLE_ALL   -D xdc_runtime_Log_DISABLE_ALL   -D IOTXX_DISPLAY_TFT   -D _BOARD5X5   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/controller/cc26xx_r2/inc/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/inc/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/common/cc26xx/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../src/app/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/icall/inc/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/inc/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/profiles/roles/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/profiles/roles/cc26xx/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/hal/src/inc/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/hal/src/target/_common/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/hal/src/target/_common/cc26xx/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/hal/src/target/cc2650/rom/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/heapmgr/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/icall/src/inc/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/osal/src/inc/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/services/src/saddr/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/services/src/sdata/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/devices/cc26x0r2/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../target/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../target/cc2640r2iot/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../target/LAUNCHIOT_CC2640R2MOD_RGZ/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../common/display/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../target/LAUNCHIOT_CC2640R2MOD_RSM/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../src/driver/gpio/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../src/driver/i2c/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../src/driver/VL53L1X/CLib/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/drivers/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/src/driver/VL53L1X/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../../xdctools_3_50_02_20_core/packages/xdc/runtime/   --silent --aeabi --cpu=Cortex-M3 --diag_suppress=Pa050,Go005 --endian=little -e --thumb   -Dxdc_cfg__xheader__='"configPkg/package/cfg/app_ble_prm3.h"'  -Dxdc_target_name__=M3 -Dxdc_target_types__=iar/targets/arm/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_8_11_3 -Ohs --dlib_config $(iar.targets.arm.M3.rootDir)/inc/c/DLib_Config_Normal.h  $(XDCINCS)  -o $@  $<
	
	-@$(FIXDEP) $@.dep $@.dep
	

package/cfg/app_ble_prm3.srm3: | .interfaces
package/cfg/app_ble_prm3.srm3: package/cfg/app_ble_prm3.c package/cfg/app_ble_prm3.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clrm3 $< ...
	LC_ALL=C $(iar.targets.arm.M3.rootDir)/bin/iccarm    -D BOARD_DISPLAY_USE_LCD=1   -D BOARD_DISPLAY_USE_UART=1   -D BOARD_DISPLAY_USE_UART_ANSI=1   -D _CC2640R2MOD_RGZ   -D CC2640R2MOD_RSM   -D CC26XX   -D CC26XX_R2   -D DeviceFamily_CC26X0R2   -D xDisplay_DISABLE_ALL   -D ICALL_EVENTS   -D ICALL_JT   -D ICALL_LITE   -D ICALL_MAX_NUM_ENTITIES=6   -D ICALL_MAX_NUM_TASKS=3   -D ICALL_STACK0_ADDR   -D POWER_SAVING   -D RF_SINGLEMODE   -D STACK_LIBRARY   -D USE_ICALL   -D xdc_runtime_Assert_DISABLE_ALL   -D xdc_runtime_Log_DISABLE_ALL   -D IOTXX_DISPLAY_TFT   -D _BOARD5X5   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/controller/cc26xx_r2/inc/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/inc/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/common/cc26xx/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../src/app/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/icall/inc/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/inc/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/profiles/roles/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/profiles/roles/cc26xx/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/hal/src/inc/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/hal/src/target/_common/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/hal/src/target/_common/cc26xx/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/hal/src/target/cc2650/rom/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/heapmgr/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/icall/src/inc/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/osal/src/inc/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/services/src/saddr/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/ble5stack/services/src/sdata/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/devices/cc26x0r2/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../target/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../target/cc2640r2iot/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../target/LAUNCHIOT_CC2640R2MOD_RGZ/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../common/display/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../target/LAUNCHIOT_CC2640R2MOD_RSM/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../src/driver/gpio/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../src/driver/i2c/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../src/driver/VL53L1X/CLib/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../source/ti/drivers/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/src/driver/VL53L1X/   -I C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/examples/rtos/CC2640R2_LAUNCHIOT/ble5stack/CPS-GATE-Counter/tirtos/iar/app/../../../../../../../../../xdctools_3_50_02_20_core/packages/xdc/runtime/   --silent --aeabi --cpu=Cortex-M3 --diag_suppress=Pa050,Go005 --endian=little -e --thumb   -Dxdc_cfg__xheader__='"configPkg/package/cfg/app_ble_prm3.h"'  -Dxdc_target_name__=M3 -Dxdc_target_types__=iar/targets/arm/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_8_11_3 -Ohs --dlib_config $(iar.targets.arm.M3.rootDir)/inc/c/DLib_Config_Normal.h  $(XDCINCS)  -o $@  $<
	
	-@$(FIXDEP) $@.dep $@.dep
	

clean,rm3 ::
	-$(RM) package/cfg/app_ble_prm3.orm3
	-$(RM) package/cfg/app_ble_prm3.srm3

app_ble.prm3: package/cfg/app_ble_prm3.orm3 package/cfg/app_ble_prm3.mak

clean::
	-$(RM) package/cfg/app_ble_prm3.mak
