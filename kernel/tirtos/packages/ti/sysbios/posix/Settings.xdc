/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
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
 *  ======== Settings.xdc ========
 */
package ti.sysbios.posix;

/*!
 *  ======== Settings ========
 *  Select ti.sysbios.posix pthread features.
 *
 *  For BIOS pthread documentation, please refer to:
 *  {@link http://processors.wiki.ti.com/index.php/SYS/BIOS_POSIX_Thread_%28pthread%29_Support SYS/BIOS POSIX Thread (pthread) Support}
 */

@Template("./Settings.xdt")

module Settings
{
    /*!
     *  ======== supportsMutexPriority ========
     *  Include support for pthread mutex priority protocols. Default is false.
     *
     *  Set to true to enable pthread mutex priority protocols.
     *  If set to true, PTHREAD_PRIO_PROTECT and PTHREAD_PRIO_INHERIT mutexes
     *  will be available.  The default value is set to false to reduce
     *  code size.
     */
    config Bool supportsMutexPriority = false;

    /*!
     *  ======== debug ========
     *  Enable debug functions. Default is true.
     *  @_nodoc
     */
    config Bool debug = false;
}
