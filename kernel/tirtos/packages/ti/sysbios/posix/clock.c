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
/*
 *  ======== clock.c ========
 */

#include <xdc/std.h>

#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/hal/Seconds.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

#include "time.h"
#include "errno.h"
#include "pthread_util.h"

static UInt32 secsPerRollover = 0;
static UInt32 prevTicks = 0;
static UInt32 rolloverCount = 0;

/*
 *  ======== clock_gettime ========
 */
int clock_gettime(clockid_t clockId, struct timespec *ts)
{
    Seconds_Time t;
    UInt32       secs;
    UInt32       ticks;
    UInt32       remTicks;
    UInt         key;
    UInt32       numRollovers;

    if (clockId == CLOCK_REALTIME) {
        Seconds_getTime(&t);

        ts->tv_sec = t.secs;
        ts->tv_nsec = t.nsecs;
    }
    else {
        /* CLOCK_MONOTONIC */
        if (secsPerRollover == 0) {
            /* Initialize number of seconds before tick count wraps */
            secsPerRollover = (1 + 4294967295 / (1000000 / Clock_tickPeriod));
        }

        key = Hwi_disable();

        ticks = Clock_getTicks();
        if (ticks < prevTicks) {
            rolloverCount++;
        }
        prevTicks = ticks;
        numRollovers = rolloverCount;

        Hwi_restore(key);

        secs = ticks / (1000000 / Clock_tickPeriod);
        remTicks = ticks - secs * (1000000 / Clock_tickPeriod);

        ts->tv_sec = (time_t)secs + secsPerRollover * numRollovers;
        ts->tv_nsec = (unsigned long)(remTicks * Clock_tickPeriod  * 1000);
    }

    return (0);
}

/*
 *  ======== clock_nanosleep ========
 */
int clock_nanosleep(clockid_t clockId, int flags,
        const struct timespec *rqtp, struct timespec *rmtp)
{
    uint32_t ticks;

    if (rmtp != NULL) {
        /*
         *  In the relative case, rmtp will contain the amount of time
         *  remaining (requested time - actual time slept).  For BIOS,
         *  this will always be 0.
         */
        rmtp->tv_nsec = rmtp->tv_sec = 0;
    }

    if (flags & TIMER_ABSTIME) {
        if (_pthread_abstime2ticks(clockId, rqtp, &ticks) != 0) {
            return (EINVAL);
        }

        if (ticks == 0) {
            return (0);
        }
    }
    else {
        if ((rqtp->tv_sec == 0) && (rqtp->tv_nsec == 0)) {
            return (0);
        }

        ticks = rqtp->tv_sec * (1000000 / Clock_tickPeriod);

        /* Take the ceiling */
        ticks += (rqtp->tv_nsec + Clock_tickPeriod * 1000 - 1) /
                (Clock_tickPeriod * 1000);

        /*
         *  Add one tick to ensure the timeout is not less than the
         *  amount of time requested.  The Clock may be about to tick,
         *  and that counts as one tick even though the amount of time
         *  until this tick is less than Clock_tickPeriod.
         */
        ticks++;
    }

    Task_sleep(ticks);

    return (0);
}

/*
 *  ======== clock_settime ========
 */
int clock_settime(clockid_t clock_id, const struct timespec *ts)
{
    if (clock_id == CLOCK_MONOTONIC) {
        /* EINVAL */
        return (-1);
    }

    Seconds_set(ts->tv_sec);

    return (0);
}
