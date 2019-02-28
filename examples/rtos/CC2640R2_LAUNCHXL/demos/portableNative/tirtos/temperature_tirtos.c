/*
 * Copyright (c) 2016-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== temperature_tirtos.c ========
 */

/* RTOS header files */
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/hal/Hwi.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>

/* Example/Board Header files */
#include "Board.h"

/*
 *  ======== HIGH_TEMP ========
 *  Send alert when this temperature (in Celsius) is exceeded
 */
#define HIGH_TEMP 30

/*
 *  ======== TMP Registers ========
 *  The DIE register is valid for both TMP006 and TMP007
 *  The OBJ register is only valid for TMP007
 */
#define TMP_DIE_TEMP     0x0001  /* Die Temp Result Register */
#define TMP_OBJ_TEMP     0x0003  /* Object Temp Result Register */

/*
 *  The CC3220 Launchpads contain an onboard TMP006 where you can
 *  get the DIE (TMP_DIE_TEMP) temperature.
 *  The Sensors Boosterpack contains a TMP007 where you can get
 *  DIE (TMP_DIE_TEMP) or OBJECT (TMP_OBJ_TEMP) temperature.
 *  We are using the OBJECT temperature for the TMP007 because it cool!
 *  If you are on a CC3220 LP and want to use the Sensors BP, please
 *  remove the ONBOARD_TMP006 define from the compiler options.
 *
 *  Additionally: no calibration is being done on the TMP00x device to simplify
 *  the example code.
 */
#if defined(ONBOARD_TMP006)
#define TMP_REGISTER TMP_DIE_TEMP
#define TMP_ADDR     Board_TMP_ADDR
#else
#define TMP_REGISTER TMP_OBJ_TEMP
#define TMP_ADDR     Board_SENSORS_BP_TMP_ADDR
#endif

/* Temperature written by the temperature thread and read by console thread */
float temperatureC;
float temperatureF;

/*
 *  ======== clearAlert ========
 *  Clear the LED
 */
static void clearAlert(float temperature)
{
    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
}

/*
 *  ======== sendAlert ========
 *  Okay, just light a LED in this example, but with the SimpleLink SDK,
 *  you could send it out over the radio to something cool!
 */
static void sendAlert(float temperature)
{
    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
}

/*
 *  ======== postSem ========
 *  Function called when the timer (created in setupTimer) expires.
 */
static Void postSem(UArg arg)
{
    Semaphore_post((Semaphore_Handle)arg);
}

/*
 *  ======== setupTimer ========
 *  Create a timer that will expire at the period specified by the
 *  time arguments. When the timer expires, the passed in semaphore
 *  will be posted by the postSem function.
 *
 *  A NULL return indicates a failure.
 */
Semaphore_Handle setupTimer(Clock_Handle *clockHandle, unsigned int ms)
{
    Clock_Params clockParams;
	Semaphore_Handle semHandle;

    /* Use default parameters */
    semHandle = Semaphore_create(0, NULL, Error_IGNORE);
    if (semHandle == NULL) {
        return(NULL);
    }

    /* Create the timer that wakes up the thread that will pend on the sem. */
    Clock_Params_init(&clockParams);
    clockParams.arg = (UArg)semHandle;
    clockParams.startFlag = TRUE;
    clockParams.period = ms;
    *clockHandle = Clock_create(postSem, ms, &clockParams, Error_IGNORE);
    if (*clockHandle == NULL) {
		Semaphore_delete(&semHandle);
        return(NULL);
    }

    return(semHandle);
}

/*
 *  ======== temperatureThread ========
 *  This thread reads the temperature every second via I2C and sends an
 *  alert if it goes above HIGH_TEMP.
 */
void *temperatureThread(void *arg0)
{
    uint8_t          txBuffer[1];
    uint8_t          rxBuffer[2];
    I2C_Handle       i2c;
    I2C_Params       i2cParams;
    I2C_Transaction  i2cTransaction;
	Semaphore_Handle semHandle;
    Clock_Handle     clockHandle;
    Int key;

    /*
     *  Create/Open the I2C that talks to the TMP sensor
     */
    I2C_init();

    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2c = I2C_open(Board_I2C_TMP, &i2cParams);
    if (i2c == NULL) {
        while (1);
    }

    txBuffer[0] = TMP_REGISTER;
    i2cTransaction.slaveAddress = TMP_ADDR;
    i2cTransaction.writeBuf   = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf    = rxBuffer;
    i2cTransaction.readCount  = 2;

    /*
     *  The temperature thread blocks on the semaphore, which the
     *  timer will post every second. The timer is created in the
     *  setupTimer function. It's returned so the thread could change the
     *  period or delete it if desired.
     */
    semHandle = setupTimer(&clockHandle, 1000);
    if (semHandle == NULL) {
        while (1);
    }

    while (1) {
        if (I2C_transfer(i2c, &i2cTransaction)) {
            /*
             *  Extract degrees C from the received data; see sensor datasheet.
             *  Make sure we are updating the global temperature variables
             *  in a thread-safe manner.
             */
            key = Hwi_disable();
            temperatureC = (rxBuffer[0] << 6) | (rxBuffer[1] >> 2);
            temperatureC *= 0.03125;
            temperatureF = temperatureC * 9 / 5 + 32;
            Hwi_restore(key);

            /*  Send an alert if the temperature is too high!! */
            if ((int)temperatureC >= HIGH_TEMP) {
                sendAlert(temperatureC);
            }
            else {
                clearAlert(temperatureC);
            }
        }

        /* 
		 *  Block until the timer posts the semaphore.
		 *  Since it is waiting forever, no need to check the return code.
		 */
        Semaphore_pend(semHandle, BIOS_WAIT_FOREVER);
    }
}
