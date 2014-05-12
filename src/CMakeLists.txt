project(AssistentDrive)
cmake_minimum_required(VERSION 2.8.11)
message("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

INCLUDE_DIRECTORIES(".")
INCLUDE_DIRECTORIES("./3rdParty")

# Check if environment variable QTDIR is set.
# Examples:
# 1. Windows: QTDIR = C:\dev\qt511\5.1.1\msvc2012
# 2. MacOS X: QTDIR = ~/Qt5.1.1/5.1.1/clang_64
if (DEFINED ENV{QTDIR})
	set(CMAKE_PREFIX_PATH $ENV{QTDIR} ${CMAKE_PREFIX_PATH})
else ()
	message (FATAL_ERROR "ERROR: Environment variable QTDIR is not set.")
endif ()

if (${BUILD_SYSTEM} STREQUAL "mingw")
	set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")
endif()

# Force asserts in debug and release modes
add_definitions(-DQT_FORCE_ASSERTS)

# Show filename and line number in logs
add_definitions(-DQS_LOG_LINE_NUMBERS)

# 3rd-party libs
add_subdirectory("3rdParty")

# QSystemTrayIcon that supports states & animations
add_subdirectory("AnimatedSystemTrayIcon")

# Single app lib and its local server name definition
add_definitions(-DLOCAL_SERVER_NAME="AssistentDriveSA")

add_subdirectory("SingleApp")

add_subdirectory("Util")
add_subdirectory("UtilUI")

add_subdirectory("rtCSS")

add_subdirectory("Settings")
add_subdirectory("SettingsUI")
add_subdirectory("LoginUI")
add_subdirectory("Network")
add_subdirectory("APIClient")
add_subdirectory("Events")

# The desktop app
add_subdirectory("Application")
