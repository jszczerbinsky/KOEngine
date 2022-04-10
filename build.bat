@echo off
md build
cd src
gcc -shared *.c -IE:\mingw-w64\x86_64-posix-seh\mingw64\include -LE:\mingw-w64\x86_64-posix-seh\mingw64\lib -lmingw32 -Wall -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_mixer -lws2_32  -o ../build/KOEngine.dll
pause > nul