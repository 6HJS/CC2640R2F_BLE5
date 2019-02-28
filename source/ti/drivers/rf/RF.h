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
/** ============================================================================
 *  @file       RF.h
 *
 *  @brief      RF driver for the CC26/13xx family
 *
 *  # Driver include #
 *  The RF header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/rf/RF.h>
 *  @endcode
 *
 *  # Overview #
 *  The RF driver allows clients (applications or other drivers) to control the
 *  radio of the device. The radio interface can either be blocking or non-blocking.
 *  Furthermore, the RF driver manages the RF power domain and tries to power down
 *  when the radio is no longer needed. It also allows clients to setup callbacks
 *  when radio operations are completed and as such allows for fully interrupt based
 *  radio software. Callback is executed in software interrupt (SWI) context, therefore
 *  users should only executed minimum codes inside the callback.
 *  When integrated with applications, it is recommended users should
 *  set the RF software interrupt (SWI) to the highest priority in order to ensure
 *  proper operaton of the RF driver.
 *
 *  The RF driver can be used in either single mode only or multi-mode (dual-mode). The same
 *  RF.h is used for both modes. RFCC26XX_multiMode.c supports both single mode
 *  and multi-mode operation. RFCC26XX_singleMode.c supports single mode operation only.
 *
 *  # Power Management #
 *  The driver sets RF Core power constraints (in the Power driver) when radio is required to
 *  keep the device out of standby. When the operation has finished, power constraints are released.
 *  The power state of the RF Core is solely controlled by the RF driver based on
 *  radio operation state. The following use-cases will power down the RF Core:
 *
 *  1. nInactivityTimeout in RF_Params specifies the time RF Core will power down after
 *  completion of a radio command and no other commands pending in the command queue. The default
 *  is BIOS_WAIT_FOREVER which means infinite timeout and no power down after end of command.  <br>
 *  2. Schedule (or post) a radio command using RF_scheduleCmd() or RF_postCmd() APIs
 *  with the startTime sufficiently far in the future with startTrigger.triggerType set to TRIG_ABSTIME.
 *  This will power down RF Core and automatically power up before the startTime to execute the
 *  scheduled command. <br>
 *  3. Force a power down using RF_yield() API. This will power down RF after all
 *  pending radio commands are complete. <br>
 *  4. RF_close() API will power down the radio if all the radio access request from each client is
 *  done. <br>
 *
 *  Note the above three cases will execute power down independently. For example when
 *  nInactivityTimeout is infinity, if a radio command is scheduled in sufficient future,
 *  the RF driver will power down and wake up at the scheduled startTime. Since RAT channels are
 *  part of the RF Core, they will not be available when the RF Core is off. The RF Driver
 *  maintains the sync between the RAT and RTC during power-up and power-down using the
 *  CMD_SYNC_RAT_START and CMD_SYNC_RAT_STOP radio operations.
 *
 *  # Supported Functions #
 *  | Generic API function  | Description                                        |
 *  |-----------------------|----------------------------------------------------|
 *  | RF_open()             | Open client connection to RF driver                |
 *  | RF_close()            | Close client connection to RF driver               |
 *  | RF_getCurrentTime()   | Return current radio timer value                   |
 *  | RF_scheduleCmd()      | Schedule an RF operation to the command queue      |
 *  | RF_postCmd()          | Post an RF operation to the command queue          |
 *  | RF_pendCmd()          | Wait for posted command to complete                |
 *  | RF_runCmd()           | Runs synchronously a (chain of) RF operation(s)    |
 *  | RF_flushCmd()         | Abort previously submitted command, clear queue    |
 *  | RF_requestAccess()    | Request access to radio resource                   |
 *  | RF_yield()            | Signal that the radio will not be used for a while |
 *  | RF_cancelCmd()        | Abort/stop/cancel single cmd in command queue      |
 *  | RF_getRssi()          | Get rssi value                                     |
 *  | RF_runImmediateCmd()  | Send any immediate command                         |
 *  | RF_runDirectCmd()     | Send any direct command                            |
 *  | RF_getInfo()          | Get value for some RF driver parameters            |
 *  | RF_ratCapture()       | Run RAT capture command                            |
 *  | RF_ratCompare()       | Run RAT compare command                            |
 *  | RF_ratHwOutput()      | Run RAT HW Output command                          |
 *  | RF_ratDisableChannel()| Disable RAT channel                                |
 *  | RF_control()          | Set RF control parameters                          |
 *
 *
 *  ## RF Driver Functionality #
 *  - IEEE is only supported in dual mode code, RFCC26XX_multiMode.c, by RF_scheduleCmd.
 *  - RF_cancelCmd and RF_flushCmd do not support CMD_IEEE_ABORT_FG and CMD_IEEE_STOP_FG.
 *
 * ## Use Cases @anchor USE_CASES_RF ##
 * ### Basic Single Mode Operation #
 *  Schedule a proprietary radio command.
 *  @code
 *  RF_Object rfObject;
 *  RF_Handle rfHandle;
 *  RF_Params rfParams;
 *
 *  RF_Mode rfMode =
 *  {
 *      .rfMode      = RF_MODE_PROPRIETARY_SUB_1,
 *      .cpePatchFxn = &rf_patch_cpe_genfsk,
 *      .mcePatchFxn = 0,
 *      .rfePatchFxn = 0,
 *  };
 *
 *  // Init RF and specify non-default parameters
 *  RF_Params_init(&rfParams);
 *  rfParams.nInactivityTimeout = 500; // 500us
 *
 *  // Open a proprietary RF handle
 *  rfHandle = RF_open(rfObjet, &rfMode, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams);
 *
 *  // Run a proprietary Fs command
 *  RF_runCmd(rfHandle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);
 *
 *  // Schedule a proprietary TX command
 *  RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropTx, RF_PriorityNormal, &callback, RF_EventLastCmdDone);
 *
 *  @endcode
 *
 * ### Basic Dual Mode Operation #
 *  Schedule BLE and proprietary radio commands.
 *  @code
 *  RF_Object rfObject_ble;
 *  RF_Object rfObject_prop;
 *
 *  RF_Handle rfHandle_ble, rfHandle_prop;
 *  RF_Params rfParams_ble, rfParams_prop;
 *  RF_ScheduleCmdParams schParams_ble, schParams_prop;
 *
 *  RF_Mode rfMode_ble =
 *  {
 *      .rfMode      = RF_MODE_MULTIPLE,  // rfMode for dual mode
 *      .cpePatchFxn = &rf_patch_cpe_ble,
 *      .mcePatchFxn = 0,
 *      .rfePatchFxn = &rf_patch_rfe_ble,
 *  };
 *
 *  RF_Mode rfMode_prop =
 *  {
 *      .rfMode      = RF_MODE_MULTIPLE,  // rfMode for dual mode
 *      .cpePatchFxn = &rf_patch_cpe_genfsk,
 *      .mcePatchFxn = 0,
 *      .rfePatchFxn = 0,
 *  };
 *
 *  // Init RF and specify non-default parameters
 *  RF_Params_init(&rfParams_ble);
 *  rfParams_ble.nInactivityTimeout = 200;     // 200us
 *
 *  RF_Params_init(&rfParams_prop);
 *  rfParams_prop.nInactivityTimeout = 200;    // 200us
 *
 *  // Configure RF schedule command parameters
 *  schParams_ble.priority  = RF_PriorityNormal;
 *  schParams_ble.endTime   = 0;
 *  schParams_prop.priority = RF_PriorityNormal;
 *  schParams_prop.endTime  = 0;
 *
 *  // Open BLE and proprietary RF handles
 *  rfHandle_ble  = RF_open(rfObj_ble,  &rfMode_ble,  (RF_RadioSetup*)&RF_cmdRadioSetup,        &rfParams_ble);
 *  rfHandle_prop = RF_open(rfObj_prop, &rfMode_prop, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams_prop);
 *
 *  // Run a proprietary Fs command
 *  RF_runCmd(rfHandle_pro, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, NULL);
 *
 *  // Schedule a proprietary RX command
 *  RF_scheduleCmd(rfHandle_pro, (RF_Op*)&RF_cmdPropRx, &schParams_prop, &prop_callback, RF_EventRxOk);
 *
 *  // Schedule a BLE advertizer command
 *  RF_scheduleCmd(rfHandle_ble, (RF_Op*)&RF_cmdBleAdv, &schParams_ble, &ble_callback,
 *                 (RF_EventLastCmdDone | RF_EventRxEntryDone | RF_EventTxEntryDone));
 *
 *  @endcode
 *
 * ### Get Dual Mode Schedule Map #
 *  Get dual mode schedule map including timing and priority information for access requests and commands.
 *  @code
 *
 *  RF_ScheduleMap rfSheduleMap;
 *  RF_InfoVal     rfGetInfoVal;
 *
 *  // Get schedule map
 *  rfGetInfoVal.pScheduleMap = &rfScheduleMap;
 *  RF_getInfo(NULL, RF_GET_SCHEDULE_MAP, &rfGetInfoVal);
 *
 *  // RF_scheduleMap includes the following information:
 *  // (RF_NUM_SCHEDULE_ACCESS_ENTRIES (default = 2)) entries of access request information
 *  // (RF_NUM_SCHEDULE_COMMAND_ENTRIES (default = 3)) entries of radio command information
 *  // Each entry has the type of RF_ScheduleMapElement.
 *
 *  @endcode
 *
 * ### Basic Error Handling #
 *  The RF driver provides a callback mechanism to handle error events. The callback function can be redirected by the user allowing
 *  customization of behaviour for certain scenarios.
 *
 *  The registered error callback function is stored and invoked in the clients context and executed as an SWI. While single-mode applications can only have one error callback
 *  registered, dual mode applications can register two error callbacks (one for each clients).
 *
 *  The customized callback function can be registered through the rfParams.pErrCb pointer passed to the RF_open() API.
 *  In case no user specific error callback is provided, the defaultCallback() implemented by the RF driver will be used.
 *  @code
 *
 *  RF_Object    rfObject;
 *  RF_Handle    rfHandle;
 *  RF_Params    rfParams;
 *
 *  RF_Mode rfMode =
 *  {
 *      .rfMode      = RF_MODE_PROPRIETARY_SUB_1,
 *      .cpePatchFxn = &rf_patch_cpe_genfsk,
 *      .mcePatchFxn = 0,
 *      .rfePatchFxn = 0,
 *  };
 *
 *  // Init the parameters to default value
 *  RF_Params_init(&rfParams);
 *
 *  // Register the user defined callback (to be invoked in case of an error)
 *  rfParams.pErrCb = &myErrorCallback;
 *
 *  // Open a proprietary RF handle
 *  rfHandle = RF_open(rfObject, &rfMode, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams);
 *  @endcode
 *
 *  If an error occurs while the defaultCallback() is registered, the error will be simply ignored.
 *
 *  @code
 *  static void defaultCallback(RF_Handle rHandle, int16_t errCode, RF_EventMask eventMask)
 *  {
 *      // Do nothing
 *  }
 *  @endcode
 *
 *  Certain error events are considered to be recoverable. The relevant examples are listed in the Errata.
 *  In case a user specified error callback is provided, the implementation must follow the prototype of defaultCallback().
 *  @code
 *  void myErrorCallback(RF_Handle rfHandle, int16_t errCode, RF_EventMask eventMask)
 *  {
 *     switch(errCode)
 *     {
 *        case(RF_ERROR_CMDFS_SYNTH_PROG): // User specified behavior in case the frequency calibration fails.
 *                                         break;
 *     }
 *  }
 *
 *  @endcode
 *
 *  =============================================================================
 */
