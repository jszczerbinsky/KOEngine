#ifndef KO_MULTITHREADING_H
#define KO_MULTITHREADING_H

#include <pthread.h>

#define LOCK()    pthread_mutex_lock(&mainLock)
#define UNLOCK()  pthread_mutex_unlock(&mainLock)

extern pthread_mutex_t mainLock;

void initMultithreading();
void freeMultithreading();

#endif
