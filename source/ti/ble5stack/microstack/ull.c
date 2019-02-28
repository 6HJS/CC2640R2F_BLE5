/******************************************************************************

 @file       ull.c

 @brief This file contains the Micro Link Layer (uLL) API for the Micro
        BLE Stack.

        This API is mostly based on the Bluetooth Core Specification,
        V4.2, Vol. 6.

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

/*********************************************************************
 * INCLUDES
 */
#include <stdlib.h>

#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Swi.h>

#include <driverlib/ioc.h>

#include <icall.h>

#include <ti/drivers/rf/RF.h>
#include <ll_common.h>
#include <bcomdef.h>
#include <util.h>

#include <uble.h>
#include <urfi.h>
#include <ull.h>

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/* uLL State */
#define UL_STATE_STANDBY               0
#define UL_STATE_ADVERTISING           1
#define UL_STATE_SCANNING              2

/* Clock Event for about to advertising */
#define UL_CLKEVT_ADV_ABOUT_TO         1
/* Clock event for advertisement interval */
#define UL_CLKEVT_ADV_INT_EXPIRED      2

/* Time gap in RAT ticks between adv commands in the same command chain */
#define UL_ADV_GAP_IN_CHAIN  ((uint32) (374.5 * US_TO_RAT))

/* uLL adv tx status */
#define UL_ADV_TX_DONE                 1
#define UL_ADV_TX_SCHEDULED            2
#define UL_ADV_TX_NO_RF_RESOURCE       3
#define UL_ADV_TX_FAILED               4


/* uLL advertisement modes */
#define UL_ADV_MODE_START              1
#define UL_ADV_MODE_IMMEDIATE          2
#define UL_ADV_MODE_PERIODIC           3
#define UL_ADV_MODE_PERIODIC_RELAXED   4
#define UL_ADV_MODE_RESCHEDULE         5

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
 extern bStatus_t ub_buildAndPostEvt(ubEvtDst_t evtDst, ubEvt_t evt,
                                    ubMsg_t *pMsg, uint16 len);

/*********************************************************************
 * EXTERNAL VARIABLES
 */

extern ubParams_t ubParams;
extern rfc_CMD_BLE_ADV_COMMON_t urAdvCmd[];
extern uint8 rfTimeCrit;

/*********************************************************************
 * LOCAL VARIABLES
 */

/* Micro Link Layer State */
static uint8 ulState = UL_STATE_STANDBY;

#if defined(FEATURE_ADVERTISER)
/* Advertiser callbacks */
static pfnAdvAboutToCB_t ul_notifyAdvAboutTo;
static pfnAdvDoneCB_t    ul_notifyAdvDone;
/* Clock object used for "about to advertise" notification */
static Clock_Struct cAdvAboutTo;
static Clock_Handle hcAdvAboutTo = NULL;

/* Advertisement status */
static uint8 advTxStatus = UL_ADV_TX_DONE;

/* Clock object used to track the advertisement interval */
static Clock_Struct cAdvInt;

#if defined(RF_MULTIMODE)
static Clock_Handle hcAdvInt = NULL;
#endif /* RF_MULTIMODE */

#endif /* FEATURE_ADVERTISER */

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */
bStatus_t ul_advSchedule(uint8 mode);

/*********************************************************************
 * CALLBACKS
 */

/*********************************************************************
 * @fn      ul_clockHandler
 *
 * @brief   Clock handler function
 *
 * @param   a0 - event
 *
 * @return  none
 */
void ul_clockHandler(UArg a0)
{
#if defined(FEATURE_ADVERTISER)
  /* Process AboutToAdv */
  if (a0 == UL_CLKEVT_ADV_ABOUT_TO)
  {
    ul_notifyAdvAboutTo();
  }
#ifdef RF_MULTIMODE
  /* Process advertise interval expiry */
  if (a0 == UL_CLKEVT_ADV_INT_EXPIRED)
  {
    ub_buildAndPostEvt(UB_EVTDST_LL, ULL_EVT_ADV_TX_TIMER_EXPIRED, NULL, 0);
  }
#endif /* RF_MULTIMODE */
#endif  /* FEATURE_ADVERTISER */
}

