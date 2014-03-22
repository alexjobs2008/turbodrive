project(drive)
cmake_minimum_required(VERSION 2.8.11)
message("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets)

set(HEADERS
    types.h
    AppController.h
    LoginController.h
    TrayIcon.h
)

set(SOURCES
    main.cpp
    AppController.cpp
    LoginController.cpp
    TrayIcon.cpp
)

set(QRC_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/res/res.qrc)

if(WIN32)
    set(RC_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/res/res.rc)
endif()

qt5_add_resources(MOCED_RESOURCES ${QRC_RESOURCES})

source_group(_h FILES ${HEADERS})
source_group(_cpp FILES ${SOURCES})
source_group(_res FILES ${QRC_RESOURCES} ${RC_RESOURCES})

if(WIN32)
    set(GUI_TYPE WIN32)
endif(WIN32)
	
if(APPLE)
    set(GUI_TYPE MACOSX_BUNDLE)
endif(APPLE)

add_executable(${PROJECT_NAME}
    ${GUI_TYPE}
    ${HEADERS}
    ${SOURCES}
    ${MOCED_RESOURCES}
    ${RC_RESOURCES}
)

qt5_use_modules(${PROJECT_NAME}
    Core
    Widgets
)

target_link_libraries(${PROJECT_NAME}
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


if(WIN32)
    install(TARGETS ${PROJECT_NAME}
        DESTINATION bin
    )

message("Qt binary dir: ${QT_BINARY_DIR}")

#    install(FILES
#        "${QT_BINARY_DIR}/QtCore${QT_VERSION_MAJOR}.dll"
#        "${QT_BINARY_DIR}/QtGui${QT_VERSION_MAJOR}.dll"
#		"${QT_BINARY_DIR}/QtNetwork${QT_VERSION_MAJOR}.dll"
#		DESTINATION bin
#    )
endif(WIN32)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/inst/icudt51.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/inst/icuin51.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/inst/icuuc51.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/inst/libEGL.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/inst/libGLESv2.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/inst/Qt5Core.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/inst/Qt5Gui.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/inst/Qt5Network.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/inst/Qt5Widgets.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/inst/libeay32.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/inst/ssleay32.dll
    DESTINATION bin
)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/inst/platforms/qwindows.dll
    DESTINATION bin/platforms
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/license.txt DESTINATION .)

#cpack installer ====================================================================

set(SPACED_NAME "Assistent Drive")

set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "bin")
include(InstallRequiredSystemLibraries)

set(CPACK_GENERATOR NSIS)

set(CPACK_PACKAGE_NAME "AssistentDrive")
set(CPACK_PACKAGE_FILE_NAME "AssistentDrive_setup")
set(CPACK_PACKAGE_VENDOR "Assistent")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Assistent Drive, cloud storage client")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")

set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
set(CPACK_PACKAGE_EXECUTABLES "AssistentDrive" "${SPACED_NAME}")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${PROJECT_NAME}")
set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\150x57.bmp")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\license.txt")

# this doesn't work wittout set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_CREATE_DESKTOP_LINKS "${PROJECT_NAME}")

set(CPACK_NSIS_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_NSIS_CONTACT "a@assistent.by")

set(CPACK_NSIS_DISPLAY_NAME "${SPACED_NAME}")
set(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.assistent.by\\\\drive")
set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.assistent.by\\\\drive")

# no need for that
#set(CPACK_NSIS_MENU_LINKS "${PROJECT_NAME}.exe" "${SPACED_NAME}")

set(CPACK_NSIS_INSTALLED_ICON_NAME "${PROJECT_NAME}.exe")

set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS " CreateShortCut '$DESKTOP\\\\${SPACED_NAME}.lnk' '$INSTDIR\\\\bin\\\\${PROJECT_NAME}.exe'  ")

set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS " Delete '$DESKTOP\\\\${SPACED_NAME}.lnk' ")

set(CPACK_NSIS_MUI_FINISHPAGE_RUN "${PROJECT_NAME}.exe")

include(CPack)