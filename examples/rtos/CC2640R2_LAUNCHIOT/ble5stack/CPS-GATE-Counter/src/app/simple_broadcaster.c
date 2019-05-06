/******************************************************************************

 @file       simple_broadcaster.c

 @brief This file contains the Simple Broadcaster sample application for
        use with the CC2650 Bluetooth Low Energy Protocol Stack.

 Group: CMCU, SCS
 Target Device: CC2640R2

 ******************************************************************************
 
 Copyright (c) 2011-2017, Texas Instruments Incorporated
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

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/display/Display.h>

#include <icall.h>
#include "util.h"
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"

#include "broadcaster.h"

#ifdef USE_RCOSC
#include "rcosc_calibration.h"
#endif //USE_RCOSC


#include "board.h"

#include "simple_broadcaster.h"

#include "hw_gpio.h"
#include "myiotboard_key.h"
#include "hw_i2c.h"
#include "cVL53L1X.h"
#include <stdbool.h>

#include <driverlib/aon_batmon.h>

#include <ti/sysbios/gates/GateHwi.h>
#include <Error.h>
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

#define SENSOR_TYPE     0x10 //GATE SENSOR
#define BATT_TYPE       0x1B 
// What is the advertising interval when device is discoverable (units of 625us, 160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL          160

// Type of Display to open
#if !defined(Display_DISABLE_ALL)
  #if defined(BOARD_DISPLAY_USE_LCD) && (BOARD_DISPLAY_USE_LCD!=0)
    #define SBB_DISPLAY_TYPE Display_Type_LCD
  #elif defined (BOARD_DISPLAY_USE_UART) && (BOARD_DISPLAY_USE_UART!=0)
    #define SBB_DISPLAY_TYPE Display_Type_UART
  #else // !BOARD_DISPLAY_USE_LCD && !BOARD_DISPLAY_USE_UART
    #define SBB_DISPLAY_TYPE 0 // Option not supported
  #endif // BOARD_DISPLAY_USE_LCD && BOARD_DISPLAY_USE_UART
#else // BOARD_DISPLAY_USE_LCD && BOARD_DISPLAY_USE_UART
  #define SBB_DISPLAY_TYPE 0 // No Display
#endif // Display_DISABLE_ALL

// Task configuration
#define SBB_TASK_PRIORITY                     1

#ifndef SBB_TASK_STACK_SIZE
#define SBB_TASK_STACK_SIZE                   660
#endif

// How often to perform periodic event (in msec)
#define SLEEP_BLK_TIME                        60000
#define SBB_PERIODIC_ADV_PERIOD               400
#define SBB_BATT_ADV_PERIOD                   400

#define NO_SLEEP_BLK                          20 // up to 32
#define HBT_BLK                               9 // report battery level after 9 data report

#define SBB_STATE_CHANGE_EVT                  0x0001
#define SBP_KEY_CHANGE_EVT                    0x0004

// Internal Events for RTOS application
#define SBB_ICALL_EVT                         ICALL_MSG_EVENT_ID // Event_Id_31
#define SBB_QUEUE_EVT                         UTIL_QUEUE_EVENT_ID // Event_Id_30

#define SBB_PERIODIC_EVT                      Event_Id_00
#define SBB_TIMER_PERIODIC_EVT                Event_Id_01

#define SBB_VL53_EVT                          Event_Id_03
// Bitwise OR of all events to pend on
#define SBB_ALL_EVENTS                        (SBB_ICALL_EVT | \
                                               SBB_QUEUE_EVT | \
                                               SBB_PERIODIC_EVT     | \
                                               SBB_TIMER_PERIODIC_EVT |\
                                               SBB_VL53_EVT  )

/*********************************************************************
 * TYPEDEFS
 */

// App event passed from profiles.
typedef struct
{
  appEvtHdr_t hdr; // Event header.
} sbbEvt_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Display Interface
Display_Handle dispHandle = NULL;

Task_Handle Task_Handel_VL53;// endg detector handle

