# --- Build target options ---
option(DIGITALCURLING_PLUGINS_BUILD_PLAYER_IDENTICAL "Build player: identical" ON)
option(DIGITALCURLING_PLUGINS_BUILD_PLAYER_NORMAL_DIST "Build player: normal_dist" ON)

# --- Build settings ---
if(DEFINED DIGITALCURLING_PLUGIN_OUTPUT_DIR_ABS)
    set(DIGITALCURLING_PLUGIN_OUTPUT_DIR "${DIGITALCURLING_PLUGIN_OUTPUT_DIR_ABS}")
else()
    set(DIGITALCURLING_PLUGIN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
endif()

set(PLAYER_PLUGIN_TARGET_LIST "")
set(PLAYER_PLUGIN_OBJ_LIST "")
set(PLAYER_PLUGIN_TEST_SOURCES "")

macro(dc3_add_player_plugin _name)
    add_subdirectory("src/${_name}")

    list(APPEND PLAYER_PLUGIN_TARGET_LIST "digitalcurling_player_${_name}")
    list(APPEND PLAYER_PLUGIN_OBJ_LIST    "digitalcurling_player_${_name}_obj")
    list(APPEND PLAYER_PLUGIN_TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test/test_${_name}.cpp")
endmacro()

# --- Build plugins ---
if (DIGITALCURLING_PLUGINS_BUILD_PLAYER_IDENTICAL)
    dc3_add_player_plugin(identical)
endif()

if (DIGITALCURLING_PLUGINS_BUILD_PLAYER_NORMAL_DIST)
    dc3_add_player_plugin(normal_dist)
endif()

# --- Check targets ---
if (NOT PLAYER_PLUGIN_TARGET_LIST)
    message(WARNING "No player plugin targets specified. No player plugins will be built.")
    return()
endif()

string(JOIN ", " PLAYER_PLUGIN_TARGETS_STRING ${PLAYER_PLUGIN_TARGET_LIST})
message(STATUS "Building player plugins: ${PLAYER_PLUGIN_TARGETS_STRING}")

list(APPEND DIGITALCURLING_ACTIVE_PLUGIN_TARGETS ${PLAYER_PLUGIN_TARGET_LIST})
set(DIGITALCURLING_ACTIVE_PLUGIN_TARGETS ${DIGITALCURLING_ACTIVE_PLUGIN_TARGETS} PARENT_SCOPE)

set_target_properties(${PLAYER_PLUGIN_TARGET_LIST} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${DIGITALCURLING_PLUGIN_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${DIGITALCURLING_PLUGIN_OUTPUT_DIR}"
)


# --- Tests ---
if(DIGITALCURLING_BUILD_TEST AND TARGET digitalcurling_test)
    target_sources(digitalcurling_test PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/test/common.cpp"
        ${PLAYER_PLUGIN_TEST_SOURCES}
    )
    target_include_directories(digitalcurling_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test)
    target_link_libraries(digitalcurling_test PRIVATE ${PLAYER_PLUGIN_OBJ_LIST})
endif()


# --- Install rules ---
cmake_path(ABSOLUTE_PATH DIGITALCURLING_PLUGIN_OUTPUT_DIR
           BASE_DIRECTORY "${CMAKE_INSTALL_LIBDIR}/digitalcurling"
           OUTPUT_VARIABLE DIGITALCURLING_PLUGIN_INSTALL_DIR)

install(TARGETS ${PLAYER_PLUGIN_TARGET_LIST}
    EXPORT DigitalCurlingTargets
    LIBRARY DESTINATION "${DIGITALCURLING_PLUGIN_INSTALL_DIR}"
    RUNTIME DESTINATION "${DIGITALCURLING_PLUGIN_INSTALL_DIR}"
)
