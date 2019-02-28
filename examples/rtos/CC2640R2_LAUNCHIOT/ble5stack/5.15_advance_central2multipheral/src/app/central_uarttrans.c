/******************************************************************************

 @file  spp_ble_client.c

 @brief This file contains the SPP BLE Client sample application for use
        with the SimpleLink CC26xx SDK

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

#include "bcomdef.h"

#include <icall.h>
#include "util.h"
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"

#include "central.h"

#include "iotboard_key.h"
#include "board.h"

#include "central_uarttrans.h"

#include "ble_user_config.h"
#include "uarttrans_service.h"
#include "inc/sdi_task.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

#define SBC_STATE_CHANGE_EVT                  0x0001
#define SBC_KEY_CHANGE_EVT                    0x0002
#define SBC_RSSI_READ_EVT                     0x0004
#define SBC_PAIRING_STATE_EVT                 0x0008
#define SBC_PASSCODE_NEEDED_EVT               0x0010

// Simple BLE Central Task Events
#define SBC_ICALL_EVT                         ICALL_MSG_EVENT_ID // Event_Id_31
#define SBC_QUEUE_EVT                         UTIL_QUEUE_EVENT_ID // Event_Id_30
#define SBC_START_DISCOVERY_EVT               Event_Id_00
#define SBC_AUTO_SCAN_EVT                     Event_Id_01
#define SBC_NOTI_ENABLE_EVT                   Event_Id_02
#define SBC_AUTO_CONN_EVT                     Event_Id_03

#define SBC_ALL_EVENTS                        (SBC_ICALL_EVT           | \
                                               SBC_QUEUE_EVT           | \
                                               SBC_START_DISCOVERY_EVT | \
                                               SBC_NOTI_ENABLE_EVT     | \
                                               SBC_AUTO_SCAN_EVT       | \
                                               SBC_AUTO_CONN_EVT)

// Maximum number of scan responses
#define DEFAULT_MAX_SCAN_RES                  8

// Scan duration in ms
#define DEFAULT_SCAN_DURATION                 4000

// Discovery mode (limited, general, all)
#define DEFAULT_DISCOVERY_MODE                DEVDISC_MODE_ALL

// TRUE to use active scan
#define DEFAULT_DISCOVERY_ACTIVE_SCAN         TRUE

// TRUE to use white list during discovery
#define DEFAULT_DISCOVERY_WHITE_LIST          FALSE

// TRUE to use high scan duty cycle when creating link
#define DEFAULT_LINK_HIGH_DUTY_CYCLE          FALSE

// TRUE to use white list when creating link
#define DEFAULT_LINK_WHITE_LIST               FALSE

// Default RSSI polling period in ms
#define DEFAULT_RSSI_PERIOD                   1000

// Whether to enable automatic parameter update request when a connection is
// formed
#define DEFAULT_ENABLE_UPDATE_REQUEST         GAPCENTRALROLE_PARAM_UPDATE_REQ_AUTO_ACCEPT

// Minimum connection interval (units of 1.25ms) if automatic parameter update
// request is enabled
#define DEFAULT_UPDATE_MIN_CONN_INTERVAL      16

// Maximum connection interval (units of 1.25ms) if automatic parameter update
// request is enabled
#define DEFAULT_UPDATE_MAX_CONN_INTERVAL      16

// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_UPDATE_SLAVE_LATENCY          0

// Supervision timeout value (units of 10ms) if automatic parameter update
// request is enabled
#define DEFAULT_UPDATE_CONN_TIMEOUT           100

// Default service discovery timer delay in ms
#define DEFAULT_SVC_DISCOVERY_DELAY           1000

// Default notification enable timer delay in ms
#define DEFAULT_NOTI_ENABLE_DELAY             200

// TRUE to filter discovery results on desired service UUID
#define DEFAULT_DEV_DISC_BY_SVC_UUID          TRUE

// TRUE to connect automatically to server to preset address
#ifndef CLIENT_AUTO_SCAN
#define CLIENT_AUTO_SCAN                      TRUE
#endif

// Task configuration
#define SBC_TASK_PRIORITY                     1

#ifndef SBC_TASK_STACK_SIZE
#define SBC_TASK_STACK_SIZE                   864
#endif

// Application states
enum
{
  BLE_STATE_IDLE,
  BLE_STATE_CONNECTING,
  BLE_STATE_CONNECTED,
  BLE_STATE_DISCONNECTING
};

// Discovery states
enum
{
  BLE_DISC_STATE_IDLE,                // Idle
  BLE_DISC_STATE_MTU,                 // Exchange ATT MTU size
  BLE_DISC_STATE_SVC,                 // Service discovery
  BLE_DISC_STATE_CHAR                 // Characteristic discovery
};

/*********************************************************************
 * TYPEDEFS
 */

