#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "Semaphore.h"
#include "Thread.h"
#include "Utility.h"

using namespace os;

Semaphore::Semaphore(int value)
{   
    int ret = sem_init(&sem, 0, value);
    if (ret != 0)
    {
        debug::DebugInfo::log("semaphore init error %d\n", errno);
    }
    
    /*pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&mutex, &attr);*/
}

Semaphore::~Semaphore()
{
    sem_destroy(&sem);
    /*pthread_mutexattr_destroy(&attr);
    pthread_mutex_destroy(&mutex);*/
    
}

int Semaphore::acquire()
{
    /*int ret = 0;
    ret = pthread_mutex_lock(&mutex);
    return ret;*/
    int ret = 0;
    ret = sem_wait(&sem);
    return ret;
}

int Semaphore::acquire(int ms)
{
    int ret = 0;
    time_t current = time(NULL);
    struct timespec req;
    req.tv_sec  = (long)(current + ms/1000);
    int mod     = ms%1000;
    req.tv_nsec = mod*1000*1000;
    ret = sem_timedwait(&sem, (const struct timespec *)&req);
    return ret;
}

int Semaphore::tryAcquire()
{
    /*int ret = 0;
    ret = pthread_mutex_lock(&mutex);
    return ret;*/
    int ret = 0;
    while (true)
    {
        ret = sem_trywait(&sem);
        if (ret == 0)
        {
            break;
        }
        else
        {
            os::Thread::sleep(50);
        }
    }
    return ret;
}

int Semaphore::release()
{
    /*int ret = 0;
    ret = pthread_mutex_unlock(&mutex);
    return ret;*/
    int ret = 0;
    ret = sem_post(&sem);
    return ret;
}

