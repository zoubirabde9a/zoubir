@echo off

REM TODO(zoubir): make a seprate file for x64 and x86
REM for executables and the ddls
REM x86 DEBUG
REM set OpenGlLink=../lib/x86/glew32d.lib OpenGL32.lib
REM set CommonLinkerFlags=-incremental:no -opt:ref user32.lib Gdi32.lib Winmm.lib %OpenGlLink%
REM x64 DEBUG
set OpenGlLink=../lib/x64/glew32d.lib OpenGL32.lib
set CommonLinkerFlags=-incremental:no -opt:ref user32.lib Gdi32.lib Winmm.lib %OpenGlLink%
REM x64 RELEASE
REM set OpenGlLink=../lib/x64/glew32.lib OpenGL32.lib
REM set CommonLinkerFlags=-incremental:no -opt:ref user32.lib Gdi32.lib Winmm.lib %OpenGlLink%

REM Set CommonCompilerFlags=-MT -nologo -Gm- -EHsc- -EHa- -GR- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -DAPP_SLOW=1 -DAPP_DEV=1 -DAPP_WIN32=1 /FC /Z7

REM DEBUG
Set CommonCompilerFlags=-MTd -nologo -Gm- -EHsc- -EHa- -GR- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -DAPP_SLOW=1 -DAPP_DEV=1 -DAPP_WIN32=1 /FC /Z7

REM RELEASE
REM set CommonCompilerFlags=-MT -nologo -Gm- -EHsc- -EHa- -GR- -Ox -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -DAPP_SLOW=0 -DAPP_DEV=0 -DAPP_WIN32=1 /FC /Z7

REM g++
REM set GCCCommonLinkerFlags=-luser32 -lGdi32 -lWinmm -lglew32d -lOpenGL32
REM Set GCCCommonCompilerFlags=-I "C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Include" -I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.10586.0\um" -I "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\include" -std=c++11 -w -DAPP_SLOW=1 -DAPP_DEV=1 -DAPP_WIN32=1

pushd build
del *.pdb > NUL 2> NUL
REM g++ %GCCCommonCompilerFlags% ..\code\app.cpp -lglew32d -lOpenGL32 -o GCC.exe
REM g++ %GCCCommonCompilerFlags% ..\code\win32_app.cpp -lglew32d -lOpenGL32 -o GCC.exe
cl %CommonCompilerFlags% ..\code\app.cpp -Fmapp.map -LD /link -incremental:no -PDB:app%random%.pdb -opt:ref -subsystem:windows,5.02 -EXPORT:AppGetSoundSamples -EXPORT:AppUpdateAndRender %OpenGlLink%
cl %CommonCompilerFlags% ..\code\win32_app.cpp -Fmwin32_app.map /link -subsystem:windows,5.02 %CommonLinkerFlags%
popd


