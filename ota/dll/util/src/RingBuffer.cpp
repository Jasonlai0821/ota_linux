#include "Utility.h"
#include "RingBuffer.h"
#include "ScopedMutex.h"
#include "Thread.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace util;
using namespace os;

#define RING_BUFFER_INFO 0

RingBlock::RingBlock(int blockSize)
    :blockSize(blockSize)
{
    this->block = new char[this->blockSize];
}

RingBlock::~RingBlock()
{
    delete[] this->block;
}

int RingBlock::read(int rb, char * out, int size)
{  
    int ret = size;
    if (size > (blockSize - rb))
    {
        ret = (blockSize - rb);
    }
    memcpy(out, block + rb, ret);  
    return ret;
}

int RingBlock::write(int wb, char * out, int size)
{    
    int ret = size;
    if (size > (blockSize - wb))
    {
        ret = (blockSize - wb);
    }    
    memcpy(block + wb, out, ret);  
    return ret;
}


RingBuffer::RingBuffer(int blockSize, int ringNumber, int ringBufferWaitingSize)
    :blockSize(blockSize), ringNumber(ringNumber), ringBufferWaitingSize(ringBufferWaitingSize)
{
    this->final = false;
    this->r = 0;
    this->w = 0;
    this->threshold = (ringNumber - 1) * blockSize;
    this->floor     = (ringNumber/2) * blockSize; 
    this->bufferSize = ringNumber*blockSize;
    this->blocks = new RingBlock*[ringNumber];

    for (int i=0; i < ringNumber; i++)
    {
        this->blocks[i] = new RingBlock(blockSize);        
    }   
}

RingBuffer::~RingBuffer()
{
    this->final = false;
    this->r = 0;
    this->w = 0;
    this->threshold = 0;
    this->bufferSize = 0;
    for (int i=0; i < ringNumber; i++)
    {
        delete this->blocks[i];        
    }
    delete [] blocks;
}


int RingBuffer::read(char * out, int len)
{
    ScopedMutex lock(this->mutex); 

#if RING_BUFFER_INFO
    debug::DebugInfo::log("BBBB Buffer size : %d - %d\n", (w -r), len);
#endif

    if (w - r == 0)
    {
        if (final)
        {
            return CacheEnd;
        }        
        return CacheEmpty;
    }

    int size = len;

    if ((size > (w-r)) && final)
    {
        size = (w-r);
    } 

    if (r + size <= w)
    {
        int remain = size; 
        int index = 0;
        int ret  = 0;
        int ri = (r % bufferSize) / blockSize;
        int rb = (r % blockSize);
        while (true)
        {
            for (; ri < ringNumber; ri++)
            {
                ret = blocks[ri]->read(rb, out + index, remain);
                index += ret;
                remain -= ret;
                rb = 0;
                if (remain == 0)
                {
                    break;                    
                }
            }
            if (remain == 0)
            {
                break;                    
            }
            ri = 0;
            rb = 0;

            for (; ri < ringNumber; ri++)
            {
                ret = blocks[ri]->read(rb, out + index, remain);
                index += ret;
                remain -= ret;
                if (remain == 0)
                {
                    break;
                }
            }  
            if (remain == 0)
            {
                break;
            }
        } 
        r += size;
        return size;
    }
    else
    {
        return CacheEmpty;
    }  

}

int RingBuffer::write(char * out, int size)
{
    ScopedMutex lock(this->mutex);

    if (size > threshold)
    {
        return CacheSmall;
    }    

    if ((w - r) >= threshold)
    {
        return CacheFull;
    }

    if (size > (bufferSize - (w -r)))
    {
        return CacheFull;
    }  

    int remain = size;
    int index = 0;
    int ret = 0;
    int wi = (w % bufferSize) / blockSize;
    int wb = w%blockSize;
    while (true)
    {
        for (; wi < ringNumber; wi++)
        {
            ret = blocks[wi]->write(wb, out + index, remain);
            index += ret;
            remain -= ret;
            wb = 0;
            if (remain == 0)
            {
                break;
            }
        }

        if (remain == 0)
        {
            break;
        }

        wi = 0;
        wb = 0;

        for (; wi < ringNumber; wi++)
        {
            ret = blocks[wi]->write(wb, out + index, remain);
            index += ret;
            remain -= ret;
            if (remain == 0)
            {
                break;
            }
        }
        if (remain == 0)
        {
            break;
        }
    } 

    w += size;

    /* check w  overflow */
    if (w < 0)
    {
        unsigned int change = (unsigned int)w;
        int nw = change % bufferSize;
        r = r % bufferSize;
        w = nw;        

        if (w < r)
        {
            w += bufferSize;
        }
        else
        {
            r += bufferSize;
            w += bufferSize;
        }
    }  
    
    return size;
}

int RingBuffer::skip(int length)
{
    {
        while(!final)
        {
            {
                ScopedMutex lock(this->mutex);
                if(length < ringBufferWaitingSize)
                {
                    if ((w - r) >= length)
                    {
                        break;
                    }                    
                }
                else
                {
                    break;
                }
            }                       
            {
                os::Thread::sleep(50);
            }
        }        
    }
    {
        ScopedMutex lock(this->mutex);
        
        if (length >= 0)
        {
            if ((w - r) < length)
            {
                return CacheEmpty;
            }
            else
            {
                r += length;
                return length;
            }
        }
        else
        {
            if ((bufferSize - (w - r)) + length < 0)
            {
                return CacheEmpty;
            }
            else
            {
                if ((w >= bufferSize) || ((r+length)>=0))
                {
                    r += length;
                    return length;
                }
            }
        }        
    }
    return CacheEnd;
}


void RingBuffer::end()
{
    ScopedMutex lock(this->mutex);
    final = true;
}

bool RingBuffer::low()
{
    bool low = false;
    ScopedMutex lock(this->mutex);
    if((w-r)< floor) 
    {
        low = true;
    }
    return low;
}

void util::RingBuffer::reset()
{
    this->final = false;
    this->r = 0;
    this->w = 0;
}