// RTOS queue for profile/app messages.
typedef struct _queueRec_
{
  Queue_Elem _elem;          // queue element
  uint8_t *pData;            // pointer to app data
} queueRec_t;

// App event passed from profiles.
typedef struct
{
  appEvtHdr_t hdr; // event header
  uint8_t *pData;  // event data
} sbcEvt_t;

// App event passed from profiles.
typedef struct
{
  uint8_t *pData;  // New data
  uint8_t length; // New status
} sbcUARTEvt_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
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
static Clock_Struct startDiscClock;

// Clock object used to signal timeout
static Clock_Struct startNotiEnableClock;

// Queue object used for app messages
static Queue_Struct appMsg;
static Queue_Handle appMsgQueue;

// Queue object used for UART messages
static Queue_Struct appUARTMsg;
static Queue_Handle appUARTMsgQueue;

// Task configuration
Task_Struct sbcTask;
Char sbcTaskStack[SBC_TASK_STACK_SIZE];

// GAP GATT Attributes
static const uint8_t attDeviceName[GAP_DEVICE_NAME_LEN] = "SPP BLE Client";

// Number of scan results and scan result index
static uint8_t scanRes = 0;

// Scan result list
static gapDevRec_t devList[DEFAULT_MAX_SCAN_RES];

// Scanning state
static bool scanningStarted = FALSE;

// Connection handle of current connection
static uint16_t connHandle = GAP_CONNHANDLE_INIT;

// Application state
static uint8_t state = BLE_STATE_IDLE;

// Discovery state
static uint8_t discState = BLE_DISC_STATE_IDLE;

// Discovered service start and end handle
static uint16_t svcStartHdl = 0;
static uint16_t svcEndHdl = 0;

// Discovered characteristic handle
static uint16_t charDataHdl = 0;

// Discovered characteristic CCCD handle
static uint16_t charCCCDHdl = 0;

//UUID of Serial Port Data Characteristic
static uint8_t uuidDataChar[ATT_UUID_SIZE] = { TI_BASE_UUID_128(SERIALPORTSERVICE_DATA_UUID) };

// Maximum PDU size (default = 27 octets)
static uint16 maxPduSize;

// Maximum MTU size (default = 23 octets)
static uint16 currentMTUSize;

// 主机连接的从设备列表
typedef struct
{
  uint8_t scanDevIndex;
  uint8_t connDevIndex;
  uint8_t connDevHandle;
  uint8_t connDevMac[B_ADDR_LEN];
}connDevList_t;


connDevList_t connDevList[MAX_NUM_BLE_CONNS];
/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void SPPBLEClient_init(void);
static void SPPBLEClient_taskFxn(UArg a0, UArg a1);

static void SPPBLEClient_processGATTMsg(gattMsgEvent_t *pMsg);
static void SPPBLEClient_handleKeys(uint8_t shift, uint8_t keys);
static void SPPBLEClient_processStackMsg(ICall_Hdr *pMsg);
static void SPPBLEClient_processAppMsg(sbcEvt_t *pMsg);
static void SPPBLEClient_processRoleEvent(gapCentralRoleEvent_t *pEvent);
static void SPPBLEClient_processGATTDiscEvent(gattMsgEvent_t *pMsg);
static void SPPBLEClient_startDiscovery(void);
static bool SPPBLEClient_findDevName(uint8_t *pData);
static void SPPBLEClient_addDeviceInfo(uint8_t *pAddr, uint8_t addrType);

static uint8_t SPPBLEClient_eventCB(gapCentralRoleEvent_t *pEvent);

void SPPBLEClient_startDiscHandler(UArg a0);
void SPPBLEClient_keyChangeHandler(uint8 keys);
void SPPBLEClient_readRssiHandler(UArg a0);

static uint8_t SPPBLEClient_enqueueMsg(uint8_t event, uint8_t status,
                                           uint8_t *pData);

void SPPBLEClient_enqueueUARTMsg(uint8_t event, uint8_t *data, uint8_t len);
static void SPPBLEClient_genericHandler(UArg arg);
static void SPPBLEClient_autoScan(void);
char* convInt32ToText(int32 value);
bool SPPBLEClient_doSetPhy(uint8 index);

void SPPBLEClient_autoConnect(void);
/*********************************************************************
 * EXTERN FUNCTIONS
 */
extern void AssertHandler(uint8 assertCause, uint8 assertSubcause);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static gapCentralRoleCB_t SPPBLEClient_roleCB =
{
  SPPBLEClient_eventCB     // Event callback
};

/*********************************************************************
 * @fn      SPPBLEClient_createTask
 *
 * @brief   Task creation function for the Simple BLE Peripheral.
 *
 * @param   none
 *
 * @return  none
 */
