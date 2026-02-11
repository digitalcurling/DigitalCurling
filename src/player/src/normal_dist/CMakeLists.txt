# --- Build plugin object ---
add_library(digitalcurling_player_normal_dist_obj OBJECT
    "./player_normal_dist.cpp"
    "./player_normal_dist_factory.cpp"
    "./player_normal_dist_storage.cpp"
)
target_include_directories(digitalcurling_player_normal_dist_obj
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(digitalcurling_player_normal_dist_obj
    PUBLIC  digitalcurling::plugin_api
)

# --- Build plugin ---
add_library(digitalcurling_player_normal_dist ${DIGITALCURLING_PLUGIN_TYPE}
    $<TARGET_OBJECTS:digitalcurling_player_normal_dist_obj>
    "./normal_dist.cpp"
)
target_include_directories(digitalcurling_player_normal_dist
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(digitalcurling_player_normal_dist
    PUBLIC digitalcurling::plugin_api
)
