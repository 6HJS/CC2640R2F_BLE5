/******************************************************************************

 @file       ull.h

 @brief This file contains the Micro Link Layer (uLL) API for the Micro
        BLE Stack.

 Group: CMCU, SCS
 Target Device: CC2640R2

 ******************************************************************************
 
 Copyright (c) 2009-2017, Texas Instruments Incorporated
 All rights reserved.

 IMPORTANT: Your use of this Software is limited to those specific rights
 granted under the terms of a software license agreement between the user
 who downloaded the software, his/her employer (which must be your employer)
 and Texas Instruments Incorporated (the "License"). You may not use this
 Software unless you agree to abide by the terms of the License. The License
 limits your use, and you acknowledge, that the Software may not be modified,
 copied or distributed unless embedded on a Texas Instruments microcontroller
 or used solely and exclusively in conjunction with a Texas Instruments radio
 frequency transceiver, which is integrated into your product. Other than for
 the foregoing purpose, you may not use, reproduce, copy, prepare derivative
 works of, modify, distribute, perform, display or sell this Software and/or
 its documentation for any purpose.

 YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
 PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
 NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
 TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
 NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
 LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
 INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
 OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
 OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
 (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

 Should you have any questions regarding your right to use this Software,
 contact Texas Instruments Incorporated at www.TI.com.

 ******************************************************************************
 Release Name: simplelink_cc2640r2_sdk_1_40_00_45
 Release Date: 2017-07-20 17:16:59
 *****************************************************************************/

#ifndef ULL_H
#define ULL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
* INCLUDES
*/

/*-------------------------------------------------------------------
 * CONSTANTS
 */

/**
 * Link layer Advertiser Events
 */
#define ULL_EVT_ADV_TX_NO_RADIO_RESOURCE    1 //!< Adv event no radio resource
#define ULL_EVT_ADV_TX_FAILED               2 //!< Adv event tx failed
#define ULL_EVT_ADV_TX_SUCCESS              3 //!< Adv event tx success
#define ULL_EVT_ADV_TX_RADIO_AVAILABLE      4 //!< Adv event radio available
#define ULL_EVT_ADV_TX_TIMER_EXPIRED        5 //!< Adv event interval timer expired

/*****************************************************************************
* DEFINITIONS
*/

/******************************************************************************
* TYPEDEFS
*/

#if defined(FEATURE_ADVERTISER)

/**
 * Callback of when an Adv event is about to start.
 */
typedef void (*pfnAdvAboutToCB_t)(void);

/**
 * Callback of when events come from RF driver regarding Adv command.
 */
typedef void (*pfnAdvDoneCB_t)(bStatus_t status);

#endif /* FEATURE_ADVERTISER */


/*****************************************************************************
 * FUNCTION PROTOTYPES
 */

/*********************************************************************
 * @fn     ul_init
 *
 * @brief  Initialization function for the Micro Link Layer.
 *
 * @param  none
 *
 * @return SUCCESS or FAILURE
 */
bStatus_t ul_init(void);

#if defined(FEATURE_ADVERTISER)
/*********************************************************************
 * @fn     ul_advRegisterCB
 *
 * @brief  Register callbacks supposed to be called by Advertiser.
 *
 * @param  pfnAdvAboutToCB - callback to nofity the application of time to
 *                           update the advertising packet payload.
 * @param  pfnAdvDoneCB    - callback to post-process Advertising Event
 *
 * @return  none
 */
void ul_advRegisterCB(pfnAdvAboutToCB_t pfnAdvAboutToCB,
                      pfnAdvDoneCB_t pfnAdvDoneCB);

/*********************************************************************
 * @fn      ul_advStart
 *
 * @brief   Enter UL_STATE_ADVERTISING
 *
 * @param   none
 *
 * @return  SUCCESS - Successfully entered UL_STATE_ADVERTISING
 *          FAILURE - Failed to enter UL_STATE_ADVERSISING
 */
bStatus_t ul_advStart(void);

/*********************************************************************
 * @fn      ul_advStop
 *
 * @brief   Exit UL_STATE_ADVERTISING
 *
 * @param   none
 *
 * @return  none
 */
void      ul_advStop(void);

#endif /* FEATURE_ADVERTISER */

#ifdef __cplusplus
}
#endif

#endif /* ULL_H */
