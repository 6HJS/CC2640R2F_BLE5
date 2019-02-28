#ifndef CLOCK_UTIL_H
#define CLOCK_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * INCLUDES
 */
#include <stdbool.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>

/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * API FUNCTIONS
 */

/**
 * @brief   Initialize a TIRTOS Clock instance.
 *
 * @param   pClock        - pointer to clock instance structure.
 * @param   clockCB       - callback function upon clock expiration.
 * @param   clockDuration - longevity of clock timer in milliseconds
 * @param   clockPeriod   - duration of a periodic clock, used continuously
 *                          after clockDuration expires.
 * @param   startFlag     - TRUE to start immediately, FALSE to wait.
 * @param   arg           - argument passed to callback function.
 *
 * @return  Clock_Handle  - a handle to the clock instance.
 */
extern Clock_Handle Util_constructClock(Clock_Struct *pClock,
                                        Clock_FuncPtr clockCB,
                                        uint32_t clockDuration,
                                        uint32_t clockPeriod,
                                        uint8_t startFlag,
                                        UArg arg);

/**
 * @brief   Start a clock.
 *
 * @param   pClock - pointer to clock struct
 */
extern void Util_startClock(Clock_Struct *pClock);

/**
 * @brief   Restart a clock by changing the timeout.
 *
 * @param   pClock - pointer to clock struct
 * @param   clockTimeout - longevity of clock timer in milliseconds
 */
extern void Util_restartClock(Clock_Struct *pClock, uint32_t clockTimeout);

/**
 * @brief   Determine if a clock is currently active.
 *
 * @param   pClock - pointer to clock struct
 *
 * @return  TRUE or FALSE
 */
extern bool Util_isActive(Clock_Struct *pClock);

/**
 * @brief   Stop a clock.
 *
 * @param   pClock - pointer to clock struct
 */
extern void Util_stopClock(Clock_Struct *pClock);

/**
 * @brief   Reschedule a clock by changing the timeout and period values.
 *
 * @param   pClock - pointer to clock struct
 * @param   clockPeriod - longevity of clock timer in milliseconds
 */
extern void Util_rescheduleClock(Clock_Struct *pClock, uint32_t clockPeriod);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* UTIL_H */

/** @} End Util */