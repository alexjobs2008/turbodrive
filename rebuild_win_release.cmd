
mkdir src-build-release
cd src-build-release

rem QTDIR=~/Qt/5.3/clang_64 

cmake -DCMAKE_BUILD_TYPE=Release ../src -G"CodeBlocks - MinGW Makefiles"
mingw32-make clean install

%QTDIR%/bin/windeployqt --release --libdir Application/inst Application/drive.exe
mingw32-make package

cd ..
pause
