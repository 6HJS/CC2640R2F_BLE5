/*
 * Copyright (c) 2014-2017, Texas Instruments Incorporated
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
 * */
/*
 *  ======== Boot.c ========
 */

#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <stdbool.h>
#include "package/internal/Boot.xdc.h"

#define REG16(x)  (*(volatile UInt16 *)(x))
#define REG32(x)  (*(volatile UInt32 *)(x))

/* CS defines */
#define CSKEY         (REG32(0x40010400))
#define CSCTL0        (REG32(0x40010404))
#define CSCTL1        (REG32(0x40010408))
#define CSCTL2        (REG32(0x4001040C))
#define CSIFG         (REG32(0x40010448))
#define CSCLRIFG      (REG32(0x40010450))
#define LFXT_EN       (0x00000100)
#define LFXTIFG       (0x00000001)
#define LFXTBYPASS    (0x00000200)
#define CLR_LFXTIFG   (0x00000001)
#define HFXT_EN       (0x01000000)
#define HFXTIFG       (0x00000002)
#define HFXTBYPASS    (0x02000000)
#define CLR_HFXTIFG   (0x00000002)
#define HFXTFREQDRIVE (0x00410000)  /* HFXTFREQ and HFXTDRIVE for 48MHz */

/* WDT defines */
#define WDTCTL   0x4000480C
#define WDTPW    0x5a00
#define WDTHOLD  0x0080

/* Device ID defines */
#define DEVICE_ID_REG 0x0020100C   /* location of Device ID in Info Block */
#define EXT_DEVICE_ID 0x0000A010   /* starting ID for extended Flash devices */

/* FLCTL defines */
#define FLCTL_RDCTL_BNK0_REG  0x40011010
#define FLCTL_RDCTL_BNK1_REG  0x40011014
#define WAIT_MASK             0x0000F000
#define WAIT_0_BITS           0x00000000  /* 0 wait states */
#define WAIT_1_BITS           0x00001000  /* 1 wait states */
#define WAIT_2_BITS           0x00002000  /* 2 wait states */
#define WAIT_3_BITS           0x00003000  /* 3 wait states */

/* DIO defines */
#define PJSEL0                REG16(0x40004D2A)
#define PJSEL1                REG16(0x40004D2C)
#define BIT0                  0x0001
#define BIT1                  0x0002
#define BIT2                  0x0004
#define BIT3                  0x0008

#define Boot_configureClocksLow ti_sysbios_family_arm_msp432_init_Boot_configureClocksLow
#define Boot_configureClocksMed ti_sysbios_family_arm_msp432_init_Boot_configureClocksMed
#define Boot_configureClocksHigh ti_sysbios_family_arm_msp432_init_Boot_configureClocksHigh
#define Boot_disableWatchdog ti_sysbios_family_arm_msp432_init_Boot_disableWatchdog
#define Boot_setupCS ti_sysbios_family_arm_msp432_init_Boot_setupCS

static Bool setPowerState_AM_LDO_VCORE1(void);

/*
 *  ======== Boot_setupCS ========
 */
Void ti_sysbios_family_arm_msp432_init_Boot_setupCS(UInt32 regCSTCL0,
    UInt32 regCSTCL1)
{
    /* unlock Clock System (CS) registers */
    CSKEY = 0x695A;

    /* if LFXT is to be enabled ... */
    if (Boot_enableLFXT) {

        /* configure pins for LFXT function */
        PJSEL0 |= BIT0;
        PJSEL1 &= ~BIT0;

        /* if not bypassing LFXT, start and wait for LF osc stabilization */
        if (!Boot_bypassLFXT) {
            CSCTL2 |= LFXT_EN;
            while (CSIFG & LFXTIFG) {
                CSCLRIFG |= CLR_LFXTIFG;
            }
        }
        /* else, enable bypass */
        else {
            CSCTL2 |= LFXTBYPASS;
        }
    }

    /* if HFXT is to be enabled ... */
    if (Boot_enableHFXT) {

        /* configure pins for HFXT function */
        PJSEL0 |= BIT3;
        PJSEL1 &= ~BIT3;

        /* if not bypassing HFXT, start and wait for HF osc stabilization */
        if (!Boot_bypassHFXT) {
            CSCTL2 |= HFXTFREQDRIVE;
            CSCTL2 |= HFXT_EN;
            while (CSIFG & HFXTIFG) {
               CSCLRIFG |= CLR_HFXTIFG;
            }
         }
        /* else, enable bypass */
        else {
            CSCTL2 |= HFXTBYPASS;
        }
    }

    /* setup the clock selectors and dividers */
    CSCTL1 = regCSTCL1;

    /* set DCO center frequency */
    CSCTL0 = regCSTCL0;

    /* re-lock CS register access */
    CSKEY = 0;
}

