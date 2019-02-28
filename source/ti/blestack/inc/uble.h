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
 *  @defgroup Micro_BLE_Stack Micro BLE Stack
 *  @brief This module implmements the Micro BLE Stack
 *  For a detailed usage section describing how to send these commands and receive events,
 *  see the <a href="../u-stack/index.html">Micro BLE Stack Section</a> of the
 *  User's Guide.
 *  @{
 *  @defgroup UBLE Stack Base
 *  @{
 *  @file       uble.h
 *  @brief  This file contains configuration, initialization,
 *          and parameter management of Micro BLE Stack. It also contains
 *          interfacing between Micro BLE Stack and the application.
 */

#ifndef UBLE_H
#define UBLE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*-------------------------------------------------------------------
 * INCLUDES
 */
#include <ti/sysbios/knl/Queue.h>
#include <urfc.h>

/*-------------------------------------------------------------------
 * DEFINITIONS & CONSTANTS
 */

/** @defgroup UBLE_Constants Micro BLE Stack Constants
 * @{
 */

/** @defgroup UBLE_Params Micro BLE Stack Parameters
 * @{
 * @brief These are set via @ref uble_setParameter
 */
/// @cond NODOC
    #define UB_PARAM_COMMON_START       0
/// @endcond NODOC

/**
  * @brief Priority used for RF command scheduling
  *
  * @note This applies to dual mode only
  *
  * default: @ref RF_PRI_NORMAL
  *
  * range: @ref UBLE_Radio_Priority
  */
    #define UB_PARAM_RFPRIORITY         (UB_PARAM_COMMON_START+0)

/**
  * @brief TX Power
  *
  * default: @ref TX_POWER_0_DBM
  *
  * range: @ref URF_TX_Power
  */
    #define UB_PARAM_TXPOWER            (UB_PARAM_COMMON_START+1)

/// @cond NODOC
    #define UB_PARAM_COMMON_END         UB_PARAM_TXPOWER
#if defined(FEATURE_ADVERTISER)
    #define UB_PARAM_ADV_CMN_START      (UB_PARAM_COMMON_END+1)
/// @endcond // NODOC

/**
  * @brief Advertising Interval (n * 0.625 ms)
  *
  * default: 1600
  *
  * range: @ref UB_MIN_ADV_INTERVAL - @ref UB_MAX_ADV_INTERVAL
  */
      #define UB_PARAM_ADVINTERVAL        (UB_PARAM_ADV_CMN_START+0)
/**
  * @brief Advertising Channel map
  *
  * default: @ref UB_ADV_CHAN_ALL
  *
  * range: @ref UBLE_Adv_Chans
  */
      #define UB_PARAM_ADVCHANMAP         (UB_PARAM_ADV_CMN_START+1)
/**
  * @brief Type of Advertising
  *
  * default: @ref UB_ADVTYPE_ADV_NC
  *
  * range: @ref UBLE_Adv_types
  */
      #define UB_PARAM_ADVTYPE            (UB_PARAM_ADV_CMN_START+2)
/**
  * @brief How early the application wants to get a notification before each advertising event happens (ms)
  *
  * The application is notified via the @ref ugbAdvDoneCb_t callback.
  *
  * @note A value of 0 will disable the callback
  *
  * default: 0 (disabled)
  *
  * range: 0-256
  */
      #define UB_PARAM_TIMETOADV          (UB_PARAM_ADV_CMN_START+3)
/**
  * @brief Advertising Data
  *
  * default: {0}
  *
  * range: 0 - @ref UB_MAX_ADVDATA_LEN bytes
  */
      #define UB_PARAM_ADVDATA            (UB_PARAM_ADV_CMN_START+4)

/// @cond NODOC
    #define UB_PARAM_ADV_CMN_END        UB_PARAM_ADVDATA
  #if defined(FEATURE_SCAN_RESPONSE)
    #define UB_PARAM_ADV_SR_START       (UB_PARAM_ADV_CMN_END+1)
/// @endcond NODOC

/**
  * @brief Scan response Data
  *
  * default: {0}
  *
  * range: 0 - @ref UB_MAX_ADVDATA_LEN bytes
  */
    #define UB_PARAM_SCANRSPDATA        (UB_PARAM_ADV_SR_START+0)
/**
  * @brief Advertising Filter Policy
  *
  * not implemented
  *
  * default: TBD
  *
  * range: TBD
  */
    #define UB_PARAM_ADVFLTPOLICY       (UB_PARAM_ADV_SR_START+1)

    #define UB_PARAM_ADV_SR_END         UB_PARAM_ADVFLTPOLICY
  #else  /* FEATURE_SCAN_RESPONSE */
    #define UB_PARAM_ADV_SR_END         UB_PARAM_ADV_CMN_END
  #endif /* FEATURE_SCAN_RESPONSE */
#else  /* FEATURE_ADVERTISER */
    #define UB_PARAM_ADV_SR_END         UB_PARAM_COMMON_END
#endif /* FEATURE_ADVERTISER */
#if defined(FEATURE_SCANNER)
    #define UB_PARAM_SCAN_START         (UB_PARAM_ADV_SR_END+1)
/**
 * @brief UB scan interval (n * 0.625 ms)
 *
 * default: @ref UB_PARAM_DFLT_SCANINTERVAL
 *
 * range: @ref UB_MIN_SCAN_INTERVAL - @ref UB_MAX_SCAN_INTERVAL
 */
    #define UB_PARAM_SCANINTERVAL       (UB_PARAM_SCAN_START+0)
/**
 * @brief The duration of the uLL scan (n * 0.625 ms)
 *
 * default: @ref UB_PARAM_DFLT_SCANWINDOW
 *
 * range: @ref UB_MIN_SCAN_WINDOW - @ref UB_MAX_SCAN_WINDOW
 */
    #define UB_PARAM_SCANWINDOW         (UB_PARAM_SCAN_START+1)
/**
 * @brief Scan Channel map
 *
 * default: @ref UB_ADV_CHAN_ALL
 *
 * range: @ref UBLE_Adv_Chans
 */
    #define UB_PARAM_SCANCHANMAP        (UB_PARAM_SCAN_START+2)
/**
 * @brief Scan Channel map
 *
 * default: TBD
 *
 * range: @ref UBLE_Adv_Chans
 */
    #define UB_PARAM_SCANTYPE           (UB_PARAM_SCAN_START+3)
/**
 * @brief Scan filter duplicate
 *
 * default: TBD
 *
 * range: TBD
 */
    #define UB_PARAM_FLTDUPLICATE       (UB_PARAM_SCAN_START+4)
/**
 * @brief Scanning filter policy
 *
 * default: TBD
 *
 * range: TBD
 */
    #define UB_PARAM_SCANFLTPOLICY      (UB_PARAM_SCAN_START+5)
/// @cond NODOC
    #define UB_PARAM_SCAN_END           UB_PARAM_SCANFLTPOLICY
#else  /*FEATURE_SCANNER */
    #define UB_PARAM_SCAN_END           UB_PARAM_ADV_SR_END
#endif /* FEATURE_SCANNER */
    #if defined(FEATURE_MONITOR)
    #define UB_PARAM_MONITOR_START         (UB_PARAM_ADV_SR_END+1)
/// @endcond NODOC

/**
 * @brief Monitor session ID. 1..4
 *
 * default: TBD
 *
 * range: TBD
 */
    #define UB_PARAM_SESSIONID         (UB_PARAM_MONITOR_START+0)
/**
 * @brief Monitor channel. 0..39
 *
 * default: TBD
 *
 * range: TBD
 */
    #define UB_PARAM_MONITOR_CHAN      (UB_PARAM_MONITOR_START+1)
/**
 * @brief Monitor duration is (n * 0.625 ms)
 *
 * default: TBD
 *
 * range: TBD
 */
    #define UB_PARAM_MONITOR_DURATION  (UB_PARAM_MONITOR_START+2)
/**
 * @brief Monitor start time
 *
 * default: TBD
 *
 * range: TBD
 */
    #define UB_PARAM_MONITOR_START_TIME  (UB_PARAM_MONITOR_START+3)
/**
 * @brief Monitor access address
 *
 * default: TBD
 *
 * range: TBD
 */
    #define UB_PARAM_MONITOR_ACCESS_ADDR (UB_PARAM_MONITOR_START+4)
/// @cond NODOC
    #define UB_PARAM_MONITOR_END           UB_PARAM_MONITOR_ACCESS_ADDR
#else  /* FEATURE_MONITOR */
    #define UB_PARAM_MONITOR_END           UB_PARAM_ADV_SR_END
#endif /* FEATURE_MONITOR */
    #define UB_NUM_PARAM  (UB_PARAM_MONITOR_END+1)
/// @endcond NODOC
/** @} End UBLE_Params */

/** @defgroup UBLE_Radio_Priority Micro BLE Stack Radio Priorities
 * @{
 */
#define RF_PRI_NORMAL      ((uint8) RF_PriorityNormal) //!< Normal Priority
#define RF_PRI_HIGH        ((uint8) RF_PriorityHigh)    //!< High Priority
#define RF_PRI_HIGHEST     ((uint8) RF_PriorityHighest) //!< Highest Priority
/** @} End UBLE_Radio_Priority */

/** @defgroup UBLE_Radio_Criticality Micro BLE Stack Radio Criticality
 * @{
 */
#define RF_TIME_RELAXED    0    //!< Relaxed Timing
#define RF_TIME_CRITICAL   1    //!< Critical Timing
/** @} End UBLE_Radio_Criticality */

/** @brief Invalid TX Power Register value */
#define UB_TX_POWER_INVALID 0xFFFF

/** @defgroup UBLE_Address_types Micro BLE Stack Address Types
 * @{
 */
#define UB_ADDRTYPE_PUBLIC          0 //!< Public Address
#define UB_ADDRTYPE_STATIC          1 //!< Random Static Address
#define UB_ADDRTYPE_NONRESOLVABLE   2 //!< Random Private Nonresolvable Address
#define UB_ADDRTYPE_RESOLVABLE      3 //!< Random Private Resolvable Address
#define UB_ADDRTYPE_BD               4 //!< Device that is currently being used
/** @} End UBLE_Address_types */

/** @defgroup UBLE_Adv_types Micro BLE Stack Advertising Types
 * @{
 */
#define UB_ADVTYPE_ADV_NC   5   //!< associated with CMD_BLE_ADV_NC
#define UB_ADVTYPE_ADV_SCAN 6   //!< associated with CMD_BLE_ADV_SCAN
/** @} End UBLE_Adv_types */

/** @defgroup UBLE_Adv_Chans Micro BLE Stack Advertising Channel Masks
 * @{
 * Applicable to Advertising and Scanning events
 */
#define UB_ADV_CHAN_37   0x01 //!< Channel 37
#define UB_ADV_CHAN_38   0x02 //!< Channel 38
#define UB_ADV_CHAN_39   0x04 //!< Channel 39
#define UB_ADV_CHAN_ALL  (UB_ADV_CHAN_37 | UB_ADV_CHAN_38 | UB_ADV_CHAN_39)  //!< All Channels
/** @} End UBLE_Adv_Chans */

/* Min/Max for parameters */
#define UB_MIN_ADV_INTERVAL    160  //!< Minimum Advertising interval
#define UB_MAX_ADV_INTERVAL    16384 //!< Maximum Advertising Interval
#define UB_MIN_CHANNEL_MAP     UB_ADV_CHAN_37 //!< Minimum Channel Map
#define UB_MAX_CHANNEL_MAP     UB_ADV_CHAN_ALL    //!< Maximum Channel Map
#define UB_MIN_TX_POWER        TX_POWER_MINUS_21_DBM //!< -21 dBm
#define UB_MAX_TX_POWER        TX_POWER_5_DBM        //!<   5 dBm
#define UB_MIN_SCAN_INTERVAL   4 //!< Minimum Scanning interval
#define UB_MAX_SCAN_INTERVAL   16384 //!< Maximum Scanning Interval
#define UB_MIN_SCAN_WINDOW     4 //!< Minimum Scanning window
#define UB_MAX_SCAN_WINDOW     16384 //!< Maximum Scanning window
#define UB_MIN_MONITOR_HANDLE    0 //!< Minimum Monitor handle
#define UB_MAX_MONITOR_HANDLE    4 //!< Maximum Monitor handle
#define UB_MIN_MONITOR_CHAN      0 //!< Minimum Monitor channel
#define UB_MAX_MONITOR_CHAN      39 //!< Maximum Monitor channel
#define UB_MIN_MONITOR_DURATION  0 //!< Minimum Monitor duration
#define UB_MAX_MONITOR_DURATION  64000 //!< Maximum Monitor duration

/* Default Parameter Values */
#define UB_PARAM_DFLT_RFPRIORITY      RF_PRI_NORMAL     //!< RF_PriorityNormal
#define UB_PARAM_DFLT_RFTIMECRIT      RF_TIME_RELAXED   //!< Time-relaxed
#define UB_PARAM_DFLT_TXPOWER         TX_POWER_0_DBM    //!< 0 dBm
#define UB_PARAM_DFLT_ADVINTERVAL     1600              //!< 1 sec
#define UB_PARAM_DFLT_ADVCHANMAP      UB_ADV_CHAN_ALL   //!< All Channels
#define UB_PARAM_DFLT_ADVTYPE         UB_ADVTYPE_ADV_NC //!< ADV_NONCONN_IND
#define UB_PARAM_DFLT_TIMETOADV       0                 //!< Disabled
#define UB_PARAM_DFLT_ADVDATA         {0}               //!< No data
#define UB_PARAM_DFLT_SCANRSPDATA     {0}               //!< No data
#define UB_PARAM_DFLT_ADVFLTPOLICY    0                 //!< TBD
#define UB_PARAM_DFLT_SCANINTERVAL    16                //!< Scan interval
#define UB_PARAM_DFLT_SCANWINDOW      16                //!< Scan window
#define UB_PARAM_DFLT_SCANCHAN        UB_ADV_CHAN_ALL   //!< All Channels
#define UB_PARAM_DFLT_SCANTYPE        0                 //!< TBD
#define UB_PARAM_DFLT_FLTDUPLICATE    0                 //!< TBD
#define UB_PARAM_DFLT_SCANFLTPOLICY   0                 //!< TBD
#define UB_PARAM_DFLT_MONITOR_HANDLE    0               //!< TBD
#define UB_PARAM_DFLT_MONITOR_CHAN      37              //!< TBD
#define UB_PARAM_DFLT_MONITOR_DURATION  320             //!< TBD: 200 ms
#define UB_PARAM_DFLT_MONITOR_START_TIME  6400          //!< TBD: 4 s
#define UB_PARAM_DFLT_MONITOR_ACCESS_ADDR 0x8E89BED6    //!< Adv address


/**
 * Event Message Destination
 */
#define UB_EVTDST_LL       0      //!< LL
#define UB_EVTDST_GAP      1      //!< GAP
#define UB_NUM_EVTDST      2      //!< Number of Event Destinations

/**
 * Unit conversions
 */
/** @brief 1 us = 4 RAT tick */
#define US_TO_RAT        4
/** @brief 1 ms = 4000 RAT tick */
#define MS_TO_RAT        4000
/** @brief 1 BLE tick(0.625 ms) = 2500 RAT tick */
#define BLE_TO_RAT       2500
/** @brief 1 system tick = (Clock_tickPeriod * US_TO_RAT) RAT tick */
#define SYSTICK_TO_RAT   (Clock_tickPeriod * US_TO_RAT)
/** @brief 1 ms = (1000 / Clock_tickPeriod) system tick */
#define MS_TO_SYSTICK    (1000 / Clock_tickPeriod)
  /** @brief 0.625 ms = (625 / Clock_tickPeriod) system tick */
#define BLE_TO_SYSTICK   (625 / Clock_tickPeriod)
/** @brief 1 byte = 8 us = 32 RAT tick, assuming 1 Mbps PHY */
#define BYTE_TO_RAT      (8 * US_TO_RAT)

/* Length of ADV Data */
#define UB_MAX_ADVDATA_LEN         B_MAX_ADV_LEN  //!< Max Advertising Data Length

/** @} End UBLE_Constants */

/*-------------------------------------------------------------------
 * TYPEDEFS
 */

/** @defgroup UBLE_Structures Micro BLE Stack Structures
 * @{
 */

/** @brief Address Type */
typedef uint8 ubAddrType_t;

/** @brief Recipient of Micro BLE Stack Event */
typedef uint8 ubEvtDst_t;

/** @brief Micro BLE Stack Event */
typedef uint8 ubEvt_t;

PACKED_TYPEDEF_STRUCT {
  Queue_Elem    _elem;    //!< queue element
  ubEvtDst_t    dst;      //!< destination
  ubEvt_t       evt;      //!< event
} ubEvtHdr_t;  //!< Type of Micro BLE Stack event header

/** @brief Type of Micro BLE Stack message */
typedef uint8 ubMsg_t;

PACKED_TYPEDEF_STRUCT {
  ubEvtHdr_t    hdr;      //!< header
  ubMsg_t       msg[];    //!< message
} ubEvtMsg_t; //!< Type of Micro BLE Stack event message

/** @} End UBLE_Structures */

/** @defgroup UBLE_Callbacks Micro BLE Stack Callbacks
 * @{
 */

/**
 * @brief Type of the callback to have @ref uble_processMsg called in the application
 * task context.
 *
 * @warning The application should provide a valid callback when calling
 * @ref uble_stackInit
 */
typedef void (*ubPostEvtProxyCB_t)(void);

/**
 * @brief Function to process event messages destined to the calle module.
 */
typedef void (*ubProcessMsg_t)(ubEvtMsg_t *pEvtMsg);

/** @} End UBLE_Callbacks */

/// @cond NODOC
/** @brief Type of Micro BLE Stack Parameters */
PACKED_TYPEDEF_STRUCT {
  uint8  rfPriority;              //!< RF Priority
  int8   txPower;                 //!< TX Power in dBm
#if defined(FEATURE_ADVERTISER)
  uint16 advInterval;             //!< Advertising Interval. 0.625 ms unit. Range: 0x0020 to 0x4000
  uint8  advChanMap;              //!< Advertising Channel Map. 37, 38 and 39 correspond to bit 0, 1 and 2
  uint8  advType;                 //!< Advertising Type - ADV_NONCONN_IND or ADV_SCAN_IND
  uint8  timeToAdv;               //!< How early to notify before the next advertising in ms
  uint8  advData[UB_MAX_ADVDATA_LEN];     //!< Advertising Data
#if defined(FEATURE_SCAN_RESPONSE)
  uint8  scanRspData[UB_MAX_ADVDATA_LEN];   //!< Scan Response Data
  uint8  advFltPolicy;            //!< Advertising Filter Policy
#endif /* FEATURE_SCAN_RESPONSE */
#endif /* FEATURE_ADVERTISER */
#if defined(FEATURE_SCANNER)
  uint16 scanInterval;            //!< Scan Interval. 0.625 ms unit. Range: 0x0004 to 0x4000
  uint16 scanWindow;              //!< Scan Window. 0.625 ms unit. Range: 0x0004 to 0x4000
  uint8  scanChanMap;             //!< Scanning Channel Map. 37, 38 and 39 correspond to bit 0, 1 and 2
  uint8  scanType;                //!< Scanning Type - SCAN_PASSIVE or SCAN_ACTIVE
  uint8  fltDuplicate;            //!< Whether to filter out duplicate advertising report or not
  uint8  scanFltPolicy;           //!< Scan Filter Policy
#endif /* FEATURE_SCANNER */
#if defined(FEATURE_MONITOR)
  uint8  monitorHandle;           //!< Monitor handle. Range: 1..CM_MAX_SESSIONS
  uint8  monitorChan;             //!< Monitor channel. Range: 0..39
  uint16 monitorDuration;         //!< Monitor duration. 0.625 ms unit. Range: 0x0004 to 0x4000
  uint32 startTime;               //!< Monitor start time in RAT ticks
  uint32 accessAddr;              //!< Monitor access address
#endif /* FEATURE_MONITOR */
} ubParams_t;

/**
 * @brief Lookup for a parameter.
 *
 * @note Min/max work only for uint8-type parameters
 */
PACKED_TYPEDEF_STRUCT {
  uint8  offset;
  uint8  len;
  uint8  min;
  uint8  max;
} ubParamLookup_t;
/// @endcond // NODOC

/*-------------------------------------------------------------------
 * API FUNCTIONS
 */

/**
 * @brief  Initialize Micro BLE Stack
 *
 * @param  addrType - @ref UBLE_Address_types <br>
 *             Type of the address which the device will use initially.
 *             Either @ref UB_ADDRTYPE_PUBLIC or @ref UB_ADDRTYPE_STATIC . If addrType is
 *             @ref UB_ADDRTYPE_PUBLIC , the public address which is stored in CCFG
 *             preferably or in FCFG otherwise will be used for BD Addr. If
 *             addrType is @ref UB_ADDRTYPE_STATIC , BD Addr will depend on pBDAddr.
 * @param  pStaticAddr - If this is NULL and addrType is @ref UB_ADDRTYPE_STATIC , a
 *             random static address will be generated by the stack and will be
 *             used for BD Address. If this is not NULL and addrType is
 *             @ref UB_ADDRTYPE_STATIC , the 6 byte-long data pointed to by pBDAddr
 *             will be used for BD Address as the random static address.
 * @param  pfnPostEvtProxyCB - Callback to trigger a context switch to the
 *             application task so that the messages queued by the stack can be
 *             processed by @ref uble_processMsg in the application task context.
 *             @warning This callback MUST be provided
 *             by the application. The role of the callback is to post a RTOS
 *             event or semaphore to the application itself so that when
 *             the application task is activated it can call @ref uble_processMsg
 *             in the application task context.
 * @param  timeCritical - rf transmit operation. For strict timing on the
 *             transmission, set value to RF_TIME_CRITICAL. For relaxed timing
 *             on the transmission set value to RF_TIME_RELAXED.
 *
 * @return @ref SUCCESS - success
 * @return @ref INVALIDPARAMETER - addrType is invalid or pfnPostEvtProxyCB is NULL
 */
bStatus_t uble_stackInit(ubAddrType_t addrType, uint8* pStaticAddr,
                         ubPostEvtProxyCB_t pfnPostEvtProxyCB, uint8 timeCritical);

/**
 * @brief  Set a Micro BLE Stack parameter.
 *
 * @param   param - parameter ID: @ref UBLE_Params
 * @param   len - length of data to write
 * @param   pValue - pointer to data to write. This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  @ref SUCCESS
 * @return @ref INVALIDPARAMETER
 * @return @ref bleInvalidRange
 */
bStatus_t uble_setParameter(uint8 param, uint8 len, void *pValue);

/**
 * @brief  Get a Micro BLE Stack parameter.
 *
 * @param   param - parameter ID: @ref UBLE_Params
 * @param   pValue - pointer to location to get the value. This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return @ref SUCCESS
 * @return @ref INVALIDPARAMETER
 */
bStatus_t uble_getParameter(uint8 param, void *pValue);

/**
 * @brief   Check if the given address is valid
 *
 * @param   addrType - @ref UB_ADDRTYPE_PUBLIC , @ref UB_ADDRTYPE_STATIC ,
 *                     @ref UB_ADDRTYPE_NONRESOLVABLE , or @ref UB_ADDRTYPE_RESOLVABLE
 * @param   pAddr    - pointer to where the address to check resides
 *
 * @return @ref SUCCESS
 * @return @ref INVALIDPARAMETER
 * @return @ref FAILURE
 */
bStatus_t uble_checkAddr(ubAddrType_t addrType, uint8 *pAddr);

/**
 * @brief  Make BD Addr revert to Random Static Address if exists or
 *         Public Address otherwise.
 */
void uble_resetAddr(void);

/**
 * @brief  Retrieve the address corresponding to the given address type
 *
 * @param addrType - @ref UB_ADDRTYPE_PUBLIC , @ref UB_ADDRTYPE_STATIC or @ref UB_ADDRTYPE_BD
 * @param pAddr pointer to memory location to return address. This should be a uint8_t[6]
 *
 * @return @ref SUCCESS
 * @return @ref INVALIDPARAMETER
 * @return @ref FAILURE
 */
bStatus_t uble_getAddr(ubAddrType_t addrType, uint8* pAddr);

/**
 * @brief  Let the Micro Stack process the queued messages.
 *
 * @warning The application MUST call this function when it
 *         process the event posted by the stack event proxy function registered
 *         through @ref uble_stackInit
 */
void uble_processMsg(void);

/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* UBLE_H */

/** @} End UBLE */

/** @} End Micro_BLE_Stack */
