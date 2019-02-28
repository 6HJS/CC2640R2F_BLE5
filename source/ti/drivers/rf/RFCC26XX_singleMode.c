/*
* Copyright (c) 2015-2017, Texas Instruments Incorporated
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

#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/SwiP.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/rf/RF.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(inc/hw_rfc_rat.h)
#include DeviceFamily_constructPath(inc/hw_rfc_dbell.h)
#include DeviceFamily_constructPath(driverlib/rfc.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include DeviceFamily_constructPath(driverlib/ioc.h)
#include DeviceFamily_constructPath(driverlib/aon_ioc.h)
#include DeviceFamily_constructPath(driverlib/rf_mailbox.h)
#include DeviceFamily_constructPath(driverlib/adi.h)
#include DeviceFamily_constructPath(driverlib/aon_rtc.h)
#include DeviceFamily_constructPath(driverlib/chipinfo.h)

#if defined(__IAR_SYSTEMS_ICC__)
#pragma diag_remark=Pa082
#endif

#if !defined(RF_SINGLEMODE)
#error "A required symbol (RF_SINGLEMODE) is missing."
#endif

/*-------------- Typedefs, structures & defines ---------------*/
typedef enum {
    Fsm_EventBootDone           = (1<<30),
    Fsm_EventPowerStep          = (1<<29),
    Fsm_EventLastCommandDone    = (1<<1),
    Fsm_EventWakeup             = (1<<2),
    Fsm_EventPowerDown          = (1<<3),
    Fsm_EventChangePhy          = (1<<12),
    Fsm_EventHwInt              = (1<<13),
    Fsm_EventCpeInt             = (1<<14)
} Fsm_Event;

/* Max # of RF driver clients */
#define N_MAX_CLIENTS               1
/* 8 RF_Cmds in pool */
#define N_CMD_POOL                  8
/* Modulus mask used for RF_CmdHandle calculations */
#define N_CMD_MODMASK               0xFFF

/* Macro rules */
#define ABS(x)   (((x) < 0) ? -(x) : (x))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

/* Internal RF constants */
#define RF_CMD0                                0x0607
/* Accessible RF Core interrupts mask MSB 32 bits : RFHW int, LSB 32 bits : RF CPE int */
#define RF_INTERNAL_IFG_MASK                   0xFFFFFFDF6000100E
#define RF_CMD_ABORTED_FLAG                    (1 << 0)
#define RF_CMD_STOPPED_FLAG                    (1 << 1)
#define RF_CMD_CANCELLED_FLAG                  (1 << 2)
#define RF_CMD_CANCEL_MASK                     (RF_CMD_ABORTED_FLAG | RF_CMD_STOPPED_FLAG | RF_CMD_CANCELLED_FLAG)
#define RF_CMD_ALLOC_FLAG                      (1 << 7)
#define RF_RUN_CMD_ON_RESTART_FLAG             (1 << 0)
#define RF_RAT_ERR_CB_EVENT_FLAG               (1 << 1)
#define RF_RAT_RPT_CPT_FLAG                    (1 << 2)
#define RF_HW_INT_RAT_CH_MASK                  (RFC_DBELL_RFHWIFG_RATCH7 | RFC_DBELL_RFHWIFG_RATCH6 | RFC_DBELL_RFHWIFG_RATCH5)
#define RF_MAX_RAT_CH_CNT                      3
#define RF_INTERNAL_HW_IFG_MASK_32_BIT         RFC_DBELL_RFHWIFG_MDMSOFT
/* Default value for power up duration (in us) used before first power cycle */
#define RF_DEFAULT_POWER_UP_TIME               2500
/* Default minimum power up duration (in us) */
#define RF_DEFAULT_MIN_POWER_UP_TIME           500
/* Default power-up margin (in us) to account for wake-up sequence outside the RF power state machine */
#define RF_DEFAULT_POWER_UP_MARGIN             (250+32)
/* Default power down duration in us */
#define RF_DEFAULT_POWER_DOWN_TIME             1000
#define RF_MAX_CHAIN_CMD_LEN                   32
#define RF_RAT_CH_5_INDEX                      0x0
#define RF_RAT_CH_6_INDEX                      0x1
#define RF_RAT_CH_7_INDEX                      0x2
/* RAT channel (0-4) are used by RF Core. Only 5,6,7 are available for application */
#define RF_RAT_CH_OFFSET                       5
#define RF_SEND_RAT_STOP_RATIO                 7
#define RF_RTC_CONV_TO_US_SHIFT                12
#define RF_SHIFT_32_BITS                       32
#define RF_SHIFT_8_BITS                        8
#define RF_SHIFT_16_BITS                       16
#define RF_RTC_TICK_INC                        (0x100000000LL/32768)
#define RF_SCALE_RTC_TO_4MHZ                   4000000
#define RF_NUM_RAT_TICKS_IN_1_US               4
/* (3/4)th of a full RAT cycle, in us */
#define RF_DISPATCH_MAX_TIME_US                (UINT32_MAX / RF_NUM_RAT_TICKS_IN_1_US * 3 / 4)
#define RF_INTERNAL_CPE_IFG_MASK_32_BIT        0xFFFFFFFF
#define RF_XOSC_HF_SWITCH_CHECK_PERIOD         50
#define RF_DEFAULT_AVAILRATCH_VAL              0xFFFF
#define RF_ABORT_CANCEL_MULTIPLE_CMD           0x2
#define RF_ABORT_CANCEL_SINGLE_CMD             0x1
#define RF_CMDSTA_REG_VAL_MASK                 0xFF
#define RF_RAT_CPT_CONFIG_RATCH_MASK           0x0F00
#define RF_RAT_CPT_CONFIG_REP_MASK             0x1000
#define RF_PHY_SWITCHING_MODE                  1
#define RF_PHY_BOOTUP_MODE                     0
/* Additional analog config time for setup command */
#define RF_ANALOG_CFG_TIME_US                  96
/* Update analog configuration in setup */
#define RF_SETUP_ANALOGCFG_UPDATE              0
/* Don't update analog configuration in setup */
#define RF_SETUP_ANALOGCFG_NOUPDATE            0x2D
/* Window (in us) to decide if wakeup was from RF power up clock */
#define RF_WAKEUP_DETECTION_WINDOW_IN_US       100

/* RF command queue member */
typedef struct RF_Cmd_s RF_Cmd;

/* RF command member */
struct RF_Cmd_s {
    RF_Cmd* volatile     pNext;   /* Pointer to next #RF_Cmd in queue */
    RF_Callback volatile pCb;     /* Pointer to callback function */
    RF_Op*               pOp;     /* Pointer to (chain of) RF operations(s) */
    RF_Object*           pClient; /* Pointer to client */
    RF_EventMask         bmEvent; /* Enable mask for interrupts from the command */
    uint32_t             rfifg;   /* Return value for callback 0:31 - RF_CPE0_INT, 32:63 - RF_HW_INT */
    RF_CmdHandle         ch;      /* Command handle */
    RF_Priority          ePri;    /* Priority of RF command */
    uint8_t volatile     flags;   /* [0: Aborted, 1: Stopped, 2: cancelled] */
};

/* RF Direct Immediate command */
typedef struct RF_CmdDirImm_s RF_CmdDirImm;

/* RF direct immediate command member */
struct RF_CmdDirImm_s {
    RF_Handle            pClient;                   /* Pointer to current client */
    RF_Callback          pRatCb[RF_MAX_RAT_CH_CNT]; /* Array of callback pointers for each RAT chan */
    uint32_t volatile    hwIntEvent;                /* RF_HW events received */
    uint16_t             availRatCh;                /* bit-map for available RAT channels */
    uint8_t              rptCptRatChNum;            /* Channel allocated for repeat mode */
    uint8_t              flags;                     /* [0:runOnRestart, 1:errEventCb, 2: rptCptFlag] */
};

/* Placeholder for direct and immediate commands */
static RF_CmdDirImm cmdDirImm;

/* Flag to indicate that the radio timer is active */
static bool bRatActive = false;

/* FSM typedefs */
typedef void (*Fsm_StateFxn)(RF_Object*, Fsm_Event const);

/*-------------- Forward declarations ---------------*/

/* Common inactivity timeout clock callback */
static void clkInactivityFxn(RF_Object* pObj);

/* FSM helper functions */
static void FsmDispatch(RF_Object* pObj, Fsm_Event const e);
static void FsmNextState(Fsm_StateFxn pTarget);

/* FSM state functions */
static void fsmPowerUpState(RF_Object *pObj, Fsm_Event e);
static void fsmSetupState(RF_Object *pObj, Fsm_Event e);
static void fsmActiveState(RF_Object *pObj, Fsm_Event e);
static void fsmXOSCState(RF_Object *pObj, Fsm_Event e);

/* RF Patching fxn */
static void applyRfCorePatch(bool mode);

/* RF direct and immediate command implementation */
static RF_Stat  postDirImmCmd(uint32_t pCmd, bool bStartRadio);

/*-------------- Constants ---------------*/

/*-------------- Global variables ---------------*/
/* Pointer to current radio client (indicates also whether radio powered) */
static RF_Object* pCurrClient;

/* Keeps track of whether radio is active */
static bool bRadioActive;

/* Current Radio FSM state */
static Fsm_StateFxn pFsmState;

/* Count of radio clients */
static uint8_t nClients;
static uint8_t nActiveClients;

/* Common RF command queue shared by all clients */
static struct {
    RF_Cmd* volatile      pPend;     /* Queue of RF_Cmds pending execution */
    RF_Cmd* volatile      pCurrCmd;  /* RF_Cmd currently executing */
    RF_Cmd* volatile      pCurrCmdCb;/* Holds current command while callback is processed */
    RF_Cmd* volatile      pDone;     /* Queue of RF_Cmds that are done but not retired */
    RF_CmdHandle volatile nSeqPost;  /* Sequence # for previously posted command */
    RF_CmdHandle volatile nSeqDone;  /* Sequence # for last done command */
} cmdQ;

/* RF_Cmd container pool. Containers with extra information about RF commands in flight
   These are organized as a circular array rather than a "retired" or "free" queue
   facilitate mapping to the sequence numbers used as command handles */
static RF_Cmd aCmdPool[N_CMD_POOL];

/* SWIs and HWIs (structures + forward reference of handlers) */
static SwiP_Struct swiFsm;
static void swiFxnFsm(uintptr_t a, uintptr_t b);
static HwiP_Struct hwiCpe0;
static void hwiFxnCpe0PowerFsm(uintptr_t a);
static void hwiFxnCpe0Active(uintptr_t a);

static SwiP_Struct swiHw;
static void swiFxnHw(uintptr_t a, uintptr_t b);
static HwiP_Struct hwiHw;
static void hwiFxnHw(uintptr_t a);