/*********************************************************************
 * @fn      ul_advDoneCb
 *
 * @brief   Callback function to be invoked by RF driver
 *
 * @param   rfHandle - RF client handle
 *
 * @param   cmdHandle - RF command handle
 *
 * @param   events - RF events
 *
 * @return  none
 */
#if defined(RF_MULTIMODE)
void ul_advDoneCb(RF_Handle rfHandle, RF_CmdHandle cmdHandle,
                  RF_EventMask events)
{
  volatile uint32 keySwi;

  keySwi = Swi_disable();

  if (events & RF_EventLastCmdDone)
  {
    ub_buildAndPostEvt(UB_EVTDST_LL, ULL_EVT_ADV_TX_SUCCESS, NULL, 0);
  }
  else if (events &
           (RF_EventCmdAborted | RF_EventCmdStopped))
  {
    if (advTxStatus == UL_ADV_TX_SCHEDULED)
    {
      ub_buildAndPostEvt(UB_EVTDST_LL, ULL_EVT_ADV_TX_FAILED, NULL, 0);
    }
  }

  Swi_restore(keySwi);
}
#else
void ul_advDoneCb(RF_Handle rfHandle, RF_CmdHandle cmdHandle,
                  RF_EventMask events)
{
  volatile uint32 keySwi;

  keySwi = Swi_disable();

  if (events & RF_EventLastCmdDone)
  {
    /* Notify uGAP that the last ADV was done successfully. */
    ul_notifyAdvDone(SUCCESS);

    if (ulState == UL_STATE_ADVERTISING)
    {
      if (SUCCESS != ul_advSchedule(UL_ADV_MODE_PERIODIC))
      {
        /* Switch to StadnBy state */
        ulState = UL_STATE_STANDBY;

        Swi_restore(keySwi);

        /* No more scheduling is available */
        ul_notifyAdvDone(bleNoResources);
      }
      else
      {
        Swi_restore(keySwi);
      }
    }
    else
    {
      Swi_restore(keySwi);
    }
  }
  else if (events &
           (RF_EventCmdAborted | RF_EventCmdStopped | RF_EventCmdCancelled))
  {
    /* Switch to StadnBy state */
    ulState = UL_STATE_STANDBY;

    Swi_restore(keySwi);
  }
  else
  {
    Swi_restore(keySwi);
  }
}
#endif /* RF_MULTIMODE */

/*********************************************************************
 * PUBLIC FUNCTIONS
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
bStatus_t ul_init(void)
{
  if (ulState != UL_STATE_STANDBY)
  {
    /* Cannot re-initialize if running */
    return FAILURE;
  }

  /* Initialize RF Interface */
  if (ur_init() != SUCCESS)
  {
    return FAILURE;
  }

#if defined(FEATURE_ADVERTISER)
  if (hcAdvAboutTo == NULL)
  {
    Clock_Params clockParams;

    /* Setup AdvAboutTo timer as a one-shot timer */
    Clock_Params_init(&clockParams);
    clockParams.arg = UL_CLKEVT_ADV_ABOUT_TO;
    clockParams.period = 0;
    clockParams.startFlag = false;
    Clock_construct(&cAdvAboutTo, ul_clockHandler, 0, &clockParams);

    hcAdvAboutTo = Clock_handle(&cAdvAboutTo);
  }

#if defined(RF_MULTIMODE)
  if (hcAdvInt == NULL)
  {
    Clock_Params clockParams;

    /* Setup hcAdvInt timer as a one-shot timer */
    Clock_Params_init(&clockParams);
    clockParams.arg = UL_CLKEVT_ADV_INT_EXPIRED;
    clockParams.period = 0;
    clockParams.startFlag = false;
    Clock_construct(&cAdvInt, ul_clockHandler, 0, &clockParams);

    hcAdvInt = Clock_handle(&cAdvInt);
  }
#endif /* RF_MULTIMODE */
#endif /* FEATURE_ADVERTISER */

  return SUCCESS;
}

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
                      pfnAdvDoneCB_t pfnAdvDoneCB)
{
  ul_notifyAdvAboutTo = pfnAdvAboutToCB;
  ul_notifyAdvDone = pfnAdvDoneCB;
}
#endif /* FEATURE_ADVERTISER */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

