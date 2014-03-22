project(Events)
cmake_minimum_required(VERSION 2.8.11)
message("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt5Network)

set(HEADERS
	LocalFileEvent.h
	LocalFileEventNotifier.h
	EventHandlerBase.h
	RemoteEventHandlers.h
	LocalEventHandlers.h
	FileEventDispatcher.h
	Syncer.h
	Cache.h
)

set(SOURCES
	LocalFileEvent.cpp
	LocalFileEventNotifier.cpp
	RemoteEventHandlers.cpp
	LocalEventHandlers.cpp
	FileEventDispatcher.cpp
	Syncer.cpp
	Cache.cpp
)

qt5_add_resources(MOCED_RESOURCES ${QRC_RESOURCES})

source_group(_h FILES ${HEADERS})
source_group(_cpp FILES ${SOURCES})
source_group(_res FILES ${QRC_RESOURCES})

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${MOCED_RESOURCES})

target_link_libraries(${PROJECT_NAME}
	Settings
	efsw
)

qt5_use_modules(${PROJECT_NAME}
	Network
)

IF(UNIX)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF(UNIX)