uint8_t gled_s= 0;
uint8_t load = 0;
uint8_t gload = 0;
uint8_t blknumber = 0;
uint16_t heartbeat = 0;
uint32_t BATstatus = 0;
uint8_t activity[20] = {0};
uint8_t skipBroad = 0;
uint16_t mm[100] = {0};
bool tout = 0;
bool frontActived = false;
bool sideActived = false;
bool goingIn = false; // Front->Side = going in
bool goingOut = false; // Side->Front = going out
bool timedOut = false; // 10s active time expired
bool sensorErr = false; //failed to open sensor

uint16_t edgeBuf[6] = {0};
uint16_t mm_read = 0;
uint8_t peopleNum = 0;
uint8_t peopleIn = 0;
uint8_t peopleOut = 0;
uint8_t min_idx = 0; //minute window index
uint16_t testV0;
uint16_t testV1;
bool VL53EVT_POSTED = false;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID selfEntity;

// Event globally used to post local events and pend on system and
// local events.
static ICall_SyncHandle syncEvent;

// Clock instances for internal periodic events./////////////////////////////
static Clock_Struct periodicClock;
static Clock_Struct TIMER_periodicClock;

// Queue object used for app messages
static Queue_Struct appMsg;
static Queue_Handle appMsgQueue;

// Task configuration
Task_Struct sbbTask;
Char sbbTaskStack[SBB_TASK_STACK_SIZE];

