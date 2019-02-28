/******************************************************************************

@file       multi_role.c

@brief This file contains the multi_role sample application for use
with the CC2650 Bluetooth Low Energy Protocol Stack.

Group: CMCU, SCS
Target Device: CC2640R2

******************************************************************************

 Copyright (c) 2013-2017, Texas Instruments Incorporated
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
#include <string.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/display/Display.h>
#include <ti/mw/lcd/LCDDogm1286.h>

#if defined( USE_FPGA ) || defined( DEBUG_SW_TRACE )
#include <driverlib/ioc.h>
#endif // USE_FPGA | DEBUG_SW_TRACE

#include <icall.h>
#include "util.h"
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"

#include "devinfoservice.h"
#include "uarttrans_service.h"
#include "multi.h"

#include "iotboard_key.h"
#include "board.h"

#include "multi_role.h"

#include "inc\sdi_task.h"
/*********************************************************************
* CONSTANTS
*/

// Maximum number of scan responses
// this can only be set to 15 because that is the maximum
// amount of item actions the menu module supports
#define DEFAULT_MAX_SCAN_RES                  8

// Advertising interval when device is discoverable (units of 625us, 160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL          160

// Limited discoverable mode advertises for 30.72s, and then stops
// General discoverable mode advertises indefinitely
#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_GENERAL

// Connection parameters
#define DEFAULT_CONN_INT                      20
#define DEFAULT_CONN_TIMEOUT                  100
#define DEFAULT_CONN_LATENCY                  0

// Default service discovery timer delay in ms
#define DEFAULT_SVC_DISCOVERY_DELAY           1000

// Scan parameters
#define DEFAULT_SCAN_DURATION                 4000
#define DEFAULT_SCAN_WIND                     80
#define DEFAULT_SCAN_INT                      80

// Discovey mode (limited, general, all)
#define DEFAULT_DISCOVERY_MODE                DEVDISC_MODE_ALL

// TRUE to use active scan
#define DEFAULT_DISCOVERY_ACTIVE_SCAN         TRUE

// TRUE to use white list during discovery
#define DEFAULT_DISCOVERY_WHITE_LIST          FALSE

// TRUE to use high scan duty cycle when creating link
#define DEFAULT_LINK_HIGH_DUTY_CYCLE          FALSE

// TRUE to use white list when creating link
#define DEFAULT_LINK_WHITE_LIST               FALSE

// Task configuration
#define MR_TASK_PRIORITY                     1
#ifndef MR_TASK_STACK_SIZE
#define MR_TASK_STACK_SIZE                   1024
#endif

// Internal Events for RTOS application
#define MR_ICALL_EVT                         ICALL_MSG_EVENT_ID // Event_Id_31
#define MR_QUEUE_EVT                         UTIL_QUEUE_EVENT_ID // Event_Id_30
#define MR_STATE_CHANGE_EVT                  Event_Id_00
#define MR_CHAR_CHANGE_EVT                   Event_Id_01
#define MR_CONN_EVT_END_EVT                  Event_Id_02
#define MR_KEY_CHANGE_EVT                    Event_Id_03
#define MR_UART_QUEUE_EVT                    Event_Id_04
#define MR_NOTI_ENABLE_EVT                   Event_Id_05

#define MR_ALL_EVENTS                        (MR_ICALL_EVT           | \
                                             MR_QUEUE_EVT            | \
                                             MR_STATE_CHANGE_EVT     | \
                                             MR_CHAR_CHANGE_EVT      | \
                                             MR_CONN_EVT_END_EVT     | \
                                             MR_KEY_CHANGE_EVT       | \
                                             MR_UART_QUEUE_EVT       | \
                                             MR_NOTI_ENABLE_EVT)

// Discovery states
typedef enum {
  BLE_DISC_STATE_IDLE,                // Idle
  BLE_DISC_STATE_MTU,                 // Exchange ATT MTU size
  BLE_DISC_STATE_SVC,                 // Service discovery
  BLE_DISC_STATE_CHAR                 // Characteristic discovery
} discState_t;

// address string length is an ascii character for each digit +
// an initial 0x + an ending null character
#define B_STR_ADDR_LEN       ((B_ADDR_LEN*2) + 3)

// How often to perform periodic event (in msec)
#define MR_PERIODIC_EVT_PERIOD               5000


/*********************************************************************
* TYPEDEFS
*/

// App event passed from profiles.
typedef struct
{
  uint16_t event;  // event type
  uint8_t *pData;  // event data pointer
} mrEvt_t;

// discovery information
typedef struct
{
  discState_t discState;   // discovery state
  uint16_t svcStartHdl;    // service start handle
  uint16_t svcEndHdl;      // service end handle
  uint16_t charHdl;        // characteristic handle
} discInfo_t;

// Discovered characteristic handle
static uint16_t charDataHdl = 0;

// Discovered characteristic CCCD handle
static uint16_t charCCCDHdl = 0;

//UUID of Serial Port Data Characteristic
static uint8_t uuidDataChar[ATT_UUID_SIZE] = { TI_BASE_UUID_128(SERIALPORTSERVICE_DATA_UUID) };

// device discovery information with room for string address
typedef struct
{
  uint8_t eventType;                // Indicates advertising event type used by the advertiser: @ref GAP_ADVERTISEMENT_REPORT_TYPE_DEFINES
  uint8_t addrType;                 // Address Type: @ref ADDRTYPE_DEFINES
  uint8_t addr[B_ADDR_LEN];         // Device's Address
  uint8_t strAddr[B_STR_ADDR_LEN];  // Device Address as String
} mrDevRec_t;

// entry to map index to connection handle and store address string for menu module
typedef struct
{
  uint16_t connHandle;              // connection handle of an active connection
  uint8_t strAddr[B_STR_ADDR_LEN];  // memory location for menu module to store address string
} connHandleMapEntry_t;

// RTOS queue for profile/app messages.
typedef struct _queueRec_
{
  Queue_Elem _elem;          // queue element
  uint8_t *pData;            // pointer to app data
} queueRec_t;

// App event passed from profiles.
typedef struct
{
  appEvtHdr_t hdr;  // event header.
} sbpEvt_t;

// App event passed from profiles.
typedef struct
{
  uint8_t event;  // Type of event
  uint8_t *pData;  // New data
  uint8_t length; // New status
} sbpUARTEvt_t;
/*********************************************************************
* GLOBAL VARIABLES
*/

// Display Interface
Display_Handle dispHandle = NULL;

/*********************************************************************
* LOCAL VARIABLES
*/

/*********************************************************************
* LOCAL VARIABLES
*/

// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID selfEntity;

