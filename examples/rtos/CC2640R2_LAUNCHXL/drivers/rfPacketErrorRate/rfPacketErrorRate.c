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

/***** Includes *****/

/* XDCtools Header files */
#include <xdc/runtime/System.h>
#include <xdc/runtime/Assert.h>
#include <xdc/std.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>

/* TI-RTOS Header files */
#include <ti/display/Display.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>

/* Board Header files */
#include "Board.h"

/* Application specific Header files */
#include "menu.h"

/***** Variable declarations *****/

/* Pin driver objects and handles */
PIN_Handle buttonPinHandle;
static PIN_State buttonPinState;

/*
Application button pin configuration table.
Buttons interrupts are configured to trigger on falling edge.
*/
PIN_Config buttonPinTable[] =
{
    Board_PIN_BUTTON0 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    Board_PIN_BUTTON1 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
#if defined __CC1350_LAUNCHXL_BOARD_H__
    Board_DIO1_RFSW | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    Board_DIO30_SWPWR | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
#endif
#if defined __CC1310_LAUNCHXL_BOARD_H__
    Board_HGM | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    Board_LNA_EN | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    Board_PA_EN | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
#endif
#if defined __CC1352R1_LAUNCHXL_BOARD_H__
    Board_DIO30_RFSW | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
#endif
    PIN_TERMINATE,
};

/*
Interrupt handler for the button pints.
*/
void buttonCallbackFunction(PIN_Handle handle, PIN_Id pinId)
{
    /* Simple debounce logic, only toggle if the button is still pushed (low) */
    CPUdelay((uint32_t)((48000000/3)*0.050f));
    if (!PIN_getInputValue(pinId))
    {
        if (PIN_getInputValue(Board_PIN_BUTTON0))
        {
            menu_notifyButtonPressed(Button_Navigate);
        }
        else if (PIN_getInputValue(Board_PIN_BUTTON1))
        {
            menu_notifyButtonPressed(Button_Select);
        }
    }
}

int main(void)
{
    /* Initialize pins and peripherals */
    Board_initGeneral();

    /* Initialise the UART and SPI for the display driver. */
    Display_init();
    UART_init();
    SPI_init();

    /* Initialize the menu task and resources */
    menu_init();

    /* Open Button pins */
    buttonPinHandle = PIN_open(&buttonPinState, buttonPinTable);
    Assert_isTrue(buttonPinHandle != NULL, NULL);

    /* Setup callback for button pins */
    PIN_Status status = PIN_registerIntCb(buttonPinHandle, &buttonCallbackFunction);
    Assert_isTrue((status == PIN_SUCCESS), NULL);

    /* Start task execution */
    BIOS_start();

    return (0);
}
