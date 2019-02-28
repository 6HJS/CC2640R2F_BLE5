## Example Summary

=======
Example to read and write data into non-volatile storage using the NVS SPI
driver.
The external SPI flash is accessed by the NVS SPI driver.

##Peripherals Exercised

* `Board_UART0` - Used for console output.
* `Board_SPI0` - Used internally by the NVS SPI driver.

## Resources & Jumper Settings

> If you're using an IDE (such as CCS or IAR), please refer to Board.html in
 your project directory for resources used and board-specific jumper settings.
 Otherwise, you can find Board.html in the directory
 &lt;SDK_INSTALL_DIR&gt;/source/ti/boards/&lt;BOARD&gt;.

## Example Usage

* Open a serial session (e.g. [`PuTTY`](http://www.putty.org/ "PuTTY's
 Homepage"), etc.) to the appropriate COM port.
 *The COM port can be determined via Device Manager in Windows or via
 `ls /dev/tty*` in Linux.

The connection will have the following settings:
```
    Baud-rate:     115200
    Data bits:          8
    Stop bits:          1
    Parity:          None
    Flow Control:    None
```

* Run the example.

* The example checks if the signature, "SimpleLink SDK Non-Volatile Storage
 (NVS) Example" is saved in flash.

 * If the signature is present in flash, the flash region is erased. Otherwise,
 the application will write the signature to flash.

 * When prompted, reset the device.

The following is example output.
```
    Sector Size: 0x1000
    Region Size: 0x4000

    Writing signature to flash...
    Reset the device.
    ==================================================
```
After device reset:
```
    Sector Size: 0x1000
    Region Size: 0x4000

    SimpleLink SDK Non-Volatile Storage (NVS) Example.
    Erasing flash...
    Reset the device.
    ==================================================
```

## Application Design Details

* The application utilizes a single thread to demonstrate using the
 non-volatile storage.

* A block of memory equal to the size of `signature` is read from flash and
copied into `buffer`. The contents are then compared to `signature`. If equal,
the application displays the signature as stored in `buffer` and erases the
flash. Otherwise, `signature` is written to flash.

* At the end of execution, the application prompts the user to reset the
device. Upon a reset, the processor will restart. The application will
automatically start over.

* `NVS_getAttrs()` will populate a `NVS_Attrs` struct with properties specific
to a `NVS_Handle` such as region base address, size and sector size.
```C
NVS_getAttrs(nvsHandle, &regionAttrs);
```

* This application demonstrates how to use the `NVS_read()`, `NVS_write()` and
`NVS_erase()` APIs with appropriate offsets.

TI-RTOS:

* When building in Code Composer Studio, the configuration project will be
 imported along with the example. The configuration project is referenced by
 the example, so it will be built first.  These projects can be found under
&lt;SDK_INSTALL_DIR&gt;/kernel/tirtos/builds/&lt;BOARD&gt;/(release|debug)/
(ccs|gcc).

FreeRTOS:

* Please view the `FreeRTOSConfig.h` header file for example configuration
information.
