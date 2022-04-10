mkdir -p build
rm -rf build/*
mkdir -p build/lib
mkdir -p build/include
mkdir -p build/include/KOE
mkdir -p build/include/KOE/uiControls
mkdir -p build/share
mkdir -p build/share/licenses
mkdir -p build/share/licenses/libKOEngine
cp LICENSE build/share/licenses/libKOEngine/LICENSE
cd src
cp *.h ../build/include/KOE
cp uiControls/*.h ../build/include/KOE/uiControls
gcc -shared *.c -fPIC -Wall -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_mixer -o ../build/lib/libKOEngine.so
cd ..
