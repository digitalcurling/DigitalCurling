# --- Doxygen setup ---
find_package(Doxygen)
if(NOT DOXYGEN_FOUND)
    message(WARNING "Doxygen not found, documentation will not be generated")
    return()
endif()

# --- doxygen-awesome.css ---
include(FetchContent)
FetchContent_Declare(
	doxygen-awesome-css
	URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/heads/main.zip
	SYSTEM
)
FetchContent_MakeAvailable(doxygen-awesome-css)

# --- Doxyfile configuration ---
set(DOXYGEN_PROJECT_NAME "DigitalCurling")
set(DOXYGEN_PROJECT_NUMBER ${PROJECT_VERSION})
set(DOXYGEN_PROJECT_BRIEF "A curling simulation system")
set(DOXYGEN_PROJECT_LOGO "logo.svg")

set(DOXYGEN_GENERATE_TREEVIEW YES)
set(DOXYGEN_DISABLE_INDEX NO)
set(DOXYGEN_FULL_SIDEBAR NO)
set(DOXYGEN_HTML_EXTRA_STYLESHEET "${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome.css")

set(DOXYGEN_RECURSIVE YES)
set(DOXYGEN_STRIP_FROM_PATH "${CMAKE_SOURCE_DIR}/src")
set(DOXYGEN_FULL_PATH_NAMES YES)
set(DOXYGEN_FILE_PATTERNS "*.h" "*.hpp" "*.dox")

set(DOXYGEN_EXCLUDE_PATTERNS "${CMAKE_SOURCE_DIR}/src/*/detail/*")
set(DOXYGEN_EXCLUDE_SYMBOLS "*::detail::*")

# --- Create documents ---
doxygen_add_docs(digitalcurling_docs
    "${CMAKE_SOURCE_DIR}/src/core/include"
    "${CMAKE_SOURCE_DIR}/src/plugin-api/include"
    "${CMAKE_SOURCE_DIR}/src/plugin-loader/include"
    "${CMAKE_SOURCE_DIR}/src/player/src"
    "${CMAKE_SOURCE_DIR}/src/simulator/src"
    mainpage.md
    players.md
    simulators.md
    coordinate.md
    json.md
)

# --- Copy static files ---
add_custom_command(TARGET digitalcurling_docs PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/logo.svg ${CMAKE_CURRENT_BINARY_DIR}
)
add_custom_command(TARGET digitalcurling_docs POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/fig ${CMAKE_CURRENT_BINARY_DIR}/html/fig
)