void SPPBLEClient_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = sbcTaskStack;
  taskParams.stackSize = SBC_TASK_STACK_SIZE;
  taskParams.priority = SBC_TASK_PRIORITY;

  Task_construct(&sbcTask, SPPBLEClient_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      SPPBLEClient_Init
 *
 * @brief   Initialization function for the Simple BLE Central App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notification).
 *
 * @param   none
 *
 * @return  none
 */
static void SPPBLEClient_init(void)
{
  // ******************************************************************
  // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ******************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
  ICall_registerApp(&selfEntity, &syncEvent);

  // Create an RTOS queue for message from profile to be sent to app.
  appMsgQueue = Util_constructQueue(&appMsg);

  appUARTMsgQueue = Util_constructQueue(&appUARTMsg);

  // Setup discovery delay as a one-shot timer
  Util_constructClock(&startDiscClock, SPPBLEClient_startDiscHandler,
                      DEFAULT_SVC_DISCOVERY_DELAY, 0, false, 0);
  Util_constructClock(&startNotiEnableClock, SPPBLEClient_genericHandler,
                      DEFAULT_NOTI_ENABLE_DELAY, 0, false, SBC_NOTI_ENABLE_EVT);

  Board_initKeys(SPPBLEClient_keyChangeHandler);

  // Setup Central Profile
  {
    uint8_t scanRes = DEFAULT_MAX_SCAN_RES;

    GAPCentralRole_SetParameter(GAPCENTRALROLE_MAX_SCAN_RES, sizeof(uint8_t),
                                &scanRes);
  }

  // Setup GAP
  GAP_SetParamValue(TGAP_GEN_DISC_SCAN, DEFAULT_SCAN_DURATION);
  GAP_SetParamValue(TGAP_LIM_DISC_SCAN, DEFAULT_SCAN_DURATION);
  GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN,
                   (void *)attDeviceName);

  //Set CI to 20ms
  GAP_SetParamValue(TGAP_CONN_EST_INT_MIN, 16); //16 - 20ms
  GAP_SetParamValue(TGAP_CONN_EST_INT_MAX, 16);

  // Initialize GATT Client
  VOID GATT_InitClient();

  // Register to receive incoming ATT Indications/Notifications
  GATT_RegisterForInd(selfEntity);

  // Initialize GATT attributes
  GGS_AddService(GATT_ALL_SERVICES);         // GAP
  GATTServApp_AddService(GATT_ALL_SERVICES); // GATT attributes

  // Start the Device
  VOID GAPCentralRole_StartDevice(&SPPBLEClient_roleCB);

  //Register to receive UART messages
  SDITask_registerIncomingRXEventAppCB(SPPBLEClient_enqueueUARTMsg);

  // Register with GAP for HCI/Host messages
  GAP_RegisterForMsgs(selfEntity);

  // Register for GATT local events and ATT Responses pending for transmission
  GATT_RegisterForMsgs(selfEntity);

  //Set default values for Data Length Extension
  //This should be included only if Extended Data Length Feature is enabled
  //in build_config.opt in stack project.
  {
    //Set initial values to maximum, RX is set to max. by default(251 octets, 2120us)
    #define APP_SUGGESTED_PDU_SIZE 251 //default is 27 octets(TX)
    #define APP_SUGGESTED_TX_TIME 2120 //default is 328us(TX)

    //This API is documented in hci.h
    //See BLE5-Stack User's Guide for information on using this command:
    //http://software-dl.ti.com/lprf/ble5stack-docs-latest/html/ble-stack/data-length-extensions.html
    HCI_LE_WriteSuggestedDefaultDataLenCmd(APP_SUGGESTED_PDU_SIZE, APP_SUGGESTED_TX_TIME);
  }

  //Display project name and Bluetooth 5 support
  SDITask_PrintfToUART("%s\r\n", "Hello from SPP BLE Client!");
}

/*********************************************************************
 * @fn      SPPBLEClient_taskFxn
 *
 * @brief   Application task entry point for the Simple BLE Central.
 *
 * @param   none
 *
 * @return  events not processed
 */
