
#include "Mutex.h"
#include "pthread.h"


os::Mutex::Mutex()
{
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex, &attr);
}

os::Mutex::~Mutex()
{
    pthread_mutexattr_destroy(&attr);
    pthread_mutex_destroy(&mutex);
}

int os::Mutex::lock()
{
    int ret = 0;
    ret = pthread_mutex_lock(&mutex);
    return ret;
}

int os::Mutex::unlock()
{
    int ret = 0;
    ret = pthread_mutex_unlock(&mutex);
    return ret;
}