//Battery level variable
static uint8 BatadvertData[26] = {'C','P','S',BATT_TYPE,0xBA,0xBA,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xAA,0xFE};

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8 scanRspData[26] =
{
  // complete name
  0x15,   // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
  'C',
  'P',
  'S',
  '-',
  'M',
  'A',
  'R',
  '2',
  '0',
  '1',
  '9',
  ':',
  ':',
  ':',
  ':',
  ':',
  ':',
  ':',
  ':',
  ':',

  // Tx power level
  0x02,   // length of this data
  GAP_ADTYPE_POWER_LEVEL,
  0       // 0dBm
};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
static uint8 advertData[26] =
{
  // Flags; this sets the device to use limited discoverable
  // mode (advertises for 30 seconds at a time) instead of general
  // discoverable mode (advertises indefinitely)
  'C',  //43                                                            [0]
  'P',  //50                                                            [1]
  'S',  //53                                                            [2]
  SENSOR_TYPE, //sensor type                                            [3]

  0,  //                                                                [4]
  0,  //                                                                [5]
  0,   //                                                               [6]
  0,   //                                                               [7]
  0,   //                                                               [8]
  0,   //                                                               [9]
  0,   //                                                               [10]
  0,   //                                                               [11]
  0,   //                                                               [12]
  0,   //                                                               [13]
  0,   //                                                               [14]
  0,   //                                                               [15]
  0,   //                                                               [16]
  0,   //                                                               [17]
  0,   //                                                               [18]
  0,   //                                                               [19]
  0,   //                                                               [20]
  0,   //                                                               [21]
  0,   //                                                               [22]
  0,    //                                                              [23]
  0xAA,                                 //                              [24]
  0xFE  //                                                              [25]
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void SimpleBLEBroadcaster_init(void);
static void SimpleBLEBroadcaster_taskFxn(UArg a0, UArg a1);

static void SimpleBLEBroadcaster_processStackMsg(ICall_Hdr *pMsg);
static void SimpleBLEBroadcaster_processAppMsg(sbbEvt_t *pMsg);
static void SimpleBLEBroadcaster_processStateChangeEvt(gaprole_States_t newState);

static void SimpleBLEBroadcaster_stateChangeCB(gaprole_States_t newState);

static void SimpleBLEPeripheral_clockHandler(UArg arg);
static void SimpleBLEPeripheral_performPeriodicTask(void);

void SimpleBLEPeripheral_keyChangeHandler(uint8 keys);
static void SimpleBLEPeripheral_handleKeys(uint8_t keys);
static void SimpleBLEPeripheral_enqueueMsg(uint8_t event, uint8_t state);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static gapRolesCBs_t simpleBLEBroadcaster_BroadcasterCBs =
{
  SimpleBLEBroadcaster_stateChangeCB   // Profile State Change Callbacks
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SimpleBLEBroadcaster_createTask
 *
 * @brief   Task creation function for the Simple Broadcaster.
 *
 * @param   none
 *
 * @return  none
 */
void SimpleBLEBroadcaster_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = sbbTaskStack;
  taskParams.stackSize = SBB_TASK_STACK_SIZE;
  taskParams.priority = SBB_TASK_PRIORITY;

  Task_construct(&sbbTask, SimpleBLEBroadcaster_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      SimpleBLEBroadcaster_init
 *
 * @brief   Initialization function for the Simple Broadcaster App
 *          Task. This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notification ...).
 *
 * @param   none
 *
 * @return  none
 */
static void SimpleBLEBroadcaster_init(void)
{
	// ******************************************************************
  // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ******************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
  ICall_registerApp(&selfEntity, &syncEvent);

  // Hard code the DB Address till CC2650 board gets its own IEEE address
  //uint8 bdAddress[B_ADDR_LEN] = { 0x33, 0x33, 0x33, 0x33, 0x33, 0x33 };
  //HCI_EXT_SetBDADDRCmd(bdAddress);

#ifdef USE_RCOSC
  RCOSC_enableCalibration();
#endif // USE_RCOSC

  // Create an RTOS queue for message from profile to be sent to app.
  appMsgQueue = Util_constructQueue(&appMsg);

    // Create one-shot clocks for internal periodic events.
   Util_constructClock(&periodicClock, SimpleBLEPeripheral_clockHandler,
                      SLEEP_BLK_TIME, 0, false, SBB_PERIODIC_EVT);

  Util_constructClock(&TIMER_periodicClock, SimpleBLEPeripheral_clockHandler,
                      SLEEP_BLK_TIME, SLEEP_BLK_TIME, true, SBB_TIMER_PERIODIC_EVT);
  
  // Open LCD
  dispHandle = Display_open(SBB_DISPLAY_TYPE, NULL);
  
  // Setup the GAP Broadcaster Role Profile
  {
    // For all hardware platforms, device starts advertising upon initialization
    uint8_t initial_advertising_enable = FALSE; 

    // By setting this to zero, the device will go into the waiting state after
    // being discoverable for 30.72 second, and will not being advertising again
    // until the enabler is set back to TRUE
    uint16_t gapRole_AdvertOffTime = 0;

#ifndef BEACON_FEATURE
    uint8_t advType = GAP_ADTYPE_ADV_SCAN_IND; // use scannable undirected adv
#else
    uint8_t advType = GAP_ADTYPE_ADV_NONCONN_IND; // use non-connectable adv
#endif // !BEACON_FEATURE

    // Set the GAP Role Parameters
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                         &initial_advertising_enable);
    GAPRole_SetParameter(GAPROLE_ADVERT_OFF_TIME, sizeof(uint16_t),
                         &gapRole_AdvertOffTime);
    GAP_SetParamValue (TGAP_LIM_ADV_TIMEOUT, 1);

    GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof (scanRspData),
                         scanRspData);
    //GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);

    GAPRole_SetParameter(GAPROLE_ADV_EVENT_TYPE, sizeof(uint8_t), &advType);
  }
  
    // Init key debouncer
  Board_initKeys(SimpleBLEPeripheral_keyChangeHandler);

  // Set advertising interval
  {
    uint16_t advInt = DEFAULT_ADVERTISING_INTERVAL;

    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MAX, advInt);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MAX, advInt);
  }
  
  //set TX power
  
  HCI_EXT_SetTxPowerCmd(HCI_EXT_TX_POWER_5_DBM);

  // Start the Device
  VOID GAPRole_StartDevice(&simpleBLEBroadcaster_BroadcasterCBs);

  Display_print0(dispHandle, 0, 0, "BLE Broadcaster");
  
  // Setup the VL53L1X sensor
  HwGPIOInit();
  HwGPIOSet(IOID_9,0);
  HwI2CInit();
  
  AONBatMonEnable();
  BATstatus = AONBatMonBatteryVoltageGet();//Get battery voltage (this will return battery voltage in decimal form you need to convert)
  // convert in Milli volts
  BATstatus = (BATstatus * 125) >> 5;
  
  BatadvertData[6] = BATstatus>>8;
  BatadvertData[7] = BATstatus&0xFF;
  GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(BatadvertData), BatadvertData);//update broadcast register
        Display_print0(dispHandle, 2, 0, "Advertising");
  uint8_t param = TRUE;
  GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),&param);
  Util_restartClock(&TIMER_periodicClock,SBB_PERIODIC_ADV_PERIOD);

}