#ifndef ti_drivers_rf__include
#define ti_drivers_rf__include

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/rf_common_cmd.h)
#include DeviceFamily_constructPath(driverlib/rf_prop_cmd.h)
#include DeviceFamily_constructPath(driverlib/rf_ble_cmd.h)

/**
 *  @addtogroup RF_EventMask
 *  RF_Event* macros are RF events defined in the
 *  RF.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/rf/RF.h>
 *  @endcode
 *  @{
 */

/**
 *  @addtogroup RF_Core_Events
 *  RF Events reported to callback functions, and used in RF_runCmd(), RF_pendCmd()
 *  @{
 */

#define   RF_EventCmdDone             (1<<0)   ///< Radio operation command finished
#define   RF_EventLastCmdDone         (1<<1)   ///< Last radio operation command in a chain finished
#define   RF_EventFGCmdDone           (1<<2)   ///< Last radio operation command in a chain finished
#define   RF_EventLastFGCmdDone       (1<<3)   ///< Last radio operation command in a chain finished
#define   RF_EventTxDone              (1<<4)   ///< Packet transmitted
#define   RF_EventTXAck               (1<<5)   ///< ACK packet transmitted
#define   RF_EventTxCtrl              (1<<6)   ///< Control packet transmitted
#define   RF_EventTxCtrlAck           (1<<7)   ///< Acknowledgement received on a transmitted control packet
#define   RF_EventTxCtrlAckAck        (1<<8)   ///< Acknowledgement received on a transmitted control packet, and acknowledgement transmitted for that packet
#define   RF_EventTxRetrans           (1<<9)   ///< Packet retransmitted
#define   RF_EventTxEntryDone         (1<<10)  ///< Tx queue data entry state changed to Finished
#define   RF_EventTxBufferChange      (1<<11)  ///< A buffer change is complete
#define   RF_EventRxOk                (1<<16)  ///< Packet received with CRC OK, payload, and not to be ignored
#define   RF_EventRxNOk               (1<<17)  ///< Packet received with CRC error
#define   RF_EventRxIgnored           (1<<18)  ///< Packet received with CRC OK, but to be ignored
#define   RF_EventRxEmpty             (1<<19)  ///< Packet received with CRC OK, not to be ignored, no payload
#define   RF_EventRxCtrl              (1<<20)  ///< Control packet received with CRC OK, not to be ignored
#define   RF_EventRxCtrlAck           (1<<21)  ///< Control packet received with CRC OK, not to be ignored, then ACK sent
#define   RF_EventRxBufFull           (1<<22)  ///< Packet received that did not fit in the Rx queue
#define   RF_EventRxEntryDone         (1<<23)  ///< Rx queue data entry changing state to Finished
#define   RF_EventDataWritten         (1<<24)  ///< Data written to partial read Rx buffer
#define   RF_EventNDataWritten        (1<<25)  ///< Specified number of bytes written to partial read Rx buffer
#define   RF_EventRxAborted           (1<<26)  ///< Packet reception stopped before packet was done
#define   RF_EventRxCollisionDetected (1<<27)  ///< A collision was indicated during packet reception
#define   RF_EventModulesUnlocked     (1<<29)  ///< As part of the boot process, the CM0 has opened access to RF core modules and memories
#define   RF_EventInternalError       (uint32_t)(1<<31)   ///< Internal error observed
#define   RF_EventMdmSoft             0x0000002000000000  ///< Modem Sync detected (MDMSOFT IFG)
/** @}*/

