#!/bin/bash

mkdir src-build-release
cd src-build-release

# QTDIR=~/Qt/5.3/clang_64

cmake -DCMAKE_BUILD_TYPE=Release ../src -G "CodeBlocks - Unix Makefiles"
make clean install

$QTDIR/bin/macdeployqt ./Application/drive.app
make package

