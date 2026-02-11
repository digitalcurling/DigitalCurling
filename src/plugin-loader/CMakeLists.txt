# --- UUIDv7 Library ---
include(FetchContent)
FetchContent_Declare(
    uuidv7
    GIT_REPOSITORY https://github.com/chromeru0312/cpp-uuidv7.git
    GIT_TAG        v1.0.0
    SYSTEM
)
FetchContent_MakeAvailable(uuidv7)

# --- Header files ---
file(GLOB_RECURSE DIGITALCURLING_LOADER_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

# --- Build Plugin Loader ---
if (DIGITALCURLING_PLUGIN_LOADER_SHARED)
    set(DIGITALCURLING_LOADER_TYPE SHARED)
else()
    set(DIGITALCURLING_LOADER_TYPE STATIC)
endif()

add_library(digitalcurling_plugin_loader ${DIGITALCURLING_LOADER_TYPE}
    "./src/plugins/loader.cpp"
    "./src/plugins/native_loader.cpp"
    "./src/plugins/plugin_manager.cpp"
    "./src/plugins/plugin_resource.cpp"
    "./src/players/plugin_player.cpp"
    "./src/simulators/plugin_simulator.cpp"
)
add_library(digitalcurling::plugin_loader ALIAS digitalcurling_plugin_loader)

include(GenerateExportHeader)
set(DIGITALCURLING_LOADER_EXPORT_HEADER
    "${CMAKE_CURRENT_BINARY_DIR}/include/digitalcurling_plugin_loader_export.h"
)
generate_export_header(digitalcurling_plugin_loader
    BASE_NAME DIGITALCURLING_PLUGIN_LOADER
    EXPORT_MACRO_NAME DIGITALCURLING_LOADER_API
    EXPORT_FILE_NAME ${DIGITALCURLING_LOADER_EXPORT_HEADER}
)

target_sources(digitalcurling_plugin_loader PUBLIC
    FILE_SET HEADERS
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
        FILES ${DIGITALCURLING_LOADER_HEADERS}

    FILE_SET generated_headers
        TYPE HEADERS
        BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/include"
        FILES ${DIGITALCURLING_LOADER_EXPORT_HEADER}
)
target_link_libraries(digitalcurling_plugin_loader
    PUBLIC  digitalcurling::plugin_api uuidv7::uuidv7
)
target_compile_definitions(digitalcurling_plugin_loader
    PRIVATE "DIGITALCURLING_LIBRARY_EXTENSION=\"${CMAKE_SHARED_LIBRARY_SUFFIX}\""
)

# --- Plugin Bundling Logic ---
if(DIGITALCURLING_BUNDLE_PLUGINS AND DIGITALCURLING_ACTIVE_PLUGIN_TARGETS)
    message(STATUS "Loader: Bundling plugins -> ${DIGITALCURLING_ACTIVE_PLUGIN_TARGETS}")

    target_link_libraries(digitalcurling_plugin_loader PRIVATE
        $<LINK_LIBRARY:WHOLE_ARCHIVE,${DIGITALCURLING_ACTIVE_PLUGIN_TARGETS}>
    )
endif()


# --- Tests ---
if(DIGITALCURLING_BUILD_TEST AND TARGET digitalcurling_test)
    if (DIGITALCURLING_BUNDLE_PLUGINS)
        target_sources(digitalcurling_test PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/test/test_static_loader.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/test/test_static_manager.cpp"
        )
    else()
        target_sources(digitalcurling_test PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/test/dynamic_test_base.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/test/test_dynamic_loader.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/test/test_dynamic_manager.cpp"
        )
    endif()

    target_link_libraries(digitalcurling_test PRIVATE digitalcurling::plugin_loader)
    target_include_directories(digitalcurling_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test)
    target_compile_definitions(digitalcurling_test
        PRIVATE "DIGITALCURLING_LIBRARY_EXTENSION=\"${CMAKE_SHARED_LIBRARY_SUFFIX}\""
    )
endif()


# --- Install rules ---
install(TARGETS digitalcurling_plugin_loader
    EXPORT DigitalCurlingTargets

    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}

    FILE_SET HEADERS
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILE_SET generated_headers
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