/**
 *  @addtogroup RF_Driver_Events
 *  RF Events generated by RF Driver. Should not be used as input to RF_postCmd()
 *  @{
 */
#define   RF_EventCmdCancelled        0x1000000000000000  ///< Command cancelled : RF driver event
#define   RF_EventCmdAborted          0x2000000000000000  ///< Command aborted : RF driver event
#define   RF_EventCmdStopped          0x4000000000000000  ///< Command stopped : RF driver event
#define   RF_EventCmdError            0x8000000000000000  ///< Command error : RF driver event
#define   RF_EventRatCh               0x0800000000000000  ///< RAT Channel interrupt : RF driver event
#define   RF_EventPowerUp             0x0400000000000000  ///< \deprecated RF power up event, will go away in future versions, see #RF_ClientEventPowerUpFinished instead

#define   RF_EventError               0x0200000000000000  ///< RF mode error event
#define   RF_EventCmdPreempted        0x0100000000000000  ///< Command preempted : RF Driver event
#define   RF_EventRadioFree           0x0080000000000000  ///< \deprecated Radio available to use, will go away in future versions, see #RF_ClientEventRadioFree instead
/** @}*/
/** @}*/

/**
 *  @addtogroup RF_CTRL
 *  RF_CTRL_* macros are control codes defined in the
 *  RF.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/rf/RF.h>
 *  @endcode
 *  @{
 */

/*!
 * @brief Control code used by RF_control to set inactivity timeout
 *
 * Setting this control allows RF to power down the radio upon completion of a radio
 * command after a specified timeout period (in us)
 * With this control code @b arg is a pointer to the timeout variable and returns RF_StatSuccess.
 */
#define RF_CTRL_SET_INACTIVITY_TIMEOUT            0
/*!
 * @brief Control code used by RF_control to update setup command
 *
 * Setting this control notifies RF that the setup command is to be updated, so that RF will take
 * proper actions when executing the next setup command.
 * Note the updated setup command will take effect in the next power up cycle when RF executes the
 * setup command. Prior to updating the setup command, user should make sure all pending commands
 * have completed.
 */
#define RF_CTRL_UPDATE_SETUP_CMD                  1
/*!
 * @brief Control code used by RF_control to set powerup duration margin
 *
 * Setting this control updates the powerup duration margin. Default is RF_DEFAULT_POWER_UP_MARGIN.
 */
#define RF_CTRL_SET_POWERUP_DURATION_MARGIN       2
/*!
 * @brief Control code used by RF_control to set max error tolerence for RAT/RTC
 *
 * Setting this control updates the error tol for how frequently the CMD_RAT_SYNC_STOP is sent.
 * Default is RF_DEFAULT_RAT_RTC_ERR_TOL_IN_US (5 us)
 * Client is recommeneded to change this setting before sending any commands.
 */
#define RF_CTRL_SET_RAT_RTC_ERR_TOL_VAL           3
/*!
 * @brief Control code used by RF_control to set power management
 *
 * Setting this control configures RF driver to enable or disable power management.
 * By default power management is enabled.
 * If disabled, once RF core wakes up, RF driver will not go to standby and will not power down RF core.
 * To configure power management, use this control to pass a parameter value of 0 to disable power management,
 * and pass a parameter value of 1 to re-enable power management.
 * This control is valid for dual mode code only. Setting this control when using single mode code has no effect
 * (power management always enabled).
 */
#define RF_CTRL_SET_POWER_MGMT                    4
/*!
 * @brief Control code used by RF_control to set the hardware interrupt priority level of the RF driver.
 *
 * This control code sets the hardware interrupt priority level that is used by the RF driver. Valid
 * values are INT_PRI_LEVEL1 (highest) until INT_PRI_LEVEL7 (lowest). The default interrupt priority is
 * set in the board support file. The default value is -1 which means "lowest possible priority".
 *
 * When using the TI-RTOS kernel, INT_PRI_LEVEL0 is reserved for zero-latency interrupts and must not be used.
 *
 * Execute this control code only while the RF core is powered down and the RF driver command queue is empty.
 * This is usually the case after calling RF_open(). Changing the interrupt priority level while the RF driver
 * is active will result in RF_StatBusyError being returned.
 *
 * Example:
 * @code
 * #include DeviceFamily_constructPath(driverlib/interrupt.h)
 *
 * int32_t hwiPriority = INT_PRI_LEVEL5;
 * RF_control(rfHandle, RF_CTRL_SET_HWI_PRIORITY, &hwiPriority);
 * @endcode
 */
