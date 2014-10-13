#!/bin/bash

DEBUG=0
REBUILD=1

echo 
echo 

if [ "$DEBUG" = "1" ]; then
   CMAKE_BUILD_TYPE_STR=Debug
   BUILD_TYPE_STR=debug
   echo  ==== Building debug
else
   CMAKE_BUILD_TYPE_STR=Release
   BUILD_TYPE_STR=release
   echo  ==== Building release
fi

if [ "$REBUILD" = "1" ]; then
    REBUILD_FLAG=clean
    echo  ==== full rebuild
else
    REBUILD_FLAG= 
    echo  ==== build changes only
fi

echo 
echo 

BUILD_PATH=src-build-$BUILD_TYPE_STR
echo "BUILD_PATH=" $BUILD_PATH
mkdir $BUILD_PATH
cd $BUILD_PATH

echo 
echo  ==== Compiling sources
echo 

cmake -Wno-dev -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE_STR -DCMAKE_INSTALL_PREFIX=./build ../src -G"CodeBlocks - Unix Makefiles"
make $REBUILD_FLAG install

echo 
echo  ==== Deploying libraries
echo 

$QTDIR/bin/macdeployqt ./Application/drive.app -dmg

echo 
echo  ==== Creating installer
echo 

make package
./Application/after_cpack.sh

cd ..
