#ifndef _OS_THREAD_H
#define _OS_THREAD_H

#include "pthread.h"
#include <string>

namespace os {

    class Thread
    {
    public:
        static void sleep(const int & ms);
        Thread(std::string const& name = "thread");
        virtual ~Thread();
        void Start();
        int Wait();
    protected:
        virtual void Run() = 0;
    private:  
        std::string name;
        pthread_t handle;
        pthread_attr_t attr;        
        static void* start(void * tag);
        class Implmentation;
        Implmentation * impl;
    };
}

#endif
