project(single_app)
cmake_minimum_required(VERSION 2.8.11)
message("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt5Network)

set(HEADERS
    mainwindow.h
)

set(SOURCES
    main.cpp
    mainwindow.cpp
)

source_group(h FILES ${HEADERS})
source_group(cpp FILES ${SOURCES})






find_package(Qt4 COMPONENTS QtCore QtGui QtNetwork REQUIRED)

set(CMAKE_AUTOMOC TRUE)

include(${QT_USE_FILE})

set(HEADERS
    src/qtlocalpeer.h 
    src/qtsingleapplication.h 
    src/qtsinglecoreapplication.h 
)

include_directories(${CMAKE_CURRENT_BINARY_DIR} src)

set(SRCS
    src/qtlocalpeer.cpp 
    src/qtsingleapplication.cpp 
    src/qtsinglecoreapplication.cpp 
)

add_library(${PROJECT_NAME} ${SRCS} ${HEADERS})

target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES})

IF(UNIX)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF(UNIX) 