#if defined(FEATURE_ADVERTISER)
/*********************************************************************
 * @fn      ul_advSchedule
 *
 * @brief   Update ADV command with latest parameters and schedule
 *          the radio operation
 *
 * @param   mode   -  if UL_ADV_MODE_START, start the advertisement and schedule
 *                    the operation for 1 ms later. If set to
 *                    UL_ADV_MODE_IMMEDIATE schedules the operation 1ms later.
 *                    UL_ADV_MODE_PERIODIC and UL_ADV_MODE_PERIODIC_RELAXED,
 *                    sechedule the operation, according to the advInterval and
 *                    the random delay for time critical and time relaxed
 *                    operation respectively. UL_ADV_MODE_RESCHEDULE,
 *                    reschedules the operation at the same previous schedule.
 *
 * @return  SUCCESS - the radio operation has been successfully scheduled
 *          FAILURE - failed to schedule the radio operation due to RF driver
 *                    error
 */
bStatus_t ul_advSchedule(uint8 mode)
{
  uint16 commandNo;
#if defined(RF_MULTIMODE)
  RF_ScheduleCmdParams cmdParams;
  uint8  numChan; /* # of adv channels */
#endif /* RF_MULTIMODE */

#if defined(FEATURE_SCAN_RESPONSE)
  if (ubParams.advType == UB_ADVTYPE_ADV_SCAN)
  {
    commandNo = CMD_BLE_ADV_SCAN;
  }
  else
#endif /* FEATURE_SCAN_RESPONSE */
  /* UB_ADVTYPE_ADV_NC */
  {
    commandNo = CMD_BLE_ADV_NC;
  }

  for (uint8 i = 0; i < 3; i++)
  {
    urAdvCmd[i].commandNo = (ubParams.advChanMap & (1 << i)) ?
                            commandNo : CMD_NOP;
  }

  if ((UL_ADV_MODE_START == mode) || (UL_ADV_MODE_IMMEDIATE == mode))
  {
    /* Schedule the first adv event for 1 ms from now */
    urAdvCmd[0].startTime = RF_getCurrentTime() + 1 * MS_TO_RAT;
  }
  else if (UL_ADV_MODE_PERIODIC == mode)
  {
    /* Add a random delay up to 10ms */
    urAdvCmd[0].startTime += ubParams.advInterval * BLE_TO_RAT +
                             (uint32) rand() % (MS_TO_RAT * 10);
  }
  else if (UL_ADV_MODE_PERIODIC_RELAXED == mode)
  {
    /* Anchor to the current time*/
    urAdvCmd[0].startTime = RF_getCurrentTime();
    /* Add a random delay up to 10ms to the interval */
    urAdvCmd[0].startTime += ubParams.advInterval * BLE_TO_RAT +
                             (uint32) rand() % (MS_TO_RAT * 10);

  }
  // clear command status values
  urAdvCmd[0].status = IDLE;
  urAdvCmd[1].status = IDLE;
  urAdvCmd[2].status = IDLE;

#if defined(RF_MULTIMODE)
    /* Calculate the end time */
    numChan = ubParams.advChanMap & 0x01 + ((ubParams.advChanMap & 0x02) >> 1) +
              ((ubParams.advChanMap & 0x04) >> 2);
    cmdParams.endTime =
      urAdvCmd[0].startTime
      + numChan * (((urAdvParams.advLen + 16) * BYTE_TO_RAT) + UL_ADV_GAP_IN_CHAIN)
      - UL_ADV_GAP_IN_CHAIN;
    cmdParams.priority = (RF_Priority) ubParams.rfPriority;

  if ((mode != UL_ADV_MODE_RESCHEDULE) && (mode != UL_ADV_MODE_IMMEDIATE))
  {
    uint32 timerTicks;

    if (RF_TIME_CRITICAL == rfTimeCrit)
    {
      if (UL_ADV_MODE_START == mode)
      {
        timerTicks = ubParams.advInterval * BLE_TO_RAT;
      }
      else
      {
        timerTicks = cmdParams.endTime - RF_getCurrentTime();
      }
      /* Set timeout equal to advertisement interval plus 1 ms */
      timerTicks = (timerTicks)/SYSTICK_TO_RAT;
      timerTicks += MS_TO_SYSTICK;

    }
    else //relaxed operation.
    {
      if (UL_ADV_MODE_START == mode)
      {
        timerTicks = ubParams.advInterval * BLE_TO_RAT;
      }
      else
      {
        timerTicks = cmdParams.endTime - RF_getCurrentTime();
      }

      /* Set timeout equal to two times the advertisement interval minus 1 ms */
      timerTicks = (timerTicks * 2)/SYSTICK_TO_RAT;
      timerTicks -= MS_TO_SYSTICK;
    }

    if (Clock_isActive(hcAdvInt))
    {
      /* stop the clock first */
      Clock_stop(hcAdvInt);
    }

    Clock_setTimeout(hcAdvInt, timerTicks);
    Clock_start(hcAdvInt);
  }

  urAdvHandle = RF_scheduleCmd(urHandle, (RF_Op*) &urAdvCmd[0],
                               &cmdParams, ul_advDoneCb,
                               RF_EventInternalError);

  if (urAdvHandle >= 0)
  {
    advTxStatus = UL_ADV_TX_SCHEDULED;
  }
  else
  {
    //ToDo: Once RF driver add the error status, use that instead
    //and one for failure case.
    advTxStatus = UL_ADV_TX_NO_RF_RESOURCE;
  }


#else /* RF Single Mode */
  urAdvHandle = RF_postCmd(urHandle, (RF_Op*) &urAdvCmd[0],
                           (RF_Priority) ubParams.rfPriority, ul_advDoneCb,
                           RF_EventInternalError);
#endif

  if (urAdvHandle >= 0)
  {
    if ((mode != UL_ADV_MODE_START) && ubParams.timeToAdv)
    {
      /* # of RAT ticks to the next Adv */
      uint32 ticksToAdv = urAdvCmd[0].startTime - RF_getCurrentTime();

      /* Is there enough time to issue "about to advertise" notification
       * before the next ADV happens?
       * Count 1 ms additionally to the required time for safety.
       */
      if (ticksToAdv >= ((uint32) ubParams.timeToAdv + 1) * MS_TO_RAT)
      {
        if (Clock_isActive(hcAdvAboutTo))
        {
          /* Stop clock first */
          Clock_stop(hcAdvAboutTo);
        }

        /* Convert RAT ticks to system ticks. */
        ticksToAdv /= SYSTICK_TO_RAT;

        /* Set the timeout */
        Clock_setTimeout(hcAdvAboutTo,
                         ticksToAdv - ubParams.timeToAdv * MS_TO_SYSTICK);

        /* Start clock instance */
        Clock_start(hcAdvAboutTo);
      }
    }

    return SUCCESS;
  }

#if defined(RF_MULTIMODE)
  return SUCCESS;
#else
  return FAILURE;
#endif
}

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
bStatus_t ul_advStart(void)
{
  bStatus_t status;

  /* Possible to enter UL_STATE_ADVERTISING from only UL_STATE_STANDBY */
  if (ulState != UL_STATE_STANDBY)
  {
    return FAILURE;
  }

  status = ul_advSchedule(UL_ADV_MODE_START);

  if (status == SUCCESS)
  {
    ulState = UL_STATE_ADVERTISING;
  }

  return status;
}