/* Clock used for triggering power-up sequences */
static ClockP_Struct clkPowerUp;
static void clkPowerUpFxn(uintptr_t a);

/* Default error callback */
static void defaultCallback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);

/* RAT sync/start/stop radio operations */
static union {
    rfc_CMD_SYNC_START_RAT_t start;
    rfc_CMD_SYNC_STOP_RAT_t  stop;
} opRatSync;

/* Default RF parameters structure */
static const RF_Params RF_defaultParams = {
    .nInactivityTimeout     = SemaphoreP_WAIT_FOREVER,
    .nPowerUpDuration       = RF_DEFAULT_POWER_UP_TIME,
    .pPowerCb               = NULL,
    .pErrCb                 = NULL,
    .bPowerUpXOSC           = true,
    .nPowerUpDurationMargin = RF_DEFAULT_POWER_UP_MARGIN,
};

/* Powerup requested when powering down */
static bool volatile bRestart;

/* Internal abortCmd */
static RF_Stat abortCmd(RF_Handle h, RF_CmdHandle ch, uint8_t type);

static uint32_t nRtc1;
static uint32_t nRtc3;
static uint32_t activeTimeInUs;
static uint32_t volatile dispatchInUs;
static bool     bStart;

/* Hardware attribute structure populated in board.c file to set HWI and SWI priorities */
extern const RFCC26XX_HWAttrs RFCC26XX_hwAttrs;

/* RAT channel allocate and free */
static int8_t ratChanAlloc(RF_CmdDirImm *pCmd);
static void ratChanFree(int8_t ratChanNum);

/* Initialize available RAT channels */
static bool bInitAvailRatCh;

/* Indicate when Radio is in power-up FSM */
static bool bPowerUpActive;

/* Indicate when Radio is in power-down FSM */
static bool bPowerDownActive;

/* Indicate if RF_yield requested to power-down the RF Core */
static bool bYieldPwrDnCalled;

/* Indicate request to cancel a power-down request from RF_yield */
static bool bYieldPwrDnCancel;

/* Obj wakeup notification from the Power driver */
static Power_NotifyObj RF_wakeupNotifyObj;

/* Variable to indicate if the XOSC_HF_SWITCHING is done by the RF Driver */
static bool bXOSCSwitchConstraint;

/* variable to indicate with the FLASH is disable during the power up */
static bool bDisableFlashInIdleConstraint;
/*-------------- Command queue internal functions ---------------*/

/* Peek at first entry in simple queue */
static RF_Cmd* Q_peek(RF_Cmd* volatile* ppHead)
{
    return (RF_Cmd*)*ppHead;
}

/* Pop first entry in simple queue */
static RF_Cmd* Q_pop(RF_Cmd* volatile* ppHead)
{
    RF_Cmd* pCmd;
    uint32_t key = HwiP_disable();
    if ((pCmd=(RF_Cmd*)*ppHead))
    {
        *ppHead = (RF_Cmd*)pCmd->pNext;
    }
    HwiP_restore(key);
    return pCmd;
}

/* Push entry onto simple queue */
static bool Q_push(RF_Cmd* volatile* ppHead, RF_Cmd* pCmd)
{
    RF_Cmd* pEnd = (RF_Cmd*)*ppHead;
    uint32_t key;

    pCmd->pNext = NULL;
    if (!pEnd)
    {
        /* First entry into queue */
        key = HwiP_disable();
        *ppHead = pCmd;
    }
    else
    {
        key = HwiP_disable();
        while (pEnd->pNext)
        {
            pEnd = (RF_Cmd*)pEnd->pNext;
        }

        /* Sanity check - to avoid pEnd->pNext pointing to self */
        if (pEnd == pCmd)
        {
            HwiP_restore(key);
            return false;
        }

        pEnd->pNext = pCmd;
    }
    HwiP_restore(key);

    return true;
}

/* Search last entry in simple queue  for particular client */
static RF_Cmd* Q_end(RF_Handle h, RF_Cmd* volatile* ppHead)
{
    RF_Cmd* pEnd = NULL;
    RF_Cmd* pClientEnd = NULL;

    uint32_t key = HwiP_disable();
    if ((pEnd = (RF_Cmd*)*ppHead))
    {
        pClientEnd = pEnd;
        while (pEnd->pNext)
        {
            pEnd = (RF_Cmd*)pEnd->pNext;
            pClientEnd = pEnd;
        }
    }
    HwiP_restore(key);

    return pClientEnd;
}

/* Allocate cmd buffer from cmd pool */
static RF_Cmd* allocCmd(RF_CmdHandle ch)
{
    uint8_t i;
    for (i = 0; i < N_CMD_POOL; i++)
    {
        if (!(aCmdPool[i].flags & RF_CMD_ALLOC_FLAG))
        {
            return &aCmdPool[i];
        }
    }
    return NULL;
}

/* Search cmd from cmd pool */
static RF_Cmd* getCmd(RF_CmdHandle ch)
{
    uint8_t i;
    for (i = 0; i < N_CMD_POOL; i++)
    {
        if ((aCmdPool[i].flags & RF_CMD_ALLOC_FLAG) && (aCmdPool[i].ch == ch))
        {
            return &aCmdPool[i];
        }
    }
    return NULL;
}

/*  Function: RF_wakeupNotifyFxn
 *
 *  Wake up notification callback from the power driver. If the callback is from RF wakeup
 *  set constraint to let RF Driver control the XOSC switching else do nothing in the
 *  callback
 *
 *  Input:
 *
 *  Return: Power_NOTIFYDONE
 */
static uint8_t RF_wakeupNotifyFxn(uint8_t eventType, uint32_t *eventArg, uint32_t *clientArg)
{
    ClockP_Handle powerUpClkHandle = ClockP_handle(&clkPowerUp);

    /* check if the callback is for wakeup from standby and if power up clock is running */
    if ((eventType == PowerCC26XX_AWAKE_STANDBY) && (ClockP_isActive(powerUpClkHandle)))
    {
        /* calculate time (in us) until next trigger (assume next trigger is max ~70 min away) */
        uint32_t timeInUsUntilNextTrig = ClockP_tickPeriod * ClockP_getTimeout(powerUpClkHandle);


        /* check if the next trig time is close enough to the actual power up */
        if (timeInUsUntilNextTrig < RF_WAKEUP_DETECTION_WINDOW_IN_US)
        {
            /* stop power up clock */
            ClockP_stop(powerUpClkHandle);
            /* setup RF Driver to do the XOSC_HF switching */
            Power_setConstraint(PowerCC26XX_DISALLOW_XOSC_HF_SWITCHING);
            /* set variable to indicate RF Driver will do the XOSC_HF switching */
            bXOSCSwitchConstraint = true;
            /* start the RF Core power up */
            SwiP_or(&swiFsm, Fsm_EventWakeup);
        }
    }

    return Power_NOTIFYDONE;
}

/* Decrement client count and signal FSM to power down if it reaches 0
   Parameter is us until next command should be dispatched (0 means never) */
static void decActiveClientCnt(uint32_t nUsUntilDispatch)
{
    uint32_t key = HwiP_disable();
    if (nActiveClients && (--nActiveClients == 0))
    {
        /* Signal to power state machine that we can power down radio */
        SwiP_or(&swiFsm, Fsm_EventPowerDown);

        if (nUsUntilDispatch)
        {

            ClockP_stop(ClockP_handle(&pCurrClient->state.clkInactivity));
            /* Set clock object that will trigger radio wakeup */
            uint32_t timeout = MAX((nUsUntilDispatch-(pCurrClient->clientConfig.nPowerUpDuration))/ ClockP_tickPeriod, 1);
            ClockP_setTimeout(ClockP_handle(&clkPowerUp), timeout);
            ClockP_start(ClockP_handle(&clkPowerUp));
        }
    }
    HwiP_restore(key);
}


/*
 *  Function: dispatchOpWhen
 *  Check whether to dispatch operation immediately or power down radio and wait.
 *  If the former it returns 0, if the latter it returns the #us until when RF
 *  need to execute the command
 *
 *  Calling context: internal (Hwi, Swi)
 *
 *  Input:   pOp - Pointer to radio operation command structure
 *
 *  Return:  Time (in us) till radio command execution
 */
static uint32_t dispatchOpWhen(RF_Op* pOp)
{
    /* Limitations:
       - Only recognizes TRIG_ABSTIME triggers, everything else gets dispatched immediately */
    if (pOp->startTrigger.triggerType == TRIG_ABSTIME)
    {
        /* Difference of startTime from current time in us */
        uint32_t deltaTimeUs = (pOp->startTime - RF_getCurrentTime()) / RF_NUM_RAT_TICKS_IN_1_US;

        /* Check if delta time is greater than (powerup duration + power down duration) for a
           power cycle, and is less than 3/4 of a RAT cycle (~17 minutes) */
        if ((deltaTimeUs > (int32_t)(pCurrClient->clientConfig.nPowerUpDuration + RF_DEFAULT_POWER_DOWN_TIME)) &&
            (deltaTimeUs <= RF_DISPATCH_MAX_TIME_US))
        {
            /* Delta time is within range */
            return deltaTimeUs;
        }
    }
    return 0;
}


/* Update implicit state based on finished commands */
static void updateImplicitState(RF_Cmd* pCmd)
{
    uint8_t nCmdChainMax = RF_MAX_CHAIN_CMD_LEN;

    /* Looks at operation or chained operations in a command and:
       - Makes a copy of any FS operation for a future power up sequence */
    RF_Op* pOp = pCmd->pOp;
    while (pOp && nCmdChainMax)
    {
        if ((pOp->commandNo == CMD_FS) || (pOp->commandNo == CMD_FS_OFF))
        {
            /* Create a copy of CMD_FS command (or CMD_FS_OFF) for later power up */
            memcpy(&pCmd->pClient->state.mode_state.cmdFs, pOp, sizeof(pCmd->pClient->state.mode_state.cmdFs));
            break;
        }
        pOp = pOp->pNextOp;
        --nCmdChainMax;
    }
}

/* The RF_HW ISR when radio is active. */
static void hwiFxnHw(uintptr_t a)
{
    RF_CmdDirImm* pDirImmCmd = &cmdDirImm;

    uint32_t rfchwifg = (HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFHWIFG) & (RF_INTERNAL_HW_IFG_MASK_32_BIT | RF_HW_INT_RAT_CH_MASK));
    HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFHWIFG) = ~rfchwifg;

    /* Post SWI to handle registered callbacks */
    if ((rfchwifg & RF_INTERNAL_HW_IFG_MASK_32_BIT) &&
        (HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFHWIEN) & RF_INTERNAL_HW_IFG_MASK_32_BIT))
    {
        SwiP_or(&swiFsm, Fsm_EventHwInt);
    }

    if (rfchwifg & RF_HW_INT_RAT_CH_MASK)
    {
        pDirImmCmd->hwIntEvent |= (rfchwifg & RF_HW_INT_RAT_CH_MASK);
        SwiP_or(&swiHw, 0);
    }
}

