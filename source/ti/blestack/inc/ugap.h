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

#if defined(FEATURE_OBSERVER)

/** @defgroup UGAP_States uGAP Scanning States
 * @{
 */
#define UG_SCAN_STATE_INVALID     0 //!< Invalid state. Not even initialized
#define UG_SCAN_STATE_INITIALIZED 1 //!< Initialized. Waiting for start request
#define UG_SCAN_STATE_IDLE        2 //!< Scanning stopped or Waiting stopped
#define UG_SCAN_STATE_SCANNING    3 //!< Scanning 
#define UG_SCAN_STATE_WAITING     4 //!< Scanning pulsed in Scan intervl
#define UG_SCAN_STATE_SUSPENDED   5 //!< Scanning or Waiting is suspended 
/** @} End UGAP_States */

/** @defgroup UGAP_Events uGAP Observer Events
 * @{
 */
#define UGO_EVT_STATE_CHANGE         4 //!< Observer's state has changed
#define UGO_EVT_SCAN_INDICATION      5 //!< Scan indication
#define UGO_EVT_SCAN_WINDOW_COMPLETE 6 //!< Scan window completed
/** @} End UGAP_Events */
#endif /* FEATURE_OBSERVER */
  
#if defined(FEATURE_MONITOR)

/** @defgroup UGAP_States uGAP Monitoring States
 * @{
 */
#define UG_MONITOR_STATE_INVALID     0 //!< Invalid state. Not even initialized
#define UG_MONITOR_STATE_INITIALIZED 1 //!< Initialized. Waiting for start request
#define UG_MONITOR_STATE_IDLE        2 //!< Monitoring stopped
#define UG_MONITOR_STATE_MONITORING  3 //!< Monitor scanning 
/** @} End UGAP_States */

/** @defgroup UGAP_Events uGAP Monitoring Events
 * @{
 */
#define UGM_EVT_STATE_CHANGE         7 //!< Monitor's state has changed
#define UGM_EVT_MONITOR_INDICATION   8 //!< Monitor indication
#define UGM_EVT_MONITOR_COMPLETE     9 //!< Monitor completed
/** @} End UGAP_Events */
#endif /* FEATURE_MONITOR */  
  
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

#if defined(FEATURE_OBSERVER)

/** @defgroup UGAP_Structures Micro GAP Structures
 * @{
 */
typedef uint8 ugoScanState_t;      //!< Observer state

/** @} End UGAP_Structures */

/** @defgroup UGAP_Callbacks Micro GAP Callbacks
 * @{
 */

/**
 * @brief Callback of when the Broadcaster's state changes.
 *
 * @param state @ref UGAP_States
 */
typedef void (*ugoStateChangeCb_t)(ugoScanState_t state);

/**
 * @brief  Callback of when a packet is received.
 *
 * @param status status of a scan
 * @param len length of the payload
 * @param pPayload pointer to payload
 */
typedef void (*ugoScanIndicationCb_t)(bStatus_t status, uint8_t len, uint8_t *pPayload);

/**
 * @brief  Callback of when a scan has just been done.
 *
 * @param status status of a scan
 */
typedef void (*ugoScanWindowCompleteCb_t)(bStatus_t status);

/**
 * @brief  Set of Observer callbacks - must be setup by the application.
 */
typedef struct
{
  ugoStateChangeCb_t        pfnStateChangeCB; //!< State change notification
  ugoScanIndicationCb_t     pfnScanIndicationCB; //!< Scan indication with payload
  ugoScanWindowCompleteCb_t pfnScanWindowCompleteCB; //!< Notification of scan window complete
} ugoScanCBs_t;

PACKED_TYPEDEF_STRUCT {
  ugoScanState_t state;	//!< new state
} ugoMsgStateChange_t; //!< Message payload for the event @ref UGO_EVT_STATE_CHANGE

