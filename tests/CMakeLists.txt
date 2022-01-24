# 参考: https://cliutils.gitlab.io/modern-cmake/chapters/testing/googletest.html

add_subdirectory("${PROJECT_SOURCE_DIR}/extern/googletest" "${PROJECT_BINARY_DIR}/extern/googletest")

mark_as_advanced(
    BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
    gmock_build_tests gtest_build_samples gtest_build_tests
    gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)

macro(digital_curling_add_test TESTNAME)
    # create an exectuable in which the tests will be stored
    add_executable(${TESTNAME} ${ARGN})
    # link the Google test infrastructure, mocking library, and a default main fuction to
    # the test executable.  Remove g_test_main if writing your own main function.
    target_link_libraries(${TESTNAME} gtest gmock gtest_main)
    # gtest_discover_tests replaces gtest_add_tests,
    # see https://cmake.org/cmake/help/v3.10/module/GoogleTest.html for more options to pass to it
    gtest_discover_tests(${TESTNAME}
        # set a working directory so your project root so that you can find test data via paths relative to the project root
        WORKING_DIRECTORY ${PROJECT_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
    )
endmacro()

digital_curling_add_test(digital_curling__test
    "test/util.hpp"
    "test/util.cpp"
    "test/test_simulator.cpp"
    "test/test_coordinate.cpp"
)

target_link_libraries(digital_curling__test
    digital_curling::digital_curling
    digital_curling::json
)

# TODO テストのカレントディレクトリを設定する
