#!/bin/bash

mkdir src-build-debug
cd src-build-debug

# QTDIR=~/Qt/5.3/clang_64 

cmake -DCMAKE_BUILD_TYPE=Debug ../src -G "CodeBlocks - Unix Makefiles"
make install

$QTDIR/bin/macdeployqt ./Application/drive.app -use-debug-libs
make package

