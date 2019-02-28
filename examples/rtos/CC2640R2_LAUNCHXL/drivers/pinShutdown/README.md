## Example Summary

This application moves into and out of shutdown. The state is controlled
by buttons. `Board_PIN_BUTTON1` will bring the device into shutdown, while
`Board_PIN_BUTTON0` will wake-up the device. A special LED toggling sequence
is used when waking from shutdown to demonstrate the get-reset-source
functionality.

## Peripherals Exercised

* `Board_PIN_LED0` -  Lit when the device is active, not lit when in shutdown.
* `Board_PIN_LED1` -  Will blink 2 times when coming out of shutdown.

## Resources & Jumper Settings

> If you're using an IDE (such as CCS or IAR), please refer to Board.html in
your project directory for resources used and board-specific jumper settings.
Otherwise, you can find Board.html in the directory
&lt;SDK_INSTALL_DIR&gt;/source/ti/boards/&lt;BOARD&gt;.

> In order to get lowest shutdown current, the JTAG pins TMS
and TCK should be disconnected by removing the jumpers on P408 of the
SmartRF06 EB. TMS and TCK have internal pull-up and are driven low when
inactive from the emulator, hence after programming the device these
jumpers should be removed to avoid the extra leakage current.

## Example Usage

* Run the example. Use the `KEY_DOWN` and `KEY_UP` buttons to shutdown
and wake-up the device. Also, use the RESET button to compare the
different start-up sequences used.

> It is not possible to do a proper shutdown sequence with the
debugger connected. For correct behaviour, this example must be run with the
debugger disconnected by resetting or performing a power-cycle of the device.

## Application Design Details

* The example consists of one task which is waiting for a semaphore
that is posted in the `Board_PIN_BUTTON1` interrupt handle. Since no other
resources are requested by the application, it will enter standby
in active mode when waiting for a posting of the semaphore. The
non-default initialization table used is equal to the default one.
It is included to show how a non-default initialization when waking
from shutdown can be used to avoid glitches on IOs.

> For IAR users using any SensorTag(STK) Board, the XDS110 debugger must be
selected with the 4-wire JTAG connection within your projects' debugger
configuration.

TI-RTOS:

* When building in Code Composer Studio, the kernel configuration project will
be imported along with the example. The kernel configuration project is
referenced by the example, so it will be built first. The "release" kernel
configuration is the default project used. It has many debug features disabled.
These feature include assert checking, logging and runtime stack checks. For a
detailed difference between the "release" and "debug" kernel configurations and
how to switch between them, please refer to the SimpleLink MCU SDK User's
Guide. The "release" and "debug" kernel configuration projects can be found
under &lt;SDK_INSTALL_DIR&gt;/kernel/tirtos/builds/&lt;BOARD&gt;/(release|debug)/(ccs|gcc).

FreeRTOS:

* Please view the `FreeRTOSConfig.h` header file for example configuration
information.
