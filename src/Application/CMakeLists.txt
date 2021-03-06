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

# For Apple set the icns file containing icons
IF(APPLE)
	# set how it shows up in the Info.plist file
	# SET(MACOSX_BUNDLE_ICON_FILE appicon_mac.icns)
	# set where in the bundle to put the icns file
	SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/../../res/icons/appicon_mac.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	# include the icns file in the target
	SET(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/../../res/icons/appicon_mac.icns)

        SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/res/folder_icons/128_ok.png PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        SET(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/res/folder_icons/128_ok.png)
        SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/res/folder_icons/128_error.png PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        SET(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/res/folder_icons/128_error.png)
        SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/res/folder_icons/128_sync.png PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        SET(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/res/folder_icons/128_sync.png)

        SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/res/folder_icons/ok.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        SET(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/res/folder_icons/ok.icns)
        SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/res/folder_icons/error.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        SET(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/res/folder_icons/error.icns)
        SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/res/folder_icons/sync.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        SET(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/res/folder_icons/sync.icns)

ENDIF(APPLE)

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
        UtilUI
	efsw
	Events
        Tutorial
)
target_link_libraries(${PROJECT_NAME} ${LINK_LIBRARIES})

#
# Install application runtime components
#

# Main executable
if(WIN32)
        install(TARGETS ${PROJECT_NAME}
                DESTINATION bin
        )
elseif(APPLE)
        install(TARGETS ${PROJECT_NAME}
                DESTINATION .
        )
endif()

# Runtime libraries
if(MINGW)
elseif(MSVC)
        install(FILES
                ${CMAKE_CURRENT_SOURCE_DIR}/../../res/inst/libs/libEGL.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../res/inst/libs/libGLESv2.dll

                DESTINATION bin
        )
endif()

if(WIN32)
        install(FILES
                # OpenSSL libs
                # http://slproweb.com/download/Win32OpenSSL_Light-1_0_1f.exe
                ${CMAKE_CURRENT_SOURCE_DIR}/../../res/inst/libs/libeay32.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../res/inst/libs/ssleay32.dll

                ${CMAKE_CURRENT_SOURCE_DIR}/../../res/inst/libs/msvcp110.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../res/inst/libs/msvcr110.dll

                DESTINATION bin
        )

        install(DIRECTORY
                ${CMAKE_INSTALL_PREFIX}/bin/

                DESTINATION bin
        )

        # Windows Explorer shell overlay icons handler
        install(FILES
                ${CMAKE_CURRENT_SOURCE_DIR}/../../src/OverlayIcons/x64/${CMAKE_BUILD_TYPE}/MTSDriveOverlayIcon.dll

                DESTINATION bin)

endif()

# License file
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/license.txt DESTINATION .)


#
# Package
#

# cpack installer ====================================================================

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

set(UNICODE true)
# TO_NATIVE_PATH
file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/res/license.txt" LICENSE_FILE)
if(WIN32)
    set(ICON_FILE "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\150x57.bmp")
    set(ICO_FILE "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\appicon.ico")
else()
    file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/res/150x57.bmp" ICON_FILE)
endif()

set(TARGET_PLATFORM "x86")

if(WIN32)
    set(SPACED_NAME "MTS Disk") # "МТС Диск")
else()
    set(SPACED_NAME "МТС Диск")
endif()

set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "bin")
include(InstallRequiredSystemLibraries)

if(WIN32)
    set(CPACK_GENERATOR NSIS)
elseif(APPLE)
    set(CPACK_GENERATOR DragNDrop)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../../res/mac/Info.plist
        ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)
    set_target_properties(drive PROPERTIES MACOSX_BUNDLE_INFO_PLIST
        ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)
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

if(APPLE)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../../res/mac/after_cpack.sh.in
        ${CMAKE_CURRENT_BINARY_DIR}/after_cpack.sh)
endif()

set(CPACK_RESOURCE_FILE_LICENSE ${LICENSE_FILE})

# this doesn't work wittout set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_CREATE_DESKTOP_LINKS "${PROJECT_NAME}")

set(CPACK_NSIS_PACKAGE_NAME "${SPACED_NAME}")
set(CPACK_NSIS_MUI_ICON "${ICO_FILE}")
set(CPACK_NSIS_MUI_UNIICON "${ICO_FILE}")
set(CPACK_PACKAGE_ICON "${ICO_FILE}")

set(CPACK_NSIS_DISPLAY_NAME "${SPACED_NAME}")
set(CPACK_NSIS_HELP_LINK "http://disk.mts.by/")
set(CPACK_NSIS_URL_INFO_ABOUT "http://disk.mts.by/")

set(CPACK_NSIS_INSTALLED_ICON_NAME "${PROJECT_NAME}.exe")

set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
    CreateShortCut '$DESKTOP\\\\${SPACED_NAME}.lnk' '$INSTDIR\\\\bin\\\\${PROJECT_NAME}.exe'
    CreateShortCut '$PROFILE\\\\Links\\\\${SPACED_NAME}.lnk' '$PROFILE\\\\${SPACED_NAME}'
    ExecWait '$INSTDIR\\\\bin\\\\${PROJECT_NAME}.exe -install'
    ExecWait 'regsvr32 /s \\\"$INSTDIR\\\\bin\\\\MTSDriveOverlayIcon.dll\\\" '
    WriteRegStr HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Explorer\\\\ShellIconOverlayIdentifiers\\\\MTSDiskOkOverlayIcon' '' '{912067E3-6243-4A8D-ADF8-57DABEA10BDC}'
    WriteRegStr HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Explorer\\\\ShellIconOverlayIdentifiers\\\\MTSDiskSyncOverlayIcon' '' '{5A00C1BA-A961-4CF3-9487-1732B4F3D138}'
    WriteRegStr HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Explorer\\\\ShellIconOverlayIdentifiers\\\\MTSDiskErrorOverlayIcon' '' '{28304A9F-005B-433D-BC48-6E97DB04968D}'

")

#    WriteRegStr HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Explorer\\\\ShellIconOverlayIdentifiers' 'OverlayIconHandlerOk' '{b50a71db-c4a7-4551-8d14-49983566afee}'
#    WriteRegStr HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Explorer\\\\ShellIconOverlayIdentifiers' 'OverlayIconHandlerSync' '{394f2bb6-af35-4c93-b5dd-b75fa44b1526}'
#    WriteRegStr HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Explorer\\\\ShellIconOverlayIdentifiers' 'OverlayIconHandlerError' '{76aa4b36-471f-4de8-8f4d-ddf1f8a77909}'


set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
    Delete '$PROFILE\\\\Links\\\\${SPACED_NAME}.lnk'
    Delete '$DESKTOP\\\\${SPACED_NAME}.lnk'
    DeleteRegValue HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Explorer\\\\ShellIconOverlayIdentifiers' 'MTSDiskOkOverlayIcon'
    DeleteRegValue HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Explorer\\\\ShellIconOverlayIdentifiers' 'MTSDiskSyncOverlayIcon'
    DeleteRegValue HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Explorer\\\\ShellIconOverlayIdentifiers' 'MTSDiskErrorOverlayIcon'
    ExecWait 'regsvr32 /u /s \\\"$INSTDIR\\\\bin\\\\MTSDriveOverlayIcon.dll\\\" '
    ExecWait '$INSTDIR\\\\bin\\\\${PROJECT_NAME}.exe -uninstall'
")

set(CPACK_NSIS_MUI_FINISHPAGE_RUN "${PROJECT_NAME}.exe")

include(CPack)
