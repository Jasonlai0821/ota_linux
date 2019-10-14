
#ifndef _RING_BUFFER_H
#define _RING_BUFFER_H

#include <string>
#include "Mutex.h"

namespace util {
    class RingBlock
    {
    public:
        int read(int rb, char * out, int size);
        int write(int wb, char * out, int size);        
        RingBlock(int blockSize);
        ~RingBlock();
    private:    
        int blockSize;
        char * block;
    };


    class RingBuffer
    {
    public:             
        static const int CacheSmall = -3;
        static const int CacheEmpty = -2;        
        static const int CacheFull  = -1;
        static const int CacheEnd   =  0;
    public:             
        int read(char * out, int size);
        int write(char * out, int size);
        int skip(int length);

        void reset();

        void end();
        bool low();

        RingBuffer(int blockSize, int ringNumber, int ringBufferWaitingSize);
        ~RingBuffer();

    private:        
        int r;
        int w;
        int bufferSize;
        int threshold;
        int floor;
        bool final;
        int blockSize;
        int ringNumber;
        int ringBufferWaitingSize;
        RingBlock** blocks; 
        os::Mutex mutex;        
    };
}


#endif

