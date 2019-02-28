/******************************************************************************

 @file       ugap.c

 @brief This file contains the Micro GAP Initialization and Configuration APIs.

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
#include <string.h>

#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Swi.h>

#include <icall.h>

#include "bcomdef.h"
#include "util.h"

#include "uble.h"
#include "ull.h"
#include "ugap.h"

#if defined(FEATURE_BROADCASTER) && !defined(FEATURE_ADVERTISER)
  #error "FEATURE_ADVERTISER should also be defined if FEATURE_BROADCASTER \
is defined."
#endif /* FEATURE_BROADCASTER && !FEATURE_ADVERTISER */
#if defined(FEATURE_OBSERVER) && !defined(FEATURE_SCANNER)
  #error "FEATURE_SCANNER should also be defined if FEATURE_OBSERVER \
is defined."
#endif /* FEATURE_OBSERVER && !FEATURE_SCANNER */
#if !defined(FEATURE_BROADCASTER) && !defined(FEATURE_OBSERVER)
  #error "At least one GAP role feature should be defined. Currently only \
FEATURE_BROADCASTER is available."
#endif /* !FEATURE_BROADCASTER && !FEATURE_OBSERVER */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
// Clock Event
#define UG_CLKEVT_BCAST_DUTY          0x0001

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */
extern ubParams_t ubParams;

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
extern bStatus_t ub_buildAndPostEvt(ubEvtDst_t evtDst, ubEvt_t evt,
                                    ubMsg_t *pMsg, uint16 len);

/*********************************************************************
 * LOCAL VARIABLES
 */

/* Micro GAP Broadcaster Role State */
static ugBcastState_t ugbState = UG_BCAST_STATE_INVALID;

#if defined(FEATURE_BROADCASTER)
/* Clock for Broadcaster Duty Control */
Clock_Struct cBcastDuty;
Clock_Handle hcBcastDuty = NULL;

/* Set of application callbacks to be issued by Broadcaster */
static ugBcastCBs_t ugbAppCBs;

/*********************************************************************
 * Broadcaster Parameters
 */
static uint16 ugbNumAdvEvent;
static uint16 ugbDutyOnTime = 0;
static uint16 ugbDutyOffTime = 0; /* Duty control is off by default */
#endif /* FEATURE_BROADCASTER */

/*********************************************************************
 * Observer Parameters
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      ugb_changeState
 *
 * @brief   Change Broadcaster State. If successful, post UGB_EVT_STATE_CHANGE
 *          event.
 *
 * @param   state - the new state to switch to
 *
 * @return  SUCCESS, INVALIDPARAMETER, FAILURE
 */
static bStatus_t ugb_changeState(ugBcastState_t state)
{
  volatile uint32 keySwi;
  bStatus_t status = SUCCESS;

  keySwi = Swi_disable();

  if (ugbState == state)
  {
    /* Caller is attempting to switch to the same state. Just return
       without posting UGB_EVT_STATE_CHANGE */
    Swi_restore(keySwi);
    return status;
  }

  switch (state)
  {
  case UG_BCAST_STATE_INITIALIZED:
    ugbState = state;
    break;

  case UG_BCAST_STATE_IDLE:
    if (Clock_isActive(hcBcastDuty))
    {
      Clock_stop(hcBcastDuty);
    }

    if (ugbState == UG_BCAST_STATE_ADVERTISING)
    {
      ul_advStop();
    }

    ugbState = state;
    break;

  case UG_BCAST_STATE_ADVERTISING:
    status = ul_advStart();
    if (status == SUCCESS)
    {
      /* Set the timeout for duty on time only if duty control is enabled */
      if (ugbDutyOnTime != 0 && ugbDutyOffTime != 0)
      {
        Clock_setTimeout(hcBcastDuty,
                         ugbDutyOnTime * UG_DUTY_TIME_UNIT * MS_TO_SYSTICK);
        Clock_start(hcBcastDuty);
      }

      ugbState = state;
    }
    break;

  case UG_BCAST_STATE_WAITING:
    ugbState = state;
    /* Set the timeout for duty off time */
    Clock_setTimeout(hcBcastDuty,
                     ugbDutyOffTime * UG_DUTY_TIME_UNIT * MS_TO_SYSTICK);
    Clock_start(hcBcastDuty);

    ul_advStop();
    break;

  case UG_BCAST_STATE_SUSPENDED:
    /* TBD */
    break;

  default:
    status = INVALIDPARAMETER;
    break;
  }

  if (status == SUCCESS)
  {
    ugbMsgStateChange_t msgStateChange;

    msgStateChange.state = ugbState;
    ub_buildAndPostEvt(UB_EVTDST_GAP, UGB_EVT_STATE_CHANGE,
                       (ubMsg_t*) &msgStateChange, sizeof(msgStateChange));
  }

  Swi_restore(keySwi);

  return status;
}