/*********************************************************************
 * @fn      SimpleBLEBroadcaster_processEvent
 *
 * @brief   Application task entry point for the Simple Broadcaster.
 *
 * @param   none
 *
 * @return  none
 */
static void SimpleBLEBroadcaster_taskFxn(UArg a0, UArg a1)
{
  // Initialize application
  SimpleBLEBroadcaster_init();

  // Application main loop
  
  for (;;)
  {
    // Get the ticks since startup
    uint32_t tickStart = Clock_getTicks();

    uint32_t events;

    events = Event_pend(syncEvent, Event_Id_NONE, SBB_ALL_EVENTS,
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
          SimpleBLEBroadcaster_processStackMsg((ICall_Hdr *)pMsg);
        }

        if (pMsg)
        {
          ICall_freeMsg(pMsg);
        }
      }

      // If RTOS queue is not empty, process app message.
      if (events & SBB_QUEUE_EVT)
      {
        while (!Queue_empty(appMsgQueue))
        {
          sbbEvt_t *pMsg = (sbbEvt_t *)Util_dequeueMsg(appMsgQueue);
          if (pMsg)
          {
            // Process message.
            SimpleBLEBroadcaster_processAppMsg(pMsg);

            // Free the space from the message.
            ICall_free(pMsg);
          }
        }
      }
      if ((events & SBB_PERIODIC_EVT) && (timedOut == false))
      {
        Util_startClock(&periodicClock);

        // Perform periodic application task
        SimpleBLEPeripheral_performPeriodicTask();
      }
	  
	  if(events & SBB_VL53_EVT){
		  HwGPIOSet(IOID_9,1);//powerup the VL53 sensor 
		  Task_sleep(1*(1000 / Clock_tickPeriod));
		  setTimeout(500);
		  if (!VL_init(true))
		  {
			// failured to detect the sensor
			sensorErr = true;
		  }
		  
		  //parameters adjusted for 100ms per read
		  startContinuous(50);
		  int i = 0;
		  uint8_t skipReads = 0;
		  for(i = 0;i < 100;i++){
			mm_read = read(true);// read new data from sensor
			if (timeoutOccurred()) {tout = 1; return;}// if failed to read
			uint8_t j;
			for(j = 0;j<5;j++){// shift buffer
					edgeBuf[j] = edgeBuf[j+1];
			}
			edgeBuf[5] = mm_read;
			if(skipReads>0){skipReads--;Task_sleep(95*(1000 / Clock_tickPeriod));continue;}
			if(i > 6){// when the buffer is populated
					uint16_t upper_half = (edgeBuf[0]+edgeBuf[1]+edgeBuf[2])/3;//average distance before the edge
					uint16_t lower_half = (edgeBuf[3]+edgeBuf[4]+edgeBuf[5])/3;//average distance before the edge
					if((upper_half - lower_half) > 300){ //300 is a typical human body width 30cm
							peopleNum++;// new peopel detected
							skipReads = 2; // clean the rest elements in the buffer
					}
					
			}
			
			Task_sleep(95*(1000 / Clock_tickPeriod));
			
		  }
		  stopContinuous();
		  
		  HwGPIOSet(IOID_9,0);//shutdown the VL53 sensor power 
		  
		  ASM_NOP;
		  
		  //timedOut = true;
		  
		  if(goingIn){peopleIn += peopleNum;}
		  if(goingOut){peopleOut += peopleNum;}
		  peopleNum = 0;
		  
		  frontActived = false;
		  sideActived = false;
		  VL53EVT_POSTED = false;
	  }
	  
      if (events & SBB_TIMER_PERIODIC_EVT)
      {
        if(skipBroad > 0){// wait for one period to allow broadcast complete
          skipBroad--;
          SimpleBLEBroadcaster_processStateChangeEvt(GAPROLE_WAITING);
        }
        if(gled_s){
          gled_s = 0;
          HwGPIOSet(Board_GLED,gled_s);
          SimpleBLEBroadcaster_processStateChangeEvt(GAPROLE_WAITING);
          Util_restartClock(&TIMER_periodicClock,SLEEP_BLK_TIME);
        }else{
			if(heartbeat < HBT_BLK){// should I report battery level?
				if(blknumber < NO_SLEEP_BLK){
					if(peopleIn > 0 && goingIn == true){ // activity happened in this interval
						activity[min_idx] |= (((peopleIn & 0x0F))<<4); 
					}
					if(peopleOut > 0 && goingOut == true){ // activity happened in this interval
						activity[min_idx] |= (peopleOut & 0x0F); 
					}
					min_idx++; // goto the next minute
					
					frontActived = false;
					sideActived = false;
					goingIn = false; // Front->Side = going in
					goingOut = false; // Side->Front = going out
					timedOut = false; // 10s active time expired
					sensorErr = false; //failed to open sensor
                                        memset(edgeBuf,0,6);
					mm_read = 0;
					peopleNum = 0;
					peopleIn = 0;
					peopleOut = 0;
					
					blknumber++;
					gled_s = 0;
					HwGPIOSet(Board_GLED,gled_s);
					SimpleBLEBroadcaster_processStateChangeEvt(GAPROLE_WAITING);
					Util_restartClock(&TIMER_periodicClock,SLEEP_BLK_TIME);
					VL53EVT_POSTED = false;
				}else{
					heartbeat++;
					blknumber = 0;
					gled_s = 1;
					min_idx = 0;// reset the minute index
					HwGPIOSet(Board_GLED,gled_s);
					advertData[4] = activity[0];
					advertData[5] = activity[1];
					advertData[6] = activity[2];
					advertData[7] = activity[3];
					advertData[8] = activity[4];
					advertData[9] = activity[5];
					advertData[10] = activity[6];
					advertData[11] = activity[7];
					advertData[12] = activity[8];
					advertData[13] = activity[9];
					advertData[14] = activity[10];
					advertData[15] = activity[11];
					advertData[16] = activity[12];
					advertData[17] = activity[13];
					advertData[18] = activity[14];
					advertData[19] = activity[15];
					advertData[20] = activity[16];
					advertData[21] = activity[17];
					advertData[22] = activity[18];
					advertData[23] = activity[19];

					memset(activity,0,sizeof(activity)); // reset activity
					GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);//update broadcast register
					SimpleBLEBroadcaster_processStateChangeEvt(GAPROLE_ADVERTISING);
					Util_restartClock(&TIMER_periodicClock,SBB_PERIODIC_ADV_PERIOD);
				}
			}else{
				heartbeat = 0;
				blknumber = 0; // need to reset block number counter as well otherwise not reporting data, only batttery level
				BATstatus = AONBatMonBatteryVoltageGet();//Get battery voltage (this will return battery voltage in decimal form you need to convert)
				// convert in Milli volts
				BATstatus = (BATstatus * 125) >> 5;
				skipBroad = 1;
				BatadvertData[6] = BATstatus>>8;
				BatadvertData[7] = BATstatus&0xFF;
				GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(BatadvertData), BatadvertData);//update broadcast register
				SimpleBLEBroadcaster_processStateChangeEvt(GAPROLE_ADVERTISING);
				Util_restartClock(&TIMER_periodicClock,SBB_BATT_ADV_PERIOD);
			}
        }
      }
    }
  }
}

