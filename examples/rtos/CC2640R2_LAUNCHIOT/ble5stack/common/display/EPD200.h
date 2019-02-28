/*
 * Copyright (c) 2015, Ghostyu  Co.Ltd.,
 *          All rights reserved.
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

#ifndef LCD_TFT_1_54_200X200_H
#define LCD_TFT_1_54_200X200_H

#ifdef __cplusplus
extern "C" {
#endif

#include <xdc/std.h>
#include <ti/drivers/SPI.h>

#define ti_sysbios_family_arm_m3_Hwi__nolocalnames
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/sysbios/knl/Semaphore.h>

/*! @internal Number of pixels in LCD display */
#define LCD_PIXELS          40000       /* 200 * 200 = 40000 */
/*! Number of bytes needed in LCD buffer */
#define LCD_BYTES           1024
/*! Number of pixel columns */
#define LCD_COLS            25
/*! First pixel on LCD x-axis */
#define LCD_X_MIN           0
/*! Last pixel on LCD x-axis */
#define LCD_X_MAX           (LCD_COLS -1 )
/*! Number of pixel rows */
#define LCD_ROWS            200
/*! First pixel on LCD y-axis */
#define LCD_Y_MIN           0
/*! Last pixel on LCD y-axis */
#define LCD_Y_MAX           (LCD_ROWS-1)
/*! @internal Number of pages */
#define LCD_PAGES           10
/*! Number of pixel rows per LCD page */
#define LCD_PAGE_ROWS       20
/*! Space used for each character */
#define LCD_CHAR_WIDTH      10
/*! Actual font character width */
#define LCD_FONT_WIDTH      20


/*!
*  @brief      A handle that is returned from a LCD_open() call.
*/
typedef struct LCD_Config   *LCD_Handle;

/*!
 *  @brief      LCD alignment enum
 *
 *  This enumeration defines the text alignment.
 *  It is used by LCD_bufferPrintxxxAligned functions.
 *
 */
typedef enum LCD_Align
{
    LCD_ALIGN_LEFT,     /*!< Text is aligned to the left */
    LCD_ALIGN_CENTER,   /*!< Text is aligned in the center */
    LCD_ALIGN_RIGHT     /*!< Text is aligned to the right */
} LCD_Align;

/*!
 *  @brief      LCD page enum
 *
 *  This enumeration defines the LCD pages.
 *  It is used by LCD_bufferxxx functions
 *
 */
typedef enum LCD_Page
{
    LCD_PAGE0 = 0,
    LCD_PAGE1,
    LCD_PAGE2,
    LCD_PAGE3,
    LCD_PAGE4,
    LCD_PAGE5,
    LCD_PAGE6,
    LCD_PAGE7,
    LCD_PAGE8,
    LCD_PAGE9,
    LCD_PAGE_COUNT
} LCD_Page;

/*!
 *  @brief      LCD x-axis enum
 *
 *  This enumeration defines x axis limit
 *  It is used for x argument in LCD_bufferxxx functions
 *
 */
typedef enum LCD_X_Limit
{
    LCD_X_FIRST = 0,
    LCD_X_LAST = (LCD_COLS-1)
} LCD_X_Limit;

/*!
 *  @brief      LCD y-axis enum
 *
 *  This enumeration defines y axis limit
 *  It is used for y argument in LCD_bufferxxx functions
 *
 */
typedef enum LCD_Y_Limit
{
    LCD_Y_FIRST = 0,
    LCD_Y_LAST = (LCD_ROWS-1)
} LCD_Y_Limit;

/*!
 *  @brief
 *  LCD Parameters are used to with the LCD_open() call. Default values for
 *  these parameters are set using LCD_Params_init().
 *
 *  @sa     SPI_Params_init
 */
typedef struct LCD_Params
{
     unsigned int       lcdWriteTimeout;    /*!< Timeout for write semaphore */

     /* SPI parameters */
     unsigned int       spiBitRate;         /*!< SPI bit rate in Hz */
     SPI_FrameFormat    spiFrameFormat;     /*!< SPI frame format */
} LCD_Params;

