#include <SDL2/SDL.h>

SDL_mutex *MainLock;

void initMultithreading() { MainLock = SDL_CreateMutex(); }

void freeMultithreading() { SDL_DestroyMutex(MainLock); }
