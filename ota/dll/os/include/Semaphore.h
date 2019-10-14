#ifndef _OS_SEMAPHORE_H
#define _OS_SEMAPHORE_H

/*#include "pthread.h"*/
#ifndef _USE_LINUX
#include "sema.h"
#else
#include <semaphore.h>
#endif
#include <string>

namespace os {

    class Semaphore
    {
    public:
        Semaphore(int value);
        ~Semaphore();
        int acquire();
        int acquire(int ms);
        int tryAcquire();
        int release();
    protected:
    private: 
        /*sem_t sem;*/
        sem_t sem;
        /*pthread_mutex_t mutex;  
        pthread_mutexattr_t attr;*/
    };
}

#endif