/*********************************************************************
 * @fn      ul_advStop
 *
 * @brief   Exit UL_STATE_ADVERTISING
 *
 * @param   none
 *
 * @return  none
 */
void ul_advStop(void)
{
  volatile uint32 keySwi;

  keySwi = Swi_disable();

  if (ulState == UL_STATE_ADVERTISING)
  {
    /* Cancel or stop ADV command */
    if (urAdvHandle > 0)
    {
      RF_cancelCmd(urHandle, urAdvHandle, 0);
      urAdvHandle = UR_CMD_HANDLE_INVALID;
    }

    /* Cancel AdvAboutTo timer */
    if (Clock_isActive(hcAdvAboutTo))
    {
      /* Stop clock first */
      Clock_stop(hcAdvAboutTo);
    }

#if defined(RF_MULTIMODE)
    /* cancel the interval timer */
    if (Clock_isActive(hcAdvInt))
    {
      /* stop the clock first */
      Clock_stop(hcAdvInt);
    }
#endif /* RF_MULTIMODE */

    ulState = UL_STATE_STANDBY;
  }

  Swi_restore(keySwi);
}
#endif /* FEATURE_ADVERTISER */

/*********************************************************************
 * @fn      ub_processLLMsg
 *
 * @brief   Process event messages sent from Micro BLE Stack to uLL through
 *          the application's Stack Event Proxy and ub_processMsg().
 *
 * @param   pEvtMsg - Pointer to the Micro BLE Stack event message destined to
 *                    uLL.
 *
 * @return  none
 */
