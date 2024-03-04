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

/** @brief Privileged data that the created task does not have access to, but
 *  will attempt to write to it, intentionally causing a memory fault.
*/
PRIVILEGED_DATA static uint32_t ulUnwritableByNonPrivileged = 0xFEEDU;

/** @brief Statically declared MPU aligned stack used by the Attempted Write task */
static StackType_t xAttemptedReadTaskStack[ configMINIMAL_STACK_SIZE ]
    __attribute__( ( aligned( configMINIMAL_STACK_SIZE * 0x4U ) ) );

/** @brief Statically declared MPU aligned stack used by task two */
static StackType_t xTaskTwoStack[ configMINIMAL_STACK_SIZE ]
    __attribute__( ( aligned( configMINIMAL_STACK_SIZE * 0x4U ) ) );




/** @brief Statically declared TCB Used by the Attempted Write Task */
PRIVILEGED_DATA static StaticTask_t xAttemptedReadTaskTCB;

/** @brief Statically declared TCB Used by the Task TWo */
PRIVILEGED_DATA static StaticTask_t xTaskTwoTCB;


/* ----------------------- Task Function Declaration ----------------------- */

/** @brief Task function used by the task that attempts to directly write to kernel data.
 *
 * @param pvParameters[in] Parameters as passed during task creation.
 */
static void prvAttemptedReadTask( void * pvParameters );
static void prvTaskTwo( void * pvParameters );


static void prvAttemptedReadTask( void * pvParameters )
{
     /* Unused parameters. */
    ( void ) pvParameters;

    /* This task attempts to write directly to kernel data, 
     * which it does not have permissions to do. */
    for( ;; )
    {
        /* Attempt to write to another task's TCB.
         * This should trigger a data abort.
         */
        /* 
         * Odd - print statement does not execute when line below it is not 
         * commented out
         */
        sci_print("Attempting to read from Task Two's TCB.\r\n\r\n");
        StaticTask_t TCBval = xTaskTwoTCB;

        /* Should not get here as we triggered a data abort. */
        sci_print("Test Failed. Entering an infinite loop.\r\n");
        for(;;)
        {
            /* Test has completed, sit in an infinite loop. */
        }
    }

}

static void prvTaskTwo( void * pvParameters )
{
    /* Unused parameters */
    ( void ) pvParameters;

    for(;;)
    {
        /*
         * Do nothing, this task exists so the other task can attempt 
         * to write to its stack.
         */
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


BaseType_t vRunTest( void )
{
    extern uint32_t __peripherals_start__[];
    extern uint32_t __peripherals_end__[];

    BaseType_t pdStatus;
    uint32_t ulPeriphRegionStart = ( uint32_t ) __peripherals_start__;
    uint32_t ulPeriphRegionSize = ( uint32_t ) __peripherals_end__ - ulPeriphRegionStart;
    uint32_t ulPeriphRegionAttr = portMPU_REGION_PRIV_RW_USER_RW_NOEXEC | portMPU_REGION_ENABLE;

     /* Initialize task parameters for non-privileged task creation. */
    TaskParameters_t xNonPrivilegedTaskParameters =
    {
        .pvTaskCode     = prvAttemptedReadTask,
        .pcName         = "NonPrivileged",
        .usStackDepth   = configMINIMAL_STACK_SIZE,
        .pvParameters   = NULL,
        .uxPriority     = ( ( configMAX_PRIORITIES - 1 ) ),
        .puxStackBuffer = xAttemptedReadTaskStack,
        .pxTaskBuffer   = &xAttemptedReadTaskTCB,
    /* This will give access to only the task's stack and peripherals for writing over UART*/
        .xRegions       = {
                           /* Necessary to write over UART */
                           {( void * ) ulPeriphRegionStart, ulPeriphRegionSize, ulPeriphRegionAttr },}
    };  

    TaskParameters_t xTaskTwoParameters =
    {
        .pvTaskCode     = prvTaskTwo,
        .pcName         = "NonPrivileged",
        .usStackDepth   = configMINIMAL_STACK_SIZE,
        .pvParameters   = NULL,
        .uxPriority     = ( ( configMAX_PRIORITIES - 1 ) ),
        .puxStackBuffer = xTaskTwoStack,
        .pxTaskBuffer   = &xTaskTwoTCB,
    /* This will give access to only the task's stack */
        .xRegions       = {},
    }; 

    sci_print("Creating two tasks, one of which attempts to read from the others TCB\r\n\r\n");

    pdStatus = xTaskCreateRestrictedStatic( &( xNonPrivilegedTaskParameters ), NULL );
    pdStatus = xTaskCreateRestrictedStatic( &( xTaskTwoParameters ), NULL );

    if ( pdStatus == pdPASS )
    {
        sci_print( "\r\n--------------------------- Starting the Scheduler"
                   " ---------------------------\r\n\r\n" );
        vTaskStartScheduler();
    }
    else
    {
        sci_print( "\r\n--------------------------- Failed to Start Scheduler"
                   " ---------------------------\r\n\r\n" );
        while(1)
        {
            /* Scheduler failed to start, sit here forever. */
        }
    }
}