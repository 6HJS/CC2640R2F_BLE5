# Simple Peripheral OAD Off-chip

## Table of Contents

* [Introduction](#Introduction)
* [Hardware Prerequisites](#Hardware Prerequisites)
* [Software Prerequisites](#Software Prerequisites)
* [Service/Profile Table](#Service/Profile Table)
* [Usage](#Usage)

## <a name="Introduction"></a>Introduction

The simple\_peripheral\_oad\_offchip project demonstrates off-chip OAD
functionality with the simple\_peripheral application. The application behavior
is the same as the stock [simple\_peripheral project](../simple_peripheral/README.html),
with the exception that OAD is added.

**Note: The ``bim_oad_offchip`` project must also be flashed to the device when
using off-chip OAD or the device will not function correctly**

This project uses stack and app configurations that are summarized in the table below:

App Build Configuration     | Compatible Stack Configuration | Description
----------------------------|--------------------------------|-------------------------------------------------------------------
FlashROM_OAD_Offchip        | FlashROM                       | OAD off chip application FlashROM configuration

FlashROM configurations use the split image configuration, which is explained
[**here**](http://software-dl.ti.com/lprf/ble5stack-docs-latest/docs/ble5stack/ble_user_guide/html/cc2640/architecture.html#split-image-configuration).


This document seeks to extend the [simple\_peripheral project](../simple_peripheral/README.html)
documentation that is already in place, and explain how simple\_peripheral can
be used to demonstrate off-chip OAD. For an advanced discussion of OAD, and
an in-depth explanation of this project, please see:

[** Off-chip OAD Software Developer's Guide Section**](http://software-dl.ti.com/lprf/ble5stack-docs-latest/docs/ble5stack/ble_user_guide/html/oad/oad_offchip.html)

## <a name="Hardware Prerequisites"></a>Hardware Prerequisites

The default Simple Peripheral OAD-offchip configuration uses the
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

## <a name="Service/Profile Table"></a>Service/Profile Table

This project contains the simple service, which has 5 characteristics.

Note that BLE Scanner may report 128 bit UUIDs, but the project is generating
16 bit UUIDs.


Characterisitic | UUID | Format         | Properties | Profile Source
----------------|------|----------------|------------|----------------------
1               | FFF1 | 1 Byte Integer | Read/Write | `simple_gatt_profile.c`
2               | FFF2 | 1 Byte Integer | Read       | `simple_gatt_profile.c`
3               | FFF3 | 1 Byte Integer | Write      | `simple_gatt_profile.c`
4               | FFF4 | 1 Byte Integer | Notify     | `simple_gatt_profile.c`
5               | FFF5 | 5 Byte Integer | Read       | `simple_gatt_profile.c`

Additionally, this project contains the OAD profile, which has 5 characteristics.

See [** OAD Service Description in BLE5-Stack User's Guide**](http://software-dl.ti.com/lprf/ble5stack-docs-latest/docs/ble5stack/ble_user_guide/html/oad/oad_offchip.html#oad-service-0xffc0)
for more information on the OAD service


## <a name="Usage"></a>Usage

This application uses the UART peripheral to provide an
interface for the application. The UART is only used for display
messages.

This document will use PuTTY to serve as the display for the output of the
CC2640R2 LaunchPad, and it will use BLE Scanner as the phone app that will act
as the central device that will connect to the Simple Peripheral device. Note
that any other serial terminal and smart phone BLE application can be used. The
following default parameters are used for the UART peripheral for display:

  UART Param     |Default Values
  -------------- |----------------
  Baud Rate      |115200
  Data length    |8 bits
  Parity         |None
  Stop bits      |1 bit
  Flow Control   |None

For further guidance refer to [** Off-chip OAD User's Guide Section**](http://software-dl.ti.com/lprf/ble5stack-docs-latest/docs/ble5stack/ble_user_guide/html/oad/oad_offchip.html)