static void SPPBLEClient_taskFxn(UArg a0, UArg a1)
{
  // Initialize application
  SPPBLEClient_init();

  // Application main loop
  for (;;)
  {
    uint32_t events;

    events = Event_pend(syncEvent, Event_Id_NONE, SBC_ALL_EVENTS,
                        ICALL_TIMEOUT_FOREVER);

    if (events)
    {
      ICall_EntityID dest;
      ICall_ServiceEnum src;
      ICall_HciExtEvt *pMsg = NULL;

      if (ICall_fetchServiceMsg(&src, &dest,
                                (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
      {
        if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))
        {
          // Process inter-task message
          SPPBLEClient_processStackMsg((ICall_Hdr *)pMsg);
        }

        if (pMsg)
        {
          ICall_freeMsg(pMsg);
        }
      }

      // If RTOS queue is not empty, process app message
      if (events & SBC_QUEUE_EVT)
      {
        // If RTOS queue is not empty, process app UART message.
        if (!Queue_empty(appUARTMsgQueue))
        {
          //Get the message at the front of the queue but still keep it in the queue
          queueRec_t *pRec = Queue_head(appUARTMsgQueue);
          sbcUARTEvt_t *pMsg = (sbcUARTEvt_t *)pRec->pData;

          if (pMsg && (state == BLE_STATE_CONNECTED))
          {
            // Process message.
            bStatus_t retVal = FAILURE;

            // Do a write
            attWriteReq_t req;

            for(int x=0; x<connDevList->connDevIndex; x++)
            {
              //Allocate data bytes to send over the air
              req.pValue = GATT_bm_alloc(connDevList->connDevHandle, ATT_WRITE_REQ, pMsg->length, NULL);

              if ( (req.pValue != NULL) && charDataHdl)
              {
                req.handle = charDataHdl; //handle for Value of Data characteristic found during service discovery
                req.len = pMsg->length;
                memcpy(req.pValue, pMsg->pData, pMsg->length);
                req.sig = FALSE;
                req.cmd = TRUE;

                retVal = GATT_WriteNoRsp(connDevList->connDevHandle, &req);

                if ( retVal != SUCCESS )
                {
                  GATT_bm_free((gattMsg_t *)&req, ATT_WRITE_REQ);
                  SDITask_PrintfToUART("%s%d\r\n", "FAIL FROM CLIENT: ", retVal);
                }else
                {

                }
              }else
              {
                SDITask_PrintfToUART("%s\r\n", "Allocation ERROR!");
              }
            }
            //Remove from the queue
            Util_dequeueMsg(appUARTMsgQueue);

            //Deallocate data payload being transmitted.
            ICall_freeMsg(pMsg->pData);
            // Free the space from the message.
            ICall_free(pMsg);

            if(!Queue_empty(appUARTMsgQueue))
            {
              // Wake up the application to flush out any remaining UART data in the queue.
              Event_post(syncEvent, SBC_QUEUE_EVT);
            }
            
          }

        }

        while (!Queue_empty(appMsgQueue))
        {
          sbcEvt_t *pMsg = (sbcEvt_t *)Util_dequeueMsg(appMsgQueue);
          if (pMsg)
          {
            // Process message
            SPPBLEClient_processAppMsg(pMsg);

            // Free the space from the message
            ICall_free(pMsg);
          }
        }
      }

      if (events & SBC_NOTI_ENABLE_EVT)
      {
        // Process message.
        uint8 retVal;
        attWriteReq_t req;
        uint8 configData[2] = {0x01,0x00};

        req.pValue = GATT_bm_alloc(connHandle, ATT_WRITE_REQ, 2, NULL);

        if ((charCCCDHdl == NULL) && (charDataHdl != NULL)) {charCCCDHdl = charDataHdl + 1;} //Hardcoded
        if ( (req.pValue != NULL) && charCCCDHdl)
        {
          req.handle = charCCCDHdl; //Handle for CCCD of Data characteristic
          req.len = 2;
          memcpy(req.pValue, configData, 2);
          req.cmd = TRUE; //Has to be true for NoRsp from server(command, not request)
          req.sig = FALSE;
          retVal = GATT_WriteNoRsp(connHandle, &req);
          if (retVal != SUCCESS)
          {
            SDITask_PrintfToUART("%s\r\n", "ERROR enabling notification...");
          }
          else
          {
            SDITask_PrintfToUART("%s\r\n", "Notification enabled...");
            
            // 如果还有其他可连接从机，继续启动自动连接功能
            if(connDevList->scanDevIndex > 0)
            {
              Event_post(syncEvent, SBC_AUTO_CONN_EVT);
            }
          }
        }
      }

      if (events & SBC_START_DISCOVERY_EVT)
      {
        if(!scanningStarted)
          SPPBLEClient_startDiscovery();
      }

      if (events & SBC_AUTO_SCAN_EVT)
      {
#if defined (CLIENT_AUTO_SCAN) && (CLIENT_AUTO_SCAN == TRUE)
        SPPBLEClient_autoScan();
#endif
      }
      
      if (events & SBC_AUTO_CONN_EVT)
      {
        SPPBLEClient_autoConnect();
      }
    }
  }
}
/*********************************************************************
 * @fn      SPPBLEClient_processStackMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void SPPBLEClient_processStackMsg(ICall_Hdr *pMsg)
{
  switch (pMsg->event)
  {
    case GAP_MSG_EVENT:
      SPPBLEClient_processRoleEvent((gapCentralRoleEvent_t *)pMsg);
      break;

    case GATT_MSG_EVENT:
      SPPBLEClient_processGATTMsg((gattMsgEvent_t *)pMsg);
      break;

    default:
      break;
  }
}

/*********************************************************************
 * @fn      SPPBLEClient_processAppMsg
 *
 * @brief   Central application event processing function.
 *
 * @param   pMsg - pointer to event structure
 *
 * @return  none
 */
static void SPPBLEClient_processAppMsg(sbcEvt_t *pMsg)
{
  switch (pMsg->hdr.event)
  {
    case SBC_STATE_CHANGE_EVT:
      SPPBLEClient_processStackMsg((ICall_Hdr *)pMsg->pData);

      // Free the stack message
      ICall_freeMsg(pMsg->pData);
      break;

    case SBC_KEY_CHANGE_EVT:
      SPPBLEClient_handleKeys(0, pMsg->hdr.state);
      break;

    default:
      // Do nothing.
      break;
  }
}

/*********************************************************************
 * @fn      SPPBLEClient_processRoleEvent
 *
 * @brief   Central role event processing function.
 *
 * @param   pEvent - pointer to event structure
 *
 * @return  none
 */
static void SPPBLEClient_processRoleEvent(gapCentralRoleEvent_t *pEvent)
{
  switch (pEvent->gap.opcode)
  {
    case GAP_DEVICE_INIT_DONE_EVENT:
      {
        maxPduSize = pEvent->initDone.dataPktLen;

        Display_print0(dispHandle, 1, 0, Util_convertBdAddr2Str(pEvent->initDone.devAddr));
        Display_print0(dispHandle, 2, 0, "Initialized");

        // Prompt user to begin scanning.
        Display_print0(dispHandle, 5, 0, "Discover ->");
#if defined (CLIENT_AUTO_SCAN) && (CLIENT_AUTO_SCAN == TRUE)
        Event_post(syncEvent, SBC_AUTO_SCAN_EVT);
#endif

      }
      break;

    case GAP_DEVICE_INFO_EVENT:
      {
        // if filtering device discovery results based on service UUID
        if (DEFAULT_DEV_DISC_BY_SVC_UUID == TRUE)
        {
          if((pEvent->deviceInfo.eventType == GAP_ADRPT_SCAN_RSP)
              &&(SPPBLEClient_findDevName(pEvent->deviceInfo.pEvtData)))
          {
            SPPBLEClient_addDeviceInfo(pEvent->deviceInfo.addr,
                                           pEvent->deviceInfo.addrType);
            SDITask_PrintfToUART("%02X%02X%02X%02X%02X%02X\r\n",
                                 pEvent->deviceInfo.addr[0],
                                 pEvent->deviceInfo.addr[1],
                                 pEvent->deviceInfo.addr[2],
                                 pEvent->deviceInfo.addr[3],
                                 pEvent->deviceInfo.addr[4],
                                 pEvent->deviceInfo.addr[5]);
          }
        }
      }
      break;

    case GAP_DEVICE_DISCOVERY_EVENT:
      {
        // discovery complete
        scanningStarted = FALSE;

        // if not filtering device discovery results based on service UUID
        if (DEFAULT_DEV_DISC_BY_SVC_UUID == FALSE)
        {
          // Copy results
          scanRes = pEvent->discCmpl.numDevs;
          memcpy(devList, pEvent->discCmpl.pDevList,
                 (sizeof(gapDevRec_t) * scanRes));
        }

        Display_print1(dispHandle, 2, 0, "Devices Found %d", scanRes);

        if (scanRes > 0)
        {
          Display_print0(dispHandle, 3, 0, "<- To Select");
        }

        // Prompt user that re-performing scanning at this state is possible.
        Display_print0(dispHandle, 5, 0, "Discover ->");

        // 启动自动连接功能
        Event_post(syncEvent, SBC_AUTO_CONN_EVT);
      }
      break;

    case GAP_LINK_ESTABLISHED_EVENT:
      {
        if (pEvent->gap.hdr.status == SUCCESS)
        {
          state = BLE_STATE_CONNECTED;
          connHandle = pEvent->linkCmpl.connectionHandle;

          // If service discovery not performed initiate service discovery
          //if (charDataHdl == 0)
          {
            Util_startClock(&startDiscClock);
          }

          SDITask_PrintfToUART("%s\r\n", "Connected!");
          
          Display_print0(dispHandle, 2, 0, "Connected");
          Display_print0(dispHandle, 3, 0, Util_convertBdAddr2Str(pEvent->linkCmpl.devAddr));
          // Display the initial options for a Right key press.
          // SPPBLEClient_handleKeys(0, KEY_LEFT);
          
          connDevList->connDevHandle = connHandle;
          connDevList->scanDevIndex--;
          connDevList->connDevIndex++;
          
          SDITask_PrintfToUART("+CONN = %02X%02X%02X%02X%02X%02X\r\n",
                     pEvent->linkCmpl.devAddr[0],
                     pEvent->linkCmpl.devAddr[1],
                     pEvent->linkCmpl.devAddr[2],
                     pEvent->linkCmpl.devAddr[3],
                     pEvent->linkCmpl.devAddr[4],
                     pEvent->linkCmpl.devAddr[5]);
        }
        else
        {
          state = BLE_STATE_IDLE;
          connHandle = GAP_CONNHANDLE_INIT;
          discState = BLE_DISC_STATE_IDLE;

          Display_print0(dispHandle, 2, 0, "Connect Failed");
          Display_print1(dispHandle, 3, 0, "Reason: %d", pEvent->gap.hdr.status);
        }
      }
      break;

    case GAP_LINK_TERMINATED_EVENT:
      {
        state = BLE_STATE_IDLE;
        connHandle = GAP_CONNHANDLE_INIT;
        discState = BLE_DISC_STATE_IDLE;
        charDataHdl = 0;

        Display_print0(dispHandle, 2, 0, "Disconnected");
        Display_print1(dispHandle, 3, 0, "Reason: %d", pEvent->linkTerminate.reason);
        Display_clearLine(dispHandle, 4);
        Display_clearLine(dispHandle, 6);

        // Prompt user to begin scanning.
        Display_print0(dispHandle, 5, 0, "Discover ->");
      }
      break;

    case GAP_LINK_PARAM_UPDATE_EVENT:
      {
        Display_print1(dispHandle, 2, 0, "Param Update: %d", pEvent->linkUpdate.status);
      }
      break;

    default:
      break;
  }
}

/*********************************************************************
 * @fn      SPPBLEClient_handleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
static void SPPBLEClient_handleKeys(uint8_t shift, uint8_t keys)
{
  (void)shift;  // Intentionally unreferenced parameter

  // Set PHY in a Connection
  if (keys & KEY_BTN1)
  {

    return;
  }

  if (keys & KEY_BTN2)
  {

    return;
  }
}

/*********************************************************************
 * @fn      SPPBLEClient_autoConnect
 *
 * @brief   Client automatically connects to hardcoded address
 *
 * @return  none
 */
void SPPBLEClient_autoScan(void)
{
  GAPCentralRole_StartDiscovery(DEFAULT_DISCOVERY_MODE,
                                DEFAULT_DISCOVERY_ACTIVE_SCAN,
                                DEFAULT_DISCOVERY_WHITE_LIST);
}

void SPPBLEClient_autoConnect(void)
{
  uint8_t addrType;
  uint8_t peerAddr[6];

  // connect to hardcoded device address i.e. 0x050403020100
  int x = 0;
  for(x = 0; x<6; x++)
  {
      peerAddr[x] = connDevList[connDevList->connDevIndex].connDevMac[x];
  }
  
  addrType = ADDRTYPE_PUBLIC;

  //SDITask_PrintfToUART("%s\r\n", "Auto next connecting...");
  GAPCentralRole_EstablishLink(DEFAULT_LINK_HIGH_DUTY_CYCLE,
                               DEFAULT_LINK_WHITE_LIST,
                               addrType, peerAddr);

}
/*********************************************************************
 * @fn      SPPBLEClient_processGATTMsg
 *
 * @brief   Process GATT messages and events.
 *
 * @return  none
 */
static void SPPBLEClient_processGATTMsg(gattMsgEvent_t *pMsg)
{
  if (state == BLE_STATE_CONNECTED)
  {
    if(pMsg->method == ATT_HANDLE_VALUE_NOTI)
    {
      //Send received bytes to serial port
      SDITask_sendToUART(pMsg->msg.handleValueNoti.pValue, pMsg->msg.handleValueNoti.len);
    }
    // See if GATT server was unable to transmit an ATT response
    if (pMsg->hdr.status == blePending)
    {
      // No HCI buffer was available. App can try to retransmit the response
      // on the next connection event. Drop it for now.
      Display_print1(dispHandle, 4, 0, "ATT Rsp dropped %d", pMsg->method);
    }
    else if ((pMsg->method == ATT_READ_RSP)   ||
             ((pMsg->method == ATT_ERROR_RSP) &&
              (pMsg->msg.errorRsp.reqOpcode == ATT_READ_REQ)))
    {
      if (pMsg->method == ATT_ERROR_RSP)
      {
        Display_print1(dispHandle, 4, 0, "Read Error %d", pMsg->msg.errorRsp.errCode);
      }
      else
      {
        // After a successful read, display the read value
        Display_print1(dispHandle, 4, 0, "Read rsp: %d", pMsg->msg.readRsp.pValue[0]);
      }
    }
    else if ((pMsg->method == ATT_WRITE_RSP)  ||
             ((pMsg->method == ATT_ERROR_RSP) &&
              (pMsg->msg.errorRsp.reqOpcode == ATT_WRITE_REQ)))
    {
      if (pMsg->method == ATT_ERROR_RSP)
      {
        Display_print1(dispHandle, 4, 0, "Write Error %d", pMsg->msg.errorRsp.errCode);
      }
      else
      {
        // After a successful write, display the value that was written and
        // increment value
        Display_print1(dispHandle, 4, 0, "Write sent: %d", charVal++);
      }

    }
    else if (pMsg->method == ATT_FLOW_CTRL_VIOLATED_EVENT)
    {
      // ATT request-response or indication-confirmation flow control is
      // violated. All subsequent ATT requests or indications will be dropped.
      // The app is informed in case it wants to drop the connection.

      // Display the opcode of the message that caused the violation.
      Display_print1(dispHandle, 4, 0, "FC Violated: %d", pMsg->msg.flowCtrlEvt.opcode);
    }
    else if (pMsg->method == ATT_MTU_UPDATED_EVENT)
    {
      currentMTUSize = pMsg->msg.mtuEvt.MTU;
      SDITask_setAppDataSize(currentMTUSize);

      // MTU size updated
      SDITask_PrintfToUART("%s%d\r\n", "MTU Size:" ,currentMTUSize);
    }
    else if (discState != BLE_DISC_STATE_IDLE)
    {
      SPPBLEClient_processGATTDiscEvent(pMsg);
    }
  } // else - in case a GATT message came after a connection has dropped, ignore it.

  // Needed only for ATT Protocol messages
  GATT_bm_free(&pMsg->msg, pMsg->method);
}

/*********************************************************************
 * @fn      SPPBLEClient_startDiscovery
 *
 * @brief   Start service discovery.
 *
 * @return  none
 */
static void SPPBLEClient_startDiscovery(void)
{
  attExchangeMTUReq_t req;

  // Initialize cached handles
  svcStartHdl = svcEndHdl = charDataHdl = 0;

  discState = BLE_DISC_STATE_MTU;

  // Discover GATT Server's Rx MTU size
  req.clientRxMTU = maxPduSize - L2CAP_HDR_SIZE;

  // ATT MTU size should be set to the minimum of the Client Rx MTU
  // and Server Rx MTU values
  VOID GATT_ExchangeMTU(connHandle, &req, selfEntity);
}

/*********************************************************************
 * @fn      SPPBLEClient_processGATTDiscEvent
 *
 * @brief   Process GATT discovery event
 *
 * @return  none
 */
static void SPPBLEClient_processGATTDiscEvent(gattMsgEvent_t *pMsg)
{
  if (discState == BLE_DISC_STATE_MTU)
  {
    // MTU size response received, discover simple BLE service
    if (pMsg->method == ATT_EXCHANGE_MTU_RSP)
    {
      uint8_t uuid[ATT_UUID_SIZE] = { TI_BASE_UUID_128(SERIALPORTSERVICE_SERV_UUID) };

      discState = BLE_DISC_STATE_SVC;

     
      SDITask_PrintfToUART("%s\r\n", "Discovering services...");
      
      // Discovery simple BLE service
      VOID GATT_DiscPrimaryServiceByUUID(connHandle, uuid, ATT_UUID_SIZE,
                                         selfEntity);
    }
  }
  else if (discState == BLE_DISC_STATE_SVC)
  {
    // Service found, store handles
    if (pMsg->method == ATT_FIND_BY_TYPE_VALUE_RSP &&
        pMsg->msg.findByTypeValueRsp.numInfo > 0)
    {
      svcStartHdl = ATT_ATTR_HANDLE(pMsg->msg.findByTypeValueRsp.pHandlesInfo, 0);
      svcEndHdl = ATT_GRP_END_HANDLE(pMsg->msg.findByTypeValueRsp.pHandlesInfo, 0);
      
      SDITask_PrintfToUART("%s\r\n", "Found Serial Port Service...");
    }

    // If procedure complete
    if (((pMsg->method == ATT_FIND_BY_TYPE_VALUE_RSP) &&
         (pMsg->hdr.status == bleProcedureComplete))  ||
        (pMsg->method == ATT_ERROR_RSP))
    {
      if (svcStartHdl != 0)
      {
        attReadByTypeReq_t req;
        uint8_t uuid[ATT_UUID_SIZE] = { TI_BASE_UUID_128(SERIALPORTSERVICE_DATA_UUID) };

        // Discover characteristic
        discState = BLE_DISC_STATE_CHAR;

        req.startHandle = svcStartHdl;
        req.endHandle = svcEndHdl;
        req.type.len = ATT_UUID_SIZE;
        memcpy(req.type.uuid,  uuid, ATT_UUID_SIZE);

        //DEBUG("Reading UUIDs...");


        // Discover characteristic descriptors
        GATT_DiscAllCharDescs(connHandle,
                              svcStartHdl + 1,
                              svcEndHdl,
                              selfEntity);
      }
    }
  }
  else if (discState == BLE_DISC_STATE_CHAR)
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
      
      discState = BLE_DISC_STATE_IDLE;
    }

  }
}

