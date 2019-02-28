/*
 *             Copyright (c) 2017, Ghostyu Co.,Ltd.
 *                      All rights reserved.
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
#ifndef _DISPLAY_TFT154_200_200_H_
#define _DISPLAY_TFT154_200_200_H_

#include <ti/drivers/SPI.h>
#include <ti/display/Display.h>
#include <EPD200.h>
#include <stdint.h>
#include <xdc/std.h>

/*
 * 相关的操作函数
 */
extern const Display_FxnTable DisplayEpd200_fxnTable;

/*
 * lcd 
 */
typedef struct DisplayEpd200_HWAttrs
{
    LCD_Handle lcdHandle;
    //PIN_Id     busyPin;
} DisplayEpd200_HWAttrs;


typedef struct DisplayEpd200_Object
{
    PIN_State             pinState;
    //PIN_Handle            hPins;
    LCD_Handle            hLcd;
    Display_LineClearMode lineClearMode;
    Semaphore_Struct      lcdMutex;
    UChar                 lcdBuffer0[1024];
    LCD_Buffer            lcdBuffers[1];
} DisplayEpd200_Object, *DisplayEpd200_Handle;

/*
 *
 */
void DisplayEpd200_init(Display_Handle handle);
/*
 * 
 */
Display_Handle DisplayEpd200_open(Display_Handle, Display_Params * params);
/*
 *
 */
void           DisplayEpd200_clear(Display_Handle handle);
/*
 *
 */
void           DisplayEpd200_clearLines(Display_Handle handle,
                                          uint8_t fromLine,
                                          uint8_t toLine);
/*
 *
 */
void           DisplayEpd200_printf(Display_Handle handle, uint8_t line,
                                    uint8_t column, char *fmt,va_list va);
/*
 *
 */
void DisplayEpd200_close(Display_Handle);
/*
 *
 */
int DisplayEpd200_control(Display_Handle handle, unsigned int cmd, void *arg);
/*
 *
 */
unsigned int   DisplayEpd200_getType(void);

#endif      //_DISPLAY_TFT144_128_128_H_