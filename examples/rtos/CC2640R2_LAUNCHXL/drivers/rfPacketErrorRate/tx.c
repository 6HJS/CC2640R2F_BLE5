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
#include <stdlib.h>
#include "menu.h"
#include "RFQueue.h"
#include "smartrf_settings/smartrf_settings.h"
#include "smartrf_settings/smartrf_settings_predefined.h"

#if (defined __CC2650DK_7ID_BOARD_H__) || (defined __CC2650_LAUNCHXL_BOARD_H__) || (defined __CC2640R2_LAUNCHXL_BOARD_H__) || (defined __CC1350_LAUNCHXL_BOARD_H__) || (defined __CC1350STK_BOARD_H__) || (defined __CC1352R1_LAUNCHXL_BOARD_H__) || (defined __CC26X2R1_LAUNCHXL_BOARD_H__)
#include "smartrf_settings/smartrf_settings_ble.h"
#endif

/***** Defines *****/
#define PAYLOAD_LENGTH          30  // Length of the packet to send
#define DATA_ENTRY_HEADER_SIZE  8   // Constant header size of a Generic Data Entry
#define MAX_LENGTH              125 // Set the length of the data entry
#define NUM_DATA_ENTRIES        1
#define NUM_APPENDED_BYTES      0

/***** Variable declarations *****/
static RF_Object rfObject;
static RF_Handle rfHandle;

static uint8_t packet[PAYLOAD_LENGTH];
static uint16_t seqNumber;
uint32_t packetCounter = 0;

/*
This interval is dependant on data rate and packet length, and migh need to be changed
if any of these parameter changes
*/
uint32_t packetInterval;

#if defined(__TI_COMPILER_VERSION__)
    #pragma DATA_ALIGN (txDataEntryBuffer, 4);
        static uint8_t txDataEntryBuffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES,
                                                                        MAX_LENGTH,
                                                                        NUM_APPENDED_BYTES)];
#elif defined(__IAR_SYSTEMS_ICC__)
    #pragma data_alignment = 4
        static uint8_t txDataEntryBuffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES,
                                                                        MAX_LENGTH,
                                                                        NUM_APPENDED_BYTES)];
#elif defined(__GNUC__)
        static uint8_t txDataEntryBuffer [RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES,
                                                                        MAX_LENGTH,
                                                                        NUM_APPENDED_BYTES)] __attribute__ ((aligned (4)));
#else
    #error This compiler is not supported.
#endif

/* TX queue or RF Core to read data from */
static dataQueue_t dataQueue;
static rfc_dataEntryGeneral_t* currentDataEntry;
static uint8_t *pPacket;

