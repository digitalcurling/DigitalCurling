# nlohmann_json
add_subdirectory(${PROJECT_SOURCE_DIR}/extern/nlohmann_json "${PROJECT_BINARY_DIR}/extern/nlohmann_json")

set(DIGITAL_CURLING_JSON_INCLUDE_FILES
    "include/digital_curling/json.hpp"
    "include/digital_curling/detail/json/polymorphic_conversion.hpp"
)

set(DIGITAL_CURLING_JSON_SOURCE_FILES
    "src/json.cpp"
)

add_library(digital_curling__json
    ${DIGITAL_CURLING_JSON_SOURCE_FILES}
    ${DIGITAL_CURLING_JSON_INCLUDE_FILES}
)
add_library(digital_curling::json ALIAS digital_curling__json)

target_include_directories(digital_curling__json
  PUBLIC
    include
)

target_link_libraries(digital_curling__json
  PUBLIC
    digital_curling::digital_curling
    nlohmann_json::nlohmann_json
)

set_target_properties(digital_curling__json PROPERTIES
    VERSION ${digital_curling_VERSION}
    SOVERSION ${digital_curling_VERSION_MAJOR}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${DIGITAL_CURLING_JSON_INCLUDE_FILES})
source_group("src" FILES ${DIGITAL_CURLING_JSON_SOURCE_FILES})