typedef struct LCD_Param_CMD
{
  char cmdId;
  char count;
  char param[10];
}LCD_Param_CMD;


/*!
 *  @brief Format of LCD commands used by the LCD controller.
 *
 */
typedef struct LCD_Command
{

    LCD_Param_CMD      GDControl;
    LCD_Param_CMD      Softstart;
    LCD_Param_CMD      VCOMVol;
    LCD_Param_CMD      DummyLine;
    LCD_Param_CMD      Gatetime;
    LCD_Param_CMD      RamDataEntryMode;
    LCD_Param_CMD      XRange;
    LCD_Param_CMD      YRange;
    LCD_Param_CMD      XAddrCounter;
    LCD_Param_CMD      YAddrCounter;
//    LCD_Param_CMD      LUTRegister;
//    LCD_Param_CMD      DisplayCtl2;
//    LCD_Param_CMD      MasterActivation;
} LCD_Command;

/*!
 *  @brief  LCD_Buffer used to store data to be printed on the LCD display.
 *
 *  A sample structure is shown below:
 *  @code
 *  LCD_Buffer lcdBuffers[] = {
 *      {lcdBuffer0, LCD_BYTES, NULL},
 *      {lcdBuffer1, LCD_BYTES, NULL},
 *  };
 *  @endcode
 */
typedef struct LCD_Buffer
{
    unsigned char      *pcBuffer;  /*!< Ptr to a buffer with data to be transmitted */
    unsigned int        bufSize;    /*!< Size of the buffer */
    Semaphore_Struct    bufMutex;   /*!< Semaphore associated with the buffer */
} LCD_Buffer;

/*!
 *  @brief      LCD Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct LCD_Object
{
    /* LCD control variables */
    Bool                 isOpen;            /* Has the obj been opened? */
    Bool                 isFull;            /* The LCD has two refresh mode: full refresh or part refresh*/
    SPI_Handle           spiHandle;         /* SPI handle used by the LCD */
    unsigned int         lcdWriteTimeout;   /* Timeout for write semaphore */
    Semaphore_Struct     lcdMutex;          /* Semaphore associated with the lcd */
    LCD_Buffer           *lcdBuffers;       /* Pointer to the array
                                               with lcd buffers */
    uint8_t              nBuffers;          /* number of buffers availible */
} LCD_Object;

/*!
 *  @brief      LCD Hardware attributes
 *
 *  A sample structure is shown below:
 *  @code
 *  const LCD_HWAttrs lcdHWAttrs = {
 *      .LCD_initCmd = &LCD_initCmd,
 *      .lcdResetPin = Board_LCD_RST,
 *      .lcdModePin = Board_LCD_MODE,
 *      .lcdCsnPin = Board_LCD_CSN,
 *      .spiIndex = Board_SPI0
 *  };
 *  @endcode
 */
typedef struct LCD_HWAttrs
{
    /*! LCD initialization command */
    LCD_Command const *LCD_initCmd;
    /* LCD mode pin */
    uint8_t           lcdModePin;
    /* LCD CSn pin */
    uint8_t           lcdCsnPin;
    /* busy pin*/
    uint8_t           lcdBusyPin;
    /* Logical peripheral number indexed
       into the SPI_config table */
    uint8_t           spiIndex;

} LCD_HWAttrs;

/*!
 *  @brief
 *  The LCD_Config structure contains a set of pointers used to characterize
 *  the LCD driver implementation.
 */
typedef struct LCD_Config
{
  /*! Pointer to a driver specific data object */
  LCD_Object            *object;
  /*! Pointer to a driver specific hardware attributes structure */
  LCD_HWAttrs   const   *hwAttrs;
} LCD_Config;

/*! @internal
 *
 *  @brief   LCD initialization command sequence used to initialize
 *           The LCD display. The format of the command in defined in
 *           the LCD_Command structure
 *
 *  @sa      LCD_Command
 */
