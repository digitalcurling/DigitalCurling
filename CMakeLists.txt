cmake_minimum_required(VERSION 3.19)

project(digitalcurling3
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
add_subdirectory(src) # digitalcurling3::digitalcurling3

# tests
option(DIGITALCURLING3_BUILD_TEST "Build digital curling test" OFF)
if(DIGITALCURLING3_BUILD_TEST)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(test)
endif()

# documents
option(DIGITALCURLING3_BUILD_DOC "Build digital curling document" OFF)
if(DIGITALCURLING3_BUILD_DOC)
    add_subdirectory(doc)
endif()
