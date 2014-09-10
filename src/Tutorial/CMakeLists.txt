project(Tutorial)
cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

file(GLOB_RECURSE HEADERS *.h)
file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE UI_FILES *.ui)
file(GLOB_RECURSE QRC_RESOURCES *.qrc)

find_package(Qt5Widgets)
qt5_wrap_ui(UI_HDRS ${UI_FILES})
qt5_add_resources(MOCED_RESOURCES ${QRC_RESOURCES})

source_group(_h FILES ${HEADERS})
source_group(_cpp FILES ${SOURCES})
source_group(_res FILES ${QRC_RESOURCES})

add_library(${PROJECT_NAME} ${UI_HDRS} ${SOURCES} ${HEADERS} ${MOCED_RESOURCES})
qt5_use_modules(${PROJECT_NAME} Widgets)