/*********************************************************************
 * @fn      SimpleBLEBroadcaster_processStackMsg
 *
 * @brief   Process an incoming stack message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void SimpleBLEBroadcaster_processStackMsg(ICall_Hdr *pMsg)
{
  switch (pMsg->event)
  {
    default:
      // do nothing
      break;
  }
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_processAppMsg
 *
 * @brief   Process an incoming callback from a profile.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void SimpleBLEBroadcaster_processAppMsg(sbbEvt_t *pMsg)
{
  switch (pMsg->hdr.event)
  {
    case SBB_STATE_CHANGE_EVT:
      SimpleBLEBroadcaster_processStateChangeEvt((gaprole_States_t)pMsg->
                                                 hdr.state);
      break;
      
    case SBP_KEY_CHANGE_EVT:
      SimpleBLEPeripheral_handleKeys(pMsg->hdr.state);
      break;

    default:
      // Do nothing.
      break;
  }
}

/*********************************************************************
 * @fn      SimpleBLEBroadcaster_stateChangeCB
 *
 * @brief   Callback from GAP Role indicating a role state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void SimpleBLEBroadcaster_stateChangeCB(gaprole_States_t newState)
{
  sbbEvt_t *pMsg;

  // Create dynamic pointer to message.
  if ((pMsg = ICall_malloc(sizeof(sbbEvt_t))))
  {
    pMsg->hdr.event = SBB_STATE_CHANGE_EVT;
    pMsg->hdr.state = newState;

    // Enqueue the message.
    Util_enqueueMsg(appMsgQueue, syncEvent, (uint8_t *)pMsg);
  }
}

/*********************************************************************
 * @fn      SimpleBLEBroadcaster_processStateChangeEvt
 *
 * @brief   Notification from the profile of a state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void SimpleBLEBroadcaster_processStateChangeEvt(gaprole_States_t newState)
{
  switch (newState)
  {
    case GAPROLE_STARTED:
      {
        uint8 ownAddress[B_ADDR_LEN];

        GAPRole_GetParameter(GAPROLE_BD_ADDR, ownAddress);
        // Display device address
        Display_print0(dispHandle, 1, 0, Util_convertBdAddr2Str(ownAddress));
        Display_print0(dispHandle, 2, 0, "Initialized");
      }
      break;

    case GAPROLE_ADVERTISING:
      { 
        Display_print0(dispHandle, 2, 0, "Advertising");
        uint8_t param = TRUE;
        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                         &param);
      }
      break;

    case GAPROLE_WAITING:
      {
        Display_print0(dispHandle, 2, 0, "Waiting");
        uint8_t param = FALSE;
        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                         &param);
      }
      break;

    case GAPROLE_ERROR:
      {
        Display_print0(dispHandle, 2, 0, "Error");
      }
      break;

    default:
      {
        Display_clearLine(dispHandle, 2);
      }
      break;
  }
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_clockHandler
 *
 * @brief   Handler function for clock timeouts.
 *
 * @param   arg - event type
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_clockHandler(UArg arg)
{
  // Wake up the application.
  Event_post(syncEvent, arg);
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_performPeriodicTask
 *
 * @brief   Perform a periodic application task. This function gets called
 *          every five seconds (SBP_PERIODIC_EVT_PERIOD). In this example,
 *          the value of the third characteristic in the SimpleGATTProfile
 *          service is retrieved from the profile, and then copied into the
 *          value of the the fourth characteristic.
 *
 * @param   None.
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_performPeriodicTask(void)
{

}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_handleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   keys - bit field for key events. Valid entries:
 *                 FrontPIR Active H
 *                 SidePIR Active H
                   LaserIR interrupt Active H
 *
 * @return  none
 */
