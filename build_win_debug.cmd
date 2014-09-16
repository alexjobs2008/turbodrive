mkdir src-build-debug
cd src-build-debug

rem QTDIR=~/Qt/5.3/clang_64 

cmake -DCMAKE_BUILD_TYPE=Debug ../src -G"CodeBlocks - MinGW Makefiles"
mingw32-make install

%QTDIR%/bin/windeployqt --debug --libdir Application/inst Application/drive.exe
mingw32-make package

cd ..
pause
