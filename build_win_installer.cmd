mkdir src-build
cd src-build

rem QTDIR=~/Qt/5.3/clang_64 

rem cmake -DCMAKE_BUILD_TYPE=Release ../src -G"CodeBlocks - MinGW Makefiles"
rem mingw32-make  install

%QTDIR%/bin/windeployqt --release --libdir Application/inst Application/drive.exe
mingw32-make package

cd ..
