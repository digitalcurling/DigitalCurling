# --- Header files ---
file(GLOB_RECURSE DIGITALCURLING_API_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

# --- Build Plugin API ---
add_library(digitalcurling_plugin_api INTERFACE)
add_library(digitalcurling::plugin_api ALIAS digitalcurling_plugin_api)

target_sources(digitalcurling_plugin_api INTERFACE
    FILE_SET HEADERS
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
        FILES ${DIGITALCURLING_API_HEADERS}
)
target_link_libraries(digitalcurling_plugin_api INTERFACE digitalcurling::core)


# --- Install rules ---
install(TARGETS digitalcurling_plugin_api
    EXPORT DigitalCurlingTargets
    FILE_SET HEADERS
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
