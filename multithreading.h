#ifndef KO_MULTITHREADING_H
#define KO_MULTITHREADING_H

#include <pthread.h>

#define LOCK()    pthread_mutex_lock(&MainLock)
#define UNLOCK()  pthread_mutex_unlock(&MainLock)

extern pthread_mutex_t MainLock;

#endif
