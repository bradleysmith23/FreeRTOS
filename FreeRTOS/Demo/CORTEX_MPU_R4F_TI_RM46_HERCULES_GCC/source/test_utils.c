/* FreeRTOS includes. */
#include "FreeRTOS.h"


PRIVILEGED_FUNCTION portDONT_DISCARD void vHandleMemoryFault(    uint32_t * pulFaultStackAddress )
{
    vTestFaultHandler();
}