project(COM)
cmake_minimum_required(VERSION 2.8.11)

add_definitions(-DUNICODE)

set(CMAKE_INCLUDE_CURRENT_DIR on)
set(CMAKE_AUTOMOC on)

file(GLOB_RECURSE HEADERS *.h)
file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE QRC_RESOURCES *.qrc)
file(GLOB_RECURSE RC_RESOURCES *.rc)

source_group(_h FILES ${HEADERS})
source_group(_cpp FILES ${SOURCES})
source_group(_res FILES ${QRC_RESOURCES} ${RC_RESOURCES})

# find_package(Qt5Core)
# find_package(Qt5Widgets)
# find_package(Qt5Designer)
# find_package(Qt5AxBase)
# find_package(Qt5AxContainer)
find_package(Qt5AxServer)

qt5_add_resources(MOCED_RESOURCES ${QRC_RESOURCES})

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS} ${MOCED_RESOURCES}
        ${RC_RESOURCES} "libCOM.def")

#add_executable(${PROJECT_NAME}
#        WIN32
#        ${HEADERS}
#        ${SOURCES}
#        ${MOCED_RESOURCES}
#        ${RC_RESOURCES}
#)


# qt5_use_modules(${PROJECT_NAME} Core Widgets Designer AxBase AxContainer AxServer)
qt5_use_modules(${PROJECT_NAME} AxServer)

# target_link_libraries(${PROJECT_NAME} Events)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