/* SWI used for user callback to any RAT event (RF_HW HWI) */
static void swiFxnHw(uintptr_t a, uintptr_t b)
{
    RF_CmdDirImm* pDirImmCmd = &cmdDirImm;

    /* RAT channel callback
       Optimize code to issue callback in the order in which the RAT ch int was received */
    uint32_t key = HwiP_disable();
    if (pDirImmCmd->hwIntEvent & RF_HW_INT_RAT_CH_MASK)
    {
        uint8_t ratCbIdx = pDirImmCmd->hwIntEvent >> RFC_DBELL_RFHWIFG_RATCH5_BITN;
        /* Process lower channel first and allow multiple interrupt flags to be processed sequentially */
        if (ratCbIdx & (1 << RF_RAT_CH_5_INDEX))
        {
            ratCbIdx = RF_RAT_CH_5_INDEX;
        }
        else if (ratCbIdx & (1 << RF_RAT_CH_6_INDEX))
        {
            ratCbIdx = RF_RAT_CH_6_INDEX;
        }
        else if (ratCbIdx & (1 << RF_RAT_CH_7_INDEX))
        {
            ratCbIdx = RF_RAT_CH_7_INDEX;
        }

        ratChanFree(ratCbIdx + RF_RAT_CH_OFFSET);
        pDirImmCmd->hwIntEvent &= ~(1 << (ratCbIdx + RFC_DBELL_RFHWIFG_RATCH5_BITN));
        HwiP_restore(key);
        if (pDirImmCmd->pRatCb[ratCbIdx])
        {
            pDirImmCmd->pRatCb[ratCbIdx](pDirImmCmd->pClient, ratCbIdx + RF_RAT_CH_OFFSET, RF_EventRatCh);
        }
    }
    else
    {
        HwiP_restore(key);
    }

    /* Process if multiple interrupt flag bits are set */
    if (pDirImmCmd->hwIntEvent)
    {
        SwiP_or(&swiHw, 0);
    }
}

/* The CPE0 ISR when radio is active. Assume that all IRQs relevant to command
   dispatcher are mapped here. Furthermore, assume that there is no need for
   critical sections here (i.e. that this ISR has higher priority than
   any HWI calling a RF API function or that HWIs can't call the RF API) */
static void hwiFxnCpe0Active(uintptr_t a)
{
    RF_Cmd* pCmd;

    /* Get and clear doorbell IRQs */
    uint32_t rfcpeifg = RFCCpeIntGetAndClear();

    /* Retire command that has run */
    if (rfcpeifg & RFC_DBELL_RFCPEIFG_LAST_COMMAND_DONE_M)
    {
        /* LAST_COMMAND_DONE received. */
        if ((pCmd = cmdQ.pCurrCmd))
        {
            /* No longer current command */
            cmdQ.pCurrCmd = NULL;

            /* Save CPEIFG for callback */
            pCmd->rfifg |= ((rfcpeifg & pCmd->bmEvent) | RF_EventLastCmdDone);

            /* Move current command to done queue */
            HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFCPEIEN) &= ~(pCmd->bmEvent & RF_INTERNAL_CPE_IFG_MASK_32_BIT);
            HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFHWIEN) &= ~(pCmd->bmEvent >> RF_SHIFT_32_BITS);
            Q_push(&cmdQ.pDone, pCmd);

            /* Post SWI to handle registered callbacks and potentially start inactivity timer */
            SwiP_or(&swiFsm, Fsm_EventLastCommandDone);
        }
    }

    /* Handle additional interrupts here? */
    pCmd = cmdQ.pCurrCmd;
    if ((!(rfcpeifg & Fsm_EventLastCommandDone)) && (rfcpeifg & (pCmd->bmEvent)))
    {
        cmdQ.pCurrCmdCb = pCmd;

        /* Save CPEIFG for callback */
        pCmd->rfifg |= (rfcpeifg & pCmd->bmEvent);

        /* Post SWI to handle registered callbacks */
        SwiP_or(&swiFsm, Fsm_EventCpeInt);
    }

    /* Dispatch pending command */
    if ((!cmdQ.pCurrCmd) && (pCmd = Q_peek(&cmdQ.pPend)))
    {
        if (pCmd->pClient != pCurrClient)
        {
            /* We need to change radio client, signal to FSM. Once radio client has
               changed (either as part of power-up or PHY change) the FSM will invoke
               this ISR and dispatch command (Unsupported feature) */
            SwiP_or(&swiFsm, Fsm_EventChangePhy);
        }
        else if ((dispatchInUs = dispatchOpWhen(pCmd->pOp)))
        {
            /* Command sufficiently far into future that it shouldn't be dispatched yet
               Decrement active client count and potentially power down radio */
            decActiveClientCnt(dispatchInUs);
        }
        else
        {
            HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFCPEIEN) |= (pCmd->bmEvent & RF_INTERNAL_CPE_IFG_MASK_32_BIT);
            HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFHWIEN) |= (pCmd->bmEvent >> RF_SHIFT_32_BITS);

            /* Dispatch immediately, set command to current and remove from pending queue */
            HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFACKIFG) = 0;
            HWREG(RFC_DBELL_BASE+RFC_DBELL_O_CMDR) = (uint32_t)pCmd->pOp;
            while(!HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFACKIFG));

            Q_pop(&cmdQ.pPend);
            cmdQ.pCurrCmd = pCmd;
        }
    }
}


/* Clock callback due to inactivity timeout OR next pending command far into future */
static void clkInactivityFxn(RF_Object* pObj)
{
    /* Decrement active client count and potentially signal to FSM that we can
       power down radio */
    decActiveClientCnt(0);
}


/* Callback used to post semaphore for runCmd() and waitCmd() */
static void syncCb(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
    if (h->state.pCbSync != NULL)
    {
        ((RF_Callback)h->state.pCbSync)(h, ch, e);
    }

    if ((e & RF_EventLastCmdDone) || (e & h->state.eventSync))
    {
        h->state.unpendCause = e;
        SemaphoreP_post(&h->state.semSync);
    }
}


/*
 *  Function: defaultCallback
 *
 *  Default callback function
 *
 */
static void defaultCallback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
  ;
}

/*-------------- RF powerup/powerdown FSM functions ---------------*/

/* Affect FSM state change */
static void FsmDispatch(RF_Object* pObj, Fsm_Event const e)
{
    pFsmState(pObj, e);
}


/* Set target state to transition to */
static void FsmNextState(Fsm_StateFxn pTarget)
{
    pFsmState = pTarget;
}


/* The SWI handler for FSM events */
static void swiFxnFsm(uintptr_t a0, uintptr_t a1)
{
    FsmDispatch(pCurrClient, (Fsm_Event)SwiP_getTrigger());
}


/* Clock callback called upon powerup */
static void clkPowerUpFxn(uintptr_t a)
{
    /* Trigger FSM SWI to get radio wakeup going */
    SwiP_or(&swiFsm, Fsm_EventWakeup);
}


/* The CPE0 ISR during FSM powerup/powerdown */
static void hwiFxnCpe0PowerFsm(uintptr_t a0)
{
    /* Clear all IRQ flags in doorbell and then clear them */
    uint32_t rfcpeifg = RFCCpeIntGetAndClear();

    if ((rfcpeifg & IRQ_BOOT_DONE) || (rfcpeifg & IRQ_LAST_COMMAND_DONE))
    {
        SwiP_or(&swiFsm, Fsm_EventPowerStep);
    }
}

/*-------------- Power management state functions ---------------*/
static void applyRfCorePatch(bool mode)
{
    /* Patch CPE if relevant */
    if (pCurrClient->clientConfig.pRfMode->cpePatchFxn)
    {
        pCurrClient->clientConfig.pRfMode->cpePatchFxn();
    }

    if ((pCurrClient->clientConfig.pRfMode->mcePatchFxn != 0) || (pCurrClient->clientConfig.pRfMode->rfePatchFxn != 0))
    {
        /* Wait for clocks to turn on */
        while(!HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFACKIFG));
        HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFACKIFG) = 0;

        /* Patch if relevant */
        if (pCurrClient->clientConfig.pRfMode->mcePatchFxn)
        {
            pCurrClient->clientConfig.pRfMode->mcePatchFxn();
        }

        /* Patch if relevant */
        if (pCurrClient->clientConfig.pRfMode->rfePatchFxn)
        {
            pCurrClient->clientConfig.pRfMode->rfePatchFxn();
        }

        /* Turn off additional clocks */
        RFCDoorbellSendTo(CMDR_DIR_CMD_2BYTE(RF_CMD0, 0));
    }
}

/*
 *  Function: radioOpDoneCb
 *
 *  Handle callback to client for RF_EventLastCmdDone and issue radio free callback if required
 *
 */
static void radioOpDoneCb(RF_Cmd* pCmd)
{
    /* Radio command done */
    if (pCmd)
    {
        /* Update implicit radio state */
        updateImplicitState(pCmd);

        uint32_t tmp = pCmd->rfifg;
        pCmd->rfifg = 0;
        /* Call callback, free container and dequeue */
        if (pCmd->pCb)
        {
            RF_EventMask cb_e = (RF_EventMask)(tmp | RF_EventLastCmdDone);
            switch (pCmd->flags & RF_CMD_CANCEL_MASK)
            {
                case RF_CMD_ABORTED_FLAG:
                    cb_e = RF_EventCmdAborted;
                    break;
                case RF_CMD_STOPPED_FLAG:
                    cb_e = RF_EventCmdStopped;
                    break;
                case RF_CMD_CANCELLED_FLAG:
                    cb_e = RF_EventCmdCancelled;
                    break;
                default:
                    break;
            }
            pCmd->pCb(pCmd->pClient, pCmd->ch, cb_e);
        }

        uint32_t key = HwiP_disable();
        cmdQ.nSeqDone = (cmdQ.nSeqDone+1)&N_CMD_MODMASK;
        cmdQ.pDone->flags = 0;
        Q_pop(&cmdQ.pDone);
        HwiP_restore(key);

        /* Check if there are any more pending commands for this client */
        if ((cmdQ.nSeqDone == pCmd->pClient->state.chLastPosted) && (!bRatActive))
        {
            /* This was last posted command for client */
            if (pCmd->pClient->state.bYielded || (pCmd->pClient->clientConfig.nInactivityTimeout == 0))
            {
                /* We can powerdown immediately */
                pCmd->pClient->state.bYielded = false;
                decActiveClientCnt(0);
            }
            else
            {
                if (pCurrClient->clientConfig.nInactivityTimeout != SemaphoreP_WAIT_FOREVER)
                {
                    /* Start inactivity timer */
                    uint32_t timeout = MAX(pCmd->pClient->clientConfig.nInactivityTimeout/ClockP_tickPeriod, 1);
                    ClockP_setTimeout(ClockP_handle(&pCmd->pClient->state.clkInactivity), timeout);
                    ClockP_start(ClockP_handle(&pCmd->pClient->state.clkInactivity));
                }
            }
        }
    }
}

