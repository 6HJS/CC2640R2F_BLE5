/*
 * Copyright (c) 2016-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/***** Includes *****/

/* XDCtools Header files */
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/System.h>
#include <xdc/std.h>

/* TI-RTOS Header files */
#include <ti/drivers/rf/RF.h>
#include <ti/drivers/PIN.h>

/* Board Header files */
#include "Board.h"

/* Application specific Header files */
#include "menu.h"
#include "RFQueue.h"
#include "smartrf_settings/smartrf_settings.h"
#include "smartrf_settings/smartrf_settings_predefined.h"

#if (defined __CC2650DK_7ID_BOARD_H__) || (defined __CC2650_LAUNCHXL_BOARD_H__) || (defined __CC2640R2_LAUNCHXL_BOARD_H__) || (defined __CC1350_LAUNCHXL_BOARD_H__) || (defined __CC1350STK_BOARD_H__) || (defined __CC26X2R1_LAUNCHXL_BOARD_H__) || (defined __CC1352R1_LAUNCHXL_BOARD_H__)
#include "smartrf_settings/smartrf_settings_ble.h"
#endif

/***** Defines *****/
#define DATA_ENTRY_HEADER_SIZE  8   // Constant header size of a Generic Data Entry
#define MAX_LENGTH              125 // Max length byte the radio will accept
#define NUM_DATA_ENTRIES        2   // NOTE: Only two data entries supported at the moment
#define NUM_APPENDED_BYTES      2   // RF_cmdRxHS.rxConf.bIncludeLen = 1: Include the received length
                                    // field (2 bytes) in the stored packet


/***** Prototypes *****/
static void rx_callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);

/***** Variable declarations *****/
static uint8_t packetReceived = false;
static uint16_t* crcOk;
static int8_t* rssi;

static RF_Object rfObject;
static RF_Handle rfHandle;
static RF_CmdHandle rxCmdHndl = 0; // Handle needed to abot the RX command

/*
Buffer which contains all Data Entries for receiving data.
Pragmas are needed to make sure this buffer is 4 byte aligned (requirement from the RF Core)
*/
#if defined(__TI_COMPILER_VERSION__)
    #pragma DATA_ALIGN (rxDataEntryBuffer, 4);
        static uint8_t rxDataEntryBuffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES,
                                                                        MAX_LENGTH,
                                                                        NUM_APPENDED_BYTES)];
#elif defined(__IAR_SYSTEMS_ICC__)
    #pragma data_alignment = 4
        static uint8_t rxDataEntryBuffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES,
                                                                        MAX_LENGTH,
                                                                        NUM_APPENDED_BYTES)];
#elif defined(__GNUC__)
        static uint8_t rxDataEntryBuffer [RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES,
                                                                        MAX_LENGTH,
                                                                        NUM_APPENDED_BYTES)] __attribute__ ((aligned (4)));
#else
    #error This compiler is not supported.
#endif

/* Receive queue for the RF Code to fill in data */
static dataQueue_t dataQueue;

/* General data entry structure (type = 0) */
rfc_dataEntryGeneral_t* currentDataEntry;

#if !(defined __CC2650DK_7ID_BOARD_H__) && !(defined __CC2650_LAUNCHXL_BOARD_H__) && !(defined __CC2640R2_LAUNCHXL_BOARD_H__) && !(defined __CC1352R1_LAUNCHXL_BOARD_H__) && !(defined __CC26X2R1_LAUNCHXL_BOARD_H__)
rfc_hsRxOutput_t rxStatistics_hs; // Output structure for CMD_HS_RX
#endif

#if (defined __CC2650DK_7ID_BOARD_H__) || (defined __CC2650_LAUNCHXL_BOARD_H__) || (defined __CC2640R2_LAUNCHXL_BOARD_H__) || (defined __CC1350_LAUNCHXL_BOARD_H__) || (defined __CC1350STK_BOARD_H__) || (defined __CC26X2R1_LAUNCHXL_BOARD_H__) || (defined __CC1352R1_LAUNCHXL_BOARD_H__)
rfc_bleGenericRxOutput_t rxStatistics_ble; // Output structure for RF_ble_cmdBleGenericRx
#endif