/*
 *  ======== Boot_configureClocksLow ========
 */
Void ti_sysbios_family_arm_msp432_init_Boot_configureClocksLow(UInt32 CTL0,
    UInt32 CTL1)
{
    UInt32 waitbits;
    UInt32 temp;

    if (REG32(DEVICE_ID_REG) < EXT_DEVICE_ID) {
        waitbits = WAIT_0_BITS;
    }
    else {
        waitbits = WAIT_1_BITS;
    }

    /* setup Flash wait states (MCLK = 12MHz requires no wait states) */
    temp = REG32(FLCTL_RDCTL_BNK0_REG) & ~WAIT_MASK;
    REG32(FLCTL_RDCTL_BNK0_REG) = temp | waitbits;
    temp = REG32(FLCTL_RDCTL_BNK1_REG) & ~WAIT_MASK;
    REG32(FLCTL_RDCTL_BNK1_REG) = temp | waitbits;

    /* setup Clock System registers */
    Boot_setupCS(CTL0, CTL1);
}

/*
 *  ======== Boot_configureClocksMed ========
 */
Void ti_sysbios_family_arm_msp432_init_Boot_configureClocksMed(UInt32 CTL0,
    UInt32 CTL1)
{
    UInt32 temp;

    /* switch to AM_LDO_VCORE1 (MCLK = 24MHz requires VCORE = 1) */
    setPowerState_AM_LDO_VCORE1();

    /* setup Flash wait states (1 wait state for all devices when VCORE=1) */
    temp = REG32(FLCTL_RDCTL_BNK0_REG) & ~WAIT_MASK;
    REG32(FLCTL_RDCTL_BNK0_REG) = temp | WAIT_1_BITS;
    temp = REG32(FLCTL_RDCTL_BNK1_REG) & ~WAIT_MASK;
    REG32(FLCTL_RDCTL_BNK1_REG) = temp | WAIT_1_BITS;

    /* setup Clock System registers */
    Boot_setupCS(CTL0, CTL1);
}

/*
 *  ======== Boot_configureClocksHigh ========
 */
Void ti_sysbios_family_arm_msp432_init_Boot_configureClocksHigh(UInt32 CTL0,
    UInt32 CTL1)
{
    UInt32 waitbits;
    UInt32 temp;

    if (REG32(DEVICE_ID_REG) < EXT_DEVICE_ID) {
        waitbits = WAIT_1_BITS;
    }
    else {
        waitbits = WAIT_3_BITS;
    }

    /* switch to AM_LDO_VCORE1 (MCLK = 48MHz requires VCORE = 1) */
    setPowerState_AM_LDO_VCORE1();

    /* setup Flash wait states (MCLK = 48MHz requires 2 wait states */
    temp = REG32(FLCTL_RDCTL_BNK0_REG) & ~WAIT_MASK;
    REG32(FLCTL_RDCTL_BNK0_REG) = temp | waitbits;
    temp = REG32(FLCTL_RDCTL_BNK1_REG) & ~WAIT_MASK;
    REG32(FLCTL_RDCTL_BNK1_REG) = temp | waitbits;

    /* setup Clock System registers */
    Boot_setupCS(CTL0, CTL1);
}

/*
 *  ======== Boot_disableWatchdog ========
 */
Void ti_sysbios_family_arm_msp432_init_Boot_disableWatchdog()
{
    REG16(WDTCTL) = WDTPW + WDTHOLD;
}

/* ======================================================================= */

/*
 * Note that the following PCM definitions and functions are in lieu of
 * making a MAP_PCM_setPowerState() function call.  In the future, the kernel
 * build flow should be updated to allow the Boot module to make direct
 * driverlib calls.  Then the following can be removed, and the above
 * calls to setPowerState_AM_LDO_VCORE1() should be replaced with calls to
 * MAP_PCM_setPowerState().  SYSBIOS-400 has been filed for this.
 */

/* PCM definitions */
typedef struct {
    volatile uint32_t CTL0;
    volatile uint32_t CTL1;
    volatile uint32_t IE;
    volatile  uint32_t IFG;
    volatile  uint32_t CLRIFG;
} PCM_Type;

