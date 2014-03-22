project(AnimatedSystemTrayIcon)
cmake_minimum_required(VERSION 2.8.11)
message("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets)

set(HEADERS
	AnimatedSystemTrayIcon.h
)

set(SOURCES
	AnimatedSystemTrayIcon.cpp
)

source_group(_h FILES ${HEADERS})
source_group(_cpp FILES ${SOURCES})

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

qt5_use_modules(${PROJECT_NAME}
	Widgets
)

IF(UNIX)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF(UNIX)

add_subdirectory("test")