PACKED_TYPEDEF_STRUCT {
  bStatus_t status; //!< scan indication process status
  uint8_t len; //!< Rx payload length
  uint8_t *pPayload; //!< pointer to Rx payload
} ugoMsgScanIndication_t; //!< Message payload for the event @ref UGO_EVT_SCAN_INDICATION

PACKED_TYPEDEF_STRUCT {
  bStatus_t status; //!< scan complete process status
} ugoMsgScanWindowComplete_t; //!< Message payload for the event @ref UGO_EVT_SCAN_WINDOW_COMPLETE

/** @} End UGAP_Callbacks */

#endif /* FEATURE_OBSERVER */

#if defined(FEATURE_MONITOR)

/** @defgroup UGAP_Structures Micro GAP Structures
 * @{
 */
typedef uint8 ugmMonitorState_t;      //!< Monitor state

/** @} End UGAP_Structures */

/** @defgroup UGAP_Callbacks Micro GAP Callbacks
 * @{
 */

/**
 * @brief Callback of when the Monitor's state changes.
 *
 * @param state @ref UGAP_States
 */
typedef void (*ugmStateChangeCb_t)(ugmMonitorState_t state);

/**
 * @brief  Callback of when a packet is received.
 *
 * @param status status of a monitoring scan
 * @param sessionId session ID
 * @param len length of the payload
 * @param pPayload pointer to payload
 */
typedef void (*ugmMonitorIndicationCb_t)(bStatus_t status, uint8_t sessionId, uint8_t len, uint8_t *pPayload);

/**
 * @brief  Callback of when a scan monitoring has just been done.
 *
 * @param status status of a scan
 * @param sessionId session ID
 */
typedef void (*ugmMonitorCompleteCb_t)(bStatus_t status, uint8_t sessionId);

/**
 * @brief  Set of Observer callbacks - must be setup by the application.
 */
typedef struct
{
  ugmStateChangeCb_t        pfnStateChangeCB; //!< State change notification
  ugmMonitorIndicationCb_t  pfnMonitorIndicationCB; //!< Scan monitor indication with payload
  ugmMonitorCompleteCb_t    pfnMonitorCompleteCB; //!< Notification of monitoring scan complete
} ugmMonitorCBs_t;

PACKED_TYPEDEF_STRUCT {
  ugmMonitorState_t state;	//!< new state
} ugmMsgStateChange_t; //!< Message payload for the event @ref UGM_EVT_STATE_CHANGE

PACKED_TYPEDEF_STRUCT {
  bStatus_t status; //!< monitor indication process status
  uint8_t sessionId; //!< monitor session ID
  uint8_t len; //!< Rx payload length
  uint8_t *pPayload; //!< pointer to Rx payload
} ugmMsgMonitorIndication_t; //!< Message payload for the event @ref UGM_EVT_MONITOR_INDICATION

PACKED_TYPEDEF_STRUCT {
  bStatus_t status; //!< monitoring scan complete process status
  uint8_t sessionId; //!< monitor session ID
} ugmMsgMonitorComplete_t; //!< Message payload for the event @ref UGM_EVT_MONITOR_COMPLETE

/** @} End UGAP_Callbacks */

#endif /* FEATURE_MONITOR */

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
bStatus_t ugap_bcastInit(ugBcastCBs_t* pCBs);

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
bStatus_t ugap_bcastSetDuty(uint16 dutyOnTime, uint16 dutyOffTime);

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
bStatus_t ugap_bcastStart(uint16 numAdvEvent);

/**
 * @brief   Stop Broadcaster. Proceed the state from either Advertising or
 *          Waiting to IDLE.
 *
 * @return  @ref SUCCESS
 * @return 	@ref INVALIDPARAMETER
 * @return  @ref FAILURE
 */
bStatus_t ugap_bcastStop(void);
#endif /* FEATURE_BROADCASTER */

#if defined(FEATURE_OBSERVER)
/*********************************************************************
 * @fn      ugap_scanInit
 *
 * @brief   Initialize Micro Observer
 *
 * @param   pCBs - a set of application callbacks for Ovserver
 *
 * @return  SUCCESS, INVALIDPARAMETER, FAILURE
 */