#define PERIPH_BASE            ((uint32_t)0x40000000)
#define PCM_BASE               (PERIPH_BASE +0x00010000)
#define PCM                    ((PCM_Type *) PCM_BASE)
#define PCM_KEY                0x695A0000
#define PCM_CTL0_CPM_MASK      ((uint32_t)0x00003F00)
#define PCM_CTL0_CPM_OFS       (8)
#define PCM_CTL0_AMR_MASK      ((uint32_t)0x0000000F)
#define PCM_CTL0_KEY_MASK      ((uint32_t)0xFFFF0000)
#define PCM_CTL1_PMR_BUSY_OFS  (8)
#define PCM_AM_LDO_VCORE0      0x00
#define PCM_AM_LDO_VCORE1      0x01
#define PCM_AM_DCDC_VCORE0     0x04
#define PCM_AM_DCDC_VCORE1     0x05
#define PCM_AM_LF_VCORE0       0x08
#define PCM_AM_LF_VCORE1       0x09
#define PCM_LPM0_LDO_VCORE0    0x10
#define PCM_LPM0_LDO_VCORE1    0x11
#define PCM_LPM0_DCDC_VCORE0   0x14
#define PCM_LPM0_DCDC_VCORE1   0x15
#define PCM_LPM0_LF_VCORE0     0x18
#define PCM_LPM0_LF_VCORE1     0x19
#define PCM_LPM3               0x20
#define PCM_LPM4               0x21
#define PCM_LPM35_VCORE0       0xC0
#define PCM_LPM45              0xA0
#define PCM_VCORE0             0x00
#define PCM_VCORE1             0x01
#define PCM_VCORELPM3          0x02
#define PCM_LDO_MODE           0x00
#define PCM_DCDC_MODE          0x01
#define PCM_LF_MODE            0x02

#define BITBAND_PERI_BASE      ((uint32_t)(0x42000000))
#define BITBAND_PERI(x, b)     (*((volatile  uint8_t *) (BITBAND_PERI_BASE + \
     (((uint32_t)(uint32_t *)&(x)) - PERIPH_BASE)*32 + (b)*4)))

static UInt8 getCoreVoltageLevel(Void);
static UInt8 getPowerMode(Void);
static UInt8 getPowerState(Void);
static Bool setCoreVoltageLevelAdvanced(UInt8 voltageLevel, UInt32 timeOut,
    Bool blocking);
static Bool setPowerModeAdvanced(UInt8 powerMode, UInt32 timeOut,
    Bool blocking);

/*
 *  ======== getCoreVoltageLevel ========
 */
static UInt8 getCoreVoltageLevel(Void)
{
    UInt8 currentPowerState = getPowerState();

    switch (currentPowerState)
    {
        case PCM_AM_LDO_VCORE0:
        case PCM_AM_DCDC_VCORE0:
        case PCM_AM_LF_VCORE0:
        case PCM_LPM0_LDO_VCORE0:
        case PCM_LPM0_DCDC_VCORE0:
        case PCM_LPM0_LF_VCORE0:
            return (PCM_VCORE0);

        case PCM_AM_LDO_VCORE1:
        case PCM_AM_DCDC_VCORE1:
        case PCM_AM_LF_VCORE1:
        case PCM_LPM0_LDO_VCORE1:
        case PCM_LPM0_DCDC_VCORE1:
        case PCM_LPM0_LF_VCORE1:
            return (PCM_VCORE1);

        case PCM_LPM3:
            return (PCM_VCORELPM3);

        default:
            return (false);
    }
}

/*
 *  ======== getPowerMode ========
 */
static UInt8 getPowerMode(Void)
{
    UInt8 currentPowerState;

    currentPowerState = getPowerState();

    switch (currentPowerState)
    {
        case PCM_AM_LDO_VCORE0:
        case PCM_AM_LDO_VCORE1:
        case PCM_LPM0_LDO_VCORE0:
        case PCM_LPM0_LDO_VCORE1:
            return (PCM_LDO_MODE);

        case PCM_AM_DCDC_VCORE0:
        case PCM_AM_DCDC_VCORE1:
        case PCM_LPM0_DCDC_VCORE0:
        case PCM_LPM0_DCDC_VCORE1:
            return (PCM_DCDC_MODE);

        case PCM_LPM0_LF_VCORE0:
        case PCM_LPM0_LF_VCORE1:
        case PCM_AM_LF_VCORE1:
        case PCM_AM_LF_VCORE0:
            return (PCM_LF_MODE);

        default:
            return (false);
    }
}

/*
 *  ======== getPowerState ========
 */
static UInt8 getPowerState(Void)
{
    return ((PCM->CTL0 & PCM_CTL0_CPM_MASK) >> PCM_CTL0_CPM_OFS);
}

/*
 *  ======== setCoreVoltageLevelAdvanced ========
 */
