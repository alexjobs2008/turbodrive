#!/bin/bash

mkdir src-build
cd src-build

# QTDIR=~/Qt/5.3/clang_64 
echo $QTDIR

cmake -DCMAKE_BUILD_TYPE=Release ../src -G "CodeBlocks - Unix Makefiles"
make clean install

$QTDIR/bin/macdeployqt ./Application/drive.app
make package