/*********************************************************************
 * @fn      SPPBLEClient_findSvcUuid
 *
 * @brief   Find a given UUID in an advertiser's service UUID list.
 *
 * @return  TRUE if service UUID found
 */
static bool SPPBLEClient_findDevName(uint8_t *pData)
{
  // 从设备广播数据名称
  uint8_t pheralDeviceName[20] = {'P','e','r','i','p','h','e','r','a','l','_','u','a','r','t','t','r','a','n','s'};

  
  if(memcmp((char const*)pData+2, (char const*)pheralDeviceName,20) == 0)
  {
    return TRUE;
  }
  
  // Match not found
  return FALSE;
}

/*********************************************************************
 * @fn      SPPBLEClient_addDeviceInfo
 *
 * @brief   Add a device to the device discovery result list
 *
 * @return  none
 */
static void SPPBLEClient_addDeviceInfo(uint8_t *pAddr, uint8_t addrType)
{
  uint8_t i;

  // If result count not at max
  if (scanRes < DEFAULT_MAX_SCAN_RES)
  {
    // Check if device is already in scan results
    for (i = 0; i < scanRes; i++)
    {
      if (memcmp(pAddr, devList[i].addr , B_ADDR_LEN) == 0)
      {
        return;
      }
    }

    // Add addr to scan result list
    memcpy(devList[scanRes].addr, pAddr, B_ADDR_LEN);
    devList[scanRes].addrType = addrType;

    // Add addr for autoConnect
    memcpy(connDevList[connDevList->scanDevIndex].connDevMac, pAddr, B_ADDR_LEN);
    connDevList->scanDevIndex++;
    
    // Increment scan result count
    scanRes++;
  }
}

