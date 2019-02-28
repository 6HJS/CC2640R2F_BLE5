SIMPLELINK_CC2640R2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/imports.mak

KERNEL_BUILD := $(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/kernel/tirtos/builds/CC2640R2_LAUNCHXL/release

CC = "$(CCS_ARMCOMPILER)/bin/armcl"
LNK = "$(CCS_ARMCOMPILER)/bin/armcl"

OBJECTS = RFQueue.obj smartrf_settings.obj smartrf_settings_ble.obj smartrf_settings_predefined.obj config.obj menu.obj rfPacketErrorRate.obj rx.obj tx.obj CC2640R2_LAUNCHXL.obj ccfg.obj

CONFIGPKG = $(KERNEL_BUILD)/ccs

NAME = rfPacketErrorRate

CFLAGS = -I../.. -DBOARD_DISPLAY_USE_LCD=1 -DDeviceFamily_CC26X0R2 "-I$(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/source" "-I$(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/kernel/tirtos/packages/ti/sysbios/posix" --silicon_version=7M3 --code_state=16 --little_endian --display_error_number --diag_warning=255 --diag_wrap=off --gen_func_subsections=on --float_support=vfplib --symdebug:dwarf

LFLAGS = "-l$(KERNEL_BUILD)/ccs/linker.cmd" "-l$(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/source/ti/display/lib/display.aem3" "-l$(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/source/ti/mw/lcd/lib/lcd.aem3" "-l$(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/source/ti/grlib/lib/grlib.aem3" "-l$(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/source/ti/drivers/rf/lib/rf_multiMode_cc26x0r2.aem3" "-l$(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/source/ti/drivers/lib/drivers_cc26x0r2.aem3" "-l$(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/source/ti/drivers/pdm/lib/pdm_cc26x0r2.aem3" "-l$(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/kernel/tirtos/packages/ti/dpl/lib/dpl_cc26x0r2.aem3" "-l$(SIMPLELINK_CC2640R2_SDK_INSTALL_DIR)/source/ti/devices/cc26x0r2/driverlib/bin/ccs/driverlib.lib" ../../tirtos/ccs/CC2640R2_LAUNCHXL_TIRTOS.cmd "-m$(NAME).map" --warn_sections --display_error_number --diag_wrap=off --rom_model "-i$(CCS_ARMCOMPILER)/lib" -llibc.a

all: $(NAME).out

$(CONFIGPKG)/linker.cmd $(CONFIGPKG)/compiler.opt:
	@ $(ECHOBLANKLINE)
	@ echo $(abspath $(CONFIGPKG)) is not built.
	@ echo You can build it by issuing $(MAKE) in $(abspath $(CONFIGPKG)).
	@ $(ECHOBLANKLINE)

RFQueue.obj: ../../RFQueue.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

smartrf_settings.obj: ../../smartrf_settings/smartrf_settings.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

smartrf_settings_ble.obj: ../../smartrf_settings/smartrf_settings_ble.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

smartrf_settings_predefined.obj: ../../smartrf_settings/smartrf_settings_predefined.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

config.obj: ../../config.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

menu.obj: ../../menu.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

rfPacketErrorRate.obj: ../../rfPacketErrorRate.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

rx.obj: ../../rx.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

tx.obj: ../../tx.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

CC2640R2_LAUNCHXL.obj: ../../CC2640R2_LAUNCHXL.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

ccfg.obj: ../../ccfg.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

$(NAME).out: $(OBJECTS) $(CONFIGPKG)/linker.cmd
	@ echo linking...
	@ $(LNK) $(CFLAGS) -z $(OBJECTS) $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