#define RF_CTRL_SET_HWI_PRIORITY                  5
/*!
 * @brief Control code used by RF_control to set the software interrupt priority level of the RF driver.
 *
 * This control code sets the software interrupt priority level that is used by the RF driver. Valid
 * values are integers starting at 0 (lowest) until <tt>Swi_numPriorities - 1</tt> (highest). The default
 * interrupt priority is set in the board support file. The default value is 0 which means means
 * "lowest possible priority".
 *
 * Execute this control code only while the RF core is powered down and the RF driver command queue is empty.
 * This is usually the case after calling RF_open(). Changing the interrupt priority level while the RF driver
 * is active will result in RF_StatBusyError being returned.
 *
 * Example:
 * @code
 * #include <ti/sysbios/knl/Swi.h>
 *
 * // Set highest possible priority
 * uint32_t swiPriority = ~0;
 * RF_control(rfHandle, RF_CTRL_SET_SWI_PRIORITY, &swiPriority);
 * @endcode
 */
#define RF_CTRL_SET_SWI_PRIORITY                  6
/** @}*/

/**
 * @{
 */
// Other defines
#define   RF_GET_RSSI_ERROR_VAL       (-128)   ///< Error return value for RF_getRssi()
#define   RF_CMDHANDLE_FLUSH_ALL      (-1)     ///< RF command handle to flush all RF commands
#define   RF_ALLOC_ERROR              (-2)     ///< RF command or RAT channel allocation error
#define   RF_SCHEDULE_CMD_ERROR       (-3)     ///< RF command schedule error
#define   RF_ERROR_INVALID_RFMODE     (-256)   ///< Invalid RF_Mode. Used in error callback.
#define   RF_ERROR_CMDFS_SYNTH_PROG   (-257)   ///< Synthesizer error with CMD_FS. Used in error callback. If this error occurred in error callback, user needs to resend CMD_Fs to recover. See errata SWRA521.

#define RF_NUM_SCHEDULE_MAP_ENTRIES        5   ///< Number of schedule map entries. This is the sum of access request and scheduled command entries
#define RF_NUM_SCHEDULE_ACCESS_ENTRIES     2   ///< Number of access request entries
#define RF_NUM_SCHEDULE_COMMAND_ENTRIES    (RF_NUM_SCHEDULE_MAP_ENTRIES - RF_NUM_SCHEDULE_ACCESS_ENTRIES) ///< Number of scheduled command entries

#define RF_SCH_CMD_EXECUTION_TIME_UNKNOWN  0   ///< For unknown execution time for RF scheduler

/** @}*/

/// Alias for the data type of the header common to all radio operations
typedef rfc_radioOp_t RF_Op;


/// Struct defining operating mode of RF driver
typedef struct {
    uint8_t rfMode;             ///< Variable for device operation mode; must be set to RF_MODE_MULTIPLE for dual mode operation
    void (*cpePatchFxn)(void);  ///< Pointer to CPE patch function
    void (*mcePatchFxn)(void);  ///< Pointer to MCE patch function
    void (*rfePatchFxn)(void);  ///< Pointer to RFE patch function
} RF_Mode;


/// Priority of RF commands
typedef enum {
    RF_PriorityHighest = 2, ///< Highest, use sparingly
    RF_PriorityHigh    = 1, ///< High, time-critical commands in synchronous protocols
    RF_PriorityNormal  = 0, ///< Normal, usually best choice
} RF_Priority;

/// RF Stat reported as return value for RF_ratCmd(), RF_getRssi(). RF_setTxPwr(), RF_cancelCmd()
typedef enum {
    RF_StatBusyError,          ///< Cmd not executed as RF driver is busy.
    RF_StatRadioInactiveError, ///< Cmd not executed as radio is inactive.
    RF_StatCmdDoneError,       ///< Cmd done but with error in CMDSTA
    RF_StatInvalidParamsError, ///< Invalid API parameters
    RF_StatError   = 0x80,     ///< General error specifier
    RF_StatCmdDoneSuccess,     ///< Cmd done and Successful
    RF_StatCmdSch,             ///< Cmd scheduled for execution
    RF_StatSuccess             ///< API ran successfully
} RF_Stat;

/// Event mask type (construct mask with combinations of #RF_EventMask)
typedef uint64_t RF_EventMask;

/// Union of the different flavors of RADIO_SETUP commands
typedef union {
    rfc_command_t                   commandId; ///< Can be used simply to get RF operation ID
    rfc_CMD_RADIO_SETUP_t           common;    ///< Radio setup command for common modes (BLE, IEEE modes)
    rfc_CMD_BLE5_RADIO_SETUP_t      ble5;      ///< Radio setup command for BLE5 mode
    rfc_CMD_PROP_RADIO_SETUP_t      prop;      ///< Radio setup command for PROPRIETARY mode
    rfc_CMD_PROP_RADIO_DIV_SETUP_t  prop_div;  ///< Radio div setup command for PROPRIETARY mode
} RF_RadioSetup;

/** @brief Supported client events.
 *  Callbacks for these events are activated by setting \ref RF_Params.nClientEventMask in Struct RF_Params.
 *  Event IDs can be OR'ed to create \ref RF_Params.nClientEventMask for multiple events.
 *
 *  #RF_ClientEventPowerUpFinished is to replace #RF_EventPowerUp,
 *  #RF_ClientEventRadioFree is to replace #RF_EventRadioFree.
 */
typedef enum {
    RF_ClientEventPowerUpFinished     = 0x00000001,   ///< Event to indicate RF core power up is done
    RF_ClientEventRadioFree           = 0x00000002,   ///< Event to indicate Radio is available to use after client with higher priority has completed
    RF_ClientEventSwitchClientEntered = 0x00000004    ///< Event to indicate start of switching to the new client
} RF_ClientEvent;

/// Event mask type (construct mask with combinations of #RF_ClientEvent)
typedef uint32_t RF_ClientEventMask;

/// @brief A command handle that is returned from RF_postCmd()
/// Used by RF_pendCmd() and RF_flushCmd(). A negative value indicates an error
typedef int16_t RF_CmdHandle;

/** @brief  RF Hardware attributes
 *
 */
typedef struct RFCC26XX_HWAttrs {
    uint8_t         hwiCpe0Priority;  ///< Priority for INT_RFC_CPE_0 interrupt
    uint8_t         hwiHwPriority;    ///< Priority for INT_RFC_HW_COMB interrupt
    uint8_t         swiCpe0Priority;  ///< Priority for CPE_0 SWI
    uint8_t         swiHwPriority;    ///< Priority for HW SWI. Used for all RAT channel callbacks.
} RFCC26XX_HWAttrs;

