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

set(QRC_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/res/res.qrc)

if(WIN32)
    set(RC_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/res/res.rc)
endif()

qt5_add_resources(MOCED_RESOURCES ${QRC_RESOURCES})

source_group(_h FILES ${HEADERS})
source_group(_cpp FILES ${SOURCES})
source_group(_res FILES ${QRC_RESOURCES} ${RC_RESOURCES})

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
    ${MOCED_RESOURCES}
    ${RC_RESOURCES}
)

qt5_use_modules(${PROJECT_NAME}
    Core
    Widgets
)

target_link_libraries(${PROJECT_NAME}
    SingleApp
    AnimatedSystemTrayIcon
    Settings
    SettingsUI
    Network
    LoginUI
    QsLog
)