// Event globally used to post local events and pend on system and
// local events.
static ICall_SyncHandle syncEvent;

// Clock object used to signal timeout
static Clock_Struct startNotiEnableClock;

// Queue object used for app messages
static Queue_Struct appMsg;
static Queue_Handle appMsgQueue;

// Queue object used for UART messages
static Queue_Struct appUARTMsg;
static Queue_Handle appUARTMsgQueue;

// Task configuration
Task_Struct mrTask;
Char mrTaskStack[MR_TASK_STACK_SIZE];

static uint8_t scanRspData[] =
{
  // complete name
  11,   // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
  'M', 'u', 'l', 't', 'i', ' ', 'R', 'o', 'l', 'e',



  // Tx power level
  0x02,   // length of this data
  GAP_ADTYPE_POWER_LEVEL,
  0       // 0dBm
};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
static uint8_t advertData[] =
{
  // Flags; this sets the device to use limited discoverable
  // mode (advertises for 30 seconds at a time) instead of general
  // discoverable mode (advertises indefinitely)
  0x02,   // length of this data
  GAP_ADTYPE_FLAGS,
  DEFAULT_DISCOVERABLE_MODE | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
};

// pointer to allocate the connection handle map
static connHandleMapEntry_t *connHandleMap;

// GAP GATT Attributes
static uint8_t attDeviceName[GAP_DEVICE_NAME_LEN] = "Multi Role :)";

// Globals used for ATT Response retransmission
static gattMsgEvent_t *pAttRsp = NULL;
static uint8_t rspTxRetry = 0;

// Pointer to per connection discovery info
discInfo_t *discInfo;

// Maximim PDU size (default = 27 octets)
static uint16 maxPduSize;

// Scan result list
static mrDevRec_t devList[DEFAULT_MAX_SCAN_RES];


// Dummy parameters to use for connection updates
gapRole_updateConnParams_t updateParams =
{
  .connHandle = INVALID_CONNHANDLE,
  .minConnInterval = 80,
  .maxConnInterval = 150,
  .slaveLatency = 0,
  .timeoutMultiplier = 200
};

// Connection index for mapping connection handles
static uint16_t connIndex = INVALID_CONNHANDLE;

// Maximum number of connected devices
static uint8_t maxNumBleConns = MAX_NUM_BLE_CONNS;

static uint8_t multiConnRole = 0;
/*********************************************************************
* LOCAL FUNCTIONS
*/
static void multi_role_init( void );
static void multi_role_taskFxn(UArg a0, UArg a1);
static uint8_t multi_role_processStackMsg(ICall_Hdr *pMsg);
static uint8_t multi_role_processGATTMsg(gattMsgEvent_t *pMsg);
static void multi_role_processAppMsg(mrEvt_t *pMsg);
static void multi_role_processCharValueChangeEvt(uint8_t paramID);
static void multi_role_processRoleEvent(gapMultiRoleEvent_t *pEvent);
static void multi_role_sendAttRsp(void);
static void multi_role_freeAttRsp(uint8_t status);
static void multi_role_charValueChangeCB(uint8_t paramID);
static uint8_t multi_role_enqueueMsg(uint16_t event, uint8_t *pData);
static void multi_role_startDiscovery(uint16_t connHandle);
static void multi_role_processGATTDiscEvent(gattMsgEvent_t *pMsg);
static void multi_role_handleKeys(uint8_t keys);
static uint8_t multi_role_eventCB(gapMultiRoleEvent_t *pEvent);

static uint16_t multi_role_mapConnHandleToIndex(uint16_t connHandle);
static void multi_role_keyChangeHandler(uint8_t keysPressed);
static uint8_t multi_role_addMappingEntry(uint16_t connHandle, uint8_t *addr);

void multi_role_enqueueUARTMsg(uint8_t event, uint8_t *data, uint8_t len);
void multi_role_connect(void);
static void multi_role_genericHandler(UArg arg);
/*********************************************************************
 * EXTERN FUNCTIONS
*/
extern void AssertHandler(uint8 assertCause, uint8 assertSubcause);

/*********************************************************************
* PROFILE CALLBACKS
*/

// GAP Role Callbacks
static gapRolesCBs_t multi_role_gapRoleCBs =
{
  multi_role_eventCB,                   // Events to be handled by the app are passed through the GAP Role here
};

// Simple GATT Profile Callbacks
static SerialPortServiceCBs_t SPPBLEServer_SerialPortService_CBs =
{
  multi_role_charValueChangeCB // Characteristic value change callback
};

/*********************************************************************
* PUBLIC FUNCTIONS
*/

/*********************************************************************
* @fn      multi_role_createTask
*
* @brief   Task creation function for multi_role.
*
* @param   None.
*
* @return  None.
*/
void multi_role_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = mrTaskStack;
  taskParams.stackSize = MR_TASK_STACK_SIZE;
  taskParams.priority = MR_TASK_PRIORITY;

  Task_construct(&mrTask, multi_role_taskFxn, &taskParams, NULL);
}

