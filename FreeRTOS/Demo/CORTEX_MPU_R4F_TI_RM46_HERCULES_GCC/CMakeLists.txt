cmake_minimum_required(VERSION 3.15)

SET(CMAKE_CROSSCOMPILING "TRUE" CACHE STRING "Set Cross Compiling to true" FORCE)

# Strip the default MacOSX flags that cause cross-compilations to fail.
SET(CMAKE_OSX_DEPLOYMENT_TARGET "" CACHE STRING "Force unset of the deployment target for iOS" FORCE)
SET(CMAKE_OSX_SYSROOT "" CACHE STRING "Force unset of the deployment target for iOS" FORCE)

# Set the compiler before declaring the project for the test build
SET(CMAKE_C_COMPILER "arm-none-eabi-gcc")
SET(CMAKE_ASM_COMPILER "arm-none-eabi-gcc")

# Set the system processor and name before declaring the project
# Needs to be set here otherwise it will fail the test compilation
SET(CMAKE_SYSTEM_NAME "Generic" CACHE STRING "Target system is a generic ARM Processor")
SET(CMAKE_SYSTEM_PROCESSOR "armv7-r" CACHE STRING "Target system is an ARM7r Processor")

# Set the ASM and C compilation flags
SET(CMAKE_ASM_FLAGS "-mcpu=cortex-r4 -mfpu=vfpv3-d16 -Og -g -ggdb -Wall -MMD -MP")
SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -specs=\"nosys.specs\" -specs=\"nano.specs\"")
SET(CMAKE_C_FLAGS "${CMAKE_ASM_FLAGS} -marm -mfloat-abi=hard")

project(RM46_FreeRTOS C ASM)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR} CACHE STRING "")

# Increase the debug level of the CMAKE build
SET(CMAKE_VERBOSE_MAKEFILE ON)

# Get the absolute path to the Demo Directory
SET(DEMO_DIR_REL "${CMAKE_CURRENT_SOURCE_DIR}")
GET_FILENAME_COMPONENT(DEMO_DIR ${DEMO_DIR_REL} ABSOLUTE)

# Get the absolute path to the Board Files
SET(BOARD_FILES_DIR_REL "${DEMO_DIR}/BoardFiles")
GET_FILENAME_COMPONENT(BOARD_FILES_DIR ${BOARD_FILES_DIR_REL} ABSOLUTE)

SET(FREERTOS_CONFIG_FILE_DIRECTORY "${DEMO_DIR}/include" CACHE STRING "Config File Path")
SET(FREERTOS_PORT "GCC_ARM_CRX_MPU" CACHE STRING "FreeRTOS Port to Use")

# Clone the tag of the FreeRTOS-Kernel last tested with this project.
INCLUDE(FetchContent)

FetchContent_Declare(
    FreeRTOS-Kernel
    GIT_REPOSITORY https://github.com/Skptak/FreeRTOS-Kernel.git
    GIT_TAG ARM_CRx_MPU
    SOURCE_DIR "FreeRTOS-Kernel"
    USES_TERMINAL_DOWNLOAD YES
    USES_TERMINAL_UPDATE   YES
    BUILD_COMMAND "")

FetchContent_MakeAvailable(FreeRTOS-Kernel)

# Get the absolute path to the FreeRTOS-Kernel Directory
SET(FREERTOS_KERNEL_DIR_REL "${freertos-kernel_SOURCE_DIR}")
GET_FILENAME_COMPONENT(FREERTOS_KERNEL_DIR ${FREERTOS_KERNEL_DIR_REL} ABSOLUTE)

# Get the absolute path to the Port Directory
SET(PORT_DIR_REL "${freertos-kernel_SOURCE_DIR}/portable/GCC/ARM_CRx_MPU")
GET_FILENAME_COMPONENT(PORT_DIR ${PORT_DIR_REL} ABSOLUTE)

# Debug
MESSAGE("Project: ${PROJECT_NAME}")
MESSAGE("Demo Directory: ${DEMO_DIR}")
MESSAGE("FREERTOS_KERNEL_DIR: ${FREERTOS_KERNEL_DIR}")
MESSAGE("PORT_DIR: ${PORT_DIR}")

