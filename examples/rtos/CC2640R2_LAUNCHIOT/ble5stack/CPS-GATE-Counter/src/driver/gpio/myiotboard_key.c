/******************************************************************************

 @file       board_key.c

 @brief This file contains the interface to the SRF06EB Key Service.

 Group: CMCU, SCS
 Target Device: CC2640R2

 ******************************************************************************
 
 Copyright (c) 2014-2017, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 Release Name: simplelink_cc2640r2_sdk_1_40_00_45
 Release Date: 2017-07-20 17:16:59
 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <stdbool.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>

#include <ti/drivers/pin/PINCC26XX.h>

#ifdef USE_ICALL
#include <icall.h>
#endif

#include <inc/hw_ints.h>

#include "util.h"
#include "myiotboard_key.h"
#include "board.h"

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void Board_keyChangeHandler(UArg a0);
static void Board_keyCallback(PIN_Handle hPin, PIN_Id pinId);

/*******************************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// Value of keys Pressed
static uint8_t keysPressed;

// Key debounce clock
static Clock_Struct keyChangeClock;

// Pointer to application callback
keysPressedCB_t appKeyChangeHandler = NULL;

// Memory for the GPIO module to construct a Hwi
Hwi_Struct callbackHwiKeys;

// PIN configuration structure to set all KEY pins as inputs with pullups enabled
PIN_Config keyPinsCfg[] =
{
//#if defined(CC2640R2MOD_RGZ)
//    Board_BTN1          | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  |  PIN_PULLUP,
//    Board_BTN2          | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  |  PIN_PULLUP,
//#endif
    
#if defined(CC2640R2MOD_RSM)
    IOID_0          | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  |  PIN_NOPULL,//Front PIR
    IOID_1          | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  |  PIN_NOPULL,//Side PIR
    //IOID_2          | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  |  PIN_NOPULL,//Laser IR
#endif
    PIN_TERMINATE
};

//PIN_State  keyPins;
//PIN_Handle hKeyPins;

PIN_State  touchPins;
PIN_Handle hTouchPins;

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
/*********************************************************************
 * @fn      Board_initKeys
 *
 * @brief   Enable interrupts for keys on GPIOs.
 *
 * @param   appKeyCB - application key pressed callback
 *
 * @return  none
 */
void Board_initKeys(keysPressedCB_t appKeyCB)
{
  // Initialize KEY pins. Enable int after callback registered
  //hKeyPins = PIN_open(&keyPins, keyPinsCfg);
  //PIN_registerIntCb(hKeyPins, Board_keyCallback);
  
  hTouchPins = PIN_open(&touchPins, keyPinsCfg);
  PIN_registerIntCb(hTouchPins, Board_keyCallback);

//#if defined(CC2640R2MOD_RGZ)
//  PIN_setConfig(hKeyPins, PIN_BM_IRQ, Board_BTN1        | PIN_IRQ_NEGEDGE);
//  PIN_setConfig(hKeyPins, PIN_BM_IRQ, Board_BTN2        | PIN_IRQ_NEGEDGE);
//#endif
  
#if defined(CC2640R2MOD_RSM)
  PIN_setConfig(hTouchPins, PIN_BM_IRQ, IOID_0        | PIN_IRQ_POSEDGE);//Front PIR
  PIN_setConfig(hTouchPins, PIN_BM_IRQ, IOID_1        | PIN_IRQ_POSEDGE);//Side PIR
  //PIN_setConfig(hTouchPins, PIN_BM_IRQ, IOID_2        | PIN_IRQ_POSEDGE);//Laser Int
#endif

#ifdef POWER_SAVING
  //Enable wakeup
//#if defined(CC2640R2MOD_RGZ)
//  PIN_setConfig(hKeyPins, PINCC26XX_BM_WAKEUP, Board_BTN1        | PINCC26XX_WAKEUP_NEGEDGE);
//  PIN_setConfig(hKeyPins, PINCC26XX_BM_WAKEUP, Board_BTN2        | PINCC26XX_WAKEUP_NEGEDGE);
//#endif
  
#if defined(CC2640R2MOD_RSM)
  PIN_setConfig(hTouchPins, PINCC26XX_BM_WAKEUP, IOID_0        | PINCC26XX_WAKEUP_POSEDGE);
  PIN_setConfig(hTouchPins, PINCC26XX_BM_WAKEUP, IOID_1        | PINCC26XX_WAKEUP_POSEDGE);
//  PIN_setConfig(hTouchPins, PINCC26XX_BM_WAKEUP, IOID_2        | PINCC26XX_WAKEUP_POSEDGE);
#endif
  
#endif //POWER_SAVING

  // Setup keycallback for keys
  Util_constructClock(&keyChangeClock, Board_keyChangeHandler,
                      KEY_DEBOUNCE_TIMEOUT, 0, false, 0);

  // Set the application callback
  appKeyChangeHandler = appKeyCB;
}

/*********************************************************************
 * @fn      Board_keyCallback
 *
 * @brief   Interrupt handler for Keys
 *
 * @param   none
 *
 * @return  none
 */
static void Board_keyCallback(PIN_Handle hPin, PIN_Id pinId)
{
  Util_startClock(&keyChangeClock);
}

/*********************************************************************
 * @fn      Board_keyChangeHandler
 *
 * @brief   Handler for key change
 *
 * @param   UArg a0 - ignored
 *
 * @return  none
 */

extern bool frontActived;
extern bool sideActived;
extern bool goingIn; // Front->Side = going in
extern bool goingOut; // Side->Front = going out
extern bool VL53EVT_POSTED;

static void Board_keyChangeHandler(UArg a0)
{
  if (appKeyChangeHandler != NULL)
  {
    keysPressed = 0;
      
    #if defined(CC2640R2MOD_RSM)
      if ( PIN_getInputValue(IOID_0) == 1 )
      {
        if (frontActived == false){
		frontActived = true;
		if(sideActived == true){
			goingIn = true;
		}
                keysPressed |= FrontPIR;
        }
      }
      
      if ( PIN_getInputValue(IOID_1) == 1 )
      {
            if (sideActived == false){
		sideActived = true;
		if(frontActived == true){
			goingOut = true;
		}
                keysPressed |= SidePIR;
        }
      }
    #endif
    
    // Notify the application
      if(VL53EVT_POSTED == false)
        (*appKeyChangeHandler)(keysPressed);
  }
}
/*********************************************************************
*********************************************************************/