/** @brief Common type definition of RF_Object used to store the internal confguration
 *         and state of the radio client.
 *  A pointer to an RF_Object is called #RF_Handle, and is used to interact with
 *  the RF driver.
 *  The size of #RF_Object can be optimized for single mode applications by providing the
 *  RF_SINGLEMODE symbol at compilation time. The prebuilt single mode libraries
 *  were generated with this symbol defined, hence any project uses these libraries
 *  must also define the symbol on project level.
 *  @note Must reside in persistent memory.
 *  @note Except configuration fields before call to RF_open(), modification of
 *        any field in forbidden.
 */
#if defined(RF_SINGLEMODE)
  typedef struct RF_ObjectSingleMode  RF_Object;
#else
  typedef struct RF_ObjectMultiMode   RF_Object;
#endif

/** @brief Definition of the RF_Object structure for single mode applications.
 *  It is applicable with the single mode RF driver through the #RF_Object common type.
 */
struct RF_ObjectSingleMode{
    /// Configuration
    struct {
        uint32_t            nInactivityTimeout;     ///< Inactivity timeout in us
        RF_Mode*            pRfMode;                ///< Mode of operation
        RF_RadioSetup*      pOpSetup;               ///< Radio setup radio operation, only ram right now.
        uint32_t            nPowerUpDuration;       ///< Measured poweruptime in us or specified startup time (if left default it will measure)
        bool                bPowerUpXOSC;           ///< Allways enable XOSC_HF at chip wakeup
        bool                bUpdateSetup;           ///< Setup command update
        uint16_t            nPowerUpDurationMargin; ///< Powerup duration margin in us
        void*               pPowerCb;               ///< \deprecated Power up callback, will go away in future versions, see clientConfig::pClienteventCb instead
        void*               pErrCb;                 ///< Error callback
    } clientConfig;
    /// State & variables
    struct {
        struct {
            rfc_CMD_FS_t        cmdFs;              ///< FS command encapsulating FS state
        } mode_state;                               ///< (Mode-specific) state structure
        SemaphoreP_Struct       semSync;            ///< Semaphore used by runCmd(), pendCmd() and powerdown sequence
        RF_EventMask volatile   eventSync;          ///< Event mask/value used by runCmd() and waitCmd()
        void*                   pCbSync;            ///< Internal storage for user callback
        RF_EventMask            unpendCause;        ///< Return value for RF_pendCmd()
        ClockP_Struct           clkInactivity;      ///< Clock used for inactivity timeouts
        RF_CmdHandle volatile   chLastPosted;       ///< Command handle of most recently posted command
        bool                    bYielded;           ///< Client has indicated that there are no more commands
    } state;
};

/** @brief Definition of the RF_Object structure for multi mode applications.
 *  It is applicable with the multi mode RF driver through the #RF_Object common type.
 */
struct RF_ObjectMultiMode{
    /// Configuration
    struct {
        uint32_t            nInactivityTimeout;     ///< Inactivity timeout in us
        RF_Mode*            pRfMode;                ///< Mode of operation
        RF_RadioSetup*      pOpSetup;               ///< Radio setup radio operation, only ram right now.
        uint32_t            nPowerUpDuration;       ///< Measured poweruptime in us or specified startup time (if left default it will measure)
        bool                bPowerUpXOSC;           ///< Allways enable XOSC_HF at chip wakeup
        bool                bUpdateSetup;           ///< Setup command update
        uint16_t            nPowerUpDurationMargin; ///< Powerup duration margin in us
        void*               pPowerCb;               ///< \deprecated Power up callback, will go away in future versions, see clientConfig::pClienteventCb instead
        void*               pErrCb;                 ///< Error callback
        void*               pClientEventCb;         ///< Client event callback
        RF_ClientEventMask  nClientEventMask;       ///< Client event mask to activate client event callback
    } clientConfig;
    /// State & variables
    struct {
        struct {
            rfc_CMD_FS_t        cmdFs;              ///< FS command encapsulating FS state
        } mode_state;                               ///< (Mode-specific) state structure
        SemaphoreP_Struct       semSync;            ///< Semaphore used by runCmd(), pendCmd() and powerdown sequence
        RF_EventMask volatile   eventSync;          ///< Event mask/value used by runCmd() and waitCmd()
        void*                   pCbSync;            ///< Internal storage for user callback
        RF_EventMask            unpendCause;        ///< Return value for RF_pendCmd()
        ClockP_Struct           clkInactivity;      ///< Clock used for inactivity timeouts
        ClockP_Struct           clkReqAccess;       ///< Clock used for request access timeouts
        RF_CmdHandle volatile   chLastPosted;       ///< Command handle of most recently posted command
        bool                    bYielded;           ///< Client has indicated that there are no more commands
    } state;
};

/// @brief A handle that is returned from a RF_open() call
/// Used for further RF client interaction with the RF driver
typedef RF_Object* RF_Handle;

/// RF_getParamsType contains the options available for using the RF_getParams()
typedef enum {
    RF_GET_CURR_CMD,                              ///< Get RF_CmdHandle for current cmd
    RF_GET_AVAIL_RAT_CH,                          ///< Get bitmap for the avail RAT channel
    RF_GET_RADIO_STATE,                           ///< Get Radio state 0: Radio OFF, 1: Radio ON
    RF_GET_SCHEDULE_MAP,                          ///< Get schedule timing map
} RF_InfoType;

/// RF schedule map entry structure
typedef struct {
    RF_Handle    pClient;                          ///< Pointer to client object
    uint32_t     startTime;                        ///< Start time (in RAT tick) of the command or access request
    uint32_t     endTime;                          ///< End time (in RAT tick) of the command or access request
    RF_Priority  priority;                         ///< Priority of the command or access request
} RF_ScheduleMapElement;

///RF schedule map structure
typedef struct {
    RF_ScheduleMapElement  accessMap[RF_NUM_SCHEDULE_ACCESS_ENTRIES];    ///< Access request schedule map
    RF_ScheduleMapElement  commandMap[RF_NUM_SCHEDULE_COMMAND_ENTRIES];  ///< Command schedule map
} RF_ScheduleMap;

/// RF_getParamsVal used to input and return RF driver paramters with RF_getParams()
typedef union {
    RF_CmdHandle ch;                              ///< cmd handle
    uint16_t     availRatCh;                      ///< RAT channels available
    bool         bRadioState;                     ///< Radio state
    void         *pScheduleMap;                   ///< Pointer to RF schedule map
} RF_InfoVal;

/**
 *  @brief RF callback function pointer type
 *  RF callbacks can occur at the completion of posted RF operation (chain). The
 *  callback is called from SWI context and provides the relevant #RF_Handle,
 *  relevant radio operation command handle #RF_CmdHandle, as well as an #RF_EventMask that indicates
 *  what has occurred.<br>
 *  In the case of error callback the #RF_CmdHandle will indicate the error code <br>
 *  It is recommended to save the command status initially in the callback function,
 *  if it needs to be used later especially if the user registers for multiple events
 *  ex. RF_EventRxOk and RF_EventLastCmdDone for a RX command. <br>
 */
