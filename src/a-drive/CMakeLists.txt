project(aDrive)
cmake_minimum_required(VERSION 2.8.11)
message("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets)

set(HEADERS
    mainwindow.h
)

set(SOURCES
    main.cpp
    mainwindow.cpp
)

source_group(h FILES ${HEADERS})
source_group(cpp FILES ${SOURCES})

if(WIN32)
    set(GUI_TYPE WIN32)
endif(WIN32)

if(APPLE)
    set(GUI_TYPE MACOSX_BUNDLE)
endif(APPLE)

add_executable(${PROJECT_NAME}
    ${GUI_TYPE}
    ${HEADERS}
    ${SOURCES}
)

qt5_use_modules(${PROJECT_NAME}
    Core
    Widgets
)