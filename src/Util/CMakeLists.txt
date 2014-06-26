project(Util)
cmake_minimum_required(VERSION 2.8.11)

add_definitions(-DUNICODE)

set(CMAKE_INCLUDE_CURRENT_DIR on)
set(CMAKE_AUTOMOC on)

file(GLOB_RECURSE HEADERS *.h)
file(GLOB_RECURSE SOURCES *.cpp)

source_group(_h FILES ${HEADERS})
source_group(_cpp FILES ${SOURCES})

find_package(Qt5Core)
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
qt5_use_modules(${PROJECT_NAME} Core)

target_link_libraries(${PROJECT_NAME} Events)
