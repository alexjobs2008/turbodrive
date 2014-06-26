project(Events)
cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

file(GLOB_RECURSE HEADERS *.h)
file(GLOB_RECURSE SOURCES *.cpp)

source_group(_h FILES ${HEADERS})
source_group(_cpp FILES ${SOURCES})

find_package(Qt5Network)
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${MOCED_RESOURCES})
qt5_use_modules(${PROJECT_NAME} Network)

target_link_libraries(${PROJECT_NAME}
	Settings
	efsw
)
