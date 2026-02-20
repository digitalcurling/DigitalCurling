# --- Build plugin object ---
add_library(digitalcurling_player_identical_obj OBJECT
    "./player_identical.cpp"
    "./player_identical_factory.cpp"
    "./player_identical_storage.cpp"
)
target_include_directories(digitalcurling_player_identical_obj
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(digitalcurling_player_identical_obj
    PUBLIC  digitalcurling::plugin_api
)

# --- Build plugin ---
add_library(digitalcurling_player_identical ${DIGITALCURLING_PLUGIN_TYPE}
    $<TARGET_OBJECTS:digitalcurling_player_identical_obj>
    "./identical.cpp"
)
target_include_directories(digitalcurling_player_identical
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(digitalcurling_player_identical
    PUBLIC digitalcurling::plugin_api
)
