# KOEngine
KOEngine is small, yet very powerfull multiplatform C framework for creating 2D games.

## Compiling
### Linux
- You need `gcc` installed
- Install `SDL2`, `SDL2_ttf` and `SDL2_mixer`using Your distro's package manager
- Clone the repo, compile and install:
```shell
git clone https://github.com/jszczerbinsky/KOEngine
cd KOEngine
./build.sh
cd build
sudo cp -R * /usr
```
### Windows (MinGW)
- You need `MinGW` or `mingw-w64` installed
- Download and install development libraries for `mingw` [SDL2](https://www.libsdl.org/download-2.0.php), 
[SDL2_ttf](https://github.com/libsdl-org/SDL_ttf) and [SDL2_mixer](https://www.libsdl.org/projects/SDL_mixer/)
- Clone the repo:
```batch
git clone https://github.com/jszczerbinsky/KOEngine
cd KOEngine
explorer .
```
- Run `build.bat`
- Go into `build`
- Copy directories into Your compiler's instance path (the same path where You installed previous development libraries)
