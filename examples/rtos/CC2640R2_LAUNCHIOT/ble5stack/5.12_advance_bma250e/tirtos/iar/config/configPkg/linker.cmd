/*
 * Do not modify this file; it is automatically generated from the template
 * linkcmd.xdt in the ti.platforms.tiva package and will be overwritten.
 */

"C:\ti\simplelink_cc2640r2_sdk_1_40_00_45\examples\rtos\CC2640R2_LAUNCHIOT\ble5stack\5.12_advance_bma250e\tirtos\iar\config\configPkg\package\cfg\app_ble_prm3.orm3"
"C:\ti\simplelink_cc2640r2_sdk_1_40_00_45\examples\rtos\CC2640R2_LAUNCHIOT\ble5stack\5.12_advance_bma250e\tirtos\iar\config\src\sysbios\rom_sysbios.arm3"
"C:\ti\simplelink_cc2640r2_sdk_1_40_00_45\kernel\tirtos\packages\iar\targets\arm\rts\lib\release\boot.arm3"


/* Content from xdc.services.global (null): */

/* Content from xdc (null): */

/* Content from xdc.corevers (null): */

/* Content from xdc.shelf (null): */

/* Content from xdc.services.spec (null): */

/* Content from xdc.services.intern.xsr (null): */

/* Content from xdc.services.intern.gen (null): */

/* Content from xdc.services.intern.cmd (null): */

/* Content from xdc.bld (null): */

/* Content from iar.targets.arm (null): */

/* Content from xdc.rov (null): */

/* Content from xdc.runtime (null): */

/* Content from iar.targets.arm.rts (): */

/* Content from ti.sysbios.rom (null): */

/* Content from ti.sysbios.interfaces (null): */

/* Content from ti.sysbios.family (null): */

/* Content from ti.sysbios.family.arm (ti/sysbios/family/arm/linkcmd.xdt): */

/* Content from xdc.services.getset (null): */

/* Content from ti.sysbios.rts (ti/sysbios/rts/linkcmd.xdt): */

/* Content from xdc.runtime.knl (null): */

/* Content from ti.catalog.arm.cortexm3 (null): */

/* Content from ti.catalog.peripherals.hdvicp2 (null): */

/* Content from ti.catalog (null): */

/* Content from ti.catalog.arm.peripherals.timers (null): */

/* Content from xdc.platform (null): */

/* Content from xdc.cfg (null): */

/* Content from ti.catalog.arm.cortexm4 (null): */

/* Content from ti.platforms.simplelink (null): */

/* Content from ti.sysbios.hal (null): */

/* Content from ti.sysbios.family.arm.cc26xx (null): */

/* Content from ti.sysbios.family.arm.m3 (ti/sysbios/family/arm/m3/linkcmd.xdt): */
--entry __iar_program_start
--keep __vector_table
--define_symbol ti_sysbios_family_arm_m3_Hwi_nvic=0xe000e000

/* Content from ti.sysbios.knl (null): */

/* Content from ti.sysbios (null): */

/* Content from ti.sysbios.gates (null): */

/* Content from ti.sysbios.heaps (null): */

/* Content from ti.sysbios.rom.cortexm.cc26xx.r2 (C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/kernel/tirtos/packages/ti/sysbios/rom/cortexm/cc26xx/r2/golden/CC26xx/CC26xx_link_iar.xdt): */

