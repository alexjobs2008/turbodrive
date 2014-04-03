project(google-breakpad)
cmake_minimum_required(VERSION 2.8.11)
message("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_custom_target(${PROJECT_NAME} ALL)

if(WIN32)
	macro(win_path PATHNAME)
		# An extra \\ escape is necessary to get a \ through CMake's processing.
		string(REPLACE "/" "\\" ${PATHNAME} "${${PATHNAME}}")
		# Enclose with UNESCAPED quotes.  This means we need to escape our
		# quotes once here, i.e. with \"
		set(${PATHNAME} \"${${PATHNAME}}\")
	endmacro(win_path)

	set(GYP ${CMAKE_CURRENT_SOURCE_DIR}/tools/gyp/gyp.bat)
	set(WINDOWS_CLIENT ${CMAKE_CURRENT_SOURCE_DIR}/client/windows/breakpad_client.gyp)

	win_path(GYP)
	win_path(WINDOWS_CLIENT)

	add_custom_command(TARGET ${PROJECT_NAME}
		PRE_BUILD
		COMMAND cmd /C
		ARGS ${GYP} --no-circular-check ${WINDOWS_CLIENT})

	set(MSBUILD C:/Windows/Microsoft.NET/Framework/v4.0.30319/msbuild.exe)
	set(SOLUTION ${CMAKE_CURRENT_SOURCE_DIR}/client/windows/handler/exception_handler.sln)

	win_path(MSBUILD)
	win_path(SOLUTION)

	add_custom_command(TARGET ${PROJECT_NAME}
		PRE_BUILD
		COMMAND cmd /C
		ARGS ${MSBUILD} ${SOLUTION} /p:Configuration=Release /p:Platform=Win32)
endif()

if(UNIX)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS -fPIC)
endif(UNIX)
