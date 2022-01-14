#include <pthread.h>

pthread_mutex_t mainLock;

void initMultithreading()
{
	pthread_mutex_init(&mainLock, NULL);
}

void freeMultithreading()
{
	pthread_mutex_destroy(&mainLock);
}