/*********************************************************************
* @fn      multi_role_init
*
* @brief   Called during initialization and contains application
*          specific initialization (ie. hardware initialization/setup,
*          table initialization, power up notification, etc), and
*          profile initialization/setup.
*
* @param   None.
*
* @return  None.
*/
static void multi_role_init(void)
{
  // ******************************************************************
  // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ******************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
  ICall_registerApp(&selfEntity, &syncEvent);

  // Hard code the BD Address till CC2650 board gets its own IEEE address
  uint8 bdAddress[B_ADDR_LEN] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
  HCI_EXT_SetBDADDRCmd(bdAddress);
  
  // Create an RTOS queue for message from profile to be sent to app.
  appMsgQueue = Util_constructQueue(&appMsg);

  // Create an RTOS queue for message from profile to be sent to uart.
  appUARTMsgQueue = Util_constructQueue(&appUARTMsg);

  // Setup discovery delay as a one-shot timer
  Util_constructClock(&startNotiEnableClock, multi_role_genericHandler,
                      200, 0, false, MR_NOTI_ENABLE_EVT);
  

  // Init keys and LCD
  Board_initKeys(multi_role_keyChangeHandler);

  // Setup the GAP
  {
    // Set advertising interval the same for all scenarios
    uint16_t advInt = DEFAULT_ADVERTISING_INTERVAL;
    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MAX, advInt);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MAX, advInt);
    GAP_SetParamValue(TGAP_CONN_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_CONN_ADV_INT_MAX, advInt);

    // Set scan duration
    GAP_SetParamValue(TGAP_GEN_DISC_SCAN, DEFAULT_SCAN_DURATION);

    // Scan interval and window the same for all scenarios
    GAP_SetParamValue(TGAP_CONN_SCAN_INT, DEFAULT_SCAN_INT);
    GAP_SetParamValue(TGAP_CONN_SCAN_WIND, DEFAULT_SCAN_WIND);
    GAP_SetParamValue(TGAP_CONN_HIGH_SCAN_INT, DEFAULT_SCAN_INT);
    GAP_SetParamValue(TGAP_CONN_HIGH_SCAN_WIND, DEFAULT_SCAN_WIND);
    GAP_SetParamValue(TGAP_GEN_DISC_SCAN_INT, DEFAULT_SCAN_INT);
    GAP_SetParamValue(TGAP_GEN_DISC_SCAN_WIND, DEFAULT_SCAN_WIND);
    GAP_SetParamValue(TGAP_LIM_DISC_SCAN_INT, DEFAULT_SCAN_INT);
    GAP_SetParamValue(TGAP_LIM_DISC_SCAN_WIND, DEFAULT_SCAN_WIND);
    GAP_SetParamValue(TGAP_CONN_EST_SCAN_INT, DEFAULT_SCAN_INT);
    GAP_SetParamValue(TGAP_CONN_EST_SCAN_WIND, DEFAULT_SCAN_WIND);

    // Set connection parameters
    GAP_SetParamValue(TGAP_CONN_EST_INT_MIN, DEFAULT_CONN_INT);
    GAP_SetParamValue(TGAP_CONN_EST_INT_MAX, DEFAULT_CONN_INT);
    GAP_SetParamValue(TGAP_CONN_EST_SUPERV_TIMEOUT, DEFAULT_CONN_TIMEOUT);
    GAP_SetParamValue(TGAP_CONN_EST_LATENCY, DEFAULT_CONN_LATENCY);

    // Register to receive GAP and HCI messages
    GAP_RegisterForMsgs(selfEntity);
  }

  // Setup the GAP Role Profile
  {
    /*--------PERIPHERAL-------------*/
    uint8_t initialAdvertEnable = TRUE;
    uint16_t advertOffTime = 0;

    // device starts advertising upon initialization
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                         &initialAdvertEnable, NULL);

    // By setting this to zero, the device will go into the waiting state after
    // being discoverable for 30.72 second, and will not being advertising again
    // until the enabler is set back to TRUE
    GAPRole_SetParameter(GAPROLE_ADVERT_OFF_TIME, sizeof(uint16_t),
                         &advertOffTime, NULL);

    // Set scan response data
    GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData),
                         scanRspData, NULL);

    // Set advertising data
    GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData, NULL);

    // set max amount of scan responses
    uint8_t scanRes = DEFAULT_MAX_SCAN_RES;

    // Set the max amount of scan responses
    GAPRole_SetParameter(GAPROLE_MAX_SCAN_RES, sizeof(uint8_t),
                         &scanRes, NULL);

    // Start the GAPRole and negotiate max number of connections
    VOID GAPRole_StartDevice(&multi_role_gapRoleCBs, &maxNumBleConns);

    // Allocate memory for index to connection handle map
    if (connHandleMap = ICall_malloc(sizeof(connHandleMapEntry_t) * maxNumBleConns))
    {
      // Init index to connection handle map
      for (uint8_t i = 0; i < maxNumBleConns; i++)
      {
        connHandleMap[i].connHandle = INVALID_CONNHANDLE;
      }
    }

    // Allocate memory for per connection discovery information
    if (discInfo = ICall_malloc(sizeof(discInfo_t) * maxNumBleConns))
    {
      // Init index to connection handle map to 0's
      for (uint8_t i = 0; i < maxNumBleConns; i++)
      {
        discInfo[i].charHdl = 0;
        discInfo[i].discState = BLE_DISC_STATE_IDLE;
        discInfo[i].svcEndHdl = 0;
        discInfo[i].svcStartHdl = 0;
      }
    }
  }

  // GATT
  {
    // Set the GAP Characteristics
    GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName);

    // Initialize GATT Server Services
    GGS_AddService(GATT_ALL_SERVICES);           // GAP
    GATTServApp_AddService(GATT_ALL_SERVICES);   // GATT attributes
    DevInfo_AddService();                        // Device Information Service
    SerialPortService_AddService(GATT_ALL_SERVICES);  //SerialPortBLE service
    
    // Register callback with SimpleGATTprofile
    SerialPortService_RegisterAppCBs(&SPPBLEServer_SerialPortService_CBs);
    
#if defined (BLE_V42_FEATURES) && (BLE_V42_FEATURES & PRIVACY_1_2_CFG)
    // Initialize GATT Client
    GATT_InitClient();

    // The line masks the RPAO Characteristic in the GAP GATT Server from being
    // detected by remote devices. This value cannot be toggled without power
    // cycling but should remain consistent across power-cycles. Removing this
    // command when Privacy is used will cause this device to be treated in Network
    // Privacy Mode by bonded devices - this means that after disconnecting they
    // will not respond to this device's PDUs which contain its Identity Address.
    GGS_SetParamValue(GGS_DISABLE_RPAO_CHARACTERISTIC);
#endif // BLE_V42_FEATURES & PRIVACY_1_2_CFG

    /*-----------------CLIENT------------------*/
    // Initialize GATT Client
    VOID GATT_InitClient();

    // Register for GATT local events and ATT Responses pending for transmission
    GATT_RegisterForMsgs(selfEntity);

    // Register to receive incoming ATT Indications/Notifications
    GATT_RegisterForInd(selfEntity);
  }


#if !defined (USE_LL_CONN_PARAM_UPDATE)
  // Get the currently set local supported LE features
  // The HCI will generate an HCI event that will get received in the main
  // loop
  HCI_LE_ReadLocalSupportedFeaturesCmd();
#endif // !defined (USE_LL_CONN_PARAM_UPDATE)

  
  #define APP_SUGGESTED_PDU_SIZE 109 //default is 27 octets(TX)
  #define APP_SUGGESTED_TX_TIME 1120 //default is 328us(TX)
  HCI_LE_WriteSuggestedDefaultDataLenCmd(APP_SUGGESTED_PDU_SIZE, APP_SUGGESTED_TX_TIME);

#ifdef SDI_USE_UART
    //Register to receive UART messages
  SDITask_registerIncomingRXEventAppCB(multi_role_enqueueUARTMsg);
#endif
}

