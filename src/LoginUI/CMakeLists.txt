project(LoginUI)
cmake_minimum_required(VERSION 2.8.11)
message("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets)

set(HEADERS
    LoginController.h
    LoginWidget.h
)

set(SOURCES
    LoginController.cpp
    LoginWidget.cpp
)

set(QRC_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/res/loginUI.qrc)

qt5_add_resources(MOCED_RESOURCES ${QRC_RESOURCES})

source_group(_h FILES ${HEADERS})
source_group(_cpp FILES ${SOURCES})
source_group(_res FILES ${QRC_RESOURCES})

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${MOCED_RESOURCES})

target_link_libraries(${PROJECT_NAME}
    Settings
)

qt5_use_modules(${PROJECT_NAME}
    Widgets
)

IF(UNIX)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF(UNIX)