static void fsmPowerUpState(RF_Object *pObj, Fsm_Event e)
{
    /* Note: pObj is NULL in this state */
    if (e & Fsm_EventLastCommandDone)
    {
        /* issue radio operation done callback */
        RF_Cmd *pCmd = Q_peek(&cmdQ.pDone);
        radioOpDoneCb(pCmd);

        /* We've handled this event now */
        e &= ~Fsm_EventLastCommandDone;
        if ((pCmd = Q_peek(&cmdQ.pDone)))
        {
            /* Trigger self if there are more commands in callback queue */
            SwiP_or(&swiFsm, (e | Fsm_EventLastCommandDone));
        }
        else if ((e & Fsm_EventWakeup) || (Q_peek(&cmdQ.pPend)))
        {
            /* If there is pending cmd, just wake up */
            /* Device will put to sleep if the pending cmd is future cmd */
            ClockP_stop(ClockP_handle(&clkPowerUp));

            /* trigger power up */
            SwiP_or(&swiFsm, Fsm_EventWakeup);
        }
    }
    else if(e & Fsm_EventWakeup)
    {
        /* Disable system going to IDLE mode */
        bDisableFlashInIdleConstraint = true;
        Power_setConstraint(PowerCC26XX_NEED_FLASH_IN_IDLE);

        nRtc1  = AONRTCCurrent64BitValueGet();

        /* Set current client from first command in command queue */
        RF_Cmd* pNextCmd;
        if ((pNextCmd = Q_peek(&cmdQ.pPend)))
        {
            pCurrClient = pNextCmd->pClient;
        }

        /* Set the RF mode in the PRCM register */
        HWREG(PRCM_BASE + PRCM_O_RFCMODESEL) = pCurrClient->clientConfig.pRfMode->rfMode;

        /* Register power dependency and constraints - i.e. power up RF. */
        Power_setConstraint(PowerCC26XX_SB_DISALLOW);
        Power_setDependency(PowerCC26XX_DOMAIN_RFCORE);

        /* Start power-up sequence */
        bPowerUpActive = true;

        if((pCurrClient->clientConfig.bPowerUpXOSC == false) || bStart == 0)
        {
            Power_setDependency(PowerCC26XX_XOSC_HF);
        }

        /* Turn on additional clocks on boot */
        if ((pCurrClient->clientConfig.pRfMode->mcePatchFxn != 0) || (pCurrClient->clientConfig.pRfMode->rfePatchFxn != 0))
        {
            HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFACKIFG) = 0;
            HWREG(RFC_DBELL_BASE+RFC_DBELL_O_CMDR) = CMDR_DIR_CMD_2BYTE(RF_CMD0, RFC_PWR_PWMCLKEN_MDMRAM | RFC_PWR_PWMCLKEN_RFERAM);
        }

        /* Turn on clock to RF core */
        RFCClockEnable();

        /* Map all radio interrupts to CPE0 and enable some sources */
        HWREG(RFC_DBELL_BASE+RFC_DBELL_O_RFCPEISL) = 0;

        HWREG(RFC_DBELL_BASE+RFC_DBELL_O_RFCPEIEN) =
                RFC_DBELL_RFCPEIEN_LAST_COMMAND_DONE_M |
                RFC_DBELL_RFCPEIEN_BOOT_DONE_M;

        /* Next FSM state */
        FsmNextState(&fsmSetupState);

        /* Enable interrupts and let BOOT_DONE interrupt kick things off */
        HwiP_enableInterrupt(INT_RFC_CPE_0);
    }
}

static void fsmSetupState(RF_Object *pObj, Fsm_Event e)
{
    if (e & Fsm_EventPowerStep)
    {
        /* Apply RF Core patch (if required) */
        applyRfCorePatch(RF_PHY_BOOTUP_MODE);

        /* Initialize bus request */
        HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFACKIFG) = 0;
        HWREG(RFC_DBELL_BASE+RFC_DBELL_O_CMDR) = CMDR_DIR_CMD_1BYTE(CMD_BUS_REQUEST, 1);

        /* Set VCOLDO reference */
        RFCAdi3VcoLdoVoltageMode(true);

        /* Next state: fsmActiveState */
        FsmNextState(&fsmActiveState);

        /* Setup RAT_SYNC command to follow SETUP command */
        opRatSync.start.commandNo                = CMD_SYNC_START_RAT;
        opRatSync.start.status                   = IDLE;
        opRatSync.start.startTrigger.triggerType = TRIG_NOW;
        opRatSync.start.pNextOp                  = NULL;
        opRatSync.start.condition.rule           = COND_NEVER;    /* Default: don't chain */

        /* Setup SETUP command */
        RF_RadioSetup* pOpSetup         = pObj->clientConfig.pOpSetup;
        pOpSetup->prop.status           = IDLE;

        /* For power saving - analogCfgMode is set to 0 only for the first power up */
        if ((bStart == 0) || (pObj->clientConfig.bUpdateSetup))
        {
            if ((pObj->clientConfig.pOpSetup->commandId.commandNo == CMD_RADIO_SETUP) ||
                (pObj->clientConfig.pOpSetup->commandId.commandNo == CMD_BLE5_RADIO_SETUP))
            {
                /* BLE or IEEE mode. BLE 5.0 shares same offset for config.analogCfgMode with BLE radio setup */
                pOpSetup->common.config.analogCfgMode = RF_SETUP_ANALOGCFG_UPDATE;
            }
            else
            {
                /* Proprietary mode */
                pOpSetup->prop.config.analogCfgMode = RF_SETUP_ANALOGCFG_UPDATE;
            }

            pObj->clientConfig.bUpdateSetup = false;
        }

        /* Trim directly the radio register values based on the ID of setup command */
        rfTrim_t rfTrim;
        RFCRfTrimRead((rfc_radioOp_t*)pObj->clientConfig.pOpSetup, (rfTrim_t*)&rfTrim);
        RFCRfTrimSet((rfTrim_t*)&rfTrim);

        RF_Op* tmp                      = (RF_Op*)&pOpSetup->prop;
        while ((tmp->pNextOp) && (tmp->pNextOp->commandNo != CMD_SYNC_START_RAT))
        {
            tmp = tmp->pNextOp;
        }
        tmp->pNextOp                    = (RF_Op*)&opRatSync.start;
        tmp->condition.rule             = COND_ALWAYS;

        /* Setup FS command to follow SETUP command */
        rfc_CMD_FS_t* pOpFs;
        RF_Op* pOpFirstPend = Q_peek(&cmdQ.pPend)->pOp;
        if ((pOpFirstPend->commandNo == CMD_FS) || (pOpFirstPend->commandNo == CMD_FS_OFF))
        {
            /* First command is FS command so no need to chain an implicit FS command
               Reset nRtc1 */
            nRtc1 = 0;
        }
        else
        {
            if (pObj->state.mode_state.cmdFs.commandNo)
            {
                /* Chain in the implicit FS command */
                pOpFs                           = &pObj->state.mode_state.cmdFs;
                pOpFs->status                   = IDLE;
                pOpFs->pNextOp                  = NULL;
                pOpFs->startTrigger.triggerType = TRIG_NOW;
                pOpFs->condition.rule           = COND_NEVER;
                opRatSync.start.pNextOp         = (RF_Op*)pOpFs;
                opRatSync.start.condition.rule  = COND_ALWAYS;
            }
        }

        /* Make sure BUS_REQUEST is done */
        while(!HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFACKIFG));
        HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFACKIFG) = 0;

        /* power up the frequency synthesizer */
        pObj->clientConfig.pOpSetup->common.config.bNoFsPowerUp = 0;

        /* Run the XOSC_HF switching if the pre-notify function setup the power
           constraint PowerCC26XX_DISALLOW_XOSC_HF_SWITCHING */
        if (bXOSCSwitchConstraint)
        {
            /* wait untill the XOSC_HF is stable */
            while(!PowerCC26XX_isStableXOSC_HF());
            /* run the XOSC_HF switch */
            PowerCC26XX_switchXOSC_HF();
        }
        else if (OSCClockSourceGet(OSC_SRC_CLK_HF) != OSC_XOSC_HF)
        {
            tmp->condition.rule = COND_NEVER;
            FsmNextState(&fsmXOSCState);
        }

        /* Send chain */
        HWREG(RFC_DBELL_BASE+RFC_DBELL_O_CMDR) = (uint32_t)&pObj->clientConfig.pOpSetup->commandId;
    }
}

static void fsmXOSCState(RF_Object *pObj, Fsm_Event e)
{
    if ((e & Fsm_EventPowerStep) || (e & Fsm_EventWakeup))
    {
        if(OSCClockSourceGet(OSC_SRC_CLK_HF) == OSC_XOSC_HF)
        {
            FsmNextState(&fsmActiveState);

            /* Do RAT sync */
            HWREG(RFC_DBELL_BASE + RFC_DBELL_O_CMDR) = (uint32_t)&opRatSync.start;
        }
        else
        {
            /* Schedule next polling */
            uint32_t timeout = MAX(RF_XOSC_HF_SWITCH_CHECK_PERIOD/ClockP_tickPeriod, 1);
            ClockP_setTimeout(ClockP_handle(&clkPowerUp), timeout);
            ClockP_start(ClockP_handle(&clkPowerUp));
        }
    }
}

