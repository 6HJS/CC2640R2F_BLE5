Packet Error Rate Example Application
=====================================

The Packet Error Rate (PER) example showcases different RF transfer modes of the
CC13xx. It combines tasks, events and several peripherals to a platform-
independent application and runs on all CC13xx launchpads as well as the
CC1310EM/SmartRF06.

Test cases are provided for the following RF modes:

- `Custom`: Custom settings to be exported from SmartRF Studio to the
smartrf_settings.c file
- `2-GFSK`: Generic frequency shift keying with binary symbols, 50 kBit/s
- `LR Mode`: Legacy Long Range (625 bps)
- `OOK`: On-off keying
- `HS Mode`: High-speed mode with up to 4 MBit/s
- `SL LR`: SimpleLink Long Rang (5 kbps)
- `BLE`: Bluetooth Low Energy (CC1350 only)

Note that when running the CC1310-CC1190, HSM and OOK are not supported by the
PER test.



Peripherals Exercised
---------------------
This example uses the following CC13xx peripherals:

- RF core for radio communication
- 2 GPIOs for buttons
- 3 GPIOs for controlling the CC1190 when running CC1310-CC1190 Launchpad
- SPI to drive the LCD displays on the boards
- UART0 (115200 Baud) as an alternative display on VT100-compatible
  terminal emulators



  | Peripheral        | Identifier        | CC13xxEM/SmartRF06 | CC13xx Launchpad     | CC1310-CC1390 Launchpad |
  | ----------------- | ----------------- | ------------------ | -------------------- |-------------------------|
  | Select button     | Board_PIN_BUTTON0 | UP                 | BTN-1                | BTN-1                   |
  | Navigate button   | Board_PIN_BUTTON1 | DOWN               | BTN-2                | BTN-2                   |
  | Display           | --                | LCD Dogm1286       | Display Booster Pack | Display Booster Pack    |
  | CC1190 Control    | Board_HGM         | --                 | --                   | DIO28                   |
  | signals           | Board_LNA_EN      | --                 | --                   | DIO29                   |
  |                   | Board_PA_EN       | --                 | --                   | DIO30                   |


Resources & Jumper Settings
---------------------------

> If you're using an IDE (such as CCS or IAR), please refer to Board.html in
your project directory for resources used and board-specific jumper settings.
Otherwise, you can find Board.html in the directory
&lt;SDK_INSTALL_DIR&gt;/source/ti/boards/&lt;BOARD&gt;.


Example Usage
-------------

This example requires two boards, each running the PER Test application.
However, the packet format is identical to the default one in SmartRF Studio, so
that any compatible hardware can be used as well.

1. Connect a display to the board or alternatively, use the UART and hook it to
   a VT100-compatible terminal emulator at 115200 Baud. Use PuTTY or
   TeraTerm on Microsoft Windows. On Linux, use the terminal emulator that is
   shipped with your distribution. After a splash screen, you will see the
   main menu (the range extender option (CC1190) is only shown for CC1310
   LAUNCHLX):


        Main Menu
        >Test: 2-GFSK
         Freq: 868.0
         Pkts: 10
         CC1190: Disable
         Mode: Rx
         Start...


2. Navigate through the rows with BTN-2/DOWN, modify a value or start the
 selected test with BTN-1/UP. Note that it is the user's responsibility to
enable CC1190 from the menu when running CC1310-CC1190 LAUNCHXL.

3. Use a second board with the PER test application as test companion for
   transmissions. Once started, the current progress is shown with these menus
   (TX mode on the left side, RX on the right):

        Sending...      |  Receiving...
        2-GFSK  868.0   |  HS Mode 868.0
                        |  Pkts ok   : 39
        Pkts sent: 47   |  RSSI [dBm]: -74
                        |  PER  [%]  : 17.01
                        |
                        |  Push a button
                        |  to abort.

   The receiver prints the amount of successfully received packets (Pkts ok),
   the Signal strength of the current packet (RSSI) and the observed packet
   error rate (PER) in percent. Please note that the PER is n/a when sending
   more packets than configured in the receiver.

5. You can always abort a running test case by pushing any button and go back
   to the main menu.


Application Design Details
--------------------------

The PER test application contains of one main task and an event handler to
synchronize the task with buttons. After setting up all resources, the menu task
is started and runs in an endless loop. It shows the menu and invokes test cases
in either `rx.c` or `tx.c`.


Changelog
---------

### Version 1.1

- released with TI-RTOS 2.17
- add version number on the splash screen
- hide cursor on VT100 terminals


### Version 1.0

 - shipped along with the CC1310 launchpads as default application

### Version 2.0

 - released with TI-RTOS 2.21
 - added support for CC2650_LAUNCHXL and CC2650DK_7ID
 - added BLE mode for CC1350 and CC2650 platforms. Note that the default
frequency is 2.402MHz, which is an advertisement channel and may result in
packet errors due to BLE devices advertising.
 - added custom mode which takes settings exported from SmartRf Studio into
rfPacketErrorRate_<platform>\smartrf_settings

### Version 2.1

 - released with simplelink_cc13x0_sdk_1_10
 - added support for SimpleLink Long Range (5 kbps)
 - added support for CC1310-CC1190 LAUNCHXL. When running on this HW, it is
the user's responsibility to enable CC1190 from the menu. Note that HSM and
OOK is not supported in the PER test when running on this HW
