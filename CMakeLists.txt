cmake_minimum_required(VERSION 3.26)

project(digitalcurling
    VERSION 3.0.0
    LANGUAGES CXX
)

# --- Build options ---
option(DIGITALCURLING_BUILD_PLAYERS "Build player plugins for DigitalCurling system" ON)
set(DIGITALCURLING_PLUGIN_OUTPUT_DIR "plugins" CACHE STRING "Output directory for plugins relative to build dir")

option(DIGITALCURLING_BUILD_TEST "Build tests for DigitalCurling system" ${PROJECT_IS_TOP_LEVEL})
option(DIGITALCURLING_BUILD_DOCS "Build documents for DigitalCurling system" OFF)

# --- Build settings ---
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/utf-8>)

if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()
if(NOT DEFINED CMAKE_CXX_STANDARD_REQUIRED)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

set(DIGITALCURLING_ACTIVE_PLUGIN_TARGETS "")
set(DIGITALCURLING_PLUGIN_TYPE MODULE)

if(DIGITALCURLING_BUILD_PLAYERS)
    cmake_path(ABSOLUTE_PATH DIGITALCURLING_PLUGIN_OUTPUT_DIR
            BASE_DIRECTORY "${CMAKE_BINARY_DIR}"
            OUTPUT_VARIABLE DIGITALCURLING_PLUGIN_OUTPUT_DIR_ABS)
    message(STATUS "Build plugins module to ${DIGITALCURLING_PLUGIN_OUTPUT_DIR_ABS}")
endif()

function(digitalcurling_apply_standard_settings target_name)
    set_target_properties(${target_name} PROPERTIES
        CXX_EXTENSIONS OFF
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN TRUE
    )
endfunction()

# --- Test preparations ---
if(DIGITALCURLING_BUILD_TEST)
    enable_testing()
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.16.0
        SYSTEM
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    mark_as_advanced(
        BUILD_GMOCK BUILD_GTEST
        gmock_build_tests gtest_build_samples gtest_build_tests
        gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
    )

    add_executable(digitalcurling_test)
    target_link_libraries(digitalcurling_test PRIVATE GTest::gtest_main GTest::gmock)
endif()


# --- Build libraries ---
add_subdirectory(src/core)
add_subdirectory(src/plugin-api)

if(DIGITALCURLING_BUILD_PLAYERS)
    add_subdirectory(src/player)
endif()


# --- Build tests ---
if(DIGITALCURLING_BUILD_TEST)
    include(GoogleTest)
    gtest_discover_tests(digitalcurling_test)
endif()

# --- Create documents ---
if(DIGITALCURLING_BUILD_DOCS)
    add_subdirectory(docs)
endif()


# --- Install rules ---
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(DIGITALCURLING_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/DigitalCurling")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/DigitalCurlingConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/DigitalCurlingConfig.cmake"
    INSTALL_DESTINATION "${DIGITALCURLING_CMAKE_DIR}"
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/DigitalCurlingConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/DigitalCurlingConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/DigitalCurlingConfigVersion.cmake"
    DESTINATION "${DIGITALCURLING_CMAKE_DIR}"
)
install(EXPORT DigitalCurlingTargets
    FILE DigitalCurlingTargets.cmake
    NAMESPACE digitalcurling::
    DESTINATION "${DIGITALCURLING_CMAKE_DIR}"
)