typedef void (*RF_Callback)(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);

/**
 *  @brief RF client callback function pointer type
 *  RF client callbacks can occur at the completion of client events defined in RF_ClientEvent. The
 *  callback is called from SWI context and provides the relevant #RF_Handle and the
 *  relevant #RF_ClientEvent. The void* arg is reserved for future use.<br>
 */
typedef void (*RF_ClientCallback)(RF_Handle h, RF_ClientEvent event, void* arg);

/// @brief RF parameter struct
/// RF parameters are used with the RF_open() and RF_Params_init() call.
typedef struct {
    uint32_t            nInactivityTimeout;      ///< Inactivity timeout in us, default is infinite
    uint32_t            nPowerUpDuration;        ///< Measured poweruptime in us or specified startup time (if left default it will measure)
    RF_Callback         pPowerCb;                ///< \deprecated Power up callback, will go away in future versions, see RF_Params::pClienteventCb instead
    RF_Callback         pErrCb;                  ///< Error callback
    bool                bPowerUpXOSC;            ///< Allways enable XOSC_HF at chip wakeup
    uint16_t            nPowerUpDurationMargin;  ///< Powerup duration margin in us
    RF_ClientCallback   pClientEventCb;          ///< Client event callback
    RF_ClientEventMask  nClientEventMask;        ///< Client event mask to activate client event callback
} RF_Params;

/// @brief RF schedule command parameter struct
/// RF schedule command parameters are used with the RF_scheduleCmd() call.
typedef struct {
    uint32_t    endTime;           ///< End time in RAT Ticks for the radio command
    RF_Priority priority;          ///< Intra client priority
    bool        bIeeeBgCmd;        ///< IEEE 15.4 background command indication. 0: FG cmd, 1: BG cmd.
}RF_ScheduleCmdParams;

/// @brief RF request access parameter struct
/// RF request access command parameters are used with the RF_requestAccess() call.
typedef struct {
    uint32_t    duration;          ///< Radio access duration in RAT Ticks requested by the client
    uint32_t    startTime;         ///< Start time window in RAT Time for radio access
    RF_Priority priority;          ///< Access priority
}RF_AccessParams;

/**
 *  @brief  Open client connection to RF driver
 *
 *  Allows a RF client (high-level driver or application) to request access to
 *  RF hardware.<br>
 *
 *  @note Calling context : Task
 *  @note This API does not start the radio.
 *  @note For dual mode operation, rfMode in the RF_Mode struct must be set to RF_MODE_MULTIPLE.
 *
 *  @param pObj      Pointer to a RF_Object that will hold the state for this
 *                   RF client. The object must be in persistent and writeable
 *                   memory
 *  @param pRfMode   Pointer to a RF_Mode struct holding patch information
 *  @param pOpSetup  Pointer to the setup command used in this RF configuration. This is used by RF Driver during power-up
 *                   so the user should allocated setup command in persistent memory.
 *  @param params    Pointer to an RF_Params object that is initialized with desired RF
 *                   parameters. Can be set to NULL for defaults
 *  @return          A handle for further RF driver calls or NULL if a client connection
 *                   was not possible or the supplied parameters invalid
 */
extern RF_Handle RF_open(RF_Object *pObj, RF_Mode *pRfMode, RF_RadioSetup *pOpSetup, RF_Params *params);


/**
 *  @brief  Close client connection to RF driver
 *
 *  Allows a RF client (high-level driver or application) to close its connection
 *  to the RF driver.
 *
 *  @note Calling context : Task
 *
 *  @param h  Handle previously returned by RF_open()
 */
extern void RF_close(RF_Handle h);


/**
 *  @brief  Return current radio timer value
 *
 *  If the radio is powered returns the current radio timer value, if not returns
 *  a conservative estimate of the current radio timer value
 *
 *  @note Calling context : Task/SWI/HWI
 *
 *  @return     Current radio timer value
 */
extern uint32_t RF_getCurrentTime(void);


/**
 *  @brief  Post an RF operation (chain) to the command queue
 *  Post an #RF_Op to the RF command queue of the client with handle h. <br>
 *  The command can be the first in a chain of RF operations or a standalone RF operation.
 *  If a chain of operations are posted they are treated atomically, i.e. either all
 *  or none of the chained operations are run. <br>
 *  All operations must be posted in strictly increasing chronological order. Function returns
 *  immediately. <br>
 *  To facilitate code optimization RF_postCmd() should be used when running in single client
 *  environment. It does not call the internal dual mode functions and the implementation are
 *  same across RFCC26XX_multiMode.c and RFCC26XX_singleMode.c files.<br>
 *  Alternately RF_scheduleCmd() can be used in single or multi client environment. <br>
 *
 *  Limitations apply to the operations posted:
 *  - The operation must be in the set supported in the chosen radio mode when
 *    RF_open() was called
 *  - Only a subset of radio operations are supported
 *  - Only some of the trigger modes are supported
 *    - TRIG_NOW, TRIG_ABSTIME
 *  - Only some of the conditional execution modes are supported
 *    - COND_ALWAYS, COND_NEVER
 *
 *  @note Calling context : Task/SWI
 *
 *  @param h         Handle previously returned by RF_open()
 *  @param pOp       Pointer to the #RF_Op. Must normally be in persistent and writeable memory
 *  @param ePri      Priority of this RF command (used for arbitration in multi-client systems)
 *  @param pCb       Callback function called upon command completion (and some other events).
 *                   If RF_postCmd() fails no callback is made
 *  @param bmEvent   Bitmask of events that will trigger the callback.
 *  @return          A handle to the RF command. Return value of RF_ALLOC_ERROR indicates error.
 */
extern RF_CmdHandle RF_postCmd(RF_Handle h, RF_Op* pOp, RF_Priority ePri, RF_Callback pCb, RF_EventMask bmEvent);

/**
 *  @brief  Schedule an RF operation (chain) to the command queue
 *  Schedule an #RF_Op to the RF command queue of the client with handle h. <br>
 *  The command can be the first in a chain of RF operations or a standalone RF operation.
 *  If a chain of operations are posted they are treated atomically, i.e. either all
 *  or none of the chained operations are run. <br>
 *  All operations must be posted in strictly increasing chronological order. Function returns
 *  immediately. <br>
 *
 *  Limitations apply to the operations posted:
 *  - The operation must be in the set supported in the chosen radio mode when
 *    RF_open() was called
 *  - Only a subset of radio operations are supported
 *  - Only some of the trigger modes are supported with potential power saving (TRIG_NOW, TRIG_ABSTIME)
 *
 *  @note Calling context : Task/SWI
 *
 *  @param h         Handle previously returned by RF_open()
 *  @param pOp       Pointer to the #RF_Op. Must normally be in persistent and writeable memory
 *  @param pSchParams Pointer to the schdule command parameter structure
 *  @param pCb       Callback function called upon command completion (and some other events).
 *                   If RF_scheduleCmd() fails no callback is made
 *  @param bmEvent   Bitmask of events that will trigger the callback.
 *  @return          A handle to the RF command. Return value of RF_ALLOC_ERROR indicates error.
 */
