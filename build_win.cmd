@echo off

SET DEBUG=0
SET REBUILD=0

ECHO.
ECHO.

IF "%DEBUG%"=="1" (
   SET CMAKE_BUILD_TYPE_STR=Debug
   SET BUILD_TYPE_STR=debug
   ECHO  ==== Building debug
) ELSE (
   SET CMAKE_BUILD_TYPE_STR=Release
   SET BUILD_TYPE_STR=release
   ECHO  ==== Building release
)

IF "%REBUILD%"=="1" (
	SET REBUILD_FLAG=clean
	ECHO  ==== full rebuild
) ELSE (
	SET REBUILD_FLAG= 
	ECHO  ==== build changes only
)

ECHO.
ECHO.

SET BUILD_PATH=src-build-%BUILD_TYPE_STR%
mkdir %BUILD_PATH%
cd %BUILD_PATH%

ECHO.
ECHO  ==== Compiling sources
ECHO.

cmake -Wno-dev -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE_STR% -DCMAKE_INSTALL_PREFIX=.\build ../src -G"CodeBlocks - MinGW Makefiles"
REM mingw32-make %REBUILD_FLAG% install

ECHO.
ECHO  ==== Deploying libraries
ECHO.

REM %QTDIR%/bin/windeployqt --%BUILD_TYPE_STR% --dir build/bin --libdir build/bin --qmldir %QTDIR%/qml --compiler-runtime Application/drive.exe

ECHO.
ECHO  ==== Creating installer
ECHO.

mingw32-make package

cd ..