static const LCD_Command LCD_initCmd =
{
  {0x01,3,LCD_Y_MAX,0,0x00},  /* Driver output control: Gate setting */
  {0x0C,3,0xD7,0xD6,0x9D}, /*Booster soft start control */
  {0x2C,1,0x9A},           /*Write VCOM Register */
  {0x3A,1,0x1A},           /*Set dummy line*/
  {0x3B,1,0x08},           /*Set Gate line width */
  {0x11,1,0x01},           /*Data entry mode set: Y = increment, X = increment*/
  {0x44,2,0x00,0x18}, /*X start and end  */
  {0x45,4,LCD_Y_MAX, 0 ,0x00, 0x00},      /* Y start and end */
  {0x4E,1,0x00},           /* X Addr counter */
  {0x4F,2,LCD_Y_MAX, 0},  /* Y Addr counter */
//  {0x32,30,0x00,0x02,0x12,0x11,0x51,0x61,0x61,0x62,0x62,0x66,0x26,0x2A,0x1A,0x19,0x19,
//   0x19,0x99,0x99,0x88,0x00,0x80,0x35,0x22,0x11,0x13,0x11,0x21,0x11,0x72,0x11},           /*LUT*/
//  {0x22,1,0xC0},           /* Display update control 2 */
//  {0x20,0}                 /* Master activtion */

};

/*!
 *  @brief  Function to close the LCD instance specified by the
 *          LCD handle.
 *
 *  @pre    LCD_open() has to be called first. Function assumes that the handle is not NULL.
 *          Calling context: Task.
 *
 *  @param  handle A LCD handle returned from LCD_open()
 *
 *  @sa     LCD_open()
 */
extern void LCDEPD_close(LCD_Handle handle);

/*!
 *  @brief  This function initializes the LCD driver module.
 *
 *  @pre    The LCD_config structure must exist and be persistent before
 *          this function can be called. This function must be called before any
 *          other LCD driver APIs.
 *          Calling context: Hwi, Swi, Task, Main.
 */
extern void LCDEPD_init(void);

/*!
 *  @brief   Function to set up the DOGM128W-6 LCD display
 *
 *  @pre     LCD controller has been initialized using LCD_init().
 *           Calling context: Task.
 *
 *  @param   buffers     Pointer to a buffer block.
 *
 *  @param   nBuffers    Number of buffers in the buffer block.
 *
 *  @param   params      Pointer to a parameter block, if NULL it will use
 *                       default values
 *
 *  @return  A pointer to a LCD_Handle on success or a NULL it was already
 *           opened
 *
 *  @sa      LCD_close()
 *  @sa      LCD_init()
 */
extern LCD_Handle LCDEPD_open(LCD_Buffer *buffers, uint8_t nBuffers, LCD_Params *params);

/*!
 *  @brief  Function to initialize the LCD_Params struct to its defaults
 *
 *  Defaults values are:
 *  @code
 *  lcdWriteTimeout = BIOS_WAIT_FOREVER;
 *  spiBitRate      = 1000000;
 *  spiFrameFormat  = SPI_POL0_PHA0;
 *  @endcode
 *
 *  @pre    Calling context: Task.
 *
 *  @param  params  Parameter structure to initialize
 */
extern void LCDEPD_Params_init(LCD_Params *params);

/*!
 *  @brief  Function that writes a string and value to a
 *          buffer and sends it to the LCD display. The written page is being
 *          cleared before it is written to.
 *
 *          LCD_write will block task execution until all
 *          the data in buffer has been written to the LCD.
 *
 *  @pre    Function assumes that the handle and buffer is not NULL.
 *          Calling context: Task
 *
 *  @param  handle     A LCD_Handle
 *
 *  @param  bufIndex   The buffer to use, specified by its index in the LCD_Buffer structure.
 *
 *  @param  str        A pointer to the string to print.
 *
 *  @param  uiValue    Value to print
 *
 *  @param  ucFormat   Base of the value to print ?2,8,16 etc.
 *                     ucFormat must be between 2 and 36, where 10 means decimal base,
 *                     16 hexadecimal, 8 octal, and 2 binary.
 *                     If ucFormat is zero, only the string will get printed.
 *
 *  @param  ucLine     The page to write. Must be a value from 0-7.
 *
 */