extern RF_CmdHandle RF_scheduleCmd(RF_Handle h, RF_Op* pOp, RF_ScheduleCmdParams *pSchParams, RF_Callback pCb, RF_EventMask bmEvent);

/**
 *  @brief  Wait for posted command to complete
 *  Wait until completion of RF command identified by handle ch for client identified
 *  by handle h to complete. <br> Some RF operations (or chains of operations) use
 *  additional events during RF_postCmd() which, if enabled in event mask bmEvent for the
 *  RF_pendCmd() will make the RF_pendCmd() return early, when that event occurs. In this
 *  case, multiple calls to RF_pendCmd() for a single command can be made. <br>
 *  If RF_pendCmd() is called for a command that registered a callback function it
 *  will take precedence and the callback function will never be called. <br>
 *
 *  @note Calling context : Task
 *  @note Only one call to RF_pendCmd() or RF_runCmd() can be made at a time for
 *        each client
 *  @note Returns immediately if command has already run or has been aborted/cancelled
 *
 *  @param h        Handle previously returned by RF_open()
 *  @param ch       Command handle previously returned by RF_postCmd().
 *  @param bmEvent  Bitmask of events that make RF_pendCmd() return. The command done
 *                  event can not be masked away.
 *  @return         The relevant RF_EventMask (including RF_EventCmdDone) for the command
 *                  or last command in a chain
 */
extern RF_EventMask RF_pendCmd(RF_Handle h, RF_CmdHandle ch, RF_EventMask bmEvent);


/**
 *  @brief  Runs synchronously a (chain of) RF operation(s) for single mode
 *  Allows a (chain of) operation(s) to be posted to the command queue and then waits
 *  for it to complete. <br> A command is completed if one of the RF_EventLastCmdDone,
 *  RF_EventCmdCancelled, RF_EventCmdAborted, RF_EventCmdStopped or RF_EventCmdError occured.
 *
 *  @note Calling context : Task
 *  @note Only one call to RF_pendCmd() or RF_runCmd() can be made at a time for
 *        each client
 *
 *  @param h         Handle previously returned by RF_open()
 *  @param pOp       Pointer to the #RF_Op. Must normally be in persistent and writeable memory
 *  @param ePri      Priority of this RF command (used for arbitration in multi-client systems)
 *  @param pCb       Callback function called upon command completion (and some other events).
 *                   If RF_runCmd() fails no callback is made
 *  @param bmEvent   Bitmask of events that will trigger the callback.
 *  @return          The relevant commmand completed event.
 */
extern RF_EventMask RF_runCmd(RF_Handle h, RF_Op* pOp, RF_Priority ePri, RF_Callback pCb, RF_EventMask bmEvent);

/**
 *  @brief  Runs synchronously a (chain of) RF operation(s) for dual or single mode
 *  Allows a (chain of) operation(s) to be scheduled to the command queue and then waits
 *  for it to complete. <br> A command is completed if one of the RF_EventLastCmdDone,
 *  RF_EventCmdCancelled, RF_EventCmdAborted, RF_EventCmdStopped or RF_EventCmdError occured.
 *
 *  @note Calling context : Task
 *  @note Only one call to RF_pendCmd() or RF_runScheduleCmd() can be made at a time for
 *        each client
 *
 *  @param h         Handle previously returned by RF_open()
 *  @param pOp       Pointer to the #RF_Op. Must normally be in persistent and writeable memory
 *  @param pSchParams Pointer to the schdule command parameter structure
 *  @param pCb       Callback function called upon command completion (and some other events).
 *                   If RF_runCmd() fails no callback is made
 *  @param bmEvent   Bitmask of events that will trigger the callback.
 *  @return          The relevant commmand completed event.
 */
extern RF_EventMask RF_runScheduleCmd(RF_Handle h, RF_Op* pOp, RF_ScheduleCmdParams *pSchParams, RF_Callback pCb, RF_EventMask bmEvent);

/**
 *  @brief  Abort/stop/cancel single command in command queue
 *  If command is running, aborts/stops it and posts callback for the
 *  aborted/stopped command. <br>
 *  If command has not yet run, cancels it it and posts callback for the
 *  cancelled command. <br>
 *  If command has already run or been aborted/stopped/cancelled, has no effect.<br>
 *  If RF_cancelCmd is called from a Swi context with same or higher priority
 *  than RF Driver Swi, when the RF core is powered OFF -> the cancel callback will be delayed
 *  until the next power-up cycle.<br>
 *
 *  @note Calling context : Task/SWI
 *
 *  @param h            Handle previously returned by RF_open()
 *  @param ch           Command handle previously returned by RF_postCmd().
 *  @param mode         1: Stop gracefully, 0: abort abruptly
 *  @return             RF_Stat indicates if command was successfully completed
 */
extern RF_Stat RF_cancelCmd(RF_Handle h, RF_CmdHandle ch, uint8_t mode);


/**
 *  @brief  Abort/stop/cancel command and any subsequent commands in command queue
 *  If command is running, aborts/stops it and then cancels all later commands in queue.<br>
 *  If command has not yet run, cancels it and all later commands in queue.<br>
 *  If command has already run or been aborted/stopped/cancelled, has no effect.<br>
 *  The callbacks for all cancelled commands are issued in chronological order.<br>
 *  If RF_flushCmd is called from a Swi context with same or higher priority
 *  than RF Driver Swi, when the RF core is powered OFF -> the cancel callback will be delayed
 *  until the next power-up cycle.<br>
 *
 *  @note Calling context : Task/SWI
 *
 *  @param h            Handle previously returned by RF_open()
 *  @param ch           Command handle previously returned by RF_postCmd().
 *  @param mode         1: Stop gracefully, 0: abort abruptly
 *  @return             RF_Stat indicates if command was successfully completed
 */
extern RF_Stat RF_flushCmd(RF_Handle h, RF_CmdHandle ch, uint8_t mode);

