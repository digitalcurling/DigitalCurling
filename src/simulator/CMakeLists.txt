# --- Build target options ---
option(DIGITALCURLING_BUILD_SIMULATOR_FCV1 "Build simulator: fcv1" ON)

# --- Build settings ---
if(DEFINED DIGITALCURLING_PLUGIN_OUTPUT_DIR_ABS)
    set(DIGITALCURLING_PLUGIN_OUTPUT_DIR "${DIGITALCURLING_PLUGIN_OUTPUT_DIR_ABS}")
else()
    set(DIGITALCURLING_PLUGIN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
endif()

set(SIMULATOR_PLUGIN_TARGET_LIST "")
set(SIMULATOR_PLUGIN_OBJ_LIST "")
set(SIMULATOR_PLUGIN_TEST_SOURCES "")

macro(dc3_add_simulator_plugin _name)
    add_subdirectory("src/${_name}")

    list(APPEND SIMULATOR_PLUGIN_TARGET_LIST "digitalcurling_simulator_${_name}")
    list(APPEND SIMULATOR_PLUGIN_OBJ_LIST    "digitalcurling_simulator_${_name}_obj")
    list(APPEND SIMULATOR_PLUGIN_TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test/test_${_name}.cpp")
endmacro()

# --- Build plugins ---
if (DIGITALCURLING_BUILD_SIMULATOR_FCV1)
    dc3_add_simulator_plugin(fcv1)
endif()

# --- Check targets ---
if (NOT SIMULATOR_PLUGIN_TARGET_LIST)
    message(WARNING "No simulator plugin targets specified. No simulator plugins will be built.")
    return()
endif()

string(JOIN ", " SIMULATOR_PLUGIN_TARGETS_STRING ${SIMULATOR_PLUGIN_TARGET_LIST})
message(STATUS "Building simulator plugins: ${SIMULATOR_PLUGIN_TARGETS_STRING}")

list(APPEND DIGITALCURLING_ACTIVE_PLUGIN_TARGETS ${SIMULATOR_PLUGIN_TARGET_LIST})
set(DIGITALCURLING_ACTIVE_PLUGIN_TARGETS ${DIGITALCURLING_ACTIVE_PLUGIN_TARGETS} PARENT_SCOPE)

set_target_properties(${SIMULATOR_PLUGIN_TARGET_LIST} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${DIGITALCURLING_PLUGIN_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${DIGITALCURLING_PLUGIN_OUTPUT_DIR}"
)


# --- Tests ---
if(DIGITALCURLING_BUILD_TEST AND TARGET digitalcurling_test)
    target_sources(digitalcurling_test PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/test/common.cpp"
        ${SIMULATOR_PLUGIN_TEST_SOURCES}
    )
    target_include_directories(digitalcurling_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test)
    target_link_libraries(digitalcurling_test PRIVATE ${SIMULATOR_PLUGIN_OBJ_LIST})
endif()


# --- Install rules ---
cmake_path(ABSOLUTE_PATH DIGITALCURLING_PLUGIN_OUTPUT_DIR
           BASE_DIRECTORY "${CMAKE_INSTALL_LIBDIR}/digitalcurling"
           OUTPUT_VARIABLE DIGITALCURLING_PLUGIN_INSTALL_DIR)

install(TARGETS ${SIMULATOR_PLUGIN_TARGET_LIST}
    EXPORT DigitalCurlingTargets
    LIBRARY DESTINATION "${DIGITALCURLING_PLUGIN_INSTALL_DIR}"
    RUNTIME DESTINATION "${DIGITALCURLING_PLUGIN_INSTALL_DIR}"
)
