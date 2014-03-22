project(Util)
cmake_minimum_required(VERSION 2.8.11)
message("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

add_definitions(-DUNICODE)

find_package(Qt5Core)

set(HEADERS
	AppStrings.h
	FileUtils.h
)

set(SOURCES
	AppStrings.cpp
	FileUtils.cpp
)


source_group(_h FILES ${HEADERS})
source_group(_cpp FILES ${SOURCES})
source_group(_res FILES ${QRC_RESOURCES})

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${MOCED_RESOURCES})

qt5_use_modules(${PROJECT_NAME}
	Core
)

target_link_libraries(${PROJECT_NAME}
	Events
)

if(UNIX)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS -fPIC)
endif(UNIX)

#set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "libs")