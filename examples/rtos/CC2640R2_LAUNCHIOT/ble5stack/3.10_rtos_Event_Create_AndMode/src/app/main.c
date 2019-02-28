//*
/*******************************************************************************
*           Copyright(c) 2017-2018, Ghostyu Co.,Ltd
*                  All rights reserved.
*
* 
*  Forum : www.iotxx.com
*          
*
*******************************************************************************/
//*

/* XDCtools Header files */
#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
// #include <ti/drivers/I2C.h>
#include <ti/drivers/PIN.h>
// #include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>
// #include <ti/drivers/Watchdog.h>
#include <ti/display/Display.h>
/* Board Header files */
#include "Board.h"



#include "UserTask.h"

/*
 *  ======== main ========
 */
int main(void)
{
  /* Call board init functions */
  Board_initGeneral();
  // Board_initI2C();
  // Board_initSPI();
  
  
  Board_initUART();
  // Board_initWatchdog();
  USER_createStartTask();
  /* Start BIOS */
  BIOS_start();
  
  return (0);
}