static Bool setCoreVoltageLevelAdvanced(UInt8 voltageLevel, UInt32 timeOut,
    Bool blocking)
{
    UInt8 bCurrentVoltageLevel;
    UInt32 regValue;

    /* Get the current voltage level */
    bCurrentVoltageLevel = getCoreVoltageLevel();

    /* If already at the voltage level requested, return */
    if (bCurrentVoltageLevel == voltageLevel)
        return (true);

    while (bCurrentVoltageLevel != voltageLevel) {
        regValue = PCM->CTL0;

        switch (getPowerState()) {
            case PCM_AM_LF_VCORE1:
            case PCM_AM_DCDC_VCORE1:
            case PCM_AM_LDO_VCORE0:
                PCM->CTL0 = (PCM_KEY | (PCM_AM_LDO_VCORE1)
                    | (regValue & ~(PCM_CTL0_KEY_MASK | PCM_CTL0_AMR_MASK)));
                break;

            case PCM_AM_LF_VCORE0:
            case PCM_AM_DCDC_VCORE0:
            case PCM_AM_LDO_VCORE1:
                PCM->CTL0 = (PCM_KEY | (PCM_AM_LDO_VCORE0)
                    | (regValue & ~(PCM_CTL0_KEY_MASK | PCM_CTL0_AMR_MASK)));
                break;
        }

        if(blocking) {
            while (BITBAND_PERI(PCM->CTL1, PCM_CTL1_PMR_BUSY_OFS)) {};
        }

        bCurrentVoltageLevel = getCoreVoltageLevel();
    }

    return (true);
}

/*
 *  ======== setPowerModeAdvanced ========
 */
static Bool setPowerModeAdvanced(UInt8 powerMode, UInt32 timeOut, Bool blocking)
{
    UInt8 bCurrentPowerMode, bCurrentPowerState;
    UInt32 regValue;

    /* Get current power mode */
    bCurrentPowerMode = getPowerMode();

    /* If the power mode being set it the same as the current mode, return */
    if (powerMode == bCurrentPowerMode)
        return (true);

    bCurrentPowerState = getPowerState();

    /* while loop while we haven't achieved the power mode */
    while (bCurrentPowerMode != powerMode) {
        regValue = PCM->CTL0;

        switch (bCurrentPowerState) {
            case PCM_AM_DCDC_VCORE0:
            case PCM_AM_LF_VCORE0:
                PCM->CTL0 = (PCM_KEY | PCM_AM_LDO_VCORE0
                    | (regValue & ~(PCM_CTL0_KEY_MASK | PCM_CTL0_AMR_MASK)));
                break;

            case PCM_AM_LF_VCORE1:
            case PCM_AM_DCDC_VCORE1:
                PCM->CTL0 = (PCM_KEY | PCM_AM_LDO_VCORE1
                    | (regValue & ~(PCM_CTL0_KEY_MASK | PCM_CTL0_AMR_MASK)));
                break;

            case PCM_AM_LDO_VCORE1:
            {
                if (powerMode == PCM_DCDC_MODE) {
                    PCM->CTL0 = (PCM_KEY | PCM_AM_DCDC_VCORE1
                      | (regValue & ~(PCM_CTL0_KEY_MASK | PCM_CTL0_AMR_MASK)));
                } else if (powerMode == PCM_LF_MODE) {
                    PCM->CTL0 = (PCM_KEY | PCM_AM_LF_VCORE1
                      | (regValue & ~(PCM_CTL0_KEY_MASK | PCM_CTL0_AMR_MASK)));
                }
                break;
            }
            case PCM_AM_LDO_VCORE0:
            {
                if (powerMode == PCM_DCDC_MODE) {
                    PCM->CTL0 = (PCM_KEY | PCM_AM_DCDC_VCORE0
                      | (regValue & ~(PCM_CTL0_KEY_MASK | PCM_CTL0_AMR_MASK)));
                } else if (powerMode == PCM_LF_MODE) {
                    PCM->CTL0 = (PCM_KEY | PCM_AM_LF_VCORE0
                      | (regValue & ~(PCM_CTL0_KEY_MASK | PCM_CTL0_AMR_MASK)));
                }
                break;
            }
        }

        if (blocking) {
            while (BITBAND_PERI(PCM->CTL1, PCM_CTL1_PMR_BUSY_OFS)) {};
        }

        bCurrentPowerMode = getPowerMode();
        bCurrentPowerState = getPowerState();
    }

    return (true);
}

/*
 *  ======== setPowerState_AM_LDO_VCORE1 ========
 */
static Bool setPowerState_AM_LDO_VCORE1(void)
{
    if (getPowerState() == PCM_AM_LDO_VCORE1) {
        return (true);
    }

    return (setCoreVoltageLevelAdvanced(PCM_VCORE1, 0, true) &&
            setPowerModeAdvanced(PCM_LDO_MODE, 0, true));
}
