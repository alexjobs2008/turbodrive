#!/bin/bash

mkdir src-build
cd src-build

QTDIR=~/Qt/5.3/clang_64 cmake -DCMAKE_BUILD_TYPE=Release ../src
make install

~/Qt/5.3/clang_64/bin/macdeployqt ./Application/drive.app
make package

