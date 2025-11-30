cmake_minimum_required(VERSION 3.26)

project(digitalcurling3
    VERSION 1.0.0
    LANGUAGES CXX
)

# --- Build options ---
option(DIGITALCURLING3_BUILD_TEST "Build tests for DigitalCurling system" ${PROJECT_IS_TOP_LEVEL})
option(DIGITALCURLING3_BUILD_DOCS "Build documents for DigitalCurling system" OFF)

# --- Build settings ---
set(BUILD_SHARED_LIBS OFF)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/utf-8>)

if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()
if(NOT DEFINED CMAKE_CXX_STANDARD_REQUIRED)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

function(digitalcurling3_apply_standard_settings target_name)
    set_target_properties(${target_name} PROPERTIES
        CXX_EXTENSIONS OFF
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN TRUE
    )
endfunction()

# --- Test preparations ---
if(DIGITALCURLING3_BUILD_TEST)
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

    add_executable(digitalcurling3_test)
    target_link_libraries(digitalcurling3_test PRIVATE GTest::gtest_main GTest::gmock)
endif()


# --- Build libraries ---
add_subdirectory(src/core)


# --- Build tests ---
if(DIGITALCURLING3_BUILD_TEST)
    include(GoogleTest)
    gtest_discover_tests(digitalcurling3_test)
endif()

# --- Create documents ---
if(DIGITALCURLING3_BUILD_DOCS)
    add_subdirectory(docs)
endif()