/*********************************************************************
 * @fn      SPPBLEClient_eventCB
 *
 * @brief   Central event callback function.
 *
 * @param   pEvent - pointer to event structure
 *
 * @return  TRUE if safe to deallocate event message, FALSE otherwise.
 */
static uint8_t SPPBLEClient_eventCB(gapCentralRoleEvent_t *pEvent)
{
  // Forward the role event to the application
  if (SPPBLEClient_enqueueMsg(SBC_STATE_CHANGE_EVT,
                                  SUCCESS, (uint8_t *)pEvent))
  {
    // App will process and free the event
    return FALSE;
  }

  // Caller should free the event
  return TRUE;
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
static void SPPBLEClient_genericHandler(UArg arg)
{
  // Wake up the application.
  Event_post(syncEvent, arg);
}

/*********************************************************************
 * @fn      SPPBLEClient_startDiscHandler
 *
 * @brief   Clock handler function
 *
 * @param   a0 - ignored
 *
 * @return  none
 */
void SPPBLEClient_startDiscHandler(UArg a0)
{
  Event_post(syncEvent, SBC_START_DISCOVERY_EVT);
}

/*********************************************************************
 * @fn      SPPBLEClient_keyChangeHandler
 *
 * @brief   Key event handler function
 *
 * @param   a0 - ignored
 *
 * @return  none
 */
void SPPBLEClient_keyChangeHandler(uint8 keys)
{
  SPPBLEClient_enqueueMsg(SBC_KEY_CHANGE_EVT, keys, NULL);
}

/*********************************************************************
 * @fn      SPPBLEClient_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   status - message status.
 * @param   pData - message data pointer.
 *
 * @return  TRUE or FALSE
 */
void SPPBLEClient_enqueueUARTMsg(uint8_t event, uint8_t *data, uint8_t len)
{
  sbcUARTEvt_t *pMsg;

  //Enqueue message only in a connected state
  if(state == BLE_STATE_CONNECTED)
  {
    // Create dynamic pointer to message.
    if (pMsg = ICall_malloc(sizeof(sbcUARTEvt_t)))
    {

      pMsg->pData = (uint8 *)ICall_allocMsg(len);
      if(pMsg->pData)
      {
        //payload
        memcpy(pMsg->pData , data, len);
      }
      pMsg->length = len;

      // Enqueue the message.
      Util_enqueueMsg(appUARTMsgQueue, syncEvent, (uint8_t *)pMsg);
    }
  }
}

/*********************************************************************
 * @fn      SPPBLEClient_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   state - message state.
 * @param   pData - message data pointer.
 *
 * @return  TRUE or FALSE
 */
static uint8_t SPPBLEClient_enqueueMsg(uint8_t event, uint8_t state,
                                           uint8_t *pData)
{
  sbcEvt_t *pMsg = ICall_malloc(sizeof(sbcEvt_t));

  // Create dynamic pointer to message.
  if (pMsg)
  {
    pMsg->hdr.event = event;
    pMsg->hdr.state = state;
    pMsg->pData = pData;

    // Enqueue the message.
    return Util_enqueueMsg(appMsgQueue, syncEvent, (uint8_t *)pMsg);
  }

  return FALSE;
}

/*********************************************************************
*********************************************************************/