bStatus_t ugap_scanInit(ugoScanCBs_t* pCBs);

/*********************************************************************
 * @fn      ugap_scanStart
 *
 * @brief   Start Observer scanning. Proceed the state from either Initialized
 *          or IDLE to Scanning.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t ugap_scanStart(void);

/*********************************************************************
 * @fn      ugap_scanStop
 *
 * @brief   Stop Observer scanning. Proceed the state from Scanning to Idle.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t ugap_scanStop(void);

/*********************************************************************
 * @fn      ugap_scanSuspend
 *
 * @brief   Suspend Observer scanning. Proceed the state from either Waiting
 *          or Scanning to Suspended. Suspend the Obesrver to make room
 *          for the Advertiser to transmit.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t ugap_scanSuspend(void);

/*********************************************************************
 * @fn      ugap_scanResume
 *
 * @brief   Resume a suspended Observer. Proceed the state from Suspended 
 *          to Scanning to Waiting. 
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t ugap_scanResume(void);

/*********************************************************************
 * @fn      ugap_scanRequest
 *
 * @brief   uGAP scan request
 *
 * @param   scanChanMap - Channels to scan advertising PDUs. One of more
 *                        of channels 37, 38 & 39. 
 *          scanWindow - Scan window to listen on a primary advertising
 *                       channel index for the duration of the scan window.
 *                       Unit in 0.625 ms; range from 2.5 ms to 40.959375 s.
 *          scanInterval - Scan interval. This is defined as the start of 
 *                         two consecutive scan windows. Unit in 0.625 ms; 
 *                         range from 2.5 ms to 40.959375 s. When scanInterval
 *                         is the same as sacnWindow, a continuous scan 
 *                         is requested.
 *
 * @return  SUCCESS or INVALIDPARAMETER
 */
bStatus_t ugap_scanRequest(uint8_t scanChanMap, uint16_t scanWindow, uint16_t scanInterval);

#endif /* FEATURE_OBSERVER */

#if defined(FEATURE_MONITOR)
/*********************************************************************
 * @fn      ugap_monitorInit
 *
 * @brief   Initialize Micro Monitor
 *
 * @param   pCBs - a set of application callbacks for Monitor
 *
 * @return  SUCCESS, INVALIDPARAMETER, FAILURE
 */
bStatus_t ugap_monitorInit(ugmMonitorCBs_t* pCBs);

/*********************************************************************
 * @fn      ugap_monitorStart
 *
 * @brief   Start Monitor scanning. Proceed the state from either 
 *          Initialized or Idle to Monitoring.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t ugap_monitorStart(void);

/*********************************************************************
 * @fn      ugap_monitorStop
 *
 * @brief   Stop Monitor scanning. Proceed the state from Monitoring
 *          to Idle.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t ugap_monitorStop(void);

/*********************************************************************
 * @fn      ugap_monitorRequest
 *
 * @brief   uGAP monitor request
 *
 * @param   channel - Channels to monitor.
 *          accessAddr - Access address. 0xFFFFFFFF is used as a 
 *                       wildcard access address. An error status will 
 *                       be returned if the channel is one of 37, 38, 
 *                       or 39 but the accessAddr is not 0x8E89BED6.
 *          startTime - Absolute start time in RAT ticks.
 *          duration - Scan window per scanChanIndexchanIndex. A monitor 
 *                     complete time-out statuscallback will be sent to 
 *                     the application. A zero indicates a continuous 
 *                     scan. Unit in 0.625 ms; range from 2.5 ms to 
 *                     40.959375 s.
 *
 * @return  SUCCESS or INVALIDPARAMETER
 */
bStatus_t ugap_monitorRequest(uint8_t channel, 
                              uint32_t accessAddr,
                              uint32_t startTime,
                              uint16_t duration);

#endif /* FEATURE_MONITOR */

/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* UGAP_H */

/** @} End UGAP */

/** @} */ // end of Micro_BLE_Stack