INCLUDE_DIRECTORIES(
    ${DEMO_DIR}
    ${DEMO_DIR}/include
    ${BOARD_FILES_DIR}/include
    ${FREERTOS_KERNEL_DIR}/include
    ${PORT_DIR}
)

# Source files used for the Board Support Package
ADD_LIBRARY(TI_BOARD_SUPPORT_PACKAGE OBJECT
    ${BOARD_FILES_DIR}/source/adc.c
    ${BOARD_FILES_DIR}/source/can.c
    ${BOARD_FILES_DIR}/source/dabort.S
    ${BOARD_FILES_DIR}/source/emac.c
    ${BOARD_FILES_DIR}/source/emif.c
    ${BOARD_FILES_DIR}/source/errata_SSWF021_45.c
    ${BOARD_FILES_DIR}/source/esm.c
    ${BOARD_FILES_DIR}/source/gio.c
    ${BOARD_FILES_DIR}/source/het.c
    ${BOARD_FILES_DIR}/source/lin.c
    ${BOARD_FILES_DIR}/source/mdio.c
    ${BOARD_FILES_DIR}/source/mibspi.c
    ${BOARD_FILES_DIR}/source/notification.c
    ${BOARD_FILES_DIR}/source/phy_dp83640.c
    ${BOARD_FILES_DIR}/source/pinmux.c
    ${BOARD_FILES_DIR}/source/sci.c
    ${BOARD_FILES_DIR}/source/spi.c
    ${BOARD_FILES_DIR}/source/sys_core.S
    ${BOARD_FILES_DIR}/source/sys_dma.c
    ${BOARD_FILES_DIR}/source/sys_intvecs.S
    ${BOARD_FILES_DIR}/source/sys_link.ld
    ${BOARD_FILES_DIR}/source/sys_main.c
    ${BOARD_FILES_DIR}/source/sys_pcr.c
    ${BOARD_FILES_DIR}/source/sys_phantom.c
    ${BOARD_FILES_DIR}/source/sys_pmm.c
    ${BOARD_FILES_DIR}/source/sys_pmu.S
    ${BOARD_FILES_DIR}/source/sys_selftest.c
    ${BOARD_FILES_DIR}/source/sys_startup.c
    ${BOARD_FILES_DIR}/source/system.c
    ${BOARD_FILES_DIR}/source/sys_vim.c
)

# FreeRTOS Kernel Files
ADD_LIBRARY(FREERTOS_KERNEL OBJECT
    ${FREERTOS_KERNEL_DIR}/croutine.c
    ${FREERTOS_KERNEL_DIR}/event_groups.c
    ${FREERTOS_KERNEL_DIR}/list.c
    ${FREERTOS_KERNEL_DIR}/queue.c
    ${FREERTOS_KERNEL_DIR}/stream_buffer.c
    ${FREERTOS_KERNEL_DIR}/tasks.c
    ${FREERTOS_KERNEL_DIR}/timers.c
    ${FREERTOS_KERNEL_DIR}/portable/Common/mpu_wrappers_v2.c
)

ADD_LIBRARY(FREERTOS_PORT OBJECT
    ${PORT_DIR}/mpu_wrappers_v2_asm.S
    ${PORT_DIR}/portASM.S
    ${PORT_DIR}/port.c
)

# On Mac the C_LINK flags by default adds "-Wl,-search_paths_first -Wl,-headerpad_max_install_names" which
# Causes the executable that gets built to strip the symbols, so force set it to empty here.
SET(CMAKE_C_LINK_FLAGS "")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-Map,\"RTOSDemo.map\" -Wl,-T\"${BOARD_FILES_DIR}/source/sys_link.ld\"")

# Debug
MESSAGE("Demo Sources: ${FREERTOS_DEMO_SOURCES}")
MESSAGE("FreeRTOS Sources: ${FREERTOS_KERNEL_SOURCES}")
MESSAGE("Port Sources: ${FREERTOS_PORT_SOURCES}")

#-------------------------------------------------------------------------------
# Add system tests
#-------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/tests)
