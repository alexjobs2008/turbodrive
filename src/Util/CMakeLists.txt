project(Util)
cmake_minimum_required(VERSION 2.8.11)

add_definitions(-DUNICODE)

set(CMAKE_INCLUDE_CURRENT_DIR on)
set(CMAKE_AUTOMOC on)

file(GLOB_RECURSE HEADERS *.h)
file(GLOB_RECURSE SOURCES *.cpp)

if(APPLE)
    file(GLOB_RECURSE OBJECTIVE_SOURCES *.mm)
endif()

source_group(_h FILES ${HEADERS})
source_group(_cpp FILES ${SOURCES})

if(APPLE)
    source_group(_mm FILES ${OBJECTIVE_SOURCES})
endif()

find_package(Qt5Core)

if(APPLE)
    add_library(${PROJECT_NAME} ${SOURCES} ${OBJECTIVE_SOURCES} ${HEADERS})
else()
    add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
endif()
qt5_use_modules(${PROJECT_NAME} Core)

target_link_libraries(${PROJECT_NAME} Events)
if(APPLE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} "-framework Cocoa")
endif()
