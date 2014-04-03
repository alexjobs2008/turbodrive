echo off

echo Environment configuring...
set PATH="C:\Windows\Microsoft.NET\Framework\v4.0.30319";"C:\Program Files (x86)\CMake 2.8\bin";G:\qt\5.2.1\5.2.1\mingw48_32\bin;G:\qt\5.2.1/Tools/mingw48_32\bin;%PATH%

echo Project generation...
call src\tools\gyp\gyp.bat --no-circular-check src\client\windows\breakpad_client.gyp

echo Project building...
cd src\client\windows
msbuild.exe handler\exception_handler.sln /p:Configuration=Release /p:Platform=Win32 
