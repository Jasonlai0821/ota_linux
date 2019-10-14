#ifndef _OS_MUTEX_H
#define _OS_MUTEX_H

#include "pthread.h"
#include "Lock.h"

namespace os {
    class Mutex : public Lock {
    public:    
        Mutex();
        ~Mutex();  

        int lock();
        int unlock(); 
    private:
        pthread_mutex_t mutex;  
        pthread_mutexattr_t attr;
    };


};
#endif