/*********************************************************************
 * CALLBACKS
 */

/*********************************************************************
 * @fn      ugb_clockHandler
 *
 * @brief   Handles a clock event.
 *
 * @param   a0 - event
 *
 * @return  None
 */
void ug_clockHandler(UArg a0)
{
#if defined(FEATURE_BROADCASTER)
  /* Process Broadcaster Duty Control */
  if (a0 == UG_CLKEVT_BCAST_DUTY)
  {
    if (ugbState == UG_BCAST_STATE_ADVERTISING)
    {
      /* ugbDutyOffTime could have been changed during advertising state,
         so should be checked here. */
      if (ugbDutyOffTime != 0)
      {
        if (ugb_changeState(UG_BCAST_STATE_WAITING) != SUCCESS)
        {
          /* What to do if failed? */
        }
      }
    }
    else /* if (ugbState == UG_BCAST_STATE_WAITING) */
    {
      if (ugb_changeState(UG_BCAST_STATE_ADVERTISING) != SUCCESS)
      {
        /* What to do if failed? */
      }
    }
  }
#endif /* FEATURE_BROADCASTER */
}

#if defined(FEATURE_BROADCASTER)
/*********************************************************************
 * @fn      ugb_advPrepareCB
 *
 * @brief   Do pre-processing of advertising event
 *
 * @param   None
 *
 * @return  None
 */
void ugb_advPrepareCB(void)
{
  if (ubParams.timeToAdv > 0)
  {
    /* Post UGB_EVT_ADV_PREPARE to itself so that it is processed
       in the application task's context */
    ub_buildAndPostEvt(UB_EVTDST_GAP, UGB_EVT_ADV_PREPARE, NULL, 0);
  }
}

/*********************************************************************
 * @fn      ugb_advPostprocessCB
 *
 * @brief   Do post-processing of advertising event
 *
 * @param   status - result of the last advertising event
 *
 * @return  none
 */
void ugb_advPostprocessCB(bStatus_t status)
{
  ugbMsgAdvPostprocess_t msg;
#ifndef RF_MULTIMODE
  if (status == bleNoResources)
  {
    /* Failed to schedule next advertising event. Switch the state to Idle. */
    ugb_changeState(UG_BCAST_STATE_IDLE);
  }
  else
#endif /* !RF_MULTIMODE */
  {
    msg.status = status;
    /* Post UGB_EVT_ADV_POSTPROCESS to itself so that it is processed
       in the application task's context */
    ub_buildAndPostEvt(UB_EVTDST_GAP, UGB_EVT_ADV_POSTPROCESS,
                       (ubMsg_t*) &msg, sizeof(ugbMsgAdvPostprocess_t));

    if (ugbNumAdvEvent > 0)
    {
      if (--ugbNumAdvEvent == 0)
      {
        ugb_changeState(UG_BCAST_STATE_IDLE);
      }
    }
  }
}
#endif /* FEATURE_BROADCASTER */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

#if defined(FEATURE_BROADCASTER)
/*********************************************************************
 * @fn      ug_bcastInit
 *
 * @brief   Initialize Micro Broadcaster
 *
 * @param   pCBs - a set of application callbacks for Broadcaster
 *
 * @return  SUCCESS, INVALIDPARAMETER, FAILURE
 */
bStatus_t ug_bcastInit(ugBcastCBs_t* pCBs)
{
  if (ugbState != UG_BCAST_STATE_INVALID &&
      ugbState != UG_BCAST_STATE_INITIALIZED &&
      ugbState != UG_BCAST_STATE_IDLE)
  {
    /* Cannot re-initialize if the state is one of the active ones */
    return FAILURE;
  }

  if (hcBcastDuty == NULL)
  {
    Clock_Params clockParams;

    /* Setup BcastDuty timer as a one-shot timer */
    Clock_Params_init(&clockParams);
    clockParams.arg = UG_CLKEVT_BCAST_DUTY;
    clockParams.period = 0;
    clockParams.startFlag = false;
    Clock_construct(&cBcastDuty, ug_clockHandler, 0, &clockParams);
    hcBcastDuty = Clock_handle(&cBcastDuty);
  }

  if (pCBs != NULL)
  {
    /* Register application callbacks to be called by Broadcaster */
    memcpy(&ugbAppCBs, pCBs, sizeof(ugBcastCBs_t));
  }

  /* Register advertiser-related uGAP callback in the uLL */
  ul_advRegisterCB(ugb_advPrepareCB, ugb_advPostprocessCB);

  return ugb_changeState(UG_BCAST_STATE_INITIALIZED);
}

