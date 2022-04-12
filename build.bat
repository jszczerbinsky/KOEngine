@echo off
rmdir /S /Q build
md build
md build\bin
md build\lib
md build\include
md build\include\KOE
md build\include\KOE\uiControls
copy LICENSE build\bin\LICENSE.KOEngine
cd src
copy *.h ..\build\include\KOE\
copy uiControls\*.h ..\build\include\KOE\uiControls\
gcc -shared *.c -fPIC -Wall -lmingw32 -lSDL2 -lSDL2_ttf -lSDL2_mixer -lws2_32 -Wl,--out-implib,..\build\lib\libKOEngine.dll.a -o ..\build\bin\KOEngine.dll
pause > nul