void ub_processLLMsg(ubEvtMsg_t *pEvtMsg)
{
  switch (pEvtMsg->hdr.evt)
  {
#if defined(FEATURE_ADVERTISER)
  case ULL_EVT_ADV_TX_SUCCESS:
    /* Adv tx was successful */
    advTxStatus = UL_ADV_TX_DONE;

#if defined(RF_MULTIMODE)
    if (Clock_isActive(hcAdvInt))
    {
      /* stop the clock first */
      Clock_stop(hcAdvInt);
    }
#endif /* RF_MULTIMODE */

    ul_notifyAdvDone(SUCCESS);
    if (ulState == UL_STATE_ADVERTISING)
    {
      /* set the next advertisement */
      if (RF_TIME_CRITICAL == rfTimeCrit)
      {
        ul_advSchedule(UL_ADV_MODE_PERIODIC);
      }
      else
      {
        ul_advSchedule(UL_ADV_MODE_PERIODIC_RELAXED);
      }
    }
    break;

  case ULL_EVT_ADV_TX_FAILED:
    /* adv rf tx failure */
    advTxStatus = UL_ADV_TX_NO_RF_RESOURCE;
    ul_notifyAdvDone(bleNoResources);
    break;

  case ULL_EVT_ADV_TX_TIMER_EXPIRED:
    /* adv interval timer expired */
    advTxStatus = UL_ADV_TX_FAILED;
    ul_notifyAdvDone(FAILURE);
    if (ulState == UL_STATE_ADVERTISING)
    {
      if (RF_TIME_CRITICAL == rfTimeCrit)
      {
        ul_advSchedule(UL_ADV_MODE_PERIODIC);
      }
      else
      {
        /* Cancel or stop ADV command */
        if (urAdvHandle > 0)
        {
          RF_cancelCmd(urHandle, urAdvHandle, 0);
          urAdvHandle = UR_CMD_HANDLE_INVALID;
        }

        ul_advSchedule(UL_ADV_MODE_PERIODIC_RELAXED);
      }
    }
    break;

  case ULL_EVT_ADV_TX_RADIO_AVAILABLE:
    /* rf radio resource available */
    if (UL_ADV_TX_NO_RF_RESOURCE == advTxStatus)
    {
      /* check if there is time to reschedule the advertisement. */
      uint32_t currentTime = RF_getCurrentTime();
      if ((currentTime + 1 * MS_TO_RAT) < urAdvCmd[0].startTime)
      {
        /* reschedule the advertisement.*/
        ul_advSchedule(UL_ADV_MODE_RESCHEDULE);
      }
      else
      {
        if (RF_TIME_CRITICAL == rfTimeCrit)
        {
          advTxStatus = UL_ADV_TX_FAILED;
          ul_notifyAdvDone(FAILURE);
#if defined(RF_MULTIMODE)
          if (Clock_isActive(hcAdvInt))
          {
            /* stop the clock first */
            Clock_stop(hcAdvInt);
          }
#endif /* RF_MULTIMODE */
          if (ulState == UL_STATE_ADVERTISING)
          {
            ul_advSchedule(UL_ADV_MODE_PERIODIC);
          }
        }
        else /* relaxed operation */
        {
          /* past due, send it immediate */
          if (ulState == UL_STATE_ADVERTISING)
          {
            ul_advSchedule(UL_ADV_MODE_IMMEDIATE);
          }
        }
      }
    }
    break;
#endif /* FEATURE_ADVERTISER */

  default:
    break;
  }
}


/*********************************************************************
*********************************************************************/