/*********************************************************************
* @fn      multi_role_taskFxn
*
* @brief   Application task entry point for the multi_role.
*
* @param   a0, a1 - not used.
*
* @return  None.
*/
static void multi_role_taskFxn(UArg a0, UArg a1)
{
  // Initialize application
  multi_role_init();

  // Application main loop
  for (;;)
  {
    uint32_t events;

    // Waits for an event to be posted associated with the calling thread.
    // Note that an event associated with a thread is posted when a
    // message is queued to the message receive queue of the thread
    events = Event_pend(syncEvent, Event_Id_NONE, MR_ALL_EVENTS,
                        ICALL_TIMEOUT_FOREVER);

    if (events)
    {
      ICall_EntityID dest;
      ICall_ServiceEnum src;
      ICall_HciExtEvt *pMsg = NULL;

      if (ICall_fetchServiceMsg(&src, &dest,
                                (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
      {
        uint8_t safeToDealloc = TRUE;

        if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))
        {
          ICall_Stack_Event *pEvt = (ICall_Stack_Event *)pMsg;

          // Check for BLE stack events first
          if (pEvt->signature == 0xffff)
          {
            if (pEvt->event_flag & MR_CONN_EVT_END_EVT)
            {
              // Try to retransmit pending ATT Response (if any)
              multi_role_sendAttRsp();
            }
          }
          else
          {
            // Process inter-task message
            safeToDealloc = multi_role_processStackMsg((ICall_Hdr *)pMsg);
          }
        }

        if (pMsg && safeToDealloc)
        {
          ICall_freeMsg(pMsg);
        }
      }

      // If RTOS queue is not empty, process app message.
      if (events & MR_QUEUE_EVT)
      {
        while (!Queue_empty(appMsgQueue))
        {
          mrEvt_t *pMsg = (mrEvt_t *)Util_dequeueMsg(appMsgQueue);
          if (pMsg)
          {
            // Process message.
            multi_role_processAppMsg(pMsg);

            // Free the space from the message.
            ICall_free(pMsg);
          }
        }
      }

      // If RTOS queue is not empty, process app message.
      if (events & MR_UART_QUEUE_EVT)
      {
        // If RTOS queue is not empty, process app message.
        if (!Queue_empty(appUARTMsgQueue))
        {
          //Get the message at the front of the queue but still keep it in the queue
          queueRec_t *pRec = Queue_head(appUARTMsgQueue);
          sbpUARTEvt_t *pMsg = (sbpUARTEvt_t *)pRec->pData;

          if(multiConnRole == GAP_PROFILE_PERIPHERAL)   
          {
            bStatus_t retVal = FAILURE;
            //Send the notification
            retVal = SerialPortService_SetParameter(SERIALPORTSERVICE_CHAR_DATA, pMsg->length, pMsg->pData);

            if(retVal != SUCCESS)
            {
              SDITask_PrintfToUART("%s\r\n", "Noti FAIL");
            }
            else
            {
              //Remove from queue
              Util_dequeueMsg(appUARTMsgQueue);

              //Deallocate data payload being transmitted.
              ICall_freeMsg(pMsg->pData);
              // Free the space from the message.
              ICall_free(pMsg);
            }

            if(!Queue_empty(appUARTMsgQueue))
            {
              // Wake up the application to flush out any remaining UART data in the queue.
              Event_post(syncEvent, MR_UART_QUEUE_EVT);
            }
          }
          else if(multiConnRole == GAP_PROFILE_CENTRAL)
          {
            // Process message.
            bStatus_t retVal = FAILURE;

            // Do a write
            attWriteReq_t req;

            //Allocate data bytes to send over the air
            req.pValue = GATT_bm_alloc(connHandleMap[0].connHandle, ATT_WRITE_REQ, pMsg->length, NULL);

            if ( (req.pValue != NULL) && charDataHdl)
            {
              req.handle = charDataHdl; //handle for Value of Data characteristic found during service discovery
              req.len = pMsg->length;
              memcpy(req.pValue, pMsg->pData, pMsg->length);
              req.sig = FALSE;
              req.cmd = TRUE;

              retVal = GATT_WriteNoRsp(connHandleMap[0].connHandle, &req);

              if ( retVal != SUCCESS )
              {
                GATT_bm_free((gattMsg_t *)&req, ATT_WRITE_REQ);
                SDITask_PrintfToUART("%s%d\r\n", "FAIL FROM CLIENT: ", retVal);
              }else
              {
                //Remove from the queue
                Util_dequeueMsg(appUARTMsgQueue);

                //Deallocate data payload being transmitted.
                ICall_freeMsg(pMsg->pData);
                // Free the space from the message.
                ICall_free(pMsg);

                if(!Queue_empty(appUARTMsgQueue))
                {
                  // Wake up the application to flush out any remaining UART data in the queue.
                  Event_post(syncEvent, MR_UART_QUEUE_EVT);
                }
              }
            }
            else
            {
              SDITask_PrintfToUART("%s\r\n", "Allocation ERROR!");
            }
          }
        }
      }
      if (events & MR_NOTI_ENABLE_EVT)
      {
        // Process message.
        uint8 retVal;
        attWriteReq_t req;
        uint8 configData[2] = {0x01,0x00};

        req.pValue = GATT_bm_alloc(connHandleMap[0].connHandle, ATT_WRITE_REQ, 2, NULL);

        if ((charCCCDHdl == NULL) && (charDataHdl != NULL)) {charCCCDHdl = charDataHdl + 1;} //Hardcoded
        if ( (req.pValue != NULL) && charCCCDHdl)
        {
          req.handle = charCCCDHdl; //Handle for CCCD of Data characteristic
          req.len = 2;
          memcpy(req.pValue, configData, 2);
          req.cmd = TRUE; //Has to be true for NoRsp from server(command, not request)
          req.sig = FALSE;
          retVal = GATT_WriteNoRsp(connHandleMap[0].connHandle, &req);
          if (retVal != SUCCESS)
          {
            SDITask_PrintfToUART("%s\r\n", "ERROR enabling notification...");
          }
          else
          {
            SDITask_PrintfToUART("%s\r\n", "Notification enabled...");
          }
        }
      }
    }
  }
}

/*********************************************************************
* @fn      multi_role_processStackMsg
*
* @brief   Process an incoming stack message.
*
* @param   pMsg - message to process
*
* @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
*/
static uint8_t multi_role_processStackMsg(ICall_Hdr *pMsg)
{
  uint8_t safeToDealloc = TRUE;

  switch (pMsg->event)
  {
    case GAP_MSG_EVENT:
      multi_role_processRoleEvent((gapMultiRoleEvent_t *)pMsg);
      break;
      
    case GATT_MSG_EVENT:
      // Process GATT message
      safeToDealloc = multi_role_processGATTMsg((gattMsgEvent_t *)pMsg);
      break;

    case HCI_GAP_EVENT_EVENT:
      {
        // Process HCI message
        switch(pMsg->status)
        {
          case HCI_COMMAND_COMPLETE_EVENT_CODE:
            // Process HCI Command Complete Event
            {
  #if !defined (USE_LL_CONN_PARAM_UPDATE)

              // This code will disable the use of the LL_CONNECTION_PARAM_REQ
              // control procedure (for connection parameter updates, the
              // L2CAP Connection Parameter Update procedure will be used
              // instead). To re-enable the LL_CONNECTION_PARAM_REQ control
              // procedures, define the symbol USE_LL_CONN_PARAM_UPDATE

              // Parse Command Complete Event for opcode and status
              hciEvt_CmdComplete_t* command_complete = (hciEvt_CmdComplete_t*) pMsg;
              uint8_t   pktStatus = command_complete->pReturnParam[0];

              //find which command this command complete is for
              switch (command_complete->cmdOpcode)
              {
                case HCI_LE_READ_LOCAL_SUPPORTED_FEATURES:
                  {
                    if (pktStatus == SUCCESS)
                    {
                      uint8_t featSet[8];

                      // get current feature set from received event (bits 1-9 of
                      // the returned data
                      memcpy( featSet, &command_complete->pReturnParam[1], 8 );

                      // Clear bit 1 of byte 0 of feature set to disable LL
                      // Connection Parameter Updates
                      CLR_FEATURE_FLAG( featSet[0], LL_FEATURE_CONN_PARAMS_REQ );

                      // Update controller with modified features
                      HCI_EXT_SetLocalSupportedFeaturesCmd( featSet );
                    }
                  }
                  break;

                default:
                  //do nothing
                  break;
              }
  #endif // !defined (USE_LL_CONN_PARAM_UPDATE)

            }
            break;

          case HCI_BLE_HARDWARE_ERROR_EVENT_CODE:
            AssertHandler(HAL_ASSERT_CAUSE_HARDWARE_ERROR,0);
            break;

          default:
            break;
        }
      }
      break;

    default:
      // Do nothing
      break;
  }

  return (safeToDealloc);
}

/*********************************************************************
* @fn      multi_role_processGATTMsg
*
* @brief   Process GATT messages and events.
*
* @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
*/
static uint8_t multi_role_processGATTMsg(gattMsgEvent_t *pMsg)
{
  // See if GATT server was unable to transmit an ATT response
  if (pMsg->hdr.status == blePending)
  {
    // No HCI buffer was available. Let's try to retransmit the response
    // on the next connection event.
    if (HCI_EXT_ConnEventNoticeCmd(pMsg->connHandle, selfEntity,
                                   MR_CONN_EVT_END_EVT) == SUCCESS)
    {
      // First free any pending response
      multi_role_freeAttRsp(FAILURE);

      // Hold on to the response message for retransmission
      pAttRsp = pMsg;

      // Don't free the response message yet
      return (FALSE);
    }
  }

  if (pMsg->method == ATT_MTU_UPDATED_EVENT)
  {
      // MTU size updated
      uint8 currentMTUSize = 0;
      currentMTUSize = pMsg->msg.mtuEvt.MTU;
      SDITask_setAppDataSize(currentMTUSize);
      SDITask_PrintfToUART("%s%d\r\n", "MTU Size: " ,currentMTUSize);
  }

  // Messages from GATT server
  if (linkDB_NumActive() > 0)
  {
    // Find index from connection handle
    connIndex = multi_role_mapConnHandleToIndex(pMsg->connHandle);
    if ((discInfo[connIndex].discState != BLE_DISC_STATE_IDLE)&&(multiConnRole == GAP_PROFILE_CENTRAL))
    {
      multi_role_processGATTDiscEvent(pMsg);
    }
  }


  if(pMsg->method == ATT_HANDLE_VALUE_NOTI)
  {
    //Send received bytes to serial port
    SDITask_sendToUART(pMsg->msg.handleValueNoti.pValue, pMsg->msg.handleValueNoti.len);
  }

  // Free message payload. Needed only for ATT Protocol messages
  GATT_bm_free(&pMsg->msg, pMsg->method);

  // It's safe to free the incoming message
  return (TRUE);
}

/*********************************************************************
* @fn      multi_role_sendAttRsp
*
* @brief   Send a pending ATT response message.
*
* @param   none
*
* @return  none
*/
static void multi_role_sendAttRsp(void)
{
  // See if there's a pending ATT Response to be transmitted
  if (pAttRsp != NULL)
  {
    uint8_t status;

    // Increment retransmission count
    rspTxRetry++;

    // Try to retransmit ATT response till either we're successful or
    // the ATT Client times out (after 30s) and drops the connection.
    status = GATT_SendRsp(pAttRsp->connHandle, pAttRsp->method, &(pAttRsp->msg));
    if ((status != blePending) && (status != MSG_BUFFER_NOT_AVAIL))
    {
      // Disable connection event end notice
      HCI_EXT_ConnEventNoticeCmd(pAttRsp->connHandle, selfEntity, 0);

      // We're done with the response message
      multi_role_freeAttRsp(status);
    }
    else
    {
      // Continue retrying
      SDITask_PrintfToUART("Rsp send retry:%d", rspTxRetry);
    }
  }
}

/*********************************************************************
* @fn      multi_role_freeAttRsp
*
* @brief   Free ATT response message.
*
* @param   status - response transmit status
*
* @return  none
*/
static void multi_role_freeAttRsp(uint8_t status)
{
  // See if there's a pending ATT response message
  if (pAttRsp != NULL)
  {
    // See if the response was sent out successfully
    if (status == SUCCESS)
    {
      SDITask_PrintfToUART("Rsp sent, retry: %d", rspTxRetry);
    }
    else
    {
      // Free response payload
      GATT_bm_free(&pAttRsp->msg, pAttRsp->method);

      SDITask_PrintfToUART("Rsp retry failed: %d", rspTxRetry);
    }

    // Free response message
    ICall_freeMsg(pAttRsp);

    // Reset our globals
    pAttRsp = NULL;
    rspTxRetry = 0;
  }
}

/*********************************************************************
* @fn      multi_role_processAppMsg
*
* @brief   Process an incoming callback from a profile.
*
* @param   pMsg - message to process
*
* @return  None.
*/
static void multi_role_processAppMsg(mrEvt_t *pMsg)
{
  switch (pMsg->event)
  {
  case MR_STATE_CHANGE_EVT:
    multi_role_processStackMsg((ICall_Hdr *)pMsg->pData);
    // Free the stack message
    ICall_freeMsg(pMsg->pData);
    break;

  case MR_CHAR_CHANGE_EVT:
    multi_role_processCharValueChangeEvt(*(pMsg->pData));
    // Free the app data
    ICall_free(pMsg->pData);
    break;

  case MR_KEY_CHANGE_EVT:
    multi_role_handleKeys(*(pMsg->pData));
    // Free the app data
    ICall_free(pMsg->pData);
    break;

  default:
    // Do nothing.
    break;
  }
}

/*********************************************************************
* @fn      multi_role_eventCB
*
* @brief   Multi GAPRole event callback function.
*
* @param   pEvent - pointer to event structure
*
* @return  TRUE if safe to deallocate event message, FALSE otherwise.
*/
static uint8_t multi_role_eventCB(gapMultiRoleEvent_t *pEvent)
{
  // Forward the role event to the application
  if (multi_role_enqueueMsg(MR_STATE_CHANGE_EVT, (uint8_t *)pEvent))
  {
    // App will process and free the event
    return FALSE;
  }

  // Caller should free the event
  return TRUE;
}

/*********************************************************************
* @fn      multi_role_processRoleEvent
*
* @brief   Multi role event processing function.
*
* @param   pEvent - pointer to event structure
*
* @return  none
*/
static void multi_role_processRoleEvent(gapMultiRoleEvent_t *pEvent)
{
  switch (pEvent->gap.opcode)
  {
    // GAPRole started
    case GAP_DEVICE_INIT_DONE_EVENT:
    {
      // Store max pdu size
      maxPduSize = pEvent->initDone.dataPktLen;

      SDITask_PrintfToUART("%s\r\n",Util_convertBdAddr2Str(pEvent->initDone.devAddr));
      SDITask_PrintfToUART("%s\r\n","Connected to 0");
      SDITask_PrintfToUART("%s\r\n","Initialized");

      // Set device info characteristic
      DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN, pEvent->initDone.devAddr);
    }
    break;

    // Advertising started
    case GAP_MAKE_DISCOVERABLE_DONE_EVENT:
    {
      SDITask_PrintfToUART("%s\r\n","Advertising");
    }
    break;

    // Advertising ended
    case GAP_END_DISCOVERABLE_DONE_EVENT:
    {
      // Display advertising info depending on whether there are any connections
      if (linkDB_NumActive() < maxNumBleConns)
      {
        SDITask_PrintfToUART("%s\r\n","Ready to Advertise");
      }
      else
      {
        SDITask_PrintfToUART("%s\r\n","Can't Adv : Max conns reached");
      }
    }
    break;

    // A discovered device report
    case GAP_DEVICE_INFO_EVENT:
    {
      // Do nothing. scan results are handled at the end of scanning
    }
    break;

    // End of discovery report
    case GAP_DEVICE_DISCOVERY_EVENT:
    {
      uint8_t i;

      // If devices were found
      if (pEvent->discCmpl.numDevs > 0)
      {
        // Loop through discovered devices to store in static device list
        for (i = 0; i < pEvent->discCmpl.numDevs; i++)
        {
          // Store address type
          devList[i].addrType = pEvent->discCmpl.pDevList[i].addrType;

          // Store event type (adv / scan response)
          devList[i].eventType = pEvent->discCmpl.pDevList[i].eventType;

          // Store address
          memcpy(devList[i].addr, pEvent->discCmpl.pDevList[i].addr, B_ADDR_LEN);

          // Convert address to string
          uint8_t *pAddr = (uint8_t*)Util_convertBdAddr2Str(devList[i].addr);

          // Copy converted string to static device list
          memcpy(devList[i].strAddr, pAddr, B_STR_ADDR_LEN);
        }
      }

      SDITask_PrintfToUART("Devices Found %d", pEvent->discCmpl.numDevs);
    }
    break;

    // Connection has been established
    case GAP_LINK_ESTABLISHED_EVENT:
    {
      // If succesfully established
      if (pEvent->gap.hdr.status == SUCCESS)
      {
        SDITask_PrintfToUART("%s\r\n","Connected!");
        SDITask_PrintfToUART("Connected to %d", linkDB_NumActive());

        multiConnRole = pEvent->linkCmpl.connRole;
        
        // Add index-to-connHandle mapping entry and update menus
        uint8_t index = multi_role_addMappingEntry(pEvent->linkCmpl.connectionHandle, pEvent->linkCmpl.devAddr);

        //turn off advertising if no available links
        if (linkDB_NumActive() >= maxNumBleConns)
        {
          uint8_t advertEnabled = FALSE;
          GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &advertEnabled, NULL);
          SDITask_PrintfToUART("%s\r\n","Can't adv: no links");
        }

        // Print last connected device
        SDITask_PrintfToUART("%s\r\n",(char*)connHandleMap[index].strAddr);

        // Start service discovery
        multi_role_startDiscovery(pEvent->linkCmpl.connectionHandle);

        // Start periodic clock if this is the first connection
        if (linkDB_NumActive() == 1)
        {
          
        }
      }
      // If the connection was not successfully established
      else
      {
        SDITask_PrintfToUART("%s\r\n","Connect Failed");
        SDITask_PrintfToUART("Reason: %d", pEvent->gap.hdr.status);
      }
    }
    break;

    // Connection has been terminated
    case GAP_LINK_TERMINATED_EVENT:
    {
      // read current num active so that this doesn't change before this event is processed
      uint8_t currentNumActive = linkDB_NumActive();

      // Find index from connection handle
      connIndex = multi_role_mapConnHandleToIndex(pEvent->linkTerminate.connectionHandle);

      // Check to prevent buffer overrun
      if (connIndex < maxNumBleConns)
      {
        // Clear screen, reset discovery info, and return to main menu
        connHandleMap[connIndex].connHandle = INVALID_CONNHANDLE;

        // Reset discovery info
        discInfo[connIndex].discState = BLE_DISC_STATE_IDLE;
        discInfo[connIndex].charHdl = 0;

        // If there aren't any active connections
        if (currentNumActive == 0)
        {
          // Stop periodic clock
          
        }

        // Clear screen
        SDITask_PrintfToUART("Connected to %d", linkDB_NumActive());
        SDITask_PrintfToUART("%s\r\n","Disconnected!");

        // If it is possible to advertise again
        if (currentNumActive == (maxNumBleConns-1))
        {
          SDITask_PrintfToUART("%s\r\n","Ready to Advertise");
          SDITask_PrintfToUART("%s\r\n","Ready to Scan");
        }
      }
    }
    break;

    // A parameter update has occurred
    case GAP_LINK_PARAM_UPDATE_EVENT:
    {
      SDITask_PrintfToUART("Param Update %d", pEvent->linkUpdate.status);
    }
    break;

  default:
    break;
  }
}

