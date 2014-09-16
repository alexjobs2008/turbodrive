#!/bin/bash

mkdir src-build-debug
cd src-build-debug

# QTDIR=~/Qt/5.3/clang_64

cmake -DCMAKE_BUILD_TYPE=Debug ../src -G "CodeBlocks - Unix Makefiles"
make clean install

$QTDIR/bin/macdeployqt -use-debug-libs ./Application/drive.app
make package

