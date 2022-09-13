# digital_curling::digital_curling ###

# box2d
add_subdirectory(${PROJECT_SOURCE_DIR}/extern/box2d "${PROJECT_BINARY_DIR}/extern/box2d")

# nlohmann_json
add_subdirectory(${PROJECT_SOURCE_DIR}/extern/nlohmann_json "${PROJECT_BINARY_DIR}/extern/nlohmann_json")

set(DIGITAL_CURLING_INCLUDE_FILES
    "../include/digital_curling/detail/json/common.hpp"
    "../include/digital_curling/detail/json/polymorphic_json.hpp"
    "../include/digital_curling/detail/moves/concede.hpp"
    "../include/digital_curling/detail/moves/shot.hpp"
    "../include/digital_curling/detail/players/identical_player_factory.hpp"
    "../include/digital_curling/detail/players/normal_dist_player_factory.hpp"
    "../include/digital_curling/detail/simulators/fcv1_simulator_factory.hpp"
    "../include/digital_curling/detail/apply_move.hpp"
    "../include/digital_curling/detail/constants.hpp"
    "../include/digital_curling/detail/coordinate.hpp"
    "../include/digital_curling/detail/game_result.hpp"
    "../include/digital_curling/detail/game_setting.hpp"
    "../include/digital_curling/detail/game_state.hpp"
    "../include/digital_curling/detail/i_player_factory.hpp"
    "../include/digital_curling/detail/i_player_storage.hpp"
    "../include/digital_curling/detail/i_player.hpp"
    "../include/digital_curling/detail/i_simulator_factory.hpp"
    "../include/digital_curling/detail/i_simulator_storage.hpp"
    "../include/digital_curling/detail/i_simulator.hpp"
    "../include/digital_curling/detail/move.hpp"
    "../include/digital_curling/detail/team.hpp"
    "../include/digital_curling/detail/transform.hpp"
    "../include/digital_curling/detail/vector2.hpp"
    "../include/digital_curling/detail/version.hpp"
    "../include/digital_curling/digital_curling.hpp"
)

# for refering to this variable from doc/CMakeLists.txt
set(DIGITAL_CURLING_INCLUDE_FILES
    ${DIGITAL_CURLING_INCLUDE_FILES}
    PARENT_SCOPE
)

set(DIGITAL_CURLING_SOURCE_FILES
    "apply_move.cpp"
    "box2d_util.hpp"
    "game_setting.cpp"
    "game_state.cpp"
    "i_player_factory.cpp"
    "i_player_storage.cpp"
    "i_simulator_factory.cpp"
    "i_simulator_storage.cpp"
    "move.cpp"
    "players_identical_player_factory.cpp"
    "players_identical_player_storage.cpp"
    "players_identical_player_storage.hpp"
    "players_identical_player.cpp"
    "players_identical_player.hpp"
    "players_normal_dist_player_factory.cpp"
    "players_normal_dist_player_storage.cpp"
    "players_normal_dist_player_storage.hpp"
    "players_normal_dist_player.cpp"
    "players_normal_dist_player.hpp"
    "simulators_fcv1_simulator_factory.cpp"
    "simulators_fcv1_simulator_storage.cpp"
    "simulators_fcv1_simulator_storage.hpp"
    "simulators_fcv1_simulator.cpp"
    "simulators_fcv1_simulator.hpp"
    "${CMAKE_CURRENT_BINARY_DIR}/version.cpp"  # generated from version.cpp.in
)

configure_file(version.cpp.in version.cpp @ONLY)

add_library(digital_curling__digital_curling STATIC
    ${DIGITAL_CURLING_SOURCE_FILES}
    ${DIGITAL_CURLING_INCLUDE_FILES}
)
add_library(digital_curling::digital_curling ALIAS digital_curling__digital_curling)

target_include_directories(digital_curling__digital_curling
  PUBLIC
    ../include
)

target_link_libraries(digital_curling__digital_curling
  PUBLIC
    nlohmann_json::nlohmann_json
  PRIVATE
    box2d
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/.." FILES ${DIGITAL_CURLING_INCLUDE_FILES})
source_group("src" FILES ${DIGITAL_CURLING_SOURCE_FILES})



