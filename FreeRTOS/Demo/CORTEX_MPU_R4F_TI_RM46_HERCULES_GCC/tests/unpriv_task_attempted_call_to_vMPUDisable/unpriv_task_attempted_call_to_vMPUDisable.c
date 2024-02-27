/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2024 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "mpu_wrappers.h"

/* Board Includes */
#include "sci.h"

/* Demo includes */
#include "demo_tasks.h"

/** @brief Size of the smallest valid MPU region, 32 bytes. */
#define SHARED_MEMORY_SIZE 0x20UL

#if( ( ( SHARED_MEMORY_SIZE % 2UL ) != 0UL ) || ( SHARED_MEMORY_SIZE < 32UL ) )
    #error SHARED_MEMORY_SIZE Must be a power of 2 that is larger than 32
#endif /* ( ( SHARED_MEMORY_SIZE % 2UL ) != 0UL ) || ( SHARED_MEMORY_SIZE < 32UL ) */

/** @brief Statically declared MPU aligned stack used by the Read Only task */
static StackType_t xAttemptedDisableMPUTaskStack[ configMINIMAL_STACK_SIZE ]
    __attribute__( ( aligned( configMINIMAL_STACK_SIZE * 0x4U ) ) );


/** @brief Statically declared TCB Used by the Idle Task */
PRIVILEGED_DATA static StaticTask_t xAttemptedDisableMPUTaskTCB;

static void vTaskAttemptMPUWrite( void ) __attribute__ (( naked ));


/* ----------------------- Task Function Declaration ----------------------- */

/** @brief Task function used by the task that attempts to directly write to kernel data.
 *
 * @param pvParameters[in] Parameters as passed during task creation.
 */
static void prvAttemptedDisableMPUTask( void * pvParameters );


static void prvAttemptedDisableMPUTask( void * pvParameters )
{
     /* Unused parameters. */
    ( void ) pvParameters;

    /* This task attempts to write directly to kernel data, 
     * which it does not have permissions to do. */
    for( ;; )
    {
        /* Attempt to write to privileged data when task does not have access.
         * This should trigger a data abort. 
         */
        sci_print("Attempting to disable the MPU with an unprivileged task.");
        /* 
         * vMPUDisable is a privileged function, so copied the code into 
         * into a function that is able to be called by the task.
         */
        vTaskAttemptMPUWrite();

        sci_print("Test Failed. Entering an infinite loop.\r\n");
        for(;;)
        {
            /* Test has completed, sit in an infinite loop. */
        }
    }

}

static void vTaskAttemptMPUWrite( void )
{
    __asm volatile
    (
        " PUSH    { R0 }                    \n"
        " MRC     p15, #0, R0, c1, c0, #0   \n" /* R0 = System Control Register (SCTLR). */
        " BIC     R0,  R0, #1               \n" /* R0 = R0 & ~0x1. Clear the M bit in SCTLR. */
        " DSB                               \n" /* Wait for all pending data accesses to complete. */
        " MCR     p15, #0, R0, c1, c0, #0   \n" /* SCTLR = R0. */
        /* Flush the pipeline and prefetch buffer(s) in the processor to ensure that
         *  all following instructions are fetched from cache or memory. */
        " ISB                               \n"
        " POP     { R0 }                    \n"
        " BX      LR                        \n"
    );
}

BaseType_t vRunTest( void )
{
    extern uint32_t __peripherals_start__[];
    extern uint32_t __peripherals_end__[];

    uint32_t ulPeriphRegionStart = ( uint32_t ) __peripherals_start__;
    uint32_t ulPeriphRegionSize = ( uint32_t ) __peripherals_end__ - ulPeriphRegionStart;
    uint32_t ulPeriphRegionAttr = portMPU_REGION_PRIV_RW_USER_RW_NOEXEC | portMPU_REGION_ENABLE;

     /* Initialize task parameters for non-privileged task creation. */
    TaskParameters_t xNonPrivilegedTaskParameters =
    {
        .pvTaskCode     = prvAttemptedDisableMPUTask,
        .pcName         = "NonPrivileged",
        .usStackDepth   = configMINIMAL_STACK_SIZE,
        .pvParameters   = NULL,
        .uxPriority     = ( ( configMAX_PRIORITIES - 1 ) ),
        .puxStackBuffer = xAttemptedDisableMPUTaskStack,
        .pxTaskBuffer   = &xAttemptedDisableMPUTaskTCB,
    /* This will give access to only the task's stack and peripherals for writing over UART*/
        .xRegions       = {
                           /* Necessary to write over UART */
                           {( void * ) ulPeriphRegionStart, ulPeriphRegionSize, ulPeriphRegionAttr },}
    };   

    sci_print("Creating the unprivileged task which attempts to directly write to kernel data\r\n\r\n");
    if ( xTaskCreateRestrictedStatic( &( xNonPrivilegedTaskParameters ), NULL ) == pdPASS )
    {
        sci_print( "\r\n--------------------------- Starting the Scheduler"
                   " ---------------------------\r\n\r\n" );
        vTaskStartScheduler();
    }
}