static void SimpleBLEPeripheral_handleKeys(uint8_t keys)
{
  ASM_NOP;
  
  if(timedOut == true){return;}//time window expired need reset.
  
  if ((keys & FrontPIR)||(keys & SidePIR))
  {
    if(VL53EVT_POSTED == false){
      VL53EVT_POSTED = true;
      Event_post(syncEvent, SBB_VL53_EVT);
    }
//                if(Task_Handel_VL53 == NULL){ // if no encoder running 
//			CreateVL53_Task();// start sampling laser IR sensor
//		}
	
  }
  // if (keys & SidePIR)
  // {
    // if(VL53EVT_POSTED == false){
      // Event_post(syncEvent, SBB_VL53_EVT);
      // VL53EVT_POSTED = true;
    // }
// //                if(Task_Handel_VL53 == NULL){ // if no encoder running 
// //			CreateVL53_Task();// start sampling laser IR sensor
// //		}
  // }
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   state - message state.
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_enqueueMsg(uint8_t event, uint8_t state)
{
  sbbEvt_t *pMsg;

  // Create dynamic pointer to message.
  if ((pMsg = ICall_malloc(sizeof(sbbEvt_t))))
  {
    pMsg->hdr.event = event;
    pMsg->hdr.state = state;

    // Enqueue the message.
    Util_enqueueMsg(appMsgQueue, syncEvent, (uint8*)pMsg);
  }
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_keyChangeHandler
 *
 * @brief   Key event handler function
 *
 * @param   keys - bitmap of pressed keys
 *
 * @return  none
 */
void SimpleBLEPeripheral_keyChangeHandler(uint8 keys)
{
  SimpleBLEPeripheral_enqueueMsg(SBP_KEY_CHANGE_EVT, keys);
}
// /*********************************************************************
// *********************************************************************/
// static void VL53_Task_Fxn(UArg arg0, UArg arg1)
// {
  // HwGPIOSet(IOID_9,1);//powerup the VL53 sensor 
  // Task_sleep(1*(1000 / Clock_tickPeriod));
  // setTimeout(500);
  // if (!VL_init(true))
  // {
    // // failured to detect the sensor
    // sensorErr = true;
  // }
  
  // //parameters adjusted for 100ms per read
  // startContinuous(50);
  // int i = 0;
  // uint8_t skipReads = 0;
  // for(i = 0;i < 100;i++){
    // mm_read = read(true);// read new data from sensor
    // if (timeoutOccurred()) {tout = 1; return;}// if failed to read
    // uint8_t j;
    // for(j = 0;j<5;j++){// shift buffer
            // edgeBuf[j] = edgeBuf[j+1];
    // }
    // edgeBuf[5] = mm_read;
    // if(skipReads>0){skipReads--;Task_sleep(95*(1000 / Clock_tickPeriod));continue;}
    // if(i > 6){// when the buffer is populated
            // uint16_t upper_half = (edgeBuf[0]+edgeBuf[1]+edgeBuf[2])/3;//average distance before the edge
            // uint16_t lower_half = (edgeBuf[3]+edgeBuf[4]+edgeBuf[5])/3;//average distance before the edge
            // if((upper_half - lower_half) > 300){ //300 is a typical human body width 30cm
                    // peopleNum++;// new peopel detected
                    // skipReads = 2; // clean the rest elements in the buffer
            // }
            
    // }
	
    // Task_sleep(95*(1000 / Clock_tickPeriod));
    
  // }
  // stopContinuous();
  
  // HwGPIOSet(IOID_9,0);//shutdown the VL53 sensor power 
  
  // ASM_NOP;
  
  // //timedOut = true;
  
  // if(goingIn){peopleIn += peopleNum;}
  // if(goingOut){peopleOut += peopleNum;}
  // peopleNum = 0;
  
  // frontActived = false;
  // sideActived = false;
  // testV0 = Clock_getTicks();
  // Task_delete(&Task_Handel_VL53);//one-time task free space after execution
  // testV1 = testV0;
// }
// static void CreateVL53_Task(void)
// {
  // Task_Params taskParams;
  // Task_Params_init(&taskParams);
  // taskParams.arg0 = 1000000 / Clock_tickPeriod;
  // taskParams.stackSize = SBB_TASK_STACK_SIZE;
  // taskParams.priority = 1;
  // Task_Handel_VL53 = Task_create((Task_FuncPtr)VL53_Task_Fxn, &taskParams, NULL);
// }