/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */
#ifndef DEMO_TASKS_H
#define DEMO_TASKS_H

/* ------------------------------ Demo Option ------------------------------ */

/** @brief Create Tasks that are written in assembly to test context swaps */
#define REGISTER_DEMO     0x1

/** @brief Demo that uses timers, timer callbacks, and Queues */
#define QUEUE_DEMO        0x2

/** @brief Demo that causes data aborts and clears them to show MPU usage */
#define MPU_DEMO          0x4

/** @brief Demo that causes and unwinds a Nested IRQ */
#define IRQ_DEMO          0x8

/** @brief Demo that uses the Task Notification APIs */
#define NOTIFICATION_DEMO 0x10

/** @brief Tests that an unprivileged task cannot directly write to kernel data */
#define ATTEMPTED_WRITE_TEST 0x20

/** @brief Tests that an unprivileged task cannot directly write to kernel data */
#define ATTEMPTED_READ_TEST  0x40

/** @brief Tests that an unprivileged task cannot directly write to kernel data */
#define ATTEMPTED_TASK_STACK_READ_TEST  0x80

/** @brief Build Register, Queue, MPU, IRQ, and Notification demos */
#define FULL_DEMO         ( REGISTER_DEMO | QUEUE_DEMO | MPU_DEMO | IRQ_DEMO | NOTIFICATION_DEMO | ATTEMPTED_WRITE_TEST | ATTEMPTED_READ_TEST | ATTEMPTED_TASK_STACK_READ_TEST )

/** @brief Bitfield used to select the Demo Tasks to build and run
 *
 * @note This project contains multiple demo and test tasks. A bitfield is used
 * to select which demos and tests are built and run as part of the executable.
 * More information about what these demos and tests do can be found in their
 * corresponding files.
 *
 * Bit 1 Set: Include the Register Test Tasks
 *
 * Bit 2 Set: Include the Queue Send and Receive Test Tasks
 *
 * Bit 3 Set: Include the MPU Data Abort Test Tasks
 *
 * Bit 4 Set: Include the Nested IRQ Test Tasks
 *
 * Bit 5 Set: Include the Notification Test Tasks
 * 
 * Bit 6 Set: Include the Unprivileged Task Attempted Direct Write To Kernel Data Test
 *
 * Bit 7 Set: Include the Unprivileged Task Attempted Direct Read From Kernel Data Test.
 * 
 * Bit 8 Set: Include the Unprivileged Task Attempted Write to Another Task's Stack Test
 */
#ifndef mainDEMO_TYPE
    #define mainDEMO_TYPE ( FULL_DEMO )
#endif /* mainDEMO_TYPE */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "portmacro.h"
#include "mpu_wrappers.h"


/* Registers required to configure the Real Time Interrupt (RTI). */
#define portRTI_GCTRL_REG        ( *( ( volatile uint32_t * ) 0xFFFFFC00UL ) )
#define portRTI_TBCTRL_REG       ( *( ( volatile uint32_t * ) 0xFFFFFC04UL ) )
#define portRTI_COMPCTRL_REG     ( *( ( volatile uint32_t * ) 0xFFFFFC0CUL ) )
#define portRTI_CNT0_FRC0_REG    ( *( ( volatile uint32_t * ) 0xFFFFFC10UL ) )
#define portRTI_CNT0_UC0_REG     ( *( ( volatile uint32_t * ) 0xFFFFFC14UL ) )
#define portRTI_CNT0_CPUC0_REG   ( *( ( volatile uint32_t * ) 0xFFFFFC18UL ) )
#define portRTI_CNT0_COMP0_REG   ( *( ( volatile uint32_t * ) 0xFFFFFC50UL ) )
#define portRTI_CNT0_UDCP0_REG   ( *( ( volatile uint32_t * ) 0xFFFFFC54UL ) )
#define portRTI_SETINTENA_REG    ( *( ( volatile uint32_t * ) 0xFFFFFC80UL ) )
#define portRTI_CLEARINTENA_REG  ( *( ( volatile uint32_t * ) 0xFFFFFC84UL ) )
#define portRTI_INTFLAG_REG      ( *( ( volatile uint32_t * ) 0xFFFFFC88UL ) )

#define portEND_OF_INTERRUPT_REG ( ( ( volatile uint32_t * ) configEOI_ADDRESS ) )
/* Registers used by the Vectored Interrupt Manager */
typedef void ( *ISRFunction_t )( void );
#define mainVIM_IRQ_INDEX     ( *( ( volatile uint32_t * ) 0xFFFFFE00 ) )
#define mainVIM_IRQ_VEC_REG   ( *( ( volatile ISRFunction_t * ) 0xFFFFFE70 ) )

#define portSSI_INT_REG_BASE  ( ( ( volatile uint32_t * ) 0xFFFFFFB0 ) )

#define portSSI_INT_REG_ONE   ( ( ( volatile uint32_t * ) 0xFFFFFFB0 ) )
#define portSSI_ONE_KEY       0x7500UL

#define portSSI_INT_REG_TWO   ( ( ( volatile uint32_t * ) 0xFFFFFFB4 ) )
#define portSSI_TWO_KEY       0x8400UL

#define portSSI_INT_REG_THREE ( ( ( volatile uint32_t * ) 0xFFFFFFB8 ) )
#define portSSI_THREE_KEY     0x9300UL

#define portSSI_INT_REG_FOUR  ( ( ( volatile uint32_t * ) 0xFFFFFFBC ) )
#define portSSI_FOUR_KEY      0xA200UL

#define portSSI_VEC_REG       ( *( ( volatile uint32_t * ) 0xFFFFFFF4 ) )
#define portSSI_INTFLAG_REG   ( *( ( volatile uint32_t * ) 0xFFFFFFF8 ) )

/* ---------------------- Shared Function Deceleration ---------------------- */

/** @brief Function to toggle LEDs on the RM57-XL2 Launchpad
 * @param ulLED Which LED to flicker
 */
void vToggleLED( uint32_t ulLED );

/* ------------------------------------------------------------------------- */

#endif /* DEMO_TASKS_H */
