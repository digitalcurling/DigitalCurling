# box2d
option(BOX2D_BUILD_UNIT_TESTS "Build the Box2D unit tests" OFF)
option(BOX2D_BUILD_TESTBED "Build the Box2D testbed" OFF)
add_subdirectory(${PROJECT_SOURCE_DIR}/extern/box2d "${PROJECT_BINARY_DIR}/extern/box2d")

# nlohmann_json
add_subdirectory(${PROJECT_SOURCE_DIR}/extern/nlohmann_json "${PROJECT_BINARY_DIR}/extern/nlohmann_json")

set(DIGITALCURLING3_INCLUDE_FILES
    "../include/digitalcurling3/detail/json/common.hpp"
    "../include/digitalcurling3/detail/json/polymorphic_json.hpp"
    "../include/digitalcurling3/detail/moves/concede.hpp"
    "../include/digitalcurling3/detail/moves/shot.hpp"
    "../include/digitalcurling3/detail/players/player_identical_factory.hpp"
    "../include/digitalcurling3/detail/players/player_normal_dist_factory.hpp"
    "../include/digitalcurling3/detail/simulators/simulator_fcv1_factory.hpp"
    "../include/digitalcurling3/detail/apply_move.hpp"
    "../include/digitalcurling3/detail/constants.hpp"
    "../include/digitalcurling3/detail/coordinate.hpp"
    "../include/digitalcurling3/detail/game_result.hpp"
    "../include/digitalcurling3/detail/game_setting.hpp"
    "../include/digitalcurling3/detail/game_state.hpp"
    "../include/digitalcurling3/detail/i_player_factory.hpp"
    "../include/digitalcurling3/detail/i_player_storage.hpp"
    "../include/digitalcurling3/detail/i_player.hpp"
    "../include/digitalcurling3/detail/i_simulator_factory.hpp"
    "../include/digitalcurling3/detail/i_simulator_storage.hpp"
    "../include/digitalcurling3/detail/i_simulator.hpp"
    "../include/digitalcurling3/detail/move.hpp"
    "../include/digitalcurling3/detail/team.hpp"
    "../include/digitalcurling3/detail/transform.hpp"
    "../include/digitalcurling3/detail/vector2.hpp"
    "../include/digitalcurling3/detail/version.hpp"
    "../include/digitalcurling3/digitalcurling3.hpp"
)

# for refering to this variable from doc/CMakeLists.txt
set(DIGITALCURLING3_INCLUDE_FILES
    ${DIGITALCURLING3_INCLUDE_FILES}
    PARENT_SCOPE
)

set(DIGITALCURLING3_SOURCE_FILES
    "apply_move.cpp"
    "box2d_util.hpp"
    "game_setting.cpp"
    "game_state.cpp"
    "i_player_factory.cpp"
    "i_player_storage.cpp"
    "i_simulator_factory.cpp"
    "i_simulator_storage.cpp"
    "move.cpp"
    "players_player_identical_factory.cpp"
    "players_player_identical_storage.cpp"
    "players_player_identical_storage.hpp"
    "players_player_identical.cpp"
    "players_player_identical.hpp"
    "players_player_normal_dist_factory.cpp"
    "players_player_normal_dist_storage.cpp"
    "players_player_normal_dist_storage.hpp"
    "players_player_normal_dist.cpp"
    "players_player_normal_dist.hpp"
    "simulators_simulator_fcv1_factory.cpp"
    "simulators_simulator_fcv1_storage.cpp"
    "simulators_simulator_fcv1_storage.hpp"
    "simulators_simulator_fcv1.cpp"
    "simulators_simulator_fcv1.hpp"
    "${CMAKE_CURRENT_BINARY_DIR}/version.cpp"  # generated from version.cpp.in
)

configure_file(version.cpp.in version.cpp @ONLY)

add_library(digitalcurling3__digitalcurling3 STATIC
    ${DIGITALCURLING3_SOURCE_FILES}
    ${DIGITALCURLING3_INCLUDE_FILES}
)
add_library(digitalcurling3::digitalcurling3 ALIAS digitalcurling3__digitalcurling3)

target_include_directories(digitalcurling3__digitalcurling3
  PUBLIC
    ../include
)

target_link_libraries(digitalcurling3__digitalcurling3
  PUBLIC
    nlohmann_json::nlohmann_json
  PRIVATE
    box2d
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/.." FILES ${DIGITALCURLING3_INCLUDE_FILES})
source_group("src" FILES ${DIGITALCURLING3_SOURCE_FILES})



