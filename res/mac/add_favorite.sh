#!/bin/sh

PLB="/usr/libexec/PlistBuddy"
userPlist="~/Library/Preferences/com.apple.sidebarlists.plist"
theName="MyShare"
theURL="~/SOURCES"

## Add the new server entry for current logged in user
"$PLB" -c "Add :favorites:VolumesList:0 dict" "$userPlist"
"$PLB" -c "Add :favorites:VolumesList:0:Name string $theName" "$userPlist"
"$PLB" -c "Add :favorites:VolumesList:0:URL string $theURL" "$userPlist"

