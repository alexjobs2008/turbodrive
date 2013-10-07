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

# 3rd-party libs
add_subdirectory("3rdParty")

# QSystemTrayIcon that supports states & animations
add_subdirectory("AnimatedSystemTrayIcon")


# Single app lib and its local server name definition
add_definitions(-DLOCAL_SERVER_NAME="AssistentDriveSA")
add_subdirectory("SingleApp")

add_subdirectory("Util")
add_subdirectory("UtilUI")
add_subdirectory("Settings")
add_subdirectory("SettingsUI")
add_subdirectory("LoginUI")
add_subdirectory("Network")

# The desktop app
add_subdirectory("aDrive")
