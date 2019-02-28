# Host Test

## Table of Contents

* [Introduction](#Introduction)
* [Hardware Prerequisites](#Hardware Prerequisites)
* [Software Prerequisites](#Software Prerequisites)
* [Usage](#Usage)

## <a name="Introduction"></a>Introduction

The host\_test project implements a pure Bluetooth low energy network
processor for use with an external microcontroller or a PC software
application such as BTool. Host Test supports all GAP roles: Central, Peripheral,
Broadcaster and Observer in addition to multi-role combinations of these roles.
A network processor incorporates the TI BLE Host and Controller components of the
protocol stack on the wireless MCU with the Bluetooth application residing on
the external microcontroller. Communication with the network processor occurs
through a serial interface (UART or SPI) using the Host Controller Interface
(HCI) protocol. The external application uses TI Vendor Specific HCI commands
and a limited subset of Bluetooth LE HCI commands/events to implement a
Bluetooth application. By placing the Host and Controller on the wireless MCU,
the memory requirements on the application processor are reduced since only the
application is required.

Note: It is not possible to use an external Bluetooth Host stack, such as BlueZ,
with Host Test since the network processor implements the Host component.
External applications must use the TI Vendor Specific HCI commands for
implementing GAP, GATT, Pairing/Bonding, etc., functionality.

This document discusses the procedure for using the Host Test
application when the FlashROM_StackLibrary configuration is used.
The FlashROM_StackLibrary_RCOSC configuration is discussed in the
[**RCOSC section of the BLE5-Stack User's Guide**](http://software-dl.ti.com/lprf/ble5stack-docs-latest/docs/ble5stack/ble_user_guide/html/ble-stack/custom-hardware.html#using-32-khz-crystal-less-mode).
The FlashROM_StackLibrary-CacheAsRAM configuration is discussed in the [**Using
the Cache as RAM section of the BLE5-Stack User's Guide**](http://software-dl.ti.com/lprf/ble5stack-docs-latest/docs/ble5stack/ble_user_guide/html/cc2640/memory_management.html#using-the-cache-as-ram).

The Simple NP (Simple Network Processor) sample application is another project
that acts as a network processor. SimpleNP allows the application processor to
be responsible for maintaining fewer aspects of the BLE stack. Note, though,
that SimpleNP can only function as a peripheral or broadcaster device.

## <a name="Hardware Prerequisites"></a>Hardware Prerequisites

The default Host Test configuration uses the
[LAUNCHXL-CC2640R2](http://www.ti.com/tool/launchxl-cc2640r2). This hardware
configuration is shown in the below image:

<img src="resource/hardware_setup.jpg" width="300" height="480" />

For custom hardware, see the [**Running the SDK on Custom Boards section of the
BLE5-Stack User's Guide**](http://software-dl.ti.com/lprf/ble5stack-docs-latest/docs/ble5stack/ble_user_guide/html/ble-stack/index.html#running-the-sdk-on-custom-boards).

## <a name="Software Prerequisites"></a>Software Prerequisites

For information on what versions of Code Composer Studio and IAR Embedded
Workbench to use, see the Release Notes located in the
docs/ble5stack folder. For
information on how to import this project into your IDE workspace and
build/run, please refer to [**The CC2640R2F Platform section in the BLE5-Stack User's Guide**](http://software-dl.ti.com/lprf/ble5stack-docs-latest/docs/ble5stack/ble_user_guide/html/cc2640/platform.html).

## <a name="Usage"></a>Usage

This application uses the UART peripheral without POWER\_SAVING by
default to communicate with the host processor. The project also
supports UART and SPI interfaces with power management enabled.

If POWER\_SAVING is not defined and UART is being used, the MRDY and SRDY pins
are defined by default as in the table below. MRDY
and SRDY are configurable GPIO pins. They can be changed and set by the
application. They are set in np_config.h. Note that MRDY and SRDY are required
to use SPI to allow for full duplex communication, regardless of whether power
management is enabled.

The following table includes the default pins for serial communication in this
project:

PIN          | CC2640R2F Pins
-------------|----------------
MRDY         | IOID_13
SRDY         | IOID_14
UART_RX      | IOID_2
UART_TX      | IOID_3
SPI_MISO     | IOID_8
SPI_MOSI     | IOID_9
SPI_CLK      | IOID_10
SPI_FLASH_CS | IOID_20

See the [**NPI wiki**](http://processors.wiki.ti.com/index.php/NPI) for more
information on power management and setting the necessary pins.

Host Test can be used with an external microcontroller or a PC application such
as [**BTool**](http://software-dl.ti.com/lprf/ble5stack-docs-latest/docs/ble5stack/btool_user_guide/BTool_Users_Guide/index.html) (provided
with the SimpleLink CC2640R2 SDK) or
[**HCI Tester**](http://processors.wiki.ti.com/index.php/LPRF_BLE_HCITester).

Information on HCI and details and descriptions of available HCI commands can be
found in the [**TI BLE Vendor Specific HCI Guide**](http://software-dl.ti.com/lprf/ble5stack-docs-latest/docs/ble5stack/TI_BLE_Vendor_Specific_HCI_Guide.pdf).
