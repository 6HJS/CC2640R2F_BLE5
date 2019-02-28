## Example Summary

This application exemplifies usage of the UART Monitor. The UARTMon module
enables host communication with a target device using the target's UART. The
target device can respond to requests to read and write memory at specified
addresses.

## Peripherals Exercised

* `Board_GPIO_LED0` - Indicates that the board was initialized within
`mainThread()` also toggled by `Board_GPIO_BUTTON0`
* `Board_GPIO_LED1` - Toggled by `Board_GPIO_BUTTON1`
* `Board_GPIO_BUTTON0` - Toggles `Board_GPIO_LED0`
* `Board_GPIO_BUTTON1` - Toggles `Board_GPIO_LED1`
* `Board_UART0` - UART instance used by the UART Monitor

## Resources & Jumper Settings

> If you're using an IDE (such as CCS or IAR), please refer to Board.html in
your project directory for resources used and board-specific jumper settings.
Otherwise, you can find Board.html in the directory
&lt;SDK_INSTALL_DIR&gt;/source/ti/boards/&lt;BOARD&gt;.


## Example Usage

* Run the example. A global variable, `count`, is modified by pressing either
`Board_GPIO_BUTTON0` or `Board_GPIO_BUTTON1`. Note that `count` is declared as
volatile.

* `Board_GPIO_LED0` is toggled by pushing `Board_GPIO_BUTTON0`. `count` is
decremented.
* `Board_GPIO_LED1` is toggled by pushing `Board_GPIO_BUTTON1`. `count` is
incremented.

## Application Design Details

* The `UARTMon_init()` function will create a dedicated thread that serves as
the UART Monitor.

* The global variable, `count`, may be accessed through one of the following
methods:

  1. [GUI Composer Application](https://dev.ti.com/gc/designer/help/Tutorials/GettingStarted/)
using [TI Cloud Tools](https://dev.ti.com/)
    * [Follow
this tutorial](https://dev.ti.com/gc/designer/help/Tutorials/monitor/index.html).
Note that the same steps apply to different boards.
    * In __step 1__ under __Target Program/Symbols__, `UPLOAD` the
uartmonitor.out found in the __Debug__ directory of your project workspace.
    * In __step 4__, a multitude of widgets may be selected. __Multipurpose
Meter__ better suites this example.
        * `count` can be bound to the Meter __value__ by typing "count".
  2. [*Expressions* module in the Debug View of Code Composer Studio. See
 here for more details.](http://processors.wiki.ti.com/index.php/Using_UARTMon)

* The `gpioButtonFxn0` and `gpioButtonFxn1` functions configured in the
*Board.c* file. These functions are called in the context of the GPIO interrupt.

* Not all boards have more than one button.

* There is no button de-bounce logic in the example.

> See also:
[GUI Composer and UART Support](http://processors.wiki.ti.com/index.php/ProgramModelUart_GuiComposer).

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