rfc_propRxOutput_t rxStatistics_prop; // Output structure for CMD_PROP_RX

/* Runs the receiving part of the test application and returns a result */
TestResult rx_runRxTest(const ApplicationConfig* config)
{
    Assert_isTrue(config != NULL, NULL);

    RF_Params rfParams;
    RF_Params_init(&rfParams);

    if( RFQueue_defineQueue(&dataQueue,
                            rxDataEntryBuffer,
                            sizeof(rxDataEntryBuffer),
                            NUM_DATA_ENTRIES,
                            MAX_LENGTH + NUM_APPENDED_BYTES))
    {
        /* Failed to allocate space for all data entries */
        while(true);
    }

    RF_cmdPropRx.pQueue = &dataQueue;
    RF_cmdPropRx.pOutput = (uint8_t*)&rxStatistics_prop;
    RF_cmdPropRx.maxPktLen = MAX_LENGTH;
    RF_cmdPropRx.pktConf.bRepeatOk = 1;
    RF_cmdPropRx.pktConf.bRepeatNok = 1;
    RF_cmdPropRx.rxConf.bAutoFlushCrcErr = 1;
    RF_cmdPropRx.rxConf.bAutoFlushIgnored = 1;

#if !(defined __CC2650DK_7ID_BOARD_H__) && !(defined __CC2650_LAUNCHXL_BOARD_H__) && !(defined __CC2640R2_LAUNCHXL_BOARD_H__) && !(defined __CC1352R1_LAUNCHXL_BOARD_H__) && !(defined __CC26X2R1_LAUNCHXL_BOARD_H__)
    RF_pCmdRxHS->pOutput = &rxStatistics_hs;
    RF_pCmdRxHS->pQueue = &dataQueue;
    RF_pCmdRxHS->maxPktLen = MAX_LENGTH;
    RF_pCmdRxHS->pktConf.bRepeatOk = 1;
    RF_pCmdRxHS->pktConf.bRepeatNok = 1;
    RF_pCmdRxHS->rxConf.bAutoFlushCrcErr = 1;
#endif

#if (defined __CC2650DK_7ID_BOARD_H__) || (defined __CC2650_LAUNCHXL_BOARD_H__) || (defined __CC2640R2_LAUNCHXL_BOARD_H__) || (defined __CC1350_LAUNCHXL_BOARD_H__) || (defined __CC1350STK_BOARD_H__) || (defined __CC26X2R1_LAUNCHXL_BOARD_H__) || (defined __CC1352R1_LAUNCHXL_BOARD_H__)
    RF_ble_pCmdBleGenericRx->pOutput = &rxStatistics_ble;
    RF_ble_pCmdBleGenericRx->pParams->pRxQ = &dataQueue;
    RF_ble_pCmdBleGenericRx->pParams->bRepeat = 1;
    RF_ble_pCmdBleGenericRx->pParams->rxConfig.bAutoFlushCrcErr = 1;
    RF_ble_pCmdBleGenericRx->channel = 0xFF;
    RF_ble_pCmdBleGenericRx->whitening.bOverride = 1;
    RF_ble_pCmdBleGenericRx->whitening.init = config->frequencyTable[config->frequency].whitening;
#endif

#if (defined __CC1310_LAUNCHXL_BOARD_H__)

    /* Modify Setup command and TX Power depending on using Range Extender or not
     * Using CC1310 + CC1190 can only be done for the following PHYs:
     * fsk (50 kbps, 2-GFSK)
     * lrm (Legacy Long Range)
     * sl_lr (SimpleLink Long Range) */

    if (config->rangeExtender == RangeExtender_Dis)
    {
        /* Settings used for the CC1310 LAUNCHXL */
        RF_pCmdPropRadioDivSetup_fsk->txPower   = 0xA73A;
        RF_pCmdPropRadioDivSetup_lrm->txPower   = 0xA73A;
        RF_pCmdPropRadioDivSetup_sl_lr->txPower = 0xA73A;
        {
            uint8_t i = 0;
            do
            {
                if ((pOverrides_fsk[i] & 0x0000FFFF) ==  0x000088A3)
                {
                    pOverrides_fsk[i] = (uint32_t)0x00FB88A3;
                }
            } while ((pOverrides_fsk[i++] != 0xFFFFFFFF));

            i = 0;
            do
            {
                if ((pOverrides_lrm[i] & 0x0000FFFF) ==  0x000088A3)
                {
                    pOverrides_lrm[i] = (uint32_t)0x00FB88A3;
                }
            } while ((pOverrides_lrm[i++] != 0xFFFFFFFF));

            i = 0;
            do
            {
                if ((pOverrides_sl_lr[i] & 0x0000FFFF) ==  0x000088A3)
                {
                    pOverrides_sl_lr[i] = (uint32_t)0x00FB88A3;
                }
            } while ((pOverrides_sl_lr[i++] != 0xFFFFFFFF));
        }
    }
    else
    {
        /* Settings used for the CC1310 CC1190 LAUNCHXL */
        if(config->frequencyTable[config->frequency].frequency == 0x0364) // 868 MHz
        {
            RF_pCmdPropRadioDivSetup_fsk->txPower   = 0x00C6;
            RF_pCmdPropRadioDivSetup_lrm->txPower   = 0x00C6;
            RF_pCmdPropRadioDivSetup_sl_lr->txPower = 0x00C6;
            {
                uint8_t i = 0;
                do
                {
                    if ((pOverrides_fsk[i] & 0x0000FFFF) ==  0x000088A3)
                    {
                        pOverrides_fsk[i] = (uint32_t)0x000188A3;
                    }
                } while ((pOverrides_fsk[i++] != 0xFFFFFFFF));

                i = 0;
                do
                {
                    if ((pOverrides_lrm[i] & 0x0000FFFF) ==  0x000088A3)
                    {
                        pOverrides_lrm[i] = (uint32_t)0x000188A3;
                    }
                } while ((pOverrides_lrm[i++] != 0xFFFFFFFF));

                i = 0;
                do
                {
                    if ((pOverrides_sl_lr[i] & 0x0000FFFF) ==  0x000088A3)
                    {
                        pOverrides_sl_lr[i] = (uint32_t)0x000188A3;
                    }
                } while ((pOverrides_sl_lr[i++] != 0xFFFFFFFF));
            }
        }
        else if(config->frequencyTable[config->frequency].frequency == 0x0393) // 915 MHz
        {
            RF_pCmdPropRadioDivSetup_fsk->txPower   = 0x00C9;
            RF_pCmdPropRadioDivSetup_lrm->txPower   = 0x00C9;
            RF_pCmdPropRadioDivSetup_sl_lr->txPower = 0x00C9;
            {
                uint8_t i = 0;
                do
                {
                    if ((pOverrides_fsk[i] & 0x0000FFFF) ==  0x000088A3)
                    {
                        pOverrides_fsk[i] = (uint32_t)0x000388A3;
                    }
                } while ((pOverrides_fsk[i++] != 0xFFFFFFFF));

                i = 0;
                do
                {
                    if ((pOverrides_lrm[i] & 0x0000FFFF) ==  0x000088A3)
                    {
                        pOverrides_lrm[i] = (uint32_t)0x000388A3;
                    }
                } while ((pOverrides_lrm[i++] != 0xFFFFFFFF));

                i = 0;
                do
                {
                    if ((pOverrides_sl_lr[i] & 0x0000FFFF) ==  0x000088A3)
                    {
                        pOverrides_sl_lr[i] = (uint32_t)0x000388A3;
                    }
                } while ((pOverrides_sl_lr[i++] != 0xFFFFFFFF));
            }
        }
    }
#endif

    /* Request access to the radio based on test case */
    switch (config->rfSetup)
    {
        case RfSetup_Custom:

            rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams);
            break;

        case RfSetup_Fsk:

#if !(defined __CC2650DK_7ID_BOARD_H__) && !(defined __CC2650_LAUNCHXL_BOARD_H__) && !(defined __CC2640R2_LAUNCHXL_BOARD_H__) && !(defined __CC26X2R1_LAUNCHXL_BOARD_H__)
            RF_pCmdPropRadioDivSetup_fsk->centerFreq = config->frequencyTable[config->frequency].frequency;
            rfHandle = RF_open(&rfObject, RF_pProp_fsk, (RF_RadioSetup*)RF_pCmdPropRadioDivSetup_fsk, &rfParams);
#else
            rfHandle = RF_open(&rfObject, RF_pProp_2_4G_fsk, (RF_RadioSetup*)RF_pCmdPropRadioSetup_2_4G_fsk, &rfParams);
#endif
            break;

#if !(defined __CC2650DK_7ID_BOARD_H__) && !(defined __CC2650_LAUNCHXL_BOARD_H__) && !(defined __CC2640R2_LAUNCHXL_BOARD_H__) && !(defined __CC26X2R1_LAUNCHXL_BOARD_H__)
        case RfSetup_Lrm:

            RF_pCmdPropRadioDivSetup_lrm->centerFreq = config->frequencyTable[config->frequency].frequency;
            rfHandle = RF_open(&rfObject, RF_pProp_lrm, (RF_RadioSetup*)RF_pCmdPropRadioDivSetup_lrm, &rfParams);
            break;

        case RfSetup_Sl_lr:

            RF_pCmdPropRadioDivSetup_sl_lr->centerFreq = config->frequencyTable[config->frequency].frequency;
            rfHandle = RF_open(&rfObject, RF_pProp_sl_lr, (RF_RadioSetup*)RF_pCmdPropRadioDivSetup_sl_lr, &rfParams);
            break;

        case RfSetup_Ook:

            RF_pCmdPropRadioDivSetup_ook->centerFreq = config->frequencyTable[config->frequency].frequency;
            rfHandle = RF_open(&rfObject, RF_pProp_ook, (RF_RadioSetup*)RF_pCmdPropRadioDivSetup_ook, &rfParams);
            break;

#if !(defined __CC1352R1_LAUNCHXL_BOARD_H__) && !(defined __CC26X2R1_LAUNCHXL_BOARD_H__)
        case RfSetup_Hsm:

            rfHandle = RF_open(&rfObject, RF_pProp_hsm, (RF_RadioSetup*)RF_pCmdRadioSetup_hsm, &rfParams);
            break;
#endif
#endif

#if (defined __CC2650DK_7ID_BOARD_H__) || (defined __CC2650_LAUNCHXL_BOARD_H__) || (defined __CC2640R2_LAUNCHXL_BOARD_H__) || (defined __CC1350_LAUNCHXL_BOARD_H__) || (defined __CC1350STK_BOARD_H__) || (defined __CC26X2R1_LAUNCHXL_BOARD_H__) || (defined __CC1352R1_LAUNCHXL_BOARD_H__)
        case RfSetup_Ble:

            rfHandle = RF_open(&rfObject, RF_pModeBle, (RF_RadioSetup*)RF_ble_pCmdRadioSetup, &rfParams);
            break;
#endif

        default:
            break;
    }

    /* Set the frequency */
    if(config->rfSetup == RfSetup_Custom)
    {
        /* Custom settings exported from SmartRf studio shall use the exported frequency */
        RF_runCmd(rfHandle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);
    }