static void fsmActiveState(RF_Object *pObj, Fsm_Event e)
{
    RF_Cmd* pCmd;
    RF_CmdDirImm* pCmdDirImm;

    if (e & Fsm_EventCpeInt)
    {
        uint32_t key = HwiP_disable();
        pCmd = Q_peek(&cmdQ.pCurrCmdCb);
        uint32_t tmp = pCmd->rfifg;

        if (!(tmp & RF_EventLastCmdDone) && pCmd)
        {
            pCmd->rfifg &= (~tmp);
            HwiP_restore(key);
            if (pCmd->pCb)
            {
                pCmd->pCb(pCmd->pClient, pCmd->ch, (RF_EventMask)tmp);
            }
        }
        else
        {
            HwiP_restore(key);
        }

        /* We've handled this event now */
        e &= ~Fsm_EventCpeInt;
    }
    else if (e & Fsm_EventLastCommandDone)
    {
        /* issue radio operation done callback */
        pCmd = Q_peek(&cmdQ.pDone);
        radioOpDoneCb(pCmd);

        /* We've handled this event now */
        e &= ~Fsm_EventLastCommandDone;
        if ((pCmd = Q_peek(&cmdQ.pDone)))
        {
            /* Trigger self if there are more commands in callback queue */
            e |= Fsm_EventLastCommandDone;
        }
    }
    else if (e & Fsm_EventPowerStep)
    {
        /* Coming from powerup states */

        /* Release disable IDLE since power up is now complete */
        if (bDisableFlashInIdleConstraint)
        {
            Power_releaseConstraint(PowerCC26XX_NEED_FLASH_IN_IDLE);
            bDisableFlashInIdleConstraint = false;
        }

        /* For power saving, set analogCfgMode to 0x2D */
        if ((pObj->clientConfig.pOpSetup->commandId.commandNo == CMD_RADIO_SETUP) ||
            (pObj->clientConfig.pOpSetup->commandId.commandNo == CMD_BLE5_RADIO_SETUP))
        {
            pObj->clientConfig.pOpSetup->common.config.analogCfgMode = RF_SETUP_ANALOGCFG_NOUPDATE;
        }
        else
        {
            pObj->clientConfig.pOpSetup->prop.config.analogCfgMode = RF_SETUP_ANALOGCFG_NOUPDATE;
        }

        /* Update power up duration if coming from the clkPowerUpFxn (nRtc not zero) */
        uint32_t nRtc2;
        if (nRtc1 && (OSCClockSourceGet(OSC_SRC_CLK_LF) != OSC_RCOSC_HF))
        {
            nRtc2  = AONRTCCurrent64BitValueGet();
            if (nRtc2 > nRtc1)
            {
                pObj->clientConfig.nPowerUpDuration = (nRtc2 - nRtc1) >> RF_RTC_CONV_TO_US_SHIFT;
            }
            else
            {
                pObj->clientConfig.nPowerUpDuration = (0xFFFFFFFF + nRtc2 - nRtc1) >> RF_RTC_CONV_TO_US_SHIFT;
            }

            /* Add margin to the power up duration to account for the TIRTOS/Drivers overhead */
            pObj->clientConfig.nPowerUpDuration += pObj->clientConfig.nPowerUpDurationMargin;

            /* Power up duration should be within certain upper and lower bounds */
            if ((pObj->clientConfig.nPowerUpDuration > RF_DEFAULT_POWER_UP_TIME) ||
                (pObj->clientConfig.nPowerUpDuration < RF_DEFAULT_MIN_POWER_UP_TIME))
            {
                pObj->clientConfig.nPowerUpDuration = RF_DEFAULT_POWER_UP_TIME;
            }
        }

        /* Check the status of the CMD_FS, if it was sent (chained) to the setup command sent
           during Fsm_EventChangePhy1 and return error callback to the client.
           The client can either resend the CMD_FS or ignore the error as per Errata on PG2.1 */
        if ((opRatSync.start.condition.rule == COND_ALWAYS) &&
            (pObj->state.mode_state.cmdFs.status == ERROR_SYNTH_PROG))
        {
            RF_Callback pErrCb = (RF_Callback)pObj->clientConfig.pErrCb;
            pErrCb(pObj, RF_ERROR_CMDFS_SYNTH_PROG, RF_EventError);
        }

        /* Change HWI handler */
        HwiP_setFunc(&hwiCpe0, hwiFxnCpe0Active, (uintptr_t)NULL);

        /* Get the available RAT channel map */
        pCmdDirImm = &cmdDirImm;
        if (bInitAvailRatCh)
        {
            rfc_CMD_GET_FW_INFO_t getFwInfoCmd;
            getFwInfoCmd.commandNo = CMD_GET_FW_INFO;
            RFCDoorbellSendTo((uint32_t)&getFwInfoCmd);
            pCmdDirImm->availRatCh = getFwInfoCmd.availRatCh;
            bInitAvailRatCh = false;
        }

        /* Mark radio and client as being active */
        bRadioActive = true;
        nActiveClients++;
        bPowerUpActive = false;

        /* Check if there is pending command */
        if (Q_peek(&cmdQ.pPend))
        {
            /* Make sure pending command gets issued by issuing CPE0 IRQ */
            HwiP_post(INT_RFC_CPE_0);
        }
        else
        {
            /* No pending command */
            e |= Fsm_EventLastCommandDone;
        }

        /* Power up callback */
        RF_Callback pPowerCb = (RF_Callback)pObj->clientConfig.pPowerCb;
        pPowerCb(pObj, 0, RF_EventPowerUp);

        /* We've handled this event now */
        e &= ~Fsm_EventPowerStep;
    }
    else if (bYieldPwrDnCancel && (e & Fsm_EventPowerDown))
    {
        bYieldPwrDnCancel = false;
        e &= ~Fsm_EventPowerDown;
    }
    else if (e & Fsm_EventPowerDown)
    {
        /* Powerdown RF core
           The proper thing to do here is to avoid switching off the digital part of
           radio without switching off the analog parts first */

        /* Release constraint to switch XOSC_HF in rf driver if set during power up */
        if (bXOSCSwitchConstraint)
        {
            bXOSCSwitchConstraint = false;
            Power_releaseConstraint(PowerCC26XX_DISALLOW_XOSC_HF_SWITCHING);
        }

        /* Radio not active */
        bRadioActive = false;
        bPowerDownActive = true;

        /* Set VCOLDO reference */
        RFCAdi3VcoLdoVoltageMode(false);

        /* Remap HWI to startup function */
        HwiP_setFunc(&hwiCpe0, hwiFxnCpe0PowerFsm, (uintptr_t)NULL);

        bool bSendOpRatSync = false;

        if ((bStart == 0) || (bInitAvailRatCh))
        {
            bStart = 1;

            /* Always do RAT_SYNC_STOP command the first time power down */
            bSendOpRatSync = true;
        }
        else if (dispatchInUs)
        {
            /* Time in active state measured */
            nRtc3 = AONRTCCurrent64BitValueGet();
            nRtc3 = (nRtc3-nRtc1) >> RF_RTC_CONV_TO_US_SHIFT;
            activeTimeInUs += nRtc3;
            if (activeTimeInUs > (dispatchInUs << RF_SEND_RAT_STOP_RATIO))
            {
                /* Reset active time accumulation */
                activeTimeInUs = 0;

                /* Setup RAT_SYNC command to follow FsPowerdown command */
                bSendOpRatSync = true;
            }
        }

        if (bSendOpRatSync)
        {
            /* Stop and synchronize the RAT if it is running */
            if(HWREG(RFC_PWR_BASE + RFC_PWR_O_PWMCLKEN) & RFC_PWR_PWMCLKEN_RAT_M)
            {
                /* Setup RAT_SYNC command to follow FsPowerdown command */
                opRatSync.stop.commandNo                = CMD_SYNC_STOP_RAT;
                opRatSync.stop.condition.rule           = COND_NEVER;
                opRatSync.stop.status                   = IDLE;
                opRatSync.stop.startTrigger.triggerType = TRIG_NOW;
                opRatSync.stop.pNextOp                  = NULL;

                /* Enter critical section */
                uint32_t key = HwiP_disable();

                /* Disable CPE last command done interrupt */
                HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFCPEIEN) &= ~RFC_DBELL_RFCPEIEN_LAST_COMMAND_DONE_M;

                /* Clear CPE last command done interrupt flag */
                HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFCPEIFG) = ~RFC_DBELL_RFCPEIFG_LAST_COMMAND_DONE_M;

                /* Send RAT Stop command */
                HWREG(RFC_DBELL_BASE + RFC_DBELL_O_CMDR) = (uint32_t)&opRatSync.stop;

                /* Wait till RAT Stop command to complete */
                while (!(HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFCPEIFG) & RFC_DBELL_RFCPEIFG_LAST_COMMAND_DONE_M));

                /* Clear CPE last command done interrupt flag and enable last command done interrupt */
                HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFCPEIFG) = ~RFC_DBELL_RFCPEIFG_LAST_COMMAND_DONE_M;
                HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFCPEIEN) |= RFC_DBELL_RFCPEIEN_LAST_COMMAND_DONE_M;

                /* Leave critical section */
                HwiP_restore(key);
            }
        }

        /* Reset nRtc1 */
        nRtc1 = 0;

        /* Turn off Synth */
        RFCSynthPowerDown();

        /* Turn off CPE */
        RFCClockDisable();

        /* Release constraints and dependencies, turning off RF core */
        Power_releaseConstraint(PowerCC26XX_SB_DISALLOW);
        Power_releaseDependency(PowerCC26XX_DOMAIN_RFCORE);
        if(pObj->clientConfig.bPowerUpXOSC == false)
        {
            Power_releaseDependency(PowerCC26XX_XOSC_HF);
        }

        /* Radio powered down */
        bPowerDownActive = false;
        bYieldPwrDnCalled = false;

        /* Closing all handles */
        if (!nClients)
        {
            SemaphoreP_post(&pObj->state.semSync);
        }

        /* Next state: fsmPowerUpState */
        FsmNextState(&fsmPowerUpState);

        /* Start radio if required */
        if (bRestart)
        {
            bRestart = false;
            SwiP_or(&swiFsm, Fsm_EventWakeup);
        }

        /* We've handled this event now */
        e &= ~Fsm_EventPowerDown;
    }
    else if (e & Fsm_EventHwInt)
    {
        pCmd = Q_peek(&cmdQ.pCurrCmd);
        if (pCmd && pCmd->pCb)
        {
            pCmd->pCb(pCmd->pClient, pCmd->ch, RF_EventMdmSoft);
        }
        /* We've handled this event now */
        e &= ~Fsm_EventHwInt;
    }
    else
    {
        /* No other FSM event has any effect */
    }

    /* Call self again if there are outstanding events to be processed */
    if (e)
    {
        SwiP_or(&swiFsm, e);
    }
}

/*-------------- Initialization & helper functions ---------------*/