/* Runs the transmitting part of the test application and returns a result. */
TestResult tx_runTxTest(const ApplicationConfig* config)
{
    Assert_isTrue(config != NULL, NULL);
    uint32_t time;
    RF_Params rfParams;
    RF_Params_init(&rfParams);

    RF_cmdPropTx.pktLen = PAYLOAD_LENGTH;
    RF_cmdPropTx.pPkt = packet;
    RF_cmdPropTx.startTrigger.triggerType = TRIG_ABSTIME;
    RF_cmdPropTx.startTrigger.pastTrig = 1;
    RF_cmdPropTx.startTime = 0;

    if( RFQueue_defineQueue(&dataQueue,
                            txDataEntryBuffer,
                            sizeof(txDataEntryBuffer),
                            NUM_DATA_ENTRIES,
                            MAX_LENGTH + NUM_APPENDED_BYTES))
    {
        /* Failed to allocate space for all data entries */
        while(true);
    }

#if !(defined __CC2650DK_7ID_BOARD_H__) && !(defined __CC2650_LAUNCHXL_BOARD_H__) && !(defined __CC2640R2_LAUNCHXL_BOARD_H__) && !(defined __CC1352R1_LAUNCHXL_BOARD_H__) && !(defined __CC26X2R1_LAUNCHXL_BOARD_H__)
    RF_pCmdTxHS->pQueue = &dataQueue;
    RF_pCmdTxHS->startTrigger.triggerType = TRIG_ABSTIME;
    RF_pCmdTxHS->startTrigger.pastTrig = 1;
    RF_pCmdTxHS->startTime = 0;
#endif

#if (defined __CC2650DK_7ID_BOARD_H__) || (defined __CC2650_LAUNCHXL_BOARD_H__) || (defined __CC2640R2_LAUNCHXL_BOARD_H__) || (defined __CC1350_LAUNCHXL_BOARD_H__) || (defined __CC1350STK_BOARD_H__) || (defined __CC26X2R1_LAUNCHXL_BOARD_H__) || (defined __CC1352R1_LAUNCHXL_BOARD_H__)
    RF_ble_pCmdBleAdvNc->pParams->pAdvData = packet;
    RF_ble_pCmdBleAdvNc->startTrigger.triggerType = TRIG_ABSTIME;
    RF_ble_pCmdBleAdvNc->startTrigger.pastTrig = 1;
    RF_ble_pCmdBleAdvNc->channel = 0xFF;
    RF_ble_pCmdBleAdvNc->whitening.bOverride = 1;
    RF_ble_pCmdBleAdvNc->whitening.init = config->frequencyTable[config->frequency].whitening;
    RF_ble_pCmdBleAdvNc->startTime = 0;
#endif

    currentDataEntry = (rfc_dataEntryGeneral_t*)&txDataEntryBuffer;
    currentDataEntry->length = PAYLOAD_LENGTH;
    pPacket = &currentDataEntry->data;

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

    /* Request access to the radio based on test case*/
    switch (config->rfSetup)
    {
        case RfSetup_Custom:

            rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams);
            packetInterval = (uint32_t)(4000000*0.06f); // Set packet interval to 60 ms
            break;

        case RfSetup_Fsk:
#if !(defined __CC2650DK_7ID_BOARD_H__) && !(defined __CC2650_LAUNCHXL_BOARD_H__) && !(defined __CC2640R2_LAUNCHXL_BOARD_H__) && !(defined __CC26X2R1_LAUNCHXL_BOARD_H__)
            RF_pCmdPropRadioDivSetup_fsk->centerFreq = config->frequencyTable[config->frequency].frequency;
            rfHandle = RF_open(&rfObject, RF_pProp_fsk, (RF_RadioSetup*)RF_pCmdPropRadioDivSetup_fsk, &rfParams);
#else
            rfHandle = RF_open(&rfObject, RF_pProp_2_4G_fsk, (RF_RadioSetup*)RF_pCmdPropRadioSetup_2_4G_fsk, &rfParams);
#endif
            packetInterval = (uint32_t)(4000000*0.06f); // Set packet interval to 60 ms
            break;

#if !(defined __CC2650DK_7ID_BOARD_H__) && !(defined __CC2650_LAUNCHXL_BOARD_H__) && !(defined __CC2640R2_LAUNCHXL_BOARD_H__) && !(defined __CC26X2R1_LAUNCHXL_BOARD_H__)
        case RfSetup_Lrm:

            RF_pCmdPropRadioDivSetup_lrm->centerFreq = config->frequencyTable[config->frequency].frequency;

            rfHandle = RF_open(&rfObject, RF_pProp_lrm, (RF_RadioSetup*)RF_pCmdPropRadioDivSetup_lrm, &rfParams);
            packetInterval = (uint32_t)(4000000*0.5f); // Set packet interval to 500 ms
            break;

        case RfSetup_Sl_lr:

            RF_pCmdPropRadioDivSetup_lrm->centerFreq = config->frequencyTable[config->frequency].frequency;

            rfHandle = RF_open(&rfObject, RF_pProp_sl_lr, (RF_RadioSetup*)RF_pCmdPropRadioDivSetup_sl_lr, &rfParams);
            packetInterval = (uint32_t)(4000000*0.08f); // Set packet interval to 80 ms
            break;

        case RfSetup_Ook:

            RF_pCmdPropRadioDivSetup_ook->centerFreq = config->frequencyTable[config->frequency].frequency;

            rfHandle = RF_open(&rfObject, RF_pProp_ook, (RF_RadioSetup*)RF_pCmdPropRadioDivSetup_ook, &rfParams);
            packetInterval = (uint32_t)(4000000*0.1f); // Set packet interval to 100 ms
            break;
#if  !(defined __CC1352R1_LAUNCHXL_BOARD_H__) && !(defined __CC26X2R1_LAUNCHXL_BOARD_H__)
        case RfSetup_Hsm:

            rfHandle = RF_open(&rfObject, RF_pProp_hsm, (RF_RadioSetup*)RF_pCmdRadioSetup_hsm, &rfParams);
            packetInterval = (uint32_t)(4000000*0.05f); // Set packet interval to 50 ms
            break;
#endif
#endif

#if (defined __CC2650DK_7ID_BOARD_H__) || (defined __CC2650_LAUNCHXL_BOARD_H__) || (defined __CC2640R2_LAUNCHXL_BOARD_H__) || (defined __CC1350_LAUNCHXL_BOARD_H__) || (defined __CC1350STK_BOARD_H__) || (defined __CC26X2R1_LAUNCHXL_BOARD_H__) || (defined __CC1352R1_LAUNCHXL_BOARD_H__)
        case RfSetup_Ble:

            rfHandle = RF_open(&rfObject, RF_pModeBle, (RF_RadioSetup*)RF_ble_pCmdRadioSetup, &rfParams);
            packetInterval = (uint32_t)(4000000*0.1f); // Set packet interval to 100 ms
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

    /* Get current time */
    time = RF_getCurrentTime();
    while (++packetCounter <= config->packetCount)
    {
        /* Create packet with incrementing sequence number and random payload */
        pPacket[0] = packet[0] = (uint8_t)(seqNumber >> 8);
        pPacket[1] = packet[1] = (uint8_t)(seqNumber++);
        uint8_t i;
        for (i = 2; i < PAYLOAD_LENGTH; i++)
        {
            pPacket[i] = packet[i] = rand();
        }

        /* Set absolute TX time to utilize automatic power management */
        time += packetInterval;
        RF_cmdPropTx.startTime = time;

        /* Send packet */
        switch (config->rfSetup)
        {
#if !(defined __CC2650DK_7ID_BOARD_H__) && !(defined __CC2650_LAUNCHXL_BOARD_H__) && !(defined __CC2640R2_LAUNCHXL_BOARD_H__) && !(defined __CC1352R1_LAUNCHXL_BOARD_H__) && !(defined __CC26X2R1_LAUNCHXL_BOARD_H__)
            case RfSetup_Hsm:
            {
                RF_pCmdTxHS->startTime = time;
                RF_EventMask result = RF_runCmd(rfHandle, (RF_Op*)RF_pCmdTxHS, RF_PriorityNormal, NULL, 0);

                if (result != RF_EventLastCmdDone)
                {
                    /* Error */
                    while(true);
                }
                break;
            }
#endif

#if (defined __CC2650DK_7ID_BOARD_H__) || (defined __CC2650_LAUNCHXL_BOARD_H__) || (defined __CC2640R2_LAUNCHXL_BOARD_H__) || (defined __CC1350_LAUNCHXL_BOARD_H__) || (defined __CC1350STK_BOARD_H__) || (defined __CC26X2R1_LAUNCHXL_BOARD_H__) || (defined __CC1352R1_LAUNCHXL_BOARD_H__)
            case RfSetup_Ble:
            {
                RF_ble_pCmdBleAdvNc->startTime = time;
                RF_EventMask result = RF_runCmd(rfHandle, (RF_Op*)RF_ble_pCmdBleAdvNc, RF_PriorityNormal, NULL, 0);
                if (!(result & RF_EventLastCmdDone))
                {
                    /* Error */
                    while(true);
                }
                break;
            }
#endif
            default:
            {
                RF_EventMask result = RF_runCmd(rfHandle, (RF_Op*)&RF_cmdPropTx, RF_PriorityNormal, NULL, 0);
                if (!(result & RF_EventLastCmdDone))
                {
                    /* Error */
                    while(true);
                }
                break;
            }
        }

        /* Update the display */
        menu_updateTxScreen(packetCounter);

        /* Check, whether a button has been pressed */
        if (menu_isButtonPressed())
        {
            packetCounter = 0;
            seqNumber = 0;
            RF_close(rfHandle);
            return TestResult_Aborted;
        }
    }

    packetCounter = 0;
    seqNumber = 0;
    RF_close(rfHandle);
    return TestResult_Finished;
}
