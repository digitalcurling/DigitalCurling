# 参考: https://cliutils.gitlab.io/modern-cmake/chapters/testing/googletest.html

add_subdirectory("${PROJECT_SOURCE_DIR}/extern/googletest" "${PROJECT_BINARY_DIR}/extern/googletest")

mark_as_advanced(
    BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
    gmock_build_tests gtest_build_samples gtest_build_tests
    gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)

macro(digitalcurling3_add_test TESTNAME)
    # create an exectuable in which the tests will be stored
    add_executable(${TESTNAME} ${ARGN})
    # link the Google test infrastructure, mocking library, a default main fuction,
    # digital curling library, and json library to the test executable.
    target_link_libraries(${TESTNAME}
      PRIVATE
        gtest gmock gtest_main
        digitalcurling3::digitalcurling3 nlohmann_json::nlohmann_json
    )
    # gtest_discover_tests replaces gtest_add_tests,
    # see https://cmake.org/cmake/help/v3.10/module/GoogleTest.html for more options to pass to it
    gtest_discover_tests(${TESTNAME}
        # set a working directory so your project root so that you can find test data via paths relative to the project root
        WORKING_DIRECTORY ${PROJECT_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
    )
endmacro()

digitalcurling3_add_test(digitalcurling3__test
    common.cpp
    common.hpp
    test_coordinate.cpp
    test_json.cpp
    test_player.cpp
    test_simulator.cpp
)
