/******************************************************************************

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

/**
 *  @addtogroup Micro_BLE_Stack
 *  @{
 *  @defgroup UGAP Micro Gap Layer
 *  @{
 *  @file       ugap.h
 *  @brief  Micro BLE GAP API
 *
 *  This file contains the interface to the Micro GAP Layer.
 */

#ifndef UGAP_H
#define UGAP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*-------------------------------------------------------------------
 * INCLUDES
 */

/*-------------------------------------------------------------------
 * CONSTANTS
 */

/** @defgroup UGAP_Constants Micro GAP Constants
 * @{
 */

/// @brief Duty Control Time Unit
#define UG_DUTY_TIME_UNIT    100L  //!< 100 ms unit

#if defined(FEATURE_BROADCASTER)

/** @defgroup UGAP_States uGAP Broadcaster States
 * @{
 */
#define UG_BCAST_STATE_INVALID     0 //!< Invalid state. Not even initialized
#define UG_BCAST_STATE_INITIALIZED 1 //!< Initialized. Waiting for start request
#define UG_BCAST_STATE_IDLE        2 //!< Advertising or Waiting stopped
#define UG_BCAST_STATE_ADVERTISING 3 //!< Advertising
#define UG_BCAST_STATE_WAITING     4 //!< Advertising paused during duty off period
#define UG_BCAST_STATE_SUSPENDED   5 //!< Advertising or Waiting is suspended
/** @} End UGAP_States */

/** @defgroup UGAP_Events uGAP Broadcaster Events
 * @{
 */
#define UGB_EVT_STATE_CHANGE       1 //!< Broadcaster's state has changed
#define UGB_EVT_ADV_PREPARE        2 //!< About to issue the next adv. Prepare adv payload
#define UGB_EVT_ADV_POSTPROCESS    3 //!< Adv has just been done. Do post processing if necessary
/** @} End UGAP_Events */
#endif /* FEATURE_BROADCASTER */

/** @} End UGAP_Constants */

/*-------------------------------------------------------------------
 * TYPEDEFS
 */

#if defined(FEATURE_BROADCASTER)

/** @defgroup UGAP_Structures Micro GAP Structures
 * @{
 */
typedef uint8 ugBcastState_t;      //!< Broadcaster state

/** @} End UGAP_Structures */

/** @defgroup UGAP_Callbacks Micro GAP Callbacks
 * @{
 */

/**
 * @brief Callback of when the Broadcaster's state changes.
 *
 * @param state @ref UGAP_States
 */
typedef void (*ugbStateChangeCb_t)(ugBcastState_t state);

/**
 * @brief Callback of when the next Adverting Event is about to start
 *
 * The application can use this to update the payload if necessary
 */
typedef void (*ugbAdvPrepareCb_t)(void);

/**
 * @brief  Callback of when an Advertising Event has just been done.
 *
 * @param status status of advertising done event
 */
typedef void (*ugbAdvDoneCb_t)(bStatus_t status);

/**
 * @brief  Set of Broadcaster callbacks - must be setup by the application.
 */
typedef struct
{
  ugbStateChangeCb_t   pfnStateChangeCB; //!< State change notification
  ugbAdvPrepareCb_t    pfnAdvPrepareCB;  //!< Notification for Adv preperation
  ugbAdvDoneCb_t       pfnAdvDoneCB;     //!< Notification of Adv event done
} ugBcastCBs_t;

PACKED_TYPEDEF_STRUCT {
  ugBcastState_t state;		//!< new state
} ugbMsgStateChange_t; //!< Message payload for the event @ref UGB_EVT_STATE_CHANGE

PACKED_TYPEDEF_STRUCT {
  bStatus_t      status;		//!< advertising post process status
} ugbMsgAdvPostprocess_t; //!< Message payload for the event @ref UGB_EVT_ADV_POSTPROCESS

/** @} End UGAP_Callbacks */

#endif /* FEATURE_BROADCASTER */

/*-------------------------------------------------------------------
 * MACROS
 */

/*-------------------------------------------------------------------
 * API FUNCTIONS
 */

#if defined(FEATURE_BROADCASTER)
/**
 * @brief   Initialize Micro Broadcaster
 *
 * @param   pCBs - a set of application callbacks for Broadcaster
 *
 * @return  @ref SUCCESS
 * @return  @ref INVALIDPARAMETER
 * @return  @ref FAILURE
 */
bStatus_t ug_bcastInit(ugBcastCBs_t* pCBs);

/**
 * @brief   Set Broadcaster Duty On/Off time
 *
 * @param   dutyOnTime  - Time period during which the Broadcaster
 *                        is in Advertising state. 100 ms unit. This cannot be
 *                        0 unless dutyOffTime is also 0.
 * @param   dutyOffTime - Time period during which the Broadcaster
 *                        is in Waiting state. 100 ms unit. If this is 0,
 *                        Duty Control is disabled regardless of dutyOnTime.
 *
 * @return  @ref SUCCESS
 * @return  @ref INVALIDPARAMETER
 */
bStatus_t ug_bcastSetDuty(uint16 dutyOnTime, uint16 dutyOffTime);

/**
 * @brief   Start Broadcaster. Proceed the state from either Initialized or IDLE
 *          to Advertising.
 *
 * @param   numAdvEvent - # of Advertising events to be performed before
 *                        the Broadcaster state goes to IDLE. If this is 0,
 *                        the Broadcaster will keep staying in Advertising or
 *                        Advertising and Waiting alternately unless an error
 *                        happens or the application requests to stop.
 *
 * @return  @ref SUCCESS
 * @return 	@ref FAILURE
 * @return	@ref INVALIDPARAMETER
 */
bStatus_t ug_bcastStart(uint16 numAdvEvent);

/**
 * @brief   Stop Broadcaster. Proceed the state from either Advertising or
 *          Waiting to IDLE.
 *
 * @return  @ref SUCCESS
 * @return 	@ref INVALIDPARAMETER
 * @return  @ref FAILURE
 */
bStatus_t ug_bcastStop(void);
#endif /* FEATURE_BROADCASTER */

/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* UGAP_H */

/** @} End UGAP */

/** @} */ // end of Micro_BLE_Stack