/*********************************************************************
* @fn      multi_role_charValueChangeCB
*
* @brief   Callback from Simple Profile indicating a characteristic
*          value change.
*
* @param   paramID - parameter ID of the value that was changed.
*
* @return  None.
*/
static void multi_role_charValueChangeCB(uint8_t paramID)
{
  uint8_t *pData;

  // Allocate space for the event data.
  if ((pData = ICall_malloc(sizeof(uint8_t))))
  {
    *pData = paramID;

    // Queue the event.
    multi_role_enqueueMsg(MR_CHAR_CHANGE_EVT, pData);
  }
}

/*********************************************************************
* @fn      multi_role_processCharValueChangeEvt
*
* @brief   Process a pending Simple Profile characteristic value change
*          event.
*
* @param   paramID - parameter ID of the value that was changed.
*
* @return  None.
*/
static void multi_role_processCharValueChangeEvt(uint8_t paramID)
{

}

/*********************************************************************
* @fn      multi_role_enqueueMsg
*
* @brief   Creates a message and puts the message in RTOS queue.
*
* @param   event - message event.
* @param   pData - pointer to data to be queued
*
* @return  None.
*/
static uint8_t multi_role_enqueueMsg(uint16_t event, uint8_t *pData)
{
  // Allocate space for the message
  mrEvt_t *pMsg = ICall_malloc(sizeof(mrEvt_t));

  // If sucessfully allocated
  if (pMsg)
  {
    // Fill up message
    pMsg->event = event;
    pMsg->pData = pData;

    // Enqueue the message.
    return Util_enqueueMsg(appMsgQueue, syncEvent, (uint8_t *)pMsg);
  }

  return FALSE;
}

