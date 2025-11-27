cmake_minimum_required(VERSION 3.19)

project(digitalcurling3
    VERSION 1.0.0
    LANGUAGES CXX
)

# use C++ 17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# symbol visiblity
set(CMAKE_CXX_VISIBLIRY_PRESET hidden)
set(CMAKE_VISIBLITY_INLINES_HIDDEN TRUE)

# tests (prepare)
option(DIGITALCURLING3_BUILD_TEST "Build digital curling test" OFF)
if(DIGITALCURLING3_BUILD_TEST)
    enable_testing()
    add_subdirectory("${PROJECT_SOURCE_DIR}/extern/googletest" "${PROJECT_BINARY_DIR}/extern/googletest")

    mark_as_advanced(
        BUILD_GMOCK BUILD_GTEST
        gmock_build_tests gtest_build_samples gtest_build_tests
        gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
    )

    add_executable(digitalcurling3_test)
    target_link_libraries(digitalcurling3_test PRIVATE GTest::gtest_main GTest::gmock)
endif()

# main source
add_subdirectory(src/core)

# tests
if(DIGITALCURLING3_BUILD_TEST)
    include(GoogleTest)
    gtest_discover_tests(digitalcurling3_test)
endif()

# documents
option(DIGITALCURLING3_BUILD_DOCS "Build digital curling document" OFF)
if(DIGITALCURLING3_BUILD_DOCS)
    add_subdirectory(docs)
endif()
