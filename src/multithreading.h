#ifndef KO_MULTITHREADING_H
#define KO_MULTITHREADING_H

#include <SDL2/SDL.h>

#define LOCK()   SDL_LockMutex(MainLock)
#define UNLOCK() SDL_UnlockMutex(MainLock)

extern SDL_mutex *MainLock;

#endif
