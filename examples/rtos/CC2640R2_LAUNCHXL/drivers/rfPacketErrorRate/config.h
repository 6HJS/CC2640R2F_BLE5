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

#ifndef CONFIG_H
#define CONFIG_H

#include <xdc/std.h>

#include "Board.h"

/* This file defines data types and variables for the application configuration */

/* Pin defines for controlling the CC1190 */
#if (defined __CC1310_LAUNCHXL_BOARD_H__)
#define Board_HGM             CC1310_LAUNCHXL_DIO28_ANALOG
#define Board_LNA_EN          CC1310_LAUNCHXL_DIO29_ANALOG
#define Board_PA_EN           CC1310_LAUNCHXL_DIO30_ANALOG
#endif

/*
RF basic settings as found in the typical settings section of Smart RF Studio.
Each one defines a test case in this application.
*/
#if (defined __CC2650DK_7ID_BOARD_H__) || (defined __CC2650_LAUNCHXL_BOARD_H__) || (defined __CC2640R2_LAUNCHXL_BOARD_H__) || (defined __CC26X2R1_LAUNCHXL_BOARD_H__)
typedef enum
{
    RfSetup_Custom = 0, // Imported from SmartRF Studio
    RfSetup_Fsk,        // Frequency shift keying
    RfSetup_Ble,
    NrOfRfSetups
} RfSetup;
#elif (defined __CC1350_LAUNCHXL_BOARD_H__) || (defined __CC1350STK_BOARD_H__) || (defined __CC1352R1_LAUNCHXL_BOARD_H__)
typedef enum
{
    RfSetup_Custom = 0, // Imported from SmartRF Studio
    RfSetup_Fsk,        // Frequency shift keying
    RfSetup_Lrm,        // Legacy Long Range
    RfSetup_Sl_lr,      // SimpleLink Long Range
    RfSetup_Ook,        // On Off Keying
    RfSetup_Hsm,        // High Speed Mode
    RfSetup_Ble,        // BLE
    NrOfRfSetups
} RfSetup;
#else
typedef enum
{
    RfSetup_Custom = 0, // Imported from SmartRF Studio
    RfSetup_Fsk,        // Frequency Shift Keying
    RfSetup_Lrm,        // Legacy Long Range
    RfSetup_Sl_lr,      // SimpleLink Long Range
    RfSetup_Ook,        // On Off Keying
    RfSetup_Hsm,        // High Speed Mode
    NrOfRfSetups
} RfSetup;
#endif

#if (defined __CC1310_LAUNCHXL_BOARD_H__)
typedef enum
{
    RangeExtender_Dis = 0, // CC1310_LAUNCHXL
    RangeExtender_En,      // CC1310 CC1190 LAUNCHXL
    NrOfRangeExtender
} RangeExtender;
#endif

/* Whether the application works as sender or receiver */
typedef enum
{
    OperationMode_Rx = 0,
    OperationMode_Tx,
    NrOfOperationModes
} OperationMode;

/* Contains a pre-defined setting for frequency selection */
typedef struct
{
    const char* const label;
    const uint16_t frequency;
    const uint16_t fractFreq;
    const uint8_t whitening; // BLE has freq dependent whitening settings
} FrequencyTableEntry;

/*
Holds the application config that is prepared in the menu and
used in the rx and tx functions.
*/
typedef struct
{
    RfSetup rfSetup;                     // Test case index
    OperationMode operationMode;         // TX/RX mode index
    uint32_t packetCount;                // Desired packet count
    FrequencyTableEntry* frequencyTable; // FrequencyTable for this test case
    uint8_t frequency;                   // Index in config_frequencyTable
#if (defined __CC1310_LAUNCHXL_BOARD_H__)
    RangeExtender rangeExtender;         // CC1310 with or without range extender (CC1190)
#endif
} ApplicationConfig;

extern FrequencyTableEntry*  config_frequencyTable_Lut[]; // Lookup table for freq table
extern const char* const config_opmodeLabels[];           // Loopup table for operation mode labels
extern const uint32_t config_packetCountTable[];          // Lookup table for different packet count options
extern const char* const config_rfSetupLabels[];          // Lookup table for RfSetup labels
extern const char* const config_rangeExtenderLabels[];    // Lookup table for range extender labels
extern const uint8_t config_NrOfPacketCounts;             // Total amount of packet count options

#endif
