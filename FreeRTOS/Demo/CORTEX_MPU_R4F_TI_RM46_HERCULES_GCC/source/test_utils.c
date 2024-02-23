/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "mpu_wrappers.h"

/* Board Includes */
#include "sci.h"

/* Demo includes */
#include "demo_tasks.h"

PRIVILEGED_FUNCTION portDONT_DISCARD void vHandleMemoryFault(    uint32_t * pulFaultStackAddress )
{
    vTestFaultHandler();
}