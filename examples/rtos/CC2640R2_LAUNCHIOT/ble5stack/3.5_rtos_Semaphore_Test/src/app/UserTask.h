/*******************************************************************************
               Copyright (c) 2017-2018, Ghostyu Co.,Ltd.
                           All rights reserved.
  TI_RTOS Semaphores experiment
  Data : $2017-07-05 15:05 $
  Version : $ 00001$
*******************************************************************************/
#ifndef _USER_TASK_H
#define _USER_TASK_H

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

extern void USER_createStartTask(void);
extern void USER_createWorkTask(UInt priority,Task_FuncPtr func,
                                Task_Struct* pStruct);

#endif //_USER_TASK_H