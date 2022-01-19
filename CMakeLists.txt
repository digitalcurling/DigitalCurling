cmake_minimum_required(VERSION 3.19)

project(digital_curling
    VERSION 0.1.0
    LANGUAGES CXX
)

# use C++ 17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# symbol visiblity
set(CMAKE_CXX_VISIBLIRY_PRESET hidden)
set(CMAKE_VISIBLITY_INLINES_HIDDEN TRUE)

# main source
add_subdirectory(digital_curling)

# apps
option(DIGITAL_CURLING_BUILD_SERVER "Build digital curling server" ON)
if(DIGITAL_CURLING_BUILD_SERVER)
    add_subdirectory(server)
endif()

# tests
option(DIGITAL_CURLING_BUILD_TEST "Build digital curling test" ON)
if(DIGITAL_CURLING_BUILD_TEST)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
endif()

