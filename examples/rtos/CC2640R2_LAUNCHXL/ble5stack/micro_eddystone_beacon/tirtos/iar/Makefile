PROJECT=$(BLE_REPO)/examples/rtos/CC2640R2_LAUNCHXL/ble5stack/micro_eddystone_beacon/tirtos/iar
BLE_REPO=../../../../../../..
IAR_PATH=C:/Program\ Files\ \(x86\)/IAR\ Systems/EWARM-$(IAR_VERSION)/common/bin
IAR_VERSION=8.11.1

all: micro_eddystone_beacon

micro_eddystone_beacon:
	$(IAR_PATH)/iarbuild.exe $(PROJECT)/app/cc2640r2lp_app.ewp \
	-build \
	FlashOnly \
	-log info \
	-parallel 1 \
	-varfile ble5_micro_eddystone_beacon.custom_argvars

clean:
	$(IAR_PATH)/iarbuild.exe $(PROJECT)/app/cc2640r2lp_app.ewp \
	-clean \
	FlashOnly \
	-log info \
	-parallel 1 \
	-varfile ble5_micro_eddystone_beacon.custom_argvars
