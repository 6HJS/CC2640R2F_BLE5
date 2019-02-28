/******************************************************************************

 @file       urfi.c

 @brief This file contains the RF driver interfacing API for the Micro
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

/*********************************************************************
 * INCLUDES
 */
#include <ti/sysbios/knl/Swi.h>
#include <driverlib/rf_mailbox.h>
#include <driverlib/rf_ble_cmd.h>
#include <driverlib/rf_ble_mailbox.h>
#include <ti/drivers/rf/RF.h>
#include <bcomdef.h>

#include <urfi.h>
#include <uble.h>
#include <ull.h>

/*********************************************************************
 * CONSTANTS
 */

#if (!defined(RF_SINGLEMODE) && !defined(RF_MULTIMODE)) ||                   \
    (defined(RF_SINGLEMODE) && defined(RF_MULTIMODE))
  #error "Either RF_SINGLEMODE or RF_MULTIMODE should be defined."
#endif /* RF_SINGLEMODE, RF_MULTIMODE */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

extern ubParams_t ubParams;
extern uint8      ubBDAddr[];
extern uint8      rfTimeCrit;

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
extern bStatus_t ub_buildAndPostEvt(ubEvtDst_t evtDst, ubEvt_t evt,
                                    ubMsg_t *pMsg, uint16 len);

/*********************************************************************
 * GLOBAL VARIABLES
 */

RF_Handle    urHandle = NULL;

/* Radio Setup Parameters.
 * config, txPower, and pRegOverride will be initialized at runtime.
 */
RF_RadioSetup urSetup =
{
  .common.commandNo                = CMD_RADIO_SETUP,
  .common.status                   = IDLE,
  .common.pNextOp                  = NULL,
  .common.startTime                = 0,
  .common.startTrigger.triggerType = TRIG_NOW,
  .common.startTrigger.bEnaCmd     = 0,
  .common.startTrigger.triggerNo   = 0,
  .common.startTrigger.pastTrig    = 0,
  .common.condition.rule           = COND_NEVER,
  .common.condition.nSkip          = 0,
  .common.mode                     = 0,
  .common.__dummy0                 = 0,
};

#if defined(FEATURE_ADVERTISER)
RF_CmdHandle urAdvHandle = UR_CMD_HANDLE_INVALID;

/* CMD_BLE_ADV_XX Params */
rfc_bleAdvPar_t urAdvParams =
{
  .pRxQ = 0,
  .rxConfig.bAutoFlushIgnored = 0,
  .rxConfig.bAutoFlushCrcErr = 0,
  .rxConfig.bAutoFlushEmpty = 0,
  .rxConfig.bIncludeLenByte = 0,
  .rxConfig.bIncludeCrc = 0,
  .rxConfig.bAppendRssi = 0,
  .rxConfig.bAppendStatus = 0,
  .rxConfig.bAppendTimestamp = 0,
  .advConfig.advFilterPolicy = 0,
  .advConfig.deviceAddrType = 0,
  .advConfig.peerAddrType = 0,
  .advConfig.bStrictLenFilter = 0,
  .advLen = 0,
  .scanRspLen = 0,
  .pAdvData = ubParams.advData,
#if defined(FEATURE_SCAN_RESPONSE)
  .pScanRspData = ubParams.scanRspData,
#else   /* FEATURE_SCAN_RESPONSE */
  .pScanRspData = 0,
#endif  /* FEATURE_SCAN_RESPONSE */
  .pDeviceAddress = (uint16*) ubBDAddr,
  .pWhiteList = 0,
  .__dummy0 = 0,
  .__dummy1 = 0,
  .endTrigger.triggerType = TRIG_NEVER,
  .endTrigger.bEnaCmd = 0,
  .endTrigger.triggerNo = 0,
  .endTrigger.pastTrig = 0,
  .endTime = 0,
};

#if defined(FEATURE_SCAN_RESPONSE)
/* CMD_BLE_ADV_XX Output */
rfc_bleAdvOutput_t urAdvOutput;
#endif  /* FEATURE_SCAN_RESPONSE */

/* CMD_BLE_ADV_XX Command */
rfc_CMD_BLE_ADV_COMMON_t urAdvCmd[3];

#endif  /* FEATURE_ADVERTISER */
#if defined(FEATURE_SCANNER)
RF_CmdHandle urScanHandle = UR_CMD_HANDLE_INVALID;
#endif  /* FEATURE_SCANNER */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static RF_Object urObject;
static RF_Params urParams;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      ur_initAdvCmd
 *
 * @brief   Initialize Adv RF command
 *
 * @param   None
 *
 * @return  None
 */
