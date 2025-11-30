# --- nlohmann_json ---
include(FetchContent)
set(DIGITALCURLING3_JSONLIB_VERSION "3.12.0")
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v${DIGITALCURLING3_JSONLIB_VERSION}
    SYSTEM
    FIND_PACKAGE_ARGS ${DIGITALCURLING3_JSONLIB_VERSION} QUIET
)
FetchContent_MakeAvailable(nlohmann_json)

# --- Header files ---
file(GLOB_RECURSE DIGITALCURLING3_INCLUDE_FILES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

set(DIGITALCURLING3_VERSION_INFO_HPP "include/digitalcurling3/detail/version_info.hpp")
set(DIGITALCURLING3_VERSION_INFO_HPP_OUT "${CMAKE_CURRENT_BINARY_DIR}/${DIGITALCURLING3_VERSION_INFO_HPP}")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/${DIGITALCURLING3_VERSION_INFO_HPP}.in"
    "${DIGITALCURLING3_VERSION_INFO_HPP_OUT}"
    @ONLY
)

# --- Build library ---
add_library(digitalcurling3_core INTERFACE)
add_library(digitalcurling3::core ALIAS digitalcurling3_core)

target_sources(digitalcurling3_core INTERFACE
    FILE_SET HEADERS
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
        FILES ${DIGITALCURLING3_INCLUDE_FILES}

    FILE_SET generated_headers
        TYPE HEADERS
        BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/include"
        FILES "${DIGITALCURLING3_VERSION_INFO_HPP_OUT}"
)
target_link_libraries(digitalcurling3_core INTERFACE
    nlohmann_json::nlohmann_json
)
target_compile_features(digitalcurling3_core INTERFACE cxx_std_17)
digitalcurling3_apply_standard_settings(digitalcurling3_core)


# --- Tests ---
if(DIGITALCURLING3_BUILD_TEST AND TARGET digitalcurling3_test)
    target_sources(digitalcurling3_test PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/test/test_coordinate.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/test/test_json.cpp"
    )
    target_link_libraries(digitalcurling3_test PRIVATE digitalcurling3::core)
    target_include_directories(digitalcurling3_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test)
endif()