--keep xdc_runtime_Error_policy__C
--keep xdc_runtime_IModule_Interface__BASE__C
--keep xdc_runtime_Startup_lastFxns__C
--keep ti_sysbios_knl_Event_Object__DESC__C
--keep ti_sysbios_gates_GateMutex_Object__DESC__C
--keep ti_sysbios_rom_ROM_ti_sysbios_family_arm_cc26xx_Timer_initDevice__I
--keep xdc_runtime_Startup_execImpl__C
--keep ti_sysbios_rom_ROM_ti_sysbios_family_arm_cc26xx_Timer_getMaxTicks__E
--keep ti_sysbios_knl_Swi_Object__count__C
--keep ti_sysbios_knl_Idle_funcList__C
--keep xdc_runtime_Text_isLoaded__C
--keep ti_sysbios_knl_Clock_Object__DESC__C
--keep ti_sysbios_knl_Semaphore_eventSync__C
--keep ti_sysbios_gates_GateMutex_Module__FXNS__C
--keep ti_sysbios_family_arm_m3_Hwi_Module__id__C
--keep ti_sysbios_family_arm_cc26xx_Timer_Module__id__C
--keep ti_sysbios_knl_Mailbox_Object__table__C
--keep ti_sysbios_family_arm_m3_Hwi_Object__table__C
--keep ti_sysbios_knl_Swi_Object__DESC__C
--keep xdc_runtime_Text_charCnt__C
--keep xdc_runtime_Error_raiseHook__C
--keep ti_sysbios_rom_ROM_ti_sysbios_family_arm_cc26xx_Timer_start__E
--keep ti_sysbios_heaps_HeapMem_Object__table__C
--keep xdc_runtime_Error_policyFxn__C
--keep ti_sysbios_rom_ROM_ti_sysbios_family_arm_cc26xx_Timer_getCount64__E
--keep xdc_runtime_Startup_firstFxns__C
--keep xdc_runtime_Text_charTab__C
--keep ti_sysbios_rom_ROM_ti_sysbios_family_arm_cc26xx_TimestampProvider_get32__E
--keep xdc_runtime_Error_maxDepth__C
--keep ti_sysbios_rom_ROM_ti_sysbios_family_arm_cc26xx_Timer_getCurrentTick__E
--keep ti_sysbios_family_arm_m3_TaskSupport_stackAlignment__C
--keep ti_sysbios_family_arm_m3_Hwi_NUM_INTERRUPTS__C
--keep ti_sysbios_knl_Clock_triggerClock__C
--keep ti_sysbios_knl_Swi_Object__table__C
--keep xdc_runtime_Memory_Module__id__C
--keep ti_sysbios_heaps_HeapMem_Module__gateObj__C
--keep ti_sysbios_family_arm_cc26xx_Timer_startupNeeded__C
--keep ti_sysbios_knl_Queue_Object__DESC__C
--keep ti_sysbios_knl_Task_Object__DESC__C
--keep xdc_runtime_Assert_E_assertFailed__C
--keep ti_sysbios_gates_GateHwi_Module__id__C
--keep xdc_runtime_IHeap_Interface__BASE__C
--keep xdc_runtime_SysCallback_exitFxn__C
--keep ti_sysbios_heaps_HeapMem_Module__id__C
--keep ti_sysbios_family_arm_m3_Hwi_excHandlerFunc__C
--keep ti_sysbios_heaps_HeapMem_Module__FXNS__C
--keep xdc_runtime_System_maxAtexitHandlers__C
--keep ti_sysbios_knl_Queue_Object__count__C
--keep ti_sysbios_knl_Task_Object__table__C
--keep ti_sysbios_knl_Mailbox_Object__DESC__C
--keep ti_sysbios_family_arm_m3_Hwi_nullIsrFunc__C
--keep ti_sysbios_knl_Clock_tickMode__C
--keep ti_sysbios_gates_GateMutex_Module__id__C
--keep ti_sysbios_knl_Swi_numPriorities__C
--keep ti_sysbios_knl_Task_numConstructedTasks__C
--keep xdc_runtime_Startup_maxPasses__C
--keep ti_sysbios_knl_Task_initStackFlag__C
--keep xdc_runtime_System_abortFxn__C
--keep ti_sysbios_gates_GateHwi_Module__FXNS__C
--keep ti_sysbios_hal_Hwi_Object__DESC__C
--keep ti_sysbios_family_arm_m3_Hwi_priGroup__C
--keep xdc_runtime_Error_E_memory__C
--keep ti_sysbios_family_arm_m3_Hwi_E_alreadyDefined__C
--keep ti_sysbios_knl_Queue_Object__table__C
--keep ti_sysbios_knl_Semaphore_eventPost__C
--keep xdc_runtime_System_exitFxn__C
--keep ti_sysbios_rom_ROM_ti_sysbios_family_arm_cc26xx_Timer_setNextTick__E
--keep ti_sysbios_heaps_HeapMem_reqAlign__C
--keep xdc_runtime_Startup_sfxnRts__C
--keep ti_sysbios_knl_Semaphore_Object__DESC__C
--keep ti_sysbios_gates_GateHwi_Object__DESC__C
--keep ti_sysbios_heaps_HeapMem_Object__count__C
--keep ti_sysbios_family_arm_m3_Hwi_numSparseInterrupts__C
--keep ti_sysbios_knl_Mailbox_maxTypeAlign__C
--keep ti_sysbios_family_arm_cc26xx_TimestampProvider_useClockTimer__C
--keep ti_sysbios_rom_ROM_xdc_runtime_System_SupportProxy_exit__E
--keep ti_sysbios_knl_Task_allBlockedFunc__C
--keep ti_sysbios_rom_ROM_xdc_runtime_System_SupportProxy_abort__E
--keep ti_sysbios_knl_Mailbox_Object__count__C
--keep xdc_runtime_Text_nameStatic__C
--keep ti_sysbios_rom_ROM_xdc_runtime_Startup_getState__I
--keep ti_sysbios_knl_Task_defaultStackSize__C
--keep xdc_runtime_IGateProvider_Interface__BASE__C
--keep ti_sysbios_family_arm_m3_Hwi_E_hwiLimitExceeded__C
--keep xdc_runtime_Startup_startModsFxn__C
--keep ti_sysbios_family_arm_m3_Hwi_Object__DESC__C
--keep xdc_runtime_Text_nameEmpty__C
--keep ti_sysbios_family_arm_m3_Hwi_Object__count__C
--keep xdc_runtime_SysCallback_abortFxn__C
--keep ti_sysbios_knl_Task_defaultStackHeap__C
--keep ti_sysbios_family_arm_m3_Hwi_ccr__C
--keep ti_sysbios_heaps_HeapMem_E_memory__C
--keep ti_sysbios_knl_Task_Object__count__C
--keep ti_sysbios_heaps_HeapMem_Object__DESC__C
--keep xdc_runtime_Text_nameUnknown__C
--keep xdc_runtime_Memory_defaultHeapInstance__C
--keep ti_sysbios_rom_ROM_ti_sysbios_family_arm_cc26xx_Timer_setThreshold__I
--keep xdc_runtime_Startup_sfxnTab__C
--keep ti_sysbios_knl_Clock_Module__state__V
--keep ti_sysbios_family_arm_cc26xx_TimestampProvider_Module__state__V
--keep xdc_runtime_Startup_Module__state__V
--keep ti_sysbios_BIOS_Module__state__V
--keep ti_sysbios_knl_Swi_Module__state__V
--keep ti_sysbios_knl_Task_Module__state__V
--keep xdc_runtime_Memory_Module__state__V
--keep xdc_runtime_System_Module__state__V
--keep ti_sysbios_family_arm_m3_Hwi_Module__state__V
--keep xdc_runtime_Error_Module__state__V
--keep ti_sysbios_family_arm_cc26xx_Timer_Module__state__V

