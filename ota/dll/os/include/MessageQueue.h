#ifndef _OS_MESSAGE_QUEUE_H
#define _OS_MESSAGE_QUEUE_H

#include "pthread.h"
#include <string>
#include <queue>

#include "Mutex.h"
#include "ScopedMutex.h"
#include "Semaphore.h"

namespace os {

    template <class T> class MessageQueue{
    public:
        MessageQueue();
        ~MessageQueue();  

        void send(const T&t);
        T& recv();
        bool empty();
    protected:
    private: 
        std::queue<T> queue;
        os::Mutex     mutex;
        os::Semaphore semaphore;
    };


    template <class T> os::MessageQueue<T>::MessageQueue():semaphore(Semaphore(0))
    {      
    }

    template <class T> os::MessageQueue<T>::~MessageQueue()
    {
    }

    template <class T> void  os::MessageQueue<T>::send(const T & t)
    {
        os::ScopedMutex scoped(mutex);
        if (queue.empty())
        {
            queue.push(t);
            semaphore.release();
        }   
        else
        {
            queue.push(t);
        }
    } 

    template <class T> T&  os::MessageQueue<T>::recv()
    {    
        while (queue.empty())
        {
            semaphore.acquire();
        }
        os::ScopedMutex scoped(mutex); 
        T & t = queue.front();
        queue.pop();
        return t;
    } 

    template <class T> bool os::MessageQueue<T>::empty()
    {
        os::ScopedMutex scoped(mutex); 
        return queue.empty();
    }
}

#endif
