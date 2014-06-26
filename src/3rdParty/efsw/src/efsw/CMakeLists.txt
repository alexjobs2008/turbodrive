project(efsw)
cmake_minimum_required(VERSION 2.8.11)

add_definitions(-DEFSW_EXPORTS)

include_directories("../../include")
include_directories("../")

file(GLOB_RECURSE SOURCES *.cpp)
add_library(efsw ${SOURCES})
set_target_properties(efsw PROPERTIES OUTPUT_NAME "efsw")