#if (defined __CC2650DK_7ID_BOARD_H__) || (defined __CC2650_LAUNCHXL_BOARD_H__) || (defined __CC2640R2_LAUNCHXL_BOARD_H__) || (defined __CC1350_LAUNCHXL_BOARD_H__) || (defined __CC1350STK_BOARD_H__) || (defined __CC26X2R1_LAUNCHXL_BOARD_H__) || (defined __CC1352R1_LAUNCHXL_BOARD_H__)
    else if(config->rfSetup == RfSetup_Ble)
    {
        RF_ble_pCmdFs->frequency = config->frequencyTable[config->frequency].frequency;
        RF_ble_pCmdFs->fractFreq = config->frequencyTable[config->frequency].fractFreq;
        RF_runCmd(rfHandle, (RF_Op*)RF_ble_pCmdFs, RF_PriorityNormal, NULL, 0);
    }
#endif
    else
    {
        RF_pCmdFs_preDef->frequency = config->frequencyTable[config->frequency].frequency;
        RF_pCmdFs_preDef->fractFreq = config->frequencyTable[config->frequency].fractFreq;
        RF_runCmd(rfHandle, (RF_Op*)RF_pCmdFs_preDef, RF_PriorityNormal, NULL, 0);
    }

    /* Enter RX mode and stay forever in RX */
    switch (config->rfSetup)
    {
#if !(defined __CC2650DK_7ID_BOARD_H__) && !(defined __CC2650_LAUNCHXL_BOARD_H__) && !(defined __CC2640R2_LAUNCHXL_BOARD_H__) && !(defined __CC1352R1_LAUNCHXL_BOARD_H__) && !(defined __CC26X2R1_LAUNCHXL_BOARD_H__)
        case RfSetup_Hsm:

            rxCmdHndl = RF_postCmd(rfHandle, (RF_Op*)RF_pCmdRxHS, RF_PriorityNormal, &rx_callback, RF_EventRxEntryDone);
            crcOk = &rxStatistics_hs.nRxOk;
            rssi = &rxStatistics_hs.lastRssi;
            break;
#endif
#if (defined __CC2650DK_7ID_BOARD_H__) || (defined __CC2650_LAUNCHXL_BOARD_H__) || (defined __CC2640R2_LAUNCHXL_BOARD_H__) || (defined __CC1350_LAUNCHXL_BOARD_H__) || (defined __CC1350STK_BOARD_H__) || (defined __CC26X2R1_LAUNCHXL_BOARD_H__) || (defined __CC1352R1_LAUNCHXL_BOARD_H__)
        case RfSetup_Ble:

            rxCmdHndl = RF_postCmd(rfHandle, (RF_Op*)RF_ble_pCmdBleGenericRx, RF_PriorityNormal, &rx_callback, RF_EventRxEntryDone);
            crcOk = &rxStatistics_ble.nRxOk;
            rssi = &rxStatistics_ble.lastRssi;
            break;
#endif

        default:

            rxCmdHndl = RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropRx, RF_PriorityNormal, &rx_callback, RF_EventRxEntryDone);
            crcOk = &rxStatistics_prop.nRxOk;
            rssi = &rxStatistics_prop.lastRssi;
            break;
    }

    *crcOk = 0;
    *rssi = 0;
    while(true)
    {
        if(packetReceived)
        {
            packetReceived = false;
            menu_updateRxScreen(*crcOk, *rssi);
        }

        if (menu_isButtonPressed())
        {
            /* Force abort gracefully */
            RF_cancelCmd(rfHandle, rxCmdHndl, 0);
            RF_pendCmd(rfHandle, rxCmdHndl, RF_EventRxEntryDone);
            RF_close(rfHandle);
            return TestResult_Aborted;
        }
    }
}

void rx_callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
    if (e & RF_EventRxEntryDone)
    {
        /* Get current unhandled data entry */
        currentDataEntry = RFQueue_getDataEntry();

        /* For TC_HSM, the packet length and a pointer to the first byte in the payload can be found as follows:
         *
         * uint8_t packetLength      = ((*(uint8_t*)(&currentDataEntry->data + 1)) << 8) | (*(uint8_t*)(&currentDataEntry->data));
         * uint8_t* packetDataPointer = (uint8_t*)(&currentDataEntry->data + 2);
         *
         * For the other test cases (TC_LRM, TC_OOK and TC_FSK), the packet length and first payload byte is found here:
         *
         * uint8_t packetLength      = *(uint8_t*)(&currentDataEntry->data);
         * uint8_t* packetDataPointer = (uint8_t*)(&currentDataEntry->data + 1);
         */
        packetReceived = true;
        RFQueue_nextEntry();
    }
}