/* Initialize RF driver.  */
static void rf_init()
{
    union {
        HwiP_Params hp;
        SwiP_Params sp;
    } params;

    /* Power init */
    Power_init();

    /* Enable output RTC clock for Radio Timer Synchronization */
    HWREG(AON_RTC_BASE + AON_RTC_O_CTL) |= AON_RTC_CTL_RTC_UPD_EN_M;

    /* Initialize SWIs and HWIs */
    SwiP_Params_init(&params.sp);
    params.sp.priority = RFCC26XX_hwAttrs.swiCpe0Priority;
    SwiP_construct(&swiFsm, swiFxnFsm, &params.sp);

    HwiP_Params_init(&params.hp);
    params.hp.enableInt = false;
    params.hp.priority = RFCC26XX_hwAttrs.hwiCpe0Priority;
    HwiP_construct(&hwiCpe0, INT_RFC_CPE_0, hwiFxnCpe0PowerFsm, &params.hp);

    SwiP_Params_init(&params.sp);
    params.sp.priority = RFCC26XX_hwAttrs.swiHwPriority;
    SwiP_construct(&swiHw, swiFxnHw, &params.sp);

    HwiP_Params_init(&params.hp);
    params.hp.enableInt = true;
    params.hp.priority = RFCC26XX_hwAttrs.hwiHwPriority;
    HwiP_construct(&hwiHw, INT_RFC_HW_COMB, hwiFxnHw, &params.hp);

    /* Initialize clock object used as power-up trigger */
    ClockP_construct(&clkPowerUp, &clkPowerUpFxn, 0, NULL);

    /* Set FSM state to power up */
    FsmNextState(fsmPowerUpState);

    /* Subscribe to wakeup notification from the Power driver */
    Power_registerNotify(&RF_wakeupNotifyObj, PowerCC26XX_AWAKE_STANDBY,
                         (Power_NotifyFxn) RF_wakeupNotifyFxn, (uintptr_t) NULL);


    /* Initialize global variables */
    RF_CmdDirImm* pCmdDirImm = &cmdDirImm;
    pCmdDirImm->availRatCh = RF_DEFAULT_AVAILRATCH_VAL;
    bInitAvailRatCh = true;
    pCmdDirImm->flags = 0;
    bStart = false;
    bPowerUpActive = false;
    bPowerDownActive = false;
    bYieldPwrDnCalled = false;
    bYieldPwrDnCancel = false;
    nRtc1 = 0;
    bXOSCSwitchConstraint = false;
    bDisableFlashInIdleConstraint = false;
}


/*-------------- API functions ---------------*/

