#ifndef _OS_LOCK_H
#define _OS_LOCK_H

namespace os { 
    class Lock {
    public:
        virtual ~Lock() {}

        virtual int lock() = 0;
        virtual int unlock() = 0;   
    };

};

#endif