/*********************************************************************
* @fn      multi_role_keyChangeHandler
*
* @brief   Key event handler function
*
* @param   a0 - ignored
*
* @return  none
*/
void multi_role_keyChangeHandler(uint8_t keys)
{
  uint8_t *pData;

  // Allocate space for the event data.
  if ((pData = ICall_malloc(sizeof(uint8_t))))
  {
    // Store the key data
    *pData = keys;

    // Queue the event.
    multi_role_enqueueMsg(MR_KEY_CHANGE_EVT, pData);
  }
}

/*********************************************************************
* @fn      multi_role_handleKeys
*
* @brief   Handles all key events for this device.
*
* @param   keys - bit field for key events. Valid entries:
*                 HAL_KEY_SW_2
*                 HAL_KEY_SW_1
*
* @return  none
*/
static void multi_role_handleKeys(uint8_t keys)
{
  if (keys & KEY_BTN1)
  {
    // Check if the key is still pressed
    if (PIN_getInputValue(Board_BUTTON0) == 0)
    {
      multi_role_connect();
    }
  }
  else if (keys & KEY_BTN2)
  {
    // Check if the key is still pressed
    if (PIN_getInputValue(Board_BUTTON1) == 0)
    {
      
    }
  }
}

/*********************************************************************
* @fn      multi_role_startDiscovery
*
* @brief   Start service discovery.
*
* @param   connHandle - connection handle
*
* @return  none
*/
static void multi_role_startDiscovery(uint16_t connHandle)
{
  // Exchange MTU request
  attExchangeMTUReq_t req;

  // Map connection handle to index
  connIndex = multi_role_mapConnHandleToIndex(connHandle);

  // Check to prevent buffer overrun
  if (connIndex < maxNumBleConns)
  {
    // Update discovery state of this connection
    discInfo[connIndex].discState= BLE_DISC_STATE_MTU;

    // Initialize cached handles
    discInfo[connIndex].svcStartHdl = discInfo[connIndex].svcEndHdl = 0;
  }

  // Discover GATT Server's Rx MTU size
  req.clientRxMTU = maxPduSize - L2CAP_HDR_SIZE;

  // ATT MTU size should be set to the minimum of the Client Rx MTU
  // and Server Rx MTU values
  VOID GATT_ExchangeMTU(connHandle, &req, selfEntity);
}

