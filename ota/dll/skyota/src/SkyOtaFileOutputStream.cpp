#include <string>
#include <stdio.h>


#include "SkyOtaFileOutputStream.h"
using namespace skyota::io;

class SkyOtaFileOutputStream::Implementation{
public:
    Implementation(const std::string & filename);
    ~Implementation();
    int write(char *buffer, int len);
    int close();
private:
    FILE * fp;
};

SkyOtaFileOutputStream::Implementation::Implementation(const std::string & filename)
{
    fp = fopen(filename.c_str(), "wb");
}

SkyOtaFileOutputStream::Implementation::~Implementation()
{
    if(fp != NULL)
    {
        fclose(fp);
    }
}

int SkyOtaFileOutputStream::Implementation::write(char * buf, int len)
{
    int ret = 0;
    if(fp != NULL)
    {
        ret = fwrite(buf, 1, len, fp);
    }
    return ret;
}

int SkyOtaFileOutputStream::Implementation::close()
{
    int ret = 0;
    if(fp != NULL)
    {
        ret = fclose(fp);
        fp = NULL;
    }
    return ret;
}

SkyOtaFileOutputStream::~SkyOtaFileOutputStream()
{
    if(impl != NULL)
    {
        delete impl;
    }    
}

SkyOtaFileOutputStream::SkyOtaFileOutputStream(const std::string & filename)
{
    impl = new SkyOtaFileOutputStream::Implementation(filename);
}

int SkyOtaFileOutputStream::write(char * buffer, int len)
{
    int ret = 0;
    if(impl != NULL)
    {
        ret = this->impl->write(buffer, len);
    }    
    return ret;
}

int SkyOtaFileOutputStream::close()
{
    int ret = 0;
    if(impl != NULL)
    {
        ret = this->impl->close();
    }    
    return ret;
}