RF_Handle RF_open(RF_Object *pObj, RF_Mode *pRfMode, RF_RadioSetup *pOpSetup, RF_Params *params)
{
    DebugP_assert(pObj != NULL);

    /* Verify RF mode */
    if (pRfMode != NULL)
    {
        /* Check whether RF mode is valid */
        uint32_t rfcModeHwOpt = HWREG(PRCM_BASE + PRCM_O_RFCMODEHWOPT);
        if (rfcModeHwOpt & (1 << pRfMode->rfMode))
        {
            pObj->clientConfig.pRfMode = pRfMode;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }

    /* Verify radio setup */
    if (pOpSetup != NULL)
    {
        /* Trim the override list. The implementation of RFCOverrideUpdate is device specific */
        RFCOverrideUpdate((rfc_radioOp_t*)pOpSetup, NULL);

        /* Register the setup command to the object */
        pObj->clientConfig.pOpSetup = pOpSetup;
    }
    else
    {
        return NULL;
    }

    /* Check whether RF driver is accepting more clients */
    uint32_t key = HwiP_disable();

    /* Register the client if possible */
    if (nClients < N_MAX_CLIENTS)
    {
        /* Initialize shared objects on first client */
        if(nClients == 0) rf_init();

        /* Register the new client */
        nClients++;

        /* Restore the key */
        HwiP_restore(key);

        /* Populate default params if not provided */
        RF_Params rfParams;
        if (params == NULL)
        {
            RF_Params_init(&rfParams);
            params = &rfParams;
        }

        /* Initialize RF_Object config */
        pObj->clientConfig.nInactivityTimeout     = params->nInactivityTimeout;
        pObj->clientConfig.nPowerUpDuration       = params->nPowerUpDuration;
        pObj->clientConfig.bPowerUpXOSC           = params->bPowerUpXOSC;
        pObj->clientConfig.bUpdateSetup           = true;
        pObj->clientConfig.nPowerUpDurationMargin = params->nPowerUpDurationMargin;

        /* Set all the callbacks to default (do nothing)*/
        pObj->clientConfig.pErrCb                 = (void*) defaultCallback;
        pObj->clientConfig.pPowerCb               = (void*) defaultCallback;

        /* If a user specified callback is provided, overwrite the default */
        if (params->pErrCb)
        {
            pObj->clientConfig.pErrCb = (void *)params->pErrCb;
        }
        if (params->pPowerCb)
        {
            pObj->clientConfig.pPowerCb = (void *)params->pPowerCb;
        }

        /* Initialize client state & variables */
        memset(&pObj->state, 0, sizeof(pObj->state));

        /* Initialize client semaphore and clock object */
        ClockP_Params cp;

        /* Initialize client specifi objects */
        SemaphoreP_constructBinary(&pObj->state.semSync, 0);
        ClockP_Params_init(&cp);
        cp.arg = (uintptr_t)pObj;
        ClockP_construct(&pObj->state.clkInactivity,
                (ClockP_Fxn)clkInactivityFxn, 0, &cp);

        return pObj;
    }
    else
    {
        /* Restore the key and return if the new client request can not be served */
        HwiP_restore(key);
        return NULL;
    }
}


void RF_close(RF_Handle h)
{
    DebugP_assert(h != NULL);

    /* Wait for all issued commands to finish */
    if(nClients)
    {
        if (cmdQ.nSeqPost != cmdQ.nSeqDone)
        {
            /* Trace the end of pending queue for client h */
            RF_Cmd *pCmd = Q_end(h, &cmdQ.pPend);
            if ((!pCmd) && (cmdQ.pCurrCmd))
            {
                /* no pending commands for client h, current command is the last command */
                pCmd = cmdQ.pCurrCmd;
            }
            if (pCmd)
            {
                RF_pendCmd(h, pCmd->ch, (RF_EventLastCmdDone | RF_EventCmdAborted | RF_EventCmdStopped | RF_EventCmdCancelled));
            }
        }

        /* Enter critical section */
        uint32_t key = HwiP_disable();

        /* Check weather this is the last client */
        if (--nClients == 0)
        {
            if (bRadioActive)
            {
                /* Power down radio */
                decActiveClientCnt(0);

                /* Wait for powerdown and unregister module RTOS objects */
                HwiP_restore(key);
                SemaphoreP_pend(&h->state.semSync, SemaphoreP_WAIT_FOREVER);
                key = HwiP_disable();
            }

            /* Unregister shared RTOS objects */
            SwiP_destruct(&swiFsm);
            HwiP_destruct(&hwiCpe0);
            SwiP_destruct(&swiHw);
            HwiP_destruct(&hwiHw);
            ClockP_destruct(&clkPowerUp);
            pCurrClient = NULL;
            bRatActive = false;

            /* Exit critical section */
            HwiP_restore(key);

            /* Unregister client specific RTOS objects */
            SemaphoreP_destruct(&h->state.semSync);
            ClockP_destruct(&h->state.clkInactivity);

            /* Unregister the wakeup notify callback */
            Power_unregisterNotify(&RF_wakeupNotifyObj);

            if (h->clientConfig.bPowerUpXOSC == true)
            {
                Power_releaseDependency(PowerCC26XX_XOSC_HF);
            }
        }
        else
        {
            /* If we're the current RF client, stop being it */
            if (pCurrClient == h)
            {
                pCurrClient = NULL;
            }

            /* Exit critical section */
            HwiP_restore(key);

            /* Unregister client specific RTOS objects */
            SemaphoreP_destruct(&h->state.semSync);
            ClockP_destruct(&h->state.clkInactivity);
        }
    }
}


uint32_t RF_getCurrentTime(void)
{
    uint32_t key = HwiP_disable();
    if (bRadioActive)
    {
        uint32_t ratCnt;
        ratCnt = HWREG(RFC_RAT_BASE + RFC_RAT_O_RATCNT);
        HwiP_restore(key);
        return ratCnt;
    }
    else
    {
        HwiP_restore(key);
        uint64_t nRtc;
        nRtc  = AONRTCCurrent64BitValueGet();
        /* Conservatively assume that we are just about to increment the RTC */
        nRtc += RF_RTC_TICK_INC;
        /* Scale with the 4 MHz that the RAT is running */
        nRtc *= RF_SCALE_RTC_TO_4MHZ;
        /* Add the RAT offset for RTC==0 */
        nRtc += ((uint64_t)opRatSync.start.rat0) << RF_SHIFT_32_BITS;
        return (uint32_t)(nRtc >> RF_SHIFT_32_BITS);
    }
}


RF_CmdHandle RF_postCmd(RF_Handle h, RF_Op* pOp, RF_Priority ePri, RF_Callback pCb, RF_EventMask bmEvent)
{
    /* Get hold of an RF command container (within critical section) */
    RF_Cmd* pCmd;
    RF_CmdHandle ch;

    /* Assert */
    DebugP_assert(h != NULL);
    DebugP_assert(pOp != NULL);

    uint32_t key = HwiP_disable();

    /* Try to allocate container and handle nSeqPost/nSeqDone overflow */
    if (((cmdQ.nSeqPost + N_CMD_MODMASK + 1 - cmdQ.nSeqDone) & N_CMD_MODMASK) < N_CMD_POOL)
    {
        /* Allocate RF_Op container */
        cmdQ.nSeqPost = (cmdQ.nSeqPost + 1) & N_CMD_MODMASK;
        ch = cmdQ.nSeqPost;

        pCmd = allocCmd(ch);
    }
    else
    {
        /* No more RF command containers available -> set status and return error code */
        HwiP_restore(key);
        return (RF_CmdHandle)RF_ALLOC_ERROR;
    }

    /* Stop inactivity clock if running */
    ClockP_stop(ClockP_handle(&h->state.clkInactivity));

    /* Populate RF_Op container */
    pCmd->pOp = pOp;
    pCmd->ePri = ePri;
    pCmd->pCb = pCb;
    pCmd->ch = ch;
    pCmd->pClient = h;
    pCmd->bmEvent = bmEvent & ~RF_INTERNAL_IFG_MASK;
    pCmd->flags = 0;

    if (cmdQ.pPend)
    {
        /* Submit to pending command queue */
        if (Q_push(&cmdQ.pPend, pCmd) == false)
        {
            cmdQ.nSeqPost = (N_CMD_MODMASK + cmdQ.nSeqPost) & N_CMD_MODMASK;
            HwiP_restore(key);
            return (RF_CmdHandle)RF_ALLOC_ERROR;
        }

        h->state.chLastPosted = ch;
        pCmd->flags |= RF_CMD_ALLOC_FLAG;
        if (h->state.bYielded == true)
        {
            h->state.bYielded = false;
            bInitAvailRatCh = false;
        }
        HwiP_restore(key);
        return ch;
    }

    /* Submit to pending command queue */
    if (Q_push(&cmdQ.pPend, pCmd) == false)
    {
        cmdQ.nSeqPost = (N_CMD_MODMASK + cmdQ.nSeqPost) & N_CMD_MODMASK;
        HwiP_restore(key);
        return (RF_CmdHandle)RF_ALLOC_ERROR;
    }

    h->state.chLastPosted = ch;
    pCmd->flags |= RF_CMD_ALLOC_FLAG;

    /* Reset yield flag */
    if (h->state.bYielded == true)
    {
        h->state.bYielded = false;
        bInitAvailRatCh = false;
    }

    if (bYieldPwrDnCalled && bRadioActive)
    {
        bYieldPwrDnCancel = true;
        bYieldPwrDnCalled = false;
        bInitAvailRatCh = false;
        nActiveClients++;
    }
    HwiP_restore(key);

    /* Trigger dispatcher HWI if there is no running command */
    if (!cmdQ.pCurrCmd)
    {
        if (bRadioActive)
        {
            HwiP_post(INT_RFC_CPE_0);
        }
        else if (!bPowerUpActive)
        {
            /* Wakeup radio. FSM will trigger dispatch again when powered up */
            key = HwiP_disable();
            if (bPowerDownActive)
            {
                bRestart = true;
                HwiP_restore(key);
            }
            else if ((dispatchInUs = dispatchOpWhen(pCmd->pOp)))
            {
                uint32_t timeout = MAX((dispatchInUs-(pCurrClient->clientConfig.nPowerUpDuration))/ ClockP_tickPeriod, 1);
                ClockP_setTimeout(ClockP_handle(&clkPowerUp), timeout);
                ClockP_start(ClockP_handle(&clkPowerUp));
                HwiP_restore(key);
            }
            else
            {
                HwiP_restore(key);
                SwiP_or(&swiFsm, Fsm_EventWakeup);
            }
        }
    }
    return ch;
}

RF_EventMask RF_pendCmd(RF_Handle h, RF_CmdHandle ch, RF_EventMask bmEvent)
{
    /* Assert */
    DebugP_assert(h != NULL);
    DebugP_assert(ch >= 0);

    /* Check whether command has finished, if not override command callback */
    uint32_t key = SwiP_disable();

    RF_Cmd* pCmd = getCmd(ch);

    /* Check if null */
    if (!pCmd)
    {
        SwiP_restore(key);
        return RF_EventLastCmdDone;
    }

    /* Check if aborted */
    if (pCmd->flags & (RF_CMD_ABORTED_FLAG | RF_CMD_STOPPED_FLAG | RF_CMD_CANCELLED_FLAG))
    {
        SwiP_restore(key);
        while(pCmd->flags & (RF_CMD_ABORTED_FLAG | RF_CMD_STOPPED_FLAG | RF_CMD_CANCELLED_FLAG));
        return RF_EventLastCmdDone;
    }

    /* Command has still not finished, override callback with one that posts to semaphore */
    if (pCmd->pCb != syncCb)
    {
        h->state.pCbSync = (void*)pCmd->pCb;
        pCmd->pCb = syncCb;
    }

    h->state.eventSync = bmEvent;
    SwiP_restore(key);

    /* Wait for semaphore */
    SemaphoreP_pend(&h->state.semSync, SemaphoreP_WAIT_FOREVER);

    /* Clear user cb */
    h->state.pCbSync = NULL;

    /* Return command event */
    return h->state.unpendCause;
}


RF_EventMask RF_runCmd(RF_Handle h, RF_Op* pOp, RF_Priority ePri, RF_Callback pCb, RF_EventMask bmEvent)
{
    if (pCb == NULL)
    {
        pCb = syncCb;
    }

    RF_CmdHandle ch = RF_postCmd(h, pOp, ePri, pCb, bmEvent);

    if (ch<0)
    {
        return RF_EventCmdError;
    }
    return RF_pendCmd(h, ch, (RF_EventLastCmdDone | RF_EventCmdAborted | RF_EventCmdStopped | RF_EventCmdCancelled));
}


void RF_yield(RF_Handle h)
{
    /* Assert */
    DebugP_assert(h != NULL);

    uint32_t key = HwiP_disable();

    /* Set flag to re-initalize the available RAT channels
       Limitation : assumes that any currently running RAT activity will be ignored to start power-down */
    bInitAvailRatCh = true;

    /* Check for nSeqDone overflow */
    if ((cmdQ.nSeqPost - h->state.chLastPosted) < 0)
    {
        if((h->state.chLastPosted <= cmdQ.nSeqDone) || (cmdQ.nSeqPost >= cmdQ.nSeqDone))
        {
            /* All commands are done -> stop inactivity timer, decrement active
               client count and potentially signal powerdown to FSM */
            ClockP_stop(ClockP_handle(&h->state.clkInactivity));
            decActiveClientCnt(0);
            if (bRadioActive)
            {
                bYieldPwrDnCalled = true;
            }
            HwiP_restore(key);
            return;
        }
    }
    else
    {
        if(h->state.chLastPosted == cmdQ.nSeqDone)
        {
            /* All commands are done -> stop inactivity timer, decrement active
               client count and potentially signal powerdown to FSM */
            ClockP_stop(ClockP_handle(&h->state.clkInactivity));
            decActiveClientCnt(0);
            if (bRadioActive)
            {
                bYieldPwrDnCalled = true;
            }
            HwiP_restore(key);
            return;
        }
    }

    /* There are still client commands that haven't finished
       -> set flag to indicate immediate powerdown when last command is done */
    h->state.bYielded = true;

    /* Reset the flag to ignore any active RAT events */
    bRatActive = false;
    HwiP_restore(key);
}

RF_Stat RF_cancelCmd(RF_Handle h, RF_CmdHandle ch, uint8_t mode)
{
    return abortCmd(h, ch, mode);
}

RF_Stat RF_flushCmd(RF_Handle h, RF_CmdHandle ch, uint8_t mode)
{
    return (abortCmd(h, ch, (mode | RF_ABORT_CANCEL_MULTIPLE_CMD)));
}

static RF_Stat abortCmd(RF_Handle h, RF_CmdHandle ch, uint8_t type)
{
    /* Assert */
    DebugP_assert(h != NULL);

    /* Initialize local variables */
    RF_Cmd* pCmdPend = Q_peek(&cmdQ.pPend);
    RF_CmdDirImm* pCmdDirImm = &cmdDirImm;
    bool bGraceful = type & RF_ABORT_CANCEL_SINGLE_CMD;
    RF_Stat status = RF_StatInvalidParamsError;

    /* Check if current command */
    uint32_t key = HwiP_disable();
    if (ch == RF_CMDHANDLE_FLUSH_ALL)
    {
        if (cmdQ.pCurrCmd)
        {
            ch = cmdQ.pCurrCmd->ch;
        }
        else
        {
            ch = cmdQ.nSeqDone + 1;
        }
        pCmdDirImm->flags = 0;
    }
    RF_Cmd* pCmd = getCmd(ch);
    if (pCmd && (pCmd == cmdQ.pCurrCmd))
    {
        /* Clear all pending commands */
        if (type & RF_ABORT_CANCEL_MULTIPLE_CMD)
        {
            pCmdPend = cmdQ.pPend;
            cmdQ.pPend = NULL;
        }

        /* Flag that operation has been aborted. */
        pCmd->flags |= (1 << bGraceful);

        uint32_t keySwi = SwiP_disable();

        RFCDoorbellSendTo((bGraceful) ? CMDR_DIR_CMD(CMD_STOP) : CMDR_DIR_CMD(CMD_ABORT));

        HwiP_restore(key);

        /* Call callbacks */
        if (type & RF_ABORT_CANCEL_MULTIPLE_CMD)
        {
            while (pCmdPend)
            {
                pCmdPend->flags |= RF_CMD_CANCELLED_FLAG;
                Q_push(&cmdQ.pDone, Q_pop(&pCmdPend));
            }
        }

        SwiP_restore(keySwi);
        status = RF_StatSuccess;
    }
    else if (pCmd)
    {
        if (pCmdPend == pCmd)
        {
            /* Cancel the first pending command */
            ClockP_stop(ClockP_handle(&clkPowerUp));
            if (type & RF_ABORT_CANCEL_MULTIPLE_CMD)
            {
                cmdQ.pPend = NULL;
            }
            else
            {
                cmdQ.pPend = cmdQ.pPend->pNext;
            }
            HwiP_restore(key);

            do {
                /* Call callbacks */
                pCmdPend->flags |= RF_CMD_CANCELLED_FLAG;
                Q_push(&cmdQ.pDone, Q_pop(&pCmdPend));
            } while ((pCmdPend) && (type & RF_ABORT_CANCEL_MULTIPLE_CMD));

            status = RF_StatSuccess;
        }
        else
        {
            /* Search for the command to be cancelled in the pending queue */
            while (pCmdPend)
            {
                if (pCmdPend->pNext == pCmd)
                {
                    ClockP_stop(ClockP_handle(&clkPowerUp));
                    RF_Cmd* pTemp = (RF_Cmd*)pCmdPend->pNext;

                    if (type & RF_ABORT_CANCEL_MULTIPLE_CMD)
                    {
                        pCmdPend->pNext = NULL;
                        pCmdPend = pTemp;
                    }
                    else
                    {
                        pCmdPend->pNext = pTemp->pNext;
                        pTemp->pNext = NULL;
                        pCmdPend = pTemp;
                    }
                    while(pCmdPend)
                    {
                      pCmdPend->flags |= RF_CMD_CANCELLED_FLAG;
                      Q_push(&cmdQ.pDone, Q_pop(&pCmdPend));
                    }
                    HwiP_restore(key);
                    status = RF_StatSuccess;
                    break;
                }
                else
                {
                    pCmdPend = (RF_Cmd*)pCmdPend->pNext;
                }
            }
            if (status != RF_StatSuccess)
            {
                HwiP_restore(key);
            }
        }

        /* If power up active is set, the last command done will be posted when power setup is complete */
        if(!bPowerUpActive)
        {
            SwiP_or(&swiFsm, Fsm_EventLastCommandDone);
        }
    }
    else
    {
        HwiP_restore(key);
    }

    return(status);
}


void RF_Params_init(RF_Params *params)
{
    /* Assert */
    DebugP_assert(params != NULL);

    *params = RF_defaultParams;
}


static RF_Stat postDirImmCmd(uint32_t pCmdStruct, bool bStartRadio)
{
    uint32_t key = HwiP_disable();
    if (bRadioActive)
    {
        /* Run command now */
        uint32_t cmdstaVal = RFCDoorbellSendTo(pCmdStruct);
        HwiP_restore(key);
        if ((cmdstaVal & RF_CMDSTA_REG_VAL_MASK) == CMDSTA_Done)
        {
            return RF_StatCmdDoneSuccess;
        }
        else
        {
            return RF_StatCmdDoneError;
        }
    }
    else
    {
        HwiP_restore(key);
        return RF_StatRadioInactiveError;
    }
}

RF_Stat RF_runImmediateCmd(RF_Handle h, uint32_t* pCmdStruct)
{
    /* Assert  */
    DebugP_assert(h != NULL);

    /* Return Error case  */
    if (h != pCurrClient)
    {
        return RF_StatInvalidParamsError;
    }

    /* Run command  */
    return postDirImmCmd((uint32_t)pCmdStruct, false);
}

RF_Stat RF_runDirectCmd(RF_Handle h, uint32_t cmd)
{
    /* Assert  */
    DebugP_assert(h != NULL);

    /* Return Error case  */
    if (h != pCurrClient)
    {
        return RF_StatInvalidParamsError;
    }

    /* Run command now */
    return postDirImmCmd(cmd, false);
}

int8_t RF_getRssi(RF_Handle h)
{
    if ((h!= pCurrClient) || (!bRadioActive))
    {
        return RF_GET_RSSI_ERROR_VAL;
    }

    uint32_t key = HwiP_disable();
    uint32_t cmdsta = RFCDoorbellSendTo(CMDR_DIR_CMD(CMD_GET_RSSI));
    HwiP_restore(key);

    if ((cmdsta & RF_CMDSTA_REG_VAL_MASK) == CMDSTA_Done)
    {
        return (int8_t)((cmdsta >> RF_SHIFT_16_BITS) & RF_CMDSTA_REG_VAL_MASK);
    }
    else
    {
        return (int8_t)RF_GET_RSSI_ERROR_VAL;
    }
}

RF_Stat RF_getInfo(RF_Handle h, RF_InfoType type, RF_InfoVal *pValue)
{
    RF_Stat status = RF_StatSuccess;

    uint32_t key = HwiP_disable();
    switch(type)
    {
        case RF_GET_CURR_CMD:
            if (cmdQ.pCurrCmd)
            {
                pValue->ch = cmdQ.pCurrCmd->ch;
            }
            else
            {
                status = RF_StatError;
            }
            break;
        case RF_GET_AVAIL_RAT_CH:
            if (bInitAvailRatCh == false)
            {
                pValue->availRatCh = cmdDirImm.availRatCh;
            }
            else
            {
                status = RF_StatError;
            }
            break;
        case RF_GET_RADIO_STATE:
            pValue->bRadioState = bRadioActive;
            break;
        default:
            status = RF_StatInvalidParamsError;
            break;
    }
    HwiP_restore(key);

    return status;
}

RF_Op* RF_getCmdOp(RF_Handle h, RF_CmdHandle cmdHnd)
{
    RF_Cmd* pCmd = getCmd(cmdHnd);
    if (pCmd)
        return (pCmd->pOp);
    else
        return NULL;
}

static int8_t ratChanAlloc(RF_CmdDirImm *pDirImmCmd)
{
    int8_t i;
    if (bInitAvailRatCh == false)
    {
        for (i=0; i<15; i++)
        {
            /* Update free RAT chan bitmap and create HW Event for allocated channel */
            if (pDirImmCmd->availRatCh & (1 << i))
            {
                pDirImmCmd->availRatCh &= ~(1<<i);
                HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFHWIEN) |= (1<<(i+RFC_DBELL_RFHWIFG_RATCH0_BITN));
                return i;
            }
        }
    }

    return (int8_t)RF_ALLOC_ERROR;
}