/*********************************************************************
* @fn      multi_role_processGATTDiscEvent
*
* @brief   Process GATT discovery event
*
* @param   pMsg - pointer to discovery event stack message
*
* @return  none
*/
static void multi_role_processGATTDiscEvent(gattMsgEvent_t *pMsg)
{
  // Map connection handle to index
  connIndex = multi_role_mapConnHandleToIndex(pMsg->connHandle);
  // Check to prevent buffer overrun
  if (connIndex < maxNumBleConns)
  { 
    // If we've updated the MTU size
    if (discInfo[connIndex].discState == BLE_DISC_STATE_MTU)
    {
      // MTU size response received, discover simple service
      if (pMsg->method == ATT_EXCHANGE_MTU_RSP)
      {
        uint8_t uuid[ATT_UUID_SIZE] = { TI_BASE_UUID_128(SERIALPORTSERVICE_SERV_UUID) };

        // Advance state
        discInfo[connIndex].discState= BLE_DISC_STATE_SVC;
        
        SDITask_PrintfToUART("%s\r\n", "Discovering services...");
        
        // Discovery of simple service
        VOID GATT_DiscPrimaryServiceByUUID(pMsg->connHandle, uuid, ATT_UUID_SIZE,
                                           selfEntity);
      }
    }
    // If we're performing service discovery
    else if (discInfo[connIndex].discState == BLE_DISC_STATE_SVC)
    {
      // Service found, store handles
      if (pMsg->method == ATT_FIND_BY_TYPE_VALUE_RSP &&
          pMsg->msg.findByTypeValueRsp.numInfo > 0)
      {
        discInfo[connIndex].svcStartHdl = ATT_ATTR_HANDLE(pMsg->msg.findByTypeValueRsp.pHandlesInfo, 0);
        discInfo[connIndex].svcEndHdl = ATT_GRP_END_HANDLE(pMsg->msg.findByTypeValueRsp.pHandlesInfo, 0);
        SDITask_PrintfToUART("%s\r\n", "Found Serial Port Service...");
      }

      // If procedure is complete
      if (((pMsg->method == ATT_FIND_BY_TYPE_VALUE_RSP) &&
           (pMsg->hdr.status == bleProcedureComplete))  ||
          (pMsg->method == ATT_ERROR_RSP))
      {
        // If we've discovered the service
        if (discInfo[connIndex].svcStartHdl != 0)
        {
          attReadByTypeReq_t req;
          uint8_t uuid[ATT_UUID_SIZE] = { TI_BASE_UUID_128(SERIALPORTSERVICE_DATA_UUID) };

          
          // Discover characteristic
          discInfo[connIndex].discState = BLE_DISC_STATE_CHAR;
          req.startHandle = discInfo[connIndex].svcStartHdl;
          req.endHandle = discInfo[connIndex].svcEndHdl;
          req.type.len = ATT_UUID_SIZE;
          memcpy(req.type.uuid,  uuid, ATT_UUID_SIZE);

          // Discover characteristic descriptors
          GATT_DiscAllCharDescs(connIndex,
                                discInfo[connIndex].svcStartHdl + 1,
                                discInfo[connIndex].svcEndHdl,
                                selfEntity);}
      }
    }
    // If we're discovering characteristics
    else if (discInfo[connIndex].discState == BLE_DISC_STATE_CHAR)
    {
      // Characteristic descriptors found
      if (pMsg->method == ATT_FIND_INFO_RSP &&
          pMsg->msg.findInfoRsp.numInfo > 0)
      {
        uint8_t i;

        // For each handle/uuid pair
        for (i = 0; i < pMsg->msg.findInfoRsp.numInfo; i++)
        {
          if(pMsg->msg.findInfoRsp.format == ATT_HANDLE_BT_UUID_TYPE)
          {
            // Look for CCCD
            if (ATT_BT_PAIR_UUID(pMsg->msg.findInfoRsp.pInfo, i) ==
                GATT_CLIENT_CHAR_CFG_UUID)
            {
              // CCCD found
              SDITask_PrintfToUART("%s\r\n", "CCCD for Data Char Found...");
              charCCCDHdl = ATT_PAIR_HANDLE(pMsg->msg.findInfoRsp.pInfo, i);
              break;
            }
          }
          else if(pMsg->msg.findInfoRsp.format == ATT_HANDLE_UUID_TYPE)
          {
            // Look for Serial Data Char.
            if (memcmp(&(pMsg->msg.findInfoRsp.pInfo[ATT_PAIR_UUID_IDX(i)]), uuidDataChar, ATT_UUID_SIZE) == 0)
            {
              // CCCD found
              SDITask_PrintfToUART("%s\r\n", "Data Char Found...");
              charDataHdl = ATT_PAIR_HANDLE(pMsg->msg.findInfoRsp.pInfo, i);
              break;
            }
          }
        }
      }
      
      // If procedure complete
      if ((pMsg->method == ATT_FIND_INFO_RSP  &&
           pMsg->hdr.status == bleProcedureComplete) ||
          (pMsg->method == ATT_ERROR_RSP))
      {

        //Enable notification on peripheral(after a few seconds delay, let it finish connection/discovery process)
        {
          Util_startClock(&startNotiEnableClock);
        }
        
        discInfo[connIndex].discState = BLE_DISC_STATE_IDLE;
      }  
    }
  }
 
}

