project(APIClient)
cmake_minimum_required(VERSION 2.8.11)
message("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt5Core)

set(HEADERS
	ApiTypes.h
	AuthenticationService.h
	DashboardService.h
	ProfileService.h
	FileUploader.h
	FileDownloader.h
	FilesService.h
	NotificationService.h
	chunkuploader.h
)

set(SOURCES
	ApiTypes.cpp
	AuthenticationService.cpp
	DashboardService.cpp
	ProfileService.cpp
	FileUploader.cpp
	FileDownloader.cpp
	FilesService.cpp
	NotificationService.cpp
	chunkuploader.cpp
)

source_group(_h FILES ${HEADERS})
source_group(_cpp FILES ${SOURCES})
source_group(_res FILES ${QRC_RESOURCES})

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${MOCED_RESOURCES})

qt5_use_modules(${PROJECT_NAME}
	Core
)

target_link_libraries(${PROJECT_NAME}
	Settings
	Network
	QsLog
	Util
)

if(UNIX)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS -fPIC)
endif(UNIX)