static void ratChanFree(int8_t ratChannelNum)
{
    RF_CmdDirImm* pCmd = &cmdDirImm;

    uint32_t key = HwiP_disable();
    if (!((pCmd->flags & RF_RAT_RPT_CPT_FLAG) && (ratChannelNum == pCmd->rptCptRatChNum)))
    {
        pCmd->availRatCh |= (1 << ratChannelNum);
        HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFHWIEN) &= ~(1 << (ratChannelNum + RFC_DBELL_RFHWIFG_RATCH0_BITN));
        if (!HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFHWIEN))
        {
            bRatActive = false;
        }
    }

    HwiP_restore(key);
}

int8_t RF_ratCompare(RF_Handle h, rfc_CMD_SET_RAT_CMP_t* pCmdStruct, \
                            uint32_t compareTime, RF_Callback pRatCb)
{
    if (h != pCurrClient)
    {
        return RF_StatInvalidParamsError;
    }

    if (!bRadioActive)
    {
        return RF_ALLOC_ERROR;
    }

    RF_CmdDirImm* pDirImmCmd = &cmdDirImm;

    pCmdStruct->commandNo = CMD_SET_RAT_CMP;
    pCmdStruct->compareTime = compareTime;

    int8_t ratCh = ratChanAlloc(pDirImmCmd);

    if (ratCh >= 0)
    {
        pCmdStruct->ratCh = ratCh;
        RF_Stat status = postDirImmCmd((uint32_t)pCmdStruct, false);
        if ((status == RF_StatCmdDoneError) || (status == RF_StatRadioInactiveError))
        {
            ratChanFree(ratCh);
            ratCh = RF_ALLOC_ERROR;
        }
        else
        {
            pDirImmCmd->pRatCb[ratCh - RF_RAT_CH_OFFSET] = pRatCb;
            pDirImmCmd->pClient = h;
            bRatActive = true;
        }
    }

    return ratCh;
}

int8_t RF_ratCapture(RF_Handle h, uint16_t config, RF_Callback pRatCb)
{
    if (h != pCurrClient)
    {
        return RF_StatInvalidParamsError;
    }

    if (!bRadioActive)
    {
        return RF_ALLOC_ERROR;
    }

    RF_CmdDirImm* pDirImmCmd = &cmdDirImm;

    int8_t ratCh = ratChanAlloc(pDirImmCmd);

    if (ratCh >= 0)
    {
        uint32_t key = HwiP_disable();
        config |= (ratCh << RF_SHIFT_8_BITS) & RF_RAT_CPT_CONFIG_RATCH_MASK;
        uint32_t cmdstaVal = RFCDoorbellSendTo((uint32_t)CMDR_DIR_CMD_2BYTE(CMD_SET_RAT_CPT,
                                                                            config));
        if ((cmdstaVal & RF_CMDSTA_REG_VAL_MASK) == CMDSTA_Done)
        {
            if (config & RF_RAT_CPT_CONFIG_REP_MASK)
            {
                HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFHWIFG) &= ~(1 << (ratCh + RFC_DBELL_RFHWIFG_RATCH0_BITN));
                pDirImmCmd->rptCptRatChNum = ratCh;
                pDirImmCmd->flags |= RF_RAT_RPT_CPT_FLAG;
            }
            pDirImmCmd->pClient = h;
            pDirImmCmd->pRatCb[ratCh - RF_RAT_CH_OFFSET] = pRatCb;
            bRatActive = true;
        }
        else
        {
            ratChanFree(ratCh);
            ratCh = RF_ALLOC_ERROR;
        }
        HwiP_restore(key);
    }
    return ratCh;
}

RF_Stat RF_ratHwOutput(RF_Handle h, uint16_t config)
{
    if (h != pCurrClient)
    {
        return RF_StatInvalidParamsError;
    }
    /* Run command now */
    return postDirImmCmd(((uint32_t)CMDR_DIR_CMD_2BYTE(CMD_SET_RAT_OUTPUT, config)), false);
}


RF_Stat RF_ratDisableChannel(RF_Handle h, int8_t ratChannelNum)
{
    if (h != pCurrClient)
    {
        return RF_StatInvalidParamsError;
    }

    RF_CmdDirImm* pCmd = &cmdDirImm;
    if ((pCmd->flags & RF_RAT_RPT_CPT_FLAG) && (ratChannelNum == pCmd->rptCptRatChNum))
    {
        pCmd->flags &= ~RF_RAT_RPT_CPT_FLAG;
        pCmd->rptCptRatChNum = 0;
    }
    uint32_t key = HwiP_disable();
    ratChanFree(ratChannelNum);
    uint32_t cmdStaVal = RFCDoorbellSendTo(CMDR_DIR_CMD_2BYTE(CMD_DISABLE_RAT_CH, (ratChannelNum << 8)));
    HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFHWIFG) &= ~(1 << (ratChannelNum + RFC_DBELL_RFHWIFG_RATCH0_BITN));
    HwiP_restore(key);

    if ((cmdStaVal & RF_CMDSTA_REG_VAL_MASK) == CMDSTA_Done)
    {
        return RF_StatCmdDoneSuccess;
    }
    else
    {
        return RF_StatCmdDoneError;
    }
}

RF_Stat RF_control(RF_Handle h, int8_t ctrl, void *args)
{
    RF_Stat ret = RF_StatError;

    uint32_t key = HwiP_disable();

    switch (ctrl)
    {
        case RF_CTRL_SET_INACTIVITY_TIMEOUT:
            /* Set inactivity timeout */
            h->clientConfig.nInactivityTimeout = *(uint32_t *)args;
            ret = RF_StatSuccess;
            break;

        case RF_CTRL_UPDATE_SETUP_CMD:
            /* Update setup command */
            h->clientConfig.bUpdateSetup = true;

            /* Since the next setup command will configure analog, need extra nPowerupDuration */
            h->clientConfig.nPowerUpDuration += RF_ANALOG_CFG_TIME_US;
            ret = RF_StatSuccess;
            break;

        case RF_CTRL_SET_POWERUP_DURATION_MARGIN:
            h->clientConfig.nPowerUpDurationMargin = *(uint32_t *)args;
            ret = RF_StatSuccess;
            break;

        case RF_CTRL_SET_HWI_PRIORITY:
            /* Changing priorities during run-time has constraints.
               To not mess up with the RF driver, we require the RF
               driver to be inactive. */
            if (bPowerUpActive || bRadioActive ||
                bPowerDownActive || (Q_peek(&cmdQ.pPend)))
            {
                ret = RF_StatBusyError;
            }
            else
            {
                HwiP_setPriority(INT_RFC_CPE_0, *(uint32_t *)args);
                HwiP_setPriority(INT_RFC_HW_COMB, *(uint32_t *)args);

                ret = RF_StatSuccess;
            }
            break;

        case RF_CTRL_SET_SWI_PRIORITY:
            /* Changing priorities during run-time has constraints.
               To not mess up with the RF driver, we require the RF
               driver to be inactive. */
            if (bPowerUpActive || bRadioActive ||
                bPowerDownActive || (Q_peek(&cmdQ.pPend)))
            {
                ret = RF_StatBusyError;
            }
            else
            {
                SwiP_setPriority(&swiFsm, *(uint32_t *)args);
                SwiP_setPriority(&swiHw, *(uint32_t *)args);

                ret = RF_StatSuccess;
            }
            break;

        default:
            ret = RF_StatInvalidParamsError;
            break;
    }

    HwiP_restore(key);

    return ret;
}