/*********************************************************************
* @fn      multi_role_mapConnHandleToIndex
*
* @brief   Translates connection handle to index
*
* @param   connHandle - the connection handle
*
* @return  index or INVALID_CONNHANDLE if connHandle isn't found
*/
static uint16_t multi_role_mapConnHandleToIndex(uint16_t connHandle)
{
  uint16_t index;
  // Loop through connection
  for (index = 0; index < maxNumBleConns; index ++)
  {
    // If matching connection handle found
    if (connHandleMap[index].connHandle == connHandle)
    {
      return index;
    }
  }
  // Not found if we got here
  return INVALID_CONNHANDLE;
}


/*********************************************************************
* @fn      multi_role_addMappingEntry
*
* @brief   add a new connection to the index-to-connHandle map
*
* @param   connHandle - the connection handle
*
* @param   addr - pointer to device address
*
* @return  index of connection handle
*/
static uint8_t multi_role_addMappingEntry(uint16_t connHandle, uint8_t *addr)
{
  uint16_t index;
  // Loop though connections
  for (index = 0; index < maxNumBleConns; index++)
  {
    // If there is an open connection
    if (connHandleMap[index].connHandle == INVALID_CONNHANDLE)
    {
      // Store mapping
      connHandleMap[index].connHandle = connHandle;

      // Convert address to string
      uint8_t *pAddr = (uint8_t *) Util_convertBdAddr2Str(addr);

      // Copy converted string to persistent connection handle list
      memcpy(connHandleMap[index].strAddr, pAddr, B_STR_ADDR_LEN);

      return index;
    }
  }
  // No room if we get here
  return bleNoResources;
}

/*********************************************************************
 * @fn      multi_role_enqueueUARTMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event  - message event.
 * @param   status - message status.
 *
 * @return  None.
 */
void multi_role_enqueueUARTMsg(uint8_t event, uint8_t *data, uint8_t len)
{
  sbpUARTEvt_t *pMsg;
  queueRec_t *pRec;

  //Enqueue message only in a connected state
  if(1)
  {
    // Create dynamic pointer to message.
    if (pMsg = ICall_malloc(sizeof(sbpUARTEvt_t)))
    {

      pMsg->event = event;
      pMsg->pData = (uint8 *)ICall_allocMsg(len);
      if(pMsg->pData)
      {
        //payload
        memcpy(pMsg->pData , data, len);
      }
      pMsg->length = len;

      // Enqueue the message.
      if ((pRec = ICall_malloc(sizeof(queueRec_t))))
      {
        pRec->pData = (uint8*)pMsg;
        // This is an atomic operation
        Queue_put(appUARTMsgQueue, &pRec->_elem);

        Event_post(syncEvent, MR_UART_QUEUE_EVT);
      }else
      {
        SDITask_PrintfToUART("%s\r\n", "appUARTMsgQueue ERROR");
        ICall_free(pMsg);
      }
    }
  }
}

/*********************************************************************
 * @fn      SPPBLEClient_Connect
 *
 * @brief   Client automatically connects to hardcoded address
 *
 * @return  none
 */
void multi_role_connect(void)
{
    uint8_t addrType;
    uint8_t peerAddr[6];

    // connect to hardcoded device address i.e. 0x050403020100
    int x = 0;
    for(x = 0; x<6; x++)
    {
        peerAddr[x] = x;
    }

    addrType = ADDRTYPE_PUBLIC;

    SDITask_PrintfToUART("%s\r\n", "Auto connecting...");
    GAPRole_EstablishLink(DEFAULT_LINK_HIGH_DUTY_CYCLE,
                                 DEFAULT_LINK_WHITE_LIST,
                                 addrType, peerAddr);
}

/*********************************************************************
 * @fn      SPPBLEClient_genericHandler
 *
 * @brief   Handler function for clock timeouts.
 *
 * @param   arg - event type
 *
 * @return  None.
 */
static void multi_role_genericHandler(UArg arg)
{
  // Wake up the application.
  Event_post(syncEvent, arg);
}
/*********************************************************************
*********************************************************************/
