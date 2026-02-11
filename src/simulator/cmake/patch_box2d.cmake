set(TARGET_FILE "CMakeLists.txt")

file(READ ${TARGET_FILE} ROOT_CONTENT)
string(REGEX REPLACE "cmake_minimum_required[ \t]*\\(VERSION[ \t]+3\\.[0-9]+\\)"
                     "cmake_minimum_required(VERSION 3.15)"
                      ROOT_MODIFIED "${ROOT_CONTENT}")

if(ROOT_MODIFIED STREQUAL ROOT_CONTENT)
    message(WARNING "Box2D Root CMakeLists.txt: Patch pattern not found!")
else()
    file(WRITE ${TARGET_FILE} "${ROOT_MODIFIED}")
endif()
