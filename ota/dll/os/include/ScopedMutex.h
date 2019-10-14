
#ifndef OS_SCOPED_MUTEX_H_
#define OS_SCOPED_MUTEX_H_

#include <Mutex.h>

namespace os {

    class ScopedMutex
    {
    public:
        explicit ScopedMutex(Lock& lock) : scopedMutex(lock)
        {
            scopedMutex.lock();
        }

        ~ScopedMutex()
        {
            scopedMutex.unlock();
        }
    private:
        Lock& scopedMutex;
    };
}
#endif 
