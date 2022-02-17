#include <pthread.h>

pthread_mutex_t MainLock;

void initMultithreading()
{
	pthread_mutex_init(&MainLock, NULL);
}

void freeMultithreading()
{
	pthread_mutex_destroy(&MainLock);
}
