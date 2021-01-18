@echo off

REM g++
set GCCCommonLinkerFlags=-luser32 -lGdi32 -lWinmm -lOpenGL32
REM Set GCCCommonCompilerFlags=-I "C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Include" -I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.10586.0\um" -I "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\include" -std=c++11 -w -DAPP_SLOW=1 -DAPP_DEV=1 -DAPP_WIN32=1
Set GCCCommonCompilerFlags=-w -DAPP_SLOW=1 -DAPP_DEV=1 -DAPP_WIN32=1

pushd buildGcc
g++ %GCCCommonCompilerFlags% ..\code\app.cpp -lOpenGL32 -o appGCC.dll
g++ %GCCCommonCompilerFlags% ..\code\win32_app.cpp -lOpenGL32 -o GCC.exe
popd
