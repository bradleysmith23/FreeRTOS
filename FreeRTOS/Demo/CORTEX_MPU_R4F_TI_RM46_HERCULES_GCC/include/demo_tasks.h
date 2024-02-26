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
