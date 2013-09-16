project(AssistentDrive)
cmake_minimum_required(VERSION 2.8.11)
message("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

add_definitions(-DLOCAL_SERVER_NAME="AssistentDriveSA")

add_subdirectory("single_app")
add_subdirectory("a-drive")