--define_symbol memcpy=0x1001cb39
--define_symbol memset=0x1001cb51
--define_symbol ti_sysbios_BIOS_RtsGateProxy_query__E=0x1001c0bd
--define_symbol ti_sysbios_knl_Queue_get__E=0x1001bd19
--define_symbol ti_sysbios_knl_Swi_enabled__E=0x1001bf21
--define_symbol ti_sysbios_knl_Clock_scheduleNextTick__E=0x1001bca9
--define_symbol ti_sysbios_knl_Swi_runLoop__I=0x1001adc9
--define_symbol ti_sysbios_knl_Clock_getTicks__E=0x1001b29d
--define_symbol xdc_runtime_Memory_HeapProxy_Object__delete__S=0x1001ba59
--define_symbol ti_sysbios_gates_GateMutex_Object__destruct__S=0x1001b9f9
--define_symbol ti_sysbios_knl_Queue_enqueue__E=0x1001bffd
--define_symbol ti_sysbios_knl_Queue_put__E=0x1001bd35
--define_symbol ti_sysbios_family_arm_m3_Hwi_Object__create__S=0x1001a7b9
--define_symbol ti_sysbios_gates_GateHwi_Instance_init__E=0x1001b153
--define_symbol ti_sysbios_hal_Hwi_Instance_finalize__E=0x1001c039
--define_symbol ti_sysbios_BIOS_RtsGateProxy_leave__E=0x1001c095
--define_symbol ti_sysbios_heaps_HeapMem_Object__create__S=0x1001b599
--define_symbol xdc_runtime_Error_raiseX__E=0x1001c66d
--define_symbol ti_sysbios_hal_Hwi_getStackInfo__E=0x1001b099
--define_symbol ti_sysbios_knl_Semaphore_construct=0x1001b45d
--define_symbol ti_sysbios_knl_Clock_Object__destruct__S=0x1001ba99
--define_symbol ti_sysbios_knl_Clock_TimerProxy_getMaxTicks__E=0x1001bef1
--define_symbol ti_sysbios_knl_Swi_Object__destruct__S=0x1001bd51
--define_symbol ti_sysbios_family_arm_cc26xx_TimestampProvider_getFreq__E=0x1001bf6b
--define_symbol ti_sysbios_gates_GateMutex_Handle__label__S=0x1001b9d9
--define_symbol ti_sysbios_knl_Mailbox_delete=0x1001c0f1
--define_symbol ti_sysbios_knl_Semaphore_destruct=0x1001c10d
--define_symbol ti_sysbios_BIOS_RtsGateProxy_enter__E=0x1001c091
--define_symbol ti_sysbios_knl_Task_processVitalTaskFlag__I=0x1001b48d
--define_symbol ti_sysbios_knl_Mailbox_create=0x1001a945
--define_symbol xdc_runtime_Core_deleteObject__I=0x1001c3b5
--define_symbol ti_sysbios_knl_Queue_delete=0x1001c0f9
--define_symbol ti_sysbios_family_arm_m3_Hwi_doSwiRestore__I=0x1001c043
--define_symbol xdc_runtime_Timestamp_getFreq__E=0x1001bf6b
--define_symbol ti_sysbios_knl_Event_Instance_init__E=0x1001be49
--define_symbol xdc_runtime_System_atexit__E=0x1001c581
--define_symbol ti_sysbios_gates_GateMutex_Params__init__S=0x1001bfb5
--define_symbol ti_sysbios_knl_Clock_getTimerHandle__E=0x1001bfcd
--define_symbol ti_sysbios_knl_Task_enable__E=0x1001c08b
--define_symbol ti_sysbios_knl_Clock_TimerProxy_getExpiredTicks__E=0x1001c0d1
--define_symbol xdc_runtime_Timestamp_SupportProxy_get64__E=0x1001b77d
--define_symbol ti_sysbios_knl_Queue_Object__destruct__S=0x1001bcfd
--define_symbol ti_sysbios_knl_Clock_Object__delete__S=0x1001b8b1
--define_symbol ti_sysbios_gates_GateMutex_delete=0x1001c0b5
--define_symbol ti_sysbios_heaps_HeapMem_restore__E=0x1001bf41
--define_symbol ti_sysbios_BIOS_RtsGateProxy_Object__delete__S=0x1001b88d
--define_symbol ti_sysbios_knl_Swi_create=0x1001ac1d
--define_symbol ti_sysbios_heaps_HeapMem_Module_GateProxy_leave__E=0x1001c0cd
--define_symbol ti_sysbios_knl_Semaphore_pend__E=0x10019b05
--define_symbol ti_sysbios_knl_Mailbox_Instance_finalize__E=0x1001a4c5
--define_symbol xdc_runtime_Startup_startMods__I=0x1001c129
--define_symbol ti_sysbios_heaps_HeapMem_init__I=0x1001b235
--define_symbol ti_sysbios_knl_Swi_Object__delete__S=0x1001bbb9
--define_symbol ti_sysbios_hal_Hwi_HwiProxy_enableInterrupt__E=0x1001af89
--define_symbol ti_sysbios_knl_Clock_removeI__E=0x1001c0e1
--define_symbol xdc_runtime_System_abort__E=0x1001c64d
--define_symbol ti_sysbios_family_arm_m3_Hwi_dispatchC__I=0x1001a81d
--define_symbol ti_sysbios_knl_Swi_construct=0x1001ad79
--define_symbol ti_sysbios_knl_Task_sleepTimeout__I=0x1001bda5
--define_symbol ti_sysbios_knl_Queue_remove__E=0x1001bf5d
--define_symbol ti_sysbios_knl_Semaphore_Instance_finalize__E=0x1001b7cd
--define_symbol ti_sysbios_gates_GateMutex_destruct=0x1001c0b9
--define_symbol ti_sysbios_knl_Task_SupportProxy_Module__startupDone__S=0x1001c0ad
--define_symbol ti_sysbios_knl_Queue_Object__delete__S=0x1001bb59
--define_symbol ti_sysbios_knl_Mailbox_Object__get__S=0x1001b649
--define_symbol ti_sysbios_family_arm_m3_Hwi_Instance_init__E=0x10019fc1
--define_symbol ti_sysbios_knl_Clock_delete=0x1001c0d9
--define_symbol ti_sysbios_knl_Clock_walkQueueDynamic__E=0x1001a2f1
--define_symbol ti_sysbios_knl_Mailbox_Object__destruct__S=0x1001bce1
--define_symbol ti_sysbios_knl_Mailbox_post__E=0x1001a0ed
--define_symbol ti_sysbios_knl_Clock_Instance_init__E=0x1001b1c5
--define_symbol ti_sysbios_knl_Event_construct=0x1001b5f1
--define_symbol ti_sysbios_knl_Task_allBlockedFunction__I=0x1001b055
--define_symbol ti_sysbios_knl_Task_postInit__I=0x1001a271
--define_symbol ti_sysbios_knl_Task_enter__I=0x1001bd89
--define_symbol ti_sysbios_hal_Hwi_switchFromBootStack__E=0x1001be35
--define_symbol ti_sysbios_knl_Semaphore_Object__destruct__S=0x1001bb99
--define_symbol ti_sysbios_hal_Hwi_HwiProxy_Object__create__S=0x1001bed1
--define_symbol ti_sysbios_family_arm_cc26xx_Timer_postInit__I=0x1001b4bd
--define_symbol ti_sysbios_knl_Swi_Module_startup__E=0x1001c07f
--define_symbol ti_sysbios_gates_GateMutex_Instance_finalize__E=0x1001bec1
--define_symbol ti_sysbios_hal_Hwi_startup__E=0x1001c04b
--define_symbol xdc_runtime_Core_assignParams__I=0x1001c4ad
--define_symbol ti_sysbios_hal_Hwi_HwiProxy_switchFromBootStack__E=0x1001be35
--define_symbol ti_sysbios_heaps_HeapMem_allocUnprotected__E=0x1001a615
--define_symbol ti_sysbios_knl_Swi_post__E=0x1001b011
--define_symbol ti_sysbios_hal_Hwi_initStack=0x1001b56d
--define_symbol xdc_runtime_Memory_alloc__E=0x1001c349
--define_symbol ti_sysbios_knl_Queue_next__E=0x1001c105
--define_symbol ti_sysbios_knl_Event_destruct=0x1001c0ed
--define_symbol ti_sysbios_knl_Clock_Instance_finalize__E=0x1001be09
--define_symbol ti_sysbios_knl_Queue_elemClear__E=0x1001c079
--define_symbol ti_sysbios_knl_Clock_TimerProxy_startup__E=0x1001b4e9
--define_symbol ti_sysbios_knl_Clock_Params__init__S=0x1001bfc1
--define_symbol ti_sysbios_knl_Task_Instance_init__E=0x10019ce1
--define_symbol ti_sysbios_hal_Hwi_HwiProxy_startup__E=0x1001c04b
--define_symbol ti_sysbios_knl_Task_self__E=0x1001c02d
--define_symbol ti_sysbios_knl_Task_startup__E=0x1001c125
--define_symbol ti_sysbios_gates_GateHwi_Object__delete__S=0x1001b9b9
--define_symbol ti_sysbios_family_arm_cc26xx_Timer_startup__E=0x1001b4e9
--define_symbol xdc_runtime_Memory_free__E=0x1001c705
--define_symbol ti_sysbios_knl_Clock_triggerFunc__I=0x1001bdbf
--define_symbol ti_sysbios_hal_Hwi_delete=0x1001c0c5
--define_symbol ti_sysbios_knl_Queue_Instance_init__E=0x1001c073
--define_symbol ti_sysbios_knl_Event_delete=0x1001c0e9
--define_symbol ti_sysbios_BIOS_RtsGateProxy_Handle__label__S=0x1001b9d9
--define_symbol ti_sysbios_family_arm_cc26xx_Timer_Module_startup__E=0x1001b845
--define_symbol ti_sysbios_knl_Event_Object__delete__S=0x1001bad9
--define_symbol xdc_runtime_Assert_raise__I=0x1001c465
--define_symbol ti_sysbios_hal_Hwi_create=0x1001a881
--define_symbol ti_sysbios_knl_Task_destruct=0x1001c121
--define_symbol ti_sysbios_hal_Hwi_Module_startup__E=0x1001be5d
--define_symbol ti_sysbios_family_arm_m3_Hwi_excHandler__I=0x1001b979
--define_symbol xdc_runtime_Core_destructObject__I=0x1001c6a5
--define_symbol ti_sysbios_knl_Swi_disable__E=0x1001bf11
--define_symbol ti_sysbios_BIOS_setThreadType__E=0x1001bea1
--define_symbol ti_sysbios_knl_Task_disable__E=0x1001bf31
--define_symbol ti_sysbios_knl_Swi_Instance_init__E=0x1001a9a5
--define_symbol ti_sysbios_knl_Semaphore_pendTimeout__I=0x1001b6f9
--define_symbol ti_sysbios_knl_Clock_create=0x1001b1fd
--define_symbol ti_sysbios_knl_Idle_loop__E=0x1001c053
--define_symbol ti_sysbios_gates_GateHwi_leave__E=0x1001c067
--define_symbol ti_sysbios_family_arm_m3_Hwi_enableInterrupt__E=0x1001af89
--define_symbol ti_sysbios_knl_Semaphore_Params__init__S=0x1001c009
--define_symbol ti_sysbios_knl_Event_pend__E=0x10019bf9
--define_symbol ti_sysbios_knl_Task_unblock__E=0x1001bdf1
--define_symbol ti_sysbios_knl_Swi_destruct=0x1001c115
--define_symbol ti_sysbios_knl_Event_sync__E=0x1001be6f
--define_symbol ti_sysbios_BIOS_getCpuFreq__E=0x1001be91
--define_symbol xdc_runtime_Memory_calloc__E=0x1001c751
--define_symbol ti_sysbios_family_arm_m3_Hwi_startup__E=0x1001c04b
--define_symbol xdc_runtime_System_Module_GateProxy_Handle__label__S=0x1001b999
--define_symbol xdc_runtime_SysCallback_exit__E=0x1001c725
--define_symbol ti_sysbios_hal_Hwi_HwiProxy_Params__init__S=0x1001bf79
--define_symbol ti_sysbios_knl_Queue_empty__E=0x1001bf4f
--define_symbol ti_sysbios_knl_Clock_logTick__E=0x1001b843
--define_symbol ti_sysbios_knl_Event_Params__init__S=0x1001bfd9
--define_symbol ti_sysbios_knl_Task_yield__E=0x1001b0d9
--define_symbol ti_sysbios_knl_Task_SupportProxy_getStackAlignment__E=0x1001bf9d
--define_symbol ti_sysbios_family_arm_m3_Hwi_create=0x1001a8e5
--define_symbol xdc_runtime_Timestamp_SupportProxy_get32__E=0x1001c735
--define_symbol ti_sysbios_family_arm_m3_Hwi_destruct=0x1001c0a9
--define_symbol xdc_runtime_Timestamp_get32__E=0x1001c735
--define_symbol ti_sysbios_knl_Clock_stop__E=0x1001bab9
--define_symbol ti_sysbios_family_arm_m3_Hwi_doTaskRestore__I=0x1001c05b
--define_symbol ti_sysbios_BIOS_linkedWithIncorrectBootLibrary__E=0x1001a683
--define_symbol ti_sysbios_knl_Swi_run__I=0x1001ac75
--define_symbol ti_sysbios_family_arm_cc26xx_Timer_Module__startupDone__S=0x1001b8f9
--define_symbol xdc_runtime_Core_createObject__I=0x1001c2c9
--define_symbol ti_sysbios_knl_Queue_create=0x1001b6cd
--define_symbol ti_sysbios_hal_Hwi_Object__delete__S=0x1001ba19
--define_symbol ti_sysbios_knl_Clock_construct=0x1001b269
--define_symbol xdc_runtime_System_abortSpin__E=0x1001c7a1
--define_symbol ti_sysbios_family_arm_m3_Hwi_Object__destruct__S=0x1001bc39
--define_symbol ti_sysbios_hal_Hwi_HwiProxy_delete=0x1001c0c1
--define_symbol ti_sysbios_gates_GateMutex_Object__create__S=0x1001b36d
--define_symbol ti_sysbios_family_arm_m3_Hwi_getStackInfo__E=0x1001b099
--define_symbol ti_sysbios_heaps_HeapMem_Module_GateProxy_enter__E=0x1001c0c9
--define_symbol ti_sysbios_knl_Semaphore_post__E=0x1001a3e1
--define_symbol ti_sysbios_knl_Task_exit__E=0x1001b119
--define_symbol ti_sysbios_heaps_HeapMem_Instance_init__E=0x1001b18d
--define_symbol ti_sysbios_knl_Swi_restore__E=0x1001b305
--define_symbol ti_sysbios_knl_Task_startCore__E=0x1001aa61
--define_symbol ti_sysbios_knl_Semaphore_create=0x1001b2d1
--define_symbol ti_sysbios_gates_GateHwi_enter__E=0x1001bfa9
--define_symbol ti_sysbios_knl_Task_blockI__E=0x1001b339
--define_symbol ti_sysbios_heaps_HeapMem_free__E=0x1001b3fd
--define_symbol ti_sysbios_knl_Task_Object__destruct__S=0x1001bd6d
--define_symbol ti_sysbios_hal_Hwi_enableInterrupt__E=0x1001af89
--define_symbol ti_sysbios_family_arm_cc26xx_Timer_periodicStub__E=0x10019f25
--define_symbol ti_sysbios_hal_Hwi_Instance_init__E=0x1001b81d
--define_symbol ti_sysbios_gates_GateHwi_query__E=0x1001c0b1
--define_symbol xdc_runtime_System_processAtExit__E=0x1001c5ed
--define_symbol ti_sysbios_family_arm_cc26xx_Timer_setPeriod__E=0x1001c0a1
/* --define_symbol xdc_runtime_Error_init__E=0x1001c745 */
--define_symbol ti_sysbios_knl_Semaphore_Instance_init__E=0x1001a685
--define_symbol xdc_runtime_System_Module_GateProxy_Object__delete__S=0x1001b9b9
--define_symbol xdc_runtime_Memory_HeapProxy_Handle__label__S=0x1001ba39
--define_symbol ti_sysbios_knl_Queue_head__E=0x1001c101
--define_symbol xdc_runtime_Timestamp_SupportProxy_getFreq__E=0x1001bf6b
--define_symbol xdc_runtime_Error_check__E=0x1001c6bd
--define_symbol xdc_runtime_Error_policySpin__E=0x1001c413
--define_symbol ti_sysbios_gates_GateMutex_create=0x1001b39d
--define_symbol xdc_runtime_Gate_leaveSystem__E=0x1001c789
--define_symbol ti_sysbios_knl_Swi_restoreHwi__E=0x1001a6ed
--define_symbol ti_sysbios_knl_Task_sleep__E=0x1001a5a5
/* --define_symbol xdc_runtime_Error_policyDefault__E=0x1001c221 */
--define_symbol ti_sysbios_knl_Task_create=0x1001accd
--define_symbol ti_sysbios_knl_Mailbox_Params__init__S=0x1001bfe5
--define_symbol ti_sysbios_knl_Task_restoreHwi__E=0x1001bc19
--define_symbol ti_sysbios_knl_Mailbox_postInit__I=0x1001b675
--define_symbol ti_sysbios_knl_Task_delete=0x1001c11d
--define_symbol ti_sysbios_heaps_HeapMem_isBlocking__E=0x1001c06d
--define_symbol ti_sysbios_knl_Clock_startI__E=0x1001a1f1
--define_symbol ti_sysbios_knl_Clock_start__E=0x1001be1f
--define_symbol ti_sysbios_heaps_HeapMem_Module_GateProxy_query__E=0x1001c0bd
--define_symbol ti_sysbios_family_arm_m3_Hwi_Object__delete__S=0x1001b959
--define_symbol ti_sysbios_knl_Clock_TimerProxy_getPeriod__E=0x1001c0d5
--define_symbol ti_sysbios_knl_Event_checkEvents__I=0x1001baf9
--define_symbol ti_sysbios_knl_Task_SupportProxy_start__E=0x1001b155
--define_symbol ti_sysbios_heaps_HeapMem_Handle__label__S=0x1001ba39
--define_symbol ti_sysbios_family_arm_m3_Hwi_delete=0x1001c0a5
--define_symbol ti_sysbios_heaps_HeapMem_Module_GateProxy_Handle__label__S=0x1001b9d9
--define_symbol ti_sysbios_knl_Semaphore_Object__delete__S=0x1001b8d5
--define_symbol ti_sysbios_hal_Hwi_HwiProxy_getStackInfo__E=0x1001b099
--define_symbol ti_sysbios_knl_Idle_run__E=0x1001bb19
--define_symbol ti_sysbios_knl_Swi_delete=0x1001c111
--define_symbol xdc_runtime_Memory_valloc__E=0x1001c689
--define_symbol ti_sysbios_knl_Mailbox_Object__delete__S=0x1001bb39
--define_symbol ti_sysbios_family_arm_m3_TaskSupport_start__E=0x1001b155
--define_symbol xdc_runtime_Timestamp_get64__E=0x1001b77d
--define_symbol ti_sysbios_family_arm_m3_Hwi_Module__startupDone__S=0x1001b939
--define_symbol ti_sysbios_heaps_HeapMem_Module_GateProxy_Params__init__S=0x1001bfb5
--define_symbol ti_sysbios_knl_Swi_startup__E=0x1001c085
--define_symbol ti_sysbios_knl_Task_schedule__I=0x1001ae69
--define_symbol ti_sysbios_gates_GateMutex_leave__E=0x1001bdd9
--define_symbol ti_sysbios_heaps_HeapMem_Object__delete__S=0x1001ba59
--define_symbol ti_sysbios_knl_Event_create=0x1001b42d
/* --define_symbol xdc_runtime_Error_setX__E=0x1001c53d */
--define_symbol ti_sysbios_knl_Clock_TimerProxy_setNextTick__E=0x1001bf01
--define_symbol ti_sysbios_knl_Swi_Object__get__S=0x1001b725
--define_symbol ti_sysbios_knl_Task_restore__E=0x1001b7f5
--define_symbol xdc_runtime_Memory_HeapProxy_alloc__E=0x1001c775
--define_symbol ti_sysbios_heaps_HeapMem_Module_GateProxy_Object__delete__S=0x1001b88d
--define_symbol ti_sysbios_gates_GateHwi_Object__create__S=0x1001b869
--define_symbol ti_sysbios_hal_Hwi_HwiProxy_disableInterrupt__E=0x1001af45
--define_symbol ti_sysbios_BIOS_start__E=0x1001beb1
--define_symbol ti_sysbios_BIOS_exit__E=0x1001be81
--define_symbol ti_sysbios_family_arm_m3_TaskSupport_getStackAlignment__E=0x1001bf9d
--define_symbol xdc_runtime_SysCallback_abort__E=0x1001c715
--define_symbol ti_sysbios_knl_Queue_destruct=0x1001c0fd
--define_symbol ti_sysbios_family_arm_m3_Hwi_postInit__I=0x10019e75
--define_symbol ti_sysbios_gates_GateMutex_Instance_init__E=0x1001bc71
--define_symbol ti_sysbios_knl_Task_Instance_finalize__E=0x1001a059
--define_symbol ti_sysbios_knl_Clock_TimerProxy_getCurrentTick__E=0x1001bee1
--define_symbol ti_sysbios_family_arm_m3_Hwi_disableFxn__E=0x1001bf85
--define_symbol xdc_runtime_Memory_HeapProxy_free__E=0x1001c77f
--define_symbol ti_sysbios_knl_Mailbox_Module_startup__E=0x1001afcd
--define_symbol ti_sysbios_knl_Task_Object__delete__S=0x1001bbd9
--define_symbol xdc_runtime_Startup_rtsDone__E=0x1001c769
--define_symbol ti_sysbios_gates_GateHwi_Handle__label__S=0x1001b999
--define_symbol xdc_runtime_Text_ropeText__E=0x1001c6ed
--define_symbol ti_sysbios_knl_Clock_destruct=0x1001c0dd
--define_symbol ti_sysbios_knl_Queue_construct=0x1001b6a1
--define_symbol ti_sysbios_family_arm_m3_Hwi_switchFromBootStack__E=0x1001be35
--define_symbol ti_sysbios_heaps_HeapMem_Object__get__S=0x1001b5c5
--define_symbol ti_sysbios_hal_Hwi_HwiProxy_create=0x1001bc8d
--define_symbol ti_sysbios_gates_GateMutex_query__E=0x1001c0bd
--define_symbol ti_sysbios_knl_Swi_schedule__I=0x1001ad25
--define_symbol ti_sysbios_knl_Task_Params__init__S=0x1001c021
--define_symbol ti_sysbios_family_arm_m3_Hwi_Params__init__S=0x1001bf79
--define_symbol ti_sysbios_family_arm_m3_Hwi_plug__E=0x1001bdc1
--define_symbol xdc_runtime_System_exitSpin__E=0x1001c7a3
--define_symbol xdc_runtime_System_Module_GateProxy_query__E=0x1001c0b1
--define_symbol ti_sysbios_gates_GateMutex_construct=0x1001b541
--define_symbol xdc_runtime_System_Module_GateProxy_leave__E=0x1001c79d
--define_symbol ti_sysbios_knl_Mailbox_pend__E=0x1001a535
--define_symbol ti_sysbios_hal_Hwi_HwiProxy_Object__delete__S=0x1001b959
--define_symbol ti_sysbios_family_arm_m3_TaskSupport_Module__startupDone__S=0x1001c0ad
--define_symbol ti_sysbios_BIOS_RtsGateProxy_Params__init__S=0x1001bfb5
--define_symbol xdc_runtime_Core_assignLabel__I=0x1001c5b9
--define_symbol ti_sysbios_heaps_HeapMem_freeUnprotected__E=0x1001ab6d
--define_symbol xdc_runtime_System_Module_GateProxy_enter__E=0x1001c799
--define_symbol xdc_runtime_System_exit__E=0x1001c6d5
--define_symbol ti_sysbios_knl_Swi_Params__init__S=0x1001c015
--define_symbol ti_sysbios_knl_Event_Object__destruct__S=0x1001bcc5
--define_symbol ti_sysbios_knl_Clock_workFunc__E=0x1001a369
--define_symbol ti_sysbios_family_arm_m3_Hwi_restoreFxn__E=0x1001c061
--define_symbol ti_sysbios_family_arm_cc26xx_TimestampProvider_Module_startup__E=0x1001b919
--define_symbol ti_sysbios_knl_Semaphore_delete=0x1001c109
--define_symbol ti_sysbios_family_arm_cc26xx_Timer_getPeriod__E=0x1001c09d
--define_symbol ti_sysbios_family_arm_m3_Hwi_initNVIC__E=0x1001a171
--define_symbol ti_sysbios_knl_Clock_addI__E=0x1001b7a5
--define_symbol ti_sysbios_family_arm_m3_Hwi_Instance_finalize__E=0x1001a755
--define_symbol ti_sysbios_heaps_HeapMem_alloc__E=0x1001ab15
--define_symbol ti_sysbios_knl_Task_unblockI__E=0x1001b751
--define_symbol ti_sysbios_knl_Swi_Instance_finalize__E=0x1001bf77
--define_symbol ti_sysbios_family_arm_m3_Hwi_disableInterrupt__E=0x1001af45
--define_symbol ti_sysbios_family_arm_m3_Hwi_enableFxn__E=0x1001bf91
--define_symbol xdc_runtime_Gate_enterSystem__E=0x1001c795
--define_symbol ti_sysbios_gates_GateMutex_Object__delete__S=0x1001b88d
--define_symbol ti_sysbios_family_arm_cc26xx_TimestampProvider_get64__E=0x1001b77d
--define_symbol ti_sysbios_knl_Mailbox_Instance_init__E=0x10019dad
--define_symbol xdc_runtime_Text_cordText__E=0x1001c621
--define_symbol xdc_runtime_Startup_exec__E=0x1001c415
--define_symbol ti_sysbios_hal_Hwi_HwiProxy_Module__startupDone__S=0x1001b939
--define_symbol ti_sysbios_heaps_HeapMem_getStats__E=0x1001aefd
--define_symbol xdc_runtime_Memory_getMaxDefaultTypeAlign__E=0x1001c75d
--define_symbol ti_sysbios_hal_Hwi_disableInterrupt__E=0x1001af45
--define_symbol ti_sysbios_knl_Event_post__E=0x1001a455
--define_symbol ti_sysbios_knl_Task_Object__get__S=0x1001bbf9
--define_symbol ti_sysbios_knl_Event_pendTimeout__I=0x1001b61d
--define_symbol ti_sysbios_family_arm_m3_Hwi_construct=0x1001aabd
--define_symbol ti_sysbios_knl_Clock_TimerProxy_Module__startupDone__S=0x1001b8f9
--define_symbol ti_sysbios_knl_Clock_Module_startup__E=0x1001ba79
--define_symbol ti_sysbios_knl_Mailbox_construct=0x1001abc5
--define_symbol ti_sysbios_knl_Task_construct=0x1001ae19
--define_symbol xdc_runtime_Core_constructObject__I=0x1001c4f5
--define_symbol ti_sysbios_knl_Queue_dequeue__E=0x1001bff1
--define_symbol ti_sysbios_knl_Task_Module_startup__E=0x1001aa05
--define_symbol ti_sysbios_family_arm_cc26xx_Timer_getExpiredTicks__E=0x1001c099
--define_symbol ti_sysbios_family_arm_m3_Hwi_Object__get__S=0x1001b515
--define_symbol ti_sysbios_knl_Mailbox_destruct=0x1001c0f5
--define_symbol xdc_runtime_System_Module_startup__E=0x1001c78f
--define_symbol ti_sysbios_knl_Swi_postInit__I=0x1001c119
--define_symbol ti_sysbios_family_arm_m3_Hwi_Module_startup__E=0x1001aeb5
--define_symbol ti_sysbios_gates_GateMutex_enter__E=0x1001b3cd
--define_symbol ti_sysbios_family_arm_m3_Hwi_setPriority__E=0x1001bc55
--define_symbol ti_sysbios_knl_Queue_Object__get__S=0x1001bb79
--define_symbol ti_sysbios_knl_Clock_setTimeout__E=0x1001c0e5
--define_symbol ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I=0x1001c901
--define_symbol ti_sysbios_family_arm_m3_Hwi_return=0x1001c957
--define_symbol ti_sysbios_family_arm_m3_Hwi_pendSV__I=0x1001c93f
--define_symbol ti_sysbios_family_arm_m3_Hwi_dispatch__I=0x1001c7a5
--define_symbol ti_sysbios_family_xxx_Hwi_switchAndRunFunc=0x1001c921
--define_symbol ti_sysbios_family_arm_m3_Hwi_initStacks__E=0x1001c8b9
--define_symbol ti_sysbios_knl_Task_swapReturn=0x1001c971
--define_symbol ti_sysbios_family_arm_m3_TaskSupport_glue=0x1001c959
--define_symbol ti_sysbios_family_arm_m3_TaskSupport_buildTaskStack=0x1001c86d
--define_symbol ti_sysbios_family_arm_m3_TaskSupport_swap__E=0x1001c969
--define_symbol ti_sysbios_knl_Task_SupportProxy_swap__E=0x1001c969

/* Content from ti.sysbios.xdcruntime (null): */

/* Content from ti.sysbios.utils (null): */

/* Content from configPkg (null): */

/* Content from xdc.services.io (null): */


--define_symbol xdc_runtime_Startup__EXECFXN__C=1
--define_symbol xdc_runtime_Startup__RESETFXN__C=1


--config_search C:/ti/simplelink_cc2640r2_sdk_1_40_00_45/kernel/tirtos/packages/ti/sysbios/rom/cortexm/cc26xx/r2/golden/CC26xx/
--config_def USE_TIRTOS_ROM=1

--keep __ASM__
--keep __PLAT__
--keep __ISA__
--keep __TARG__
--keep __TRDR__
