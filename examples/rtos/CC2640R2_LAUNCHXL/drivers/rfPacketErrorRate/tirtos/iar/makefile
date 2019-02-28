SIMPLELINK_CC2640R2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/imports.mak

KERNEL_BUILD := $(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/kernel/tirtos/builds/CC2640R2_LAUNCHXL/release

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
LNK = "$(IAR_ARMCOMPILER)/bin/ilinkarm"

OBJECTS = RFQueue.obj smartrf_settings.obj smartrf_settings_ble.obj smartrf_settings_predefined.obj config.obj menu.obj rfPacketErrorRate.obj rx.obj tx.obj CC2640R2_LAUNCHXL.obj ccfg.obj

CONFIGPKG = $(KERNEL_BUILD)/iar

NAME = rfPacketErrorRate

CFLAGS = -I../.. -DBOARD_DISPLAY_USE_LCD=1 -DDeviceFamily_CC26X0R2 "-I$(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/source" "-I$(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/kernel/tirtos/packages/ti/sysbios/posix" --debug --silent -e --aeabi --thumb --diag_suppress=Pa050

LFLAGS = "$(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/source/ti/display/lib/display.arm3" "$(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/source/ti/mw/lcd/lib/lcd.arm3" "$(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/source/ti/grlib/lib/grlib.arm3" "$(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/source/ti/drivers/rf/lib/rf_multiMode_cc26x0r2.arm3" "$(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/source/ti/drivers/lib/drivers_cc26x0r2.arm3" "$(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/source/ti/drivers/pdm/lib/pdm_cc26x0r2.arm3" "$(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/kernel/tirtos/packages/ti/dpl/lib/dpl_cc26x0r2.arm3" -f "$(KERNEL_BUILD)/iar/linker.cmd" "$(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/source/ti/devices/cc26x0r2/driverlib/bin/iar/driverlib.lib" --config ../../tirtos/iar/CC2640R2_LAUNCHXL_TIRTOS.icf --map "$(NAME).map" --silent --cpu=Cortex-M3 --redirect _Printf=_PrintfSmall --redirect _Scanf=_ScanfSmall --semihosting=iar_breakpoint

all: $(NAME).out

$(CONFIGPKG)/linker.cmd $(CONFIGPKG)/compiler.opt:
	@ $(ECHOBLANKLINE)
	@ echo $(abspath $(CONFIGPKG)) is not built.
	@ echo You can build it by issuing $(MAKE) in $(abspath $(CONFIGPKG)).
	@ $(ECHOBLANKLINE)

RFQueue.obj: ../../RFQueue.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

smartrf_settings.obj: ../../smartrf_settings/smartrf_settings.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

smartrf_settings_ble.obj: ../../smartrf_settings/smartrf_settings_ble.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

smartrf_settings_predefined.obj: ../../smartrf_settings/smartrf_settings_predefined.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

config.obj: ../../config.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

menu.obj: ../../menu.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

rfPacketErrorRate.obj: ../../rfPacketErrorRate.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

rx.obj: ../../rx.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

tx.obj: ../../tx.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

CC2640R2_LAUNCHXL.obj: ../../CC2640R2_LAUNCHXL.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

ccfg.obj: ../../ccfg.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

$(NAME).out: $(OBJECTS) $(CONFIGPKG)/linker.cmd
	@ echo linking...
	@ $(LNK)  $(OBJECTS) $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
