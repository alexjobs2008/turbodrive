#!/bin/bash

# set -x

echo
echo ==== Checking app signature
echo

codesign -v --verbose $PACKAGE_DIR/$PACKAGE_NAME/МТС\ Диск.app

#$CHECK_SIGN_COMMAND/Contents/Frameworks/QtCore.framework/Versions/5
#$CHECK_SIGN_COMMAND/Contents/Frameworks/QtGui.framework/Versions/5
#$CHECK_SIGN_COMMAND/Contents/Frameworks/QtNetwork.framework/Versions/5
#$CHECK_SIGN_COMMAND/Contents/Frameworks/QtPrintSupport.framework/Versions/5
#$CHECK_SIGN_COMMAND/Contents/Frameworks/QtWidgets.framework/Versions/5
#$CHECK_SIGN_COMMAND/Contents/PlugIns/accessible/libqtaccessiblewidgets.dylib
#$CHECK_SIGN_COMMAND/Contents/PlugIns/imageformats/libqdds.dylib
#$CHECK_SIGN_COMMAND/Contents/PlugIns/imageformats/libqgif.dylib
#$CHECK_SIGN_COMMAND/Contents/PlugIns/imageformats/libqicns.dylib
#$CHECK_SIGN_COMMAND/Contents/PlugIns/imageformats/libqico.dylib
#$CHECK_SIGN_COMMAND/Contents/PlugIns/imageformats/libqjp2.dylib
#$CHECK_SIGN_COMMAND/Contents/PlugIns/imageformats/libqjpeg.dylib
#$CHECK_SIGN_COMMAND/Contents/PlugIns/imageformats/libqmng.dylib
#$CHECK_SIGN_COMMAND/Contents/PlugIns/imageformats/libqtga.dylib
#$CHECK_SIGN_COMMAND/Contents/PlugIns/imageformats/libqtiff.dylib
#$CHECK_SIGN_COMMAND/Contents/PlugIns/imageformats/libqwbmp.dylib
#$CHECK_SIGN_COMMAND/Contents/PlugIns/imageformats/libqwebp.dylib
#$CHECK_SIGN_COMMAND/Contents/PlugIns/platforms/libqcocoa.dylib
#$CHECK_SIGN_COMMAND/Contents/PlugIns/printsupport/libcocoaprintersupport.dylib

echo
