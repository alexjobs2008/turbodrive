project(google-breakpad)
cmake_minimum_required(VERSION 2.8.11)
message("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(WIN32)
	add_custom_command(TARGET generate
		PRE_BUILD
		COMMAND cmd ARGS //C gyp.bat --no-circular-check src/client/windows/breakpad_client.gyp
		WORKING_DIRECTORY src/tools/gyp)

endif()


if(UNIX)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS -fPIC)
endif(UNIX)
