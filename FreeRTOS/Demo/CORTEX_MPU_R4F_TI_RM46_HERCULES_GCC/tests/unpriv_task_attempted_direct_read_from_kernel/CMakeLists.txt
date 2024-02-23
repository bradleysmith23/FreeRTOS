set(TEST_NAME unpriv_task_attempted_direct_read_from_kernel_data)

set(TEST_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/${TEST_NAME}.c
)

set(COMMON_TEST_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/../../source/test_utils.c
    ${CMAKE_CURRENT_LIST_DIR}/../../source/main.c
    ${CMAKE_CURRENT_LIST_DIR}/../../source/generic_fault_handler.c
    )

ADD_EXECUTABLE(RM46_Unpriv_Attempted_Direct_Read_Test.out
    ${FREERTOS_DEMO_SOURCES}
    ${TEST_SOURCES}
    ${COMMON_TEST_SOURCES}
)


SET_TARGET_PROPERTIES(RM46_Unpriv_Attempted_Direct_Read_Test.out PROPERTIES COMPILE_DEFINITIONS "mainDEMO_TYPE=0x40")


TARGET_LINK_LIBRARIES(RM46_Unpriv_Attempted_Direct_Read_Test.out   FREERTOS_PORT FREERTOS_KERNEL TI_BOARD_SUPPORT_PACKAGE )