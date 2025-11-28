# nlohmann_json
add_subdirectory(${PROJECT_SOURCE_DIR}/extern/nlohmann_json "${PROJECT_BINARY_DIR}/extern/nlohmann_json")

set(DIGITALCURLING3_INCLUDE_FILES
    "include/digitalcurling3/detail/moves/concede.hpp"
    "include/digitalcurling3/detail/moves/shot.hpp"
    "include/digitalcurling3/detail/common.hpp"
    "include/digitalcurling3/detail/constants.hpp"
    "include/digitalcurling3/detail/coordinate.hpp"
    "include/digitalcurling3/detail/game_result.hpp"
    "include/digitalcurling3/detail/game_setting.hpp"
    "include/digitalcurling3/detail/game_state.hpp"
    "include/digitalcurling3/detail/i_player_factory.hpp"
    "include/digitalcurling3/detail/i_player_storage.hpp"
    "include/digitalcurling3/detail/i_player.hpp"
    "include/digitalcurling3/detail/i_simulator_factory.hpp"
    "include/digitalcurling3/detail/i_simulator_storage.hpp"
    "include/digitalcurling3/detail/i_simulator.hpp"
    "include/digitalcurling3/detail/move.hpp"
    "include/digitalcurling3/detail/team.hpp"
    "include/digitalcurling3/detail/transform.hpp"
    "include/digitalcurling3/detail/vector2.hpp"
    "include/digitalcurling3/detail/version.hpp"
    "include/digitalcurling3/digitalcurling3.hpp"
)

# for refering to this variable from doc/CMakeLists.txt
set(DIGITALCURLING3_INCLUDE_FILES
    ${DIGITALCURLING3_INCLUDE_FILES}
    PARENT_SCOPE
)

set(DIGITALCURLING3_VERSION_INFO_HPP "include/digitalcurling3/detail/version_info.hpp")
set(DIGITALCURLING3_VERSION_INFO_HPP_OUT "${CMAKE_CURRENT_BINARY_DIR}/${DIGITALCURLING3_VERSION_INFO_HPP}")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/${DIGITALCURLING3_VERSION_INFO_HPP}.in"
    "${DIGITALCURLING3_VERSION_INFO_HPP_OUT}"
    @ONLY
)

add_library(digitalcurling3_core INTERFACE
    ${DIGITALCURLING3_INCLUDE_FILES}
    ${DIGITALCURLING3_VERSION_INFO_HPP_OUT}
)
add_library(digitalcurling3::core ALIAS digitalcurling3_core)
add_library(digitalcurling3:: digitalcurlingALIAS digitalcurling3_core)

target_include_directories(digitalcurling3_core INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}/include"
)
target_link_libraries(digitalcurling3_core INTERFACE
    nlohmann_json::nlohmann_json
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${DIGITALCURLING3_INCLUDE_FILES})
source_group("Generated Files" FILES ${DIGITALCURLING3_VERSION_INFO_HPP_OUT})