/**
 *  @brief Send any Immediate command. <br>
 *
 *  Immediate Comamnd is send to RDBELL, if radio is active and the RF_Handle points
 *  to the current client. <br>
 *  In other appropriate RF_Stat values are returned. <br>
 *
 *  @note Calling context : Task/SWI/HWI
 *
 *  @param h            Handle previously returned by RF_open()
 *  @param pCmdStruct   Pointer to the immediate command structure
 *  @return             RF_Stat indicates if command was successfully completed
*/
extern RF_Stat RF_runImmediateCmd(RF_Handle h, uint32_t* pCmdStruct);

/**
 *  @brief Send any Direct command. <br>
 *
 *  Direct Comamnd value is send to RDBELL immediately, if radio is active and
 *  the RF_Handle point to the current client. <br>
 *  In other appropriate RF_Stat values are returned. <br>
 *
 *  @note Calling context : Task/SWI/HWI
 *
 *  @param h            Handle previously returned by RF_open()
 *  @param cmd          Direct command value.
 *  @return             RF_Stat indicates if command was successfully completed.
*/
extern RF_Stat RF_runDirectCmd(RF_Handle h, uint32_t cmd);

/**
 *  @brief  Signal that radio client is not going to issue more commands in a while. <br>
 *
 *  Hint to RF driver that, irrespective of inactivity timeout, no new further
 *  commands will be issued for a while and thus the radio can be powered down at
 *  the earliest convenience.<br>
 *
 *  @note Calling context : Task
 *
 *  @param h       Handle previously returned by RF_open()
 */
extern void RF_yield(RF_Handle h);

/**
 *  @brief  Function to initialize the RF_Params struct to its defaults
 *
 *  @param  params      An pointer to RF_Params structure for
 *                      initialization
 *
 *  Defaults values are:
 *      nInactivityTimeout = BIOS_WAIT_FOREVER
 *      nPowerUpDuration   = RF_DEFAULT_POWER_UP_TIME
 *      bPowerUpXOSC       = true
 */
extern void RF_Params_init(RF_Params *params);

/**
 *  @brief Get value for some RF driver parameters. <br>
 *
 *  @note Calling context : Task/SWI/HWI
 *
 *  @param h            Handle previously returned by RF_open()
 *  @param type         Request value paramter defined by RF_InfoType
 *  @param pValue       Pointer to return parameter values specified by RF_InfoVal
 *  @return             RF_Stat indicates if command was successfully completed
*/
extern RF_Stat RF_getInfo(RF_Handle h, RF_InfoType type, RF_InfoVal *pValue);

/**
 *  @brief Get rssi value.
 *
 *  @note Calling context : Task/SWI/HWI
 *
 *  @param  h            Handle previously returned by RF_open()
 *  @return              RSSI value. Return value of RF_GET_RSSI_ERROR_VAL indicates error case.
 */
extern int8_t RF_getRssi(RF_Handle h);

/**
 *  @brief  Get command structure pointer.
 *
 *  @note Calling context : Task/SWI/HWI
 *
 *  @param h            Handle previously returned by RF_open()
 *  @param cmdHnd       Command handle returned by RF_postCmd()
 *  @return             Pointer to the command structure.
*/
extern RF_Op* RF_getCmdOp(RF_Handle h, RF_CmdHandle cmdHnd);

/**
 *  @brief  Setup RAT compare, and callback when compare matches. Note radio needs to be on for
 *          RAT to operate. If radio is off, this API returns RF_ALLOC_ERROR
 *
 *  @note Calling context : Task/SWI
 *
 *  @param h             Handle previously returned by RF_open()
 *  @param pCmdStruct    Pointer to the RAT compare command structure.
 *  @param compareTime   Compare time in RAT ticks
 *  @param pRatCb        Callback function when capture event happens
 *  @return              Allocated RAT channel. Returned value RF_ALLOC_ERROR indicates error.
 */
extern int8_t RF_ratCompare(RF_Handle h, rfc_CMD_SET_RAT_CMP_t* pCmdStruct, \
                            uint32_t compareTime, RF_Callback pRatCb);

/**
 *  @brief  Setup RAT capture, and callback when capture event happens. Note radio needs to be on for
 *          RAT to operate. If radio is off, this API returns RF_ALLOC_ERROR
 *
 *  @note Calling context : Task/SWI
 *
 *  @param h             Handle previously returned by RF_open()
 *  @param config        Config field of RAT capture command structure
 *  @param pRatCb        Callback function when capture event happens
 *  @return              Allocated RAT channel. Returned value RF_ALLOC_ERROR indicates error.
 */
extern int8_t RF_ratCapture(RF_Handle h, uint16_t config, RF_Callback  pRatCb);

/**
 *  @brief  Setup RAT HW output. Note radio needs to be on for
 *          RAT to operate. If radio is off, this API returns RF_ALLOC_ERROR
 *
 *  @note Calling context : Task/SWI
 *
 *  @param h             Handle previously returned by RF_open()
 *  @param config        Config field of RAT HW output command structure
 *  @return              RF_Stat indicates if command was successfully completed
 */
extern RF_Stat RF_ratHwOutput(RF_Handle h, uint16_t config);

/**
 *  @brief  Disable a RAT channel
 *
 *  @note Calling context : Task/SWI
 *
 *  @param h             Handle previously returned by RF_open()
 *  @param ratChannelNum RAT channel number to be disabled
 *  @return              RF_Stat indicates if command was successfully completed.
 */
extern RF_Stat RF_ratDisableChannel(RF_Handle h, int8_t ratChannelNum);

/**
 *  @brief  Set RF control parameters
 *
 *  @note Calling context : Task
 *
 *  @param h             Handle previously returned by RF_open()
 *  @param ctrl          Control codes
 *  @param args          Pointer to control arguments
 *  @return              RF_Stat indicates if API call was successfully completed.
 */
extern RF_Stat RF_control(RF_Handle h, int8_t ctrl, void *args);

/**
 *  @brief  Request radio access <br>
 *  Scope:
 *  1. Only suppports request access which start immediately.<br>
 *  2. The #RF_AccessParams duration should be less than a pre-defined value
 *     RF_REQ_ACCESS_MAX_DUR_US in RFCC26XX_multiMode.c.<br>
 *  3. The #RF_AccessParams priority should be set RF_PriorityHighest.<br>
 *  4. Single request for a client at anytime.<br>
 *  5. Command from different client are blocked untill the radio access
 *     period is completed.<br>
 *
 *  @note Calling context : Task
 *
 *  @param h             Handle previously returned by RF_open()
 *  @param pParams       Pointer to RF_AccessRequest parameters
 *  @return              RF_Stat indicates if API call was successfully completed.
 */
extern RF_Stat RF_requestAccess(RF_Handle h, RF_AccessParams *pParams);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_rf__include */