#if defined(FEATURE_ADVERTISER)
void ur_initAdvCmd(void)
{
  for (uint8 i = 0; i < 3; i++)
  {
    /* Advertising channel */
    urAdvCmd[i].channel                = 37 + i;

    urAdvCmd[i].whitening.init         = 0; /* No whitening */
    urAdvCmd[i].pParams                = &urAdvParams;

    urAdvCmd[i].startTrigger.bEnaCmd   = 0;

  #if defined(FEATURE_SCAN_RESPONSE)
    urAdvCmd[i].pOutput                = &urAdvOutput;
  #else  /* FEATURE_SCAN_RESPONSE */
    urAdvCmd[i].pOutput                = NULL;
  #endif /* FEATURE_SCAN_RESPONSE */
  }

  /* 1st channel adv is supposed to start at a certain time */
  urAdvCmd[0].startTrigger.triggerType = TRIG_ABSTIME;
  /* 2nd and 3rd channel adv's are supposed to start as soon as
     the previous channel operation ends */
  urAdvCmd[1].startTrigger.triggerType =
  urAdvCmd[2].startTrigger.triggerType = TRIG_NOW;

  urAdvCmd[1].startTime                =
  urAdvCmd[2].startTime                = 0;

  urAdvCmd[0].condition.rule           =
  urAdvCmd[1].condition.rule           = COND_STOP_ON_FALSE;
  urAdvCmd[2].condition.rule           = COND_NEVER;

#ifdef RF_MULTIMODE
  if (RF_TIME_RELAXED == rfTimeCrit)
  {
    urAdvCmd[0].startTrigger.pastTrig    =
    urAdvCmd[1].startTrigger.pastTrig    =
    urAdvCmd[2].startTrigger.pastTrig    = 1;
  }
  else
  {
    urAdvCmd[0].startTrigger.pastTrig    =
    urAdvCmd[1].startTrigger.pastTrig    =
    urAdvCmd[2].startTrigger.pastTrig    = 0;
  }
#endif /* RF_MULTIMODE */

  urAdvCmd[0].pNextOp                  = (rfc_radioOp_t*) &urAdvCmd[1];
  urAdvCmd[1].pNextOp                  = (rfc_radioOp_t*) &urAdvCmd[2];
  urAdvCmd[2].pNextOp                  = NULL;
}
#endif /* FEATURE_ADVERTISER */

/*********************************************************************
 * @fn      ur_powerCb
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
void ur_powerCb(RF_Handle rfHandle, RF_CmdHandle cmdHandle,
                RF_EventMask events)
{
  volatile uint32 keySwi;

  keySwi = Swi_disable();

  if (events & RF_EventRadioFree)
  {
    ub_buildAndPostEvt(UB_EVTDST_LL, ULL_EVT_ADV_TX_RADIO_AVAILABLE, NULL, 0);
  }

  Swi_restore(keySwi);
}

/*********************************************************************
 * @fn      ur_init
 *
 * @brief   Initialize radio interface and radio commands
 *
 * @param   None
 *
 * @return  SUCCESS - RF driver has been successfully opened
 *          FAILURE - Failed to open RF driver
 */
bStatus_t ur_init(void)
{
  if (urHandle == NULL)
  {
    RF_Params_init(&urParams); /* Get default values from RF driver */
    urParams.nInactivityTimeout = 0; /* Do not use the default value for this */
    urParams.pPowerCb = ur_powerCb;

    /* Setup radio setup command.
     * TODO: Make more variables user-configurable
     */
    /* Differential mode */
    urSetup.common.config.frontEndMode  = ubFeModeBias & 0x07;
    /* Internal bias */
    urSetup.common.config.biasMode      = (ubFeModeBias & 0x08) >> 3;
    /* Keep analog configuration */
    urSetup.common.config.analogCfgMode = 0x2D;
    /* Power up frequency synth */
    urSetup.common.config.bNoFsPowerUp  =    0;
    /* 0 dBm */
    urSetup.common.txPower            = ur_getTxPowerVal(UB_PARAM_DFLT_TXPOWER);
    /* Register Overrides */
    urSetup.common.pRegOverride       = (uint32_t*) ubRfRegOverride;

    /* Request access to the radio */
    urHandle = RF_open(&urObject, (RF_Mode*) &ubRfMode, &urSetup, &urParams);

    if (urHandle == NULL)
    {
      return FAILURE;
    }
  }

#if defined(FEATURE_ADVERTISER)
  ur_initAdvCmd();
#endif  /* FEATURE_ADVERTISER */

  return SUCCESS;
}

/*********************************************************************
 * CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      ur_getTxPowerVal
 *
 * @brief   Get the value, corresponding with the given TX Power,
 *          to be used to setup the radio accordingly.
 *
 * @param   txPower - TX Power in dBm.
 *
 * @return  The register value correspondign with txPower, if found.
 *          UB_TX_POWER_INVALID otherwise.
 */
uint16 ur_getTxPowerVal(int8 txPower)
{
  uint8 i;

  for (i = 0; i < ubTxPowerTable.numTxPowerVal; i++)
  {
    if (ubTxPowerTable.pTxPowerVals[i].dBm == txPower)
    {
      return ubTxPowerTable.pTxPowerVals[i].txPowerVal;
    }
  }

  return UB_TX_POWER_INVALID;
}


/*********************************************************************
*********************************************************************/
