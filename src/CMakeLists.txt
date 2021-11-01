# box2d
add_subdirectory(${PROJECT_SOURCE_DIR}/extern/box2d "${PROJECT_BINARY_DIR}/extern/box2d")

# nlohmann_json
add_subdirectory(${PROJECT_SOURCE_DIR}/extern/nlohmann_json "${PROJECT_BINARY_DIR}/extern/nlohmann_json")

set(DIGITAL_CURLING_INCLUDE_FILES
    "../include/digital_curling/digital_curling.hpp"
    "../include/digital_curling/detail/common.hpp"
    "../include/digital_curling/detail/coordinate.hpp"
    "../include/digital_curling/detail/version.hpp"
    "../include/digital_curling/detail/game/common.hpp"
    "../include/digital_curling/detail/game/normal_dist_shot_randomizer.hpp"
    "../include/digital_curling/detail/game/normal.hpp"
    "../include/digital_curling/detail/simulation/simulator.hpp"
    "../include/digital_curling/detail/simulation/fcv1.hpp"
)

set(DIGITAL_CURLING_GENERATED_INCLUDE_FILES
    "${CMAKE_CURRENT_BINARY_DIR}/digital_curling_export.h"  # generated from generate_export_header()
)

set(DIGITAL_CURLING_SOURCE_FILES
    "box2d_util.hpp"
    "common.cpp"
    "game_common.cpp"
    "game_normal_dist_shot_randomizer.cpp"
    "game_normal.cpp"
    "simulation_fcv1.hpp"
    "simulation_fcv1.cpp"
    "simulation_simulator.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/version.cpp"  # generated from version.cpp.in
)

configure_file(version.cpp.in version.cpp @ONLY)

add_library(digital_curling__digital_curling
    ${DIGITAL_CURLING_SOURCE_FILES}
    ${DIGITAL_CURLING_INCLUDE_FILES}
    ${DIGITAL_CURLING_GENERATED_INCLUDE_FILES}
)
add_library(digital_curling::digital_curling ALIAS digital_curling__digital_curling)

target_include_directories(digital_curling__digital_curling
  PUBLIC
    ../include
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(digital_curling__digital_curling
  PUBLIC
    nlohmann_json::nlohmann_json
  PRIVATE
    box2d
)

set_target_properties(digital_curling__digital_curling PROPERTIES
    VERSION ${digital_curling_VERSION}
    SOVERSION ${digital_curling_VERSION_MAJOR}
)

include(GenerateExportHeader)
generate_export_header(digital_curling__digital_curling BASE_NAME digital_curling)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../" FILES ${DIGITAL_CURLING_INCLUDE_FILES})
source_group("include" FILES ${DIGITAL_CURLING_GENERATED_INCLUDE_FILES})
source_group("src" FILES ${DIGITAL_CURLING_SOURCE_FILES})
