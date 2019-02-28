/*********************************************************************
 * INCLUDES
 */
#include <stdbool.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/hal/Hwi.h>

#ifdef USE_ICALL
#include <icall.h>
#else
#include <stdlib.h>
#endif

#include "util.h"
/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Util_constructClock
 *
 * @brief   Initialize a TIRTOS Clock instance.
 *
 * @param   pClock        - pointer to clock instance structure.
 * @param   clockCB       - callback function upon clock expiration.
 * @param   clockDuration - longevity of clock timer in milliseconds
 * @param   clockPeriod   - if set to a value other than 0, the first
 *                          expiry is determined by clockDuration.  All
 *                          subsequent expiries use the clockPeriod value.
 * @param   startFlag     - TRUE to start immediately, FALSE to wait.
 * @param   arg           - argument passed to callback function.
 *
 * @return  Clock_Handle  - a handle to the clock instance.
 */
Clock_Handle Util_constructClock(Clock_Struct *pClock,
                                 Clock_FuncPtr clockCB,
                                 uint32_t clockDuration,
                                 uint32_t clockPeriod,
                                 uint8_t startFlag,
                                 UArg arg)
{
  Clock_Params clockParams;

  // Convert clockDuration in milliseconds to ticks.
  uint32_t clockTicks = clockDuration * (1000 / Clock_tickPeriod);

  // Setup parameters.
  Clock_Params_init(&clockParams);

  // Setup argument.
  clockParams.arg = arg;

  // If period is 0, this is a one-shot timer.
  clockParams.period = clockPeriod * (1000 / Clock_tickPeriod);

  // Starts immediately after construction if true, otherwise wait for a call
  // to start.
  clockParams.startFlag = startFlag;

  // Initialize clock instance.
  Clock_construct(pClock, clockCB, clockTicks, &clockParams);

  return Clock_handle(pClock);
}

/*********************************************************************
 * @fn      Util_startClock
 *
 * @brief   Start a clock.
 *
 * @param   pClock - pointer to clock struct
 *
 * @return  none
 */
void Util_startClock(Clock_Struct *pClock)
{
  Clock_Handle handle = Clock_handle(pClock);

  // Start clock instance
  Clock_start(handle);
}

/*********************************************************************
 * @fn      Util_restartClock
 *
 * @brief   Restart a clock by changing the timeout.
 *
 * @param   pClock - pointer to clock struct
 * @param   clockTimeout - longevity of clock timer in milliseconds
 *
 * @return  none
 */
void Util_restartClock(Clock_Struct *pClock, uint32_t clockTimeout)
{
  uint32_t clockTicks;
  Clock_Handle handle;

  handle = Clock_handle(pClock);

  if (Clock_isActive(handle))
  {
    // Stop clock first
    Clock_stop(handle);
  }

  // Convert timeout in milliseconds to ticks.
  clockTicks = clockTimeout * (1000 / Clock_tickPeriod);

  // Set the initial timeout
  Clock_setTimeout(handle, clockTicks);

  // Start clock instance
  Clock_start(handle);
}

/*********************************************************************
 * @fn      Util_isActive
 *
 * @brief   Determine if a clock is currently active.
 *
 * @param   pClock - pointer to clock struct
 *
 * @return  TRUE if Clock is currently active
            FALSE otherwise
 */
bool Util_isActive(Clock_Struct *pClock)
{
  Clock_Handle handle = Clock_handle(pClock);

  // Start clock instance
  return Clock_isActive(handle);
}

/*********************************************************************
 * @fn      Util_stopClock
 *
 * @brief   Stop a clock.
 *
 * @param   pClock - pointer to clock struct
 *
 * @return  none
 */
void Util_stopClock(Clock_Struct *pClock)
{
  Clock_Handle handle = Clock_handle(pClock);

  // Stop clock instance
  Clock_stop(handle);
}

/*********************************************************************
 * @fn      Util_rescheduleClock
 *
 * @brief   Reschedule a clock by changing the timeout and period values.
 *
 * @param   pClock - pointer to clock struct
 * @param   clockPeriod - longevity of clock timer in milliseconds
 * @return  none
 */
void Util_rescheduleClock(Clock_Struct *pClock, uint32_t clockPeriod)
{
  bool running;
  uint32_t clockTicks;
  Clock_Handle handle;

  handle = Clock_handle(pClock);
  running = Clock_isActive(handle);

  if (running)
  {
    Clock_stop(handle);
  }

  // Convert period in milliseconds to ticks.
  clockTicks = clockPeriod * (1000 / Clock_tickPeriod);

  Clock_setTimeout(handle, clockTicks);
  Clock_setPeriod(handle, clockTicks);

  if (running)
  {
    Clock_start(handle);
  }
}