/*********************************************************************
 * @fn      ug_bcastSetDuty
 *
 * @brief   Set Broadcaster Duty On/Off time
 *
 * @param   dutyOnTime  - Time period during which the Broadcaster
 *                        is in Advertising state. 100 ms unit. This cannot be
 *                        0 unless dutyOffTime is also 0.
 *          dutyOffTime - Time period during which the Broadcaster
 *                        is in Waiting state. 100 ms unit. If this is 0,
 *                        Duty Control is disabled regardless of dutyOnTime.
 *
 * @return  SUCCESS or INVALIDPARAMETER
 */
bStatus_t ug_bcastSetDuty(uint16 dutyOnTime, uint16 dutyOffTime)
{
  if (dutyOnTime == 0 && dutyOffTime != 0)
  {
    return INVALIDPARAMETER;
  }

  ugbDutyOnTime = dutyOnTime;

  if (ugbState == UG_BCAST_STATE_ADVERTISING &&
      ugbDutyOffTime == 0 && dutyOffTime != 0)
  {
    /* If duty control gets enabled while advertising,
       start from WAITING state */
    ugbDutyOffTime = dutyOffTime;
    ugb_changeState(UG_BCAST_STATE_WAITING);
  }
  else
  {
    ugbDutyOffTime = dutyOffTime;
  }

  return SUCCESS;
}

/*********************************************************************
 * @fn      ug_bcastStart
 *
 * @brief   Start Broadcaster. Proceed the state from either Initialized or IDLE
 *          to Advertising.
 *
 * @param   numAdvEvent - # of Advertising events to be performed before
 *                        the Broadcaster state goes to IDLE. If this is 0,
 *                        the Broadcaster will keep staying in Advertising or
 *                        Advertising and Waiting alternately unless an error
 *                        happens or the application requests to stop.
 *
 * @return  SUCCESS, FAILURE, or INVALIDPARAMETER
 */
bStatus_t ug_bcastStart(uint16 numAdvEvent)
{
  /* Micro Broadcaster can start only from UG_BCAST_STATE_INITIALIZED state */
  if (ugbState != UG_BCAST_STATE_INITIALIZED && ugbState != UG_BCAST_STATE_IDLE)
  {
    return FAILURE;
  }

  ugbNumAdvEvent = numAdvEvent;

  return ugb_changeState(UG_BCAST_STATE_ADVERTISING);
}

/*********************************************************************
 * @fn      ug_bcastStop
 *
 * @brief   Stop Broadcaster. Proceed the state from either Advertising or
 *          Waiting to IDLE.
 *
 * @return  SUCCESS, FAILURE, or INVALIDPARAMETER
 */
bStatus_t ug_bcastStop(void)
{
  /* Micro Broadcaster should have ever been initialized */
  if (ugbState == UG_BCAST_STATE_INVALID ||
      ugbState == UG_BCAST_STATE_INITIALIZED)
  {
    return FAILURE;
  }

  return ugb_changeState(UG_BCAST_STATE_IDLE);
}
#endif /* FEATURE_BROADCASTER */

/*********************************************************************
 * @fn      ug_processGAPMsg
 *
 * @brief   Process event messages sent from Micro BLE Stack to uGAP through
 *          the application's Stack Event Proxy and ub_processMsg().
 *
 * @param   pEvtMsg - Pointer to the Micro BLE Stack event message destined to
 *                    uGAP.
 *
 * @return  none
 */
void ub_processGAPMsg(ubEvtMsg_t *pEvtMsg)
{
  switch (pEvtMsg->hdr.evt)
  {
#if defined(FEATURE_BROADCASTER)
  case UGB_EVT_ADV_PREPARE:
    if (ugbAppCBs.pfnAdvPrepareCB != NULL)
    {
      ugbAppCBs.pfnAdvPrepareCB();
    }
    break;

  case UGB_EVT_ADV_POSTPROCESS:
    if (ugbAppCBs.pfnAdvDoneCB != NULL)
    {
      ugbMsgAdvPostprocess_t *pMsg = (ugbMsgAdvPostprocess_t*) &(pEvtMsg->msg);
      ugbAppCBs.pfnAdvDoneCB(pMsg->status);
    }
    break;
#endif /* FEATURE_BROADCASTER */

  case UGB_EVT_STATE_CHANGE:
    if (ugbAppCBs.pfnStateChangeCB != NULL)
    {
      ugbMsgStateChange_t *pMsg = (ugbMsgStateChange_t*) &(pEvtMsg->msg);
      ugbAppCBs.pfnStateChangeCB(pMsg->state);
    }
    break;

  default:
    break;
  }
}


/*********************************************************************
*********************************************************************/