extern void LCDEPD_writeLine(LCD_Handle handle, char *str, unsigned int uiValue, unsigned char ucFormat, unsigned char ucLine);


/*!
 *  @brief  Function that empties the specified LCD buffer
 *
 *          LCD_bufferClear will block task execution until all
 *          the buffer modification has finished.
 *
 *  @pre    Function assumes that the handle and buffer is not NULL.
 *          Calling context: Task.
 *
 *  @param  handle      A LCD_Handle
 *
 *  @param  bufIndex    The buffer to use, specified by its index in the LCD_Buffer structure.
 *
 */
extern void LCDEPD_bufferClear(LCD_Handle handle);
/*!
 *  @brief  display start cover 
 *
 *          LCD_bufferStartCover will block task execution until all
 *          the buffer modification has finished.
 *
 *  @pre    Function assumes that the handle and buffer is not NULL.
 *          Calling context: Task.
 *
 *  @param  handle      A LCD_Handle
 *
 *  @param  bufIndex    The buffer to use, specified by its index in the LCD_Buffer structure.
 *
 */
extern void LCDEPD_bufferStartCover(LCD_Handle handle);

/*!
 *  @brief  This function clears the page specified by @e iPage in the given buffer
 *
 *          LCD_bufferClearPage will block task execution until all
 *          the buffer modification has finished.
 *
 *  @pre    Function assumes that the handle and buffer is not NULL.
 *          Calling context: Task.
 *
 *  @param  handle      A LCD_Handle
 *
 *  @param  bufIndex    The buffer to use, specified by its index in the LCD_Buffer structure.
 *
 *  @param  iPage       The page to clear. Must be one of the
 *                      following enumerated values:
 *                      @li @b LCD_PAGE0
 *                      @li @b LCD_PAGE1
 *                      @li @b LCD_PAGE2
 *                      @li @b LCD_PAGE3
 *                      @li @b LCD_PAGE4
 *                      @li @b LCD_PAGE5
 *                      @li @b LCD_PAGE6
 *                      @li @b LCD_PAGE7
 *
 *  @return   None
 *
 */
extern void LCDEPD_bufferClearPage(LCD_Handle handle, LCD_Page iPage);

/*!
 *  @brief  This function clears the pixels in a given piece of a page.
 *          Resolution is given in coulmns [0--127] and pages [0--7]. The
 *          function assumes @e ucXFrom <= @e ucXTo and
 *          @e iPageFrom <= @e iPageTo.
 *
 *          LCD_bufferClearPart will block task execution until all
 *          the buffer modification has finished.
 *
 *  @pre    Function assumes that the handle and buffer is not NULL.
 *          Calling context: Task.
 *
 *  @param  handle     A LCD_Handle
 *
 *  @param  bufIndex   The buffer to use, specified by its index in the LCD_Buffer structure.
 *
 *  @param  ucXFrom    The lowest x-position (column) to be cleared [0--127].
 *
 *  @param  ucXTo      The highest x-position to be cleared [ucXFrom--127].
 *
 *  @param  iPageFrom  The first page cleared. Must be one of the
 *                     following enumerated values:
 *                     @li @b LCD_PAGE0
 *                     @li @b LCD_PAGE1
 *                     @li @b LCD_PAGE2
 *                     @li @b LCD_PAGE3
 *                     @li @b LCD_PAGE4
 *                     @li @b LCD_PAGE5
 *                     @li @b LCD_PAGE6
 *                     @li @b LCD_PAGE7
 *
 * @param    iPageTo   The last page cleared [iPageFrom--LCD_PAGE7].
 *
 * @return   None
 *
 */
extern void LCDEPD_bufferClearPart(LCD_Handle handle,unsigned char ucXFrom, unsigned char ucXTo,
                   LCD_Page iPageFrom, LCD_Page iPageTo);

