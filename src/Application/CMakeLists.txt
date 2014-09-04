project(drive)
cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

file(GLOB_RECURSE HEADERS *.h)
file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE QRC_RESOURCES *.qrc)

if(WIN32)
	file(GLOB_RECURSE RC_RESOURCES *.rc)
endif()

source_group(_h FILES ${HEADERS})
source_group(_cpp FILES ${SOURCES})
source_group(_res FILES ${QRC_RESOURCES} ${RC_RESOURCES})

if(WIN32)
	set(GUI_TYPE WIN32)
elseif(APPLE)
	set(GUI_TYPE MACOSX_BUNDLE)

	find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
	find_library(CORESERVICES_FRAMEWORK CoreServices)
	find_library(SSL_LIBRARY ssl)

	set(LINK_LIBRARIES ${LINK_LIBRARIES}
		${COREFOUNDATION_FRAMEWORK}
		${CORESERVICES_FRAMEWORK}
		${SSL_LIBRARY}
	)
endif()

find_package(Qt5Widgets)
qt5_add_resources(MOCED_RESOURCES ${QRC_RESOURCES})

add_executable(${PROJECT_NAME}
	${GUI_TYPE}
	${HEADERS}
	${SOURCES}
	${MOCED_RESOURCES}
	${RC_RESOURCES}
)

qt5_use_modules(${PROJECT_NAME}
	Widgets
)

set(LINK_LIBRARIES ${LINK_LIBRARIES}
	SingleApp
	AnimatedSystemTrayIcon
	Settings
	SettingsUI
	Network
	APIClient
	LoginUI
	QsLog
	rtCSS
	Util
	efsw
	Events
)
target_link_libraries(${PROJECT_NAME} ${LINK_LIBRARIES})

if(WIN32)
	install(TARGETS ${PROJECT_NAME}
		DESTINATION bin
	)
elseif(APPLE)
	install(TARGETS ${PROJECT_NAME}
		DESTINATION .
	)
endif()

if(MINGW)
	install(FILES
		${CMAKE_CURRENT_BINARY_DIR}/inst/libgcc_s_dw2-1.dll
		${CMAKE_CURRENT_BINARY_DIR}/inst/libstdc++-6.dll
		${CMAKE_CURRENT_BINARY_DIR}/inst/libwinpthread-1.dll

		DESTINATION bin
	)
elseif(MSVC)
	install(FILES
		${CMAKE_CURRENT_SOURCE_DIR}/../../res/inst/inst/libEGL.dll
		${CMAKE_CURRENT_SOURCE_DIR}/../../res/inst/inst/libGLESv2.dll

		DESTINATION bin
	)

endif()

if(WIN32)
	install(FILES
		# OpenSSL libs
		# http://slproweb.com/download/Win32OpenSSL_Light-1_0_1f.exe
		${CMAKE_CURRENT_SOURCE_DIR}/../../res/inst/inst/libeay32.dll
		${CMAKE_CURRENT_SOURCE_DIR}/../../res/inst/inst/ssleay32.dll

		# Qt libs
		${CMAKE_CURRENT_BINARY_DIR}/inst/Qt5Core.dll
		${CMAKE_CURRENT_BINARY_DIR}/inst/Qt5Gui.dll
		${CMAKE_CURRENT_BINARY_DIR}/inst/Qt5Network.dll
		${CMAKE_CURRENT_BINARY_DIR}/inst/Qt5Widgets.dll

		${CMAKE_CURRENT_SOURCE_DIR}/../../res/inst/inst/icudt51.dll
		${CMAKE_CURRENT_SOURCE_DIR}/../../res/inst/inst/icuin51.dll
		${CMAKE_CURRENT_SOURCE_DIR}/../../res/inst/inst/icuuc51.dll

		DESTINATION bin
	)

	install(FILES
		${CMAKE_CURRENT_SOURCE_DIR}/../../res/inst/inst/platforms/qwindows.dll
		DESTINATION bin/platforms
	)
endif()

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/license.txt DESTINATION .)

#cpack installer ====================================================================

macro(today RESULT)
	if(WIN32)
		execute_process(COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE ${RESULT})
		string(REGEX REPLACE "(..)\\.(..)\\.(....).*" "\\3\\2\\1" ${RESULT} ${${RESULT}})
	elseif(UNIX)
		execute_process(COMMAND "date" "+%d/%m/%Y" OUTPUT_VARIABLE ${RESULT})
		string(REGEX REPLACE "(..)/(..)/..(..).*" "\\3\\2\\1" ${RESULT} ${${RESULT}})
	else()
		message(SEND_ERROR "date not implemented")
		set(${RESULT} 000000)
	endif()
endmacro(today)
today(CURRENT_DATE)

# TO_NATIVE_PATH
file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/res/license.txt" LICENSE_FILE)
if(WIN32)
	set(ICON_FILE "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\150x57.bmp")
else()
	file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/res/150x57.bmp" ICON_FILE)
endif()

set(TARGET_PLATFORM "x86")

if(WIN32)
	set(SPACED_NAME "MTS Disk")
else()
	set(SPACED_NAME "МТС Диск")
endif()

set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "bin")
include(InstallRequiredSystemLibraries)

if(WIN32)
	set(CPACK_GENERATOR NSIS)
elseif(APPLE)
	set(CPACK_GENERATOR DragNDrop)
endif()

set(CPACK_PACKAGE_NAME "${SPACED_NAME}")
set(CPACK_PACKAGE_FILE_NAME "drive_${TARGET_PLATFORM}_${BUILD_SYSTEM}_${CURRENT_DATE}")
STRING(REGEX REPLACE "[ \n]" "" CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_FILE_NAME})
set(CPACK_PACKAGE_VENDOR "МТС")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${SPACED_NAME}")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")

set(CPACK_PACKAGE_INSTALL_DIRECTORY "${SPACED_NAME}")
set(CPACK_PACKAGE_EXECUTABLES ${PROJECT_NAME} "${SPACED_NAME}")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${PROJECT_NAME}")
set(CPACK_PACKAGE_ICON ${ICON_FILE})

if(WIN32)
	set(CPACK_RESOURCE_FILE_LICENSE ${LICENSE_FILE})
endif()

# this doesn't work wittout set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_CREATE_DESKTOP_LINKS "${PROJECT_NAME}")

set(CPACK_NSIS_PACKAGE_NAME "${SPACED_NAME}")

set(CPACK_NSIS_DISPLAY_NAME "${SPACED_NAME}")
set(CPACK_NSIS_HELP_LINK "http://disk.mts.by/")
set(CPACK_NSIS_URL_INFO_ABOUT "http://disk.mts.by/")

set(CPACK_NSIS_INSTALLED_ICON_NAME "${PROJECT_NAME}.exe")

set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS " CreateShortCut '$DESKTOP\\\\${SPACED_NAME}.lnk' '$INSTDIR\\\\bin\\\\${PROJECT_NAME}.exe'  ")

set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS " Delete '$DESKTOP\\\\${SPACED_NAME}.lnk' ")

set(CPACK_NSIS_MUI_FINISHPAGE_RUN "${PROJECT_NAME}.exe")

include(CPack)
