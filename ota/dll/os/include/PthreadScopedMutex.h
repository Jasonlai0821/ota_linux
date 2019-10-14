
#ifndef _OS_PTHREAD_SCOPED_MUTEX_H_
#define _OS_PTHREAD_SCOPED_MUTEX_H_

#include <pthread.h>


static pthread_mutex_t s_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

namespace os {
    class PthreadScopedMutex
    {
    public:
        PthreadScopedMutex()  {pthread_mutex_lock(&s_mutex);}
        ~PthreadScopedMutex() {pthread_mutex_unlock(&s_mutex);}
    };
}
#endif 