/*!
 *  @brief  This function inverts the pixels (bits) in a given region of the
 *          specified buffer.
 *
 *          LCD_bufferInvert will block task execution until all
 *          the buffer modification has finished.
 *
 *  @pre    Function assumes that the handle and buffer is not NULL.
 *          Calling context: Task.
 *
 *  @param  handle     A LCD_Handle
 *
 *  @param  bufIndex   The buffer to use, specified by its index in the LCD_Buffer structure.
 *
 *  @param  ucXFrom    The first x-position (column) to invert [0--127].
 *
 *  @param  ucYFrom    is the first y-position (row) to invert [0--63].
 *
 *  @param  ucXTo      The last x-position (column) to invert [0--127].
 *
 *  @param  ucYTo      The last y-position (row) to invert [0--63].
 *
 *  @return None
 *
 */
extern void LCDEPD_bufferInvert(LCD_Handle handle, unsigned char ucXFrom, unsigned char ucYFrom,
                    unsigned char ucXTo, unsigned char ucYTo);

/*!
 *  @brief  This function inverts a range of columns in the display buffer on a
 *          specified page (for example, @b LCD_PAGE0). This function assumes
 *          @e ucXFrom <= @e ucXTo.
 *
 *          LCD_bufferInvertPage will block task execution until all
 *          the buffer modification has finished.
 *
 *  @pre    Function assumes that the handle and buffer is not NULL.
 *          Calling context: Task.
 *
 *  @param  handle     A LCD_Handle
 *
 *  @param  bufIndex   The buffer to use, specified by its index in the LCD_Buffer structure.
 *
 *  @param  ucXFrom    The first x-position (column) to invert [0--127].
 *
 *  @param  ucXTo      The last x-position (column) to invert [ucXFrom--127].
 *
 *  @param  iPage      The page on which to invert. Must be one of the
 *                     following enumerated values:
 *                     @li @b LCD_PAGE0
 *                     @li @b LCD_PAGE1
 *                     @li @b LCD_PAGE2
 *                     @li @b LCD_PAGE3
 *                     @li @b LCD_PAGE4
 *                     @li @b LCD_PAGE5
 *                     @li @b LCD_PAGE6
 *                     @li @b LCD_PAGE7
 *
 *  @return   None
 */
extern void LCDEPD_bufferInvertPage(LCD_Handle handle, unsigned char ucXFrom, unsigned char ucXTo,
                    LCD_Page iPage);

/*!
 *  @brief  Function that writes a string to the specified buffer
 *
 *          LCD_bufferPrintString will block task execution until all
 *          the buffer modification has finished.
 *
 *  @pre    Function assumes that the handle and buffer is not NULL.
 *          Calling context: Task.
 *
 *  @param  handle      A LCD_Handle
 *
 *  @param  bufIndex    The buffer to use, specified by its index in the LCD_Buffer structure.
 *
 *  @param  pcStr       A pointer to the string to print.
 *
 *  @param  ucX         The x-position (column) to begin printing [0--127].
 *
 *  @param  iPage       The page on which to print. Must be one of the
 *                      following enumerated values:
 *                      @li @b LCD_PAGE0
 *                      @li @b LCD_PAGE1
 *                      @li @b LCD_PAGE2
 *                      @li @b LCD_PAGE3
 *                      @li @b LCD_PAGE4
 *                      @li @b LCD_PAGE5
 *                      @li @b LCD_PAGE6
 *                      @li @b LCD_PAGE7
 *
 *
 */
extern void LCDEPD_bufferPrintString(LCD_Handle handle, const char *pcStr, unsigned char ucX,
                                  LCD_Page iPage);


extern void LCDEPD_setRefreshMode(LCD_Handle handle,bool isFull);


/* Do not interfere with the app if they include the family Hwi module */
#undef ti_sysbios_family_arm_m3_Hwi__nolocalnames

#ifdef __cplusplus
}
#endif

#endif /* LCD_TFT_1_44_128X128_H */